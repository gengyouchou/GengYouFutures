//+------------------------------------------------------------------+
//|                                               MyFixedRiskEA.mq4  |
//| 示例：固定金额止损止盈 (10 美元止损, 20 美元止盈)                 |
//+------------------------------------------------------------------+
#property strict

// 输入参数：固定止损与止盈金额，单位：美元
extern double FixedStopLossUSD = 10.0;
extern double FixedTakeProfitUSD = 20.0;

// 计算订单的当前盈亏（单位：美元）
double GetOrderProfit(int ticket)
{
   if (OrderSelect(ticket, SELECT_BY_TICKET))
   {
      double profit = OrderProfit() + OrderSwap() + OrderCommission();
      return profit;
   }
   return 0;
}

// 计算价格差距的函数：
// 公式：价格差 = 固定金额 / (手数 * (tick_value / tick_size))
// tick_value = 每手每tick的价值，tick_size = 每个tick的大小
double CalculatePriceDiff(double fixedAmount, double lots)
{
   double tickValue = MarketInfo(Symbol(), MODE_TICKVALUE); // 每手的tick价值（USD）
   double tickSize = MarketInfo(Symbol(), MODE_TICKSIZE);   // tick大小
   if (lots <= 0 || tickValue <= 0 || tickSize <= 0)
      return 0;
   return (fixedAmount * tickSize) / (lots * tickValue);
}

// 检查并手动平仓达到止损或止盈条件的订单，同时打印订单信息
void CheckAndCloseOrders()
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
         double currentProfit = GetOrderProfit(OrderTicket());

         // 计算固定金额对应的价格差
         double riskPrice = CalculatePriceDiff(FixedStopLossUSD, lots);
         double rewardPrice = CalculatePriceDiff(FixedTakeProfitUSD, lots);

         double desiredSL, desiredTP;
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

         // 打印订单信息
         PrintFormat("订单票号: %d, 手数: %.2f, 当前盈亏: %.2f USD, 预设止损: %.2f USD, 预设止盈: %.2f USD",
                     OrderTicket(), lots, currentProfit, FixedStopLossUSD, FixedTakeProfitUSD);

         // 检查是否达到止损或止盈条件
         if (currentProfit <= -FixedStopLossUSD || currentProfit >= FixedTakeProfitUSD)
         {
            // 平仓订单
            if (!OrderClose(OrderTicket(), OrderLots(), OrderClosePrice(), 3, clrRed))
            {
               Print("OrderClose 失败，票号 ", OrderTicket(), " 错误码：", GetLastError());
            }
            else
            {
               Print("订单票号 ", OrderTicket(), " 已平仓，盈亏=", currentProfit);
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
   CheckAndCloseOrders();
}
