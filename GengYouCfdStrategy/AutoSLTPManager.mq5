//+------------------------------------------------------------------+
//|                                                AutoSLTPManager.mq5|
//|                      Copyright © 2025                            |
//+------------------------------------------------------------------+
#property script_show_inputs
#property strict

input double LotSize = 0.1;
input double FixedStopLossUSD = 30.0;
input double FixedTakeProfitUSD = 60.0;
input double MaxLossPerPositionUSD = 35.0;
datetime g_lastAutoCloseDate = 0;

//+------------------------------------------------------------------+
//| 計算某個 USD 金額對應的價格差距                                  |
//+------------------------------------------------------------------+
double CalculatePriceDiff(double usd, double volume, string symbol)
{
   double tick_value = SymbolInfoDouble(symbol, SYMBOL_TRADE_TICK_VALUE);
   double tick_size  = SymbolInfoDouble(symbol, SYMBOL_TRADE_TICK_SIZE);
   if (tick_value == 0 || tick_size == 0)
      return 0;

   return (usd / (tick_value / tick_size)) / volume;
}

//+------------------------------------------------------------------+
//| 建立帶止損 / 止盈的下單請求                                      |
//+------------------------------------------------------------------+
void OpenOrderWithSLTP(string symbol, ENUM_ORDER_TYPE order_type, double lot)
{
   double price = (order_type == ORDER_TYPE_BUY) ? SymbolInfoDouble(symbol, SYMBOL_ASK)
                                                 : SymbolInfoDouble(symbol, SYMBOL_BID);

   double slDiff = CalculatePriceDiff(FixedStopLossUSD, lot, symbol);
   double tpDiff = CalculatePriceDiff(FixedTakeProfitUSD, lot, symbol);

   double sl = 0, tp = 0;
   if (order_type == ORDER_TYPE_BUY)
   {
      sl = price - slDiff;
      tp = price + tpDiff;
   }
   else
   {
      sl = price + slDiff;
      tp = price - tpDiff;
   }

   MqlTradeRequest request;
   MqlTradeResult result;
   ZeroMemory(request);
   ZeroMemory(result);

   request.action = TRADE_ACTION_DEAL;
   request.symbol = symbol;
   request.type = order_type;
   request.volume = lot;
   request.price = NormalizeDouble(price, _Digits);
   request.sl = NormalizeDouble(sl, _Digits);
   request.tp = NormalizeDouble(tp, _Digits);
   request.deviation = 10;
   request.magic = 12345;

   if (!OrderSend(request, result))
      Print("下單失敗: ", result.retcode, " - ", result.comment);
   else
      Print("✅ 已下單：", symbol, ", SL=", request.sl, ", TP=", request.tp);
}

//+------------------------------------------------------------------+
//| 自動平倉超過損失限制的部位                                      |
//+------------------------------------------------------------------+
void CheckAndCloseOrders()
{
   for (int i = PositionsTotal() - 1; i >= 0; i--)
   {
      if (!PositionGetTicket(i)) continue;

      string symbol = PositionGetString(POSITION_SYMBOL);
      double profit = PositionGetDouble(POSITION_PROFIT);
      ulong ticket = PositionGetInteger(POSITION_IDENTIFIER);
      int type = (int)PositionGetInteger(POSITION_TYPE);
      double volume = PositionGetDouble(POSITION_VOLUME);

      if (profit <= -MaxLossPerPositionUSD)
      {
         double price = (type == POSITION_TYPE_BUY) ? SymbolInfoDouble(symbol, SYMBOL_BID)
                                                    : SymbolInfoDouble(symbol, SYMBOL_ASK);

         MqlTradeRequest request;
         MqlTradeResult result;
         ZeroMemory(request);
         ZeroMemory(result);

         request.action = TRADE_ACTION_DEAL;
         request.symbol = symbol;
         request.position = ticket;
         request.volume = volume;
         request.type = (type == POSITION_TYPE_BUY) ? ORDER_TYPE_SELL : ORDER_TYPE_BUY;
         request.price = NormalizeDouble(price, _Digits);
         request.deviation = 10;

         if (!OrderSend(request, result))
            Print("❌ 平倉失敗：", result.retcode, " - ", result.comment);
         else
            Print("⚠️ 已平倉超損部位：", symbol, " 損益=", profit);
      }
   }
}

