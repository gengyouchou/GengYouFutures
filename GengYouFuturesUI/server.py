import asyncio
import websockets
import json
import win32pipe
import win32file

# WebSocket 客户端列表
connected_clients = set()

# WebSocket 处理函数，处理客户端连接和断开
async def echo(websocket, path):
    # 添加到已连接的客户端列表
    connected_clients.add(websocket)
    print(f"New client connected: {websocket.remote_address}")
    try:
        # 保持连接直到客户端断开
        async for message in websocket:
            print(f"Received WebSocket message: {message}")
            await websocket.send(f"Message received: {message}")
    except websockets.exceptions.ConnectionClosed:
        print(f"Client disconnected: {websocket.remote_address}")
    finally:
        # 从客户端列表中移除断开连接的客户端
        connected_clients.remove(websocket)

# 从命名管道读取数据
def read_from_pipe():
    pipe_name = r'\\.\pipe\FuturesPipe'  # C++ 端定义的管道名称
    try:
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

        while True:
            # 从管道读取数据
            result, data = win32file.ReadFile(handle, 4096)
            if result == 0:  # 如果读取成功
                message = data.decode("utf-8")  # 解码为字符串
                print(f"Received data from pipe: {message}")
                yield message  # 使用生成器逐条返回数据
    except Exception as e:
        print(f"Error reading from pipe: {e}")

# 从管道读取数据并广播给 WebSocket 客户端
async def pipe_to_websocket():
    for message in read_from_pipe():
        if connected_clients:  # 只有当有客户端连接时才广播
            print(f"Broadcasting to {len(connected_clients)} clients: {message}")
            await asyncio.gather(
                *[client.send(message) for client in connected_clients]
            )
        await asyncio.sleep(1)  # 每秒检查一次管道数据

# 启动 WebSocket 服务器
async def main():
    # 启动 WebSocket 服务器
    server = await websockets.serve(echo, 'localhost', 8765)
    print('WebSocket server started at ws://localhost:8765')

    # 启动管道到 WebSocket 的数据转发任务
    await pipe_to_websocket()

if __name__ == '__main__':
    asyncio.run(main())
