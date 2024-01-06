#pragma once
#include <list>
#include <string>
#include <istream>

#include "PolishForm.h"

class ExpressionToAutomaton
{
public:
	ExpressionToAutomaton() = default;
	ExpressionToAutomaton(std::istream& input);
	~ExpressionToAutomaton() = default;

	bool IsExpressionValid();

	std::string GetExpression();
	void SetPolishForm();


private:
	std::string m_expression;
	std::list<char> m_operators;
	std::list<char> m_states;

	PolishForm m_polishForm;
	//after the polish form is created, we will use it to create the automaton
	//via the CreateAutomatonFromPolishForm() method
	//DeterministicFiniteAutomaton m_automaton;

};

