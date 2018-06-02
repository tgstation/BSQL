#include "BSQL.h"

MySqlConnection::MySqlConnection() : 
	Connection(Type::MySql),
	newestConnectionAttempt(nullptr)
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
	//can't connect twice
	if (!operations.empty() || !availableConnections.empty())
		return std::string();

	this->address = address;
	this->port = port;
	this->username = username;
	this->password = password;

	LoadNewConnection();

	return operations.begin()->first;
}

bool MySqlConnection::LoadNewConnection() {
	if (newestConnectionAttempt)
		//this will chain into calling ReleaseConnection and clear the var
		return newestConnectionAttempt->IsComplete() && availableConnections.size() > 0;

	auto newCon(std::make_unique<MySqlConnectOperation>(*this, address, port, username, password));
	newestConnectionAttempt = newCon.get();
	AddOp(std::move(newCon));

	return false;
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
	if (newestConnectionAttempt) {
		auto tmp(newestConnectionAttempt);
		newestConnectionAttempt = nullptr;
		if (!tmp->IsComplete())
			newestConnectionAttempt = tmp;
	}
}

bool MySqlConnection::ReleaseOperation(const std::string& identifier) {
	auto op(GetOperation(identifier));
	if (op && op == newestConnectionAttempt)
		newestConnectionAttempt = nullptr;
	auto result(Connection::ReleaseOperation(identifier));
	if (availableConnections.empty())
		LoadNewConnection();
	return result;
}