mkdir -p build-sol2/Debug
mkdir -p build-sol2/Release
cd build-sol2

echo "=== Compiler and tool variables ==="
ninja --version
cmake --version

cd Debug
	cmake ${SOL2_DIR} -G Xcode -DCMAKE_BUILD_TYPE=Debug -DLUA_VERSION="${LUA_VERSION}" -DCI=ON -DPLATFORM=${PLATFORM} -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DEXAMPLES_SINGLE=ON -DTESTS_SINGLE=ON
	cmake --build . --config Debug
	ctest --build-config Debug --output-on-failure
cd ..

cd Release
	cmake ${SOL2_DIR} -G Xcode -DCMAKE_BUILD_TYPE=Release -DLUA_VERSION="${LUA_VERSION}" -DCI=ON -DPLATFORM=${PLATFORM} -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DEXAMPLES_SINGLE=ON -DTESTS_SINGLE=ON
	cmake --build . --config Release
	ctest --build-config Release --output-on-failure
cd ..
