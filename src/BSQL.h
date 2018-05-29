#pragma once
#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
#else
//  GCC
#define EXPORT __attribute__((visibility("default")))
#endif

#include <mysql/mysql.h>
