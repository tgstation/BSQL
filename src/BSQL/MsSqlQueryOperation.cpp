#include "BSQL.h"

MsSqlQueryOperation::MsSqlQueryOperation(SQLHDBC connectionHandle, const std::string& queryText, unsigned int& currentStatementCount) :
	connectionHandle(connectionHandle),
	queryText(queryText),
	statementHandle(nullptr),
	currentStatementCount(currentStatementCount)
{
	SQLSMALLINT unused;
	const auto qRes(SQLGetInfo(connectionHandle, SQL_MAX_ASYNC_CONCURRENT_STATEMENTS, &maximum, sizeof(SQLUINTEGER), &unused));
	if (qRes != SQL_SUCCESS && qRes != SQL_SUCCESS_WITH_INFO)
		throw std::bad_alloc();
	IsComplete(false);
}

MsSqlQueryOperation::~MsSqlQueryOperation() {
	if (statementHandle) {
		if (!complete)
			SQLCancel(statementHandle);
		SQLFreeStmt(statementHandle, SQL_CLOSE);
		--currentStatementCount;
	}
}

bool MsSqlQueryOperation::IsComplete(bool noSkip) {
	if (noSkip)
		return complete;
	if (complete)
		return true;

	if (!statementHandle) {
		if (currentStatementCount >= maximum)
			//waiting on available statement handle
			return false;
		const auto res(SQLAllocStmt(connectionHandle, &statementHandle));
		if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
		{
			errnum = res;
			SQLSTATE output;
			SQLINTEGER err;
			SQLSMALLINT bufflen;
			SQLCHAR buf[4096];
			const auto errRes(SQLGetDiagRec(SQL_HANDLE_DBC, connectionHandle, 1, output, &err, buf, 4096, &bufflen));
			if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
				errnum = err;
				error = std::string(reinterpret_cast<char*>(buf), bufflen);
			}
			else
				error = "Unable to determine error details (SQLAllocStmt): " + std::to_string(errRes);
			complete = true;
			return true;
		}
		++currentStatementCount;
	}
}