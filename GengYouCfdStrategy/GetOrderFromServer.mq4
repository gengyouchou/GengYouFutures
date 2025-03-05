#property strict

// 導入 DLL 中的函數 (請確保 DLL 已放在 MQL4\Libraries 目錄中)
#import "GengYouCfdStrategy.dll"
   // 啟動與停止 DLL 內部 HTTP 服務器（及 CMD log 輸出）
   void StartHttpServer();
   void StopHttpServer();
   // 更新當前 CFD 價格，以商品代號與價格傳遞
   void GetCurCfdPrices(string commodityId, double commodityCurPrice);
   // 傳遞單筆未平倉訂單資料給 DLL，包含多空方向（1：多頭，-1：空頭）
   void GetCurOpenPosition(string commodityId, int ticket, double costPrice, double lots, double floatingPL, int longShort);
   // 處理所有傳入的開倉資料，計算停損／停利邏輯，返回下單 JSON
   string ProcessSimulatedPositions();
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
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
   // 1. 更新 CFD 價格：僅對指定商品更新成交價
   // 根據您的需求，僅對 "XAUUSD", "USDX", "NAS100ft" 更新 CFD 價格
   string symbol = _Symbol;
   if(symbol=="XAUUSD" || symbol=="USDX" || symbol=="NAS100ft")
   {
      // 使用成交價 (最近一根 K 線的收盤價)
      double curPrice = iClose(symbol, 0, 0);
      GetCurCfdPrices(symbol, curPrice);
   }

   // 2. 遍歷所有開倉訂單，將每筆資料傳遞給 DLL
   int total = OrdersTotal();
   for (int i = 0; i < total; i++)
   {
      if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int ticket = OrderTicket();
         double costPrice = OrderOpenPrice();
         double lots = OrderLots();
         double floatingPL = OrderProfit() + OrderSwap() + OrderCommission();
         string sym = OrderSymbol();
         // 判斷訂單方向：OP_BUY 為多頭 (1)，OP_SELL 為空頭 (-1)
         int direction = (OrderType() == OP_BUY) ? 1 : -1;
         GetCurOpenPosition(sym, ticket, costPrice, lots, floatingPL, direction);
      }
   }

   // 3. 呼叫 ProcessSimulatedPositions() 進行停損/停利邏輯計算，並取得下單 JSON
   string ordersJson = ProcessSimulatedPositions();
   if (StringLen(ordersJson) > 2)
   {
      Print("DLL orders: ", ordersJson);
   }
}
