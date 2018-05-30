#include "BSQL.h"

MySqlQueryOperation::MySqlQueryOperation(MySqlConnection& connPool, std::string&& queryText) :
	queryText(queryText),
	connPool(connPool),
	connection(connPool.RequestConnection()),
	result(nullptr),
	complete(false),
	queryFinished(false)
{
	StartQuery();
}

MySqlQueryOperation::~MySqlQueryOperation() {
	if (result)
		mysql_free_result(result);
	if(connection)
		connPool.ReleaseConnection(connection);
}

void MySqlQueryOperation::StartQuery() {
	if (connection == nullptr)
		return;
	mysql_real_query_start(&queryError, connection, queryText.c_str(), queryText.length());
}

bool MySqlQueryOperation::BeginGetNextRow() {
	if (!IsComplete() || !error.empty())
		return false;

	mysql_fetch_row_start(&row, result);
	return true;
}

bool MySqlQueryOperation::IsComplete() {
	if (complete)
		return true;

	if (!connection) {
		connection = connPool.RequestConnection();
		StartQuery();
		return false;
	}

	if (!queryFinished) {
		const auto status(mysql_real_query_cont(&queryError, connection, 0));
		if (queryError) {
			complete = true;
			error = "mysql_real_query() returns error " + std::to_string(queryError);
			return true;
		}

		queryFinished = true;

		result = mysql_use_result(connection);
		if (!result)
			throw std::bad_alloc();

		BeginGetNextRow();
		return false;
	}

	const auto status(mysql_fetch_row_cont(&row, result, 0));

	complete = status == 0;
	if (!complete)
		return false;

	struct ColInfo {
		std::string name;
		enum_field_types type;
	};

	std::string json("{");
	bool first(true);
	const auto numFields(mysql_num_fields(result));
	for (auto I(0U); I < numFields; ++I) {
		const auto field(mysql_fetch_field(result));
		if (first)
			first = false;
		else
			json.append(",");
		json.append("\"");
		json.append(field->name);
		json.append("\"");
		if (row[I] == nullptr)
			json.append("null");
		else {
				json.append("\"");
				json.append(row[I]);
				json.append("\"");
		}
	}
	json.append("}");

	currentRow = std::move(json);
	return true;
}