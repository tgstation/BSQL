#pragma once

class MySqlConnectOperation;

class MySqlConnection : public Connection {
private:
	std::string address;
	std::string username;
	std::string password;
	std::string database;

	std::stack<MYSQL*> availableConnections;
	MYSQL* quoteConnection;
	std::string newestConnectionAttemptKey;

	std::atomic_uint_fast32_t threadCounter;

	const unsigned int asyncTimeout, threadLimit;
	unsigned short port;
private:
	bool LoadNewConnection(std::string& fail, int& failno);
public:
	MySqlConnection(Library& library, const unsigned int asyncTimeout, const unsigned int blockingTimeout, const unsigned int threadLimit);
	~MySqlConnection() override;

	std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) override;
	std::string CreateQuery(const std::string& queryText) override;
	std::string Quote(const std::string& str) override;

	MYSQL* RequestConnection(std::string& fail, int& failno);
	void ReleaseConnection(MYSQL* connection);
};