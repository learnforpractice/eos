declare ARCH=$( uname )

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

function build_pyeos() {
	pushd programs/pyeos
	if [[ $ARCH == "Linux" ]]; then
		./build-linux.sh
	else
		./build-mac.sh
	fi
	popd
}

build_project

if [ $? == 0 ]; then
	build_pyeos
fi
