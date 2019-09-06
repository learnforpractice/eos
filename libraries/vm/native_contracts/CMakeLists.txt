add_library(native_contracts SHARED
    main.cpp
    wasm-rt-impl.c
    eosio.system.c
#    eosio.token.c
    contracts.cpp

    contracts/eosio-047c7e09c81da6dec89ab970603696268049d1cc3ded37987ff9b8d89dedd371-1722535.c
    contracts/eosio-254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed-12246.c
    contracts/eosio-254247a18af399e237f47ea63e7b5970a8f9d6819150efe443d8e711895915ed-186.c
    contracts/eosio-5710f43eca84ffc70bd6f152aad9e10068687c9a18307c586eba68c116766623-1717927.c
    contracts/eosio-5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59-11999.c
    contracts/eosio-5e8e655a05b34e782467684d7148244404b591a6c1ec2687eacb268926a37e59-127.c
    contracts/eosio.msig-5cf017909547b2d69cee5f01c53fe90f3ab193c57108f81a17f0716a4c83f9c0-176.c
    contracts/eosio.token-3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348-180.c
    contracts/eosio.unregd-51f681f756ded1ebbf2775406fe089138dddc7761885595ec7f885e276393af0-185.c
    contracts/eosiochaince-3e0cf4172ab025f9fff5f1db11ee8a34d44779492e1d668ae1dc2d129e865348-1488192.c

    contracts/eosio.token-e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb-0.c
    contracts/eosio.token-e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb-10.c
    contracts/eosio.token-e536996bc480ccc3486b630f0c42ca1ef3b9f477595e885dcf8862848a2185bb-17.c
)

target_link_libraries(native_contracts PRIVATE eosiolib_native vm_api4c)
target_include_directories(native_contracts PRIVATE ${Boost_INCLUDE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/libraries/vm/vm_api4c
    PRIVATE ${CMAKE_SOURCE_DIR}/contracts
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

if (APPLE)
    set(_symbols_list "${CMAKE_CURRENT_SOURCE_DIR}/symbols.list")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,-exported_symbols_list,'${_symbols_list}'")
else()
    set(_version_script "${CMAKE_CURRENT_SOURCE_DIR}/version.script")
    set(LINK_FLAGS "${LINK_FLAGS} -Wl,--version-script,\"${_version_script}\"")
endif (APPLE)
set_target_properties(native_contracts  PROPERTIES LINK_FLAGS "${LINK_FLAGS}")


#target_compile_options(eosio.system PRIVATE -fvisibility=hidden)

