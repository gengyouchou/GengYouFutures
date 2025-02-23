//+------------------------------------------------------------------+
//|                                           GetOrderFromServer.mq4   |
//| 此 EA 定時呼叫 DLL 的 CustomProcessParameters() 傳入必要參數，    |
//| DLL 負責構造 JSON 字串返回，同時也透過 HTTP GET 請求查詢 DLL 內新訂單  |
//| 訊號，並打印出來。                                               |
//+------------------------------------------------------------------+
#property strict

// 設定輪詢間隔（最小單位為 1 秒）
int TimerPeriod = 1;

// 導入 DLL 中的函數
#import "GengYouCfdStrategy.dll"
   void StartHttpServer();
   void StopHttpServer();
   // 原有函數保留（如有需要）：
   string ProcessPositions(string jsonInput);
   string GetNewOrder();
   // 新增：僅傳入必要參數，由 DLL 內部構造 JSON 字串返回
   string CustomProcessParameters(double margin, double closedPL, string commodityId, double lots, double floatingPL);
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   // 啟動 DLL 的 HTTP 服務器（如果尚未啟動），建議放在 EA 啟動時呼叫一次
   StartHttpServer();
   Print("GetOrderFromServer EA 啟動，開始監聽新訂單訊號...");
   
   // 啟動計時器，每秒輪詢一次
   EventSetTimer(TimerPeriod);
   return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   EventKillTimer();
   StopHttpServer();
   Print("GetOrderFromServer EA 停止。");
}

//+------------------------------------------------------------------+
//| 定時任務，每秒呼叫一次                                           |
//+------------------------------------------------------------------+
void OnTimer()
{
   // 1. 取得必要參數
   double margin = AccountFreeMargin();
   // 此處假設當日已實現損益為 0，實際上可根據歷史數據計算
   double closedPL = 0.0;
   
   // 取得第一筆開盤持倉參數（若無則傳入空值）
   string commodityId = "";
   double lots = 0.0;
   double floatingPL = 0.0;
   if(OrdersTotal() > 0)
   {
      if(OrderSelect(0, SELECT_BY_POS, MODE_TRADES))
      {
         commodityId = OrderSymbol();
         lots = OrderLots();
         floatingPL = OrderProfit() + OrderSwap() + OrderCommission();
      }
   }
   
   // 2. 呼叫 DLL 函數，僅傳入必要參數，由 DLL 构造 JSON 字串返回
   string jsonResult = CustomProcessParameters(margin, closedPL, commodityId, lots, floatingPL);
   Print("CustomProcessParameters 返回的 JSON: ", jsonResult);
   
   // 3. 透過 HTTP GET 查詢 DLL HTTP 服務中是否有新訂單訊號
   string url = "http://127.0.0.1:1688/getNewOrder";
   char result[];
   char headers[];
   string resultStr = "";
   int timeout = 5000;
   char postData[];
   int res = WebRequest("GET", url, "", timeout, postData, 0, result, headers);
   if(res == 200)
   {
      resultStr = CharArrayToString(result);
      // 當返回的 JSON 內容非空（非 "{}"）且長度大於 2 時，打印新訂單訊號
      if(resultStr != "{}" && StringLen(resultStr) > 2)
      {
         Print("取得新訂單 JSON 訊號: ", resultStr);
      }
   }
   else
   {
      Print("WebRequest Error: ", GetLastError());
   }
}
