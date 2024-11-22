import asyncio
import websockets
import json
import win32pipe
import win32file

# WebSocket 客户端列表
connected_clients = set()
# WebSocket 处理函数
async def handle_websocket(websocket, path):
    try:
        print(f"New WebSocket connection from {websocket.remote_address} with path {path}")
        # 将客户端添加到客户端列表
        connected_clients.add(websocket)

        while True:
            message = await websocket.recv()  # 等待客户端消息
            print(f"Received WebSocket message: {message}")
            # 这里您可以根据业务需要处理消息

    except websockets.exceptions.ConnectionClosed:
        print(f"Client disconnected: {websocket.remote_address}")
    except Exception as e:
        print(f"Error handling WebSocket connection: {e}")
    finally:
        # 从客户端列表中移除断开连接的客户端
        connected_clients.remove(websocket)

# 从命名管道读取数据
async def read_from_pipe(pipe_name):
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
                yield message  # 使用异步生成器逐条返回数据
            await asyncio.sleep(0.1)  # 避免过度占用资源
    except Exception as e:
        print(f"Error reading from pipe: {e}")

# 从管道读取数据并广播给 WebSocket 客户端
async def pipe_to_websocket(pipe_name):
    async for message in read_from_pipe(pipe_name):
        if connected_clients:  # 只有当有客户端连接时才广播
            print(f"Broadcasting to {len(connected_clients)} clients: {message}")
            await asyncio.gather(
                *[client.send(message) for client in connected_clients],
                return_exceptions=True  # 防止单个客户端异常中断所有广播
            )

# 主函数，启动服务器和管道数据处理
async def main():
    pipe_name = r'\\.\pipe\FuturesPipe'  # C++ 定义的管道名称

    # 启动 WebSocket 服务器
    websocket_server = websockets.serve(handle_websocket, 'localhost', 8765)
    print('WebSocket server started at ws://localhost:8765')

    # 同时运行 WebSocket 服务器和管道处理
    await asyncio.gather(
        websocket_server,
        pipe_to_websocket(pipe_name)
    )

if __name__ == '__main__':
    asyncio.run(main())
