#pragma once

class MySqlConnectOperation : public Operation {
private:
	MySqlConnection& connPool;
	MYSQL *mysql;

	const std::string address, username, password, database;
	const unsigned short port;

	bool complete, started;
	std::shared_ptr<ClassState> state;
	std::thread connectThread;
	std::atomic_uint_fast32_t& threadCounter;
	const unsigned int threadLimit, timeout;
	
private:
	static MYSQL* InitMySql(const unsigned int timeout);

	void TryStartConnecting();
	void DoConnect(MYSQL* localMySql, std::shared_ptr<ClassState> localState);
public:
	MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database, const unsigned int timeout, std::atomic_uint_fast32_t& threadCounter, const unsigned int threadLimit);
	MySqlConnectOperation(const MySqlConnectOperation&) = delete;
	MySqlConnectOperation(MySqlConnectOperation&&) = delete;
	~MySqlConnectOperation() override;

	bool IsComplete(bool noSkip) override;
	bool IsQuery() override;
	std::thread* GetActiveThread() override;
};
