declare ARCH=$( uname )
export PROJECT_DIR=$( pwd )

if [[ $ARCH == "Linux" ]]; then
	declare NPROC=$( nproc )
	declare DYLIB_EXT="so"
else
	declare NPROC=$( sysctl -n hw.logicalcpu )
	declare DYLIB_EXT="dylib"
fi


function build_project() {
	local ret=0
	if [ -d "$(pwd)/build" ]; then
        	pushd build
        	make -j$NPROC
			ret=$?
	        popd
	else
		./scripts/eosio_build.sh
		ret=$?
	fi
	return $ret
}

function build_uuos() {
#	rsync -av --delete programs/uuos build/programs
	rsync -av programs/uuos/extlib build/programs/uuos
	cp build/libraries/chain_api/libchain_api.$DYLIB_EXT build/programs/uuos/extlib/lib
	echo build/libraries/vm_api/libvm_api.$DYLIB_EXT
	cp build/libraries/vm_api/libvm_api.$DYLIB_EXT build/programs/uuos/extlib/lib
	cp build/programs/uuos/uuos build/programs/uuos/extlib/bin
#	strip build/programs/uuos/extlib/bin/uuos
    pushd build/programs/uuos/extlib

	if [[ $ARCH == "Linux" ]]; then
		./build-linux.sh
	else
		./build-mac.sh
	fi
	popd
}

build_project

if [ $? == 0 ]; then
	build_uuos
fi
