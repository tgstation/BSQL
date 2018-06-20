#pragma once

class MsSqlConnectOperation : Operation {
private:
	const SQLHDBC connectionHandle;
	const std::string connectionString;
	bool& connected;
	bool complete;

private:
	static std::string BuildConnectionString(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database);
public:
	MsSqlConnectOperation(SQLHDBC connectionHandle, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database, bool& connected);
	MsSqlConnectOperation(const MsSqlConnectOperation&) = delete;
	MsSqlConnectOperation(MsSqlConnectOperation&&) = delete;

	bool IsComplete(bool noSkip) override;
	bool IsQuery() override;
};