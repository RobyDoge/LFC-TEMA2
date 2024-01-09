#include "DeterministicFiniteAutomaton.h"
#include <ranges>
#include <algorithm>
#include <format>
#include <fstream>
#include <ostream>
#include <stack>

using string = std::string;


//this method takes a non-deterministic finite automaton with lambda transitions and turns it into a deterministic finite automaton
//we do this by :
//1. eliminating lambda transitions via lambda closure
//2. creating a new state for each set of states that can be reached from the initial state
//3. creating new transitions for each transition that can be reached from the initial state
//this method should have minimal states and transitions, the input alphabet should remain the same, without lambda
void DeterministicFiniteAutomaton::TurnDeterministic()
{
    std::unordered_multimap<string, char> transitions;
    std::string states;
    std::string currentState;
    char index = 'A';
    std::string auxState;
    std::list<string> statesToBeProcessed;
    std::unordered_map<string, char> stateEquivalents;
    m_inputAlphabet.erase(std::ranges::find(m_inputAlphabet, '$'));

    currentState = StartStep();

    states += 'A';
    stateEquivalents.insert({currentState, index});
    index++;
    statesToBeProcessed.push_back(currentState);

    while (statesToBeProcessed.size() != 0)
    {
        currentState = statesToBeProcessed.front();
         statesToBeProcessed.pop_front();

        std::ranges::for_each(m_inputAlphabet, [&](const auto& symbol)
            {
                auto newState = CreateNewState(symbol, currentState);
                if (newState != "")
                {
					// check if the new state is already in the set of states
					bool ok = true;
					if (stateEquivalents.find({ newState }) == stateEquivalents.end())
					{
					 ok = false;
					}

                    //bool ok = std::any_of(currentState.begin(), currentState.end(), [&](char state) { return stateEquivalents.find({state}) != stateEquivalents.end(); });
                     if (!ok && newState!=currentState)
                    {
						
						stateEquivalents.insert({ newState, index });
						auxState = stateEquivalents[currentState];
                    	auxState+= symbol;
                        transitions.insert({auxState, index});
                        statesToBeProcessed.push_back(newState);
                        states += index;
                        index++;
                    }
                    else if(newState != currentState)
                    {
						auxState = stateEquivalents[currentState];
						auxState += symbol;
                        transitions.insert({auxState, stateEquivalents[newState]});
                    }
					else
					{
						 auxState = stateEquivalents[currentState];
						 auxState += symbol;
						transitions.insert({auxState, stateEquivalents[currentState] });
					}
                }
            });
    }

    m_startState = 'A';
    m_states = states;
    m_transitions = transitions;
    m_finalStates = stateEquivalents[m_finalStates];
}


//from the given state, we will find all the states that can be reached via lambda transitions
std::string DeterministicFiniteAutomaton::EliminateLambdaTransitions(const char state)
{
	std::string result{};
	std::ranges::for_each(m_transitions, [&](const auto& transition)
		{
			if (transition.first[0] == state && transition.first[1] == '$')
			{
				result += transition.second;
			}
		});
	return result;

}

void DeterministicFiniteAutomaton::RecursiveEliminateLambdaTransitions(const char state, std::string& result)
{
	//bool ok = std::any_of(m_transitions.begin(), m_transitions.end(), [&](const auto& transition)
	//{
	//	return transition.first[0] == state && transition.first[1] == '$';
	//});
	//if (ok)
	//	return;
	for (const auto& transition : m_transitions)
	{
		if (transition.first[0] == state && transition.first[1] == '$')
		{
			result += transition.second;
			RecursiveEliminateLambdaTransitions(transition.second, result);
		}
	}
}

std::string DeterministicFiniteAutomaton::StartStep()
{
	std::string currentState;

	//creating the first state to be able to start the algorithm
	//to do this i will use the start state of the non-deterministic finite automaton
	//i will find all the transitions that include the start state as the initial state
	std::ranges::for_each(m_transitions, [&](const auto& transition)
		{
			if (transition.first[0] == m_startState)
			{
				currentState += transition.second;
				auto lambdaClosure = EliminateLambdaTransitions(transition.second);
				currentState += lambdaClosure;
				EliminateDuplicates(currentState);
			}
		});
	if(currentState.size() == 0)
		currentState = m_startState;

	currentState += EliminateLambdaTransitions(m_startState);
	currentState += m_startState;
	EliminateDuplicates(currentState);

	return currentState;
}

std::string DeterministicFiniteAutomaton::CreateNewState(const char symbol, std::string& equivalentStartState)
{
	std::string newState{};
	std::ranges::for_each(m_transitions, [&](const auto& transition)
	{
		std::ranges::for_each(equivalentStartState, [&](const auto& state)
		{
			if (transition.first[0] == state && transition.first[1] == symbol)
			{
				
				newState += transition.second;
				//auto lambdaClosure = EliminateLambdaTransitions(transition.second);
				std::string lambdaClosure{};
				RecursiveEliminateLambdaTransitions(transition.second, lambdaClosure);
				newState += lambdaClosure;
				newState += transition.second;
				EliminateDuplicates(newState);
			}
		});
	});
	return newState;
}

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

void DeterministicFiniteAutomaton::EliminateDuplicates(std::string& string)
{
	std::ranges::sort(string);
	string.erase(std::unique(string.begin(), string.end()), string.end());
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
