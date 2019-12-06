#include <string>
using namespace std;

typedef int (*fn_on_accepted_block)(string& packed_block, uint32_t num, string& block_id);
void register_on_accepted_block(fn_on_accepted_block cb);
int on_accepted_block(string& packed_block, uint32_t num, string& block_id);

void register_on_accepted_block_cb_() {
    register_on_accepted_block(on_accepted_block);
}
