//BSQL - DMAPI

//types of connections
#define BSQL_CONNECTION_TYPE_MARIADB "MySql"
#define BSQL_CONNECTION_TYPE_SQLSERVER "SqlServer"

//Call this before rebooting or shutting down your world to clean up gracefully. This invalidates all active connection and operation datums
/world/proc/BSQL_Shutdown()
	return

/*
Create a new database connection, does not perform the actual connect
  connection_type: The BSQL connection_type to use
*/
/datum/BSQL_Connection/New(connection_type)
	return ..()

/*
Starts an operation to connect to a database. Should only have 1 successful call
  ipaddress: The ip/hostname of the target server
  port: The port of the target server
  username: The username to login to the target server
  password: The password for the target server
 Returns: A /datum/BSQL_Operation representing the connection or null if an error occurred
*/
/datum/BSQL_Connection/proc/BeginConnect(ipaddress, port, username, password)
	return

/*
Starts an operation for a query
  query: The text of the query
 Returns: A /datum/BSQL_Operation/Query representing the running query and subsequent result set or null if an error occurred
*/
/datum/BSQL_Connection/proc/BeginQuery(query)
	return

/*
Checks if the operation is complete

 Returns: TRUE if the operation is complete, FALSE if it's not, null on error
*/
/datum/BSQL_Operation/proc/IsComplete()
	return

/*
Get the error message associated with an operation. Should not be used while IsComplete() returns FALSE

 Returns: The error message, if any. null otherwise
*/
/datum/BSQL_Operation/proc/GetError()
	return

/*
Gets an associated list of column name -> value representation of the most recent row in the query. Only valid if IsComplete() returns TRUE. If this returns null and no errors are present there are no more results in the query.

 Returns: An associated list of column name -> value for the row
*/
/datum/BSQL_Operation/Query/proc/CurrentRow()
	return

/*
Starts a fetch of the next query in the result set. Only valid if IsComplete() returns TRUE. IsComplete() will return FALSE after this is called
*/
/datum/BSQL_Operation/Query/proc/BeginFetchNextRow()
	return

/* 
Code configuration options below

Define this to avoid modifying this file but the following defines must be declared somewhere else before BSQL/includes.dm is included
*/
#ifndef BSQL_EXTERNAL_CONFIGURATION

//Modify this if you disagree with byond's GC schemes. Ensure this is called for all connections and operations when they are deleted or they will leak native resources until /world/proc/BSQL_Shutdown() is called
#define BSQL_DEL_PROC(path) ##path/Del()

//Modify this to add protections to the connection and query datums
#define BSQL_PROTECT_DATUM(path)

//Modify this to change up error handling for the library
#define BSQL_ERROR(message) CRASH("BSQL: [##message]")

#endif


Copyright 2018 Jordan Brown

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.