import os
import _uuos
import sys
import time
import signal
import asyncio
import aioconsole
import uuos

def shutdown():
    _uuos.uuos_shutdown2()
    sys.exit(0)

async def interactive_console():
    await aioconsole.interact(handle_sigint=False, banner=False)

def uuos_main():
    print('uuos_main started!')
    ret = _uuos.uuos_init(sys.argv)
    if not 0 == ret:
        sys.exit(ret)

    uuos.init()
    
    _uuos.uuos_exec()
#        _uuos.uuos_exec_one()
    print('++++++++++++uuos_main exit!')

async def init_signal():
    # task = loop.run_in_executor(None, uuos_main)
    # tasks.append(task)

    signals = (signal.SIGHUP, signal.SIGTERM, signal.SIGINT)
    for s in signals:
        loop.add_signal_handler(s, shutdown)

async def main(loop):
    tasks = []
    task = asyncio.create_task(interactive_console())
    tasks.append(task)

    # task = asyncio.create_task(uuos_main())
    # tasks.append(task)

    task = loop.run_in_executor(None, uuos_main)
    tasks.append(task)

    task = asyncio.create_task(init_signal())
    tasks.append(task)

    task = asyncio.create_task(aioconsole.start_interactive_server(host='localhost', port=8800))
    tasks.append(task)

    res = await asyncio.gather(*tasks, return_exceptions=False)

print(os.getpid())
# input('<<<')

_uuos.uuos_set_log_level('default', 10)
loop = asyncio.get_event_loop()

loop.run_until_complete(main(loop))



