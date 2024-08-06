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
Logger StrategyLog("Strategy.log");

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 2>> gCurCommHighLowPoint;
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

VOID StrategyStopFuturesLoss(CSKOrderLib *SKOrderLib, string strUserId)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    SKOrderLib->GetOpenInterest(strUserId, 1);

    // AutoOrderMTX(0); // new
    // AutoOrderMTX(1); // close

    // if over loss then do GetOpenInterest again in order to make sure 
    // AutoOrderMTX(1) won’t place a closing order with the wrong amount
    // else continue to calculate profit and loss and update to global variables

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}
