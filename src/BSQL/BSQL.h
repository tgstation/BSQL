#ifdef _MSC_VER
#define BYOND_FUNC __declspec(dllexport) const char* _cdecl
#else
#define BYOND_FUNC __attribute__((visibility("default"))) const char*
#endif

#include <mysql/mysql.h>

#include <atomic>
#include <chrono>
#include <deque>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <thread>

#include "Operation.h"
#include "Query.h"
#include "Connection.h"

#include "MySqlConnection.h"
#include "MySqlConnectOperation.h"
#include "MySqlQueryOperation.h"

#include "Library.h"
