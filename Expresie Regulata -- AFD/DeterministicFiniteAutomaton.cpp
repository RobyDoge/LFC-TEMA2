#include "DeterministicFiniteAutomaton.h"
#include <ranges>
#include <algorithm>
#include <format>
#include <fstream>
#include <ostream>

using string = std::string;

void DeterministicFiniteAutomaton::SetStates(const string& states)
{
	m_states = states;
}

void DeterministicFiniteAutomaton::SetInputAlphabet(const string& inputAlphabet)
{
	m_inputAlphabet = inputAlphabet;
}

void DeterministicFiniteAutomaton::SetStartState(const char& startState)
{
	m_startState = startState;
}

void DeterministicFiniteAutomaton::SetFinalStates(const string& finalStates)
{
	m_finalStates = finalStates;
}

void DeterministicFiniteAutomaton::SetTransitions(const std::unordered_multimap<string, char>& transitions)
{
	m_transitions = transitions;
}

string DeterministicFiniteAutomaton::GetStates() const
{
	return m_states;
}

string DeterministicFiniteAutomaton::GetInputAlphabet() const
{
	return m_inputAlphabet;
}

char DeterministicFiniteAutomaton::GetStartState() const
{
	return m_startState;
}

string DeterministicFiniteAutomaton::GetFinalStates() const
{
	return m_finalStates;
}

std::unordered_multimap<string, char> DeterministicFiniteAutomaton::GetTransitions() const
{
	return m_transitions;
}

bool DeterministicFiniteAutomaton::VerifyAutomaton()
{
	return IsDeterministic() &&
		StartingStateValidation() &&
		FinalStatesValidation() &&
		StateTransitionFunctionsValidation();
}

bool DeterministicFiniteAutomaton::CheckWord(const string& word)
{
	const string& wordToBeChecked = word;
	return CheckWordRecursive(wordToBeChecked, m_startState);
}

void DeterministicFiniteAutomaton::SaveToDisk(const string& fileName) const
{
	std::ofstream file(fileName);
	file << *this;
	file.close();
}

bool DeterministicFiniteAutomaton::IsDeterministic()
{
	return std::ranges::all_of(m_transitions, [&](const auto& transitionFunction)
	{
		return std::ranges::count_if(m_transitions, [&](const auto& otherTransitionFunction)
		{
			return transitionFunction.first == otherTransitionFunction.first;
		}) == 1;
	});
}

bool DeterministicFiniteAutomaton::StartingStateValidation()
{
	return std::ranges::find(m_states, m_startState) != m_states.end();
}

bool DeterministicFiniteAutomaton::FinalStatesValidation()
{
	return std::ranges::all_of(m_finalStates, [&](const auto& finalState)
	{
		return std::ranges::find(m_states, finalState) != m_states.end();
	});
}

bool DeterministicFiniteAutomaton::StateTransitionFunctionsValidation()
{
	return std::ranges::all_of(m_transitions, [&](const auto& transitionFunction)
	{
		return std::ranges::find(m_states, transitionFunction.first[0]) != m_states.end() &&
			std::ranges::find(m_inputAlphabet, transitionFunction.first[1]) != m_inputAlphabet.end() &&
			std::ranges::find(m_states, transitionFunction.second) != m_states.end();
	});
}

bool DeterministicFiniteAutomaton::CheckWordRecursive(const string& remainingWord, char currentState)
{

	if (remainingWord.empty() && std::ranges::find(m_finalStates, currentState) != m_finalStates.end())
	{
		return true;
	}
	if (remainingWord.empty() && std::ranges::find(m_finalStates, currentState) == m_finalStates.end())
	{
		return std::ranges::any_of(m_transitions, [&](const auto& transitionFunction)
			{
				return transitionFunction.first[0] == currentState
					&& transitionFunction.first[1] == '$'
					&& std::ranges::find(m_finalStates, transitionFunction.second) == m_finalStates.end();
			});
	}

	char symbol = remainingWord[0];
	const string keySymbol = std::format("{}{}", currentState, symbol);
	const string keyLambda = std::format("{}{}", currentState, '$');
	std::vector<std::pair<std::string, char>> availableTransitionFunctions;
	for (const auto& [inputSymbol, outputSymbols] : m_transitions)
	{
		if (inputSymbol == keySymbol || inputSymbol == keyLambda)
		{
			availableTransitionFunctions.emplace_back(inputSymbol, outputSymbols);
		}
	}

	const bool aux = std::ranges::any_of(availableTransitionFunctions, [&](const auto& transitionFunction)
	{
			int symbolPosition{ 0 };
		currentState = transitionFunction.second;
		if (transitionFunction.first[1] == '$')
		{
			symbolPosition--;
		}
		if (const string newWord = remainingWord.substr(symbolPosition + 1);
			CheckWordRecursive(newWord, currentState))
		{
			return true;
		}
		return false;
	});

	return aux;
	

}

std::ostream& operator<<(std::ostream& os, const DeterministicFiniteAutomaton& automaton)
{
	os << "Deterministic Finite Automaton: M = ({";
	for (const auto& state : automaton.m_states)
	{
		os << state;
		if (state != automaton.m_states.back())
		{
			os << ",";
		}
	}
	os << "},{";
	for (const auto symbol : automaton.m_inputAlphabet)
	{
		os << symbol;
		if (symbol != automaton.m_inputAlphabet.back())
		{
			os << ",";
		}
	}
	os << "},P," << automaton.m_startState << ",{";
	for (const auto& finalState : automaton.m_finalStates)
	{
		os << finalState;
		if (finalState != automaton.m_finalStates.back())
		{
			os << ",";
		}
	}
	os << "})." << '\n';
	os << "P containing the following transitions : \n";
	for (const auto& [inputSymbol, outputSymbols] : automaton.m_transitions)
	{
		os << std::format("P({},{}) -> ({})\n",
			inputSymbol[0], inputSymbol[1], outputSymbols);
	}
	return os;
}
