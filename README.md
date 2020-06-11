# Deprecation warning

## This repository is deprecated, currently /tg/station is using a rust based SQL client library, embedded in rust-g
This client library has a nasty habit of segfaulting if you do a reconnection option while there are queries out for processing.

You can find rust-g here - https://github.com/tgstation/rust-g - but it also comes packaged with the game.





[![Build Status](https://travis-ci.com/tgstation/BSQL.svg?branch=master)](https://travis-ci.com/tgstation/BSQL)

[![forthebadge](http://forthebadge.com/images/badges/built-with-love.svg)](http://forthebadge.com) [![forinfinityandbyond](https://user-images.githubusercontent.com/5211576/29499758-4efff304-85e6-11e7-8267-62919c3688a9.gif)](https://www.reddit.com/r/SS13/comments/5oplxp/what_is_the_main_problem_with_byond_as_an_engine/dclbu1a)

# BSQL

This is a non-blocking SQL library for [BYOND](http://www.byond.com) games

## Building

Install the c++ build chain for your operating system as well as [CMake](https://cmake.org). Remember that you must build the library as x86 (because BYOND)

### Windows

- Install the [vcpkg](https://github.com/Microsoft/vcpkg) package manager (Example done in powershell)
	- `git clone https://github.com/Microsoft/vcpkg`
	- `cd vcpkg`
	- `.\bootstrap-vcpkg.bat`
	- `.\vcpkg.exe integrate install` (Accept admin prompt. Must restart shell after this)
	
- Install libmariadb with `.\vcpkg.exe install libmariadb:x86-windows`

- Option 1: Visual Studio
	- Set up a [CMakeSettings.json](https://github.com/Microsoft/vcpkg/blob/master/docs/examples/using-sqlite.md#cmake-toolchain-file) in the project root with the path to the vcpkg toolchain file 
	- Open and build `BSQL.sln`
- Option 2: [VS Command Line Build Tools](https://www.visualstudio.com/downloads/#build-tools-for-visual-studio-2017) (Requires CMake 3.8 or higher)
	- Run `cmake -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake path/to/code/root`
	- Run `C:/path/to/msbuild.exe BSQL.sln`

### Linux

- Dependencies (Debian based distros)
- libmariadb2:i386 libmariadbclient-dev:i386 libssl1.1:i386 (exact libssl version probably doesn't matter exactly)
- Cmake expects the includes to be in `/usr/include/mysql` and the libraries to be in `/usr/lib/i386-linux-gnu`, you'll have to adjust the paths in CMakelists if your distro does it differently
- Generate makefiles with `cmake`
- Use `make` to build

### Linux Alternative

- Make the LinuxInstall.sh file executable via `chmod +x LinuxInstall.sh`
- run LinuxInstall.sh as a super user
- All dependencies will be installed and the binaries will be build for you.

#### Troubleshooting
- Run ldd on the output .so file, ensure all dependencies exist and are valid
- Run file on the output .so file and validate it's a 32 bit lib

## Integrating

To integrate BSQL into your DM project, build it [or download a windows release](https://github.com/tgstation/BSQL/releases) and drop the libmariadb and BSQL binaries in the root of your project folder. Then include the DMAPI (under `src/DMAPI`) in your project. Only include `BSQL.dm` and `BSQL/includes.dm` for maximum future compatibility. Modify the configuration options in `BSQL.dm` to your needs or create and include [seperate config file](https://github.com/Cyberboss/tgstation/blob/105fd3f6fbd59c5e21e77cb98769a89ea81de131/code/__DEFINES/bsql.config.dm). Follow the comments in `BSQL.dm` for further instructions

## LICENSE

This project is licensed under the [MIT](https://en.wikipedia.org/wiki/MIT_License) license.

See LICENSE for more details.
