#pragma once
#include <string>
#include <unordered_map>

using string = std::string;
class DeterministicFiniteAutomaton
{
	
public:
	DeterministicFiniteAutomaton() = default;
	~DeterministicFiniteAutomaton() = default;

	void TurnDeterministic();

	void SetStates(const string& states);
	void SetInputAlphabet(const string& inputAlphabet);
	void SetStartState(const char& startState);
	void SetFinalStates(const string& finalStates);
	void SetTransitions(const std::unordered_multimap<string, char>& transitions);

	string GetStates() const;
	string GetInputAlphabet() const;
	char GetStartState() const;
	string GetFinalStates() const;
	std::unordered_multimap<string, char> GetTransitions() const;

	bool VerifyAutomaton();
	friend std::ostream& operator<<(std::ostream& os, const DeterministicFiniteAutomaton& automaton);
	bool CheckWord(const string& word);
	void SaveToDisk(const string& fileName) const;
private:

	std::string EliminateLambdaTransitions(const char state);
	void RecursiveEliminateLambdaTransitions(const char state, std::string& result);
	
	std::string StartStep();
	std::string CreateNewState(const char symbol, std::string& equivalentStartState);

	bool IsDeterministic();
	bool StartingStateValidation();
	bool FinalStatesValidation();
	bool StateTransitionFunctionsValidation();
	bool CheckWordRecursive(const string& remainingWord, char currentState);
	void EliminateDuplicates(std::string& string);
private:
	string m_states{};
	string m_inputAlphabet{};
	char m_startState{};
	string m_finalStates{};
	std::unordered_multimap<string,char> m_transitions{};

};

