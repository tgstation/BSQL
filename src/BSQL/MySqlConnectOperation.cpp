#include "BSQL.h"

MySqlConnectOperation::MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) :
	connPool(connPool),
	mysql(nullptr),
	complete(false),
	state(std::make_shared<ClassState>()),
	connectThread(&MySqlConnectOperation::DoConnect, this, address, port, username, password, database, InitMySql(), state)
{
}

MYSQL* MySqlConnectOperation::InitMySql() {
	const auto res(mysql_init(nullptr));
	if (!res)
		throw std::bad_alloc();
	return res;
}

void MySqlConnectOperation::DoConnect(const std::string address, const unsigned short port, const std::string username, const std::string password, const std::string database, MYSQL* localMySql, std::shared_ptr<ClassState> localState) {
	mysql_thread_init();
	const auto result(mysql_real_connect(localMySql, address.c_str(), username.c_str(), password.c_str(), database.empty() ? nullptr : database.c_str(), port, nullptr, 0));
	localState->lock.lock();
	if (localState->alive) {
		error = mysql_error(localMySql);
		errnum = mysql_errno(localMySql);
		if (result)
			mysql = localMySql;
		complete = true;
	}
	if (!result || !localState->alive)
		mysql_close(localMySql);
	mysql_thread_end();
	localState->lock.unlock();
}

bool MySqlConnectOperation::IsQuery() {
	return false;
}

bool MySqlConnectOperation::IsComplete(bool noSkip) {
	if (!complete)
		return false;

	if (mysql) {
		auto tmp(mysql);
		mysql = nullptr;	//recursion issue
		connPool.ReleaseConnection(tmp);
	}

	return true;
}

std::thread* MySqlConnectOperation::GetActiveThread() {
	state->lock.lock();

	if (IsComplete(false)) {
		state->lock.unlock();
		connectThread.join();
		return nullptr;
	}

	state->alive = false;
	state->lock.unlock();

	return &connectThread;
}
