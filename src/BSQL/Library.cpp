#include "BSQL.h"

Library::Library() :
	identifierCounter(0)
{}

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
			std::string identifier(++identifierCounter);

			switch (type)
			{
			case Connection::Type::MySql:
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