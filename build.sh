function build_project() {
	pushd build
	make -j$(nproc)
	popd
}

function build_pyeos() {
	pushd programs/pyeos
	if [[ $( uname ) == "Linux" ]]; then
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
