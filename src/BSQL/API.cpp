#include "BSQL.h"

std::unique_ptr<Library> library;
std::string lastCreatedConnection, lastCreatedOperation, lastCreatedOperationConnectionId, lastRow, returnValueHolder;

const char* TryLoadQuery(const int argumentCount, const char* const* const args, Query** query) noexcept {
	if (argumentCount != 2)
		return "Invalid arguments!";
	const auto& connectionIdentifier(args[0]), operationIdentifier(args[1]);
	if (!connectionIdentifier)
		return "Invalid connection identifier!";
	if (!operationIdentifier)
		return "Invalid operation identifier!";

	try {
		auto connection(library->GetConnection(lastCreatedOperationConnectionId));
		if (!connection)
			return "Connection identifier does not exist!";
		auto operation(connection->GetOperation(operationIdentifier));
		if (!operation)
			return "Operation identifier does not exist!";
		if (!operation->IsQuery())
			return "Operation is not a query!";
		*query = static_cast<Query*>(operation);
		return nullptr;
	}
	catch (std::bad_alloc&) {
		return "Out of memory!";
	}
}

extern "C" {
	BYOND_FUNC Initialize(const int argumentCount, const char* const* const args) noexcept {
		try {
			library = std::make_unique<Library>();
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
		return nullptr;
	}

	BYOND_FUNC Shutdown(const int argumentCount, const char* const* const args) noexcept {
		if (library) {
			if (!lastCreatedConnection.empty())
				lastCreatedConnection = std::string();
			if (!lastCreatedOperation.empty())
				lastCreatedOperation = std::string();
			library.reset();
		}
		return nullptr;
	}

	BYOND_FUNC GetError(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 2)
			return "Invalid arguments!";
		const auto& connectionIdentifier(args[0]), operationIdentifier(args[1]);
		if (!library)
			return "Library not initialized!";
		if (!connectionIdentifier)
			return "Invalid connection identifier!";
		if (!operationIdentifier)
			return "Invalid operation identifier!";
		try {
			auto connection(library->GetConnection(connectionIdentifier));
			if (!connection)
				return "Connection identifier does not exist!";
			auto operation(connection->GetOperation(operationIdentifier));
			if (!operation)
				return "Operation identifier does not exist!";
			if (!operation->IsComplete(true))
				return "Operation is not complete!";
			returnValueHolder = operation->GetError();
			return returnValueHolder.c_str();
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
	}

	BYOND_FUNC CreateConnection(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 1)
			return "Invalid arguments!";
		if (!library)
			return "Library not initialized!";
		const auto& connectionType(args[0]);
		Connection::Type type;
		try {
			std::string conType(connectionType);
			if (conType == "MySql")
				type = Connection::Type::MySql;
			else if (conType == "SqlServer")
				//type = Connection::Type::SqlServer;
				return "SqlServer is not supported in this release!";
			else
				return "Invalid connection type!";
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}

		if (!lastCreatedConnection.empty())
			//guess they didn't want it
			library->ReleaseConnection(lastCreatedConnection);

		auto result(library->CreateConnection(type));
		if (result.empty())
			return "Out of memory";

		lastCreatedConnection = std::move(result);
		return nullptr;
	}

	BYOND_FUNC GetConnection(const int argumentCount, const char* const* const args) noexcept {
		if (!library || lastCreatedConnection.empty())
			return nullptr;
		returnValueHolder = std::string();
		std::swap(returnValueHolder, lastCreatedConnection);
		return returnValueHolder.c_str();
	}

	BYOND_FUNC ReleaseConnection(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 1)
			return "Invalid arguments!";
		const auto& connectionIdentifier(args[0]);
		if (!connectionIdentifier)
			return "Invalid connection identifier!";
		if (!library)
			return "Library not initialized!";
		try {
			if (!library->ReleaseConnection(connectionIdentifier))
				return "Connection identifier does not exist!";
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
		return nullptr;
	}

	BYOND_FUNC GetOperation(const int argumentCount, const char* const* const args) noexcept {
		if (!library || lastCreatedOperation.empty())
			return nullptr;
		returnValueHolder = std::string();
		std::swap(returnValueHolder, lastCreatedOperation);
		return returnValueHolder.c_str();
	}

	BYOND_FUNC ReleaseOperation(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 2)
			return "Invalid arguments!";
		const auto& connectionIdentifier(args[0]), operationIdentifier(args[1]);
		if (!connectionIdentifier)
			return "Invalid connection identifier!";
		if (!operationIdentifier)
			return "Invalid operation identifier!";
		if (!library)
			return "Library not initialized!";
		try {
			auto connection(library->GetConnection(connectionIdentifier));
			if (!connection)
				return "Connection identifier does not exist!";
			if (!connection->ReleaseOperation(operationIdentifier))
				return "Operation identifier does not exist!";
			return nullptr;
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
	}

	BYOND_FUNC OpenConnection(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 6)
			return "Invalid arguments!";
		const auto& connectionIdentifier(args[0]), ipaddress(args[1]), port(args[2]), username(args[3]), password(args[4]), database(args[5]);

		if (!connectionIdentifier)
			return "Invalid connection identifier!";
		if (!ipaddress)
			return "Invalid ip address!";
		if (!port)
			return "Invalid port!";
		if (!password)
			return "Invalid password!";
		unsigned short realPort;
		bool outOfRange;
		try {
			auto asInt(std::stoi(port));
			outOfRange = asInt < 0 || asInt > std::numeric_limits<unsigned short>::max();
			if (!outOfRange)
				realPort = asInt;
		}
		catch (std::invalid_argument&) {
			return "Port is not a number!";
		}
		catch (std::out_of_range&) {
			outOfRange = true;
		}
		if (outOfRange)
			return "Port is out of acceptable range!";

		if (!library)
			return "Library not initialized!";
		try {
			//clear the cache
			GetOperation(0, nullptr);
			lastCreatedOperationConnectionId = connectionIdentifier;
			auto connection(library->GetConnection(lastCreatedOperationConnectionId));
			if (!connection)
				return "Connection identifier does not exist!";
			lastCreatedOperation = connection->Connect(ipaddress, realPort, username, password, database);
			return nullptr;
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
	}

	BYOND_FUNC NewQuery(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 2)
			return "Invalid arguments!";
		const auto& connectionIdentifier(args[0]), queryText(args[1]);
		if (!connectionIdentifier)
			return "Invalid connection identifier!";
		if (!queryText)
			return "Invalid query text!";
		if (!library)
			return "Library not initialized!";
		try {
			//clear the cache
			GetOperation(0, nullptr);
			auto connection(library->GetConnection(lastCreatedOperationConnectionId));
			if (!connection)
				return "Connection identifier does not exist!";
			lastCreatedOperation = connection->CreateQuery(queryText);
			if (lastCreatedOperation.empty())
				return "Error creating query! Is the connection complete?";
			return nullptr;
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
	}

	BYOND_FUNC OpComplete(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 2)
			return nullptr;
		const auto& connectionIdentifier(args[0]), operationIdentifier(args[1]);
		if (!connectionIdentifier || !operationIdentifier)
			return nullptr;
		try {
			auto connection(library->GetConnection(lastCreatedOperationConnectionId));
			if (!connection)
				return nullptr;
			auto operation(connection->GetOperation(operationIdentifier));
			if (!operation)
				return nullptr;
			return operation->IsComplete(false) ? "DONE" : "NOTDONE";
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
	}

	BYOND_FUNC GetRow(const int argumentCount, const char* const* const args) noexcept {
		if (!library)
			return "Library not initialized!";
		if (lastRow.empty())
			return nullptr;
		returnValueHolder = std::string();
		std::swap(returnValueHolder, lastRow);
		return returnValueHolder.c_str();
	}

	BYOND_FUNC ReadyRow(const int argumentCount, const char* const* const args) noexcept {
		Query* query;
		auto res(TryLoadQuery(argumentCount, args, &query));
		if (res != nullptr)
			return res;
		try {
			if (query->IsComplete(false)) {
				lastRow = query->CurrentRow();
				return "DONE";
			}
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
		return "NOTDONE";
	}

	BYOND_FUNC QuoteString(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 2)
			return nullptr;
		auto connectionIdentifier(args[0]), str(args[1]);
		if (!connectionIdentifier || !str || !library)
			return nullptr;

		try {
			//clear the cache
			auto connection(library->GetConnection(connectionIdentifier));
			if (!connection)
				return nullptr;
			returnValueHolder = connection->Quote(str);
			return returnValueHolder.c_str();
		}
		catch (std::bad_alloc&) {
			return nullptr;
		}
		catch (std::runtime_error&) {
			return nullptr;
		}
	}

	BYOND_FUNC BlockOnOperation(const int argumentCount, const char* const* const args) noexcept {
		if (argumentCount != 2)
			return "Invalid arguments!";
		const auto& connectionIdentifier(args[0]), operationIdentifier(args[1]);
		if (!connectionIdentifier)
			return "Invalid connection identifier!";
		if (!operationIdentifier)
			return "Invalid operation identifier!";
		if (!library)
			return "Library not initialized!";
		try {
			auto connection(library->GetConnection(connectionIdentifier));
			if (!connection)
				return "Connection identifier does not exist!";
			auto op(connection->GetOperation(operationIdentifier));
			if (!op)
				return "Operation identifier does not exist!";
			while (!op->IsComplete(false))
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			return nullptr;
		}
		catch (std::bad_alloc&) {
			return "Out of memory!";
		}
	}
}