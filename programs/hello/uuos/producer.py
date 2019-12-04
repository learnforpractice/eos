import asyncio

class Producer(object):

    def get_sleep_time(self):
        return 1.0

    def start_produce_block(self):
        pass

    def end_produce_block(self):
        pass

    def can_produce_block(self):
        return False

    def on_incoming_block(self, block):
        pass

    async def run(self):
        while True:
            if not self.can_produce_block():
                await asyncio.sleep(0.2)

            self.start_produce_block()
            sleep_time = self.get_sleep_time()
            await asyncio.sleep(sleep_time)
            self.end_produce_block()

