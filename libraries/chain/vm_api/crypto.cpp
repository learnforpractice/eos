#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/vm/span.hpp>
#include <fc/uint128.hpp>
#include <fc/crypto/sha256.hpp>
#include <fc/crypto/sha1.hpp>
#include <fc/crypto/ripemd160.hpp>

#include "vm_api_proxy.hpp"

void vm_api_proxy::assert_recover_key(const char *digest, size_t digest_size,
                                      const char *sig, size_t siglen,
                                      const char *pub, size_t publen) {
    fc::sha256 _digest(digest, digest_size);

    legacy_ptr<const fc::sha256> __digest((void *)&_digest);
    legacy_span<const char> _sig((void *)sig, siglen);
    legacy_span<const char> _pub((void *)pub, publen);

    _interface->assert_recover_key(std::move(__digest), std::move(_sig), std::move(_pub));
}

int32_t vm_api_proxy::recover_key(const char *digest, size_t digest_size,
                                  const char *sig, size_t siglen,
                                  char *pub, size_t publen) {
    fc::sha256 _digest(digest, digest_size);

    legacy_ptr<const fc::sha256> __digest((void *)&_digest);
    legacy_span<const char> _sig((void *)sig, siglen);
    legacy_span<char> _pub(pub, publen);
    
    return _interface->recover_key(std::move(__digest), std::move(_sig), std::move(_pub));
}

void vm_api_proxy::assert_sha256(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::sha256 _hash((char *)hash, (uint32_t)hash_size);
    legacy_ptr<const fc::sha256> __hash((void *)&_hash);

    _interface->assert_sha256(std::move(_data), std::move(__hash));
}

void vm_api_proxy::assert_sha1(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::sha1 _hash;
    if (_hash.data_size() != hash_size) {
        EOS_THROW( eosio_assert_message_exception, "bad hash size: ${n1}, expect ${n2}", ("n1", hash_size)("n2", _hash.data_size()));
    }
    memcpy(_hash.data(), hash, hash_size);
    legacy_ptr<const fc::sha1> __hash((void *)&_hash);
    _interface->assert_sha1(std::move(_data), std::move(__hash));
}

void vm_api_proxy::assert_sha512(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::sha512 _hash;
    if (_hash.data_size() != hash_size) {
        EOS_THROW( eosio_assert_message_exception, "bad hash size: ${n1}, expect ${n2}", ("n1", hash_size)("n2", _hash.data_size()));
    }
    memcpy(_hash.data(), hash, hash_size);

    legacy_ptr<const fc::sha512> __hash((void *)&_hash);
    _interface->assert_sha512(std::move(_data), std::move(__hash));
}

void vm_api_proxy::assert_ripemd160(const char *data, size_t length, const uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::ripemd160 _hash;
    if (_hash.data_size() != hash_size) {
        EOS_THROW( eosio_assert_message_exception, "bad hash size: ${n1}, expect ${n2}", ("n1", hash_size)("n2", _hash.data_size()));
    }
    memcpy(_hash.data(), hash, hash_size);

    legacy_ptr<const fc::ripemd160> __hash((void *)&_hash);
    _interface->assert_ripemd160(std::move(_data), std::move(__hash));
}

void vm_api_proxy::sha1(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::sha1 _hash;
    legacy_ptr<fc::sha1> __hash(&_hash);
    _interface->sha1(std::move(_data), std::move(__hash));
}

void vm_api_proxy::sha256(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::sha256 _hash;
    legacy_ptr<fc::sha256> __hash(&_hash);

    _interface->sha256(std::move(_data), std::move(__hash));
}

void vm_api_proxy::sha512(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::sha512 _hash;
    legacy_ptr<fc::sha512> __hash(&_hash);
    _interface->sha512(std::move(_data), std::move(__hash));
}

void vm_api_proxy::ripemd160(const char *data, size_t length, uint8_t *hash, size_t hash_size) {
    legacy_span<const char> _data((void *)data, length);

    fc::ripemd160 _hash;
    legacy_ptr<fc::ripemd160> __hash(&_hash);
    _interface->ripemd160(std::move(_data), std::move(__hash));
}
