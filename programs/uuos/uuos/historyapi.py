import _uuos
import os
import ujson as json
from .application import get_app
'''
struct history_plugin_options {
   string db_dir;
   uint64_t db_size_mb;
   vector<string> filter_on;
   vector<string> filter_out;
   bool filter_transfer;   
};
'''

default_history_options = dict(
    db_dir = '',
    db_size_mb = 300,
    filter_on = [],
    filter_out = [],
    filter_transfer = False,
)

class HistoryApi(object):
    def __init__(self):
        config = get_app().config
        history_db_dir = os.path.join(config.data_dir, 'history')
        if not os.path.exists(history_db_dir):
            os.mkdir(history_db_dir)
        cfg = dict()
        cfg['db_dir'] = history_db_dir
        if config.network == 'eos':
            if config.history_db_size_mb < 550:
                cfg['db_size_mb'] = 550
            else:
                cfg['db_size_mb'] = config.history_db_size_mb
        else:
            cfg['db_size_mb'] = config.history_db_size_mb

        cfg['filter_on'] = config.filter_on
        cfg['filter_out'] = config.filter_out
        cfg['filter_transfer'] = config.filter_transfer
        cfg = json.dumps(cfg)
        self.ptr = _uuos.history_new(get_app().chain.ptr, cfg)
        if not self.ptr:
            raise Exception('initialize history api failed!')

    def new(self, cfg):
        return _uuos.history_new(self.ptr, cfg)

    def startup(self):
        ret = _uuos.history_startup(self.ptr)
        if not ret:
            raise Exception('history startup failed!')

    def free(self):
        if self.ptr:
            print('free history begin')
            _uuos.history_free(self.ptr)
            print('free history end')
            self.ptr = 0

    def get_actions(self, param):
        return _uuos.history_get_actions(self.ptr, param)

    def get_transaction(self, param):
        return _uuos.history_get_transaction(self.ptr, param)

    def get_key_accounts(self, param):
        return _uuos.history_get_key_accounts(self.ptr, param)

    def get_key_accounts_ex(self, param):
        return _uuos.history_get_key_accounts_ex(self.ptr, param)

    def get_controlled_accounts(self, param):
        return _uuos.history_get_controlled_accounts(self.ptr, param)

    def get_db_size(self):
        return _uuos.history_get_db_size(self.ptr)
