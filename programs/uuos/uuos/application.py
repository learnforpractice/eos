import asyncio
import logging

logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

app = None

def set_app(_app):
    global app
    app = _app

def get_app():
    if not app:
        raise Exception("application not initialized")
    return app

class Hub(object):

    def __init__(self):
        self.subscriptions = set()

    def publish(self, message):
        for queue in self.subscriptions:
            queue.put_nowait(message)

class Subscription(object):

    def __init__(self, hub):
        self.hub = hub
        self.queue = asyncio.Queue()

    def __enter__(self):
        self.hub.subscriptions.add(self.queue)
        return self.queue

    def __exit__(self, type, value, traceback):
        self.hub.subscriptions.remove(self.queue)

class Message(object):
    type_transaction = 0
    type_raw_transaction = 1
    def __init__(self, message_type, data):
        self.type = message_type
        self.data = data
        self.event = asyncio.Event()
        self.result = None

    def get_data(self):
        return self.data

    def publish(self):
        get_app().publish_message(self)

    def notify(self, result):
        self.result = result
        self.event.set()

    async def wait(self):
        await self.event.wait()
        return self.result

class TransactionMessage(Message):

    def __init__(self, packed_trx):
        super().__init__(Message.type_transaction, packed_trx)
        get_app().publish_message(self)

class RawTransactionMessage(Message):

    def __init__(self, packed_trx):
        super().__init__(Message.type_raw_transaction, packed_trx)
        get_app().publish_message(self)


class Application(object):

    def __init__(self):
        self.hub = Hub()
        self.task = asyncio.create_task(self.handle_message())

    def publish_message(self, msg):
        self.hub.publish(msg)

    async def handle_message(self):
        with Subscription(self.hub) as queue:
            while True:
                msg = await queue.get()
                try:
                    if msg.type == Message.type_transaction:
                        self.producer.start_block()
                        ret, result, raw_packed_trx = self.producer.process_incomming_transaction(msg.data.decode('utf8'))
                        #TODO: check failure of process transaction
                        msg.notify(result)
                        # continue
                        if ret == 0:
                            continue
                        for c in self.connections:
                            c.send_transaction(raw_packed_trx)
                    elif msg.type == Message.type_raw_transaction:
                        self.producer.start_block()
                        ret, result = self.producer.process_raw_transaction(msg.data)
                        msg.notify(result)
                        if ret == 0:
                            continue
                        for c in self.connections:
                            c.send_transaction(msg.data)
                except Exception as e:
                    logger.exception(e)
        logger.info('+++++++++++++handle message returned!')
        raise Exception('handle message return')
__all__ = {
    'Message',
    'TransactionMessage',
    'Application',
    'app',
    'set_app',
    'get_app'
}

