#pragma once

class Connection {
public:
	enum Type {
		MySql,
		SqlServer
	};
public:
	const Type type;
private:
	std::map<std::string, Operation> operations;

	unsigned long long identifierCounter;
public:
	virtual ~Connection() = 0;

	Operation* GetOperation(const std::string& identifier);
	bool ReleaseOperation(const std::string& identifier);

	virtual std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password) = 0;

	virtual std::string CreateQuery(const std::string& queryText) = 0;
};