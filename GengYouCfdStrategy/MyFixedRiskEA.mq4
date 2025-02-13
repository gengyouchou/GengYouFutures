//+------------------------------------------------------------------+
//|                                               MyFixedRiskEA.mq4  |
//| 示例：固定金額停損停利 (10 美金停損, 20 美金停利)                   |
//+------------------------------------------------------------------+
#property strict

// 輸入參數：固定停損與停利金額，單位：美金
extern double FixedStopLossUSD = 10.0;
extern double FixedTakeProfitUSD = 20.0;
extern int Slippage = 3;
extern color OrderColor = clrBlue;

// 計算價格差距的函數：
// 公式：價格差 = 固定金額 / (手數 * (tick_value / tick_size))
// tick_value = 每手每 tick 的價值，tick_size = 每個 tick 的大小
double CalculatePriceDiff(double fixedAmount, double lots)
{
   double tickValue = MarketInfo(Symbol(), MODE_TICKVALUE); // 每手的 tick 價值 (USD)
   double tickSize = MarketInfo(Symbol(), MODE_TICKSIZE);   // tick 大小
   if (lots <= 0 || tickValue <= 0 || tickSize <= 0)
      return 0;
   return (fixedAmount * tickSize) / (lots * tickValue);
}

// 修改訂單停損、停利的邏輯
void UpdateOrderSLTP()
{
   for (int i = OrdersTotal() - 1; i >= 0; i--)
   {
      if (OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int type = OrderType();
         if (type != OP_BUY && type != OP_SELL)
            continue;

         double entryPrice = OrderOpenPrice();
         double lots = OrderLots();
         double desiredSL, desiredTP;

         // 計算固定金額對應的價格差
         double riskPrice = CalculatePriceDiff(FixedStopLossUSD, lots);
         double rewardPrice = CalculatePriceDiff(FixedTakeProfitUSD, lots);

         if (type == OP_BUY)
         {
            desiredSL = entryPrice - riskPrice;
            desiredTP = entryPrice + rewardPrice;
         }
         else if (type == OP_SELL)
         {
            desiredSL = entryPrice + riskPrice;
            desiredTP = entryPrice - rewardPrice;
         }

         // 取得目前停損與停利價格
         double currentSL = OrderStopLoss();
         double currentTP = OrderTakeProfit();
         bool needModify = false;
         if (MathAbs(currentSL - desiredSL) > 0.00001)
            needModify = true;
         if (MathAbs(currentTP - desiredTP) > 0.00001)
            needModify = true;

         if (needModify)
         {
            if (!OrderModify(OrderTicket(), entryPrice, desiredSL, desiredTP, 0, OrderColor))
            {
               Print("OrderModify 失敗，票號 ", OrderTicket(), " 錯誤碼：", GetLastError());
            }
            else
            {
               Print("訂單票號 ", OrderTicket(), " 已更新：新停損=", desiredSL, " 新停利=", desiredTP);
            }
         }
      }
   }
}

//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   UpdateOrderSLTP();
}
