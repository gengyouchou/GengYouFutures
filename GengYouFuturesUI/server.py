from aiohttp import web
from datetime import datetime
import asyncio
import json

# 全局存储接收到的 HTTP 数据
received_data = []

# CORS 中间件
@web.middleware
async def cors_middleware(request, handler):
    """
    处理跨域请求的中间件，允许所有来源。
    """
    if request.method == 'OPTIONS':
        return web.Response(headers={
            'Access-Control-Allow-Origin': '*',
            'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
            'Access-Control-Allow-Headers': 'Content-Type, Authorization'
        })
    response = await handler(request)
    response.headers['Access-Control-Allow-Origin'] = '*'
    response.headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'
    response.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'
    return response

# HTTP 处理函数，用于返回接收到的数据和当前时间
async def handle_http_get(request):
    """
    返回接收到的所有数据和当前时间。
    """
    current_time = datetime.now().isoformat()
    response_data = {
        'time': current_time,
        'data': received_data
    }
    print(f"HTTP GET Response: {response_data}")
    return web.json_response(response_data)

# HTTP 处理函数，用于接收来自 C++ 程序的 JSON 数据
async def handle_http_post(request):
    """
    处理 POST 请求，接收并存储来自 C++ 发送的 JSON 数据。
    """
    try:
        # 接收 JSON 数据
        post_data = await request.json()
        received_data.append(post_data)
        print(f"Received POST data: {post_data}")
        return web.json_response({'status': 'success', 'received': post_data})
    except json.JSONDecodeError:
        print("Error: Invalid JSON format.")
        return web.json_response({'status': 'error', 'message': 'Invalid JSON format'}, status=400)
    except Exception as e:
        print(f"Error processing POST request: {e}")
        return web.json_response({'status': 'error', 'message': str(e)}, status=400)

# 启动 HTTP 服务器
async def main():
    app = web.Application(middlewares=[cors_middleware])

    # 添加 GET 和 POST 路由
    app.router.add_get('/', handle_http_get)
    app.router.add_post('/futures', handle_http_post)

    runner = web.AppRunner(app)
    await runner.setup()
    site = web.TCPSite(runner, '0.0.0.0', 8080)  # 开放给所有网络接口
    print('HTTP server started at http://0.0.0.0:8080')
    await site.start()

    await asyncio.Event().wait()

if __name__ == '__main__':
    asyncio.run(main())
