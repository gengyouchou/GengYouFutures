#property strict

// 導入 DLL 中的函數
#import "GengYouCfdStrategy.dll"
void StartHttpServer();
void StopHttpServer();
void GetCurCfdPrices(string commodityId, double commodityCurPrice);
void GetCurOpenPosition(string commodityId, int ticket, double costPrice, double lots, double floatingPL, int longShort);
string ProcessSimulatedPositions();
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
//| 訂單格式: CommodityId,OrderType,Lots,Amount,OrderSerialNumber,LongShort;... |
//+------------------------------------------------------------------+
void ProcessOrdersFromDLL(string ordersStr)
{
   if (StringLen(ordersStr) <= 2)
      return;

   // 將分隔符號存入變數，避免隱式轉換問題
   string delimiter = ";";
   string ordersArray[];
   int orderCount = StringSplit(ordersStr, delimiter, ordersArray);

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

      // 使用 StringCompare() 進行比對
      if (StringCompare(orderType, "TakeProfit") == 0 || StringCompare(orderType, "StopLoss") == 0)
      {
         if (OrderSelect(ticket, SELECT_BY_TICKET, MODE_TRADES))
         {
            double price = 0.0;
            if (longShort == 1)
               price = SymbolInfoDouble(commodityId, SYMBOL_BID);
            else if (longShort == -1)
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
      else if (StringCompare(orderType, "BaseOrder") == 0 || StringCompare(orderType, "AddOrder") == 0)
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
   // 1. 更新 CFD 價格：僅針對 "XAUUSD", "nas100ft" 使用 iClose() (成交價)
   string sym = _Symbol;
   // 注意：使用 StringCompare() 來比較字串，避免直接使用 "==" 帶來的問題
   if (StringCompare(sym, "XAUUSD") == 0 || StringCompare(sym, "nas100ft") == 0)
   {
      double curPrice = iClose(sym, 0, 0);
      GetCurCfdPrices(sym, curPrice);
   }

   // 2. 遍歷所有開倉訂單，傳送未平倉資訊給 DLL
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

   // 3. 呼叫 ProcessSimulatedPositions() 更新停損/停利邏輯 (JSON 格式)
   string ordersJson = ProcessSimulatedPositions();
   if (StringLen(ordersJson) > 2)
   {
      Print("DLL orders (JSON): ", ordersJson);

      // 4. 取得執行訂單字串並執行下單操作
      string ordersStr = GetOrdersForExecution();
      if (StringLen(ordersStr) > 2)
      {
         Print("DLL orders (for execution): ", ordersStr);
         ProcessOrdersFromDLL(ordersStr);
      }
   }
}
