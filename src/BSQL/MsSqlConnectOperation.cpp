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
	std::string result("DRIVER=ODBC Driver 17 for SQL Server; Server=");
	const auto integrated(username == "ok_but_if_someone_in_the_byond_community_actually_makes_this_their_username_they_deserve_to_fail_with_this");

	if (integrated) {
		if (port == 0)
			result.append(address);
		else
			result.append(address + ',' + std::to_string(port));
		result.append("; Trusted_Connection=Yes");
	}
	else
		result.append(address + ',' + std::to_string(port) + "; Uid=" + username + "; Pwd=" + password);
	if (!database.empty())
		result.append("; Database=" + database);
	result.append(";");
	return result;
}

bool MsSqlConnectOperation::IsComplete(bool noOps) {
	if (noOps)
		return complete;
	if (complete)
		return true;
	SQLSMALLINT unused;
	const auto res(SQLDriverConnect(connectionHandle, nullptr, const_cast<SQLCHAR*>(reinterpret_cast<const SQLCHAR*>(connectionString.c_str())), static_cast<SQLSMALLINT>(connectionString.length()), nullptr, 0, &unused, SQL_DRIVER_NOPROMPT));
	if (res == SQL_STILL_EXECUTING)
		return false;
	complete = true;
	if (res == SQL_SUCCESS || res == SQL_SUCCESS_WITH_INFO)
		connected = true;
	else {
		SQLSTATE output;
		SQLINTEGER err;
		SQLSMALLINT bufflen;
		SQLCHAR buf[4096];
		const auto errRes(SQLGetDiagRec(SQL_HANDLE_DBC, connectionHandle, 1, output, &err, buf, 4096, &bufflen));
		if (errRes == SQL_SUCCESS || errRes == SQL_SUCCESS_WITH_INFO) {
			errnum = std::string(reinterpret_cast<char*>(output), SQL_SQLSTATE_SIZE);
			error = std::string(reinterpret_cast<char*>(buf), bufflen);
		}
		else
			error = "Unable to determine error details (SQLDriverConnect): " + std::to_string(errRes);
	}
	return true;
}

bool MsSqlConnectOperation::IsQuery() {
	return false;
}
