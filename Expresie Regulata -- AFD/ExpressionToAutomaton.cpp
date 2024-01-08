#include "ExpressionToAutomaton.h"

#include <algorithm>

#include "PolishForm.h"

ExpressionToAutomaton::ExpressionToAutomaton(std::istream& input)
{
	std::getline(input, m_expression);
}

DeterministicFiniteAutomaton ExpressionToAutomaton::CreateAutomatonFromPolishForm()
{
	char index = 'A';

	for(auto& character: m_polishForm)
	{
		if(std::isalnum(character))
		{
			LetterStep(index, character);
		}
		else if(character == '.')
		{
			ConcatenationStep();
		}
		else if(character == '|')
		{
			OrStep(index);
		}
		else if(character == '*')
		{

			KleeneStarStep(index);
		}
	}

	DeterministicFiniteAutomaton automaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();
	return automaton;
}


bool ExpressionToAutomaton::IsExpressionValid()
{
	std::stack<char> parenthesesStack;

	auto isValidOperand = [](char c)
	{
		return isalnum(c) || isdigit(c);

	};

	int operatorIndex = 0;

	for (char c : m_expression) 
	{
		if (c == '(') 
		{
			parenthesesStack.push(c);
		}

		else if (c == ')') 
		{
			if (parenthesesStack.empty() || parenthesesStack.top() != '(') {
				return false; 
			}
			parenthesesStack.pop();
		}

		
		else if (c == '.' || c == '|') 
		{
			
			if (!isValidOperand(m_expression[operatorIndex - 1]) && 
				(m_expression[operatorIndex - 1] != '*' && m_expression[operatorIndex - 1] != ')')
				) 
			{
				return false;
			}

			
			if (!isValidOperand(m_expression[operatorIndex + 1]) && m_expression[operatorIndex+1]!='(') 
			{
				return false; 
			}
		}
		else if (c == '*') 
		{
			if (!isValidOperand(m_expression[operatorIndex - 1]) && m_expression[operatorIndex - 1] != ')')
			{
				return false; 
			}
		}
		else if (!isValidOperand(c) &&!isalnum(c)) {
			return false; 
		}
		operatorIndex++;
	}

	return parenthesesStack.empty(); 
}

std::string ExpressionToAutomaton::GetExpression()
{
	return m_expression;
}

void ExpressionToAutomaton::SetPolishForm()
{
	PolishForm polishForm(m_expression);
	m_polishForm = polishForm.GetResult();
}

void ExpressionToAutomaton::LetterStep(char& index, char character)
{
	DeterministicFiniteAutomaton newAutomaton;
	std::string states;
	std::unordered_multimap<string, char> transitions;

	newAutomaton.SetInputAlphabet({ character });
	newAutomaton.SetStartState(index);
	states += index;

	transitions.insert({ { index, character }, index + 1 });
	newAutomaton.SetTransitions(transitions);

	index++;
	newAutomaton.SetFinalStates({ index });
	states += index;


	newAutomaton.SetStates(states);
	m_intermediateAutomatons.push(newAutomaton);

	
	index++;
}


