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

class Application(object):

    def __init__(self):
        pass

__all__ = {
    'Application',
    'app',
    'set_app',
    'get_app'
}

