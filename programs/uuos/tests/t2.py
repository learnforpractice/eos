import logging
import argparse

logging.basicConfig(filename='logfile.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

# parser = argparse.ArgumentParser()
# parser.add_argument('--data-dir',               type=str, default='dd',                  help='data directory')
# parser.add_argument("echo")

configs = r'''
data-dir = dd \n
#p2p-server-address = <domain-name>:9000
'''
logger.info(configs.split('\n'))

parser = argparse.ArgumentParser()
parser.add_argument('--data-dir',               type=str, default='dd',                  help='data directory')
args = parser.parse_args(configs)
logger.info(parser)
print(args.data_dir)
