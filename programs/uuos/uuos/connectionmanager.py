from .connection import Connection
class ConnectionManager(object):
    
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

