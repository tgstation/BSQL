#pragma once
#ifdef _MSC_VER
#define EXPORT __declspec(dllexport) _cdecl
#else
#define EXPORT __attribute__((visibility("default"))) _cdecl
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
