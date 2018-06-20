#include "BSQL.h"

MsSqlConnection::MsSqlConnection(Library& library, SQLHDBC environmentHandle) :
	Connection(Type::SqlServer, library),
	currentStatementCount(0),
	connectionStarted(false),
	connected(false)
{
	auto res(SQLAllocConnect(environmentHandle, &connectionHandle));
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
		throw std::bad_alloc();
	SQLSetConnectAttr(connectionHandle, SQL_ATTR_ASYNC_DBC_FUNCTIONS_ENABLE, reinterpret_cast<SQLPOINTER>(SQL_ASYNC_DBC_ENABLE_ON), 0);	//guarnteed success
}

MsSqlConnection::~MsSqlConnection() {
	operations.clear();
	if (connected)
		library.RegisterZombieThread(std::thread(CloseConnection, connectionHandle));
	else {
		if (connectionStarted)
			SQLCancelHandle(SQL_HANDLE_DBC, connectionHandle);
		SQLFreeConnect(connectionHandle);
	}
}

void MsSqlConnection::CloseConnection(SQLHDBC connectionHandle) {
	while (SQLDisconnect(connectionHandle) == SQL_STILL_EXECUTING);
	SQLFreeConnect(connectionHandle);
}

std::string MsSqlConnection::Connect(const std::string& address, const unsigned short port, const std::string& username, const std::string& password, const std::string& database) {
	if (connectionStarted)
		return std::string();
	connectionStarted = true;
	return AddOp(std::make_unique<MsSqlConnectOperation>(connectionHandle, address, port, username, password, database, connected));
}