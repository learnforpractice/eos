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

build_project
