#include <uwebsocket/App.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <thread>
#include <ctime>
#include <cstdlib>

// 全局价格数据
std::mutex data_mutex;
std::vector<std::pair<std::string, double>> price_data = {
    {"2024-11-17 10:00", 1234.56},
    {"2024-11-17 10:01", 1235.78},
    {"2024-11-17 10:02", 1233.21}};

// 广播函数
std::mutex clients_mutex;
std::vector<uWS::WebSocket<uWS::SERVER> *> clients;

void broadcast_price(const std::string &message)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto client : clients)
    {
        client->send(message.c_str(), message.length(), uWS::OpCode::TEXT);
    }
}

// 模拟价格实时更新
void simulate_price_updates()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        double new_price = 1230.0 + (rand() % 100) / 10.0;
        std::time_t now = std::time(nullptr);
        char time_buffer[100];
        std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M", std::localtime(&now));

        std::lock_guard<std::mutex> lock(data_mutex);
        price_data.push_back({time_buffer, new_price});

        std::string message = "New Price: " + std::to_string(new_price);
        broadcast_price(message);
    }
}

int main()
{
    // 启动价格更新模拟线程
    std::thread(simulate_price_updates).detach();

    uWS::App()
        // WebSocket 路由: 实时推送价格
        .ws<true>("/*", {.open = [](auto *ws, auto *req)
                         {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(ws);
                std::cout << "Client connected" << std::endl; },
                         .close = [](auto *ws, int code, auto *message, size_t length)
                         {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(std::remove(clients.begin(), clients.end(), ws), clients.end());
                std::cout << "Client disconnected" << std::endl; },
                         .message = [](auto *ws, std::string_view message, uWS::OpCode opCode)
                         { std::cout << "Received message: " << message << std::endl; }})
        .listen(8080, [](auto *token)
                {
            if (token) {
                std::cout << "Server started at ws://localhost:8080" << std::endl;
            } })
        .run();
}
