cd build-sol2
cd Debug
cmake ../../sol2 -G Ninja -DCMAKE_BUILD_TYPE=Debug   -DLUA_VERSION="${LUA_VERSION}" -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DTESTS_SINGLE=ON
cmake --build . --config Debug
ctest -C Debug
cd ..

cd Release
cmake ../../sol2 -G Ninja -DCMAKE_BUILD_TYPE=Release -DLUA_VERSION="${LUA_VERSION}" -DBUILD_LUA=ON -DBUILD_LUA_AS_DLL=OFF -DTESTS=ON -DEXAMPLES=ON -DSINGLE=ON -DTESTS_EXAMPLES=ON -DTESTS_SINGLE=ON
cmake --build . --config Release
ctest -C Release
cd ..
