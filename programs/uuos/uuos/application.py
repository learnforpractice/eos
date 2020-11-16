import asyncio
import logging
from . import log

app = None

def get_logger(name):
    return log.get_logger(name)

def set_app(_app):
    global app
    app = _app

def get_app():
    if not app:
        raise Exception("application not initialized")
    return app

class Application(object):

    def __init__(self):
        pass

    def get_p2p_manager(self):
        pass

__all__ = {
    'Application',
    'app',
    'set_app',
    'get_app'
}

