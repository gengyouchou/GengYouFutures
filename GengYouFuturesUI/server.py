import asyncio
import websockets
import json
import win32pipe
import win32file

# WebSocket 处理函数，接收到消息后将其发送回客户端
async def echo(websocket, path):
    async for message in websocket:
        print(f'Received WebSocket message: {message}')
        await websocket.send(f'Message received: {message}')

# 从命名管道读取数据
def read_from_pipe():
    pipe_name = r'\\.\pipe\FuturesPipe'  # C++ 端定义的管道名称
    try:
        # 打开命名管道
        handle = win32file.CreateFile(
            pipe_name,
            win32file.GENERIC_READ,
            0,  # 不共享
            None,  # 默认安全属性
            win32file.OPEN_EXISTING,
            0,  # 默认属性
            None
        )

        while True:
            # 从管道读取数据
            data = win32file.ReadFile(handle, 4096)
            message = data[1].decode("utf-8")  # 解码
            print(f"Received data from pipe: {message}")
            return message  # 返回数据

    except Exception as e:
        print(f"Error reading from pipe: {e}")
        return None

# 启动 WebSocket 服务器并接收管道数据
async def main():
    async with websockets.serve(echo, 'localhost', 8765):
        print('WebSocket server started at ws://localhost:8765')

        # 读取管道数据并通过 WebSocket 发送
        while True:
            pipe_data = read_from_pipe()  # 获取管道数据
            if pipe_data:
                print(f"Sending to WebSocket: {pipe_data}")
                # 发送数据到所有连接的 WebSocket 客户端
                await websockets.broadcast([websocket], pipe_data)  # 这里需要遍历所有连接的 WebSocket
            await asyncio.sleep(1)  # 每秒检查一次管道数据

if __name__ == '__main__':
    # 使用 asyncio.run 启动事件循环
    asyncio.run(main())
