
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
#include <yaml-cpp/yaml.h>
#include "Strategy.h"

#pragma comment(lib, "Ws2_32.lib")
char buffer[10240]  = {0};


extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;
extern std::unordered_map<long, std::array<long, 5>> gTransactionList;

extern COMMODITY_INFO gCommodtyInfo;
extern DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice;
extern OpenInterestInfo gOpenInterestInfo;
extern LONG gBidOfferLongShort;
extern LONG gTransactionListLongShort;
extern double gCostMovingAverageVal;
extern STRATEGY_CONFIG gStrategyConfig;


extern string g_strUserId;
extern string gPwd;


void thread_socket()
{
    WSADATA wsaData;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    const char *greeting = "Connected to server";

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
      
    }

    // 创建套接字文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    // 绑定套接字到端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
       
    }
    // 监听传入连接
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(server_fd);
        WSACleanup();
       
    }
    std::cout << "Server is listening on port " << PORT << std::endl;

    // string temp = "[UserId:], [LongShortThreshold:%], [BidOfferLongShortThreshold:], [ActivePoint:], [MaximumLoss:]\n";
    // char tab2[1024];
    // strncpy(tab2, temp.c_str(), sizeof(tab2));
    // tab2[sizeof(tab2) - 1] = 0;

    char buffer_empty[4096]={0};

    while (true) {
        std::cout << "Waiting for new connection..." << std::endl;
        // 接受客户端连接
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed" << std::endl;
            continue; // 继续等待其他客户端连接
        }

        send(new_socket, greeting, strlen(greeting), 0); // 发送初始连接消息

        while (true) {
            std::cout<<"client in";
            // Sleep(500);
            // int valread = 1023;
            int valread = recv(new_socket, buffer_empty, 4096, 0); // 读取客户端消息
            if (valread > 0) {

                buffer_empty[valread] = '\0'; // 确保字符串以 null 结尾
                std::cout << "Message from client: " << buffer_empty << std::endl;

                send(new_socket, buffer, 4095, 0); // 发送消息回客户端
                // send(new_socket, tab2, 1023, 0); // 发送消息回客户端
            } else if (valread == 0) {
                std::cout << "Client disconnected" << std::endl;
                break; // 客户端断开连接，退出内循环
            } else {
                std::cerr << "recv failed" << std::endl;
                break;
            }
        }

        closesocket(new_socket); // 关闭与当前客户端的连接
    }
    closesocket(server_fd);
    WSACleanup();
}
void printfToClinet(){
  
    snprintf(buffer, sizeof(buffer),"[UserId:%s], [LongShortThreshold:%ld], [BidOfferLongShortThreshold:%ld], [ActivePoint:%ld], [MaximumLoss:%f]\n=========================================\n[CurMtxPrice: %ld],[TSEA prices: %ld, Valume: %ld],[Diff: %d],[ServerTime: %d: %d: %d]\n=========================================\nOpen: %ld, CurHigh: %ld, CurLow: %ld, CostMovingAverage: %ld,CurAvg: %ld, CurAmp : %ld\n=========================================\nOpen Position: %d, AvgCost:%f, ProfitAndLoss: %f\n=========================================\nLong Key 5: %ld\nLong Key 4: %ld\nLong Key 3: %ld\nLong Key 2: %ld\nLong Key 1: %ld\n=========================================\nShort Key 1: %ld\nShort Key 2: %ld\nShort Key 3: %ld\nShort Key 4: %ld\nShort Key 5: %ld\n=========================================\nSmallestAmp : %ld, SmallAmp : %ld,AvgAmp : %ld, LargerAmp : %ld,LargestAmp : %ld\n=========================================\n[LongShortThreshold:%ld], StrategyCaluLongShort:%ld, BidOfferLongShort:%ld, TransactionListLongShort:%ld\n=========================================\n",
                                                            g_strUserId.c_str(), gStrategyConfig.ClosingKeyPriceLevel, gStrategyConfig.BidOfferLongShortThreshold, gStrategyConfig.ActivePoint, gStrategyConfig.MaximumLoss,gCurCommPrice[MtxCommodtyInfo] / 100,gCurCommPrice[gCommodtyInfo.TSEAIdxNo] / 100, gCurTaiexInfo[0][1],(gCurCommPrice[MtxCommodtyInfo] - gCurCommPrice[gCommodtyInfo.TSEAIdxNo]) / 100,gCurServerTime[0], gCurServerTime[1], gCurServerTime[2],gCurCommHighLowPoint[MtxCommodtyInfo][2], CurHigh, CurLow, CostMovingAverage,(CurHigh + CurLow) / 2, CurHigh - CurLow,gOpenInterestInfo.openPosition,
                                            gOpenInterestInfo.avgCost,
                                            gOpenInterestInfo.profitAndLoss,gDayAmpAndKeyPrice.LongKey5,gDayAmpAndKeyPrice.LongKey4,gDayAmpAndKeyPrice.LongKey3,gDayAmpAndKeyPrice.LongKey2,gDayAmpAndKeyPrice.LongKey1,gDayAmpAndKeyPrice.ShortKey1,gDayAmpAndKeyPrice.ShortKey2,gDayAmpAndKeyPrice.ShortKey3,gDayAmpAndKeyPrice.ShortKey4,gDayAmpAndKeyPrice.ShortKey5,gDayAmpAndKeyPrice.SmallestAmp,gDayAmpAndKeyPrice.SmallAmp,gDayAmpAndKeyPrice.AvgAmp,gDayAmpAndKeyPrice.LargerAmp,gDayAmpAndKeyPrice.LargestAmp,gStrategyConfig.BidOfferLongShortThreshold, StrategyCaluLongShort(), gBidOfferLongShort, gTransactionListLongShort);

    sprintf(buffer+strlen(buffer),"%s : %ld Open: %ld, CurHigh: %ld, CurLow: %ld\nTotal Offer: [%ld]\nAsk5: [%ld]: [%ld]\nAsk4: [%ld]: [%ld]\nAsk3: [%ld]: [%ld]\nAsk2: [%ld]: [%ld]\nAsk1: [%ld]: [%ld]\n",ProductName.c_str(), gCurCommPrice[ProductIdxNo],Open, CurHigh, CurLow,TotalOffer,gBest5BidOffer[ProductIdxNo][9].first, gBest5BidOffer[ProductIdxNo][9].second,gBest5BidOffer[ProductIdxNo][8].first, gBest5BidOffer[ProductIdxNo][8].second,gBest5BidOffer[ProductIdxNo][7].first, gBest5BidOffer[ProductIdxNo][7].second,gBest5BidOffer[ProductIdxNo][6].first, gBest5BidOffer[ProductIdxNo][6].second,gBest5BidOffer[ProductIdxNo][5].first, gBest5BidOffer[ProductIdxNo][5].second);    
    sprintf(buffer+strlen(buffer),"=========================================\n");
    sprintf(buffer+strlen(buffer),"Bid1: [%ld]: [%ld]\nBid2: [%ld]: [%ld]\nBid3: [%ld]: [%ld]\nBid4: [%ld]: [%ld]\nBid5: [%ld]: [%ld]\nTotal Bid:   [%ld]\n=========================================\n",gBest5BidOffer[ProductIdxNo][0].first, gBest5BidOffer[ProductIdxNo][0].second,gBest5BidOffer[ProductIdxNo][1].first, gBest5BidOffer[ProductIdxNo][1].second,gBest5BidOffer[ProductIdxNo][2].first, gBest5BidOffer[ProductIdxNo][2].second,gBest5BidOffer[ProductIdxNo][3].first, gBest5BidOffer[ProductIdxNo][3].second,gBest5BidOffer[ProductIdxNo][4].first, gBest5BidOffer[ProductIdxNo][4].second,TotalBid);
}