#ifndef GENG_YOU_CFD_STRATEGY_H
#define GENG_YOU_CFD_STRATEGY_H

#ifdef __cplusplus
extern "C"
{
#endif

    // 启动 HTTP 服务器，监听本机端口80接收外部下单 JSON 请求
    __declspec(dllexport) void StartHttpServer(void);

    // 停止 HTTP 服务器（示例实现，实际项目请实现优雅停止逻辑）
    __declspec(dllexport) void StopHttpServer(void);

    // 处理来自 MQL4 的持仓 JSON 数据，计算下单信号并返回 JSON 字符串
    // 输入格式（字符串）：\n// {\n//   \"Margin\": 10000,\n//   \"ClosedProfitLoss\": 0,\n//   \"OpenPosition\": [\n//       { \"CommodityId\": \"XAUUSD\", \"Lots\": 0.1, \"FloatingProfitLoss\": 10.0 },\n//       { \"CommodityId\": \"NQ100\", \"Lots\": 0.1, \"FloatingProfitLoss\": -5.5 }\n//   ]\n// }\n// 返回格式（字符串）：\n// [\n//   { \"CommodityId\": \"XAUUSD\", \"Lots\": 0.1, \"OrderType\": \"TakeProfit\", \"Amount\": 100.0 },\n//   ...\n// ]\n
    __declspec(dllexport) const char* ProcessPositions(const char* jsonInput);

#ifdef __cplusplus
}
#endif

#endif // GENG_YOU_CFD_STRATEGY_H
