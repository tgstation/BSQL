#pragma once

class MsSqlQueryOperation : Query {
private:
	const SQLHDBC connectionHandle;
	const std::string queryText;
	SQLHSTMT statementHandle;
	SQLUINTEGER maximum;
	unsigned int& currentStatementCount;
	bool complete;
public:
	MsSqlQueryOperation(SQLHDBC connectionHandle, const std::string& queryText, unsigned int& currentStatementCount);
	MsSqlQueryOperation(const MsSqlQueryOperation&) = delete;
	MsSqlQueryOperation(MsSqlQueryOperation&&) = delete;
	~MsSqlQueryOperation() override;

	bool IsComplete(bool noSkip) override;
};
