import sys
import json

import zmq
from pyeoskit import eosapi

eosapi.set_filter_on('*')
# eosapi.set_filter_on('learnfortest1::')
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
# socket.setsockopt_string(zmq.SUBSCRIBE, "learnfortest1")
# socket.setsockopt_string(zmq.SUBSCRIBE, "helloworld11")
# socket.setsockopt_string(zmq.SUBSCRIBE, "helloworld12")
# socket.setsockopt_string(zmq.SUBSCRIBE, "eosio")


# Process 5 updates
total_value = 0
#for update_nbr in range (5):
while True:
    string = socket.recv_string()
    action = socket.recv_string()
    a = json.loads(action)
    print('++++++++++++:', string, a['receiver'], a['receipt']['receiver'])
    if a['act']['name'] == 'transfer':
        d = a['act']['data']
#        print(d)
        d = bytes.fromhex(d)
        try:
            s = eosapi.unpack_args('eosio.token', 'transfer', d)
            print(s)
        except Exception as e:
            print(e)
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