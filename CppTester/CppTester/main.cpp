#include "SKCenterLib.h"
#include "SKOrderLib.h"
#include "SKQuoteLib.h"
#include "SKReplyLib.h"
#include <Logger.h>
#include <array>
#include <chrono>  // For std::chrono::steady_clock
#include <conio.h> // For kbhit() and _getch()
#include <cstdlib> // For system("cls")
#include <deque>
#include <iostream>
#include <thread> // For std::this_thread::sleep_for
#include <unordered_map>

// Define the global logger instance
Logger logger("debug.log");

CSKCenterLib *pSKCenterLib;
CSKQuoteLib *pSKQuoteLib;
CSKReplyLib *pSKReplyLib;
CSKOrderLib *pSKOrderLib;

long g_nCode = 0;
string g_strUserId;

void AutoLogIn()
{
	DEBUG(DEBUG_LEVEL_DEBUG, "Started");

	// 初始化
	g_nCode = pSKOrderLib->Initialize();
	pSKCenterLib->PrintfCodeMessage("AutoLogIn", "Initialize", g_nCode);

	// 讀取憑證
	g_nCode = pSKOrderLib->ReadCertByID(g_strUserId);
	pSKCenterLib->PrintfCodeMessage("AutoLogIn", "ReadCertByID", g_nCode);

	// 取得帳號
	g_nCode = pSKOrderLib->GetUserAccount();
	pSKCenterLib->PrintfCodeMessage("AutoLogIn", "GetUserAccount", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoOrderMTX()
{
	DEBUG(DEBUG_LEVEL_DEBUG, "Started");

	g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId, false, "MTX00", 2, 1, 0, 2, "P", 1, 0);
	pSKCenterLib->PrintfCodeMessage("AutoOrderMTX", "SendFutureOrder", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "SendFutureOrder res = %d", g_nCode);

	g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
										   false,
										   "MTX00",
										   2,
										   0, // buy
										   0,
										   2,
										   "P",
										   1,
										   0);

	pSKCenterLib->PrintfCodeMessage("AutoOrderMTX", "SendFutureOrder", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "SendFutureOrder res = %d", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoGetFutureRights()
{
	DEBUG(DEBUG_LEVEL_DEBUG, "Started");

	g_nCode = pSKOrderLib->GetFutureRights(g_strUserId);

	pSKCenterLib->PrintfCodeMessage("AutoGetFutureRights", "GetFutureRights", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "GetFutureRights res = %d", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoQuote(IN string ProductNum, short sPageNo)
{
	DEBUG(DEBUG_LEVEL_DEBUG, "Started");

	while (pSKQuoteLib->IsConnected() != 1)
	{
		g_nCode = pSKQuoteLib->EnterMonitorLONG();
		pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
	}

	g_nCode = pSKQuoteLib->RequestStocks(&sPageNo, ProductNum);
	pSKCenterLib->PrintfCodeMessage("Quote", "RequestStocks", g_nCode);
	DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode= %d", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoQuoteTicks(IN string ProductNum, short sPageNo)
{
	DEBUG(DEBUG_LEVEL_DEBUG, "Started");

	while (pSKQuoteLib->IsConnected() != 1)
	{
		g_nCode = pSKQuoteLib->EnterMonitorLONG();
		pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
	}

	g_nCode = pSKQuoteLib->RequestTicks(&sPageNo, ProductNum);

	pSKCenterLib->PrintfCodeMessage("Quote", "RequestTicks", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode= %d", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoKLineData(IN string ProductNum)
{
	DEBUG(DEBUG_LEVEL_DEBUG, "Started");

	while (pSKQuoteLib->IsConnected() != 1)
	{
		g_nCode = pSKQuoteLib->EnterMonitorLONG();
		pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
	}

	g_nCode = pSKQuoteLib->RequestKLine(ProductNum);

	pSKCenterLib->PrintfCodeMessage("Quote", "RequestKLine", g_nCode);

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void Order()
{
	bool bOrder = true;
	string strStockNo = "", strPrice = "", strNo = "";
	short sPrime = 0, sPeriod = 0, sFlag = 0, sBuySell = 0, sTradeType = 0, sDayTrade = 0, sNewClose = 0, sReserved = 0;
	int nOrderType = 0, nMarket = 0, nType = 0;
	long nQty = 0, nTradeType = 0, nSpecialTradeType = 0;

	// 初始化

	g_nCode = pSKOrderLib->Initialize();
	pSKCenterLib->PrintfCodeMessage("Order", "Initialize", g_nCode);

	// 讀取憑證
	g_nCode = pSKOrderLib->ReadCertByID(g_strUserId);
	pSKCenterLib->PrintfCodeMessage("Order", "ReadCertByID", g_nCode);

	// 取得帳號
	g_nCode = pSKOrderLib->GetUserAccount();
	pSKCenterLib->PrintfCodeMessage("Order", "GetUserAccount", g_nCode);

	while (bOrder)
	{
		cout << endl
			 << "請選擇功能 (1：下單, 2：刪單, 3：改價, 4：改量, -1：退出)：";
		cin >> nOrderType;
		switch (nOrderType)
		{
		case 1:
			cout << "請選擇市場 (0：證券, 1：期貨, 2：選擇權, -1：退出)：";
			cin >> nOrderType;
			switch (nOrderType)
			{
			case 0:
				cout << "請輸入商品代碼：";
				cin >> strStockNo;
				cout << "請輸入公司種類 (0：上市櫃, 1：興櫃)：";
				cin >> sPrime;
				cout << "請輸入下單時段 (0：盤中, 1：盤後, 2：盤後零股)：";
				cin >> sPeriod;
				cout << "請輸入股票種類 (0：現股, 1：融資, 2：融券)：";
				cin >> sFlag;
				cout << "請輸入買賣別 (0：買, 1：賣)：";
				cin >> sBuySell;
				cout << "請輸入價格：";
				cin >> strPrice;
				cout << "請輸入數量：";
				cin >> nQty;
				cout << "請輸入ROD/IOC/FOK (0：ROD, 1：IOC, 2：FOK)：";
				cin >> nTradeType;
				cout << "請輸入限市價 (1：市價, 2：限價)：";
				cin >> nSpecialTradeType;

				g_nCode = pSKOrderLib->SendStockOrder(g_strUserId, false, strStockNo, sPrime, sPeriod, sFlag, sBuySell, strPrice, nQty, nTradeType, nSpecialTradeType);

				pSKCenterLib->PrintfCodeMessage("Order", "SendStockOrder", g_nCode);

				break;
			case 1:
			case 2:
				cout << "請輸入商品代碼：";
				cin >> strStockNo;
				cout << "請輸入ROD/IOC/FOK (0：ROD, 1：IOC, 2：FOK)：";
				cin >> sTradeType;
				cout << "請輸入買賣別 (0：買, 1：賣)：";
				cin >> sBuySell;
				cout << "請輸入是否當沖 (0：否, 1：是)：";
				cin >> sDayTrade;
				cout << "請輸入新平倉 (0：新倉, 1：平倉, 2：自動)：";
				cin >> sNewClose;
				cout << "請輸入價格：";
				cin >> strPrice;
				cout << "請輸入數量：";
				cin >> nQty;
				cout << "請輸入盤別 (0：盤中, 1：T盤預約)：";
				cin >> sReserved;

				if (nOrderType == 1)
				{
					g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId, false, strStockNo, sTradeType, sBuySell, sDayTrade, sNewClose, strPrice, nQty, sReserved);
					pSKCenterLib->PrintfCodeMessage("Order", "SendFutureOrder", g_nCode);
				}
				else if (nOrderType == 2)
				{
					g_nCode = pSKOrderLib->SendOptionOrder(g_strUserId, false, strStockNo, sTradeType, sBuySell, sDayTrade, sNewClose, strPrice, nQty, sReserved);
					pSKCenterLib->PrintfCodeMessage("Order", "SendOptionOrder", g_nCode);
				}

				break;
			case -1:
				break;
			default:
				cout << "輸入代碼錯誤，請重新輸入" << endl;
				break;
			}
			break;
		case 2:
			cout << "請輸入市場別 (0：證券, 1：期貨, 2：選擇權)：";
			cin >> nMarket;
			cout << "請輸入刪單類別 (0：序號刪單, 1：書號刪單, 2：商品代號刪單)：";
			cin >> nType;
			cout << "請輸入刪單碼 (序號、書號、代號)：";
			cin >> strNo;

			g_nCode = pSKOrderLib->CancelOrder(g_strUserId, false, nMarket, nType, strNo);

			pSKCenterLib->PrintfCodeMessage("Order", "CancelOrder", g_nCode);

			break;
		case 3:
			cout << "請輸入市場別 (0：證券, 1：期貨, 2：選擇權)：";
			cin >> nMarket;
			cout << "請輸入改價類別 (0：序號改價, 1：書號改價)：";
			cin >> nType;
			cout << "請輸入改價碼 (序號、書號)：";
			cin >> strNo;
			cout << "請輸入改價價格：";
			cin >> strPrice;
			cout << "請輸入ROD/IOC/FOK (0：ROD, 1：IOC, 2：FOK)：";
			cin >> nTradeType;
			g_nCode = pSKOrderLib->CorrectPrice(g_strUserId, false, nMarket, nType, strNo, strPrice, nTradeType);

			pSKCenterLib->PrintfCodeMessage("Order", "CorrectPrice", g_nCode);

			break;
		case 4:
			cout << "請輸入市場別 (0：證券, 1：期貨, 2：選擇權)：";
			cin >> nMarket;
			cout << "請輸入改量碼 (序號)：";
			cin >> strNo;
			cout << "請輸入減量數量：";
			cin >> nQty;

			g_nCode = pSKOrderLib->DecreaseOrder(g_strUserId, false, nMarket, strNo, nQty);

			pSKCenterLib->PrintfCodeMessage("Order", "CorrectPrice", g_nCode);
			break;
		case -1:
			bOrder = false;
			break;
		default:
			cout << "輸入代碼錯誤，請重新輸入" << endl;
			break;
		}
	}
}

void Reply()
{
	bool bReply = true;
	int nReplyType = 0;

	while (bReply)
	{
		cout << endl
			 << "請選擇功能 (1：回報連線, 2：連線狀態, 3：回報斷線, -1：退出)：";
		cin >> nReplyType;
		switch (nReplyType)
		{
		case 1:
			g_nCode = pSKReplyLib->SKReplyLib_ConnectByID(g_strUserId);

			pSKCenterLib->PrintfCodeMessage("Reply", "SKReplyLib_ConnectByID", g_nCode);
			break;
		case 2:
			g_nCode = pSKReplyLib->SKReplyLib_IsConnectedByID(g_strUserId);

			cout << "SKReplyLib_IsConnectedByID (0：未連線, 1：連線, 2：下載中)：" << g_nCode << endl;
			break;
		case 3:
			g_nCode = pSKReplyLib->SKReplyLib_SolaceCloseByID(g_strUserId);

			pSKCenterLib->PrintfCodeMessage("Reply", "SKReplyLib_SolaceCloseByID", g_nCode);
			break;
		case -1:
			bReply = false;
			break;
		default:
			cout << "輸入代碼錯誤，請重新輸入" << endl;
			break;
		}
	}
}

void Quote()
{
	DEBUG(DEBUG_LEVEL_DEBUG, "start");
	bool bQuote = true;
	int QuoteType = 0;
	short sPageNo = -1, sMarket = 0;
	string strStockNo = "";

	// 報價功能
	while (bQuote)
	{
		// 選擇功能
		cout << endl
			 << "輸入代號(1：報價連線, 2：連線狀態, 3：報價斷線, 4：Quote, 5：Ticks & Best5, 6：StockList, -1：離開報價功能) ：";

		cin >> QuoteType;
		switch (QuoteType)
		{
		case 1:
			g_nCode = pSKQuoteLib->EnterMonitorLONG();

			pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
			break;
		case 2:
			g_nCode = pSKQuoteLib->IsConnected();

			cout << "SKQuoteLib_IsConnected (0：未連線, 1：連線, 2：下載中)：" << g_nCode << endl;
			break;
		case 3:
			g_nCode = pSKQuoteLib->LeaveMonitor();

			pSKCenterLib->PrintfCodeMessage("Quote", "LeaveMonitor", g_nCode);
			break;
		case 4:
			if (pSKQuoteLib->IsConnected() != 1)
			{
				cout << "尚未連線" << endl;
				break;
			}
			cout << "請輸入商品代碼 (多筆以,分隔)：";
			cin >> strStockNo;

			sPageNo = 1;
			g_nCode = pSKQuoteLib->RequestStocks(&sPageNo, strStockNo);

			pSKCenterLib->PrintfCodeMessage("Quote", "RequestStocks", g_nCode);
			break;
		case 5:
			if (pSKQuoteLib->IsConnected() != 1)
			{
				cout << "尚未連線" << endl;
				break;
			}
			cout << "請輸入商品代碼 (限單筆)：";
			cin >> strStockNo;

			sPageNo = -1;
			g_nCode = pSKQuoteLib->RequestTicks(&sPageNo, strStockNo);

			pSKCenterLib->PrintfCodeMessage("Quote", "RequestTicks", g_nCode);
			break;
		case 6:
			if (pSKQuoteLib->IsConnected() != 1)
			{
				cout << "尚未連線" << endl;
				break;
			}
			cout << "請輸入市場代碼 (0：上市, 1：上櫃, 2：期貨, 3：選擇權, 4：興櫃, 5：上市盤中零股, 6：上櫃盤中零股)：";
			cin >> sMarket;

			g_nCode = pSKQuoteLib->RequestStockList(sMarket);

			pSKCenterLib->PrintfCodeMessage("Quote", "RequestStockList", g_nCode);
			break;
		case -1:
			bQuote = false;
			break;
		default:
			cout << "輸入代碼錯誤，請重新輸入" << endl;
			break;
		}
	}

	DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void init()
{
	pSKCenterLib = new CSKCenterLib;
	pSKQuoteLib = new CSKQuoteLib;
	pSKReplyLib = new CSKReplyLib;
	pSKOrderLib = new CSKOrderLib;
}

void release()
{
	delete pSKCenterLib;
	delete pSKQuoteLib;
	delete pSKReplyLib;
	delete pSKOrderLib;

	CoUninitialize();
}

extern std::deque<long> gDaysKlineDiff;
extern bool gEatOffer;
extern std::unordered_map<long, std::array<long, 2>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];

void thread_main()
{
	AutoLogIn();
	// AutoOrderMTX();
	// AutoGetFutureRights();

	// g_nCode = pSKQuoteLib->LeaveMonitor();

	// pSKCenterLib->PrintfCodeMessage("Quote", "LeaveMonitor", g_nCode);

	int x = 1;

	// hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	AutoKLineData("MTX00");

	long long accu = 0;
	long AvgAmp = 0, LargestAmp = LONG_MIN, SmallestAmp = LONG_MAX, LargerAmp = 0, SmallAmp = 0;

	for (int i = 0; i < gDaysKlineDiff.size(); ++i)
	{
		DEBUG(DEBUG_LEVEL_INFO, "Diff = %ld ", gDaysKlineDiff[i]);

		accu += gDaysKlineDiff[i];

		LargestAmp = max(LargestAmp, gDaysKlineDiff[i]);
		SmallestAmp = min(SmallestAmp, gDaysKlineDiff[i]);
	}

	AvgAmp = accu / DayMA;

	LargerAmp = (AvgAmp + LargestAmp) / 2;
	SmallAmp = (AvgAmp + SmallestAmp) / 2;

	DEBUG(DEBUG_LEVEL_INFO, "SmallestAmp : %ld", SmallestAmp);
	DEBUG(DEBUG_LEVEL_INFO, "SmallAmp : %ld", SmallAmp);
	DEBUG(DEBUG_LEVEL_INFO, "AvgAmp : %ld", AvgAmp);
	DEBUG(DEBUG_LEVEL_INFO, "LargerAmp : %ld", LargerAmp);
	DEBUG(DEBUG_LEVEL_INFO, "LargestAmp : %ld", LargestAmp);

	AutoQuoteTicks("2330", 1);

	AutoQuoteTicks("MTX00", 2);

	// current time
	// Estimated trading volume
	// Instant profit and loss

	// cin >> x;

	long res = pSKQuoteLib->RequestServerTime();

	DEBUG(DEBUG_LEVEL_DEBUG, "pSKQuoteLib->RequestServerTime()=%d", res);

	SKCOMLib::SKSTOCKLONG skStock;

	res = pSKQuoteLib->RequestStockIndexMap("MTX00", &skStock);

	DEBUG(DEBUG_LEVEL_DEBUG, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

	long MTXIdxNo = skStock.nStockIdx;

	res = pSKQuoteLib->RequestStockIndexMap("2330", &skStock);

	DEBUG(DEBUG_LEVEL_DEBUG, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

	res = pSKQuoteLib->RequestStockIndexMap("2317", &skStock);

	DEBUG(DEBUG_LEVEL_DEBUG, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

	res = pSKQuoteLib->RequestStockIndexMap("2454", &skStock);

	DEBUG(DEBUG_LEVEL_DEBUG, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

	// 设置定期清屏的时间间隔（以毫秒为单位）
	const int refreshInterval = 1000; // 1000毫秒
	auto lastClearTime = std::chrono::steady_clock::now();

	while (true)
	{
		// 获取当前时间
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClearTime);

		// 检查是否需要清屏
		if (elapsed.count() >= refreshInterval)
		{
			// 清屏
			system("cls");

			// 更新最后清屏时间
			lastClearTime = now;

			if (gCurCommHighLowPoint.count(MTXIdxNo) > 0)
			{
				long CurHigh = gCurCommHighLowPoint[MTXIdxNo][0] / 100;
				long CurLow = gCurCommHighLowPoint[MTXIdxNo][1] / 100;

				DEBUG(DEBUG_LEVEL_INFO, "MTXIdxNo: %ld. High: %ld, Low: %ld", MTXIdxNo, CurHigh, CurLow);

				printf("Long Key 5: %ld\n", CurLow + LargestAmp);
				printf("Long Key 4: %ld\n", CurLow + LargerAmp);
				printf("Long Key 3: %ld\n", CurLow + AvgAmp);
				printf("Long Key 2: %ld\n", CurLow + SmallAmp);
				printf("Long Key 1: %ld\n", CurLow + SmallestAmp);
				printf("=========================================\n");
				printf("Short Key 1: %ld\n", CurHigh - SmallestAmp);
				printf("Short Key 2: %ld\n", CurHigh - SmallAmp);
				printf("Short Key 3: %ld\n", CurHigh - AvgAmp);
				printf("Short Key 4: %ld\n", CurHigh - LargerAmp);
				printf("Short Key 5: %ld\n", CurHigh - LargestAmp);

				printf("=========================================\n");

				printf("SmallestAmp : %ld\n", SmallestAmp);
				printf("SmallAmp : %ld\n", SmallAmp);
				printf("AvgAmp : %ld\n", AvgAmp);
				printf("LargerAmp : %ld\n", LargerAmp);
				printf("LargestAmp : %ld\n", LargestAmp);
				printf("\n");
				printf("CurAmp : %d\n", CurHigh - CurLow);

				printf("=========================================\n");

				printf("ServerTime: %d: %d: %d", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);
			}
		}

		// 检测按键事件
		if (_kbhit())
		{
			// 读取按键
			char ch = _getch();
			// 退出循环
			break;
		}

		// 继续循环，确保不阻塞其他操作
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 短暂休眠，避免过度占用 CPU
	}

	// CloseHandle(hEvent);

	release();

	system("pause");

	exit(0);
}

int main()
{

	DEBUG(DEBUG_LEVEL_DEBUG, "start");

	CoInitialize(NULL);

	init();

	// printf("請輸入身分證字號：");
	// cin >> g_strUserId;

	g_strUserId = "F129305651";

	// printf("請輸入密碼：");
	string pwd;
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
	pwd = "youlose1A";
	cout << endl;

	cout << g_strUserId << " " << pwd << endl;

	g_nCode = pSKCenterLib->Login(g_strUserId.c_str(), pwd.c_str());

	pSKCenterLib->PrintfCodeMessage("Center", "Login", g_nCode);

	if (g_nCode != 0)
	{
		return 0;
	}

	SetConsoleMode(hStdin, mode);

	thread tMain(thread_main);
	if (tMain.joinable())
		tMain.detach();

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0)) // Get SendMessage loop
	{
		DispatchMessageW(&msg);
	}

	DEBUG(DEBUG_LEVEL_DEBUG, "end");

	system("pause");

	return 0;
}
