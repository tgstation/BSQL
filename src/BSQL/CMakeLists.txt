cmake_minimum_required (VERSION 3.8)

find_path(MARIA_INCLUDE_DIR mysql/mysql.h)
include_directories(${MARIA_INCLUDE_DIR})
find_library(MARIA_LIBRARY libmariadb)

find_path(JSON_INCLUDE_DIR nlohmann/json.hpp)
include_directories(${JSON_INCLUDE_DIR})

add_library (BSQL SHARED
BSQL.cpp
API.cpp
Library.cpp
Connection.cpp
MySqlConnection.cpp
Operation.cpp
MySqlConnectOperation.cpp
MySqlQueryOperation.cpp
)

add_precompiled_header(BSQL BSQL.h FORCEINCLUDE)

target_link_libraries(BSQL ${MARIA_LIBRARY})
