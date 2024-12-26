import json
from fubon_neo.sdk import FubonSDK

# Function to read configuration from LogConfig.json
def read_config():
    try:
        with open('LogConfig.json', 'r', encoding='utf-8') as file:
            config = json.load(file)
            return config
    except Exception as e:
        print(f"Error reading LogConfig.json: {e}")
        return None

# 初始化 SDK
sdk = FubonSDK()

# 读取配置文件中的账号信息
config = read_config()

if config:
    # 从配置文件中获取登录信息
    account = config.get('account')
    password = config.get('password')
    cert_path = config.get('cert_path')
    cert_password = config.get('cert_password')

    # 登录 - 使用配置文件中的信息
    try:
        accounts = sdk.login(account, password, cert_path, cert_password)
        print(f"登录成功，账号: {account}")

        # 初始化实时行情
        sdk.init_realtime()

        # 获取即时报价
        symbol = "TXFA4"  # 替换为实际商品代码
        try:
            quote = sdk.marketdata.rest_client.futopt.intraday.quote(symbol=symbol)
            print(f"商品代碼: {quote['symbol']}")
            print(f"商品名稱: {quote['name']}")
            print(f"最後成交價: {quote['lastPrice']}")
            print(f"漲跌: {quote['change']}")
            print(f"漲跌幅: {quote['changePercent']}%")
            print(f"開盤價: {quote['openPrice']}")
            print(f"最高價: {quote['highPrice']}")
            print(f"最低價: {quote['lowPrice']}")
            print(f"累計成交量: {quote['total']['tradeVolume']}")
            print(f"最後成交時間: {quote['lastTrade']['time']}")
        except Exception as e:
            print(f"無法取得即时报价: {e}")
    except Exception as e:
        print(f"登录失败: {e}")
else:
    print("无法读取配置文件。")
