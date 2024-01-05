#pragma once
#include "Grammar.h"
#include <string>
#include <stack>
#include <unordered_map>

class PushDownAutomaton
{
	using string = std::string;
public:
	~PushDownAutomaton() = default;
	PushDownAutomaton(const Grammar& grammar);

	friend std::ostream& operator<<(std::ostream& os, const PushDownAutomaton& automaton);
	bool IsDeterministic();
	bool CheckWord(const string& checkWord);

private:
	bool CheckSymbolRecursive(char currentState, std::stack<char>& currentStack, char symbol);
	bool DeterministicAutomatonCheckWord(const string& string);
	bool NonDeterministicAutomatonCheckWord(const string& string);

private:
	std::vector<string> m_states{};
	string m_inputAlphabet{};
	string m_stackAlphabet{};
	string m_startingState{};
	char m_initialStackHead{};
	std::vector<string> m_finalStates{};
	/*{currentState(q+no), inputSymbol, stackHead} -> {outputState, newStackElements}*/
	std::unordered_multimap<string,string> m_transitionFunctions{};

	/*custom symbols for stack
	 * $ - removes the top of the stack
	 * #  - does not change the stack
	 */
};

