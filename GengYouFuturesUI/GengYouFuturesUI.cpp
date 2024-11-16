#include "crow.h"
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// 全局价格数据
std::mutex data_mutex;
std::vector<std::pair<std::string, double>> price_data = {
    {"2024-11-17 10:00", 1234.56},
    {"2024-11-17 10:01", 1235.78},
    {"2024-11-17 10:02", 1233.21}};

// 广播函数用于 WebSocket
std::vector<crow::websocket::connection *> clients;
std::mutex clients_mutex;

void broadcast_price(const std::string &message)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto client : clients)
    {
        client->send_text(message);
    }
}

int main()
{
    crow::SimpleApp app;

    // REST API: 获取价格数据
    CROW_ROUTE(app, "/prices")([]()
                               {
        std::lock_guard<std::mutex> lock(data_mutex);
        crow::json::wvalue response;
        for (size_t i = 0; i < price_data.size(); ++i) {
            response[i]["time"] = price_data[i].first;
            response[i]["price"] = price_data[i].second;
        }
        return response; });

    // WebSocket: 实时推送价格
    CROW_WEBSOCKET_ROUTE(app, "/price_updates")
        .onopen([](crow::websocket::connection &conn)
                {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(&conn); })
        .onclose([](crow::websocket::connection &conn, const std::string &reason)
                 {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), &conn), clients.end()); });

    // 模拟价格实时更新
    std::thread([]()
                {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            double new_price = 1230.0 + (rand() % 100) / 10.0;
            {
                std::lock_guard<std::mutex> lock(data_mutex);
                price_data.emplace_back("2024-11-17 10:05", new_price);
            }
            broadcast_price("New Price: " + std::to_string(new_price));
        } })
        .detach();

    // 启动服务器
    app.port(8080).multithreaded().run();
}
