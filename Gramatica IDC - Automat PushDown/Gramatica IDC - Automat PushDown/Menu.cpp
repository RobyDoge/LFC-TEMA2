#include "Menu.h"
#include <sstream>
#include <string>
#include <algorithm>
#include <iostream>

void Menu::Run(std::ifstream& input)
{
	m_grammar = std::make_unique<Grammar>(input);
	if (!m_grammar->IsIDC())
	{
		std::cout << "Grammar is not IDC\n";
		return;
	}
	CreateAPD();

	while (ShowMenu())
	{
	}
}

void Menu::CreateAPD()
{
	auto copyGrammar = *m_grammar;
	copyGrammar.SimplifyGrammar();
	copyGrammar.FNC();
	copyGrammar.FNG();
	m_pushDownAutomaton = std::make_unique<PushDownAutomaton>(copyGrammar);
}

bool Menu::ShowMenu() const
{
	const auto menu = std::format("\nMenu:\n"
		"1. Print Grammar.\n"
		"2. Generate a Word.\n"
		"3. Print Simplified Grammar.\n"
		"4. Print FNG Grammar.\n"
		"5. Generate a Word and Verify It Using The Automaton.\n"
		"6. Input a Word and Verify It Using The Automaton.\n"
		"0. Exit\n");
	std::cout << menu;
	int choice{};
	std::cin >> choice;
	if (!choice)
	{
		return false;
	}
	MENU_OPTIONS.at(choice)();
	return true;
}

void Menu::PrintGrammar() const
{
	std::cout << *m_grammar;
}

void Menu::GeneratedWord() const
{
	std::stringstream output;
	std::string word (1,m_grammar->GetStartSymbol());
	bool showSteps;
	char choice;
	int attempts{20};

	std::cout << "Should the steps be shown? (Y/N)\n";
	if (std::cin >> choice;
		choice == 'Y' || choice == 'y')
	{
		showSteps = true;
	}
	else
	{
		showSteps = false;
	}

	while (attempts)
	{
		m_grammar->GenerateRandomWord(word, output, showSteps);
		const auto startingPositionOfGeneratedWord = output.str().rfind(" => ");

		if (std::string generatedWord = output.str().substr(startingPositionOfGeneratedWord + 4);
			m_generatedWords.contains(generatedWord))
		{
			--attempts;
			continue;
		}
		std::cout << output.str();
		break;
	}
}

void Menu::PrintSimplifiedGrammar() const
{
	auto copyGrammar = *m_grammar;
	copyGrammar.SimplifyGrammar();
	std::cout << copyGrammar;
}

void Menu::PrintFngGrammar() const
{
	auto copyGrammar = *m_grammar;
	copyGrammar.SimplifyGrammar();
	copyGrammar.FNC();
	copyGrammar.FNG();
	std::cout << copyGrammar;
}

void Menu::GeneratedAndVerifyWord() const
{
	std::stringstream output;
	std::string word(1, m_grammar->GetStartSymbol());
	int attempts{20};
	std::string generatedWord{};
	while (attempts)
	{
		m_grammar->GenerateRandomWord(word, output, false);
		const auto startingPositionOfGeneratedWord = output.str().rfind(" => ");

		if (generatedWord = output.str().substr(startingPositionOfGeneratedWord + 4);
			m_generatedWords.contains(generatedWord))
		{
			--attempts;
			continue;
		}
		break;
	}
	std::string wordStatus = m_pushDownAutomaton->CheckWord(generatedWord) ? "Accepted" : "Rejected";
	std::cout << std::format("The Generated Word Is {} And Is {} By The Automaton.", generatedWord, wordStatus);
}

void Menu::InputAndVerifyWord() const
{
	std::string word;
	std::cout << "Input a word: ";
	std::cin >> word;
	std::string wordStatus = m_pushDownAutomaton->CheckWord(word) ? "Accepted" : "Rejected";
	std::cout << std::format("\nThe Word Is {} By The Automaton.", wordStatus);
}
