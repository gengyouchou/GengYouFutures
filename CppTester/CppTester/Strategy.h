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

VOID StrategyStopFuturesLoss(CSKOrderLib *SKOrderLib, string strUserId);
void AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell);

#define MAXIMUM_LOSS 5000
#define COMMODITY_MAIN "MTX00"
#define COMMODITY_OTHER "TM0000"
