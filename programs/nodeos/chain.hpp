#pragma once
#include <string>
using namespace std;
void *chain_new_(string& config, string& protocol_features_dir);
void chain_free_(void *ptr);
