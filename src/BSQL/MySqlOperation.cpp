#include "BSQL.h"

int MySqlOperation::Poll(MYSQL* mysql, std::chrono::milliseconds timeoutEpoch, int status) {
	struct pollfd pfd;
	pfd.fd = mysql_get_socket(mysql);
	pfd.events =
		(status & MYSQL_WAIT_READ ? POLLIN : 0) |
		(status & MYSQL_WAIT_WRITE ? POLLOUT : 0) |
		(status & MYSQL_WAIT_EXCEPT ? POLLPRI : 0);

	auto res(poll(&pfd, 1, -1));
	auto waitingForTimeout((status & MYSQL_WAIT_TIMEOUT) != 0);
	status = 0;
	if (res <= 0) {
		if (waitingForTimeout && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) > timeoutEpoch)
			status |= MYSQL_WAIT_TIMEOUT;
	}
	else {
		if (pfd.revents & POLLIN) status |= MYSQL_WAIT_READ;
		if (pfd.revents & POLLOUT) status |= MYSQL_WAIT_WRITE;
		if (pfd.revents & POLLPRI) status |= MYSQL_WAIT_EXCEPT;
	}
	return status;
}

std::chrono::milliseconds MySqlOperation::GetTimeout(MYSQL* mysql) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) + std::chrono::milliseconds(mysql_get_timeout_value_ms(mysql));
}
