#pragma once

class MySqlConnectOperation : public Operation {
private:
	MySqlConnection& connPool;
	MYSQL *mysql;

	bool complete;
	std::shared_ptr<ClassState> state;
	std::thread connectThread;

	int callsToGetActiveThread = 0;
private:
	static MYSQL* InitMySql();
	void DoConnect(const std::string address, const unsigned short port, const std::string username, const std::string password, const std::string database, MYSQL* localMySql, std::shared_ptr<ClassState> localState);
public:
	MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database);
	MySqlConnectOperation(const MySqlConnectOperation&) = delete;
	MySqlConnectOperation(MySqlConnectOperation&&) = delete;
	~MySqlConnectOperation() override = default;

	bool IsComplete(bool noSkip) override;
	bool IsQuery() override;
	std::thread* GetActiveThread() override;
};
