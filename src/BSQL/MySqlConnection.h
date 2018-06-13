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
	std::string newestConnectionAttemptKey;
private:
	bool LoadNewConnection(std::string& fail, int& failno);
public:
	MySqlConnection(Library& library);
	~MySqlConnection() override;

	std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) override;
	std::string CreateQuery(const std::string& queryText) override;
	std::string Quote(const std::string& str) override;

	MYSQL* RequestConnection(std::string& fail, int& failno, bool& doNotClose);
	void ReleaseConnection(MYSQL* connection);
};