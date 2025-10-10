//+------------------------------------------------------------------+
//|                                            AutoSLTPManager.mq4  |
//|        固定美元止損/止盈 + 自動風控 EA (MQL4版)                 |
//+------------------------------------------------------------------+
#property copyright "AutoSLTPManager"
#property version   "1.00"
#property strict

//—— 輸入參數 ——//
extern double FixedStopLossUSD      = 20.0;   // 固定止損（美元）
extern double FixedTakeProfitUSD    = 40.0;   // 固定止盈（美元）
extern double MaxLossPerPositionUSD = 35.0;   // 超過此美元虧損則強制平倉

//—— 全域變數 ——//
datetime g_lastCloseAllTime = 0; // 上次 04:00 全平時間

//+------------------------------------------------------------------+
//| 計算美元金額對應點差                                             |
//+------------------------------------------------------------------+
double CalculatePriceDiff(string symbol, double usd, double volume)
{
   double tick_value = MarketInfo(symbol, MODE_TICKVALUE);
   double tick_size  = MarketInfo(symbol, MODE_TICKSIZE);
   if (tick_value <= 0 || tick_size <= 0 || volume <= 0.0)
      return 0.0;
   return (usd / (tick_value / tick_size)) / volume;
}

//+------------------------------------------------------------------+
//| 下單並自帶 SL/TP                                                 |
//+------------------------------------------------------------------+
bool OpenOrderWithSLTP(string symbol, int order_type, double volume)
{
   double price = (order_type == OP_BUY)
                  ? MarketInfo(symbol, MODE_ASK)
                  : MarketInfo(symbol, MODE_BID);

   double sl_diff = CalculatePriceDiff(symbol, FixedStopLossUSD, volume);
   double tp_diff = CalculatePriceDiff(symbol, FixedTakeProfitUSD, volume);

   double sl = (order_type == OP_BUY) ? price - sl_diff : price + sl_diff;
   double tp = (order_type == OP_BUY) ? price + tp_diff : price - tp_diff;

   int slippage = 10;

   int ticket = OrderSend(symbol, order_type, volume, price, slippage, sl, tp, 
                          "AutoSLTP", 12345, 0, clrBlue);
   if (ticket < 0)
   {
      Print("OrderSend failed: ", GetLastError());
      return false;
   }
   return true;
}

//+------------------------------------------------------------------+
//| 檢查持倉浮動盈虧                                                 |
//+------------------------------------------------------------------+
void CheckAndCloseOrders()
{
   for(int i = OrdersTotal() - 1; i >= 0; i--)
   {
      if(!OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
         continue;

      string symbol = OrderSymbol();
      double profit = OrderProfit() + OrderSwap() + OrderCommission();
      double volume = OrderLots();
      int type      = OrderType();

      if(type != OP_BUY && type != OP_SELL) continue;

      if(profit <= -MaxLossPerPositionUSD || profit >= FixedTakeProfitUSD)
      {
         double price = (type == OP_BUY)
                        ? MarketInfo(symbol, MODE_BID)
                        : MarketInfo(symbol, MODE_ASK);

         int close_type = (type == OP_BUY) ? OP_SELL : OP_BUY;

         bool closed = OrderClose(OrderTicket(), volume, price, 10, clrRed);
         if(closed)
            Print("Order ", OrderTicket(), " closed at profit ", profit);
         else
            Print("Close failed: ", GetLastError());
      }
   }
}

//+------------------------------------------------------------------+
//| 自動補齊未設 SL/TP 的單                                          |
//+------------------------------------------------------------------+
void CheckAndSetSLTPIfMissing()
{
   for(int i = OrdersTotal() - 1; i >= 0; i--)
   {
      if(!OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
         continue;

      string symbol = OrderSymbol();
      double sl_current = OrderStopLoss();
      double tp_current = OrderTakeProfit();
      double open_price = OrderOpenPrice();
      double volume     = OrderLots();
      int type          = OrderType();

      if(type != OP_BUY && type != OP_SELL)
         continue;

      if(sl_current != 0.0 && tp_current != 0.0)
         continue;

      double sl_diff = CalculatePriceDiff(symbol, FixedStopLossUSD, volume);
      double tp_diff = CalculatePriceDiff(symbol, FixedTakeProfitUSD, volume);

      double new_sl = (type == OP_BUY) ? open_price - sl_diff : open_price + sl_diff;
      double new_tp = (type == OP_BUY) ? open_price + tp_diff : open_price - tp_diff;

      bool modified = OrderModify(OrderTicket(),
                                  open_price,
                                  NormalizeDouble(new_sl, Digits),
                                  NormalizeDouble(new_tp, Digits),
                                  0, clrYellow);
      if(!modified)
         Print("OrderModify failed: ", GetLastError());
   }
}

//+------------------------------------------------------------------+
//| 每天 04:00 全平倉 (使用本地時間)                                 |
//+------------------------------------------------------------------+
void CloseAllPositionsAt4AM()
{
   datetime now = TimeLocal();  // ← 改這裡：使用本地時間
   int hour = TimeHour(now);
   int day  = TimeDay(now);
   int last_day = TimeDay(g_lastCloseAllTime);

   if(hour == 4 && day != last_day)
   {
      for(int i = OrdersTotal() - 1; i >= 0; i--)
      {
         if(!OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
            continue;

         string symbol = OrderSymbol();
         double volume = OrderLots();
         int type      = OrderType();

         if(type != OP_BUY && type != OP_SELL)
            continue;

         double price = (type == OP_BUY)
                        ? MarketInfo(symbol, MODE_BID)
                        : MarketInfo(symbol, MODE_ASK);

         bool closed = OrderClose(OrderTicket(), volume, price, 10, clrRed);
         if(!closed)
            Print("OrderClose failed: ", GetLastError());
      }
      g_lastCloseAllTime = now;
   }
}

//+------------------------------------------------------------------+
//| 主迴圈 OnTick                                                    |
//+------------------------------------------------------------------+
void OnTick()
{
   CloseAllPositionsAt4AM();
   CheckAndSetSLTPIfMissing();
   CheckAndCloseOrders();
}
