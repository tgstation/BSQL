/datum/BSQL_Operation
	var/datum/BSQL_Connection/connection
	var/id

BSQL_PROTECT_DATUM(/datum/BSQL_Operation)

/datum/BSQL_Operation/New(datum/BSQL_Connection/connection, id)
	src.connection = connection
	src.id = id

BSQL_DEL_PROC(/datum/BSQL_Operation)
	var/error
	if(connection)
		error = world._BSQL_Internal_Call("ReleaseOperation", connection.id, id)
	. = ..()
	if(error)
		BSQL_ERROR(error)

/datum/BSQL_Operation/IsComplete()
	var/result = world._BSQL_Internal_Call("OpComplete", connection.id, id)
	if(!result)
		BSQL_ERROR("Error fetching operation [id] for connection [connection.id]!")
		return
	return result == "DONE"

/datum/BSQL_Operation/GetError()
	return world._BSQL_Internal_Call("GetError", connection.id, id)
