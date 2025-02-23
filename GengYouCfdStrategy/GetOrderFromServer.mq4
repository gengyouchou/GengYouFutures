//+------------------------------------------------------------------+
//|                                           GetOrderFromServer.mq4 |
//| 此 EA 定時呼叫 DLL 的 CustomProcessParameters() 傳入必要參數，    |
//| 並直接呼叫 ParseNewOrder() 取得 DLL 解析後的下單資訊，然後打印出來。 |
//+------------------------------------------------------------------+
#property strict

// 輪詢間隔（最小單位為 1 秒）
int TimerPeriod = 1;

// 導入 DLL 中的函數
#import "GengYouCfdStrategy.dll"
   void   StartHttpServer();
   void   StopHttpServer();
   string CustomProcessParameters(double margin, double closedPL, string commodityId, double lots, double floatingPL);
   string ParseNewOrder(); // 解析並返回分開的下單資訊
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   // 啟動 DLL 的 HTTP 服務器
   StartHttpServer();
   Print("GetOrderFromServer EA 啟動，等待外部訂單傳入...");

   // 每秒輪詢一次
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
//| 每秒呼叫一次                                                     |
//+------------------------------------------------------------------+
void OnTimer()
{
   // 取得必要參數
   double margin = AccountFreeMargin();
   double closedPL = 0.0; // 可依實際情況計算

   // 若有至少一筆持倉，取第一筆參數
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

   // 呼叫 DLL 函數，構造 JSON（僅作為參考示例）
   string jsonResult = CustomProcessParameters(margin, closedPL, commodityId, lots, floatingPL);
   Print("CustomProcessParameters 返回的 JSON: ", jsonResult);

   // 呼叫 DLL 新增的解析函數，取得分開的下單資訊
   string parsedOrder = ParseNewOrder();
   if(parsedOrder != "{}" && StringLen(parsedOrder) > 2)
   {
      Print("取得解析後的下單資訊: ", parsedOrder);
      // 進一步處理 parsedOrder（例如解析各欄位，下單操作等）
   }
}
