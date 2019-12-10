networks = ("api-full1.eoseoul.io:9876",
"api-full2.eoseoul.io:9876",
"boot.eostitan.com:9876",
"bp.cryptolions.io:9876",
"bp.eosbeijing.one:8080",
"bp.libertyblock.io:9800",
"br.eosrio.io:9876",
"eos-seed-de.privex.io:9876",
"eu1.eosdac.io:49876",
"fullnode.eoslaomao.com:443",
"mainnet.eoscalgary.io:5222",
"node.eosflare.io:1883",
"node1.eoscannon.io:59876",
"node1.eosnewyork.io:6987",
"node2.eosnewyork.io:6987",
"p2p.eosdetroit.io:3018",
"p2p.genereos.io:9876",
"p2p.meet.one:9876",
"peer.eosn.io:9876",
"peer.main.alohaeos.com:9876",
"peer1.mainnet.helloeos.com.cn:80",
"peer2.mainnet.helloeos.com.cn:80",
"peering.mainnet.eoscanada.com:9876",
"peering1.mainnet.eosasia.one:80",
"peering2.mainnet.eosasia.one:80",
"publicnode.cypherglass.com:9876",
"seed1.greymass.com:9876",
"seed2.greymass.com:9876",
)
import time
import asyncio
import traceback
from uuos.connection import Connection

async def connect_to_peer(host, port, costs):
    try:
#        print(host, port)
        start = time.time()
        con =  asyncio.open_connection(host, port, limit=1024*1024)
        reader, writer = await asyncio.wait_for(con, timeout=5)
        cost = time.time()-start
        print('connect to', host, port, 'cost', cost)
        costs.append([cost, host, port])
        # c = Connection(reader, writer, None)
        # c.send_handshake()
        # await c.handle_message()
        print('connect to ', host , port, 'succeeded')
    except asyncio.TimeoutError:
        return
    except Exception as e:
        traceback.print_exc(e)

async def main():
    tasks = []
    costs = []
    for network in networks:
        host, port = network.split(':')
        port = int(port)
        task = asyncio.create_task(connect_to_peer(host, port, costs))
        tasks.append(task)
    await asyncio.gather(*tasks)
    costs.sort(key=lambda x: x[0], reverse=False)
    for cost in costs:
        print(cost)
    print('done!')

#loop = asyncio.get_event_loop()
asyncio.run(main())



'''
[0.31722116470336914, 'fullnode.eoslaomao.com', 443]
[0.31746721267700195, 'p2p.meet.one', 9876]
[0.3175480365753174, 'mainnet.eoscalgary.io', 5222]
[0.3175690174102783, 'node.eosflare.io', 1883]
[0.3176701068878174, 'peer2.mainnet.helloeos.com.cn', 80]
[0.3177070617675781, 'peer1.mainnet.helloeos.com.cn', 80]
[0.3177812099456787, 'publicnode.cypherglass.com', 9876]
[0.3178269863128662, 'peering2.mainnet.eosasia.one', 80]
[0.31786108016967773, 'peering1.mainnet.eosasia.one', 80]
[0.3178701400756836, 'p2p.genereos.io', 9876]
[0.31787800788879395, 'node1.eoscannon.io', 59876]
[0.3182840347290039, 'api-full2.eoseoul.io', 9876]
[0.31836891174316406, 'api-full1.eoseoul.io', 9876]
[0.3185548782348633, 'boot.eostitan.com', 9876]
[0.3293948173522949, 'eos-seed-de.privex.io', 9876]
[0.33725404739379883, 'bp.cryptolions.io', 9876]
[0.36600399017333984, 'node2.eosnewyork.io', 6987]
[0.3676159381866455, 'peer.eosn.io', 9876]
[0.3726069927215576, 'node1.eosnewyork.io', 6987]
[0.37267613410949707, 'peering.mainnet.eoscanada.com', 9876]
[0.3730030059814453, 'p2p.eosdetroit.io', 3018]
[0.3918418884277344, 'peer.main.alohaeos.com', 9876]
[0.41058993339538574, 'eu1.eosdac.io', 49876]
[0.7099277973175049, 'br.eosrio.io', 9876]
[1.2229151725769043, 'bp.libertyblock.io', 9800]
[2.1212987899780273, 'bp.eosbeijing.one', 8080]
'''
