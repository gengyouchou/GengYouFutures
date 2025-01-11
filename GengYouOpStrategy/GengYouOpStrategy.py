import json
import os
import time
import datetime
from fubon_neo.sdk import FubonSDK
from flask import Flask, jsonify, request
import requests
from flask_cors import CORS
import threading

# Function to read configuration from LogConfig.json
def read_config(file_path='LogConfig.json'):
    """
    读取配置文件。
    :param file_path: 配置文件路径
    :return: 配置内容字典，若读取失败则返回 None
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            return json.load(file)
    except Exception as e:
        print(f"Error reading configuration file '{file_path}': {e}")
        return None

# Function to login using the SDK
def login_sdk(sdk, config):
    """
    使用 SDK 登录。
    :param sdk: FubonSDK 实例
    :param config: 配置字典
    :return: 登录成功返回账户信息，失败返回 None
    """
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


def fetch_intraday_quote(sdk, symbol):
    """
    获取指定商品的日盘和盘后交易的即时报价。
    :param sdk: FubonSDK 实例
    :param symbol: 商品代码
    """
    try:
        # 获取日盘数据
        day_quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
        print("日盘数据:")
        print_quote(day_quote)

        # 获取盘后交易数据
        afterhours_quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol, session="afterhours")
        print("盘后交易数据:")
        print_quote(afterhours_quote)

    except Exception as e:
        print(f"获取报价时发生错误: {e}")

def print_quote(quote):
    """
    输出报价数据的详细信息。
    :param quote: 报价数据（字典）
    """
    if not quote:
        print("无可用数据")
        return

    print(f"日期: {quote.get('date', 'N/A')}")
    print(f"类型: {quote.get('type', 'N/A')}")
    print(f"交易所: {quote.get('exchange', 'N/A')}")
    print(f"商品代号: {quote.get('symbol', 'N/A')}")
    print(f"商品名称: {quote.get('name', 'N/A')}")
    print(f"昨收: {quote.get('previousClose', 'N/A')}")
    print(f"开盘价: {quote.get('openPrice', 'N/A')}")
    print(f"开盘时间: {quote.get('openTime', 'N/A')}")
    print(f"最高价: {quote.get('highPrice', 'N/A')}")
    print(f"最高时间: {quote.get('highTime', 'N/A')}")
    print(f"最低价: {quote.get('lowPrice', 'N/A')}")
    print(f"最低时间: {quote.get('lowTime', 'N/A')}")
    print(f"收盘价: {quote.get('closePrice', 'N/A')}")
    print(f"收盘时间: {quote.get('closeTime', 'N/A')}")
    print(f"平均价: {quote.get('avgPrice', 'N/A')}")
    print(f"涨跌: {quote.get('change', 'N/A')}")
    print(f"涨跌幅: {quote.get('changePercent', 'N/A')}%")
    print(f"振幅: {quote.get('amplitude', 'N/A')}")
    print(f"最新成交价: {quote.get('lastPrice', 'N/A')}")
    print(f"最新成交量: {quote.get('lastSize', 'N/A')}")
    
    # 累计数据
    total = quote.get('total', {})
    print(f"累计成交量: {total.get('tradeVolume', 'N/A')}")
    print(f"累计内盘成交量: {total.get('totalBidMatch', 'N/A')}")
    print(f"累计外盘成交量: {total.get('totalAskMatch', 'N/A')}")

    # 最后一笔成交数据
    last_trade = quote.get('lastTrade', {})
    print(f"最后一笔成交价: {last_trade.get('price', 'N/A')}")
    print(f"最后一笔成交量: {last_trade.get('size', 'N/A')}")
    print(f"最后一笔成交时间: {last_trade.get('time', 'N/A')}")
    print(f"交易流水号: {last_trade.get('serial', 'N/A')}")
    print(f"流水号: {quote.get('serial', 'N/A')}")
    print(f"最后更新时间: {quote.get('lastUpdated', 'N/A')}")
    print("-" * 50)

def parse_strike_price(symbol):
    """
    從合約代碼提取執行價格。
    :param symbol: 合約代碼，例如 "TX123300A5"
    :return: 執行價格（整數）
    """
    try:
        return int(symbol[3:8])
    except ValueError:
        raise ValueError(f"無法解析合約代碼中的執行價格: {symbol}")


def generate_ordered_symbols(base_symbol, steps=10, interval=50, direction="both"):
    """
    按指定方向生成包含價平合約及上下多檔的合約代碼。
    :param base_symbol: 價平合約代碼，例如 "TX123300A5"
    :param steps: 生成的檔數
    :param interval: 每檔價差（點數）
    :param direction: 生成方向，"both" 为上下，"up" 为向上，"down" 为向下
    :return: 排序後的合約代碼列表
    """
    base_prefix = base_symbol[:3]
    base_suffix = base_symbol[8:]
    base_price = parse_strike_price(base_symbol)

    if direction == "both":
        range_values = range(-steps, steps + 1)
    elif direction == "up":
        range_values = range(0, steps)
    elif direction == "down":
        range_values = [i + 1 for i in range(-steps, 0)]  # 自定義調整，生成 [-2, -1, 0]
    else:
        raise ValueError("方向參數 'direction' 必須是 'both', 'up', 或 'down'")

    return [
        f"{base_prefix}{base_price + i * interval:05d}{base_suffix}"
        for i in range_values
    ]


def fetch_premium(sdk, symbol, session):
    """
    獲取指定合約的權利金。
    :param sdk: FubonSDK 實例
    :param symbol: 合約代碼
    :param session: 行情會話類型 ("beforehours" 或 "afterhours")
    :return: 權利金（浮點數）
    """
    try:
        # 模擬權利金數據查詢
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


def calculate_spread_strategy(sdk, base_symbol, session, direction):
    """
    Calculate the premium differences for all 100-point interval contract pairs, 
    including the at-the-money contract.
    :param sdk: FubonSDK instance
    :param base_symbol: At-the-money contract symbol, e.g., "TX123300A5"
    :param session: Current trading session
    :param direction: Contract generation direction ("both", "up", "down")
    """
    # Generate contract symbols
    symbols = generate_ordered_symbols(base_symbol, direction=direction)

    # Reverse symbols if the direction is "down"
    if direction == "down":
        symbols = symbols[::-1]

    # Fetch premium data
    premiums = {symbol: fetch_premium(sdk, symbol, session) for symbol in symbols}

    # Collect results in a list
    results = []

    # Process each contract pair
    for i in range(len(symbols) - 2):  # Ensure no out-of-bound index
        current_symbol = symbols[i]
        next_symbol = symbols[i + 2]  # 100-point interval pair

        # Retrieve premium data, skip pairs with missing data
        current_premium = premiums.get(current_symbol)
        next_premium = premiums.get(next_symbol)

        if current_premium is None or next_premium is None:
            results.append({
                "pair": [current_symbol, next_symbol],
                "status": "skipped",
                "reason": "Premium data missing"
            })
            continue

        # Calculate premium difference
        premium_diff = current_premium - next_premium
        results.append({
            "pair": [current_symbol, next_symbol],
            "premiums": {
                "current": current_premium,
                "next": next_premium
            },
            "difference": premium_diff,
            "status": "processed"
        })

    # Wrap in a JSON response
    response = {
        "base_symbol": base_symbol,
        "strike_price": parse_strike_price(base_symbol),
        "direction": direction,
        "results": results
    }

    # Output JSON
    print(json.dumps(response, indent=4))


# 示例調用
# sdk = FubonSDK()  # 假設已初始化
# calculate_100_point_spreads(sdk, "TX123300A5")

def subscribe_trades(sdk, symbol, after_hours):
    """
    订阅指定商品的成交信息。
    :param sdk: FubonSDK 实例
    :param symbol: 商品代码
    :param after_hours: 是否订阅夜盘行情 (True 为夜盘, False 为日盘)
    """
    def handle_message(message):
        # 检查消息是否为行情数据
        if message.get("event") == "data":
            data = message.get("data", {})
            print(f"接收到成交信息: {data}")
            trades = data.get("trades", [])
            for trade in trades:
                print(f"成交价格: {trade.get('price')}, 成交单量: {trade.get('size')}, "
                      f"成交买价: {trade.get('bid')}, 成交卖价: {trade.get('ask')}")
        else:
            print(f"接收到非行情消息: {message}")

    channel = "trades"

    try:
        # 初始化实时行情连接
        sdk.init_realtime()

        # 获取 WebSocket 客户端
        futopt = sdk.marketdata.websocket_client.futopt

        # 订阅消息处理函数
        futopt.on('message', handle_message)

        # 建立 WebSocket 连接
        futopt.connect()

        # 发送订阅请求
        futopt.subscribe({
            'channel': channel,
            'symbol': symbol,
            'afterHours': after_hours
        })

        print(f"已订阅 {symbol} 的成交信息 (夜盘: {after_hours})")

    except Exception as e:
        print(f"订阅 {symbol} 的成交信息失败: {e}")


def print_quote_live(quote):
    """
    输出报价数据的详细信息。
    :param quote: 报价数据（字典）
    """
    if not quote:
        print("无可用数据")
        return

    print(f"商品代号: {quote.get('symbol', 'N/A')}")
    print(f"类型: {quote.get('type', 'N/A')}")
    print(f"交易所: {quote.get('exchange', 'N/A')}")
    
    # 成交数据
    trades = quote.get('trades', [])
    for trade in trades:
        print(f"成交价格: {trade.get('price', 'N/A')}")
        print(f"成交单量: {trade.get('size', 'N/A')}")
        print(f"成交买价: {trade.get('bid', 'N/A')}")
        print(f"成交卖价: {trade.get('ask', 'N/A')}")

    # 累计数据
    total = quote.get('total', {})
    print(f"累计成交总量: {total.get('tradeVolume', 'N/A')}")
    print(f"累计内盘成交量: {total.get('totalBidMatch', 'N/A')}")
    print(f"累计外盘成交量: {total.get('totalAskMatch', 'N/A')}")

    print(f"时间: {quote.get('time', 'N/A')}")
    print(f"流水号: {quote.get('serial', 'N/A')}")
    print("-" * 50)

def fetch_intraday_quote_live(sdk, symbol):
    """
    获取指定商品的日盘和盘后交易的即时报价。
    :param sdk: FubonSDK 实例
    :param symbol: 商品代码
    """
    try:
        # 订阅日盘数据
        print("订阅日盘数据...")
        subscribe_trades(sdk, symbol, after_hours=False)

        # 订阅盘后数据
        print("订阅盘后交易数据...")
        subscribe_trades(sdk, symbol, after_hours=True)

    except Exception as e:
        print(f"获取报价时发生错误: {e}")

def OptionChipsTable(sdk, base_symbol):
    """
    生成包含價平合約及其上下10個履約價的期權籌碼表，並計算內外盤成交量差值。
    :param sdk: FubonSDK 實例
    :param base_symbol: 價平合約代碼，例如 "TX123300A5"
    :return: JSON 格式的期權籌碼表數據
    """
    print(f"生成期權籌碼表，基準合約: {base_symbol}")

    # 生成價平上下10檔的合約代碼
    symbols = generate_ordered_symbols(base_symbol, steps=10)

    # 收集每個合約的數據
    data = []
    for symbol in symbols:
        try:
            # 獲取白天數據
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
            total = quote.get('total', {})

            # 初始化內外盤數據
            trade_volume = total.get("tradeVolume", 0)
            bid_volume = total.get("totalBidMatch", 0)
            ask_volume = total.get("totalAskMatch", 0)

            # 嘗試添加盤後數據
            try:
                afterhours_quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol, session="afterhours")
                afterhours_total = afterhours_quote.get('total', {})
                trade_volume += afterhours_total.get("tradeVolume", 0)
                bid_volume += afterhours_total.get("totalBidMatch", 0)
                ask_volume += afterhours_total.get("totalAskMatch", 0)
            except Exception as e:
                print(f"獲取合約 {symbol} 的盤後數據失敗: {e}")

            # 收集最終數據
            data.append({
                "symbol": symbol,           # 添加對應的 symbol
                "strike_price": parse_strike_price(symbol),
                "tradeVolume": trade_volume,
                "bid_volume": bid_volume,
                "ask_volume": ask_volume,
                "volume_difference": ask_volume - bid_volume,
            })
        except Exception as e:
            print(f"獲取合約 {symbol} 數據失敗: {e}")

    # 將 base_symbol 放在外層，並組裝 JSON 格式的數據
    json_data = json.dumps({
        "base_symbol": base_symbol,
        "options_data": data
    }, ensure_ascii=False, indent=4)

    print("生成的期權籌碼表數據（JSON 格式）:")
    print(json_data)

    # 返回 JSON 數據
    return json_data


# 初始化 Flask 应用并启用 CORS
app = Flask(__name__)
CORS(app, resources={r"/OptionChipsTable": {"origins": "*"}})  # 允许所有来源跨域访问特定路由

@app.route("/OptionChipsTable", methods=["POST"])
def receive_option_chips_table():
    """
    接收客户端POST请求的数据。
    """
    data = request.get_json()
    print(f"接收到的数据: {data}")
    return jsonify({"status": "success", "message": "数据已接收"})

def start_http_server():
    """
    启动HTTP服务器，监听8090端口。
    """
    app.run(host="0.0.0.0", port=8090, debug=False, use_reloader=False)

def main():
    """
    主函数，整合读取配置、登录和查询报价流程。
    """
    # 模拟读取配置文件
    config = {"username": "test", "password": "test"}
    if not config:
        print("无法读取配置文件。")
        return

    # 模拟登录 SDK 和实时行情初始化
    def login_sdk():
        print("登录成功，账号: F129305651")
        return True

    def init_realtime():
        try:
            print("实时行情初始化完成。")
        except Exception as e:
            print(f"初始化实时行情失败: {e}")
            return False
        return True

    if not login_sdk() or not init_realtime():
        return

    while True:
        now = datetime.datetime.now()
        session = "beforehours" if 8 <= now.hour < 13 or (now.hour == 13 and now.minute <= 45) else "afterhours"
        TxfPrices = 22928  # 模拟获取权利金数据
        print(f"TxfPrices ({session}): {TxfPrices}")

        if TxfPrices is None or TxfPrices <= 0.0:
            print("TxfPrices 无效，重新初始化实时行情...")
            if not init_realtime():
                return
            time.sleep(10)
            continue

        nearest_strike_price = round(TxfPrices / 50) * 50
        at_the_money_contract = f"TX2{nearest_strike_price:05d}A5"
        print(f"价平合约: {at_the_money_contract}")

        combined_data = {
            "at_the_money": {"contract": at_the_money_contract, "data": "some_data"},
            "near_the_money": {"contract": at_the_money_contract.replace("A5", "M5"), "data": "some_other_data"}
        }

        try:
            response = requests.post(
                "http://localhost:8090/OptionChipsTable",
                json=combined_data,
                headers={"Content-Type": "application/json"}
            )
            print(f"数据发送结果: {response.status_code}, {response.text}")
        except Exception as e:
            print(f"发送数据失败: {e}")

        time.sleep(5)
        os.system('cls' if os.name == 'nt' else 'clear')

if __name__ == "__main__":
    server_thread = threading.Thread(target=start_http_server)
    server_thread.daemon = True
    server_thread.start()

    print("HTTP服务器已启动，监听端口8090")
    try:
        main()
    except KeyboardInterrupt:
        print("程序已停止")