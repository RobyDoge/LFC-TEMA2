#include <iostream>
#include <fstream>

#include "DeterministicFiniteAutomaton.h"
#include "ExpressionToAutomaton.h"
#include "PolishForm.h"

int main()
{
	DeterministicFiniteAutomaton automaton;

	std::ifstream input("input.txt");
	ExpressionToAutomaton expressionToAutomaton(input);
	if (expressionToAutomaton.IsExpressionValid())
	{
		std::cout << "The expression is valid" << std::endl;
		expressionToAutomaton.SetPolishForm();
		automaton = expressionToAutomaton.CreateAutomatonFromPolishForm();
	}
	else
	{
		std::cout << "The expression is not valid" << std::endl;
	}

	std::cout << automaton;
	return 0;
}