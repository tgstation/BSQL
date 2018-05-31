set -e
retval=1
source $HOME/BYOND-${BYOND_MAJOR}.${BYOND_MINOR}/byond/bin/byondsetup

if hash DreamMaker 2>/dev/null
then
	DreamMaker travistester.dme 2>&1 | tee result.log
	retval=$?
	if ! grep '\- 0 errors, 0 warnings' result.log
	then
		retval=1 #hard fail, due to warnings or errors
	else
		#move in the artifacts
		cp ../artifacts/BSQL.so ./
		cp $HOME/vcpkg/packages/libmariadb/libmariadb_x86-linux/lib/libmariadb.so ./
		DreamDaemon travistester.dmb -close -trusted -params "dbaddr=127.0.0.1&dbport=3306&dbuser=root&dbpass="
		cat clean_run.lk
	fi
else
	echo "Couldn't find the DreamMaker executable, aborting."
	retval=2
fi
exit $retval