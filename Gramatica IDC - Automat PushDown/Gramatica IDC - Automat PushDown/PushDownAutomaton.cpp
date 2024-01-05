#include "PushDownAutomaton.h"
#include <algorithm>
#include <format>
#include <ranges>

PushDownAutomaton::PushDownAutomaton(const Grammar& grammar)
{
	m_states.emplace_back("q0");
	string firstState = m_states[0];
	for(const auto& inputAlphabetSymbol : grammar.GetTerminals())
	{
		m_inputAlphabet.push_back(inputAlphabetSymbol);
	}
	for(const auto& stackAlphabetSymbol : grammar.GetNonTerminals())
	{
		m_stackAlphabet.push_back(stackAlphabetSymbol);
	}

	for (const auto& [inputSymbol, outputSymbols] : grammar.GetProductions())
	{
		//if there is only one leemts we need to delete the top of the stack
		if(outputSymbols.size()==1)
		{
			m_transitionFunctions.emplace
			(
				std::format("{}{}{}", firstState, outputSymbols[0], inputSymbol),
				std::format("{}{}", firstState, '$')
			);
			continue;
		}

		//if there is more than one element we need to add them to the stack
		m_transitionFunctions.emplace
		(
			std::format("{}{}{}", firstState, outputSymbols[0], inputSymbol),
			std::format("{}{}", firstState, outputSymbols.substr(1))
		);
	}

	m_startingState = firstState;
	m_initialStackHead = grammar.GetStartSymbol();
}

bool PushDownAutomaton::IsDeterministic()
{
	return std::ranges::all_of(m_transitionFunctions, [&](const auto& transitionFunction)
	{
		//check if there is only one transition function for each input symbol
		return std::ranges::count_if(m_transitionFunctions, [&](const auto& otherTransitionFunction)
		{
			return transitionFunction.first == otherTransitionFunction.first;
		}) == 1;
	});
}

bool PushDownAutomaton::CheckWord(const std::string& checkWord)
{
	if(IsDeterministic())
	{
		return DeterministicAutomatonCheckWord(checkWord);
	}
	return NonDeterministicAutomatonCheckWord(checkWord);
}

bool PushDownAutomaton::DeterministicAutomatonCheckWord(const std::string& string)
{
	auto currentState = m_startingState;
	std::stack<char> stack;
	stack.push(m_initialStackHead);

	for(const auto& symbol : string)
	{
		const auto availableTransitionFunction=std::ranges::find_if(m_transitionFunctions,[&](const auto& transitionFunction)
		{
			return transitionFunction.first[1] == currentState[1] && transitionFunction.first[2] == symbol &&
				transitionFunction.first[3] == stack.top();
		});

		if(availableTransitionFunction == m_transitionFunctions.end())
		{
			return false;
		}
		currentState = availableTransitionFunction->second[0];
		if(availableTransitionFunction->second[1] == '$')
		{
			if(stack.empty())
			{
				return false;
			}
			stack.pop();
			continue;
		}
		if (availableTransitionFunction->second[1] != '#')
		{
			stack.pop();
			continue;
		}
		for(const auto& stackSymbol : availableTransitionFunction->second.substr(1))
		{
			stack.push(stackSymbol);
		}
	}

	return true;
}

bool PushDownAutomaton::NonDeterministicAutomatonCheckWord(const std::string& string)
{
	//todo
	return true;
}

bool PushDownAutomaton::CheckSymbolRecursive(char currentState, std::stack<char>& currentStack, char symbol)
{
	//used for nondeterminiscn automantons
	return true;
}

std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton)
{
	os << "Push Down Automaton: M = ({";
	for(const auto state : automaton.m_states)
	{
		os << state;
		if(state != automaton.m_states.back())
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
	os << "},{";
	for(const auto stackSymbol : automaton.m_stackAlphabet)
	{
		os << stackSymbol;
		if (stackSymbol != automaton.m_stackAlphabet.back())
		{
			os << ",";
		}
	}
	os << "},P,"<<automaton.m_startingState<<","<<automaton.m_initialStackHead<<",{";
	for(const auto finalState : automaton.m_finalStates)
	{
		os << finalState;
		if (finalState != automaton.m_finalStates.back())
		{
			os << ",";
		}
	}
	os<< "})." << std::endl;
	os<<"P containing the following transitions : \n";
		for (const auto& [inputSymbol, outputSymbols] : automaton.m_transitionFunctions)
		{
			os << std::format("P({},{},{},) -> ({},{})\n",
				inputSymbol[0],inputSymbol[1],inputSymbol[2],outputSymbols[0],outputSymbols[1]);
		}
	return os;
}
