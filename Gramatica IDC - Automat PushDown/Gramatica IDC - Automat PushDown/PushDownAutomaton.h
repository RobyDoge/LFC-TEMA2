#pragma once
#include "Grammar.h"
#include <string>
#include <stack>

class PushDownAutomaton
{
public:
	~PushDownAutomaton() = default;
	PushDownAutomaton(const Grammar& grammar);

	friend std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton);
	bool IsDeterministic();
	bool CheckWord(const std::string& checkWord);

private:
	bool CheckSymbolRecursive(char currentState, std::stack<char>& currentStack, char symbol);
	bool DeterministicAutomatonCheckWord(const std::string& string);
	bool NonDeterministicAutomatonCheckWord(const std::string& string);

private:
	std::string m_states{};
	std::string m_inputAlphabet{};
	std::string m_stackAlphabet{};
	char m_startingState{};
	char m_initialStackHead{};
	//std::stack<char> m_stack{}; - we might not need this - TBD
	std::string m_finalStates{};
	/*{currentState, inputSymbol, stackHead} -> {outputState, newStackHead}*/
	std::unordered_multimap<std::string,std::string> m_transitionFunctions{};

};

