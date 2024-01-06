#include "DeterministicFiniteAutomaton.h"
#include <ranges>
#include <algorithm>
#include <format>
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

void DeterministicFiniteAutomaton::SetFinalStates(const std::vector<char>& finalStates)
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

std::vector<char> DeterministicFiniteAutomaton::GetFinalStates() const
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
	auto currentState = m_startState;
	for (const auto& symbol : word)
	{
		auto transition = std::ranges::find_if(m_transitions, [&](const auto& transitionFunction)
		{
			return transitionFunction.first[0] == currentState && transitionFunction.first[1] == symbol;
		});
		if (transition == m_transitions.end())
		{
			return false;
		}
		currentState = transition->second;
	}
	return std::ranges::find(m_finalStates, currentState) != m_finalStates.end();
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
