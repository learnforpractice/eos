extern "C" {
/* import: 'env' 'read_action_data' */
u32 (*Z_envZ_read_action_dataZ_iii)(u32, u32);
/* import: 'env' 'action_data_size' */
u32 (*Z_envZ_action_data_sizeZ_iv)(void);
/* import: 'env' 'require_recipient' */
void (*Z_envZ_require_recipientZ_vj)(u64);
/* import: 'env' 'require_auth' */
void (*Z_envZ_require_authZ_vj)(u64);
/* import: 'env' 'require_auth2' */
void (*Z_envZ_require_auth2Z_vjj)(u64, u64);
/* import: 'env' 'has_auth' */
u32 (*Z_envZ_has_authZ_ij)(u64);
/* import: 'env' 'is_account' */
u32 (*Z_envZ_is_accountZ_ij)(u64);

/* import: 'env' 'send_inline' */
void (*Z_envZ_send_inlineZ_vii)(u32, u32);
/* import: 'env' 'send_context_free_inline' */
void (*Z_envZ_send_context_free_inlineZ_vii)(u32, u32);

/* import: 'env' 'publication_time' */
u64 (*Z_envZ_publication_timeZ_jv)(void);
/* import: 'env' 'current_receiver' */
u64 (*Z_envZ_current_receiverZ_jv)(void);

}

static u32 read_action_data(u32 msg_offset, u32 len) {
   void *msg = offset_to_ptr(msg_offset, len);
   return get_vm_api()->read_action_data(msg, len);
}

static u32 action_data_size(){
    return get_vm_api()->action_data_size();
}

static void require_recipient( u64 name ) {
   return get_vm_api()->require_recipient( name );
}

static void require_auth( u64 name ) {
   get_vm_api()->require_auth(name);
}

static void require_auth2( u64 name, u64 permission ) {
   get_vm_api()->require_auth2( name, permission );
}

static u32 has_auth( u64 name ) {
   return get_vm_api()->has_auth( name );
}

static u32 is_account( u64 name ) {
   return get_vm_api()->is_account( name ) ;
}

static void send_inline(u32 serialized_action_offset, u32 size) {
   char *serialized_action;
   serialized_action = (char *)offset_to_ptr(serialized_action_offset, size);
   get_vm_api()->send_inline(serialized_action, size);
}

static void send_context_free_inline(u32 serialized_action_offset, u32 size) {
   char *serialized_action;
   serialized_action = (char *)offset_to_ptr(serialized_action_offset, size);
   get_vm_api()->send_context_free_inline(serialized_action, size);
}

static u64 publication_time() {
   return get_vm_api()->publication_time();
}

static u64 current_receiver() {
   return get_vm_api()->current_receiver();
}

void init_action() {
    Z_envZ_read_action_dataZ_iii = read_action_data;
    Z_envZ_action_data_sizeZ_iv = action_data_size;
    Z_envZ_require_recipientZ_vj = require_recipient;
    Z_envZ_require_authZ_vj = require_auth;
    Z_envZ_require_auth2Z_vjj = require_auth2;
    Z_envZ_has_authZ_ij = has_auth;
    Z_envZ_is_accountZ_ij = is_account;
    Z_envZ_send_inlineZ_vii = send_inline;
    Z_envZ_send_context_free_inlineZ_vii = send_context_free_inline;
    Z_envZ_publication_timeZ_jv = publication_time;
    Z_envZ_current_receiverZ_jv = current_receiver;   
}
