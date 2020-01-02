from _uuos import set_accepted_block_callback

import asyncio
import ujson as json
from .connection import Connection
from .nativeobject import HandshakeMessage
from . import application

logger=application.get_logger(__name__)

keep_alive_interval = 32

class P2pManager(object):
    
    def __init__(self, config):
        self.config = config
        self.connections = set()
        set_accepted_block_callback(self.on_accepted_block)
        self.monitor_task = asyncio.create_task(self.monitor())
        self.client_count = 0

    async def monitor(self):
        counter = 0
        while True:
            try:
                await asyncio.sleep(1.0)
                counter += 1
                # if counter >= keep_alive_interval:
                #     counter = 0
                #     for c in self.connections:
                #         if not c.closed:
                #             c.send_time_message()

    #            logger.info('++++++check connection')
                timeout_connections = set()
                for c in self.connections:
                    c.time_counter -= 1
                    # logger.info(f'++++++time_counter {c.time_counter}')
                    if c.time_counter > 0:
                        continue
                    #too long to not receive a message, consider it as a dead connection
                    c.close()
                    timeout_connections.add(c)

                for c in timeout_connections:
                    if c.client_mode:
                        logger.info(f'reconnec to {c.host}:{c.port}')
                        new_connection = Connection(c.host, c.port)
                        ret = await new_connection.connect()
                        if not ret:
                            continue
                        ret = await new_connection.start()
                        if not ret:
                            continue
                        self.add(new_connection)
                        self.connections.remove(c)
                    else:
                        self.connections.remove(c)
            except Exception as e:
                logger.exception(e)

    def get_connections(self):
        ret = {}
        for c in self.connections:
            ret['peer'] = f'{c.host}:{c.port}'
        return json.dumps(ret)

    def get_net_status(self, peer):
        try:
            host, port = peer.split(':')
            for c in self.connections:
                if c.host == host and c.port == port:
                    return c.status()
        except Exception as e:
            logger.exception(e)
        return None

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
                msg_type, msg = await c.read_message()
                ret = await c.handle_message(msg_type, msg)
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


    async def handle_connection(self, c):
        try:
            await c.handle_message_loop()
        except Exception as e:
            logger.exception(e)
        finally:
            self.client_count -= 1

    async def handle_p2p_client(self, reader, writer):
        addr = writer.get_extra_info('peername')
        print(f"connection from {addr}")
        print(f"connection from {addr} {self.config.max_clients} {self.client_count}")
        if self.config.max_clients and self.client_count > self.config.max_clients:
            writer.close()
            return
        self.client_count += 1

        if self.config.allowed_connection == 'any': #specified any
            pass

        try:
            host = writer.get_extra_info('peername')
            logger.info(f'++++host:{host}')
            sock = writer.get_extra_info('socket')
            if sock is not None:
                logger.info(f'++++++++++sock.getsockname: {sock.getsockname()}')
            c = Connection(host[0], host[1], False)
            c.reader = reader
            c.writer = writer

            msg_type, msg = await asyncio.wait_for(c.read_message(), timeout=5.0)
            print(msg_type, msg)
            if msg is None or msg_type is None:
                return
            if not msg_type == 0:
                logger.info('bad message, not handshake message')
                return
            handshake_message = HandshakeMessage.unpack(msg)
            if handshake_message.generation == 1:
                pass
            #TODO verify handshake message
            c.send_handshake()
            if not await c.handle_message(msg_type, msg):
                return
            self.add(c)
            task = asyncio.create_task(self.handle_connection(c))

        except asyncio.TimeoutError:
            logger.info('timeout!')
            return
        except Exception as e:
            logger.exception(e)

    async def p2p_server(self):
        address, port = self.config.p2p_listen_endpoint.split(':')
        port = int(port)
        logger.info(f'listen p2p connection on {address}:{port}')
        server = await asyncio.start_server(self.handle_p2p_client, address, port)
        addr = server.sockets[0].getsockname()
        print(f'Serving on {addr}')
        async with server:
            await server.serve_forever()

    async def connect_to_peers(self):
        logger.info(self.config.p2p_peer_address)
        for address in self.config.p2p_peer_address:
            try:
                logger.info(address)
                host, port = address.split(':')
                c = Connection(host, port)
                ret = await c.connect()
                if not ret:
                    continue
                ret = await c.start()
                if ret:
                    self.add(c)
            except ConnectionResetError as e:
                print(e)
            except Exception as e:
                print(e)
#                logger.exception(e)

    def on_accepted_block(self, block, num, block_id):
        for c in self.connections:
            if not c.last_handshake:
                continue
            # if c.catch_up:
            c.reset_time_counter()
            c.send_block(block)

    def close(self):
        for c in self.connections:
            logger.info(f'close {c.host}')
            c.close()
