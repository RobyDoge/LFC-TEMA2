#pragma once
#include "Grammar.h"
#include <string>
#include <stack>
#include <unordered_map>

using string = std::string;
class PushDownAutomaton
{
public:
	PushDownAutomaton() =default;
	~PushDownAutomaton() = default;
	PushDownAutomaton(const Grammar& grammar);

	friend std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton);
	bool IsDeterministic();
	bool CheckWord(const string& checkWord);

private:
	bool CheckSymbolRecursive(string& currentState, std::stack<char>& stack, string& remainingWord);
	bool DeterministicAutomatonCheckWord(const string& word);
	bool NonDeterministicAutomatonCheckWord(const string& word);

private:
	std::vector<string> m_states{};
	string m_inputAlphabet{};
	string m_stackAlphabet{};
	string m_startingState{};
	char m_initialStackHead{};
	std::vector<string> m_finalStates{};
	std::unordered_multimap<string,string> m_transitionFunctions{};

};

