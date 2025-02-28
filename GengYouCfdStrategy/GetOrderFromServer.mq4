#property strict

// 導入 DLL 中的函數，使用多個參數傳遞訂單明細
#import "GengYouCfdStrategy.dll"
   void StartHttpServer();
   void StopHttpServer();
   // 以多參數傳遞未平倉資訊：商品代號、訂單號(ticket)、開倉價格、口數、浮動盈虧
   void GetCurOpenPosition(string commodityId, int ticket, double costPrice, double lots, double floatingPL);
   // 處理所有傳入的開倉資料，並返回下單 JSON 字串
   string ProcessSimulatedPositions();
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   // 啟動 DLL 的 HTTP 服務器（並開啟 CMD 視窗輸出 log）
   StartHttpServer();
   Print("EA initialized. DLL HTTP server started.");
   return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   StopHttpServer();
   Print("EA deinitialized. DLL HTTP server stopped.");
}

//+------------------------------------------------------------------+
//| Expert Tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   // 每個 Tick 遍歷所有開倉訂單，傳送未平倉資訊給 DLL
   int total = OrdersTotal();
   for(int i = 0; i < total; i++)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int ticket = OrderTicket();
         double costPrice = OrderOpenPrice();
         double lots = OrderLots();
         double floatingPL = OrderProfit() + OrderSwap() + OrderCommission();
         string sym = OrderSymbol();
         // 呼叫 DLL 函數，依據多參數傳遞
         GetCurOpenPosition(sym, ticket, costPrice, lots, floatingPL);
      }
   }
   
   // 每次 Tick 呼叫 ProcessSimulatedPositions()，取得 DLL 處理後的下單 JSON
   string ordersJson = ProcessSimulatedPositions();
   if(StringLen(ordersJson) > 2)
   {
      // 將 DLL 返回的下單訊號輸出到 Experts 日誌
      Print("DLL orders: ", ordersJson);
   }
}
