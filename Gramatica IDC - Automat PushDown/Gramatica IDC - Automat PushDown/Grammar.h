#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

class Grammar
{
public:
	Grammar() = default;
	Grammar(std::ifstream& input);

	std::string GetNonTerminals() const;
	std::string GetTerminals() const;
	std::unordered_multimap<char, std::string> GetProductions() const;
	char GetStartSymbol() const;

	bool IsIDC();
	void GenerateRandomWord(std::string& word, std::ostream& outputStream, bool printSteps);
	friend std::ostream& operator<<(std::ostream& out, const Grammar& grammar);
	std::string GetLastWord();
	void SimplifyGrammar();
	void FNC();
private:
	static std::string ApplyRandomProduction(const std::string& input, std::pair<char, std::string> production);
	void CreateProductions(std::ifstream& input);
	void RemoveNonGeneratingSymbols();
	void RemoveInaccessibleSymbols();
	void EliminateUnitProductions();
private:
	std::string m_nonTerminals;
	std::string m_terminals;
	char m_startSymbol;
	std::unordered_multimap<char,std::string> m_productions;
	std::string m_lastWord{};
};
