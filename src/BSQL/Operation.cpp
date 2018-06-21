#include "BSQL.h"

std::string Operation::GetError() {
	if (!IsComplete(true))
		return std::string();
	return error;
}

std::string Operation::GetErrorCode() {
	if (!IsComplete(true))
		return std::string();
	return errnum;
}

std::thread* Operation::GetActiveThread() {
	return nullptr;
}
