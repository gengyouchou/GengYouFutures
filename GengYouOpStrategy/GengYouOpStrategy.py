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

# Function to find and display near-month weekly options
def find_near_month_weekly_options(sdk):
    """
    查询并筛选近月周选择权 (W1)。
    :param sdk: FubonSDK 实例
    """
    try:
        restfutopt = sdk.marketdata.rest_client.futopt
        response = restfutopt.intraday.products(
            type='OPTION',
            exchange='TAIFEX',
            contractType='I',  # 指数类期权
            session='REGULAR',  # 一般交易时段，可根据需要调整
        )

        # 筛选结果，找到近月周选择权 (W1)
        weekly_options = [
            item for item in response['data']
            if item['expiryType'] == 'W' and item['symbol'].endswith('W1')
        ]

        if weekly_options:
            print("近月周选择权 (W1) 代号:")
            for option in weekly_options:
                print(f"代号: {option['symbol']}, 名称: {option['name']}")
            return weekly_options[0]['symbol']  # 返回第一个找到的商品代号
        else:
            print("未找到近月周选择权 (W1) 数据。")
            return None
    except Exception as e:
        print(f"查询期权数据失败: {e}")
        return None

# Function to fetch intraday quote for a specific symbol
def fetch_intraday_quote(sdk, symbol):
    """
    获取商品的即时报价。
    :param sdk: FubonSDK 实例
    :param symbol: 商品代号
    """
    try:
        quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
        print(f"商品代碼: {quote.get('symbol', 'N/A')}")
        print(f"商品名稱: {quote.get('name', 'N/A')}")
        print(f"最後成交價: {quote.get('lastPrice', 'N/A')}")
        print(f"漲跌: {quote.get('change', 'N/A')}")
        print(f"漲跌幅: {quote.get('changePercent', 'N/A')}%")
        print(f"開盤價: {quote.get('openPrice', 'N/A')}")
        print(f"最高價: {quote.get('highPrice', 'N/A')}")
        print(f"最低價: {quote.get('lowPrice', 'N/A')}")
        print(f"累計成交量: {quote.get('total', {}).get('tradeVolume', 'N/A')}")
        print(f"最後成交時間: {quote.get('lastTrade', {}).get('time', 'N/A')}")
    except Exception as e:
        print(f"無法取得即时报价: {e}")

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

    # 查询近月周选择权并获取其报价
    symbol = find_near_month_weekly_options(sdk)
    if symbol:
        fetch_intraday_quote(sdk, symbol)

if __name__ == "__main__":
    main()
