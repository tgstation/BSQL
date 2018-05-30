#include "BSQL.h"

std::string Query::CurrentRow() const {
	return currentRow;
}

bool Query::IsQuery() {
	return true;
}
