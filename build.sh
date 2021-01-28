declare ARCH=$( uname )
export PROJECT_DIR=$( pwd )

if [[ $ARCH == "Linux" ]]; then
	declare NPROC=$( nproc )
else
	declare NPROC=$( sysctl -n hw.logicalcpu )
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
	cp build/libraries/chain_api/libchain_api.dylib build/programs/uuos/extlib/lib
	cp build/programs/uuos/uuos build/programs/uuos/extlib/bin
	strip build/programs/uuos/extlib/bin/uuos
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
