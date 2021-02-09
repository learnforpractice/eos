import os
import json
import time
import hashlib
import argparse
import requests
import requests_unixsocket

from flask import Flask,redirect,request
from uuoskit import uuosapi, wallet
import traceback
from flask_cors import CORS

import logging

MAX_ACCOUNT_NUM = 20


logger = None
#TEST=False
TEST=False
USE_UNIX_SOCKET = False

if os.path.exists('test.wallet'):
    os.remove('test.wallet')
wallet.create('test')
#wallet.import_key('test', PRIV_KEY)
'''
{'public': 'EOS8FmNcTG1bCjPKXKjQvwRzjM4aECBrBWFQExm3513tdN36K1c1U',
 'private': '5JW8qfLKQzZHbdSfUTvd9zZHMGokniZ5oWmPZigqgGY4UDB14of'}
'''

#PUB_KEY = 'EOS8FmNcTG1bCjPKXKjQvwRzjM4aECBrBWFQExm3513tdN36K1c1U'
#wallet.import_key('test', '5JW8qfLKQzZHbdSfUTvd9zZHMGokniZ5oWmPZigqgGY4UDB14of')

if TEST:
    PUB_KEY = 'EOS8FmNcTG1bCjPKXKjQvwRzjM4aECBrBWFQExm3513tdN36K1c1U'
    PRIV_KEY = '5JW8qfLKQzZHbdSfUTvd9zZHMGokniZ5oWmPZigqgGY4UDB14of'
else:
    PUB_KEY = 'EOS5nJRbFwn6Rqcmihf4ZukCE5bWV73knUQAFB8Yyfkih4tk7afWn'
    PRIV_KEY = '5JkRtzuX6JBbfnxkf9iRrqYonhQEaqqCMgRThJpxMsSnAxqBzkJ'
wallet.import_key('test', PRIV_KEY)
wallet.import_key('test', '5JfRVv4vDLNz8EtBofUe1STHdPE9NoEoEEKm9Pdm28RhRtsD9wL')

headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}

VER_PUB_KEY = 'EOS8DuWFav9Eo8gt3ettbjZmMEEPbuJWbvuhx3ECu7Kk6TAekqS9V'

app = Flask(__name__)

#cors = CORS(app, resources={r"/v1/*": {"origins": "https://uuos.network"}})
cors = CORS(app, resources={r"/v1/*": {"origins": "*"}})

#url = 'http://127.0.0.1:8889'
if TEST:
    PORT = '9001'
else:
    PORT = '8888'
url = 'http://127.0.0.1:'+PORT

if USE_UNIX_SOCKET:
   url = '/home/newworld/dev/uuosmainnet/dd/uuos.sock'
   url = url.replace('/', '%2F')
   url = 'unix://'+url

uuosapi.set_node(url)

if USE_UNIX_SOCKET:
    url = 'http+'+url
#url = 'http://120.76.57.92'

session = requests_unixsocket.Session()

@app.route('/v1/<api>/<endpoint>', methods=['GET', 'POST'])
def profile(api, endpoint):
#    print('json:', request.json)
#    print('form:', request.form)
    logger.info(uuosapi.node_url)
    url = uuosapi.node_url
    try:
        print( time.ctime())
        if api in ['net', 'producer']:
            return "Not found", 404
        logger.info(f"{api} {endpoint} {request.method}")
        if request.environ.get('HTTP_X_FORWARDED_FOR'):
            print('HTTP_X_FORWARDED_FOR:', request.environ['HTTP_X_FORWARDED_FOR'])
        else:
            print('REMOTE_ADDR:', request.environ['REMOTE_ADDR'])
        # logger.info(request.headers)

        if request.method == "GET":
            r = session.get(url + f'/v1/{api}/{endpoint}', headers=headers)
        else: #request.method == "POST":
            js =  request.get_data()
            # js =  request.get_json(force=True)
            logger.info(f'++++++++++js:{js}')
            # js = json.dumps(js)
            r = session.post(url + f'/v1/{api}/{endpoint}', data=js, headers=headers)
            if not api in ['get_info', 'get_table_rows']:
                logger.info(r.text)
        logger.info(r.text)
        return r.text, r.status_code
    except Exception as e:
        logger.exception(e)
        return "Not found", 404

if __name__ == '__main__':
#    app.run(debug=True, host='127.0.0.1', port=8899)
#    app.run(debug=True, host='127.0.0.1', port=8891)
#    app.run(debug=False, host='127.0.0.1', port=8891)
    parser = argparse.ArgumentParser(description='uuos proxy')
    parser.add_argument('--debug', type=bool, default=False, help='set to True to enable debug')
    parser.add_argument('--http-server-address', type=str, default='http://127.0.0.1:8888', help='node rpc url')
    parser.add_argument('--host', type=str, default='127.0.0.1', help='host address')
    parser.add_argument('--port', type=int, default=8891, help='port number')
    parser.add_argument('--logfile', type=str, default='logfile.log', help='log file')
    args = parser.parse_args()

    logging.basicConfig(filename=args.logfile, level=logging.INFO, 
                        format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
    logger=logging.getLogger(__name__)

    handler = logging.StreamHandler()
    formatter = logging.Formatter('%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)

    uuosapi.set_node(args.http_server_address)
    app.run(debug=args.debug, host=args.host, port=args.port)
#    app.run(debug=args.debug, host=args.host, port=args.port, ssl_context='adhoc')
