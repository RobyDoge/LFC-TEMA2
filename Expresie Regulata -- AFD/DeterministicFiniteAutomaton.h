#pragma once
#include <string>
#include <unordered_map>

using string = std::string;
class DeterministicFiniteAutomaton
{
	
public:
	DeterministicFiniteAutomaton() = default;
	~DeterministicFiniteAutomaton() = default;

	void SetStates(const string& states);
	void SetInputAlphabet(const string& inputAlphabet);
	void SetStartState(const char& startState);
	void SetFinalStates(const std::vector<char>& finalStates);
	void SetTransitions(const std::unordered_multimap<string, char>& transitions);

	string GetStates() const;
	string GetInputAlphabet() const;
	char GetStartState() const;
	std::vector<char> GetFinalStates() const;
	std::unordered_multimap<string, char> GetTransitions() const;

	bool VerifyAutomaton();
	friend std::ostream& operator<<(std::ostream& os, const DeterministicFiniteAutomaton& automaton);
	bool CheckWord(const string& word);

private:
	bool IsDeterministic();
	bool StartingStateValidation();
	bool FinalStatesValidation();
	bool StateTransitionFunctionsValidation();

private:
	string m_states{};
	string m_inputAlphabet{};
	char m_startState{};
	std::vector<char> m_finalStates{};
	std::unordered_multimap<string,char> m_transitions{};

};

