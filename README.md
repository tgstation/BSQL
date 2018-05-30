# BSQL

## Building

Install the c++ build chain for your operating system as well as [CMake](https://cmake.org) and [vcpkg](https://github.com/Microsoft/vcpkg)

Run `vcpkg install libmariadb nlohmann-json`

Generate makefiles with `cmake "-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"` or with a [CMakeSettings.json file](https://github.com/Microsoft/vcpkg/blob/master/docs/examples/using-sqlite.md#cmake-toolchain-file) in visual studio

Run generated build files to build project
