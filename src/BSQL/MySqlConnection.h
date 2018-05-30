#pragma once

class MySqlConnection : public Connection {
private:
	std::string address;
	std::string username;
	std::string password;
	unsigned short port;

	std::queue<MYSQL*> availableConnections;
	std::unique_ptr<MySqlConnectOperation> newestConnectionAttempt;
private:
	bool LoadNewConnection();
public:
	MySqlConnection();
	~MySqlConnection() override;

	std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password) override;
	std::string CreateQuery(const std::string& queryText) override;

	MYSQL* RequestConnection();
	void ReleaseConnection(MYSQL* connection);
};