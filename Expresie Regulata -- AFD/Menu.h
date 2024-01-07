#pragma once
#include <functional>
#include <memory>
#include "ExpressionToAutomaton.h"

class Menu
{
public:
	void Run(const std::string& inputFileName);


private:
	bool ShowMenu() const;
	void PrintAutomatonAndSaveItOnDisk() const;
	void PrintRegularExpression() const;
	void InputAndVerifyWord();

private:
	const std::unordered_map<int, std::function<void()>> MENU_OPTIONS{
		{1, [this]() { PrintAutomatonAndSaveItOnDisk(); }},
		{2, [this]() { PrintRegularExpression(); }},
		{3, [this]() { InputAndVerifyWord(); }}
	};
private:
	std::unique_ptr<ExpressionToAutomaton> m_expressionToAutomaton;
	DeterministicFiniteAutomaton m_automaton;
};

