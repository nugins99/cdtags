.PHONY: all debug release

debug: 
	conan install . --output-folder=build/debug --build=missing -s build_type=Debug
	cd build/debug ; cmake ../..  -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
	cmake --build build/debug --parallel

release:
	conan install . --output-folder=build/release --build=missing -s build_type=Release
	cd build/release ; cmake ../..  -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
	cmake --build build/release --parallel

all: debug release

clean:
	rm -rf build
	
