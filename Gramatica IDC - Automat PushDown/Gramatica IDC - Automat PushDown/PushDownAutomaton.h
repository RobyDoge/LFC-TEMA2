#pragma once
#include "Grammar.h"
#include <string>
#include <stack>

class PushDownAutomaton
{
public:
	PushDownAutomaton() =default;
	~PushDownAutomaton() = default;
	PushDownAutomaton(Grammar grammar);

	friend std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton);
	bool IsDeterministic();
	void CheckWord(std::string word);
private:
private:
	std::string m_states;
	std::string m_inputAlphabet;
	std::string m_stackAlphabet;
	char m_startingState;
	char m_initialStackHead;
	std::string m_finalStates;
	/*{currentState, inputSymbol, stackHead} -> {outputState, newStackHead}*/
	std::unordered_multimap<std::string,std::string> m_transitionFunctions;

};

