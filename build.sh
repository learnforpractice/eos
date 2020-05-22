declare ARCH=$( uname )

if [[ $ARCH == "Linux" ]]; then
	declare NPROC=$( nproc )
else
	declare NPROC=$( sysctl -n hw.logicalcpu )
fi


function build_project() {
	if [ -d "$(pwd)/build" ]; then
        	pushd build
        	make -j$NPROC
	        popd
	else
		./scripts/eosio_build.sh
	fi
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