void ExpressionToAutomaton::ConcatenationStep()
{
	
	DeterministicFiniteAutomaton secondAutomaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();
	DeterministicFiniteAutomaton firstAutomaton = m_intermediateAutomatons.top(); 
	m_intermediateAutomatons.pop();

	DeterministicFiniteAutomaton newAutomaton;
	char character = secondAutomaton.GetInputAlphabet()[0];
	char finalState = secondAutomaton.GetFinalStates()[0];
	std::string states = firstAutomaton.GetStates() + secondAutomaton.GetStates();
	states.erase(std::remove(states.begin(), states.end(), secondAutomaton.GetStartState()), states.end());
	std::unordered_multimap<string, char> transitions = firstAutomaton.GetTransitions();
	std::string inputAlphabet = firstAutomaton.GetInputAlphabet() + secondAutomaton.GetInputAlphabet();

	EliminateDuplicates(inputAlphabet);

	std::ranges::for_each(secondAutomaton.GetTransitions(), [&](const auto& transition)
	{
		if(transition.first[0] == secondAutomaton.GetStartState())
			transitions.insert({ { firstAutomaton.GetFinalStates()[0], transition.first[1] }, transition.second });
		else
		transitions.insert({ { transition.first[0], transition.first[1] }, transition.second });
	});

	newAutomaton.SetStates(states);
	newAutomaton.SetInputAlphabet(inputAlphabet);
	newAutomaton.SetStartState(firstAutomaton.GetStartState());
	newAutomaton.SetFinalStates({finalState});
	newAutomaton.SetTransitions(transitions);

	m_intermediateAutomatons.push(newAutomaton);
}

void ExpressionToAutomaton::OrStep(char& index)
{
	
	DeterministicFiniteAutomaton secondAutomaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();
	DeterministicFiniteAutomaton firstAutomaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();

	DeterministicFiniteAutomaton newAutomaton;
	std::unordered_multimap<string, char> transitions;

	

	transitions.insert({ { index, '$' }, firstAutomaton.GetStartState() });
	transitions.insert({ { index, '$' }, secondAutomaton.GetStartState() });

	std::ranges::for_each(firstAutomaton.GetTransitions(), [&](const auto& transition)
	{
				transitions.insert({ { transition.first[0], transition.first[1] }, transition.second });
	});
	std::ranges::for_each(secondAutomaton.GetTransitions(), [&](const auto& transition)
	{
						transitions.insert({ { transition.first[0], transition.first[1] }, transition.second });
	});

	transitions.insert({ { firstAutomaton.GetFinalStates()[0], '$' }, index + 1 });
	transitions.insert({ { secondAutomaton.GetFinalStates()[0], '$' }, index + 1 });

	std::string inputAlphabet = firstAutomaton.GetInputAlphabet() + secondAutomaton.GetInputAlphabet() + '$';
	EliminateDuplicates(inputAlphabet);

	newAutomaton.SetStates(firstAutomaton.GetStates() + secondAutomaton.GetStates() + index + char(index + 1));
	newAutomaton.SetInputAlphabet(inputAlphabet);
	newAutomaton.SetStartState(index);
	newAutomaton.SetFinalStates({ char(index + 1) });
	newAutomaton.SetTransitions(transitions);

	index += 2;
	m_intermediateAutomatons.push(newAutomaton);
}

void ExpressionToAutomaton::KleeneStarStep(char& index)
{
	
	DeterministicFiniteAutomaton automaton = m_intermediateAutomatons.top();
	m_intermediateAutomatons.pop();

	DeterministicFiniteAutomaton newAutomaton;
	std::string inputAlphabet = automaton.GetInputAlphabet() + '$';
	EliminateDuplicates(inputAlphabet);
	std::unordered_multimap<string, char> transitions = automaton.GetTransitions();
	transitions.insert({ { index, '$' }, automaton.GetStartState() });
	transitions.insert({ { automaton.GetFinalStates()[0], '$' }, index + 1 });
	transitions.insert({ { automaton.GetFinalStates()[0], '$' }, automaton.GetStartState() });
	transitions.insert({ { index, '$' }, index + 1 });

	newAutomaton.SetStates(automaton.GetStates() + index + char(index + 1));
	newAutomaton.SetInputAlphabet(inputAlphabet);
	newAutomaton.SetStartState(index);
	newAutomaton.SetFinalStates({ char(index + 1) });
	newAutomaton.SetTransitions(transitions);

	index += 2;
	m_intermediateAutomatons.push(newAutomaton);
}

void ExpressionToAutomaton::EliminateDuplicates(std::string& string)
{
	std::ranges::sort(string);
	string.erase(std::ranges::unique(string).begin(), string.end());
}
