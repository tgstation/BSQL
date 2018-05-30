#pragma once

class Query : public Operation {
private:
	std::string currentRow;
public:
	std::string CurrentRow() const;

	virtual bool BeginGetNextRow() = 0;
};