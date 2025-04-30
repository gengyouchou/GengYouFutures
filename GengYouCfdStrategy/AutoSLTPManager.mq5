//+------------------------------------------------------------------+
//|                                            AutoSLTPManager.mq5  |
//|        固定美元止损/止盈 + 自动风控 EA                          |
//+------------------------------------------------------------------+
#property copyright "AutoSLTPManager"
#property version   "1.00"
#property strict
#property description "Manage USD-based SL/TP, auto-close and SL/TP recovery"

//—— 输入参数 ——//
input double FixedStopLossUSD      = 30.0;   // 固定止损（美元）
input double FixedTakeProfitUSD    = 60.0;   // 固定止盈（美元）
input double MaxLossPerPositionUSD = 35.0;   // 超过此美元亏损则强制平仓

//—— 全局变量 ——//
datetime g_lastCloseAllTime = 0;               // 记录上次 04:00 全平仓的时间

//+------------------------------------------------------------------+
//| 计算 USD 金额对应的点差                                           |
//+------------------------------------------------------------------+
double CalculatePriceDiff(double usd, double volume, string symbol)
{
   double tick_value = SymbolInfoDouble(symbol, SYMBOL_TRADE_TICK_VALUE);
   double tick_size  = SymbolInfoDouble(symbol, SYMBOL_TRADE_TICK_SIZE);
   if (tick_value <= 0 || tick_size <= 0 || volume <= 0.0)
      return 0.0;
   // USD / (每手每点价值) 再除以手数 = 点差
   return (usd / (tick_value / tick_size)) / volume;
}

//+------------------------------------------------------------------+
//| 下单并自动带入 SL/TP                                             |
//+------------------------------------------------------------------+
bool OpenOrderWithSLTP(string symbol, ENUM_ORDER_TYPE order_type, double volume)
{
   double price = (order_type == ORDER_TYPE_BUY)
                  ? SymbolInfoDouble(symbol, SYMBOL_ASK)
                  : SymbolInfoDouble(symbol, SYMBOL_BID);

   double sl_diff = CalculatePriceDiff(FixedStopLossUSD,   volume, symbol);
   double tp_diff = CalculatePriceDiff(FixedTakeProfitUSD, volume, symbol);

   double sl = (order_type == ORDER_TYPE_BUY) ? price - sl_diff : price + sl_diff;
   double tp = (order_type == ORDER_TYPE_BUY) ? price + tp_diff : price - tp_diff;

   MqlTradeRequest request; MqlTradeResult result;
   ZeroMemory(request); ZeroMemory(result);

   request.action       = TRADE_ACTION_DEAL;
   request.symbol       = symbol;
   request.type         = order_type;
   request.volume       = volume;
   request.price        = NormalizeDouble(price, _Digits);
   request.sl           = NormalizeDouble(sl,    _Digits);
   request.tp           = NormalizeDouble(tp,    _Digits);
   request.deviation    = 10;
   request.magic        = 12345;
   request.type_filling = ORDER_FILLING_IOC;

   return OrderSend(request, result);
}

//+------------------------------------------------------------------+
//| 浮动盈亏检查，超损/达盈则市价平仓                                 |
//+------------------------------------------------------------------+
void CheckAndCloseOrders()
{
   int total = PositionsTotal();
   for(int i = total - 1; i >= 0; --i)
   {
      // 取出票号并选中该仓位
      ulong ticket = PositionGetTicket(i);
      if(ticket == 0 || !PositionSelectByTicket(ticket))
         continue;

      string symbol = PositionGetString(POSITION_SYMBOL);
      double profit = PositionGetDouble(POSITION_PROFIT);
      double volume = PositionGetDouble(POSITION_VOLUME);
      int    type   = (int)PositionGetInteger(POSITION_TYPE);

      // 超过最大亏损 或 达到止盈金额
      if(profit <= -MaxLossPerPositionUSD || profit >= FixedTakeProfitUSD)
      {
         double price = (type == POSITION_TYPE_BUY)
                        ? SymbolInfoDouble(symbol, SYMBOL_BID)
                        : SymbolInfoDouble(symbol, SYMBOL_ASK);

         MqlTradeRequest request; MqlTradeResult result;
         ZeroMemory(request); ZeroMemory(result);

         request.action       = TRADE_ACTION_DEAL;
         request.symbol       = symbol;
         request.position     = ticket;
         request.volume       = volume;
         request.type         = (type == POSITION_TYPE_BUY)
                                ? ORDER_TYPE_SELL
                                : ORDER_TYPE_BUY;
         request.price        = NormalizeDouble(price, _Digits);
         request.deviation    = 10;
         request.type_filling = ORDER_FILLING_IOC;

         OrderSend(request, result);
      }
   }
}

