//+------------------------------------------------------------------+
//|                                            AutoSLTPManager.mq5  |
//|        固定美元止损/止盈 + 风控管理 EA                           |
//+------------------------------------------------------------------+
#property copyright "AutoSLTPManager"
#property version   "1.00"
#property strict
#property description "Manage USD-based SL/TP, auto-close and SL/TP recovery"

//—— 输入参数 ——//
input double FixedStopLossUSD    = 30.0;   // 每单固定止损（美元）
input double FixedTakeProfitUSD  = 60.0;   // 每单固定止盈（美元）
input double MaxLossPerPositionUSD = 35.0; // 超过此美元亏损则强制平仓

//—— 全局变量 ——//
datetime g_lastCloseAllTime = 0;  // 记录上次每日清仓时间

//+------------------------------------------------------------------+
//| 计算给定 USD 金额对应的价格差（以点为单位）                     |
//+------------------------------------------------------------------+
double CalculatePriceDiff(double usd, double volume, string symbol)
{
   double tick_value = SymbolInfoDouble(symbol, SYMBOL_TRADE_TICK_VALUE);
   double tick_size  = SymbolInfoDouble(symbol, SYMBOL_TRADE_TICK_SIZE);
   if (tick_value <= 0 || tick_size <= 0 || volume <= 0)
      return 0.0;
   // USD / (每手每点价值) 再除以手数 = 点差
   return (usd / (tick_value / tick_size)) / volume;
}

//+------------------------------------------------------------------+
//| 下单并自动带入 SL/TP                                            |
//+------------------------------------------------------------------+
bool OpenOrderWithSLTP(string symbol, ENUM_ORDER_TYPE order_type, double volume)
{
   double price = (order_type == ORDER_TYPE_BUY)
                  ? SymbolInfoDouble(symbol, SYMBOL_ASK)
                  : SymbolInfoDouble(symbol, SYMBOL_BID);

   double sl_diff = CalculatePriceDiff(FixedStopLossUSD,    volume, symbol);
   double tp_diff = CalculatePriceDiff(FixedTakeProfitUSD,  volume, symbol);

   double sl = (order_type == ORDER_TYPE_BUY) ? price - sl_diff : price + sl_diff;
   double tp = (order_type == ORDER_TYPE_BUY) ? price + tp_diff : price - tp_diff;

   MqlTradeRequest request; MqlTradeResult result;
   ZeroMemory(request); ZeroMemory(result);

   request.action   = TRADE_ACTION_DEAL;
   request.symbol   = symbol;
   request.type     = order_type;
   request.volume   = volume;
   request.price    = NormalizeDouble(price, _Digits);
   request.sl       = NormalizeDouble(sl,    _Digits);
   request.tp       = NormalizeDouble(tp,    _Digits);
   request.deviation= 10;
   request.magic    = 12345;
   request.type_filling = ORDER_FILLING_IOC;

   return OrderSend(request, result);
}

//+------------------------------------------------------------------+
//| 若浮动亏损超限／盈利达标，则市价平仓                             |
//+------------------------------------------------------------------+
void CheckAndCloseOrders()
{
   int total = PositionsTotal();
   for(int i = 0; i < total; i++)
   {
      if(!PositionSelectByIndex(i)) 
         continue;

      string symbol = PositionGetString(POSITION_SYMBOL);
      double profit = PositionGetDouble(POSITION_PROFIT);
      double volume = PositionGetDouble(POSITION_VOLUME);
      int    type   = (int)PositionGetInteger(POSITION_TYPE);
      ulong  ticket = PositionGetInteger(POSITION_TICKET);

      if(profit <= -MaxLossPerPositionUSD || profit >=  FixedTakeProfitUSD)
      {
         double price = (type == POSITION_TYPE_BUY)
                        ? SymbolInfoDouble(symbol, SYMBOL_BID)
                        : SymbolInfoDouble(symbol, SYMBOL_ASK);

         MqlTradeRequest request; MqlTradeResult result;
         ZeroMemory(request); ZeroMemory(result);

         request.action   = TRADE_ACTION_DEAL;
         request.symbol   = symbol;
         request.position = ticket;
         request.volume   = volume;
         request.type     = (type == POSITION_TYPE_BUY) ? ORDER_TYPE_SELL : ORDER_TYPE_BUY;
         request.price    = NormalizeDouble(price, _Digits);
         request.deviation= 10;
         request.type_filling = ORDER_FILLING_IOC;

         OrderSend(request, result);
      }
   }
}

