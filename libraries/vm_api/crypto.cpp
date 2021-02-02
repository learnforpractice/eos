#include <vm_api_proxy.hpp>

extern "C" {

void assert_recover_key(const char *digest, size_t digest_size,
                        const char *sig, size_t siglen,
                        const char *pub, size_t publen) {
    get_vm_api()->assert_recover_key(
        digest, digest_size,
        sig, siglen,
        pub, publen
    );
}

int32_t recover_key(const char *digest, size_t digest_size,
                    const char *sig, size_t siglen,
                    char *pub, size_t publen) {    
    return get_vm_api()->recover_key(
        digest, digest_size,
        sig, siglen,
        pub, publen
    );
}

void assert_sha256(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    get_vm_api()->assert_sha256(data, length, hash, hash_size);
}

void assert_sha1(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    get_vm_api()->assert_sha1(data, length, hash, hash_size);
}

void assert_sha512(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    get_vm_api()->assert_sha512(data, length, hash, hash_size);
}

void assert_ripemd160(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    get_vm_api()->assert_ripemd160(data, length, hash, hash_size);
}

void sha1(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    get_vm_api()->sha1(data, length, hash, hash_size);
}

void sha256(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    get_vm_api()->sha256(data, length, hash, hash_size);
}

void sha512(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    get_vm_api()->sha512(data, length, hash, hash_size);
}

void ripemd160(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    get_vm_api()->ripemd160(data, length, hash, hash_size);
}

}
