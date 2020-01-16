#include <string>
using namespace std;

void string_to_float128_(string& s, string& result) {
    long double d = std::strtold(s.c_str(), nullptr);
    result = string((char *)&d, sizeof(long double));
}

void float128_to_string_(string& s, string& result) {
    result = std::to_string(*((long double*)s.c_str()));
}
