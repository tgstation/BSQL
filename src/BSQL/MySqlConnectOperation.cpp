#include "BSQL.h"

MySqlConnectOperation::MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) :
	connPool(connPool),
	mysql(mysql_init(nullptr)),
	complete(false)
{
	if (mysql == nullptr)
		throw std::bad_alloc();
	mysql_options(mysql, MYSQL_OPT_NONBLOCK, 0);
	mysql_real_connect_start(&ret, mysql, address.c_str(), username.c_str(), password.c_str(), database.empty() ? nullptr : database.c_str(), port, nullptr, 0);
}

MySqlConnectOperation::~MySqlConnectOperation() {
	if (!IsComplete(false))
		mysql_close(mysql);
}

bool MySqlConnectOperation::IsQuery() {
	return false;
}

bool MySqlConnectOperation::IsComplete(bool noOps) {
	if (complete)
		return true;
	const auto status(mysql_real_connect_cont(&ret, mysql, 0));
	complete = status == 0;
	if (!complete)
		return false;

	if (!ret) {
		error = "mysql_real_connect() returns error: " + std::string(mysql_error(mysql));
		mysql_close(mysql);	//don't use connPool Kill since it's never seen this connection
	}
	else
		connPool.ReleaseConnection(mysql);

	return true;
}
