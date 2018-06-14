#include "BSQL.h"

std::string Operation::GetError() {
	if (!IsComplete(true))
		return std::string();
	return error;
}

int Operation::GetErrno() {
	if (!IsComplete(true))
		return -1;
	return errnum;
}
