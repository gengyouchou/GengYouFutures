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

# ---------------------------
# 配置与登录相关函数
# ---------------------------
def read_config(file_path='LogConfig.json'):
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            return json.load(file)
    except Exception as e:
        print(f"Error reading configuration file '{file_path}': {e}")
        return None

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

# ---------------------------
# 辅助函数：日期与合约编码处理
# ---------------------------
# 返回指定年份、月份的第三个星期三（通常为月选结算日）
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

# 根据当天日期判断结算周别代码（1-3位），不考虑时间，仅基于日历
def get_settlement_week_code(date_obj):
    day = date_obj.day
    if day <= 7:
        return "TX1"
    elif day <= 14:
        return "TX2"
    elif day <= 21:
        return "TXO"  # 第三周（月选）
    elif day <= 28:
        return "TX4"
    else:
        return "TX5"

# 将数字月份转换为字母代码 (A=1, B=2, …, L=12)
def get_month_code(month):
    mapping = {1:"A", 2:"B", 3:"C", 4:"D", 5:"E", 6:"F", 7:"G", 8:"H", 9:"I", 10:"J", 11:"K", 12:"L"}
    return mapping.get(month, "A")

# 从合约代码中解析履约价格（假设占第4～8位）
def parse_strike_price(symbol):
    try:
        return int(symbol[3:8])
    except ValueError:
        raise ValueError(f"无法解析合约代码中的执行价格: {symbol}")

# ---------------------------
# 合约代码生成与数据聚合
# ---------------------------
# 根据基准合约生成上下档合约代码（每档间隔 interval 点）
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
        raise ValueError("方向参数 'direction' 必须是 'both', 'up', 或 'down'")
    return [
        f"{base_prefix}{base_price + i * interval:05d}{base_suffix}"
        for i in range_values
    ]

# 获取指定合约的权利金（或台指期货价格）
def fetch_premium(sdk, symbol, session):
    try:
        if session == "beforehours":
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
        elif session == "afterhours":
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol, session=session)
        else:
            raise ValueError("无效的 session 类型，只能为 'beforehours' 或 'afterhours'")
        return quote.get("closePrice", 0.0)
    except Exception as e:
        print(f"无法获取 {symbol} 的权利金: {e}")
        return 0.0

# 生成单一合约的期权筹码表，并计算内外盘比
def OptionChipsTable(sdk, base_symbol):
    print(f"生成期权筹码表，基准合约: {base_symbol}")
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
                print(f"获取合约 {symbol} 的盘后数据失败: {e}")
            ratio = (bid_volume / ask_volume) if ask_volume != 0 else None
            data.append({
                "symbol": symbol,
                "strike_price": parse_strike_price(symbol),
                "tradeVolume": trade_volume,
                "bid_volume": bid_volume,
                "ask_volume": ask_volume,
                "volume_difference": ask_volume - bid_volume,
                "bid_ask_ratio": ratio
            })
        except Exception as e:
            print(f"获取合约 {symbol} 数据失败: {e}")
    return {
        "base_symbol": base_symbol,
        "options_data": data
    }

# 聚合多个筹码表（按履约价格累加）
def aggregate_chip_tables(tables_list):
    aggregated = {}
    for table in tables_list:
        for item in table.get("options_data", []):
            strike = item.get("strike_price")
            if strike not in aggregated:
                aggregated[strike] = {
                    "tradeVolume": 0,
                    "bid_volume": 0,
                    "ask_volume": 0,
                    "volume_difference": 0,
                    "bid_ask_ratio_sum": 0,
                    "count": 0
                }
            aggregated[strike]["tradeVolume"] += item.get("tradeVolume", 0)
            aggregated[strike]["bid_volume"] += item.get("bid_volume", 0)
            aggregated[strike]["ask_volume"] += item.get("ask_volume", 0)
            aggregated[strike]["volume_difference"] += item.get("volume_difference", 0)
            ratio = item.get("bid_ask_ratio")
            if ratio is not None:
                aggregated[strike]["bid_ask_ratio_sum"] += ratio
                aggregated[strike]["count"] += 1
    result = []
    for strike, data in aggregated.items():
        avg_ratio = data["bid_ask_ratio_sum"] / data["count"] if data["count"] > 0 else None
        result.append({
            "strike_price": strike,
            "tradeVolume": data["tradeVolume"],
            "bid_volume": data["bid_volume"],
            "ask_volume": data["ask_volume"],
            "volume_difference": data["volume_difference"],
            "avg_bid_ask_ratio": avg_ratio
        })
    result.sort(key=lambda x: x["strike_price"])
    return result

