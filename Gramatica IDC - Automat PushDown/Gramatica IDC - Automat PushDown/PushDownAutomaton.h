#pragma once
#include <string>
#include <vector>
class PushDownAutomaton
{
public:
	PushDownAutomaton();
	~PushDownAutomaton();

	void PrintAutomaton();
	void isDeterministic();
	void CheckWord(std::string word);
private:
	std::vector <std::string> m_states;
	std::vector <std::string> m_inputAlphabet;
	std::vector <std::string> m_stackAlphabet;
	std::vector <std::string> m_transitionFunctions;
	std::string m_startingState;
	std::vector<std::string> m_finalStates;
};

