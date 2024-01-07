#include <iostream>
#include <fstream>

#include "DeterministicFiniteAutomaton.h"
#include "ExpressionToAutomaton.h"
#include "PolishForm.h"

int main()
{
	std::ifstream input("input.txt");
	ExpressionToAutomaton expressionToAutomaton(input);
	if (expressionToAutomaton.IsExpressionValid())
	{
		std::cout << "The expression is valid" << std::endl;
		expressionToAutomaton.SetPolishForm();
		expressionToAutomaton.CreateAutomatonFromPolishForm();
	}
	else
	{
		std::cout << "The expression is not valid" << std::endl;
	}

	DeterministicFiniteAutomaton automaton;
	std::cout << automaton;
	return 0;
}