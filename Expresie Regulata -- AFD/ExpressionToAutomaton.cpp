#include "ExpressionToAutomaton.h"

ExpressionToAutomaton::ExpressionToAutomaton(std::istream& input)
{
	std::getline(input, m_expression);
}

bool ExpressionToAutomaton::IsExpressionValid()
{
	return true;
}

std::string ExpressionToAutomaton::GetExpression()
{
	return m_expression;
}
