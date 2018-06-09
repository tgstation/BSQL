#ifdef _MSC_VER
#define BYOND_FUNC __declspec(dllexport) const char* _cdecl
#else
#define BYOND_FUNC __attribute__((visibility("default"))) const char*
#endif

#include <mysql/mysql.h>

#include <atomic>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <stack>
#include <string>

#include "Operation.h"
#include "Query.h"
#include "Connection.h"

#include "MySqlConnection.h"
#include "MySqlConnectOperation.h"
#include "MySqlQueryOperation.h"

#include "Library.h"
