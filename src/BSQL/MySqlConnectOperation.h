#pragma once

class MySqlConnectOperation : public Operation {
private:
	MYSQL* ret, *mysql;

	bool complete;
public:
	MySqlConnectOperation(MYSQL* const mysql, const std::string& address, const unsigned short port, const std::string& username, const std::string& password);
	MySqlConnectOperation(const MySqlConnectOperation&) = delete;
	MySqlConnectOperation(MySqlConnectOperation&&) = delete;
	~MySqlConnectOperation() override;

	bool IsComplete() override;
	bool IsQuery() override;
};
