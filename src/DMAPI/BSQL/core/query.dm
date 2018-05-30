/datum/BSQL_Operation/Query
	var/last_result_json
	var/list/last_result

BSQL_PROTECT_DATUM(/datum/BSQL_Operation/Query)

/datum/BSQL_Operation/Query/CurrentRow()
	return last_result

/datum/BSQL_Operation/Query/BeginFetchNextRow()
	var/error = world._BSQL_Internal_Call("BeginFetchNextRow", connection.id, id)
	if(error)
		BSQL_ERROR(error)

/datum/BSQL_Operation/Query/IsComplete()
	//whole different ballgame here
	var/result = world._BSQL_Internal_Call("ReadyRow", connection.id, id)
	switch(result)
		if("DONE")
			//load the data
			last_result_json = world._BSQL_Internal_Call("GetRow", connection.id, id)
			last_result = json_decode(last_result_json)
			return TRUE
		if("NOTDONE")
			return FALSE
		else
			BSQL_ERROR(result)
