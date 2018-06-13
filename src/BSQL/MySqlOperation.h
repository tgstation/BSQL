#pragma once

class MySqlOperation {
public:
	static bool Poll(MYSQL* mysql, std::chrono::milliseconds timeoutEpoch, int& status);
	static std::chrono::milliseconds GetTimeout(MYSQL* mysql);
};