#pragma once

class Library {
private:
	unsigned long long identifierCounter;

	SQLHENV mssqlHandle;

	std::map<std::string, std::unique_ptr<Connection>> connections;
	std::deque<std::thread> zombieThreads;
public:
	Library() noexcept;
	~Library() noexcept;

	std::string CreateConnection(Connection::Type connectionType) noexcept;
	Connection* GetConnection(const std::string& identifier) noexcept;
	bool ReleaseConnection(const std::string& identifier) noexcept;
	void RegisterZombieThread(std::thread&& thread) noexcept;
};