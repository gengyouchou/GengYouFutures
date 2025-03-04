import json
import os
import time
import datetime
from fubon_neo.sdk import FubonSDK
from flask import Flask, jsonify, request
import requests
from flask_cors import CORS
import threading
from waitress import serve  # 引入 waitress WSGI 服务器

# 讀取配置檔
def read_config(file_path='LogConfig.json'):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            return json.load(file)
    except Exception as e:
        print(f"Error reading configuration file '{file_path}': {e}")
        return None

# 使用 SDK 登錄
def login_sdk(sdk, config):
    try:
        account = config.get('account')
        password = config.get('password')
        cert_path = config.get('cert_path')
        cert_password = config.get('cert_password')
        accounts = sdk.login(account, password, cert_path, cert_password)
        print(f"登录成功，账号: {account}")
        return accounts
    except Exception as e:
        print(f"登录失败: {e}")
        return None

# 輔助函數：計算當前月份的第三個星期三
def get_third_wednesday(year, month):
    count = 0
    for day in range(1, 32):
        try:
            d = datetime.date(year, month, day)
        except:
            break
        if d.weekday() == 2:  # Wednesday
            count += 1
            if count == 3:
                return d
    return None

# 輔助函數：獲取月份對應代碼 (A=1, B=2, … L=12)
def get_month_code(month):
    mapping = {1:"A", 2:"B", 3:"C", 4:"D", 5:"E", 6:"F", 7:"G", 8:"H", 9:"I", 10:"J", 11:"K", 12:"L"}
    return mapping.get(month, "A")

# 原有函數：從合約代碼中提取履約價格
def parse_strike_price(symbol):
    try:
        return int(symbol[3:8])
    except ValueError:
        raise ValueError(f"無法解析合約代碼中的執行價格: {symbol}")

# 原有函數：依據基準合約生成上下檔（每檔間隔 interval 點）
def generate_ordered_symbols(base_symbol, steps=10, interval=50, direction="both"):
    base_prefix = base_symbol[:3]
    base_suffix = base_symbol[8:]
    base_price = parse_strike_price(base_symbol)
    if direction == "both":
        range_values = range(-steps, steps + 1)
    elif direction == "up":
        range_values = range(0, steps)
    elif direction == "down":
        range_values = [i + 1 for i in range(-steps, 0)]
    else:
        raise ValueError("方向參數 'direction' 必須是 'both', 'up', 或 'down'")
    return [
        f"{base_prefix}{base_price + i * interval:05d}{base_suffix}"
        for i in range_values
    ]

# 原有函數：根據 session 取權利金
def fetch_premium(sdk, symbol, session):
    try:
        if session == "beforehours":
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
        elif session == "afterhours":
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol, session=session)
        else:
            raise ValueError("無效的 session 類型，只能為 'beforehours' 或 'afterhours'")
        return quote.get("closePrice", 0.0)
    except Exception as e:
        print(f"無法獲取 {symbol} 的權利金: {e}")
        return 0.0

# 產生籌碼表，原 OptionChipsTable 函數保持基本邏輯
def OptionChipsTable(sdk, base_symbol):
    print(f"生成期權籌碼表，基準合約: {base_symbol}")
    symbols = generate_ordered_symbols(base_symbol, steps=10)
    data = []
    for symbol in symbols:
        try:
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
            total = quote.get('total', {})
            trade_volume = total.get("tradeVolume", 0)
            bid_volume = total.get("totalBidMatch", 0)
            ask_volume = total.get("totalAskMatch", 0)
            try:
                afterhours_quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol, session="afterhours")
                afterhours_total = afterhours_quote.get("total", {})
                trade_volume += afterhours_total.get("tradeVolume", 0)
                bid_volume += afterhours_total.get("totalBidMatch", 0)
                ask_volume += afterhours_total.get("totalAskMatch", 0)
            except Exception as e:
                print(f"獲取合約 {symbol} 的盤後數據失敗: {e}")
            data.append({
                "symbol": symbol,
                "strike_price": parse_strike_price(symbol),
                "tradeVolume": trade_volume,
                "bid_volume": bid_volume,
                "ask_volume": ask_volume,
                "volume_difference": ask_volume - bid_volume,
            })
        except Exception as e:
            print(f"獲取合約 {symbol} 數據失敗: {e}")
    return {
        "base_symbol": base_symbol,
        "options_data": data
    }

# Flask 應用及 CORS 設定
app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})

