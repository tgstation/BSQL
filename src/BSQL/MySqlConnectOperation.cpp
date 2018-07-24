#include "BSQL.h"

MySqlConnectOperation::MySqlConnectOperation(MySqlConnection& connPool, const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database, const unsigned int timeout, std::atomic_uint_fast32_t& threadCounter, const unsigned int threadLimit) :
	connPool(connPool),
	mysql(nullptr),
	address(address),
	username(username),
	password(password),
	database(database),
	port(port),
	complete(false),
	started(false),
	state(std::make_shared<ClassState>()),
	threadCounter(threadCounter),
	threadLimit(threadLimit),
	timeout(timeout)
{
	TryStartConnecting();
}

void MySqlConnectOperation::TryStartConnecting() {
	if (threadCounter.fetch_add(1) >= threadLimit) {
		--threadCounter;
		return;
	}
	started = true;
	connectThread = std::thread(&MySqlConnectOperation::DoConnect, this, InitMySql(timeout), state);
}

MYSQL* MySqlConnectOperation::InitMySql(const unsigned int timeout) {
	const auto res(mysql_init(nullptr));
	if (!res)
		throw std::bad_alloc();
	mysql_options(res, MYSQL_OPT_CONNECT_TIMEOUT, static_cast<const void*>(&timeout));
	mysql_options(res, MYSQL_OPT_READ_TIMEOUT, static_cast<const void*>(&timeout));
	mysql_options(res, MYSQL_OPT_WRITE_TIMEOUT, static_cast<const void*>(&timeout));
	return res;
}

void MySqlConnectOperation::DoConnect(MYSQL* localMySql, std::shared_ptr<ClassState> localState) {
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
	if (!started) {
		TryStartConnecting();
		return false;
	}

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
	if (!started)
		return nullptr;

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
