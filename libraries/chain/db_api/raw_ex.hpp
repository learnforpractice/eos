#include <fc/io/raw.hpp>
namespace fc {
    namespace raw {
        template<typename Stream, typename T>
        inline void pack_ex( Stream& s, const std::vector<T>& value ) {
    //      FC_ASSERT( value.size() <= MAX_NUM_ARRAY_ELEMENTS );
        fc::raw::pack( s, unsigned_int((uint32_t)value.size()) );
        auto itr = value.begin();
        auto end = value.end();
        while( itr != end ) {
            fc::raw::pack( s, *itr );
            ++itr;
        }
        }

        template<typename Stream, typename T>
        inline void unpack_ex( Stream& s, std::vector<T>& value ) {
        unsigned_int size; fc::raw::unpack( s, size );
    //      FC_ASSERT( size.value <= MAX_NUM_ARRAY_ELEMENTS );
        value.resize(size.value);
        auto itr = value.begin();
        auto end = value.end();
        while( itr != end ) {
            fc::raw::unpack( s, *itr );
            ++itr;
        }
        }

        template<typename T>
        inline std::vector<char> pack_ex(  const T& v ) {
        datastream<size_t> ps;
        pack_ex(ps,v );
        std::vector<char> vec(ps.tellp());

        if( vec.size() ) {
            datastream<char*>  ds( vec.data(), size_t(vec.size()) );
            pack_ex(ds,v);
        }
        return vec;
        }

        template<typename T>
        inline T unpack_ex( const std::vector<char>& s )
        { try  {
        T tmp;
        datastream<const char*>  ds( s.data(), size_t(s.size()) );
        unpack_ex(ds,tmp);
        return tmp;
        } FC_RETHROW_EXCEPTIONS( warn, "error unpacking ${type}", ("type",fc::get_typename<T>::name() ) ) }

        template<typename T>
        inline T unpack_ex( const std::vector<uint8_t>& s )
        { try  {
        T tmp;
        datastream<const char*>  ds( (const char*)s.data(), size_t(s.size()) );
        unpack_ex(ds,tmp);
        return tmp;
        } FC_RETHROW_EXCEPTIONS( warn, "error unpacking ${type}", ("type",fc::get_typename<T>::name() ) ) }
    }
}