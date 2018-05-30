#pragma once


class Operation {
protected:
	std::string error;
public:
	virtual ~Operation() = 0;

	std::string GetError();

	virtual bool IsComplete() = 0;
	virtual bool IsQuery() = 0;
};
