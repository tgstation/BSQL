#pragma once

class Operation {
private:
	std::string error;
public:
	const std::string& GetError() const;

	virtual bool IsComplete() const = 0;
	virtual bool IsQuery() const = 0;
};
