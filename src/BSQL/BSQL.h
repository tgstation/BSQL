#ifdef _MSC_VER
#include <WinSock2.h>
#undef max
#define poll WSAPoll

#define BYOND_FUNC __declspec(dllexport) const char* _cdecl
#else
#include <poll.h>
#define BYOND_FUNC __attribute__((visibility("default"))) const char*
#endif

#include <mysql/mysql.h>

#include <atomic>
#include <chrono>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <thread>

#include "Operation.h"
#include "Query.h"
#include "Connection.h"

#include "MySqlConnection.h"
#include "MySqlOperation.h"
#include "MySqlConnectOperation.h"
#include "MySqlQueryOperation.h"

#include "Library.h"
