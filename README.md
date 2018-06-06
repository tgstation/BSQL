[![Build Status](https://travis-ci.com/tgstation/BSQL.svg?branch=master)](https://travis-ci.com/tgstation/BSQL)

[![forthebadge](http://forthebadge.com/images/badges/built-with-love.svg)](http://forthebadge.com) [![forinfinityandbyond](https://user-images.githubusercontent.com/5211576/29499758-4efff304-85e6-11e7-8267-62919c3688a9.gif)](https://www.reddit.com/r/SS13/comments/5oplxp/what_is_the_main_problem_with_byond_as_an_engine/dclbu1a)

# BSQL

This is a non-blocking SQL library for [BYOND](http://www.byond.com) games

## Building

Install the c++ build chain for your operating system as well as [CMake](https://cmake.org).

### Windows

- Option 1: Visual Studio
	- Install the [vcpkg](https://github.com/Microsoft/vcpkg) package manager
	- `vcpkg install libmariadb:x86-windows`
	- Run `cmake` with the option `-DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake` 
	- Open and build `BSQL.sln`
- Option 2: [VS Command Line Build Tools](https://www.visualstudio.com/downloads/#build-tools-for-visual-studio-2017) (Requires CMake 3.8 or higher)
    - Acquire libmariadb header files in one folder and the .lib, and .dll in another (available [here](https://downloads.mariadb.org/connector-c))
	- Run `cmake` with the options `-DMARIA_LIBRARY:FILEPATH="C:/path/to/libmariadb.lib" -DMARIA_INCLUDE_DIR:PATH="C:/path/to/headers"` (`/path/to/headers` is the folder that contains the `mysql` include FOLDER)
	- Run `msbuild BSQL.sln`

### Linux

- Install the i386 libmariadbclient-dev package for your system. The includes are expected to be in `/usr/include/mysql` and the libraries in `/usr/lib/i386-linux-gnu` (See the travis build chain for an example)
- Generate makefiles with `cmake`
- Use `make` to build

## Integrating

To integrate BSQL into your DM project, build it [or download a windows release](https://github.com/tgstation/BSQL/releases) and drop the libmariadb and BSQL binaries in the root of your project folder. Then include the DMAPI (under `src/DMAPI`) in your project. Only include `BSQL.dm` and `BSQL/includes.dm` for maximum future compatibility. Modify the configuration options in `BSQL.dm` to your needs or create and include [seperate config file](https://github.com/Cyberboss/tgstation/blob/105fd3f6fbd59c5e21e77cb98769a89ea81de131/code/__DEFINES/bsql.config.dm). Follow the comments in `BSQL.dm` for further instructions

## LICENSE

This project is licensed under the [MIT](https://en.wikipedia.org/wiki/MIT_License) license.

See LICENSE for more details.
