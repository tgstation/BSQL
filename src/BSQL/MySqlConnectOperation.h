#pragma once

class MySqlConnectOperation : public Operation {
private:
	MySqlConnection& connPool;
	MYSQL* ret, *mysql;
	std::chrono::milliseconds timeoutAt;

	int status;
	bool complete;
public:
	MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database);
	MySqlConnectOperation(const MySqlConnectOperation&) = delete;
	MySqlConnectOperation(MySqlConnectOperation&&) = delete;
	~MySqlConnectOperation() override;

	bool IsComplete(bool noOps) override;
	bool IsQuery() override;
};
