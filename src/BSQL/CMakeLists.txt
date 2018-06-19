cmake_minimum_required (VERSION 3.0)


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
#use static crt
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
find_library(MARIA_LIBRARY libmariadb)
find_path(MARIA_INCLUDE_DIR mysql/mysql.h)
add_precompiled_header(BSQL BSQL.h FORCEINCLUDE)
set(WSLIB ws2_32)
else() #system package, this pmuch only works for travis
set_target_properties(BSQL PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
find_path(MARIA_INCLUDE_DIR NAMES "mysql.h" PATHS "/usr/include/mysql")
find_library(MARIA_INCLUDE_DIR mariadb HINTS ~/MariaDB)
endif()

include_directories(${MARIA_INCLUDE_DIR})

target_link_libraries(BSQL ${MARIA_LIBRARY} ${WSLIB})