
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

static u32 to_base58(u32 in_offset, u32 size1, u32 out_offset, u32 size2) {
   const char* in = (char *)offset_to_ptr(in_offset, size1);
   char* out = (char *)offset_to_ptr(out_offset, size2);
   return get_vm_api()->to_base58(in, size1, out, size2);
}

static u32 from_base58(u32 in_offset, u32 size1, u32 out_offset, u32 size2) {
   const char* in = (char *)offset_to_ptr(in_offset, size1);
   char* out = (char *)offset_to_ptr(out_offset, size2);
   return get_vm_api()->from_base58(in, size1, out, size2);
}

