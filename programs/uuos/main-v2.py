import _uuos
import sys
import signal
import asyncio
import aioconsole

ret = _uuos.uuos_init(sys.argv)
if not 0 == ret:
    sys.exit(ret)

def shutdown():
    _uuos.uuos_shutdown2()
    sys.exit(0)


async def interactive_console():
    await aioconsole.interact(handle_sigint=False, banner=False)

async def uuos_main():
    while True:
        if 0 == _uuos.uuos_exec_one():
            await asyncio.sleep(0)
        else:
            await asyncio.sleep(0.1)

async def main(loop):
    tasks = []
    task = asyncio.create_task(interactive_console())
    tasks.append(task)

    task = asyncio.create_task(uuos_main())
    tasks.append(task)

    signals = (signal.SIGHUP, signal.SIGTERM, signal.SIGINT)
    for s in signals:
        loop.add_signal_handler(s, shutdown)

    res = await asyncio.gather(*tasks, return_exceptions=False)

_uuos.uuos_set_log_level('default', 10)
loop = asyncio.get_event_loop()

loop.run_until_complete(main(loop))