# ---------------------------
# Flask 应用与服务
# ---------------------------
app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})

# 修改后的 get_option_chips_table：
# 基于候选台股期货合约数据（如 TXFC5）确定基础价格，
# 并自动生成10码选权合约代码（包括买权和卖权），生成筹码表并聚合返回。
@app.route("/OptionChipsTable", methods=["GET"])
def get_option_chips_table():
    global sdk

    now = datetime.datetime.now()
    # 判断交易时段（日盘：08:00-13:45；否则盘后）
    if 8 <= now.hour < 13 or (now.hour == 13 and now.minute <= 45):
        session = "beforehours"
    else:
        session = "afterhours"

    # 使用候选合约中的基础合约，此处固定使用 "TXFC5"（例如：臺股期貨2025/03）
    future_contract = "TXFC5"
    print(f"使用的台股期货基础合约: {future_contract}")
    TxfPrices = fetch_premium(sdk, future_contract, session)
    if TxfPrices is None or TxfPrices <= 0.0:
        return jsonify({"error": "无效的 TxfPrices"}), 500

    nearest_strike_price = round(TxfPrices / 50) * 50

    # 判断目标结算月份：若今天在本月第三个星期三之前，则使用本月；否则使用下月
    today = datetime.date.today()
    third_wed = get_third_wednesday(today.year, today.month)
    if third_wed and (today < third_wed or (today == third_wed and now.hour < 13 and now.minute < 45)):
        target_month = today.month
        target_year = today.year
    else:
        target_month = today.month + 1
        target_year = today.year
        if target_month > 12:
            target_month = 1
            target_year += 1

    # 根据当天日期确定结算周别代码：
    # 如果今天正好是第三个星期三且未过13:45，则应使用 TXO，否则采用 get_settlement_week_code(today)
    if third_wed and today == third_wed and (now.hour < 13 or (now.hour == 13 and now.minute < 45)):
        settlement_week_code = "TXO"
    else:
        settlement_week_code = get_settlement_week_code(today)

    # 生成第9位：到期月份代码
    # 买权使用 A-L（A代表1月，…，L代表12月），卖权使用 M-X（M代表1月，…，X代表12月）
    call_month_letter = chr(ord('A') + (target_month - 1))
    put_month_letter  = chr(ord('M') + (target_month - 1))
    # 第10位：目标年份的最后一位数字
    year_last_digit = str(target_year)[-1]

    call_contract = f"{settlement_week_code}{nearest_strike_price:05d}{call_month_letter}{year_last_digit}"
    put_contract  = f"{settlement_week_code}{nearest_strike_price:05d}{put_month_letter}{year_last_digit}"
    print(f"生成买权合约代码: {call_contract}")
    print(f"生成卖权合约代码: {put_contract}")

    # 生成买权与卖权的筹码表
    call_table = OptionChipsTable(sdk, call_contract)
    put_table  = OptionChipsTable(sdk, put_contract)
    aggregated_call = aggregate_chip_tables([call_table])
    aggregated_put  = aggregate_chip_tables([put_table])
    response = {
        "base_contract_used": future_contract,
        "TxfPrices": TxfPrices,
        "nearest_strike_price": nearest_strike_price,
        "call_contract": call_contract,
        "put_contract": put_contract,
        "aggregated_call_chip_table": aggregated_call,
        "aggregated_put_chip_table": aggregated_put
    }
    return jsonify(response)

# 以下其他函数保持不变
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
