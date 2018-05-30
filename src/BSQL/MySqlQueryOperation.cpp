#include "BSQL.h"

MySqlQueryOperation::MySqlQueryOperation(MySqlConnection& connPool, std::string&& queryText) :
	queryText(queryText),
	connPool(connPool)
{

}


bool MySqlQueryOperation::IsQuery() {
	return true;
}
