
#include <stdlib.h>
#include <stdint.h>

#include <eosiolib_native/vm_api.h>
#include "vm_defines.h"

static void __ashlti3(__int128& ret, uint64_t low, uint64_t high, uint32_t shift) {
    get_vm_api()->__ashlti3(&ret, low, high, shift);
}

static void __multi3(__int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
    __int128 lhs = ha;
    __int128 rhs = hb;

    lhs <<= 64;
    lhs |=  la;

    rhs <<= 64;
    rhs |=  lb;

    lhs *= rhs;
    ret = lhs;
}

static void __udivti3(unsigned __int128& ret, uint64_t la, uint64_t ha, uint64_t lb, uint64_t hb) {
    unsigned __int128 lhs = ha;
    unsigned __int128 rhs = hb;

    lhs <<= 64;
    lhs |=  la;

    rhs <<= 64;
    rhs |=  lb;

    get_vm_api()->eosio_assert(rhs != 0, "divide by zero");

    lhs /= rhs;
    ret = lhs;
}

/* import: 'env' '__ashlti3' */
extern "C" {

void Z_envZ___ashlti3Z_vijji(u32 ret, u64 low, u64 high, u32 shift) {
   __int128* _ret = (__int128 *)offset_to_ptr(ret, sizeof(unsigned __int128));
   __ashlti3(*_ret, low, high, shift);
}

/* import: 'env' '__multi3' */
void Z_envZ___multi3Z_vijjjj(u32 ret, u64 la, u64 ha, u64 lb, u64 hb) {
   __int128* _ret = (__int128 *)offset_to_ptr(ret, sizeof(unsigned __int128));
   __multi3(*_ret, la, ha, lb, hb);
}

/* import: 'env' '__udivti3' */
void Z_envZ___udivti3Z_vijjjj(u32 ret, u64 la, u64 ha, u64 lb, u64 hb) {
   unsigned __int128* _ret = (unsigned __int128 *)offset_to_ptr(ret, sizeof(unsigned __int128));
   __udivti3(*_ret, la, ha, lb, hb);
}

}