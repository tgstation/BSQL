#include "BSQL.h"

MsSqlQueryOperation::MsSqlQueryOperation(SQLHDBC connectionHandle, const std::string& queryText, unsigned int& currentStatementCount) :
	connectionHandle(connectionHandle),
	queryText(queryText),
	statementHandle(nullptr),
	currentStatementCount(currentStatementCount),
	executed(false),
	complete(false)
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
			SQLSTATE output;
			SQLINTEGER err;
			SQLSMALLINT bufflen;
			SQLCHAR buf[4096];
			const auto errRes(SQLGetDiagRec(SQL_HANDLE_STMT, statementHandle, 1, output, &err, buf, 4096, &bufflen));
			if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
				errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
				error = std::string(reinterpret_cast<char*>(buf), bufflen);
			}
			else
				error = "Unable to determine error details (SQLAllocStmt): " + std::to_string(errRes);
			complete = true;
			return true;
		}
		++currentStatementCount;
	}

	if (!executed) {
		auto res(SQLExecDirect(statementHandle, const_cast<SQLCHAR*>(reinterpret_cast<const SQLCHAR*>(queryText.c_str())), queryText.length()));
		if (res == SQL_STILL_EXECUTING)
			return false;

		if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
		{
			SQLSTATE output;
			SQLINTEGER err;
			SQLSMALLINT bufflen;
			SQLCHAR buf[4096];
			const auto errRes(SQLGetDiagRec(SQL_HANDLE_STMT, statementHandle, 1, output, &err, buf, 4096, &bufflen));
			if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
				errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
				error = std::string(reinterpret_cast<char*>(buf), bufflen);
			}
			else
				error = "Unable to determine error details (SQLExecDirect): " + std::to_string(errRes);
			complete = true;
			return true;
		}

		res = SQLNumResultCols(statementHandle, &resultColumnCount);

		if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
		{
			SQLSTATE output;
			SQLINTEGER err;
			SQLSMALLINT bufflen;
			SQLCHAR buf[4096];
			const auto errRes(SQLGetDiagRec(SQL_HANDLE_STMT, statementHandle, 1, output, &err, buf, 4096, &bufflen));
			if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
				errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
				error = std::string(reinterpret_cast<char*>(buf), bufflen);
			}
			else {
				error = "Unable to determine error details (SQLNumResultCols): " + std::to_string(errRes);
				errnum = "-1";
			}
			complete = true;
			return true;
		}

		if (resultColumnCount == 0) {
			complete = true;
			return true;
		}

		executed = true;
	}

	const auto res(SQLFetch(statementHandle));
	if (res == SQL_STILL_EXECUTING)
		return false;

	if (res == SQL_NO_DATA) {
		complete = true;
		currentRow = std::string();
	}
	else if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
	{
		SQLSTATE output;
		SQLINTEGER err;
		SQLSMALLINT bufflen;
		SQLCHAR buf[4096];
		const auto errRes(SQLGetDiagRec(SQL_HANDLE_STMT, statementHandle, 1, output, &err, buf, 4096, &bufflen));
		if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
			errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
			error = std::string(reinterpret_cast<char*>(buf), bufflen);
		}
		else {
			errnum = "-1";
			error = "Unable to determine error details (SQLFetch): " + std::to_string(errRes);
		}
		complete = true;
	}
	else {
		std::string json("{");
		bool first(true);
		for (auto I(0); I < resultColumnCount; ++I) {
			//tell me about yourself
			SQLCHAR buf[4096];
			SQLSMALLINT bufflen, dataType, decimalDigits, nullable;
			SQLUINTEGER columnSize;
			const auto descRes(SQLDescribeCol(statementHandle, I + 1, buf, 4096, &bufflen, &dataType, &columnSize, &decimalDigits, &nullable));
			if (descRes != SQL_SUCCESS && descRes != SQL_SUCCESS_WITH_INFO) {
				SQLSTATE output;
				SQLINTEGER err;
				const auto errRes(SQLGetDiagRec(SQL_HANDLE_STMT, statementHandle, 1, output, &err, buf, 4096, &bufflen));
				if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
					errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
					error = std::string(reinterpret_cast<char*>(buf), bufflen);
				}
				else {
					error = "Unable to determine error details (SQLDescribeCol): " + std::to_string(errRes);
				}
				complete = true;
				return true;
			}
			std::string columnName(reinterpret_cast<char*>(buf), bufflen);
			std::string result;
			do {
				SQLINTEGER resultLen;
				const auto bufferRes(SQLGetData(statementHandle, I + 1, SQL_C_CHAR, buf, 4096, &resultLen));
				if (bufferRes != SQL_SUCCESS && bufferRes != SQL_SUCCESS_WITH_INFO) {
					SQLSTATE output;
					SQLINTEGER err;
					const auto errRes(SQLGetDiagRec(SQL_HANDLE_STMT, statementHandle, 1, output, &err, buf, 4096, &bufflen));
					if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
						errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
						error = std::string(reinterpret_cast<char*>(buf), bufflen);
					}
					else
						error = "Unable to determine error details (SQLGetData): " + std::to_string(errRes);
					complete = true;
					return true;
				}
				result.append(std::string(reinterpret_cast<char*>(buf), resultLen));
				if (bufferRes == SQL_SUCCESS_WITH_INFO && resultLen == 4096)
					//truncation
					continue;

				if (first)
					first = false;
				else
					json.append(",");
				json.append("\"");
				json.append(columnName);
				json.append("\":");
				if (bufflen == SQL_NULL_DATA)
					json.append("null");
				else {
					json.append("\"");
					json.append(std::move(result));
					json.append("\"");
				}
			} while (false);
		}
		json.append("}");
		currentRow = std::move(json);
	}
	return true;
}