# 修改後的 get_option_chips_table：
@app.route("/OptionChipsTable", methods=["GET"])
def get_option_chips_table():
    global sdk

    now = datetime.datetime.now()
    # 判斷交易時段（日盤：08:00-13:45；否則盤後）
    if 8 <= now.hour < 13 or (now.hour == 13 and now.minute <= 45):
        session = "beforehours"
    else:
        session = "afterhours"

    today = datetime.date.today()
    third_wed = get_third_wednesday(today.year, today.month)
    # 判斷是否處於當月第三個星期三之前（含當天13:00前）
    if third_wed and (today < third_wed or (today == third_wed and now.hour < 13)):
        prefix = "TXO"
        # 使用當月台指期貨合約代碼
        year_str = str(today.year)[-2:]
        month_code = get_month_code(today.month)
    else:
        # 過了當月第三個星期三後，使用週選合約（TX1）
        prefix = "TX1"
        # 使用下一個月的台指期貨合約代碼
        next_month = today.month + 1
        next_year = today.year
        if next_month > 12:
            next_month = 1
            next_year += 1
        year_str = str(next_year)[-2:]
        month_code = get_month_code(next_month)
    # 構造台指期貨合約代碼，格式例如 "TXF25C"
    future_symbol = f"TXF{year_str}{month_code}"
    print(f"使用的台指期貨合約: {future_symbol}")

    # 获取 TxfPrices 使用當前或下一月的台指期貨合約代碼
    TxfPrices = fetch_premium(sdk, future_symbol, session)
    if TxfPrices is None or TxfPrices <= 0.0:
        return jsonify({"error": "无效的 TxfPrices"}), 500

    nearest_strike_price = round(TxfPrices / 50) * 50
    # 以目前自動判斷的前綴產生期權合約代碼（以賣權B5為例，若需要其他類型可自行調整）
    at_the_money_contract = f"{prefix}{nearest_strike_price:05d}B5"

    combined_data = {
        "at_the_money": OptionChipsTable(sdk, at_the_money_contract),
        "near_the_money": OptionChipsTable(sdk, at_the_money_contract.replace("B5", "N5"))
    }
    return jsonify(combined_data)

# 其他函數保持不變：例如 subscribe_trades, print_quote_live, fetch_intraday_quote_live 等...
def subscribe_trades(sdk, symbol, after_hours):
    def handle_message(message):
        if message.get("event") == "data":
            data = message.get("data", {})
            print(f"接收到成交信息: {data}")
            trades = data.get("trades", [])
            for trade in trades:
                print(f"成交价格: {trade.get('price')}, 成交单量: {trade.get('size')}, 成交买价: {trade.get('bid')}, 成交卖价: {trade.get('ask')}")
        else:
            print(f"接收到非行情消息: {message}")

    channel = "trades"
    try:
        sdk.init_realtime()
        futopt = sdk.marketdata.websocket_client.futopt
        futopt.on('message', handle_message)
        futopt.connect()
        futopt.subscribe({
            'channel': channel,
            'symbol': symbol,
            'afterHours': after_hours
        })
        print(f"已订阅 {symbol} 的成交信息 (夜盘: {after_hours})")
    except Exception as e:
        print(f"订阅 {symbol} 的成交信息失败: {e}")

def print_quote_live(quote):
    if not quote:
        print("无可用数据")
        return
    print(f"商品代号: {quote.get('symbol', 'N/A')}")
    print(f"类型: {quote.get('type', 'N/A')}")
    print(f"交易所: {quote.get('exchange', 'N/A')}")
    trades = quote.get('trades', [])
    for trade in trades:
        print(f"成交价格: {trade.get('price', 'N/A')}")
        print(f"成交单量: {trade.get('size', 'N/A')}")
        print(f"成交买价: {trade.get('bid', 'N/A')}")
        print(f"成交卖价: {trade.get('ask', 'N/A')}")
    total = quote.get('total', {})
    print(f"累计成交总量: {total.get('tradeVolume', 'N/A')}")
    print(f"累计内盘成交量: {total.get('totalBidMatch', 'N/A')}")
    print(f"累计外盘成交量: {total.get('totalAskMatch', 'N/A')}")
    print(f"时间: {quote.get('time', 'N/A')}")
    print(f"流水号: {quote.get('serial', 'N/A')}")
    print("-" * 50)

def fetch_intraday_quote_live(sdk, symbol):
    try:
        print("订阅日盘数据...")
        subscribe_trades(sdk, symbol, after_hours=False)
        print("订阅盘后交易数据...")
        subscribe_trades(sdk, symbol, after_hours=True)
    except Exception as e:
        print(f"获取报价时发生错误: {e}")

# Flask 服務初始化及 main() 保持不變
def start_http_server():
    try:
        print("请使用浏览器访问 http://0.0.0.0:8090/OptionChipsTable")
        serve(app, host="0.0.0.0", port=8090)
    except Exception as e:
        print(f"伺服器啟動失敗: {e}")

def main():
    global sdk, config
    print("请使用浏览器访问 http://192.168.50.168:8090/OptionChipsTable")
    config = read_config()
    if not config:
        print("无法读取配置文件")
        return
    sdk = FubonSDK()
    if not login_sdk(sdk, config):
        print("登录 SDK 失败")
        return
    def init_realtime():
        try:
            sdk.init_realtime()
        except Exception as e:
            return False, str(e)
        return True, None
    success, error = init_realtime()
    if not success:
        print(f"初始化实时行情失败: {error}")
        return
    start_http_server()

if __name__ == "__main__":
    server_thread = threading.Thread(target=start_http_server)
    server_thread.daemon = True
    server_thread.start()
    print("HTTP 伺服器已啟動，監聽埠 8090")
    try:
        main()
    except KeyboardInterrupt:
        print("程序已停止")
