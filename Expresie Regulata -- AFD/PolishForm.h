#pragma once
#include <stack>
#include <string>

class PolishForm
{
public:
	PolishForm() = default;
	PolishForm(std::string expression);
	~PolishForm() = default;

private:
	std::stack<char> m_operators;

private:
	static int GetOperatorPrecedence(char op);
	std::string ToPolishForm(const std::string& expression);
};

