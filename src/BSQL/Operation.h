#pragma once


class Operation {
protected:
	struct ClassState {
		std::mutex lock;
		bool alive = true;
	};
protected:
	int errnum;
	std::string error;
public:
	virtual ~Operation() = default;

	std::string GetError();
	int GetErrno();

	virtual bool IsComplete(bool noSkip) = 0;
	virtual bool IsQuery() = 0;
	virtual std::thread* GetActiveThread() = 0;
};
