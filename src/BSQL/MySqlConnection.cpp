#include "BSQL.h"

MySqlConnection::MySqlConnection() : 
	Connection(Type::MySql),
	firstSuccessfulConnection(nullptr)
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

	LoadNewConnection(newestConnectionAttemptKey);
	newestConnectionAttemptKey = std::string();
	return operations.begin()->first;
}

bool MySqlConnection::LoadNewConnection(std::string& fail) {
	if (!newestConnectionAttemptKey.empty()) {
		//this will chain into calling ReleaseConnection and clear the var
		auto nca(newestConnectionAttemptKey);
		auto& op(*GetOperation(nca));
		if (op.IsComplete(false)) {
			const auto success(availableConnections.size() > 0);
			if (success) {
				ReleaseOperation(nca);
				return true;
			}
			else {
				fail = op.GetError();
				ReleaseOperation(nca);
			}
		}
		else
			return false;
	}

	newestConnectionAttemptKey = AddOp(std::make_unique<MySqlConnectOperation>(*this, address, port, username, password, database));

	return false;
}

std::string MySqlConnection::CreateQuery(const std::string& queryText) {
	return AddOp(std::make_unique<MySqlQueryOperation>(*this, std::string(queryText)));
}

MYSQL* MySqlConnection::RequestConnection(std::string& fail) {
	if (availableConnections.empty() && !LoadNewConnection(fail))
		return nullptr;

	auto front(availableConnections.top());
	availableConnections.pop();
	return front;
}

void MySqlConnection::ReleaseConnection(MYSQL* connection) {
	availableConnections.emplace(connection);

	if (!firstSuccessfulConnection)
		firstSuccessfulConnection = connection;

	if (!newestConnectionAttemptKey.empty()) {
		std::string tmp;
		std::swap(tmp, newestConnectionAttemptKey);
		if (!GetOperation(tmp)->IsComplete(false))
			std::swap(tmp, newestConnectionAttemptKey);
	}
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