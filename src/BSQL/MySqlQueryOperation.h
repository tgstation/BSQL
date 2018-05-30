#pragma once

class MySqlQueryOperation : public Query {
private:
	const std::string queryText;
	MySqlConnection& connPool;
	MYSQL* connection;
	MYSQL_RES* result;
public:
	MySqlQueryOperation(MySqlConnection& connPool, std::string&& queryText);
	~MySqlQueryOperation() override;

	bool IsComplete() override;
	bool IsQuery() override;

	bool BeginGetNextRow() override;
};
