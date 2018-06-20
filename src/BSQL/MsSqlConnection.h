#pragma once

class MsSqlConnection : Connection {
private:
	SQLHDBC connectionHandle;

	unsigned int currentStatementCount;
	bool connectionStarted, connected;

private:
	static void CloseConnection(SQLHDBC connectionHandle);
public:
	MsSqlConnection(Library& library, SQLHDBC environmentHandle);
	~MsSqlConnection() override;

	std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) override;
};