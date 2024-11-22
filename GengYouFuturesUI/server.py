import asyncio
import websockets
import json

async def echo(websocket, path):
    async for message in websocket:
        print(f'Received message: {message}')
        await websocket.send(f'Message received: {message}')

async def main():
    # 使用 websockets.serve 来启动服务器
    async with websockets.serve(echo, 'localhost', 8765):
        print('WebSocket server started at ws://localhost:8765')
        await asyncio.Future()  # run forever

if __name__ == '__main__':
    # 使用 asyncio.run 来启动事件循环
    asyncio.run(main())
