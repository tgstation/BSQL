#pragma once

class Library {
public:
	static const char* GoodReturn;
	static const char* BadReturn;
private:
	std::map<std::string, Connection> connections;

	unsigned long long identifierCounter;
public:
	Library();

	std::string CreateConnection(Connection::Type connectionType);
	Connection* GetConnection(const std::string& identifier);
	bool ReleaseConnection(const std::string& identifier);
};