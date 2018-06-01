/world/New()
	log << "world/New"
	TestStart()

/world/Error()
	. = ..()
	del(src)

/proc/TestStart()
	set waitfor = FALSE
	world.log << "TestStart"
	sleep(10)
	world.log << "Init time elapsed"
	Test()
	del(world)

/proc/WaitOp(datum/BSQL_Operation/op)
	world.log << "Waiting on op [op.id] (conn: [op.connection.id])"
	while(!op.IsComplete())
		sleep(10)
	world.log << "Op [op.id] (conn: [op.connection.id]) complete"

/proc/Test()
	world.log << "Beginning test"
	var/datum/BSQL_Connection/conn = new(BSQL_CONNECTION_TYPE_MARIADB)
	world.log << "Connection id: [conn.id]"
	var/datum/BSQL_Operation/connectOp = conn.BeginConnect(world.params["dbaddr"], text2num(world.params["dbport"]), world.params["dbuser"], world.params["dbpass"])
	world.log << "Connect op id: [connectOp.id]"

	WaitOp(connectOp)
	del(connectOp)
	var/datum/BSQL_Operation/Query/q = conn.BeginQuery("CREATE DATABASE BSQLTest; USE BSQLTest");
	world.log << "Create db op id: [q.id]"
	WaitOp(q)
	
	q = conn.BeginQuery("CREATE TABLE `asdf` (`id` int(11) NOT NULL AUTO_INCREMENT, `datetime` datetime NOT NULL, `round_id` int(11) unsigned NOT NULL, PRIMARY KEY (`id`))")
	world.log << "Create table op id: [q.id]"
	WaitOp(q)
	
	q = conn.BeginQuery("INSERT INTO asdf (datetime, round_id) VALUES (NOW(), 42)")
	world.log << "Insert 1 op id: [q.id]"
	
	var/datum/BSQL_Operation/Query/q2 = conn.BeginQuery("INSERT INTO asdf (datetime, round_id) VALUES ([time2text(world.timeofday, "YYYY-MM-DD hh:mm:ss")], 77)")
	world.log << "Insert 2 op id: [q2.id]"

	WaitOp(q)
	WaitOp(q2)
	del(q2)
	
	world.log << "Select op id: [q.id]"
	q = conn.BeginQuery("SELECT * FROM asdf")
	WaitOp(q)

	var/error = q.GetError()
	if(error)
		CRASH(error)

	var/list/results = q.CurrentRow()
	if(!results)
		CRASH("No results!")

	world.log << json_encode(results)

	if(results.len != 3)
		CRASH("Expected 3 columns, got [results.len]!")

	q.BeginFetchNextRow()
	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)
	results = q.CurrentRow()
	if(!results)
		CRASH("No second results!")

	world.log << json_encode(results)

	if(results.len != 3)
		CRASH("Second row: Expected 3 columns, got [results.len]!")

	q.BeginFetchNextRow()
	WaitOp(q)
	error = q.GetError()
	if(error)
		CRASH(error)

	results = q.CurrentRow()
	if(results)
		CRASH("Expected no third row! Got: [json_encode(results)] !")

	del(q)
	del(conn)

	world.BSQL_Shutdown()

	text2file("Success!", "clean_run.lk")
