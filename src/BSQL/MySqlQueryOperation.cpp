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
	if (connection) {
		//must ensure everything is taken care of

		try {
			//is complete must return twice before we know the result set is empty
			while (!IsComplete() || !IsComplete());
		}
		//can't save it
		catch (std::bad_alloc&) {
			mysql_close(connection);
			connection = nullptr;
		}
		if (result)
			mysql_free_result(result);
		if (connection)
			connPool.ReleaseConnection(connection);
	}
}

void MySqlQueryOperation::StartQuery() {
	if (connection == nullptr)
		return;

	mysql_real_query_start(&queryError, connection, queryText.c_str(), queryText.length());
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
		if (status != 0)
			return false;

		if (queryError) {
			complete = true;
			error = "mysql_real_query() returns error: " + std::string(mysql_error(connection));
			return true;
		}

		queryFinished = true;

		result = mysql_use_result(connection);
		if (!result) {
			//resultless?
			complete = true;
			if(mysql_errno(connection))
				//no it's an error
				error = "mysql_use_result() returns error: " + std::string(mysql_error(connection));
			return true;
		}
		mysql_fetch_row_start(&row, result);
		return false;
	}

	const auto status(mysql_fetch_row_cont(&row, result, 0));

	if (status)
		return false;

	if (row != nullptr) {
		mysql_fetch_row_start(&row, result);
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
	}
	else
		complete = true;
	return true;
}