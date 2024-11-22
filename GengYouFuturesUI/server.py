import asyncio
import websockets
import json

# 记录所有连接的 WebSocket 客户端
clients = []

# WebSocket 处理函数
async def echo(websocket, path):
    clients.append(websocket)  # 将新的客户端连接加入到列表

    try:
        while True:
            # 接收 C++ 发送的数据
            message = await websocket.recv()
            print(f"Received from C++: {message}")

            # 将接收到的数据转发给所有连接的前端客户端
            for client in clients:
                try:
                    data = json.dumps({"data": message})  # 格式化数据
                    await client.send(data)
                except:
                    # 如果客户端断开连接，移除该客户端
                    clients.remove(client)

    except websockets.ConnectionClosed:
        clients.remove(websocket)
        print("Connection closed")

# 启动 WebSocket 服务器
async def start_server():
    server = await websockets.serve(echo, "localhost", 8081)  # 启动在本地 8081 端口
    await server.wait_closed()

# 运行事件循环
asyncio.get_event_loop().run_until_complete(start_server())
