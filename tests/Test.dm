/world/New()
	log << "world/New"
	TestStart()

/world/Error()
	. = ..()
	fdel("clean_run.lk")
	del(src)

/proc/TestStart()
	set waitfor = FALSE
	world.log << "TestStart"
	sleep(10)
	world.log << "Init time elapsed"
	//run the test 10 times for those awkward race conditions
	var/fail = FALSE
	for(var/I in 1 to 10)
		if(!Test())
			fail = TRUE
			break
	if(!fail)
		text2file("Success!", "clean_run.lk")
	del(world)

/proc/WaitOp(datum/BSQL_Operation/op)
	world.log << "Waiting on op [op.id] (conn: [op.connection.id])"
	while(!op.IsComplete())
		sleep(1)
	world.log << "Op [op.id] (conn: [op.connection.id]) complete"

/proc/DelOp(datum/BSQL_Operation/op)
	world.log << "Deleting operation [op.id] (conn: [op.connection.id])"
	del(op)

/proc/Test()
	world.log << "Beginning test"

	var/host = world.params["dbhost"]
	var/user = world.params["dbuser"]
	var/port = text2num(world.params["dbport"])
	var/pass = world.params["dbpass"]
	var/db = world.params["dbdb"]

	var/datum/BSQL_Connection/conn = new(BSQL_CONNECTION_TYPE_MARIADB)
	world.log << "Root connection id: [conn.id]"
	var/datum/BSQL_Operation/connectOp = conn.BeginConnect(host, port, user, pass, null)
	world.log << "Connect op id: [connectOp.id]"

	WaitOp(connectOp)
	var/error = connectOp.GetError()
	if(error)
		CRASH(error)
	DelOp(connectOp)

	var/quoted_db = conn.Quote(db)
	world.log << "Db quoted: [quoted_db]"

	var/other_quote_test = "m'brapper"
	var/quote_test = conn.Quote(other_quote_test)
	if(other_quote_test == quote_test)
		CRASH("Failed to quote \"[other_quote_test]\"! Got: \"[quote_test]\"")
	world.log << "Quoted \"[other_quote_test]\" to \"[quote_test]\""

	var/datum/BSQL_Operation/Query/q = conn.BeginQuery("DROP DATABASE IF EXISTS [quoted_db]");
	world.log << "Drop db op id: [q.id]"
	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)
	DelOp(q)

	q = conn.BeginQuery("CREATE DATABASE [quoted_db]");
	world.log << "Create db op id: [q.id]"
	q.WaitForCompletion()
	if(!q.IsComplete())
		CRASH("Wait for completion didn't work!")
	error = q.GetError()
	if(error)
		CRASH(error)
	DelOp(q)

	conn = new(BSQL_CONNECTION_TYPE_MARIADB)
	world.log << "Db connection id: [conn.id]"
	connectOp = conn.BeginConnect(host, port, user, pass, db)
	world.log << "Db connect op id: [connectOp.id]"
	WaitOp(connectOp)
	error = connectOp.GetError()
	if(error)
		CRASH(error)
	DelOp(connectOp)
	
	q = conn.BeginQuery("CREATE TABLE `asdf` (`id` int(11) NOT NULL AUTO_INCREMENT, `datetime` datetime NOT NULL, `round_id` int(11) unsigned NOT NULL, PRIMARY KEY (`id`))")
	world.log << "Create table op id: [q.id]"
	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)
	DelOp(q)
	
	q = conn.BeginQuery("INSERT INTO asdf (datetime, round_id) VALUES (NOW(), 42)")
	world.log << "Insert 1 op id: [q.id]"
	
	var/datum/BSQL_Operation/Query/q2 = conn.BeginQuery("INSERT INTO asdf (datetime, round_id) VALUES ('[time2text(world.timeofday, "YYYY-MM-DD hh:mm:ss")]', 77)")
	world.log << "Insert 2 op id: [q2.id]"

	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)
	WaitOp(q2)
	error = q2.GetError()
	if(error)
		CRASH(error)
	DelOp(q2)
	DelOp(q)

	q = conn.BeginQuery("SELECT * FROM asdf")
	world.log << "Select op id: [q.id]"
	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)
	DelOp(q)


	var/list/results = q.CurrentRow()
	if(!results)
		CRASH("No results!")

	world.log << json_encode(results)

	if(results.len != 3)
		CRASH("Expected 3 columns, got [results.len]!")

	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)
	results = q.CurrentRow()
	if(!results)
		CRASH("No second results!")
	DelOp(q)

	world.log << json_encode(results)

	if(results.len != 3)
		CRASH("Second row: Expected 3 columns, got [results.len]!")

	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)

	results = q.CurrentRow()
	if(results)
		CRASH("Expected no third row! Got: [json_encode(results)] !")
		
	DelOp(q)

	del(conn)

	world.BSQL_Shutdown()

	return TRUE
