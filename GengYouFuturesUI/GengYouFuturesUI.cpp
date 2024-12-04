#include "httplib.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp> // 用於生成 JSON

using json = nlohmann::json;
double generateRandomPrice(double currentPrice)
{
    double fluctuation = (rand() % 21 - 10) / 10.0;
    return currentPrice + fluctuation;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    httplib::Server svr;

    double currentPrice = 20000.0;

    svr.Get("/", [&currentPrice](const httplib::Request &, httplib::Response &res)
            {
        currentPrice += generateRandomPrice(1);
        json data = {
            {"time", time(0)},
            {"price", currentPrice},
            {"volume", 100},
            {"symbol", "FUTURE1"}};
        res.set_content(data.dump(), "application/json"); });

    svr.set_error_handler([](const httplib::Request &req, httplib::Response &res)
                          {
        res.set_content(R"({"error": "Unsupported method"})", "application/json");
        res.status = 501; });

    std::cout << "Server started at http://localhost:8000" << std::endl;
    svr.listen("0.0.0.0", 8000);
}
