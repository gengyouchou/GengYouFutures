#property strict

// 定義與 C++ DLL 相容的訂單結構
struct SIMULATED_POSITION
{
   ulong   OrderSerialNumber;  // 訂單號（64 位整數）
   double  CostPrice;          // 開倉價格
   double  Lots;               // 口數
   double  FloatingPL;         // 浮動盈虧
};

// 導入 DLL 中的函數
#import "GengYouCfdStrategy.dll"
   void StartHttpServer();
   void StopHttpServer();
   // 傳入單筆訂單資料，將其存入 DLL 的全局變量
   void GetCurOpenPosition(string commodityId, SIMULATED_POSITION Position);
   // 處理所有接收到的未平倉資料，計算停損、停利、加碼邏輯後返回下單 JSON 字串
   string ProcessSimulatedPositions();
#import

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
{
   // 啟動 DLL 的 HTTP 服務器（同時 DLL 會開啟 CMD 視窗輸出 log）
   StartHttpServer();
   Print("EA initialized. DLL HTTP server started. Sending open positions on every tick.");
   return(INIT_SUCCEEDED);
}

//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
{
   StopHttpServer();
   Print("EA deinitialized. DLL HTTP server stopped.");
}

//+------------------------------------------------------------------+
//| Expert Tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
{
   // 遍歷所有開倉訂單，將每筆資料傳給 DLL
   int total = OrdersTotal();
   for(int i = 0; i < total; i++)
   {
      if(OrderSelect(i, SELECT_BY_POS, MODE_TRADES))
      {
         SIMULATED_POSITION pos;
         // 以 OrderTicket 作為訂單號 (MQ4 的 OrderTicket 為 int，但視為 ulong 處理)
         pos.OrderSerialNumber = OrderTicket();
         pos.CostPrice = OrderOpenPrice();
         pos.Lots = OrderLots();
         pos.FloatingPL = OrderProfit() + OrderSwap() + OrderCommission();
         
         // 取得商品代號
         string sym = OrderSymbol();
         // 傳入 DLL
         GetCurOpenPosition(sym, pos);
      }
   }
   
   // 每次 Tick 呼叫 ProcessSimulatedPositions() 來計算停損/停利邏輯，
   // 並取得下單 JSON（若有有效下單訊號，返回的 JSON 字串長度會大於 2）
   string ordersJson = ProcessSimulatedPositions();
   if(StringLen(ordersJson) > 2)
   {
      Print("DLL orders: ", ordersJson);
   }
}
