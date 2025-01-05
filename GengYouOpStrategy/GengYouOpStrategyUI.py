from flask import Flask, jsonify, request
from flask_cors import CORS
from GengYouOpStrategy import OptionChipsTable  # 導入您的函數
import threading

app = Flask(__name__)
CORS(app)  # 啟用 CORS 支持

# 創建伺服器路由
@app.route("/OptionChipsTable", methods=["GET"])
def get_option_chips_table():
    # 獲取查詢參數中的基準合約代碼
    base_symbol = request.args.get("base_symbol", "TX123300A5")
    # 模擬 FubonSDK 實例
    sdk = None  # 替換為您的 SDK 實例

    # 調用 OptionChipsTable 並生成數據
    data = OptionChipsTable(sdk, base_symbol)
    return jsonify(data)

def start_http_server():
    # 啟動 HTTP 伺服器，監聽 8090 埠
    app.run(host="0.0.0.0", port=8090, debug=False, use_reloader=False)

if __name__ == "__main__":
    # 使用單獨的執行緒啟動伺服器
    server_thread = threading.Thread(target=start_http_server)
    server_thread.daemon = True
    server_thread.start()

    print("HTTP 伺服器已啟動，監聽埠 8090")
    try:
        while True:
            pass  # 模擬主程式運行
    except KeyboardInterrupt:
        print("伺服器已停止")
