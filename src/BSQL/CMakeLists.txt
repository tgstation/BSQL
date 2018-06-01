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
set_target_properties(BSQL PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
find_path(MARIA_INCLUDE_DIR NAMES "mysql.h" PATHS "/usr/include/mysql")
find_library(MARIA_LIBRARY NAMES "mariadbclient" PATHS "/usr/lib/i386-linux-gnu")
endif()

include_directories(${MARIA_INCLUDE_DIR})

target_link_libraries(BSQL ${MARIA_LIBRARY})
add_precompiled_header(BSQL BSQL.h FORCEINCLUDE)

