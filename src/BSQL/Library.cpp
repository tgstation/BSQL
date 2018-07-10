#include "BSQL.h"

Library::Library() noexcept :
	identifierCounter(0)
{
	mysql_library_init(0, nullptr, nullptr);
}

Library::~Library() noexcept {
	for (auto& I : zombieThreads)
		I.join();
	//https://jira.mariadb.org/browse/CONC-336
	//mysql_library_end();
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

std::string Library::CreateConnection(Connection::Type type, const unsigned int asyncTimeout, const unsigned int blockingTimeout) noexcept {
	if (identifierCounter < std::numeric_limits<unsigned long long>().max()) {
		try {
			auto identifier(std::to_string(++identifierCounter));

			switch (type)
			{
			case Connection::Type::MySql:
				connections.emplace(identifier, std::make_unique<MySqlConnection>(*this, asyncTimeout, blockingTimeout));
				break;
			case Connection::Type::SqlServer:
				--identifierCounter;
				return std::string();
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
