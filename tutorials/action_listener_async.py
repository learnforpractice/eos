import sys
import json
import logging
import zmq
import asyncio
import zmq.asyncio

from pyeoskit import eosapi

logging.basicConfig(level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)


ctx = zmq.asyncio.Context()
eosapi.set_filter_on('eosio.token::')

async def recv_and_process():
    sock = ctx.socket(zmq.SUB)
    sock.setsockopt_string(zmq.SUBSCRIBE, "eosio.token")
    sock.setsockopt_string(zmq.SUBSCRIBE, "helloworld11")
    sock.setsockopt_string(zmq.SUBSCRIBE, "helloworld12")
    sock.setsockopt_string(zmq.SUBSCRIBE, "eosio")

    # url='http://127.0.0.1'
    # sock.bind(url)
    sock.connect ("ipc:///tmp/0")

    while True:
        contract, msg = await sock.recv_multipart() # waits for msg to be ready
        logger.info((contract, msg))
        a = json.loads(msg)

        trx_id = a['trx_id']
        block_num = a['block_num']
        logger.info((trx_id, block_num))
        trx = eosapi.get_transaction(trx_id, block_num)
        packed_trx = trx['trx']['receipt']['trx'][1]
        logger.info(packed_trx)
        # trx = trx['trx']['trx']
        # logger.info(trx._dict)
        # trx = json.dumps(trx._dict)
        # logger.info(trx)
        # trx = eosapi.pack_transaction(trx)

    # reply = await async_process(msg)
    # await sock.send_multipart(reply)

asyncio.get_event_loop().run_until_complete(recv_and_process())




eosapi.set_filter_on('eosio.token::')

port = "5556"
if len(sys.argv) > 1:
    port =  sys.argv[1]
    int(port)
    
if len(sys.argv) > 2:
    port1 =  sys.argv[2]
    int(port1)

# Socket to talk to server
context = zmq.Context()
socket = context.socket(zmq.SUB)

print("Collecting updates from weather server...")
#socket.connect ("tcp://localhost:%s" % port)
socket.connect ("ipc:///tmp/0")

if len(sys.argv) > 2:
    socket.connect ("tcp://localhost:%s" % port1)


# Subscribe to zipcode, default is NYC, 10001
topicfilter = "2222"
#socket.setsockopt_string(zmq.SUBSCRIBE, topicfilter)
socket.setsockopt_string(zmq.SUBSCRIBE, "eosio.token")
socket.setsockopt_string(zmq.SUBSCRIBE, "helloworld11")
socket.setsockopt_string(zmq.SUBSCRIBE, "helloworld12")
socket.setsockopt_string(zmq.SUBSCRIBE, "eosio")


# Process 5 updates
total_value = 0
#for update_nbr in range (5):
while True:
    string = socket.recv_string()
    action = socket.recv_string()
    a = json.loads(action)
    if a['act']['account'] == 'eosio' and a['act']['name'] == 'onblock':
        continue
    print(string, action)

    logger.info((a['trx_id'], a['block_num']))
    trx = eosapi.get_transaction(a['trx_id'], a['block_num'])
    print(trx)
    logger.info(trx['trx'])

    if a['act']['name'] == 'transfer':
        d = a['act']['data']
#        print(d)
        d = bytes.fromhex(d)
        try:
            s = eosapi.unpack_args('eosio.token', 'transfer', d)
            print(s)
        except Exception as e:
            logger.exception(e)
            try:
                s = eosapi.unpack_args('eosio.token', 'transfer', d[:32]+b'\x00')
                print(s)
            except Exception as e:
                print(e)
    else:
        print(a['act'])
#    string = string.decode('utf8')
#    topic, messagedata = string.split()
#    total_value += int(messagedata)
#    print(topic, messagedata)

#print("Average messagedata value for topic '%s' was %dF" % (topicfilter, total_value / update_nbr))