//+------------------------------------------------------------------+
//| 自動補上未設定 SL/TP 的倉位                                      |
//+------------------------------------------------------------------+
void CheckAndSetSLTPIfMissing()
{
   for (int i = PositionsTotal() - 1; i >= 0; i--)
   {
      if (!PositionGetTicket(i)) continue;

      string symbol = PositionGetString(POSITION_SYMBOL);
      double sl = PositionGetDouble(POSITION_SL);
      double tp = PositionGetDouble(POSITION_TP);
      double price_open = PositionGetDouble(POSITION_PRICE_OPEN);
      double volume = PositionGetDouble(POSITION_VOLUME);
      int type = (int)PositionGetInteger(POSITION_TYPE);
      ulong ticket = PositionGetInteger(POSITION_IDENTIFIER);

      if (sl != 0 && tp != 0) continue;

      double slDiff = CalculatePriceDiff(FixedStopLossUSD, volume, symbol);
      double tpDiff = CalculatePriceDiff(FixedTakeProfitUSD, volume, symbol);

      double new_sl = 0, new_tp = 0;
      if (type == POSITION_TYPE_BUY)
      {
         new_sl = price_open - slDiff;
         new_tp = price_open + tpDiff;
      }
      else
      {
         new_sl = price_open + slDiff;
         new_tp = price_open - tpDiff;
      }

      MqlTradeRequest request;
      MqlTradeResult result;
      ZeroMemory(request);
      ZeroMemory(result);

      request.action = TRADE_ACTION_SLTP;
      request.symbol = symbol;
      request.position = ticket;
      request.sl = NormalizeDouble(new_sl, _Digits);
      request.tp = NormalizeDouble(new_tp, _Digits);
      request.magic = 12345;

      if (!OrderSend(request, result))
         PrintFormat("❌ SLTP補設失敗：%d - %s", result.retcode, result.comment);
      else
         PrintFormat("✅ 自動補上 SL/TP：%s, SL=%.5f, TP=%.5f", symbol, request.sl, request.tp);
   }
}

//+------------------------------------------------------------------+
//| 每天清晨全平倉                                                   |
//+------------------------------------------------------------------+
void CloseAllPositions()
{
   for (int i = PositionsTotal() - 1; i >= 0; i--)
   {
      if (!PositionGetTicket(i)) continue;

      string symbol = PositionGetString(POSITION_SYMBOL);
      int type = (int)PositionGetInteger(POSITION_TYPE);
      double volume = PositionGetDouble(POSITION_VOLUME);
      ulong ticket = PositionGetInteger(POSITION_IDENTIFIER);

      double price = (type == POSITION_TYPE_BUY) ? SymbolInfoDouble(symbol, SYMBOL_BID)
                                                 : SymbolInfoDouble(symbol, SYMBOL_ASK);

      MqlTradeRequest request;
      MqlTradeResult result;
      ZeroMemory(request);
      ZeroMemory(result);

      request.action = TRADE_ACTION_DEAL;
      request.symbol = symbol;
      request.position = ticket;
      request.volume = volume;
      request.type = (type == POSITION_TYPE_BUY) ? ORDER_TYPE_SELL : ORDER_TYPE_BUY;
      request.price = NormalizeDouble(price, _Digits);
      request.deviation = 10;

      if (!OrderSend(request, result))
         Print("❌ 全平倉失敗：", result.retcode, " - ", result.comment);
      else
         Print("🕓 已平倉 (每日清晨)：", symbol);
   }
}

//+------------------------------------------------------------------+
//| OnTick 主函式                                                    |
//+------------------------------------------------------------------+
void OnTick()
{
   datetime now = TimeLocal();

   // 每天 4:00 清晨自動平倉（只執行一次）
   if (TimeHour(now) == 4 && TimeDay(now) != TimeDay(g_lastAutoCloseDate))
   {
      Print("🕓 執行每日自動平倉...");
      CloseAllPositions();
      g_lastAutoCloseDate = now;
   }

   CheckAndCloseOrders();
   CheckAndSetSLTPIfMissing();
   
}
