#include "PushDownAutomaton.h"

#include <algorithm>
#include <format>
#include <ranges>
PushDownAutomaton::PushDownAutomaton(const Grammar& grammar)
{

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
	char currentState = m_startingState;
	std::stack<char> stack;
	stack.push(m_initialStackHead);

	for(const auto& symbol : string)
	{
		const auto availableTransitionFunction=std::ranges::find_if(m_transitionFunctions,[&](const auto& transitionFunction)
		{
			return transitionFunction.first[0] == currentState && transitionFunction.first[1] == symbol &&
				transitionFunction.first[2] == stack.top();
		});

		if(availableTransitionFunction == m_transitionFunctions.end())
		{
			return false;
		}
		currentState = availableTransitionFunction->second[0];
		if(availableTransitionFunction->second[1] == '$')
		{
			stack.pop();
			break;
		}

		stack.push(availableTransitionFunction->second[1]);
	}

	return true;
}

bool PushDownAutomaton::NonDeterministicAutomatonCheckWord(const std::string& string)
{
	//todo
}

bool PushDownAutomaton::CheckSymbolRecursive(char currentState, std::stack<char>& currentStack, char symbol)
{
	//used for nondeterminiscn automantons
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
