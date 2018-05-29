cmake_minimum_required (VERSION 3.8)

find_path(MARIA_INCLUDE_DIR mysql/mysql.h)
include_directories(${MARIA_INCLUDE_DIR})
find_library(MARIA_LIBRARY libmariadb)

find_path(JSON_INCLUDE_DIR nlohmann/json.hpp)
include_directories(${JSON_INCLUDE_DIR})

add_library (BSQL SHARED
"BSQL.cpp"
)
target_link_libraries(BSQL ${MARIA_LIBRARY})
