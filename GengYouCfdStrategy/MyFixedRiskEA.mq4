//+------------------------------------------------------------------+
//|                                               MyFixedRiskEA.mq4  |
//| Example: Fixed amount stop loss and take profit (10 USD stop loss, 20 USD take profit) |
//+------------------------------------------------------------------+
#property strict

// Input parameters: Fixed stop loss and take profit amounts in USD
extern double FixedStopLossUSD = 10.0;
extern double FixedTakeProfitUSD = 20.0;

// Function to calculate the current profit/loss of an order (in USD)
double GetOrderProfit(int ticket)
{
   if (OrderSelect(ticket, SELECT_BY_TICKET))
   {
      double profit = OrderProfit() + OrderSwap() + OrderCommission();
      return profit;
   }
   return 0;
}

// Function to calculate the price difference:
// Formula: Price difference = Fixed amount / (Lots * (tick_value / tick_size))
// tick_value = Value per tick per lot, tick_size = Size of one tick
double CalculatePriceDiff(double fixedAmount, double lots)
{
   double tickValue = MarketInfo(OrderSymbol(), MODE_TICKVALUE); // Tick value per lot (USD)
   double tickSize = MarketInfo(OrderSymbol(), MODE_TICKSIZE);   // Tick size
   if (lots <= 0 || tickValue <= 0 || tickSize <= 0)
      return 0;
   return (fixedAmount * tickSize) / (lots * tickValue);
}

// Function to check and manually close orders that reach stop loss or take profit conditions,
// while printing order information
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
         string symbol = OrderSymbol();

         // Calculate the price difference corresponding to the fixed amount
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

         // Print order information in English
         PrintFormat("Order Ticket: %d, Symbol: %s, Lots: %.2f, Current Profit/Loss: %.2f USD, Set Stop Loss: %.2f USD, Set Take Profit: %.2f USD",
                     OrderTicket(), symbol, lots, currentProfit, FixedStopLossUSD, FixedTakeProfitUSD);

         // Check if stop loss or take profit conditions are met
         if (currentProfit <= -FixedStopLossUSD || currentProfit >= FixedTakeProfitUSD)
         {
            // Close the order
            if (!OrderClose(OrderTicket(), OrderLots(), OrderClosePrice(), 3, clrRed))
            {
               PrintFormat("OrderClose failed, Ticket: %d, Error Code: %d", OrderTicket(), GetLastError());
            }
            else
            {
               PrintFormat("Order Ticket: %d has been closed, Profit/Loss: %.2f USD", OrderTicket(), currentProfit);
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
