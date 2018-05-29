#pragma once
#ifdef _MSC_VER
#define BYOND_FUNC __declspec(dllexport) const char* _cdecl
#else
#define BYOND_FUNC __attribute__((visibility("default"))) const char* _cdecl
#endif

#include <mysql/mysql.h>
#include <nlohmann/json.hpp>

#include <atomic>
#include <map>
#include <memory>
#include <string>

#include "Operation.h"
#include "Query.h"
#include "Connection.h"
#include "Library.h"
