#pragma once
#include <functional>
#include <set>

#include "Grammar.h"
#include "PushDownAutomaton.h"
#include "memory"
#include <unordered_map>
class Menu
{
public:
	void Run(std::ifstream& input);
private:
	void CreateAPD();
	bool ShowMenu() const;
	void PrintGrammar() const;
	void GeneratedWord() const;
	void PrintSimplifiedGrammar() const;
	void PrintFngGrammar() const;
	void GeneratedAndVerifyWord() const;
	void InputAndVerifyWord() const;
private:
	const std::unordered_map<int, std::function<void()>> MENU_OPTIONS{
		{1, [this]() { PrintGrammar(); }},
		{2, [this]() { GeneratedWord(); }},
		{3, [this]() { PrintSimplifiedGrammar(); }},
		{4, [this]() { PrintFngGrammar(); }},
		{5, [this]() { GeneratedAndVerifyWord(); }},
		{6, [this]() { InputAndVerifyWord(); }}
	};
private:
	std::unique_ptr<Grammar> m_grammar{};
	std::unique_ptr<PushDownAutomaton> m_pushDownAutomaton{};
	std::set<std::string> m_generatedWords{};
};

