//+------------------------------------------------------------------+
//|                                               MyFixedRiskEA.mq4  |
//| Example: Fixed USD stop loss (10 USD) and take profit (20 USD)     |
//|         using market order to close positions                    |
//+------------------------------------------------------------------+
#property strict

// Input parameters: fixed stop loss and take profit amounts in USD
extern double FixedStopLossUSD = 10.0;
extern double FixedTakeProfitUSD = 20.0;
extern int Slippage = 3;

// Function to calculate the current profit/loss of an order (in USD)
double GetOrderProfit(int ticket)
{
   if(OrderSelect(ticket, SELECT_BY_TICKET))
   {
      double profit = OrderProfit() + OrderSwap() + OrderCommission();
      return profit;
   }
   return 0;
}

// Function to calculate the price difference (in price units):
// Price difference = Fixed amount / (lots * (tick_value / tick_size))
// tick_value: value per tick per lot (in USD)
// tick_size: size of one tick
double CalculatePriceDiff(double fixedAmount, double lots)
{
   double tickValue = MarketInfo(OrderSymbol(), MODE_TICKVALUE); 
   double tickSize  = MarketInfo(OrderSymbol(), MODE_TICKSIZE);
   if(lots <= 0 || tickValue <= 0 || tickSize <= 0)
      return 0;
   return (fixedAmount * tickSize) / (lots * tickValue);
}

// Function to check orders, print details, and send a market close order if conditions are met.
void CheckAndCloseOrders()
{
   for(int i = OrdersTotal()-1; i >= 0; i--)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int type = OrderType();
         if(type != OP_BUY && type != OP_SELL)
            continue;
         
         string symbol = OrderSymbol();
         double lots = OrderLots();
         double currentProfit = GetOrderProfit(OrderTicket());
         
         // Print order basic information
         PrintFormat("Order Ticket: %d, Symbol: %s, Lots: %.2f, Current Profit/Loss: %.2f USD", 
                     OrderTicket(), symbol, lots, currentProfit);
         
         // Check if profit/loss condition is met (fixed amounts: stop loss = -10 USD, take profit = 20 USD)
         if(currentProfit <= -FixedStopLossUSD || currentProfit >= FixedTakeProfitUSD)
         {
            double closePrice;
            // Use market price to close the order
            if(type == OP_BUY)
            {
               closePrice = MarketInfo(symbol, MODE_BID); // For BUY orders, use Bid price
            }
            else  // For SELL orders, use Ask price
            {
               closePrice = MarketInfo(symbol, MODE_ASK);
            }
            
            if(!OrderClose(OrderTicket(), lots, closePrice, Slippage, clrRed))
            {
               PrintFormat("Market OrderClose failed, Ticket: %d, Error Code: %d", OrderTicket(), GetLastError());
            }
            else
            {
               PrintFormat("Order Ticket: %d has been closed at market price %.5f. Profit/Loss: %.2f USD", 
                           OrderTicket(), closePrice, currentProfit);
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
