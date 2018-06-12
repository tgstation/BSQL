#include "BSQL.h"

MySqlQueryOperation::MySqlQueryOperation(MySqlConnection& connPool, std::string&& queryText) :
	queryText(std::move(queryText)),
	connPool(connPool),
	connection(connPool.RequestConnection(queryText)),
	result(nullptr),
	connectFailCount(0),
	complete(false),
	queryFinished(false)
{
	StartQuery();
}

MySqlQueryOperation::~MySqlQueryOperation() {
	if (!connection)
		return;
	//must ensure everything is taken care of
	bool kill;
	try {
		kill = !IsComplete(false) || !complete;
	}
	//can't save it
	catch (std::bad_alloc&) {
		kill = true;
	}
	if (result)
		mysql_free_result(result);
	if (kill)
		connPool.KillConnection(connection);
	else
		connPool.ReleaseConnection(connection);
}

void MySqlQueryOperation::StartQuery() {
	if (!connection)
		return;

	mysql_real_query_start(&queryError, connection, queryText.c_str(), queryText.length());
}

bool MySqlQueryOperation::IsComplete(bool noOps) {
	if (complete)
		return true;

	if (!connection) {
		std::string fail;
		connection = connPool.RequestConnection(fail);
		if (!fail.empty() && ++connectFailCount == 3) {
			complete = true;
			error = "Failed to establish connection from connection pool: " + fail;
			return false;
		}
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
		waitNext = mysql_fetch_row_start(&row, result) != 0;
		if (waitNext)
			return false;
	}
	
	if (waitNext) {
		const auto status(mysql_fetch_row_cont(&row, result, 0));

		if (status != 0)
			return false;
	}

	if (row != nullptr) {
		std::string json("{");
		bool first(true);
		const auto numFields(mysql_num_fields(result));
		mysql_field_seek(result, 0);
		for (auto I(0U); I < numFields; ++I) {
			const auto field(mysql_fetch_field(result));
			if (first)
				first = false;
			else
				json.append(",");
			json.append("\"");
			json.append(field->name);
			json.append("\":");
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

		if(!noOps)
			waitNext = mysql_fetch_row_start(&row, result) != 0;
	}
	else {
		//resultless?
		complete = true;
		if (mysql_errno(connection))
			//no it's an error
			error = "mysql_fetch_row() returns error: " + std::string(mysql_error(connection));
		else
			currentRow = std::string();
	}
	return true;
}