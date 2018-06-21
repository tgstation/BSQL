#pragma once

class MsSqlQueryOperation : public Query {
private:
	const SQLHDBC connectionHandle;
	const std::string queryText;
	SQLHSTMT statementHandle;
	SQLUINTEGER maximum;
	SQLSMALLINT resultColumnCount;
	unsigned int& currentStatementCount;
	bool executed, complete;
public:
	MsSqlQueryOperation(SQLHDBC connectionHandle, const std::string& queryText, unsigned int& currentStatementCount);
	MsSqlQueryOperation(const MsSqlQueryOperation&) = delete;
	MsSqlQueryOperation(MsSqlQueryOperation&&) = delete;
	~MsSqlQueryOperation() override;

	bool IsComplete(bool noSkip) override;
};
