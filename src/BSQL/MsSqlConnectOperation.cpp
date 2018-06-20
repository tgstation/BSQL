#include "BSQL.h"

MsSqlConnectOperation::MsSqlConnectOperation(SQLHDBC connectionHandle, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database, bool& connected) :
	connectionHandle(connectionHandle),
	connectionString(BuildConnectionString(address, port, username, password, database)),
	connected(connected),
	complete(false)
{
	IsComplete(false);
}

std::string MsSqlConnectOperation::BuildConnectionString(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) {
	std::string result("Data Source=" + address + ',' + std::to_string(port));
	if (username == "ok_but_if_someone_in_the_byond_community_actually_makes_this_their_username_they_deserve_to_fail_with_this")
		//integrated security
		result += ";Integrated Security=true;";
	else 
		result += ";User Id=" + username + "; Password=" + password;
	if (!database.empty())
		result += ";Initial Catalog=" + database;
	result += ";";
	return result;
}

bool MsSqlConnectOperation::IsComplete(bool noOps) {
	if (noOps)
		return complete;
	if (!complete)
		return false;
	SQLSMALLINT unused;
	const auto res(SQLDriverConnect(connectionHandle, nullptr, const_cast<SQLCHAR*>(reinterpret_cast<const SQLCHAR*>(connectionString.c_str())), connectionString.length(), nullptr, 0, &unused, SQL_DRIVER_NOPROMPT));
	if (res == SQL_STILL_EXECUTING)
		return false;
	complete = true;
	if (res == SQL_SUCCESS || res == SQL_SUCCESS_WITH_INFO)
		connected = true;
	else {
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
			error = "Unable to determine error details (SQLDriverConnect): " + std::to_string(errRes);
	}
}

bool MsSqlConnectOperation::IsQuery() {
	return false;
}
