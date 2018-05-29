#pragma once

class Query : public Operation {
private:
	std::string currentRow;
public:
	std::string CurrentRow() const;

	bool BeginGetNextRow();
};