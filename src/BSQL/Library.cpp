#include "BSQL.h"

Library::Library() noexcept :
	identifierCounter(0)
{
	auto res(SQLAllocEnv(&mssqlHandle));
	if(res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO)
		throw std::bad_alloc();
	res = SQLSetEnvAttr(mssqlHandle, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3_80), 0);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) {
		SQLFreeEnv(mssqlHandle);
		throw std::bad_alloc();
	}
	mysql_library_init(0, nullptr, nullptr);
}

Library::~Library() noexcept {
	for (auto& I : zombieThreads)
		I.join();

	connections.clear();

	//https://jira.mariadb.org/browse/CONC-336
	//mysql_library_end();

	SQLFreeEnv(mssqlHandle);
}

Connection* Library::GetConnection(const std::string& identifier) noexcept {
	auto iter(connections.find(identifier));
	if (iter == connections.end())
		return nullptr;
	return iter->second.get();
}

bool Library::ReleaseConnection(const std::string& identifier) noexcept {
	return connections.erase(identifier) > 0;
}

std::string Library::CreateConnection(Connection::Type type) noexcept {
	if (identifierCounter < std::numeric_limits<unsigned long long>().max()) {
		try {
			auto identifier(std::to_string(++identifierCounter));

			switch (type)
			{
			case Connection::Type::MySql:
				connections.emplace(identifier, std::make_unique<MySqlConnection>(*this));
				break;
			case Connection::Type::SqlServer:
				connections.emplace(identifier, std::make_unique<MsSqlConnection>(*this, mssqlHandle));
				break;
			}
			return identifier;
		}
		catch (std::bad_alloc&) {
		}
	}
	return std::string();
}

void Library::RegisterZombieThread(std::thread&& thread) noexcept {
	try {
		zombieThreads.emplace_back(std::move(thread));
	}
	catch(std::bad_alloc&) {
		//gotta wait then
		thread.join();
	}
}
