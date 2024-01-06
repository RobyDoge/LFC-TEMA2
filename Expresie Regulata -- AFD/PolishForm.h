#pragma once
#include <stack>
#include <string>

class PolishForm
{
public:
	PolishForm() = default;
	PolishForm(std::string expression);
	~PolishForm() = default;

	std::string GetResult();

private:
	std::stack<char> m_operators;
	std::string m_result;

private:
	static int GetOperatorPrecedence(char op);
	std::string ToPolishForm(const std::string& expression);
};

