#pragma once


class Operation {
protected:
	std::string error;
public:
	virtual ~Operation() = default;

	std::string GetError();

	virtual bool IsComplete(bool noOps) = 0;
	virtual bool IsQuery() = 0;
};
