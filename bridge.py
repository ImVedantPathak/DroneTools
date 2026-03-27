import serial
import asyncio
import websockets

# CHANGE THIS to your port
ser = serial.Serial("COM3", 115200, timeout=1)

clients = set()

async def handler(websocket):
    clients.add(websocket)
    try:
        while True:
            await asyncio.sleep(1)
    finally:
        clients.remove(websocket)

async def send_data():
    while True:
        line = ser.readline().decode(errors="ignore").strip()

        # Only forward valid JSON
        if not line.startswith("{"):
            continue

        for ws in clients:
            try:
                await ws.send(line)
            except:
                pass

async def main():
    print("WebSocket running at ws://localhost:8080")
    await websockets.serve(handler, "localhost", 8080)
    await send_data()

asyncio.run(main())

'''
Must look like this

{"roll":10,"pitch":5,"yaw":30,"ax":0.1,"ay":0.2,"az":9.8}
'''
