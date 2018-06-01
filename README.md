# BSQL

## Building

Install the c++ build chain for your operating system as well as [CMake](https://cmake.org).

### Windows

- Install [vcpkg](https://github.com/Microsoft/vcpkg) on and the libmariadb:x86-windows package
- Generate makefiles with `cmake "-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"` or with a [CMakeSettings.json file](https://github.com/Microsoft/vcpkg/blob/master/docs/examples/using-sqlite.md#cmake-toolchain-file) in visual studio

### Linux

- Install the libmariadb-dev package for your system
- Generate makefiles with `cmake`


- Run generated build files to build project
