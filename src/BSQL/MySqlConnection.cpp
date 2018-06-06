#include "BSQL.h"

MySqlConnection::MySqlConnection() : 
	Connection(Type::MySql),
	firstSuccessfulConnection(nullptr),
	newestConnectionAttempt(nullptr)
{}

MySqlConnection::~MySqlConnection() {
	//do this first so all reserved connections are returned to the queue
	operations.clear();
	//and release them
	while (!availableConnections.empty()) {
		auto front(availableConnections.top());
		mysql_close(front);
		if (front == firstSuccessfulConnection)
			firstSuccessfulConnection = nullptr;
		availableConnections.pop();
	}
	if (firstSuccessfulConnection)
		mysql_close(firstSuccessfulConnection);
}

std::string MySqlConnection::Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) {
	//can't connect twice
	if (!operations.empty() || !availableConnections.empty())
		return std::string();

	this->address = address;
	this->port = port;
	this->username = username;
	this->password = password;
	this->database = database;

	LoadNewConnection();

	return operations.begin()->first;
}

bool MySqlConnection::LoadNewConnection() {
	if (newestConnectionAttempt) {
		//this will chain into calling ReleaseConnection and clear the var
		if (newestConnectionAttempt->IsComplete(false)) {
			if (availableConnections.size() > 0)
				return true;
			newestConnectionAttempt = nullptr;
		}
		else
			return false;
	}

	auto newCon(std::make_unique<MySqlConnectOperation>(*this, address, port, username, password, database));
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

	auto front(availableConnections.top());
	availableConnections.pop();
	return front;
}

void MySqlConnection::ReleaseConnection(MYSQL* connection) {
	availableConnections.emplace(connection);
	if (!firstSuccessfulConnection)
		firstSuccessfulConnection = connection;
	if (newestConnectionAttempt) {
		auto tmp(newestConnectionAttempt);
		newestConnectionAttempt = nullptr;
		if (!tmp->IsComplete(false))
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

void MySqlConnection::KillConnection(MYSQL* connection) {
	if (connection != firstSuccessfulConnection)	//we keep the first around for quoting
		mysql_close(connection);
}

std::string MySqlConnection::Quote(const std::string& str) {
	if (!firstSuccessfulConnection)
		throw std::runtime_error("Not connected!");
	auto buffer(std::make_unique<char[]>(str.length() * 2 + 1));
	const auto length(mysql_real_escape_string(firstSuccessfulConnection, buffer.get(), str.c_str(), str.length()));
	return std::string(buffer.get(), length);
}