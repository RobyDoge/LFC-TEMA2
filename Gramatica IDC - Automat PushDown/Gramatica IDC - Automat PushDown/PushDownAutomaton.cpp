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
		if(outputSymbols.size()==1)
		{
			m_transitionFunctions.emplace
			(
				std::format("{}{}{}", firstState, outputSymbols[0], inputSymbol),
				std::format("{}{}", firstState, '$')
			);
			continue;
		}
		std::string newStackSymbols = outputSymbols.substr(1);
		std::ranges::reverse(newStackSymbols);

		m_transitionFunctions.emplace
		(
			std::format("{}{}{}", firstState, outputSymbols[0], inputSymbol),
			std::format("{}{}", firstState, newStackSymbols)
		);
	}

	m_startingState = firstState;
	m_initialStackHead = grammar.GetStartSymbol();
}

bool PushDownAutomaton::IsDeterministic()
{
	return std::ranges::all_of(m_transitionFunctions, [&](const auto& transitionFunction)
	{
	
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

bool PushDownAutomaton::DeterministicAutomatonCheckWord(const std::string& word)
{
	auto currentState = m_startingState;
	std::stack<char> stack;
	stack.push(m_initialStackHead);

	for(const auto& symbol : word)
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
		currentState[1] = availableTransitionFunction->second[1];

		if (stack.empty())
		{
			return false;
		}
		stack.pop();
		for(const auto& stackSymbol : availableTransitionFunction->second.substr(2))
		{
			if (stackSymbol == '$')
			{
				continue;
			}
			stack.push(stackSymbol);
		}
	}

	if(stack.empty() || std::ranges::find(m_finalStates,currentState)!=m_finalStates.end())
	{
		return true;
	}

}

bool PushDownAutomaton::NonDeterministicAutomatonCheckWord(const std::string& word)
{
	std::stack<char> currentStack;
	currentStack.emplace(m_initialStackHead);
	string wordToBeChecked = word;
	return CheckSymbolRecursive(m_startingState, currentStack, wordToBeChecked);
}

bool PushDownAutomaton::CheckSymbolRecursive(string& currentState, std::stack<char>& stack, string& remainingWord)
{
	if(remainingWord.empty() && !stack.empty())
	{
		return false;
	}
	if(!remainingWord.empty() && stack.empty())
	{
		return false;
	}
	if(remainingWord.empty() && stack.empty())
	{
		return true;
	}
	if(remainingWord.empty() && std::ranges::find(m_finalStates, currentState) != m_finalStates.end())
	{
		return true;
	}


	for(int symbolPosition = 0; symbolPosition < remainingWord.size();symbolPosition++)
	{
		char symbol = remainingWord[symbolPosition];
		const string key = std::format("{}{}{}{}", 'q', currentState[1], symbol, stack.top());
		std::vector<std::pair<std::string, std::string>>availableTransitionFunctions;
		for(const auto& [inputSymbol, outputSymbols] : m_transitionFunctions)
		{
			if (inputSymbol == key)
			{
				availableTransitionFunctions.emplace_back(inputSymbol, outputSymbols);
			}
		}

		const bool aux = std::ranges::any_of(availableTransitionFunctions, [&](const auto& transitionFunction)
		{
			auto currentStack = stack;
			currentState[0] = transitionFunction.second[0];
			currentState[1] = transitionFunction.second[1];
			currentStack.pop();
			for (const auto& stackSymbol : transitionFunction.second.substr(2))
			{
				if (stackSymbol == '$')
				{
					continue;
				}
				currentStack.push(stackSymbol);
			}
			if(string newWord = remainingWord.substr(symbolPosition+1);
				CheckSymbolRecursive(currentState, currentStack, newWord))
			{
				return true;
			}
			return false;
		});

		return aux;
	}
}

std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton)
{
	os << "Push Down Automaton: M = ({";
	for(const auto& state : automaton.m_states)
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
	for(const auto& finalState : automaton.m_finalStates)
	{
		os << finalState;
		if (finalState != automaton.m_finalStates.back())
		{
			os << ",";
		}
	}
	os<< "})." << '\n';
	os<<"P containing the following transitions : \n";
		for (const auto& [inputSymbol, outputSymbols] : automaton.m_transitionFunctions)
		{
			os << std::format("P({}{},{},{}) -> ({}{},{})\n",
				inputSymbol[0],inputSymbol[1],inputSymbol[2], inputSymbol[3], outputSymbols[0], outputSymbols[1],outputSymbols[2]);
		}
	return os;
}
