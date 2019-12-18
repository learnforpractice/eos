import asyncio

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
        hub.subscriptions.add(self.queue)
        return self.queue

    def __exit__(self, type, value, traceback):
        hub.subscriptions.remove(self.queue)

class Message(object):

    def __init__(self, obj):
        self.obj = obj
        self.event = asyncio.Event()

    def get_message(self):
        return self.obj

    def notify(self):
        self.event.set()

    async def wait(self):
        await self.event.wait()

class Application(object):
    def __init__(self):
        self.transaction_hub = Hub()
        self.task = asyncio.create_task(self.handle_transaction())

    async def handle_transaction(self):
        with Subscription(hub) as queue:
            while True:
                msg = await queue.get()
                for c in self.connections:
                    c.send_transaction(msg)

app = None

def set_app(_app):
    global app
    app = _app

def get_app():
    if not app:
        raise Exception("application not initialized")
    return app

__all__ = {
    'Message',
    'Application',
    'app',
    'set_app',
    'get_app'
}

