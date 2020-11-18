pushd ../../../build/externals/micropython
make -j7
popd

mkdir tmp
pushd tmp
~/dev/wasm/wabt/build/wasm2c -o micropython.c ../../../../build/externals/micropython/ports/uuosio/micropython
popd
mv ./tmp/micropython.h micropython.h
mv ./tmp/micropython.c micropython.c.bin
python3 patch.py
rm -r tmp

 
