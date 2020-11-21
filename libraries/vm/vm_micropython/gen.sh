pushd ../../../build/externals/micropython
make -j7
popd

./../../../build/libraries/vm/vm_wasm/wasm_injector ../../../build/externals/micropython/ports/uuosio/micropython ../../../build/externals/micropython/ports/uuosio/micropython_softfloat.wasm

mkdir tmp
pushd tmp
#~/dev/wasm/wabt/build/wasm2c -o micropython.c ../../../../build/externals/micropython/ports/uuosio/micropython_softfloat.wasm
./../../../../build/libraries/wabt/wasm2c -o micropython.c ../../../../build/externals/micropython/ports/uuosio/micropython_softfloat.wasm

popd
mv ./tmp/micropython.h micropython.h
mv ./tmp/micropython.c micropython.c.bin
python3 patch.py
rm -r tmp

 
