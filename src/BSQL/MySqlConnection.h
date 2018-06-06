#pragma once

class MySqlConnectOperation;

class MySqlConnection : public Connection {
private:
	std::string address;
	std::string username;
	std::string password;
	std::string database;
	unsigned short port;

	std::stack<MYSQL*> availableConnections;
	MYSQL* firstSuccessfulConnection;
	MySqlConnectOperation* newestConnectionAttempt;
private:
	bool LoadNewConnection();
public:
	MySqlConnection();
	~MySqlConnection() override;

	bool ReleaseOperation(const std::string& identifier) override;
	std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) override;
	std::string CreateQuery(const std::string& queryText) override;
	std::string Quote(const std::string& str) override;

	MYSQL* RequestConnection();
	void KillConnection(MYSQL* connection);
	void ReleaseConnection(MYSQL* connection);
};