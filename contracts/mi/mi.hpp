#pragma once

#include <eosio/action.hpp>
#include <eosio/name.hpp>
#include <eosio/serialize.hpp>
#include <eosio/fixed_bytes.hpp>

#include <vector>
#include <tuple>
#include <boost/hana.hpp>
#include <functional>
#include <utility>
#include <type_traits>
#include <iterator>
#include <limits>
#include <algorithm>
#include <memory>

namespace eosio {
namespace internal_use_do_not_use {
extern "C" {
    __attribute__((eosio_wasm_import))
    int32_t db_store_i64(uint64_t, uint64_t, uint64_t, uint64_t,  const void*, uint32_t);

    __attribute__((eosio_wasm_import))
    void db_update_i64(int32_t, uint64_t, const void*, uint32_t);

    __attribute__((eosio_wasm_import))
    void db_remove_i64(int32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_get_i64(int32_t, const void*, uint32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_next_i64(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_previous_i64(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_find_i64(uint64_t, uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_lowerbound_i64(uint64_t, uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_upperbound_i64(uint64_t, uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_end_i64(uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_store(uint64_t, uint64_t, uint64_t, uint64_t, const uint64_t*);

    __attribute__((eosio_wasm_import))
    void db_idx64_update(int32_t, uint64_t, const uint64_t*);

    __attribute__((eosio_wasm_import))
    void db_idx64_remove(int32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_next(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_previous(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_find_primary(uint64_t, uint64_t, uint64_t, uint64_t*, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_find_secondary(uint64_t, uint64_t, uint64_t, const uint64_t*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_lowerbound(uint64_t, uint64_t, uint64_t, uint64_t*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_upperbound(uint64_t, uint64_t, uint64_t, uint64_t*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx64_end(uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_store(uint64_t, uint64_t, uint64_t, uint64_t, const uint128_t*);

    __attribute__((eosio_wasm_import))
    void db_idx128_update(int32_t, uint64_t, const uint128_t*);

    __attribute__((eosio_wasm_import))
    void db_idx128_remove(int32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_next(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_previous(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_find_primary(uint64_t, uint64_t, uint64_t, uint128_t*, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_find_secondary(uint64_t, uint64_t, uint64_t, const uint128_t*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_lowerbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_upperbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx128_end(uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_store(uint64_t, uint64_t, uint64_t, uint64_t, const uint128_t*, uint32_t);

    __attribute__((eosio_wasm_import))
    void db_idx256_update(int32_t, uint64_t, const uint128_t*, uint32_t);

    __attribute__((eosio_wasm_import))
    void db_idx256_remove(int32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_next(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_previous(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_find_primary(uint64_t, uint64_t, uint64_t, uint128_t*, uint32_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_find_secondary(uint64_t, uint64_t, uint64_t, const uint128_t*, uint32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_lowerbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_upperbound(uint64_t, uint64_t, uint64_t, uint128_t*, uint32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx256_end(uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_store(uint64_t, uint64_t, uint64_t, uint64_t, const double*);

    __attribute__((eosio_wasm_import))
    void db_idx_double_update(int32_t, uint64_t, const double*);

    __attribute__((eosio_wasm_import))
    void db_idx_double_remove(int32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_next(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_previous(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_find_primary(uint64_t, uint64_t, uint64_t, double*, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_find_secondary(uint64_t, uint64_t, uint64_t, const double*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_lowerbound(uint64_t, uint64_t, uint64_t, double*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_upperbound(uint64_t, uint64_t, uint64_t, double*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_double_end(uint64_t, uint64_t, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_store(uint64_t, uint64_t, uint64_t, uint64_t, const long double*);

    __attribute__((eosio_wasm_import))
    void db_idx_long_double_update(int32_t, uint64_t, const long double*);

    __attribute__((eosio_wasm_import))
    void db_idx_long_double_remove(int32_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_next(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_previous(int32_t, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_find_primary(uint64_t, uint64_t, uint64_t, long double*, uint64_t);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_find_secondary(uint64_t, uint64_t, uint64_t, const long double*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_lowerbound(uint64_t, uint64_t, uint64_t, long double*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_upperbound(uint64_t, uint64_t, uint64_t, long double*, uint64_t*);

    __attribute__((eosio_wasm_import))
    int32_t db_idx_long_double_end(uint64_t, uint64_t, uint64_t);
}
};

constexpr static inline name same_payer{};

template<class Class,typename Type,Type (Class::*PtrToMemberFunction)()const>
struct const_mem_fun
{
    typedef typename std::remove_reference<Type>::type result_type;

    template<typename ChainedPtr>

    auto operator()(const ChainedPtr& x)const -> std::enable_if_t<!std::is_convertible<const ChainedPtr&, const Class&>::value, Type>
    {
        return operator()(*x);
    }

    Type operator()(const Class& x)const
    {
        return (x.*PtrToMemberFunction)();
    }

    Type operator()(const std::reference_wrapper<const Class>& x)const
    {
        return operator()(x.get());
    }

    Type operator()(const std::reference_wrapper<Class>& x)const
    {
        return operator()(x.get());
    }
};

class secondary_index_db_functions {
    public:
        virtual int32_t db_idx_next( int32_t iterator, uint64_t* primary );
        virtual int32_t db_idx_previous( int32_t iterator, uint64_t* primary );
        virtual void    db_idx_remove( int32_t iterator  );
        virtual int32_t db_idx_end( uint64_t code, uint64_t scope, uint64_t table );
        virtual int32_t db_idx_store( uint64_t scope, uint64_t table, uint64_t id, const void* secondary, uint32_t size, uint64_t payer );
        virtual void    db_idx_update( int32_t iterator, const void *secondary, uint32_t size, uint64_t payer );
        virtual int32_t db_idx_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t primary, void* secondary, uint32_t size );
        virtual int32_t db_idx_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const void* secondary, uint32_t size, uint64_t& primary );
        virtual int32_t db_idx_lowerbound( uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t size, uint64_t& primary );
        virtual int32_t db_idx_upperbound( uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t size, uint64_t& primary );
};

#define WRAP_SECONDARY_SIMPLE_TYPE(IDX, TYPE)\
class secondary_index_db_functions_##IDX : public secondary_index_db_functions {\
    public: \
        virtual int32_t db_idx_next( int32_t iterator, uint64_t* primary )         { return internal_use_do_not_use::db_##IDX##_next( iterator, primary ); } \
        virtual int32_t db_idx_previous( int32_t iterator, uint64_t* primary )    { return internal_use_do_not_use::db_##IDX##_previous( iterator, primary ); } \
        virtual void     db_idx_remove( int32_t iterator  )                  { internal_use_do_not_use::db_##IDX##_remove( iterator ); } \
        virtual int32_t db_idx_end( uint64_t code, uint64_t scope, uint64_t table ) { return internal_use_do_not_use::db_##IDX##_end( code, scope, table ); } \
        virtual int32_t db_idx_store( uint64_t scope, uint64_t table, uint64_t id, const void* secondary, uint32_t size, uint64_t payer ) {\
        check(sizeof(TYPE) == size, "bad size of "#TYPE); \
        return internal_use_do_not_use::db_##IDX##_store( scope, table, payer, id, (TYPE*)secondary ); \
        }\
        virtual void db_idx_update( int32_t iterator, const void* secondary, uint32_t size, uint64_t payer ) {\
        internal_use_do_not_use::db_##IDX##_update( iterator, payer, (TYPE*)secondary ); \
        }\
        virtual int32_t db_idx_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t primary, void* secondary, uint32_t size ) {\
        return internal_use_do_not_use::db_##IDX##_find_primary( code, scope, table, (TYPE*)secondary, primary ); \
        }\
        virtual int32_t db_idx_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const void* secondary, uint32_t size, uint64_t& primary ) {\
        return internal_use_do_not_use::db_##IDX##_find_secondary( code, scope, table, (TYPE*)secondary, &primary ); \
        }\
        virtual int32_t db_idx_lowerbound( uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t size, uint64_t& primary ) {\
        return internal_use_do_not_use::db_##IDX##_lowerbound( code, scope, table, (TYPE*)secondary, &primary ); \
        }\
        virtual int32_t db_idx_upperbound( uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t size, uint64_t& primary ) {\
        return internal_use_do_not_use::db_##IDX##_upperbound( code, scope, table, (TYPE*)secondary, &primary ); \
        }\
};

#define WRAP_SECONDARY_ARRAY_TYPE(IDX, TYPE)\
struct secondary_index_db_functions_##IDX: public secondary_index_db_functions {\
    public: \
        virtual int32_t db_idx_next( int32_t iterator, uint64_t* primary )         { return internal_use_do_not_use::db_##IDX##_next( iterator, primary ); } \
        virtual int32_t db_idx_previous( int32_t iterator, uint64_t* primary )    { return internal_use_do_not_use::db_##IDX##_previous( iterator, primary ); } \
        virtual void     db_idx_remove( int32_t iterator )                    { internal_use_do_not_use::db_##IDX##_remove( iterator ); } \
        virtual int32_t db_idx_end( uint64_t code, uint64_t scope, uint64_t table ) { return internal_use_do_not_use::db_##IDX##_end( code, scope, table ); } \
        virtual int32_t db_idx_store( uint64_t scope, uint64_t table, uint64_t id, const void* secondary, uint32_t size, uint64_t payer ) {\
        return internal_use_do_not_use::db_##IDX##_store( scope, table, payer, id, (uint128_t*)secondary, TYPE::num_words() ); \
        }\
        virtual void     db_idx_update( int32_t iterator, const void* secondary, uint32_t size, uint64_t payer ) {\
        internal_use_do_not_use::db_##IDX##_update( iterator, payer, (uint128_t*)secondary, TYPE::num_words() ); \
        }\
        virtual int32_t db_idx_find_primary( uint64_t code, uint64_t scope, uint64_t table, uint64_t primary, void* secondary, uint32_t size ) {\
            check(size == TYPE::num_words()*sizeof(uint128_t), "bad secondary size"); \
            return internal_use_do_not_use::db_##IDX##_find_primary( code, scope, table, (uint128_t*)secondary, TYPE::num_words(), primary ); \
        }\
        virtual int32_t db_idx_find_secondary( uint64_t code, uint64_t scope, uint64_t table, const void* secondary, uint32_t size, uint64_t& primary ) {\
            check(size == TYPE::num_words()*sizeof(uint128_t), "bad secondary size"); \
            return internal_use_do_not_use::db_##IDX##_find_secondary( code, scope, table, (uint128_t*)secondary, TYPE::num_words(), &primary ); \
        }\
        virtual int32_t db_idx_lowerbound( uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint32_t size, uint64_t& primary ) {\
        return internal_use_do_not_use::db_##IDX##_lowerbound( code, scope, table, (uint128_t*)secondary, TYPE::num_words(), &primary ); \
        }\
        virtual int32_t db_idx_upperbound( uint64_t code, uint64_t scope, uint64_t table, void* secondary, uint64_t& primary ) {\
        return internal_use_do_not_use::db_##IDX##_upperbound( code, scope, table, (uint128_t*)secondary, TYPE::num_words(), &primary ); \
        }\
};

#define MAKE_TRAITS_FOR_ARITHMETIC_SECONDARY_KEY(TYPE)\
template<>\
struct secondary_key_traits<TYPE> {\
    static_assert( std::numeric_limits<TYPE>::is_specialized, "TYPE does not have specialized numeric_limits" );\
    static constexpr TYPE true_lowest() { return std::numeric_limits<TYPE>::lowest(); }\
};

namespace _multi_index_detail {

    namespace hana = boost::hana;

    template<typename T>
    struct secondary_key_traits;

    WRAP_SECONDARY_SIMPLE_TYPE(idx64,  uint64_t)
    MAKE_TRAITS_FOR_ARITHMETIC_SECONDARY_KEY(uint64_t)

    WRAP_SECONDARY_SIMPLE_TYPE(idx128, uint128_t)
    MAKE_TRAITS_FOR_ARITHMETIC_SECONDARY_KEY(uint128_t)

    WRAP_SECONDARY_SIMPLE_TYPE(idx_double, double)
    template<>
    struct secondary_key_traits<double> {
    static constexpr double true_lowest() { return -std::numeric_limits<double>::infinity(); }
    };

    WRAP_SECONDARY_SIMPLE_TYPE(idx_long_double, long double)
    template<>
    struct secondary_key_traits<long double> {
    static constexpr long double true_lowest() { return -std::numeric_limits<long double>::infinity(); }
    };

    WRAP_SECONDARY_ARRAY_TYPE(idx256, eosio::fixed_bytes<32>)
    template<>
    struct secondary_key_traits<eosio::fixed_bytes<32>> {
    static constexpr eosio::fixed_bytes<32> true_lowest() { return eosio::fixed_bytes<32>(); }
    };

}