//+------------------------------------------------------------------+
//| 补齐所有未设置 SL/TP 的持仓                                      |
//+------------------------------------------------------------------+
void CheckAndSetSLTPIfMissing()
{
   int total = PositionsTotal();
   for(int i = total - 1; i >= 0; --i)
   {
      ulong ticket = PositionGetTicket(i);
      if(ticket == 0 || !PositionSelectByTicket(ticket))
         continue;

      string symbol      = PositionGetString(POSITION_SYMBOL);
      double sl_current  = PositionGetDouble(POSITION_SL);
      double tp_current  = PositionGetDouble(POSITION_TP);
      double open_price  = PositionGetDouble(POSITION_PRICE_OPEN);
      double volume      = PositionGetDouble(POSITION_VOLUME);
      int    type        = (int)PositionGetInteger(POSITION_TYPE);

      // 如果已经带了 SL/TP 就跳过
      if(sl_current != 0.0 && tp_current != 0.0)
         continue;

      double sl_diff = CalculatePriceDiff(FixedStopLossUSD,   volume, symbol);
      double tp_diff = CalculatePriceDiff(FixedTakeProfitUSD, volume, symbol);

      double new_sl = (type == POSITION_TYPE_BUY)
                      ? open_price - sl_diff
                      : open_price + sl_diff;
      double new_tp = (type == POSITION_TYPE_BUY)
                      ? open_price + tp_diff
                      : open_price - tp_diff;

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
//| 每天 04:00 自动全平仓                                             |
//+------------------------------------------------------------------+
void CloseAllPositionsAt4AM()
{
   datetime now = TimeCurrent();
   MqlDateTime dt_now, dt_last;
   TimeToStruct(now,     dt_now);
   TimeToStruct(g_lastCloseAllTime, dt_last);

   // 本地时间 4 点，且与上次记录的日期不同
   if(dt_now.hour == 4 && dt_now.day != dt_last.day)
   {
      int total = PositionsTotal();
      for(int i = total - 1; i >= 0; --i)
      {
         ulong ticket = PositionGetTicket(i);
         if(ticket == 0 || !PositionSelectByTicket(ticket))
            continue;

         string symbol = PositionGetString(POSITION_SYMBOL);
         double volume = PositionGetDouble(POSITION_VOLUME);
         int    type   = (int)PositionGetInteger(POSITION_TYPE);

         double price = (type == POSITION_TYPE_BUY)
                        ? SymbolInfoDouble(symbol, SYMBOL_BID)
                        : SymbolInfoDouble(symbol, SYMBOL_ASK);

         MqlTradeRequest request; MqlTradeResult result;
         ZeroMemory(request); ZeroMemory(result);

         request.action       = TRADE_ACTION_DEAL;
         request.symbol       = symbol;
         request.position     = ticket;
         request.volume       = volume;
         request.type         = (type == POSITION_TYPE_BUY)
                                ? ORDER_TYPE_SELL
                                : ORDER_TYPE_BUY;
         request.price        = NormalizeDouble(price, _Digits);
         request.deviation    = 10;
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
