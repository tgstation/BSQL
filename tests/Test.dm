/world/New()
	TestStart()

/world/Error()
	. = ..()
	del(src)

/proc/TestStart()
	set waitfor = FALSE
	sleep(10)
	Test()
	del(world)

/proc/WaitOp(datum/BSQL_Operation/op)
	while(!op.IsComplete())
		sleep(10)

/proc/Test()
	var/datum/BSQL_Connection/conn = new
	var/datum/BSQL_Operation/connectOp = conn.BeginConnect(world.params["dbaddr"], text2num(world.params["dbport"]), world.params["dbuser"], world.params["dbpass"])

	WaitOp(connectOp)
	del(connectOp)
	var/datum/BSQL_Operation/Query/q = conn.BeginQuery("CREATE DATABASE BSQLTest; USE BSQLTest");

	WaitOp(q)

	q = connectOp.BeginQuery("CREATE TABLE `asdf` (`id` int(11) NOT NULL AUTO_INCREMENT, `datetime` datetime NOT NULL, `round_id` int(11) unsigned NOT NULL, PRIMARY KEY (`id`))")
	WaitOp(q)

	q = conn.BeginQuery("INSERT INTO asdf (datetime, round_id) VALUES (NOW(), 42)")
	var/datum/BSQL_Operation/Query/q2 = conn.BeginQuery("INSERT INTO asdf (datetime, round_id) VALUES ([time2text(world.timeofday, "YYYY-MM-DD hh:mm:ss")], 77)")

	WaitOp(q)
	WaitOp(q2)
	del(q2)

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
