#include "BSQL.h"

std::string Operation::GetError() {
	if (!IsComplete(true))
		return std::string();
	return error;
}
