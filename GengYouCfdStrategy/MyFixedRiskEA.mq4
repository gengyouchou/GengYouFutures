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

// 全域變數：記錄上次自動平倉的日期（以天為單位）
datetime g_lastAutoCloseDate = 0;

//------------------------------------------------------------------
// Function: GetOrderProfit
// 取得訂單當前損益 (含盈虧、Swap 與手續費)，單位 USD
//------------------------------------------------------------------
double GetOrderProfit(int ticket)
{
   if(OrderSelect(ticket, SELECT_BY_TICKET))
   {
      double profit = OrderProfit() + OrderSwap() + OrderCommission();
      return profit;
   }
   return 0;
}

//------------------------------------------------------------------
// Function: CalculatePriceDiff
// 計算固定金額對應的價格差值 (以點數計)
// Price difference = Fixed amount / (lots * (tick_value / tick_size))
//------------------------------------------------------------------
double CalculatePriceDiff(double fixedAmount, double lots)
{
   double tickValue = MarketInfo(OrderSymbol(), MODE_TICKVALUE); 
   double tickSize  = MarketInfo(OrderSymbol(), MODE_TICKSIZE);
   if(lots <= 0 || tickValue <= 0 || tickSize <= 0)
      return 0;
   return (fixedAmount * tickSize) / (lots * tickValue);
}

//------------------------------------------------------------------
// 新增函數: CloseAllPositions
// 每天凌晨 5 點時平掉所有持倉
//------------------------------------------------------------------
void CloseAllPositions()
{
   int total = OrdersTotal();
   Print("[CloseAllPositions] AutoClose: Attempting to close all positions, count = ", total);
   // 從後往前遍歷，避免平倉時 index 變動問題
   for(int i = total - 1; i >= 0; i--)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         int ticket = OrderTicket();
         string symbol = OrderSymbol();
         double lots = OrderLots();
         int type = OrderType();
         double closePrice = 0.0;
         // 根據訂單類型決定平倉價格：買單以 BID 平倉、賣單以 ASK 平倉
         if(type == OP_BUY)
            closePrice = MarketInfo(symbol, MODE_BID);
         else if(type == OP_SELL)
            closePrice = MarketInfo(symbol, MODE_ASK);
         else
            continue;
         
         PrintFormat("[CloseAllPositions] Closing Ticket: %d, Symbol: %s, Price: %.5f, Lots: %.2f", 
                     ticket, symbol, closePrice, lots);
         
         if(OrderClose(ticket, lots, closePrice, Slippage, clrRed))
            PrintFormat("[CloseAllPositions] Order %d closed successfully.", ticket);
         else
            PrintFormat("[CloseAllPositions] Order %d close failed. Error: %d", ticket, GetLastError());
      }
      else
      {
         Print("[CloseAllPositions] OrderSelect failed at index: ", i);
      }
   }
}

//------------------------------------------------------------------
// Function: CheckAndCloseOrders
// 檢查訂單，若損益達到固定 stop loss 或 take profit 則以市價平倉
//------------------------------------------------------------------
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
         
         // 若損益條件達到，則平倉 (固定 stop loss = -10 USD, take profit = 20 USD)
         if(currentProfit <= -FixedStopLossUSD || currentProfit >= FixedTakeProfitUSD)
         {
            double closePrice;
            // 使用市價平倉：買單以 BID 平倉、賣單以 ASK 平倉
            if(type == OP_BUY)
               closePrice = MarketInfo(symbol, MODE_BID);
            else // OP_SELL
               closePrice = MarketInfo(symbol, MODE_ASK);
            
            if(!OrderClose(OrderTicket(), lots, closePrice, Slippage, clrRed))
            {
               PrintFormat("Market OrderClose failed, Ticket: %d, Error Code: %d", OrderTicket(), GetLastError());
            }
            else
            {
               PrintFormat("Order Ticket: %d closed at market price %.5f. Profit/Loss: %.2f USD", 
                           OrderTicket(), closePrice, currentProfit);
            }
         }
      }
   }
}

//------------------------------------------------------------------
// Expert Tick function
//------------------------------------------------------------------
void OnTick()
{
   // 新增：每天凌晨 5 點自動平倉
   datetime currentTime = TimeCurrent();
   int currentHour = TimeHour(currentTime);
   int currentDay = TimeDay(currentTime);
   // 若當前小時為 5 且今日尚未執行過自動平倉
   if(currentHour == 5 && TimeDay(g_lastAutoCloseDate) != currentDay)
   {
      Print("[OnTick] It is 5 AM. Initiating auto-close of all positions.");
      CloseAllPositions();
      g_lastAutoCloseDate = currentTime;
   }
   
   // 3. 檢查訂單並根據固定條件平倉
   CheckAndCloseOrders();
}
