//+------------------------------------------------------------------+
//|                                           GetOrderFromServer.mq4 |
//| This EA periodically calls DLL's CustomProcessParameters() to    |
//| pass required parameters, then calls ParseNewOrder() to obtain     |
//| the parsed order information, and prints the result.             |
//+------------------------------------------------------------------+
#property strict

// Polling interval (minimum 1 second)
int TimerPeriod = 1;

// Import functions from the DLL
#import "GengYouCfdStrategy.dll"
   void   StartHttpServer();
   void   StopHttpServer();
   string CustomProcessParameters(double margin, double closedPL, string commodityId, double lots, double floatingPL);
   string ParseNewOrder(); // Parse and return separated order information
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   // Start the DLL's HTTP server
   StartHttpServer();
   Print("GetOrderFromServer EA started, waiting for external order input...");

   // Set the timer to poll every second
   EventSetTimer(TimerPeriod);
   return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   EventKillTimer();
   StopHttpServer();
   Print("GetOrderFromServer EA stopped.");
}

//+------------------------------------------------------------------+
//| Called every second                                               |
//+------------------------------------------------------------------+
void OnTimer()
{
   // Retrieve required parameters
   double margin = AccountFreeMargin();
   double closedPL = 0.0; // This can be calculated based on historical data

   // If at least one open order exists, get the first order's parameters
   string commodityId = "";
   double lots = 0.0;
   double floatingPL = 0.0;
   if(OrdersTotal() > 0)
   {
      if(OrderSelect(0, SELECT_BY_POS, MODE_TRADES))
      {
         commodityId = OrderSymbol();
         lots = OrderLots();
         floatingPL = OrderProfit() + OrderSwap() + OrderCommission();
      }
   }

   // Call the DLL function to construct the JSON (for demonstration)
   string jsonResult = CustomProcessParameters(margin, closedPL, commodityId, lots, floatingPL);
   Print("CustomProcessParameters returned JSON: ", jsonResult);

   // Call the DLL function to parse and get the order information
   string parsedOrder = ParseNewOrder();
   if(parsedOrder != "{}" && StringLen(parsedOrder) > 2)
   {
      Print("Parsed order information obtained: ", parsedOrder);
      // Further process parsedOrder (e.g. parse individual fields, execute orders, etc.)
   }
}
