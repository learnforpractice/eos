#include "vm_api4c.hpp"

static vm_api4c_proxy *s_proxy;

extern "C" void int_vm_api4c_proxy(vm_api4c_proxy *proxy) {
    s_proxy = proxy;
}

extern "C" vm_api4c_proxy *get_vm_api4c_proxy() {
    return s_proxy;
}

//
u32 read_action_data(u32 msg_offset, u32 len) {
   return get_vm_api4c_proxy()->read_action_data(msg_offset, len);
}

u32 action_data_size(){
    return get_vm_api4c_proxy()->action_data_size();
}

void require_recipient( u64 name ) {
   return get_vm_api4c_proxy()->require_recipient( name );
}

void require_auth( u64 name ) {
   get_vm_api4c_proxy()->require_auth(name);
}

void require_auth2( u64 name, u64 permission ) {
   get_vm_api4c_proxy()->require_auth2( name, permission );
}

u32 has_auth( u64 name ) {
   return get_vm_api4c_proxy()->has_auth( name );
}

u32 is_account( u64 name ) {
   return get_vm_api4c_proxy()->is_account( name );
}

void send_inline(u32 serialized_action_offset, u32 size) {
   get_vm_api4c_proxy()->send_inline(serialized_action_offset, size);
}

void send_context_free_inline(u32 serialized_action_offset, u32 size) {
   get_vm_api4c_proxy()->send_context_free_inline(serialized_action_offset, size);
}

u64 publication_time() {
   return get_vm_api4c_proxy()->publication_time();
}

u64 current_receiver() {
   return get_vm_api4c_proxy()->current_receiver();
}

u32 get_active_producers( u32 producers_offset, uint32_t datalen ) {
   return get_vm_api4c_proxy()->get_active_producers(producers_offset, datalen);
}

void assert_sha256( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->assert_sha256(data_offset, length, hash_offset );
}

void assert_sha1( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->assert_sha1(data_offset, length, hash_offset );
}

void assert_sha512( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->assert_sha512(data_offset, length, hash_offset );
}

void assert_ripemd160( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->assert_ripemd160(data_offset, length, hash_offset );
}

void sha256( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->sha256(data_offset, length, hash_offset );
}

void sha1( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->sha1(data_offset, length, hash_offset );
}

void sha512( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->sha512(data_offset, length, hash_offset );
}

void ripemd160( u32 data_offset, u32 length, u32 hash_offset ) {
   get_vm_api4c_proxy()->ripemd160(data_offset, length, hash_offset );
}

u32 recover_key( u32 digest_offset, u32 sig_offset, u32 siglen, u32 pub_offset, u32 publen ) {
   return get_vm_api4c_proxy()->recover_key(digest_offset, sig_offset, siglen, pub_offset, publen);
}

void assert_recover_key( u32 digest_offset, u32 sig_offset, u32 siglen, u32 pub_offset, u32 publen ) {
   get_vm_api4c_proxy()->assert_recover_key(digest_offset, sig_offset, siglen, pub_offset, publen);
}

