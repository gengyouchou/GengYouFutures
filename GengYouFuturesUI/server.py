import asyncio
from aiohttp import web
import json
import win32pipe
import win32file
from datetime import datetime

# 全局存储从管道接收到的数据
received_data = []

# CORS 中间件
@web.middleware
async def cors_middleware(request, handler):
    response = await handler(request)
    # 允许所有来源的请求
    response.headers['Access-Control-Allow-Origin'] = '*'  # 允许所有来源
    response.headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'  # 允许的方法
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type'  # 允许的头部
    return response

# HTTP 处理函数，用于返回从管道接收到的数据和当前时间
async def handle_http(request):
    """
    返回从命名管道接收到的所有数据和当前时间。
    """
    # 获取当前时间
    current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    # 构建响应数据
    response_data = {
        'time': current_time,
        'data': received_data
    }

    # 调试信息，打印即将返回的响应数据
    print(f"HTTP Response: {response_data}")

    # 返回包含当前时间和管道数据的JSON响应
    return web.json_response(response_data)

# 从命名管道读取数据
async def read_from_pipe(pipe_name):
    """
    异步地从命名管道读取数据，并将其存储在全局列表中。
    """
    while True:
        try:
            print(f"Attempting to connect to pipe: {pipe_name}")
            # 打开命名管道
            handle = win32file.CreateFile(
                pipe_name,
                win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                0,  # 不共享
                None,  # 默认安全属性
                win32file.OPEN_EXISTING,
                0,  # 默认属性
                None
            )
            print(f"Connected to pipe: {pipe_name}")

            while True:
                try:
                    # 从管道读取数据
                    result, data = win32file.ReadFile(handle, 4096)
                    if result == 0:  # 如果读取成功
                        message = data.decode("utf-8")  # 解码为字符串
                        print(f"Received data from pipe: {message}")
                        received_data.append(message)  # 将数据存储到全局列表
                        # 如果数据过多，可以选择定期清理
                        if len(received_data) > 1000:
                            received_data.pop(0)  # 移除最早的一条数据
                    await asyncio.sleep(0.1)  # 避免过度占用资源
                except Exception as e:
                    print(f"Error reading from pipe: {e}")
                    break  # 出现错误时退出内循环，重新连接
        except Exception as e:
            print(f"Error connecting to pipe: {e}")
            await asyncio.sleep(5)  # 连接失败时，等待5秒再重试

# 启动 HTTP 服务器和测试数据注入
async def main():
    pipe_name = r'\\.\pipe\FuturesPipe'  # C++ 定义的管道名称

    # 创建 aiohttp 应用
    app = web.Application(middlewares=[cors_middleware])  # 启用 CORS 中间件

    # 添加路由：访问根路径返回从管道读取的数据
    app.router.add_get('/', handle_http)

    # 启动 HTTP 服务器
    runner = web.AppRunner(app)
    await runner.setup()
    site = web.TCPSite(runner, 'localhost', 8080)
    print('HTTP server started at http://localhost:8080')
    await site.start()

    # 启动管道读取任务（如果管道工作正常）
    asyncio.create_task(read_from_pipe(pipe_name))

    # 保持主线程运行，等待终止
    await asyncio.Event().wait()

if __name__ == '__main__':
    asyncio.run(main())
