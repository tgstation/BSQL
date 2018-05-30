#include "BSQL.h"

MySqlConnectOperation::MySqlConnectOperation(MYSQL* const mysql, const std::string& address, const unsigned short port, const std::string& username, const std::string& password) :
	mysql(mysql),
	status(mysql_real_connect_start(&ret, mysql, address.c_str(), username.c_str(), password.c_str(), nullptr, port, nullptr, 0)),
	complete(false)
{}

MySqlConnectOperation::~MySqlConnectOperation() {
	if (complete)
		return;
	status = mysql_real_connect_cont(&ret, mysql, status);
	while (status) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		status = mysql_real_connect_cont(&ret, mysql, status);
	}
}

bool MySqlConnectOperation::IsQuery() {
	return false;
}

bool MySqlConnectOperation::IsComplete() {
	if (!complete) {
		if (status)
			status = mysql_real_connect_cont(&ret, mysql, status);
		complete = status == 0;
		if (complete) {
			if (!ret)
				error = "Failed to mysql_real_connect()!";
		}
		else
			return false;
	}
	return true;
}
