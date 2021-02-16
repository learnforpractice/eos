#include "vm_api4c.hpp"
#include <vm_api_proxy.hpp>

void assert_sha256( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum256 *hash = (struct capi_checksum256 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum256));
   get_vm_api()->assert_sha256(data, length, hash );
}

void assert_sha1( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum160 *hash = (struct capi_checksum160 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum160));
   get_vm_api()->assert_sha1(data, length, hash );
}

void assert_sha512( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum512 *hash = (struct capi_checksum512 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum512));
   get_vm_api()->assert_sha512(data, length, hash );
}

void assert_ripemd160( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum160 *hash = (struct capi_checksum160 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum160));
   get_vm_api()->assert_ripemd160(data, length, hash );
}

void sha256( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum256 *hash = (struct capi_checksum256 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum256));
   get_vm_api()->sha256(data, length, hash );
}

void sha1( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum160 *hash = (struct capi_checksum160 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum160));
   get_vm_api()->sha1(data, length, hash );
}


void sha512( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum512 *hash = (struct capi_checksum512 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum512));
   get_vm_api()->sha512(data, length, hash );
}

void ripemd160( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct capi_checksum160 *hash = (struct capi_checksum160 *)offset_to_ptr(hash_offset, sizeof(struct capi_checksum160));
   get_vm_api()->ripemd160(data, length, hash );
}

u32 recover_key( u32 digest_offset, u32 sig_offset, u32 siglen, u32 pub_offset, u32 publen ) {
   const struct capi_checksum256* digest = (const struct capi_checksum256*)offset_to_ptr(digest_offset, sizeof(struct capi_checksum256));
   const char* sig = (char *)offset_to_ptr(sig_offset, siglen);
   char* pub = (char *)offset_to_ptr(pub_offset, publen);
   return get_vm_api()->recover_key(digest, sig, siglen, pub, publen);
}

void assert_recover_key( u32 digest_offset, u32 sig_offset, u32 siglen, u32 pub_offset, u32 publen ) {
   const struct capi_checksum256* digest = (const struct capi_checksum256*)offset_to_ptr(digest_offset, sizeof(struct capi_checksum256));
   const char* sig = (char *)offset_to_ptr(sig_offset, siglen);
   char* pub = (char *)offset_to_ptr(pub_offset, publen);
   get_vm_api()->assert_recover_key(digest, sig, siglen, pub, publen);
}

