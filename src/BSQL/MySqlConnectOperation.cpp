#include "BSQL.h"

MySqlConnectOperation::MySqlConnectOperation(MySqlConnection& connPool, MYSQL* const mysql, const std::string& address, const unsigned short port, const std::string& username, const std::string& password) :
	connPool(connPool),
	mysql(mysql),
	complete(false)
{
	mysql_real_connect_start(&ret, mysql, address.c_str(), username.c_str(), password.c_str(), nullptr, port, nullptr, 0);
}

MySqlConnectOperation::~MySqlConnectOperation() {
	if (complete)
		return;
	auto status(mysql_real_connect_cont(&ret, mysql, 0));
	while (status) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		status = mysql_real_connect_cont(&ret, mysql, 0);
	}
}

bool MySqlConnectOperation::IsQuery() {
	return false;
}

bool MySqlConnectOperation::IsComplete() {
	if (complete)
		return true;
	const auto status(mysql_real_connect_cont(&ret, mysql, 0));
	complete = status == 0;
	if (complete) {
		if (!ret)
			error = "Failed to mysql_real_connect()!";
		else
			connPool.ReleaseConnection(mysql);
	}
	else
		return false;
	return true;
}
