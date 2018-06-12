#pragma once

class MySqlOperation {
public:
	static int Poll(MYSQL* mysql, std::chrono::milliseconds timeoutEpoch, int status);
	static std::chrono::milliseconds GetTimeout(MYSQL* mysql);
};