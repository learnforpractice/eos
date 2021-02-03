/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#ifndef __TYPES_H
#define __TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

/**
 *  @defgroup c_types
 *  @ingroup c_api
 *  @brief Specifies builtin types, typedefs and aliases
 */

/**
 *  @addtogroup c_types
 *  @brief Specifies builtin types, typedefs and aliases
 *  @{
 */

/**
 * Macro to align/overalign a type to ensure calls to intrinsics with pointers/references are properly aligned
 */

/* macro to align/overalign a type to ensure calls to intrinsics with pointers/references are properly aligned */
#define ALIGNED(X) __attribute__ ((aligned (16))) X

typedef uint64_t capi_name;

/**
 * EOSIO Public Key. K1 and R1 keys are 34 bytes.  Newer keys can be variable-sized
 */
struct __attribute__((deprecated("newer public key types cannot be represented as a fixed size structure", "char[]")))
capi_public_key {
   char data[34];
};

/**
 * EOSIO Signature. K1 and R1 signatures are 66 bytes. Newer signatures can be variable-sized
 */
struct __attribute__((deprecated("newer signature types cannot be represented as a fixed size structure", "char[]")))
capi_signature {
   uint8_t data[66];
};

/**
 * 256-bit hash
 */
struct ALIGNED(capi_checksum256) {
   uint8_t hash[32];
};

/**
 * 160-bit hash
 */
struct ALIGNED(capi_checksum160) {
   uint8_t hash[20];
};

/**
 * 512-bit hash
 */
struct ALIGNED(capi_checksum512) {
   uint8_t hash[64];
};


#ifdef __WASM
   #define VM_API  __attribute__((eosio_wasm_import))
#else
   #define VM_API __attribute__ ((visibility ("default")))
#endif

#ifdef __NATIVE
   #define VM_API_NO_RETURN  __attribute__((eosio_wasm_import, noreturn))
#else
   #define VM_API_NO_RETURN __attribute__ ((visibility ("default"), noreturn))
#endif

typedef unsigned __int128 __uint128;
typedef unsigned __int128 uint128_t;
typedef __int128 int128_t;

/// @}

#endif
