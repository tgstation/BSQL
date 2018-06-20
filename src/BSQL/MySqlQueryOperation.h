#pragma once

class MySqlQueryOperation : public Query {
private:
	std::string queryText;
	MySqlConnection& connPool;
	MYSQL* connection;
	bool noClose;
	std::shared_ptr<ClassState> state;
	std::queue<std::string> results;
	int connectionAttempts;
	bool started, complete;
	std::thread operationThread;
private:
	std::thread TryStart();

	void QuestionableExit(MYSQL* mysql, std::shared_ptr<ClassState>& localClassState);
	void StartQuery(MYSQL* mysql, std::string&& localQueryText, std::shared_ptr<ClassState> localClassState);
public:
	MySqlQueryOperation(MySqlConnection& connPool, std::string&& queryText);
	MySqlQueryOperation(const MySqlQueryOperation&) = delete;
	MySqlQueryOperation(MySqlQueryOperation&&) = delete;
	~MySqlQueryOperation() override;

	bool IsComplete(bool noSkip) override;
	std::thread* GetActiveThread() override;
};
