import json
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
    
# Function to display all product details
def display_all_product_details(sdk):
    """
    查询并输出所有商品的完整细节。
    :param sdk: FubonSDK 实例
    """
    try:
        restfutopt = sdk.marketdata.rest_client.futopt
        response = restfutopt.intraday.products(
            type='OPTION',  # 根据需求调整商品类型
            exchange='TAIFEX',
            contractType='I',  # 指数类期货
            session='REGULAR',  # 一般交易时段，可根据需要调整
        )

        # 输出完整的商品数据
        if 'data' in response and response['data']:
            print("查询到的商品完整细节:")
            for item in response['data']:
                print("-" * 50)
                for key, value in item.items():
                    print(f"{key}: {value}")
            print("-" * 50)
        else:
            print("未查询到任何商品数据。")
    except Exception as e:
        print(f"查询商品数据失败: {e}")

def subscribe_trades(sdk, symbol, after_hours):
    """
    订阅指定商品的成交信息。
    :param sdk: FubonSDK 实例
    :param symbol: 商品代码
    :param after_hours: 是否订阅夜盘行情
    """
    def handle_message(message):
        print("接收到成交信息:")
        print_quote_live(message.get("data", {}))

    channel = "trades"
    sdk.init_realtime()  # 建立行情连接
    futopt = sdk.marketdata.websocket_client.futopt
    futopt.on('message', handle_message)
    futopt.connect()
    futopt.subscribe({
        'channel': channel,
        'symbol': symbol,
        'afterHours': after_hours
    })

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
def parse_strike_price(symbol):
    """
    从选择权合约代码中提取执行价格。
    :param symbol: 合约代码，例如 "TX123300A5"
    :return: 执行价格（整数）
    """
    try:
        # 直接提取合约代码中表示执行价格的部分
        return int(symbol[3:8])
    except ValueError:
        raise ValueError(f"无法从合约代码中解析执行价格: {symbol}")

def generate_option_symbols(base_symbol, strike_price, steps=10, interval=50):
    """
    生成指定价平合约向多空两个方向的选择权合约代码。
    :param base_symbol: 基础合约代码，例如 "TX123300A5"
    :param strike_price: 当前执行价格
    :param steps: 遍历的档数
    :param interval: 每档价差（点数）
    :return: 多空方向的合约代码列表
    """
    base_prefix = base_symbol[:3]
    base_suffix = base_symbol[8:]

    long_symbols = [f"{base_prefix}{strike_price + i * interval:05d}{base_suffix}" for i in range(1, steps + 1)]
    short_symbols = [f"{base_prefix}{strike_price - i * interval:05d}{base_suffix}" for i in range(1, steps + 1)]

    return long_symbols, short_symbols

def fetch_premium(sdk, symbol):
    """
    获取指定选择权合约的当前权利金。
    :param sdk: FubonSDK 实例
    :param symbol: 合约代码
    :return: 权利金（浮点数）
    """
    try:
        quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
        return quote.get("closePrice", 0.0)
    except Exception as e:
        print(f"无法获取 {symbol} 的权利金: {e}")
        return 0.0

def calculate_spread_strategy(sdk, base_symbol):
    """
    从价平合约出发，计算价差为 100 点的两对合约的权利金差值。
    :param sdk: FubonSDK 实例
    :param base_symbol: 价平合约代码，例如 "TX123300A5"
    """
    strike_price = parse_strike_price(base_symbol)
    long_symbols, short_symbols = generate_option_symbols(base_symbol, strike_price)

    print(f"价平合约: {base_symbol}, 执行价格: {strike_price}")

    for i in range(len(long_symbols) - 1):
        long_symbol1, long_symbol2 = long_symbols[i], long_symbols[i + 1]

        long_premium1 = fetch_premium(sdk, long_symbol1)
        long_premium2 = fetch_premium(sdk, long_symbol2)
        
        long_spread_premium = long_premium1 - long_premium2

        print(f"指數上漲: {long_symbol1}, {long_symbol2} | 权利金: {long_premium1}, {long_premium2} | 差值: {long_spread_premium}")

    print("=====================================================================================================================")

    for i in range(len(long_symbols) - 1):
        short_symbol1, short_symbol2 = short_symbols[i], short_symbols[i + 1]

        short_premium1 = fetch_premium(sdk, short_symbol1)
        short_premium2 = fetch_premium(sdk, short_symbol2)

        short_spread_premium = short_premium2 - short_premium1

        print(f"指數下跌: {short_symbol1}, {short_symbol2} | 权利金: {short_premium1}, {short_premium2} | 差值: {short_spread_premium}")

# 示例调用
# sdk = FubonSDK()  # 假设 SDK 已经初始化
# calculate_spread_strategy(sdk, "TX123300A5")

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
    try:
        sdk.init_realtime()
        print("实时行情初始化完成。")
    except Exception as e:
        print(f"初始化实时行情失败: {e}")
        return
    
    # display_all_product_details(sdk)

    
    # fetch_intraday_quote(sdk, "TX123400A5")
    # fetch_intraday_quote(sdk, "TX123300A5")
    # fetch_intraday_quote(sdk, "TXFA5")

    
    # fetch_intraday_quote_live(sdk, "TX123400A5")
    # fetch_intraday_quote_live(sdk, "TX123300A5")
    # fetch_intraday_quote_live(sdk, "TXFA5")

    calculate_spread_strategy(sdk, "TX123300A5")



if __name__ == "__main__":
    main()
