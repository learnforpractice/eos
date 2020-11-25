extern "C" {
   /* import: 'env' 'assert_sha1' */
   void (*Z_envZ_assert_sha1Z_viii)(u32, u32, u32);
   /* import: 'env' 'assert_sha256' */
   void (*Z_envZ_assert_sha256Z_viii)(u32, u32, u32);
   /* import: 'env' 'assert_sha512' */
   void (*Z_envZ_assert_sha512Z_viii)(u32, u32, u32);
   /* import: 'env' 'assert_ripemd160' */
   void (*Z_envZ_assert_ripemd160Z_viii)(u32, u32, u32);
   /* import: 'env' 'sha1' */
   void (*Z_envZ_sha1Z_viii)(u32, u32, u32);
   /* import: 'env' 'sha256' */
   void (*Z_envZ_sha256Z_viii)(u32, u32, u32);
   /* import: 'env' 'sha512' */
   void (*Z_envZ_sha512Z_viii)(u32, u32, u32);
   /* import: 'env' 'ripemd160' */
   void (*Z_envZ_ripemd160Z_viii)(u32, u32, u32);
   /* import: 'env' 'recover_key' */
   u32 (*Z_envZ_recover_keyZ_iiiiii)(u32, u32, u32, u32, u32);
   /* import: 'env' 'assert_recover_key' */
   void (*Z_envZ_assert_recover_keyZ_viiiii)(u32, u32, u32, u32, u32);   
}

static void assert_sha256( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum256 *hash = (struct checksum256 *)offset_to_ptr(hash_offset, sizeof(struct checksum256));
   get_vm_api()->assert_sha256(data, length, hash );
}

static void assert_sha1( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum160 *hash = (struct checksum160 *)offset_to_ptr(hash_offset, sizeof(struct checksum160));
   get_vm_api()->assert_sha1(data, length, hash );
}

static void assert_sha512( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum512 *hash = (struct checksum512 *)offset_to_ptr(hash_offset, sizeof(struct checksum512));
   get_vm_api()->assert_sha512(data, length, hash );
}

static void assert_ripemd160( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum160 *hash = (struct checksum160 *)offset_to_ptr(hash_offset, sizeof(struct checksum160));
   get_vm_api()->assert_ripemd160(data, length, hash );
}

static void sha256( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum256 *hash = (struct checksum256 *)offset_to_ptr(hash_offset, sizeof(struct checksum256));
   get_vm_api()->sha256(data, length, hash );
}

static void sha1( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum160 *hash = (struct checksum160 *)offset_to_ptr(hash_offset, sizeof(struct checksum160));
   get_vm_api()->sha1(data, length, hash );
}


static void sha512( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum512 *hash = (struct checksum512 *)offset_to_ptr(hash_offset, sizeof(struct checksum512));
   get_vm_api()->sha512(data, length, hash );
}

static void ripemd160( u32 data_offset, u32 length, u32 hash_offset ) {
   const char* data = (char *)offset_to_ptr(data_offset, length);
   struct checksum160 *hash = (struct checksum160 *)offset_to_ptr(hash_offset, sizeof(struct checksum160));
   get_vm_api()->ripemd160(data, length, hash );
}

static u32 recover_key( u32 digest_offset, u32 sig_offset, u32 siglen, u32 pub_offset, u32 publen ) {
   const struct checksum256* digest = (const struct checksum256*)offset_to_ptr(digest_offset, sizeof(struct checksum256));
   const char* sig = (char *)offset_to_ptr(sig_offset, siglen);
   char* pub = (char *)offset_to_ptr(pub_offset, publen);
   return get_vm_api()->recover_key(digest, sig, siglen, pub, publen);
}

static void assert_recover_key( u32 digest_offset, u32 sig_offset, u32 siglen, u32 pub_offset, u32 publen ) {
   const struct checksum256* digest = (const struct checksum256*)offset_to_ptr(digest_offset, sizeof(struct checksum256));
   const char* sig = (char *)offset_to_ptr(sig_offset, siglen);
   char* pub = (char *)offset_to_ptr(pub_offset, publen);
   get_vm_api()->assert_recover_key(digest, sig, siglen, pub, publen);
}

void init_crypto() {
    Z_envZ_assert_sha256Z_viii = assert_sha256;
    Z_envZ_assert_sha1Z_viii = assert_sha1;
    Z_envZ_assert_sha512Z_viii = assert_sha512;
    Z_envZ_assert_ripemd160Z_viii = assert_ripemd160;
    Z_envZ_sha256Z_viii = sha256;
    Z_envZ_sha1Z_viii = sha1;
    Z_envZ_sha512Z_viii = sha512;
    Z_envZ_ripemd160Z_viii = ripemd160;
    Z_envZ_assert_recover_keyZ_viiiii = assert_recover_key;
    Z_envZ_recover_keyZ_iiiiii = recover_key;
}
