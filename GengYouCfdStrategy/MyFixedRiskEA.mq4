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

//------------------------------------------------------------------+
// Function: GetOrderProfit
// 取得訂單當前損益 (含盈虧、Swap 與手續費)，單位 USD
//------------------------------------------------------------------+
double GetOrderProfit(int ticket)
{
   if(OrderSelect(ticket, SELECT_BY_TICKET))
   {
      double profit = OrderProfit() + OrderSwap() + OrderCommission();
      return profit;
   }
   return 0;
}

//------------------------------------------------------------------+
// Function: CalculatePriceDiff
// 計算固定金額對應的價格差值 (以點數計)
// Price difference = Fixed amount / (lots * (tick_value / tick_size))
//------------------------------------------------------------------+
double CalculatePriceDiff(double fixedAmount, double lots)
{
   double tickValue = MarketInfo(OrderSymbol(), MODE_TICKVALUE); 
   double tickSize  = MarketInfo(OrderSymbol(), MODE_TICKSIZE);
   if(lots <= 0 || tickValue <= 0 || tickSize <= 0)
      return 0;
   return (fixedAmount * tickSize) / (lots * tickValue);
}

//------------------------------------------------------------------+
// Function: CloseAllPositions
// 每天凌晨 4:00 本地時間時平掉所有持倉
//------------------------------------------------------------------+
void CloseAllPositions()
{
   int total = OrdersTotal();
   Print("CloseAllPositions: Attempting to close all positions, count = ", total);
   // 從後往前遍歷以避免平倉時索引改變問題
   for(int i = total - 1; i >= 0; i--)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int ticket = OrderTicket();
         string symbol = OrderSymbol();
         double lots = OrderLots();
         int type = OrderType();
         double closePrice = 0.0;
         // 對於 BUY 訂單，採用 Bid 價平倉；對於 SELL 訂單，採用 Ask 價平倉
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

//------------------------------------------------------------------+
// Function: CheckAndCloseOrders
// 檢查訂單是否達到固定停損/停利條件，並平倉
//------------------------------------------------------------------+
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
         
         // 輸出訂單基本資訊
         PrintFormat("Order Ticket: %d, Symbol: %s, Lots: %.2f, Current Profit/Loss: %.2f USD", 
                     OrderTicket(), symbol, lots, currentProfit);
         
         // 如果損益達到固定值則平倉 (止損 = -10 USD, 止盈 = 20 USD)
         if(currentProfit <= -FixedStopLossUSD || currentProfit >= FixedTakeProfitUSD)
         {
            double closePrice = 0.0;
            if(type == OP_BUY)
               closePrice = MarketInfo(symbol, MODE_BID); // BUY 訂單以 Bid 價平倉
            else // SELL 訂單以 Ask 價平倉
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

//------------------------------------------------------------------+
//| Expert tick function                                             |
//------------------------------------------------------------------+
void OnTick()
{
   // 自動平倉功能：每天凌晨 4:00（本地時間）自動平倉所有持倉
   // 使用 TimeLocal() 取得本地時間 (請確保電腦時區設定正確)
   datetime currentTime = TimeLocal();
   int currentHour = TimeHour(currentTime);
   int currentDay = TimeDay(currentTime);
   // 如果當前時間為 4:00 且今日尚未執行過自動平倉
   if(currentHour == 4 && TimeDay(g_lastAutoCloseDate) != currentDay)
   {
      Print("AutoClose: It is 4 AM local time. Initiating auto-close of all positions.");
      CloseAllPositions();
      g_lastAutoCloseDate = currentTime;
   }
   
   // 原有的訂單檢查邏輯
   CheckAndCloseOrders();
}
