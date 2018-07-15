#pragma once

class Library {
private:
	unsigned long long identifierCounter;

	std::map<std::string, std::unique_ptr<Connection>> connections;
	std::deque<std::thread> zombieThreads;
public:
	Library() noexcept;
	~Library() noexcept;

	static std::string EscapeJsonString(const std::string& str);

	std::string CreateConnection(Connection::Type connectionType, const unsigned int asyncTimeout, const unsigned int blockingTimeout) noexcept;
	Connection* GetConnection(const std::string& identifier) noexcept;
	bool ReleaseConnection(const std::string& identifier) noexcept;
	void RegisterZombieThread(std::thread&& thread) noexcept;
};