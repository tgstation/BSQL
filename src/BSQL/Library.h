#pragma once

class Library {
public:
	static const char* GoodReturn;
	static const char* BadReturn;
private:
	unsigned long long identifierCounter;

	std::map<std::string, std::unique_ptr<Connection>> connections;
public:
	Library() noexcept;

	std::string CreateConnection(Connection::Type connectionType) noexcept;
	Connection* GetConnection(const std::string& identifier) noexcept;
	bool ReleaseConnection(const std::string& identifier) noexcept;
};