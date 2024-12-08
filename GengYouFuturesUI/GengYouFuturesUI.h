#include "httplib.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <thread>
#include <nlohmann/json.hpp>
#include <random>
#include <ctime>
#include <atomic>
#include <Strategy.h>
#include <SKCenterLib.h>
#include <SKOSQuoteLib.h>
#include <SKOrderLib.h>
#include <SKQuoteLib.h>
#include <SKReplyLib.h>
#include <Logger.h>

// User account
struct USER_ACCOUNT_UI
{
    string g_strUserId;
};

struct STRATEGY_CONFIG_UI
{
    LONG ClosingKeyPriceLevel;
    LONG BidOfferLongShortThreshold;
    LONG BidOfferLongShortExtremeValue;
    DOUBLE BidOfferLongShortAttackSlope;
    LONG ActivePoint;
    DOUBLE MaximumLoss;
    LONG StrategyMode;
    SHORT SpecifyLongShort;
};

struct MARKET_DATA_UI
{
    std::unordered_map<long, long> gCurCommPrice;
    std::unordered_map<SHORT, std::array<long, 6>> gCurTaiexInfo;
    SHORT gCurServerTime[3];
    std::unordered_map<long, long> gCurOsCommPrice;
    std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;
    std::unordered_map<long, std::array<long, 6>> gTransactionList;

    DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice;

    LONG gBidOfferLongShort;
    LONG gTransactionListLongShort;
    LONG gOsTransactionListLongShort;
    LONG gLongShort;
    double gCostMovingAverageVal;
    double gMa5;
    double gMa5LongShort;
    double gNQMa20;
    double gNQMa20LongShort;

    double gBidOfferLongShortSlope;
    double gNumberOfStocksRisingAndFalling;
    std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint; // {High, Low, Open, Data}

    LONG gEvaluatePosition;
    double gClosedProfitLoss;
    double gFutureRight;
};

struct OPEN_INTEREST_INFO_UI
{
    string product;
    string buySell;
    LONG openPosition;
    LONG dayTradePosition;
    DOUBLE avgCost;
    DOUBLE profitAndLoss;
    BOOLEAN NeedToUpdate;
};

extern USER_ACCOUNT_UI gUserAccountUI;
extern STRATEGY_CONFIG_UI gStrategyConfigUI;
extern MARKET_DATA_UI gMarketDataUI;
extern OPEN_INTEREST_INFO_UI gOpenInterestInfoUI;
