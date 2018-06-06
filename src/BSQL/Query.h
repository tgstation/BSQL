#pragma once

class Query : public Operation {
protected:
	std::string currentRow;
public:
	std::string CurrentRow() const;

	bool IsQuery() override;
};