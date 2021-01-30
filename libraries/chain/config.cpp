namespace eosio { namespace chain { namespace config {

static int      block_interval_ms = 1000;

int get_block_interval_ms() {
    return block_interval_ms;
}

void set_block_interval_ms(int ms) {
    block_interval_ms = ms;
}

int get_block_interval_us() {
    return block_interval_ms*1000;
}

}}}
