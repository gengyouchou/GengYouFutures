#pragma once

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

struct DAY_AMP_AND_KEY_PRICE
{
    long AvgAmp;
    long LargestAmp;
    long SmallestAmp;
    long LargerAmp;
    long SmallAmp;

    long LongKey1;
    long LongKey2;
    long LongKey3;
    long LongKey4;
    long LongKey5;

    long ShortKey1;
    long ShortKey2;
    long ShortKey3;
    long ShortKey4;
    long ShortKey5;

    long CostMovingAverage;
};

struct BID_OFFER_LONG_AND_SHORT
{
    LONG Tsmc;
    LONG Foxconn;
    LONG MediaTek;
};

VOID StrategyStopFuturesLoss(string strUserId);
VOID StrategyClosePosition(string strUserId);
VOID StrategyNewLongPosition(string strUserId);
VOID StrategyNewShortPosition(string strUserId);
LONG StrategyCaluBidOfferLongShort(VOID);
VOID StrategyNewIntervalAmpLongShortPosition(string strUserId, LONG LongShort);

LONG AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell);
VOID AutoCalcuKeyPrices(LONG nStockidx);

#define MAXIMUM_LOSS 3000
#define DOLLARS_PER_TICK 10
#define STOP_POINT 100

#define BID_OFFER_LONG_SHORT_THRESHOLD 100

// Order

#define ORDER_CLOSE_POSITION 1
#define ORDER_NEW_POSITION 0
