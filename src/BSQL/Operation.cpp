#include "BSQL.h"

std::string Operation::GetError() {
	if (!IsComplete(true))
		return std::string();
	return error;
}

std::string Operation::GetErrorCode() {
	if (!IsComplete(true))
		return std::string();
	return std::to_string(errnum);
}

int Operation::GetErrno() {
	if (!IsComplete(true))
		return -1;
	return errnum;
}

std::thread* Operation::GetActiveThread() {
	return nullptr;
}
