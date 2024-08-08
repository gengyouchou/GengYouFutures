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

VOID StrategyStopFuturesLoss(string strUserId);
VOID StrategyClosePosition(string strUserId);
LONG AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell);
VOID AutoCalcuKeyPrices();

#define MAXIMUM_LOSS 5000
