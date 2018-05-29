#include "BSQL.h"

const char* Library::GoodReturn = "SUCCESS";
const char* Library::BadReturn = "FAILURE";

Library::Library() :
	identifierCounter(0)
{}