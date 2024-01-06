#pragma once
#include <list>
#include <string>
#include <istream>

class ExpressionToAutomaton
{
public:
	ExpressionToAutomaton() = default;
	ExpressionToAutomaton(std::istream& input);
	~ExpressionToAutomaton() = default;

	bool IsExpressionValid();

	std::string GetExpression();


private:
	std::string m_expression;
	std::list<char> m_operators;
	std::list<char> m_states;



	//after the polish form is created, we will use it to create the automaton
	//via the CreateAutomatonFromPolishForm() method
	//DeterministicFiniteAutomaton m_automaton;

};

