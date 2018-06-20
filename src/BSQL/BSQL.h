#ifdef _MSC_VER
#define BYOND_FUNC __declspec(dllexport) const char* _cdecl
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef max
#else
#define BYOND_FUNC __attribute__((visibility("default"))) const char*
#endif

#include <mysql/mysql.h>

#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>

#include <chrono>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <thread>

class Library;

#include "Operation.h"
#include "Query.h"
#include "Connection.h"

#include "MySqlConnection.h"
#include "MySqlConnectOperation.h"
#include "MySqlQueryOperation.h"

#include "MsSqlConnection.h"
#include "MsSqlConnectOperation.h"
#include "MsSqlQueryOperation.h"

#include "Library.h"
