from .connection import Connection
class P2pManager(object):
    
    def __init__(self, config):
        self.config = config
        self.connections = set()

    def add(self, c):
        self.connections.add(c)
    
    def remove(self, c):
        self.connections.remove(c)

    def broadcast_trx(self, trx):
        for c in self.connections:
            c.send_transaction(trx)
    
    def broadcast_block(self, block):
        for c in self.connections:
            c.send_block(block)

    async def analyze_peer(self):
        peers = '''
        p2p-peer-address = api-full1.eoseoul.io:9876
        p2p-peer-address = api-full2.eoseoul.io:9876
        p2p-peer-address = boot.eostitan.com:9876
        p2p-peer-address = bp.cryptolions.io:9876
        p2p-peer-address = bp.eosbeijing.one:8080
        p2p-peer-address = bp.libertyblock.io:9800
        p2p-peer-address = br.eosrio.io:9876
        p2p-peer-address = eos-seed-de.privex.io:9876
        p2p-peer-address = fullnode.eoslaomao.com:443
        p2p-peer-address = mainnet.eoscalgary.io:5222
        p2p-peer-address = node.eosflare.io:1883
        p2p-peer-address = node1.eoscannon.io:59876
        p2p-peer-address = node1.eosnewyork.io:6987
        p2p-peer-address = p2p.eosdetroit.io:3018
        p2p-peer-address = p2p.genereos.io:9876
        p2p-peer-address = p2p.meet.one:9876
        p2p-peer-address = peer.eosn.io:9876
        p2p-peer-address = peer.main.alohaeos.com:9876
        p2p-peer-address = peer1.mainnet.helloeos.com.cn:80
        p2p-peer-address = peer2.mainnet.helloeos.com.cn:80
        p2p-peer-address = peering.mainnet.eoscanada.com:9876
        p2p-peer-address = peering1.mainnet.eosasia.one:80
        p2p-peer-address = peering2.mainnet.eosasia.one:80
        p2p-peer-address = publicnode.cypherglass.com:9876
        p2p-peer-address = seed1.greymass.com:9876
        p2p-peer-address = seed2.greymass.com:9876
        '''
        peers = '''
        p2p-peer-address = peer.main.alohaeos.com:9876
        p2p-peer-address = peer.eos-mainnet.eosblocksmith.io:5010
        p2p-peer-address = eos-p2p.slowmist.io:9876
        p2p-peer-address = peer1.swisseos.com:9876
        p2p-peer-address = mainnet.get-scatter.com:9876
        '''
        peers = peers.split('\n')
        self.delays = []
        import time
        async def check_time(self, host, port):
            try:
                start = time.time()
                c = Connection(host, port)
                ret = await c.connect()
                if not ret:
                    return
                c.send_handshake()
                ret = await c.handle_message()
                c.close()
                if not ret:
                    return
                self.delays.append((time.time() - start, host, port))
                self.delays.sort(key = lambda x: x[0])
                print(self.delays)
            except Exception as e:
                logger.exception(e)
        tasks = []
        for peer in peers:
            peer = peer.split(' = ')
            if len(peer) >= 2:
                peer = peer[1]
                host, port = peer.split(':')
                print(host, port)
                task = asyncio.create_task(check_time(self, host, port))
                tasks.append(task)
        for task in tasks:
            await task
        print(self.delays)