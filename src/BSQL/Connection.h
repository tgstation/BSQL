#pragma once

class Connection {
public:
	enum Type {
		MySql,
		SqlServer
	};
public:
	const Type type;
protected:
	Library & library;
	std::map<std::string, std::unique_ptr<Operation>> operations;
private:
	unsigned long long identifierCounter;
protected:
	Connection(Type type, Library& library);

	std::string AddOp(std::unique_ptr<Operation>&& operation);
public:
	virtual ~Connection() = default;

	Operation* GetOperation(const std::string& identifier);
	virtual bool ReleaseOperation(const std::string& identifier);

	virtual std::string Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) = 0;

	virtual std::string CreateQuery(const std::string& queryText) = 0;

	virtual std::string Quote(const std::string& str) = 0;
};