//+------------------------------------------------------------------+
//| 补齐未带 SL/TP 的持仓                                            |
//+------------------------------------------------------------------+
void CheckAndSetSLTPIfMissing()
{
   int total = PositionsTotal();
   for(int i = 0; i < total; i++)
   {
      if(!PositionSelectByIndex(i)) 
         continue;

      string symbol = PositionGetString(POSITION_SYMBOL);
      double sl_open = PositionGetDouble(POSITION_SL);
      double tp_open = PositionGetDouble(POSITION_TP);
      if(sl_open != 0.0 && tp_open != 0.0) 
         continue;

      double volume     = PositionGetDouble(POSITION_VOLUME);
      double price_open = PositionGetDouble(POSITION_PRICE_OPEN);
      int    type       = (int)PositionGetInteger(POSITION_TYPE);
      ulong  ticket     = PositionGetInteger(POSITION_TICKET);

      double sl_diff = CalculatePriceDiff(FixedStopLossUSD,   volume, symbol);
      double tp_diff = CalculatePriceDiff(FixedTakeProfitUSD, volume, symbol);

      double new_sl = (type == POSITION_TYPE_BUY) ? price_open - sl_diff : price_open + sl_diff;
      double new_tp = (type == POSITION_TYPE_BUY) ? price_open + tp_diff : price_open - tp_diff;

      MqlTradeRequest request; MqlTradeResult result;
      ZeroMemory(request); ZeroMemory(result);

      request.action   = TRADE_ACTION_SLTP;
      request.symbol   = symbol;
      request.position = ticket;
      request.sl       = NormalizeDouble(new_sl, _Digits);
      request.tp       = NormalizeDouble(new_tp, _Digits);
      request.magic    = 12345;

      OrderSend(request, result);
   }
}

//+------------------------------------------------------------------+
//| 每天 04:00 全平仓                                                  |
//+------------------------------------------------------------------+
void CloseAllPositionsAt4AM()
{
   datetime now = TimeCurrent();
   MqlDateTime dt_now, dt_last;
   TimeToStruct(now,     dt_now);
   TimeToStruct(g_lastCloseAllTime, dt_last);

   if(dt_now.hour == 4 && dt_now.day != dt_last.day)
   {
      int total = PositionsTotal();
      for(int i = 0; i < total; i++)
      {
         if(!PositionSelectByIndex(i)) 
            continue;

         string symbol = PositionGetString(POSITION_SYMBOL);
         double volume = PositionGetDouble(POSITION_VOLUME);
         int    type   = (int)PositionGetInteger(POSITION_TYPE);
         ulong  ticket = PositionGetInteger(POSITION_TICKET);

         double price = (type == POSITION_TYPE_BUY)
                        ? SymbolInfoDouble(symbol, SYMBOL_BID)
                        : SymbolInfoDouble(symbol, SYMBOL_ASK);

         MqlTradeRequest request; MqlTradeResult result;
         ZeroMemory(request); ZeroMemory(result);

         request.action   = TRADE_ACTION_DEAL;
         request.symbol   = symbol;
         request.position = ticket;
         request.volume   = volume;
         request.type     = (type == POSITION_TYPE_BUY) ? ORDER_TYPE_SELL : ORDER_TYPE_BUY;
         request.price    = NormalizeDouble(price, _Digits);
         request.deviation= 10;
         request.type_filling = ORDER_FILLING_IOC;

         OrderSend(request, result);
      }
      g_lastCloseAllTime = now;
   }
}

//+------------------------------------------------------------------+
//| Expert 主循环                                                    |
//+------------------------------------------------------------------+
void OnTick()
{
   CloseAllPositionsAt4AM();
   CheckAndSetSLTPIfMissing();
   CheckAndCloseOrders();
}
