#pragma once
#include <string>
using namespace std;
void *chain_new_(string& config, string& protocol_features_dir);
void chain_free_(void *ptr);
void chain_on_incoming_block_(void *ptr, string& packed_signed_block, uint32_t& num, string& id);