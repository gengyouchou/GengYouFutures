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
    
    display_all_product_details(sdk)

    
    fetch_intraday_quote(sdk, "TX123400A5")
    fetch_intraday_quote(sdk, "TX123300A5")
    fetch_intraday_quote(sdk, "TXFA5")

    
    fetch_intraday_quote_live(sdk, "TX123400A5")
    fetch_intraday_quote_live(sdk, "TX123300A5")
    fetch_intraday_quote_live(sdk, "TXFA5")



if __name__ == "__main__":
    main()
