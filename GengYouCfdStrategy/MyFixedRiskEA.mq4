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

// 全域變數：記錄上次自動平倉的日期
datetime g_lastAutoCloseDate = 0;

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

// 新增函數：CloseAllPositions
// 每天凌晨 5 點時平掉所有持倉
void CloseAllPositions()
{
   int total = OrdersTotal();
   Print("CloseAllPositions: Attempting to close all positions, count = ", total);
   // 從後往前遍歷，避免平倉時索引改變問題
   for(int i = total - 1; i >= 0; i--)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int ticket = OrderTicket();
         string symbol = OrderSymbol();
         double lots = OrderLots();
         int type = OrderType();
         double closePrice = 0.0;
         // 對於 BUY 訂單，以 Bid 價平倉；對於 SELL 訂單，以 Ask 價平倉
         if(type == OP_BUY)
            closePrice = MarketInfo(symbol, MODE_BID);
         else if(type == OP_SELL)
            closePrice = MarketInfo(symbol, MODE_ASK);
         else
            continue;
         
         PrintFormat("CloseAllPositions: Closing Ticket: %d, Symbol: %s, Price: %.5f, Lots: %.2f", 
                     ticket, symbol, closePrice, lots);
         
         if(!OrderClose(ticket, lots, closePrice, Slippage, clrRed))
            PrintFormat("CloseAllPositions: OrderClose failed for Ticket: %d, Error Code: %d", ticket, GetLastError());
         else
            PrintFormat("CloseAllPositions: Order Ticket: %d closed successfully.", ticket);
      }
      else
      {
         Print("CloseAllPositions: OrderSelect failed at index: ", i);
      }
   }
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
         
         // Check if profit/loss condition is met (stop loss = -10 USD, take profit = 20 USD)
         if(currentProfit <= -FixedStopLossUSD || currentProfit >= FixedTakeProfitUSD)
         {
            double closePrice = 0.0;
            if(type == OP_BUY)
               closePrice = MarketInfo(symbol, MODE_BID); // For BUY orders, use Bid price
            else // For SELL orders, use Ask price
               closePrice = MarketInfo(symbol, MODE_ASK);
            
            if(!OrderClose(OrderTicket(), lots, closePrice, Slippage, clrRed))
            {
               PrintFormat("OrderClose failed, Ticket: %d, Error Code: %d", OrderTicket(), GetLastError());
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
   // 新增部分：每天凌晨 5 點自動平倉 (以本地時間判斷)
   // 注意：請確保您的電腦時區設定為台灣時間
   datetime currentTime = TimeLocal();
   int currentHour = TimeHour(currentTime);
   int currentDay = TimeDay(currentTime);
   if(currentHour == 5 && TimeDay(g_lastAutoCloseDate) != currentDay)
   {
      Print("AutoClose: It is 5 AM local time. Initiating auto-close of all positions.");
      CloseAllPositions();
      g_lastAutoCloseDate = currentTime;
   }
   
   // 3. 檢查訂單是否達到固定停損/停利條件，並平倉
   CheckAndCloseOrders();
}
