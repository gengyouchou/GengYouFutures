from fubon_neo.sdk import FubonSDK

# 初始化 SDK
sdk = FubonSDK()

# 登录 - 替换为您的账号、密码和证书信息
accounts = sdk.login("YourID", "YourPassword", "YourCertPath", "YourCertPassword")

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
