#include "BSQL.h"

Connection::Connection(Type type, Library& library, const unsigned int blockingTimeout) :
	blockingTimeout(blockingTimeout),
	library(library),
	type(type),
	identifierCounter(0)
{}

std::string Connection::AddOp(std::unique_ptr<Operation>&& operation) {
	auto identifier(std::to_string(++identifierCounter));
	operations.emplace(identifier, std::move(operation));
	return identifier;
}

bool Connection::ReleaseOperation(const std::string& identifier) {
	auto op(GetOperation(identifier));
	if (!op)
		return false;
	auto thread(op->GetActiveThread());
	if (thread)
		library.RegisterZombieThread(std::move(*thread));
	return operations.erase(identifier) > 0;
}

Operation* Connection::GetOperation(const std::string& identifier) {
	auto res(operations.find(identifier));
	if (res == operations.end())
		return nullptr;
	return res->second.get();
}
