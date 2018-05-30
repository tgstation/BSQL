#include "BSQL.h"

Connection::Connection(Type type) :
	type(type),
	identifierCounter(0)
{}
