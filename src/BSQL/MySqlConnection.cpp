#include "BSQL.h"

MySqlConnection::MySqlConnection(Library& library, const unsigned int asyncTimeout, const unsigned int blockingTimeout, const unsigned int threadLimit) :
	Connection(Type::MySql, library, blockingTimeout),
	quoteConnection(nullptr),
	asyncTimeout(asyncTimeout),
	threadLimit(threadLimit),
	threadCounter(0)
{}

MySqlConnection::~MySqlConnection() {
	//do this first so all reserved connections are returned to the queue
	for (auto& I : operations) {
		auto thread(I.second->GetActiveThread());
		if (thread)
			library.RegisterZombieThread(std::move(*thread));
	}
	operations.clear();
	//and release them
	while (!availableConnections.empty()) {
		auto front(availableConnections.top());
		mysql_close(front);
		availableConnections.pop();
	}

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
	int tmp;
	LoadNewConnection(newestConnectionAttemptKey, tmp);
	newestConnectionAttemptKey = std::string();
	return operations.begin()->first;
}

bool MySqlConnection::LoadNewConnection(std::string& fail, int& failno) {
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
				failno = op.GetErrno();
				ReleaseOperation(nca);
			}
		}
		else
			return false;
	}

	newestConnectionAttemptKey = AddOp(std::make_unique<MySqlConnectOperation>(*this, address, port, username, password, database, asyncTimeout, threadCounter, threadLimit));

	return false;
}

std::string MySqlConnection::CreateQuery(const std::string& queryText) {
	return AddOp(std::make_unique<MySqlQueryOperation>(*this, std::string(queryText), threadCounter, threadLimit));
}

MYSQL* MySqlConnection::RequestConnection(std::string& fail, int& failno) {
	if (availableConnections.empty() && !LoadNewConnection(fail, failno))
		return nullptr;

	auto front(availableConnections.top());
	availableConnections.pop();
	return front;
}

void MySqlConnection::ReleaseConnection(MYSQL* connection) {
	if (!quoteConnection)
		quoteConnection = connection;
	else
		availableConnections.emplace(connection);


	if (!newestConnectionAttemptKey.empty()) {
		std::string tmp;
		std::swap(tmp, newestConnectionAttemptKey);
		auto newestOperation(GetOperation(tmp));
		if (newestOperation && !newestOperation->IsComplete(false))
			std::swap(tmp, newestConnectionAttemptKey);
	}
}

std::string MySqlConnection::Quote(const std::string& str) {
	if (!quoteConnection)
		throw std::runtime_error("Not connected!");
	
	auto buffer(std::make_unique<char[]>(str.length() * 2 + 1));	
	const auto length(mysql_real_escape_string(quoteConnection, buffer.get(), str.c_str(), str.length()));
	return std::string(buffer.get(), length);
}