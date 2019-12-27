import asyncio
import logging

logging.basicConfig(filename='logfile.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
formatter = logging.Formatter('%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
handler = logging.StreamHandler()
handler.setFormatter(formatter)

def get_logger(name):
    logger=logging.getLogger(name)
    logger.addHandler(handler)
    return logger

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

    def get_connection_manager(self):
        pass

__all__ = {
    'Application',
    'app',
    'set_app',
    'get_app'
}

