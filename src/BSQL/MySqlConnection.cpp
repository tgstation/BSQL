#include "BSQL.h"

MySqlConnection::MySqlConnection() : Connection(Type::MySql) {
	mysql_init(&mysql);	//should never fail since we own the struct
	mysql_options(&mysql, MYSQL_OPT_NONBLOCK, 0);
}

MySqlConnection::~MySqlConnection() {
	mysql_close(&mysql);
}

std::string MySqlConnection::Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password) {
	return AddOp(std::make_unique<MySqlConnectOperation>(&mysql, address, port, username, password));
}
std::string MySqlConnection::CreateQuery(const std::string& queryText) {
	return AddOp(std::make_unique<MySqlQueryOperation>(*this, std::string(queryText)));
}