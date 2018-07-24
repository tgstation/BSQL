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

std::string Library::CreateConnection(Connection::Type type, const unsigned int asyncTimeout, const unsigned int blockingTimeout, const unsigned int threadLimit) noexcept {
	if (identifierCounter < std::numeric_limits<unsigned long long>().max()) {
		try {
			auto identifier(std::to_string(++identifierCounter));

			switch (type)
			{
			case Connection::Type::MySql:
				connections.emplace(identifier, std::make_unique<MySqlConnection>(*this, asyncTimeout, blockingTimeout, threadLimit));
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

//code below from here: https://github.com/nlohmann/json/blob/ec7a1d834773f9fee90d8ae908a0c9933c5646fc/src/json.hpp#L4604-L4697

static std::size_t extra_space(const std::string& s) noexcept
{
	std::size_t result = 0;

	for (const auto& c : s)
	{
		switch (c)
		{
		case '"':
		case '\\':
		case '\b':
		case '\f':
		case '\n':
		case '\r':
		case '\t':
		{
			// from c (1 byte) to \x (2 bytes)
			result += 1;
			break;
		}

		default:
		{
			if (c >= 0x00 && c <= 0x1f)
			{
				// from c (1 byte) to \uxxxx (6 bytes)
				result += 5;
			}
			break;
		}
		}
	}

	return result;
}

std::string Library::EscapeJsonString(const std::string& s)
{
	const auto space = extra_space(s);
	if (space == 0)
	{
		return s;
	}

	// create a result string of necessary size
	std::string result(s.size() + space, '\\');
	std::size_t pos = 0;

	for (const auto& c : s)
	{
		switch (c)
		{
			// quotation mark (0x22)
		case '"':
		{
			result[pos + 1] = '"';
			pos += 2;
			break;
		}

		// reverse solidus (0x5c)
		case '\\':
		{
			// nothing to change
			pos += 2;
			break;
		}

		// backspace (0x08)
		case '\b':
		{
			result[pos + 1] = 'b';
			pos += 2;
			break;
		}

		// formfeed (0x0c)
		case '\f':
		{
			result[pos + 1] = 'f';
			pos += 2;
			break;
		}

		// newline (0x0a)
		case '\n':
		{
			result[pos + 1] = 'n';
			pos += 2;
			break;
		}

		// carriage return (0x0d)
		case '\r':
		{
			result[pos + 1] = 'r';
			pos += 2;
			break;
		}

		// horizontal tab (0x09)
		case '\t':
		{
			result[pos + 1] = 't';
			pos += 2;
			break;
		}

		default:
		{
			if (c >= 0x00 && c <= 0x1f)
			{
				// print character c as \uxxxx
				sprintf(&result[pos + 1], "u%04x", static_cast<int>(c));
				pos += 6;
				// overwrite trailing null character
				result[pos] = '\\';
			}
			else
			{
				// all other characters are added as-is
				result[pos++] = c;
			}
			break;
		}
		}
	}

	return result;
}