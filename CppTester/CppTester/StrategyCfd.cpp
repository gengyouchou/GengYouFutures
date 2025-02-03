#include "Strategy.h"
#include "StrategyCfd.h"
#include "SKCenterLib.h"
#include "SKOSQuoteLib.h"
#include "SKOrderLib.h"
#include "SKQuoteLib.h"
#include "SKReplyLib.h"
#include <Logger.h>
#include <algorithm>
#include <array>
#include <chrono> // For std::chrono::steady_clock
#include <cmath>
#include <conio.h> // For kbhit() and _getch()
#include <cstdlib> // For system("cls")
#include <deque>
#include <iostream>
#include <map>
#include <numeric> // This header is needed for std::accumulate
#include <queue>
#include <set>
#include <thread> // For std::this_thread::sleep_for
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "config.h"

using namespace std;

// Object pointer

extern CSKCenterLib *pSKCenterLib;
extern CSKQuoteLib *pSKQuoteLib;
extern CSKReplyLib *pSKReplyLib;
extern CSKOrderLib *pSKOrderLib;

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];
extern std::unordered_map<SHORT, std::array<long, 6>> gCurTaiexInfo;
extern std::deque<long> gDaysKlineDiff;
extern std::deque<long> gCostMovingAverage;
extern std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint;
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

extern OpenInterestInfo gOpenInterestInfo;

extern string g_strUserId;

extern COMMODITY_INFO gCommodtyInfo;

extern std::map<string, pair<double, double>> gDaysCommHighLowPoint;         // Max len: DAY_NIGHT_HIGH_LOW_K_LINE
extern std::map<string, pair<double, double>> gDaysNightAllCommHighLowPoint; // Max len: DAY_NIGHT_HIGH_LOW_K_LINE