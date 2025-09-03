.PHONY: release debug clean clang gcc msvc build config

debug:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_CLI=ON
	cmake --build build

msvc:
	cmake -S . -B build -G "Visual Studio 17 2022" -A x64
	cmake --build build --config Debug 

config:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug -DBUILD_CLI=ON

build:
	cmake --build build

clang: 
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

gcc:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build --config Release

release:
	cmake -G "MinGW Makefiles" -Bbuild -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release -DBUILD_CLI=OFF
	cmake --build build

clean:
	rm -rf build
