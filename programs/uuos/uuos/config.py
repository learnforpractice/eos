import argparse

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "1")

class Config(object):
    def __init__(self, config_file = None):
        parser = argparse.ArgumentParser(description='')
        parser.register('type','bool',str2bool) # add type keyword to registries
        parser.add_argument('--data-dir',               type=str, default='dd',                  help='data directory')
        parser.add_argument('--config-dir',             type=str, default='cd',                  help='config directory')
        parser.add_argument('--http-server-address',    type=str, default='127.0.0.1:8888',      help='http server address')

        #p2p
        parser.add_argument('--p2p-listen-endpoint',    type=str, default='127.0.0.1:9877',      help='p2p listen endpoint')
        parser.add_argument('--p2p-peer-address',       type=str, default=[], action='append',   help='p2p peer address')
        parser.add_argument('--network',                type=str, default='test',                help='network: uuos, eos, test')
        parser.add_argument('--max-clients',            type=int, default=25,                    help='Maximum number of clients from which connections are accepted, use 0 for no limit')
        parser.add_argument('--peer-private-key',       type=str, default='',                    help='peer private key')
        parser.add_argument('--peer-key',               type=str, default=[], action='append',   help='peer key')
        parser.add_argument('--p2p-max-nodes-per-host',   type=int, default=1,                   help ='Maximum number of client nodes from any single IP address')

        #chain
        parser.add_argument('--hard-replay-blockchain', default=False, action="store_true",      help='clear chain state database, recover as many blocks as possible from the block log, and then replay those blocks')
        parser.add_argument('--replay-blockchain',      default=False, action="store_true",      help='clear chain state database and replay all blocks')
        parser.add_argument('--fix-reversible-blocks',  default=False, action="store_true",      help='recovers reversible block database if that database is in a bad state')
        parser.add_argument('--uuos-mainnet',           type=str2bool, default=True,             help='uuos main network')
        parser.add_argument('--snapshot',               type=str,      default='',               help='File to read Snapshot State from')
        parser.add_argument('--snapshots-dir',          type=str,      default='snapshots',      help='the location of the snapshots directory (absolute path or relative to application data dir)')
        parser.add_argument('--chain-state-db-size-mb', type=int,      default=300,              help='the location of the snapshots directory (absolute path or relative to application data dir)')


        #producer
        parser.add_argument('-p', '--producer-name',    type=str, default=[], action='append',   help='ID of producer controlled by this node (e.g. inita; may specify multiple times)')
        parser.add_argument('-e', '--enable-stale-production',    default=False, action="store_true", help='Enable block production, even if the chain is stale.')


        self.args = parser.parse_args()
        print('++++peer key:', self.args.peer_key)
    #    print(self.args.data_dir, args.config_dir, args.http_server_address, args.p2p_listen_endpoint)
        print(self.args.p2p_peer_address)
        print(self.args.data_dir)
        print(self.args.uuos_mainnet)
        
        if not config_file:
            return

        with open(config_file, 'r') as f:
            configs = f.readlines()
            self.parse_config(configs)
    
    def get_config(self):
        return self.args

    def parse_config(self, configs):
        line = 0
        for config in configs:
            line += 1
            config_bk = config
            i = config.find('#')
            if i >= 0:
                config = config[:i]
            config = config.strip()
            if not config:
                continue
            index = config.find('=')
            if index < 0:
                raise Exception(f'bad config at line {line}: {config_bk}')
            name = config[:index]
            name = name.strip()
            if not name:
                raise Exception(f'bad config at line {line}')
            value = config[index+1:]
            print(name, value)
