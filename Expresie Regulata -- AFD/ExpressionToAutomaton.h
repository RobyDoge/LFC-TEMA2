#pragma once
#include <stack>
#include <string>
#include <istream>

#include "DeterministicFiniteAutomaton.h"

class ExpressionToAutomaton
{
public:
	ExpressionToAutomaton() = default;
	ExpressionToAutomaton(std::istream& input);
	~ExpressionToAutomaton() = default;

public:
	DeterministicFiniteAutomaton CreateAutomatonFromPolishForm();

	bool IsExpressionValid();

	std::string GetExpression();
	void SetPolishForm();


private:
	std::string m_expression;
	std::string m_polishForm;

	std::stack<DeterministicFiniteAutomaton> m_intermediateAutomatons;

private:
	void LetterStep(char& index, char character);
	void ConcatenationStep();
	void OrStep(char& index);
	void KleeneStarStep(char& index);


	void EliminateDuplicates(std::string& string);
};

