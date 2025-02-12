#pragma once

#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include "SKCOM_reference.h"

#define COMMODITY_OS_GC "NYM,GC0000"
#define COMMODITY_OS_DX "ICEUS,DX0000"

extern std::unordered_map<long, long> gCfdTransactionListLongShort;
extern std::unordered_map<long, double> gCfdTransactionListLongShortSlope;

VOID CfdStrategySwitch();
