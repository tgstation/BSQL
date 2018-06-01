cmake_minimum_required (VERSION 3.8)


add_library (BSQL SHARED
BSQL.cpp
API.cpp
Library.cpp
Connection.cpp
MySqlConnection.cpp
Operation.cpp
MySqlConnectOperation.cpp
MySqlQueryOperation.cpp
Query.cpp
)

if(WIN32) #vcpkg
find_library(MARIA_LIBRARY libmariadb)
find_path(MARIA_INCLUDE_DIR mysql/mysql.h)
else() #system package
find_library(MARIA_LIBRARY libmariadbclient)
set_target_properties(BSQL PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
set(CMAKE_FIND_ROOT_PATH /usr/lib32)# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

find_path(MARIA_INCLUDE_DIR mysql/mysql.h PATHS "/usr/include/")
endif()

include_directories(${MARIA_INCLUDE_DIR})

target_link_libraries(BSQL ${MARIA_LIBRARY})
add_precompiled_header(BSQL BSQL.h FORCEINCLUDE)

