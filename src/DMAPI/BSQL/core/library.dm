/world/proc/_BSQL_Internal_Call(func, ...)
	return call(system_type == MS_WINDOWS ? "BSQL.dll" : "BSQL.so", func)(arglist(args.Copy(1)))

/world/proc/_BSQL_Initialized(new_val)
	var/static/bsql_library_initialized = FALSE
	if(new_val != null)
		bsql_library_initialized = new_val
	return bsql_library_initialized

/world/BSQL_Shutdown()
	BSQL_Internal_Call("Shutdown")
	_BSQL_Initialized(FALSE)
