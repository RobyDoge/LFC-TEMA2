#include "Menu.h"

#include <algorithm>
#include <fstream>
#include <iostream>

void Menu::Run(const std::string& inputFileName)
{
	std::ifstream input(inputFileName);
	m_expressionToAutomaton = std::make_unique<ExpressionToAutomaton>(input);
	if(!m_expressionToAutomaton->IsExpressionValid())
	{
		std::cout << "The expression is not valid" << std::endl;
		return;
	}
	m_expressionToAutomaton->SetPolishForm();
	m_automaton = m_expressionToAutomaton->CreateAutomatonFromPolishForm();

	while (ShowMenu())
	{
	}
}

bool Menu::ShowMenu() const
{
	const auto menu = std::format("\nMenu:\n"
		"1. Print Automaton And Save It On Disk.\n"
		"2. Print Regular Expression.\n"
		"3. Input a Word and Verify It Using The Automaton.\n"
		"0. Exit\n");
	std::cout << menu;
	int choice{};
	std::cin >> choice;
	if (!choice)
	{
		return false;
	}
	while(!MENU_OPTIONS.contains(choice))
	{
		std::cout << "\nInvalid choice. Please Select Another Option.\n";
		std::cin >> choice;
	}
	MENU_OPTIONS.at(choice)();
	return true;

}

void Menu::PrintAutomatonAndSaveItOnDisk() const
{
	std::cout << m_automaton;
	m_automaton.SaveToDisk("automaton.txt");
}

void Menu::PrintRegularExpression() const
{
	auto expression = m_expressionToAutomaton->GetExpression();
	std::ranges::for_each(expression,[](char& character)
	{
		if (character == '.')
		{
			character = '\0';
		}
	});
	std::cout << std::format("The regular expression is: {}.", expression);
}

void Menu::InputAndVerifyWord()
{
	std::string word{};
	std::cout << "Input a word: ";
	std::cin >> word;

	if(word == "$")
		word.clear();

	std::string wordStatus = m_automaton.CheckWord(word) ? "Accepted" : "Rejected";
	std::cout << std::format("\nThe Word Is {} By The Automaton.", wordStatus);
}
