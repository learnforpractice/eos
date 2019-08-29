#include "chain_api.hpp"

[[ noreturn ]] void chain_throw_exception(int type, const char* fmt, ...) {
   std::vector<char> output(1024);
   memset(output.data(), 0, output.size());
   va_list args;
   va_start(args, fmt);
   int len = vsnprintf(output.data(), output.size()-1, fmt, args);
   va_end(args);
   get_chain_api()->throw_exception(type, output.data());
}
