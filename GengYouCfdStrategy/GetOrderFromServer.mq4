//+------------------------------------------------------------------+
//|                                           GetOrderFromServer.mq4   |
//| 此 EA 定時呼叫 DLL 的 ProcessPositions() 傳入最新未平倉狀態，    |
//| 並同時透過 HTTP GET 請求查詢 DLL 內的新訂單訊號，然後打印出來。     |
//+------------------------------------------------------------------+
#property strict

// 設定輪詢間隔（最小單位為 1 秒）
int TimerPeriod = 1;

// 導入 DLL 中的函數
#import "GengYouCfdStrategy.dll"
   void StartHttpServer();
   void StopHttpServer();
   // ProcessPositions 接收持倉 JSON，並回傳下單訊號 JSON
   string ProcessPositions(string jsonInput);
   // 也可直接呼叫此函數取得最新訂單訊號
   string GetNewOrder();
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
   // 1. 整理並傳送最新未平倉狀態給 DLL 處理
   double margin = AccountFreeMargin();
   // 這裡假設當日已實現損益為 0，實際上可依歷史數據計算
   double closedPL = 0.0;
   string jsonStr = "{";
   jsonStr += "\"Margin\":" + DoubleToString(margin,2) + ",";
   jsonStr += "\"ClosedProfitLoss\":" + DoubleToString(closedPL,2) + ",";
   jsonStr += "\"OpenPosition\":[";
   
   bool first = true;
   for(int i=0; i<OrdersTotal(); i++)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         string symbol = OrderSymbol();
         double lots = OrderLots();
         double floatingPL = OrderProfit() + OrderSwap() + OrderCommission();
         
         if(!first)
            jsonStr += ",";
         jsonStr += "{";
         jsonStr += "\"CommodityId\":\"" + symbol + "\",";
         jsonStr += "\"Lots\":" + DoubleToString(lots,2) + ",";
         jsonStr += "\"FloatingProfitLoss\":" + DoubleToString(floatingPL,2);
         jsonStr += "}";
         first = false;
      }
   }
   jsonStr += "]}";
   
   Print("傳送至 DLL 的持倉 JSON: ", jsonStr);
   // 呼叫 DLL 函數處理最新持倉，並取得下單訊號 JSON
   string ordersJson = ProcessPositions(jsonStr);
   Print("從 DLL ProcessPositions 回傳的下單訊號 JSON: ", ordersJson);
   
   // 2. 透過 HTTP GET 查詢 DLL HTTP 服務中是否有新訂單訊號
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
      // 若返回的 JSON 內容不為空（非 "{}"），則打印出新訂單訊號
      if(StringTrim(resultStr) != "{}" && StringLen(StringTrim(resultStr)) > 2)
      {
         Print("取得新訂單 JSON 訊號: ", resultStr);
      }
   }
   else
   {
      Print("WebRequest Error: ", GetLastError());
   }
}
