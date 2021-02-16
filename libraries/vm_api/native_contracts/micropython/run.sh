mkdir tmp
pushd tmp
~/dev/wasm/wabt/build/wasm2c -o micropython_eosio.c ~/dev/uuos3/externals/micropython/build/ports/micropython_eosio.wasm
mv micropython_eosio.c micropython_eosio.c.bin
cp micropython_eosio.* ..
popd
rm -r tmp

