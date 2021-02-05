#include "vm_api4c.hpp"
#include <vm_api_proxy.hpp>

u32 vm_api4c_proxy::read_action_data(u32 msg_offset, u32 len) {
   void *msg = offset_to_ptr(msg_offset, len);
   return get_vm_api()->read_action_data(msg, len);
}

u32 vm_api4c_proxy::action_data_size(){
    return get_vm_api()->action_data_size();
}

void vm_api4c_proxy::require_recipient( u64 name ) {
   return get_vm_api()->require_recipient( name );
}

void vm_api4c_proxy::require_auth( u64 name ) {
   get_vm_api()->require_auth(name);
}

void vm_api4c_proxy::require_auth2( u64 name, u64 permission ) {
   get_vm_api()->require_auth2( name, permission );
}

u32 vm_api4c_proxy::has_auth( u64 name ) {
   return get_vm_api()->has_auth( name );
}

u32 vm_api4c_proxy::is_account( u64 name ) {
   return get_vm_api()->is_account( name ) ;
}

void vm_api4c_proxy::send_inline(u32 serialized_action_offset, u32 size) {
   char *serialized_action;
   serialized_action = (char *)offset_to_ptr(serialized_action_offset, size);
   get_vm_api()->send_inline(serialized_action, size);
}

void vm_api4c_proxy::send_context_free_inline(u32 serialized_action_offset, u32 size) {
   char *serialized_action;
   serialized_action = (char *)offset_to_ptr(serialized_action_offset, size);
   get_vm_api()->send_context_free_inline(serialized_action, size);
}

u64 vm_api4c_proxy::publication_time() {
   return get_vm_api()->publication_time();
}

u64 vm_api4c_proxy::current_receiver() {
   return get_vm_api()->current_receiver();
}
