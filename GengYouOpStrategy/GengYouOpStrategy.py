import json
import os
import time
import datetime
from fubon_neo.sdk import FubonSDK

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
    print(f"累计内盘成交量: {total.get('tradeVolumeAtBid', 'N/A')}")
    print(f"累计外盘成交量: {total.get('tradeVolumeAtAsk', 'N/A')}")

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


def generate_ordered_symbols(base_symbol, steps=5, interval=50, direction="both"):
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
        range_values = range(-steps, 0)
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
    計算包含價平合約在內的所有 100 點差合約對的權利金差額。
    :param sdk: FubonSDK 實例
    :param base_symbol: 價平合約代碼，例如 "TX123300A5"
    """
    symbols = generate_ordered_symbols(base_symbol, direction = direction)
    premiums = {symbol: fetch_premium(sdk, symbol, session) for symbol in symbols}

    print(f"價平合約: {base_symbol}, 執行價格: {parse_strike_price(base_symbol)}")

    for i in range(len(symbols) - 2):  # 遍歷當前與下一檔（100 點差）的合約對
        current_symbol = symbols[i]
        next_symbol = symbols[i + 2]  # 價差為 100 點的合約對

        if next_symbol in premiums and current_symbol in premiums:
            premium_diff = premiums[current_symbol] - premiums[next_symbol]
            print(
                f"合約對: {current_symbol}, {next_symbol} | "
                f"權利金: {premiums[current_symbol]}, {premiums[next_symbol]} | 差值: {premium_diff}"
            )


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
    生成包含價平合約及其上下5個履約價的期權籌碼表。
    :param sdk: FubonSDK 實例
    :param base_symbol: 價平合約代碼，例如 "TX123300A5"
    """
    print(f"生成期權籌碼表，基準合約: {base_symbol}")

    # 生成價平上下5檔的合約代碼
    symbols = generate_ordered_symbols(base_symbol, steps=5)

    # 收集每個合約的數據
    data = {}
    for symbol in symbols:
        try:
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
            data[symbol] = {
                "strike_price": parse_strike_price(symbol),
                "last_price": quote.get("lastPrice", 0.0),
                "open_interest": quote.get("openInterest", 0),
                "bid_volume": quote.get("bidVolume", 0),
                "ask_volume": quote.get("askVolume", 0),
                "change_percent": quote.get("changePercent", 0.0)
            }
        except Exception as e:
            print(f"獲取合約 {symbol} 數據失敗: {e}")

    # 打印表格
    print("-" * 50)
    print(f"{'履約價':<10}{'最新價':<10}{'持倉量':<10}{'買量':<10}{'賣量':<10}{'漲跌幅(%)':<10}")
    print("-" * 50)
    for symbol, info in sorted(data.items(), key=lambda x: x[1]["strike_price"]):
        print(
            f"{info['strike_price']:<10}{info['last_price']:<10.2f}"
            f"{info['open_interest']:<10}{info['bid_volume']:<10}"
            f"{info['ask_volume']:<10}{info['change_percent']:<10.2f}"
        )
    print("-" * 50)


def main():
    """
    主函数，整合读取配置、登录和查询报价流程。
    """
    config = read_config()
    if not config:
        print("无法读取配置文件。")
        return

    # 初始化 SDK
    sdk = FubonSDK()

    # 登录
    if not login_sdk(sdk, config):
        return

    # 初始化实时行情
    def init_realtime():
        try:
            sdk.init_realtime()
            print("实时行情初始化完成。")
        except Exception as e:
            print(f"初始化实时行情失败: {e}")
            return False
        return True

    if not init_realtime():
        return

    while True:
        # 获取当前时间
        now = datetime.datetime.now()
        # 判断时段
        if 8 <= now.hour < 13 or (now.hour == 13 and now.minute <= 45):
            session = "beforehours"
        else:
            session = "afterhours"

        # 根据时段获取权利金
        TxfPrices = fetch_premium(sdk, "TXFA5", session)
        print(f"TxfPrices ({session}): {TxfPrices}")

        # 检查 TxfPrices 的有效性
        if TxfPrices is None or TxfPrices <= 0.0:
            print("TxfPrices 无效，重新初始化实时行情...")
            if not init_realtime():
                return
            # 延迟 1 秒
            time.sleep(10)
            continue

        # 计算价平合约
        nearest_strike_price = round(TxfPrices / 50) * 50
        at_the_money_contract = f"TX1{nearest_strike_price:05d}A5"
        print(f"价平合约: {at_the_money_contract}")

        # 执行计算函数
        calculate_spread_strategy(sdk, at_the_money_contract, session, "up")
        calculate_spread_strategy(sdk, at_the_money_contract.replace("A5", "M5"), session, "down")

        # 延迟 1 秒
        time.sleep(2)
        
        # 清空輸出
        os.system('cls' if os.name == 'nt' else 'clear')




if __name__ == "__main__":
    main()
