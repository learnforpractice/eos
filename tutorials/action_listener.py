import sys
import json
import asyncio
import zmq
import zmq.asyncio
import websockets

from pyeoskit import eosapi

# eosapi.set_filter_on('*')
eosapi.clear_filter_on()
eosapi.set_filter_on('learnfortest1::')
# eosapi.set_filter_on('eosio.token::')

port = "5556"
if len(sys.argv) > 1:
    port =  sys.argv[1]
    int(port)
    
if len(sys.argv) > 2:
    port1 =  sys.argv[2]
    int(port1)

USERS = []

async def do_work():
    print("listening...")
    # Socket to talk to server
    context = zmq.asyncio.Context()
    socket = context.socket(zmq.SUB)
    #socket.connect ("tcp://localhost:%s" % port)
    socket.connect ("ipc:///tmp/0")
    if len(sys.argv) > 2:
        socket.connect ("tcp://localhost:%s" % port1)
    #socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)
    # socket.setsockopt_string(zmq.SUBSCRIBE, "eosio.token")
    socket.setsockopt_string(zmq.SUBSCRIBE, "learnfortest1")

    while True:
        string = await socket.recv_string()
        action = await socket.recv_string()
        a = json.loads(action)
        print('++++++++++++:', string, action, a['act'])
        if a['act']['account'] == 'eosio.token' and a['act']['name'] == 'transfer':
            d = a['act']['data']
    #        print(d)
            d = bytes.fromhex(d)
            try:
                s = eosapi.unpack_args('eosio.token', 'transfer', d)
                print(s)
                for user in USERS:
                    try:
                        await user.send(s)
                    except Exception as e:
                        print(e)
            except Exception as e:
                print(e)
                try:
                    s = eosapi.unpack_args('eosio.token', 'transfer', d[:32]+b'\x00')
                    print(s)
                except Exception as e:
                    print(e)

async def hello(websocket, path):
    USERS.append(websocket)
    print('+++++++++connected', websocket, path)
    try:
        await websocket.recv()
    except Exception as e:
        USERS.remove(websocket)
        print(e)

async def websocket_serve():
    print('+++++++++++websocket_serve')
    start_server = websockets.serve(hello, "localhost", 6789)
    asyncio.get_event_loop().run_until_complete(start_server)

async def main():
    task1 = asyncio.create_task(do_work())
    task2 = asyncio.create_task(websocket_serve())
    await task1
    await task2

asyncio.run(main())

