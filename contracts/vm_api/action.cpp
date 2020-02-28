/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <eosiolib/action.h>

#include "vm_api.h"

uint32_t read_action_data( void* msg, uint32_t len ) {
   return get_vm_api()->read_action_data(msg, len);
}

uint32_t action_data_size() {
   return get_vm_api()->action_data_size();
}

void require_recipient( account_name name ) {
   return get_vm_api()->require_recipient( name );
}

void require_auth( account_name name ) {
   get_vm_api()->require_auth(name);
}

bool has_auth( account_name name ) {
   return get_vm_api()->has_auth( name );
}

void require_auth2( account_name name, permission_name permission ) {
   get_vm_api()->require_auth2( name, permission );
}

bool is_account( account_name name ) {
   return get_vm_api()->is_account( name ) ;
}

void send_inline(char *serialized_action, uint32_t size) {
   get_vm_api()->send_inline(serialized_action, size);
}

void send_context_free_inline(char *serialized_action, uint32_t size) {
   get_vm_api()->send_context_free_inline(serialized_action, size);
}


uint64_t  publication_time() {
   return get_vm_api()->publication_time();
}

account_name current_receiver() {
   return get_vm_api()->current_receiver();
}

bool is_feature_activated(const char *digest, uint32_t size) {
   return get_vm_api()->is_feature_activated(digest, size);
}

void preactivate_feature(const char *digest, uint32_t size) {
   get_vm_api()->preactivate_feature(digest, size);
}

void set_action_return_value( const char* packed_blob, uint32_t datalen ) {
   get_vm_api()->set_action_return_value(packed_blob, datalen);
}

#include <stdio.h>

extern "C" {
	void say_hello() {
		printf("hhhhhhhhello,world\n");
	}

   void checktime() {
      get_vm_api()->checktime();
   }
}
