#include "BSQL.h"

MySqlConnectOperation::MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password) :
	connPool(connPool),
	mysql(mysql_init(nullptr)),
	complete(false)
{
	if (mysql == nullptr)
		throw std::bad_alloc();
	mysql_options(mysql, MYSQL_OPT_NONBLOCK, 0);
	mysql_real_connect_start(&ret, mysql, address.c_str(), username.c_str(), password.c_str(), nullptr, port, nullptr, CLIENT_MULTI_STATEMENTS);
}

MySqlConnectOperation::~MySqlConnectOperation() {
	while (!IsComplete())
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
		if (!ret) {
			error = "mysql_real_connect() returns error: " + std::string(mysql_error(mysql));
			mysql_close(mysql);
		}
		else
			connPool.ReleaseConnection(mysql);
	}
	else
		return false;
	return true;
}
