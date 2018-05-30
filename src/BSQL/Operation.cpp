#include "BSQL.h"

std::string Operation::GetError() {
	if (!IsComplete())
		return std::string();
	return error;
}