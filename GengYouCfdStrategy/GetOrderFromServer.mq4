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
//| ProcessOrdersFromDLL: 解析 DLL 返回的訂單字串並執行下單操作      |
//| 訂單格式: CommodityId,OrderType,Lots,Amount,OrderSerialNumber,LongShort;... |
//+------------------------------------------------------------------+
void ProcessOrdersFromDLL(string ordersStr)
{
   // 加入額外的 debug 訊息以確保訂單字串正確解析
   Print("[ProcessOrdersFromDLL] Starting processing of orders string: ", ordersStr);
   if (StringLen(ordersStr) <= 2)
   {
      Print("[ProcessOrdersFromDLL] No orders received. ordersStr = ", ordersStr);
      return;
   }

   // 將訂單字串以分號分割成各筆訂單
   string ordersArray[];
   int orderCount = StringSplit(ordersStr, ";", ordersArray);
   Print("[ProcessOrdersFromDLL] Order count: ", orderCount);

   for (int i = 0; i < orderCount; i++)
   {
      if (StringLen(ordersArray[i]) < 5)
      {
         Print("[ProcessOrdersFromDLL] Skipping order index ", i, " because its length is less than 5.");
         continue;
      }

      // 以逗號分割各個欄位
      string fields[];
      int fieldCount = StringSplit(ordersArray[i], ",", fields);
      Print("[ProcessOrdersFromDLL] Order index ", i, " has ", fieldCount, " fields.");
      if (fieldCount < 6)
      {
         Print("[ProcessOrdersFromDLL] Skipping order index ", i, " because field count (", fieldCount, ") is less than 6.");
         continue;
      }

      // 解析各欄位
      string commodityId = fields[0];
      string orderType = fields[1];
      double lots = StrToDouble(fields[2]);
      double amount = StrToDouble(fields[3]);
      int ticket = (int)StrToDouble(fields[4]);
      int longShort = (int)StrToDouble(fields[5]);

      Print("[ProcessOrdersFromDLL] Parsed Order (index ", i, "): CommodityId = ", commodityId,
            ", OrderType = ", orderType, ", Lots = ", DoubleToString(lots, 2),
            ", Amount = ", DoubleToString(amount, 2), ", Ticket = ", IntegerToString(ticket),
            ", LongShort = ", IntegerToString(longShort));

      // 若為停損或停利訂單，則以 OrderClose() 平倉
      if (orderType == "TakeProfit" || orderType == "StopLoss")
      {
         Print("[ProcessOrdersFromDLL] Processing SL/TP order for Ticket = ", IntegerToString(ticket));
         if (OrderSelect(ticket, SELECT_BY_TICKET, MODE_TRADES))
         {
            double price = 0.0;
            // 若多單，平倉價格取 BID；若空單，取 ASK
            if (longShort == 1)
               price = SymbolInfoDouble(commodityId, SYMBOL_BID);
            else if (longShort == -1)
               price = SymbolInfoDouble(commodityId, SYMBOL_ASK);

            Print("[ProcessOrdersFromDLL] Attempting OrderClose for Ticket = ", IntegerToString(ticket),
                  " at price = ", DoubleToString(price, 2));
            if (OrderClose(ticket, OrderLots(), price, 3, clrRed))
               Print("[ProcessOrdersFromDLL] OrderClose succeeded for Ticket = ", IntegerToString(ticket));
            else
               Print("[ProcessOrdersFromDLL] OrderClose failed for Ticket = ", IntegerToString(ticket),
                     " Error = ", IntegerToString(GetLastError()));
         }
         else
         {
            Print("[ProcessOrdersFromDLL] OrderSelect failed for Ticket = ", IntegerToString(ticket));
         }
      }
      // 若為開倉訂單，則以 OrderSend() 下單
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
         else // longShort == -1
         {
            type = OP_SELL;
            price = SymbolInfoDouble(commodityId, SYMBOL_BID);
            stoploss = price + amount;
            takeprofit = price - amount;
         }
         Print("[ProcessOrdersFromDLL] Attempting OrderSend for Commodity = ", commodityId,
               ", Type = ", (type == OP_BUY ? "Buy" : "Sell"),
               ", Price = ", DoubleToString(price, 2),
               ", Lots = ", DoubleToString(lots, 2),
               ", StopLoss = ", DoubleToString(stoploss, 2),
               ", TakeProfit = ", DoubleToString(takeprofit, 2));
         int newTicket = OrderSend(commodityId, type, lots, price, 3, stoploss, takeprofit, "AutoTrade", 12345, 0, (type == OP_BUY ? clrGreen : clrRed));
         if (newTicket > 0)
            Print("[ProcessOrdersFromDLL] OrderSend succeeded: New Ticket = ", IntegerToString(newTicket));
         else
            Print("[ProcessOrdersFromDLL] OrderSend failed. Error = ", IntegerToString(GetLastError()));
      }
      else
      {
         Print("[ProcessOrdersFromDLL] Unknown order type: ", orderType, " for Ticket = ", IntegerToString(ticket));
      }
   }
}

//+------------------------------------------------------------------+
//| Expert Tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   // 1. 更新 CFD 價格：直接傳入 MQ4 抓到的 Symbol (不再限定特定符號)
   string sym = _Symbol;
   double curPrice = iClose(sym, 0, 0);
   GetCurCfdPrices(sym, curPrice);

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

      // 4. 取得可執行訂單字串並執行下單操作
      string ordersStr = GetOrdersForExecution();
      if (StringLen(ordersStr) > 2)
      {
         Print("DLL orders (for execution): ", ordersStr);
         ProcessOrdersFromDLL(ordersStr);
      }
   }
}
