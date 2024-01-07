#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>

class Grammar
{
public:
	Grammar() = default;
	Grammar(std::ifstream& input);

	std::string GetNonTerminals() const;
	std::string GetTerminals() const;
	std::vector<std::pair<char, std::string>> GetProductions() const;
	char GetStartSymbol() const;

	bool IsIDC();
	void GenerateRandomWord(std::string& word, std::ostream& outputStream, bool printSteps);
	friend std::ostream& operator<<(std::ostream& out, const Grammar& grammar);
	std::string GetLastWord();
	void SimplifyGrammar();
	void FNC();
	void FNG();
private:
	void RemoveDuplicates(std::vector<std::pair<char, std::string>>& input);
	static std::string ApplyRandomProduction(const std::string& input, std::pair<char, std::string> production);
	void CreateProductions(std::ifstream& input);
	void RemoveNonGeneratingSymbols();
	void RemoveInaccessibleSymbols();
	void RemoveLambdaProductions();
	void EliminateUnitProductions();
	void FngLema1(const std::pair< char, std::string>& production, size_t BPos);
	void FngLema2(const std::pair<char, std::string>& production);
    bool IsNonTerminal(char symbol);
	void CreatePriority();
	int GetPriority(char symbol);

private:
	std::string m_nonTerminals;
	std::string m_terminals;
	char m_startSymbol;

	std::vector<std::pair<char,std::string>> m_productions;
	std::string m_lastWord{};
	std::vector<std::pair<int, char>> m_priority;
	char m_newNonTerminal = '1';

};
