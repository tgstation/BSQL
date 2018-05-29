#pragma once

class Library {
private:
	std::map<std::string, Connection> connections;

	unsigned long long identifierCounter;
public:
	Library();

	std::string CreateConnection(Connection::Type connectionType);
	Connection* GetConnection(const std::string& identifier);
	bool ReleaseConnection(const std::string& identifier);
};