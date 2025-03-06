#property strict

// 導入 DLL 中的函數
#import "GengYouCfdStrategy.dll"
void StartHttpServer();
void StopHttpServer();
void GetCurCfdPrices(string commodityId, double commodityCurPrice);
void GetCurOpenPosition(string commodityId, int ticket, double costPrice, double lots, double floatingPL, int longShort);
string ProcessSimulatedPositions();
// 注意此處回傳型態為 string
string GetOrdersForExecution();
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   StartHttpServer();
   Print("EA initialized. DLL HTTP server started.");
   return (INIT_SUCCEEDED);
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
//| Helper function: Process orders from DLL and execute trades      |
//| 格式: CommodityId,OrderType,Lots,Amount,OrderSerialNumber,LongShort;... |
//+------------------------------------------------------------------+
void ProcessOrdersFromDLL(string ordersStr)
{
   if (StringLen(ordersStr) <= 2)
      return;

   // *** 修改處：先把分隔符號存到變數 delimiter，避免 "implicit conversion" 錯誤 ***
   string delimiter = ";";

   // 以 delimiter 分割 ordersStr
   string ordersArray[];
   int orderCount = StringSplit(ordersStr, delimiter, ordersArray);

   // 其餘程式碼保持原狀
   for (int i = 0; i < orderCount; i++)
   {
      if (StringLen(ordersArray[i]) < 5)
         continue;
      string fields[];
      int fieldCount = StringSplit(ordersArray[i], ",", fields);
      if (fieldCount < 6)
         continue;

      string commodityId = fields[0];
      string orderType = fields[1];
      double lots = StrToDouble(fields[2]);
      double amount = StrToDouble(fields[3]);
      int ticket = (int)StrToDouble(fields[4]);
      int longShort = (int)StrToDouble(fields[5]);

      // (以下示範停損停利/開倉/平倉邏輯皆不變)
      if (orderType == "TakeProfit" || orderType == "StopLoss")
      {
         if (OrderSelect(ticket, SELECT_BY_TICKET, MODE_TRADES))
         {
            double price = 0.0;
            if (longShort == 1)
               price = SymbolInfoDouble(commodityId, SYMBOL_BID);
            else
               price = SymbolInfoDouble(commodityId, SYMBOL_ASK);

            if (OrderClose(ticket, OrderLots(), price, 3, clrRed))
               Print("OrderClose succeeded for Ticket ", IntegerToString(ticket));
            else
               Print("OrderClose failed for Ticket ", IntegerToString(ticket), " Error: ", IntegerToString(GetLastError()));
         }
         else
         {
            Print("OrderSelect failed for Ticket ", IntegerToString(ticket));
         }
      }
      else if (orderType == "BaseOrder" || orderType == "AddOrder")
      {
         int type;
         double price, stoploss, takeprofit;
         if (longShort == 1)
         {
            type = OP_BUY;
            price = SymbolInfoDouble(commodityId, SYMBOL_ASK);
            stoploss = price - amount;
            takeprofit = price + amount;
         }
         else
         {
            type = OP_SELL;
            price = SymbolInfoDouble(commodityId, SYMBOL_BID);
            stoploss = price + amount;
            takeprofit = price - amount;
         }
         int newTicket = OrderSend(commodityId, type, lots, price, 3, stoploss, takeprofit, "AutoTrade", 12345, 0, (type == OP_BUY) ? clrGreen : clrRed);
         if (newTicket > 0)
            Print("OrderSend succeeded: Ticket ", IntegerToString(newTicket));
         else
            Print("OrderSend failed. Error: ", IntegerToString(GetLastError()));
      }
   }
}

//+------------------------------------------------------------------+
//| Expert Tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   // 1. 更新 CFD 價格 (針對 "GOLD", "USD", "NAS100" 等)
   string sym = _Symbol;
   if (sym == "GOLD" || sym == "USD" || sym == "NAS100")
   {
      double curPrice = iClose(sym, 0, 0);
      GetCurCfdPrices(sym, curPrice);
   }

   // 2. 遍歷所有開倉訂單，將未平倉資訊傳給 DLL
   int total = OrdersTotal();
   for (int i = 0; i < total; i++)
   {
      if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int ticket = OrderTicket();
         double costPrice = OrderOpenPrice();
         double lots = OrderLots();
         double floatingPL = OrderProfit() + OrderSwap() + OrderCommission();
         string orderSym = OrderSymbol();
         int direction = (OrderType() == OP_BUY) ? 1 : -1;
         GetCurOpenPosition(orderSym, ticket, costPrice, lots, floatingPL, direction);
      }
   }

   // 3. 呼叫 ProcessSimulatedPositions() 更新邏輯，並列印 JSON
   string ordersJson = ProcessSimulatedPositions();
   if (StringLen(ordersJson) > 2)
   {
      Print("DLL orders (JSON): ", ordersJson);

      // 4. 取得執行訂單字串 => ProcessOrdersFromDLL()
      string ordersStr = GetOrdersForExecution();
      if (StringLen(ordersStr) > 2)
      {
         Print("DLL orders (for execution): ", ordersStr);
         ProcessOrdersFromDLL(ordersStr);
      }
   }
}
