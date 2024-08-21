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
};

struct BID_OFFER_LONG_AND_SHORT
{
    LONG Tsmc;
    LONG Foxconn;
    LONG MediaTek;
};

struct STRATEGY_CONFIG
{
    LONG ClosingKeyPriceLevel;
    LONG BidOfferLongShortThreshold;
    LONG ActivePoint;
    DOUBLE MaximumLoss;
};

VOID StrategyStopFuturesLoss(string strUserId, LONG MtxCommodtyInfo);
VOID StrategyClosePosition(string strUserId, LONG MtxCommodtyInfo);
VOID StrategyNewLongShortPosition(string strUserId, LONG MtxCommodtyInfo, LONG LongShort);
LONG StrategyCaluBidOfferLongShort(VOID);
VOID StrategyNewIntervalAmpLongShortPosition(string strUserId, LONG MtxCommodtyInfo, LONG LongShort);
DOUBLE CountCostMovingAverage(VOID);

LONG AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell);
VOID AutoCalcuKeyPrices(VOID);

// Strategy

#define SWING_POINTS 20

// Order

#define ORDER_BUY_LONG_POSITION 0   // 0: Buy, 1: Sell
#define ORDER_SELL_SHORT_POSITION 1 // 0: Buy, 1: Sell
#define ORDER_CLOSE_POSITION 1
#define ORDER_NEW_POSITION 0
#define DOLLARS_PER_TICK 10

// Mode

#define TEST_MODE 1

#if TEST_MODE

#define VIRTUAL_ACCOUNT_ORDER

#endif