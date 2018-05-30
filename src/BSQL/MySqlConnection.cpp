#include "BSQL.h"

MySqlConnection::MySqlConnection() : Connection(Type::MySql) 
{}

MySqlConnection::~MySqlConnection() {
	//do this first so all reserved connections are returned to the queue
	operations.clear();
	//and release them
	while (!availableConnections.empty()) {
		auto front(availableConnections.front());
		mysql_close(front);
		availableConnections.pop();
	}
}

std::string MySqlConnection::Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password) {
	auto mysql(mysql_init(nullptr));
	if (mysql == nullptr)
		throw std::bad_alloc();
	mysql_options(mysql, MYSQL_OPT_NONBLOCK, 0);
	return AddOp(std::make_unique<MySqlConnectOperation>(mysql, address, port, username, password));
}

std::string MySqlConnection::CreateQuery(const std::string& queryText) {
	return AddOp(std::make_unique<MySqlQueryOperation>(*this, std::string(queryText)));
}

MYSQL* MySqlConnection::RequestConnection() {
	if (availableConnections.empty() && !LoadNewConnection())
		return nullptr;

	auto front(availableConnections.front());
	availableConnections.pop();
	if (availableConnections.empty())
		LoadNewConnection();
	return front;
}

void MySqlConnection::ReleaseConnection(MYSQL* connection) {
	availableConnections.emplace(connection);
}
