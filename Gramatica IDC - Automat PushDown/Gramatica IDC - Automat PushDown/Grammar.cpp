#include "Grammar.h"
#include <fstream>
#include <numeric>
#include <random>
#include <ranges>

Grammar::Grammar(std::ifstream& input)
{
	std::getline(input, m_nonTerminals);
	std::getline(input, m_terminals);
	input >> m_startSymbol;
	CreateProductions(input);
	input.close();
}	

bool Grammar::IsIDC()
{
	return std::ranges::all_of(m_productions, [&](const auto& rule)
		{
		const char input = rule.first;
		const std::string output = rule.second;
			return std::ranges::find(m_nonTerminals, input) != m_nonTerminals.end() &&
				std::ranges::all_of(output, [&](const auto& symbol)
					{
						return std::ranges::find(m_terminals, symbol) != m_terminals.end() ||
							std::ranges::find(m_nonTerminals, symbol) != m_nonTerminals.end();
					});
		});
}

void Grammar::CreateProductions(std::ifstream& input)
{
	std::string line;
	std::getline(input, line);
	while (!input.eof())
	{
		std::getline(input, line);
		m_productions.emplace(line[0],line.substr(2));
	}
}

std::ostream& operator<<(std::ostream& out, const Grammar& grammar)
{
	out << "Grammar: G = ({";
	for(const auto& nonTerminal : grammar.m_nonTerminals)
	{
		out << nonTerminal;
		if (nonTerminal != grammar.m_nonTerminals.back())
		{
			out << " , ";
		}
	}
	
	out << "},{";
	for (const auto& terminal : grammar.m_terminals)
	{
		out << terminal;
		if (terminal != grammar.m_terminals.back())
		{
			out << " , ";
		}
	}
	out << "}," << grammar.m_startSymbol << ", P)"
		<< ".\nP containing the following productions:\n";
	for (const auto& [inputSymbol, outputSymbols] : grammar.m_productions)
	{
		out << inputSymbol;
		out << " -> ";
		out<< outputSymbols;
		out << std::endl;
	}
	return out;
}

std::string Grammar::GetLastWord()
{
	return m_lastWord;
}

void Grammar::GenerateRandomWord(std::string& word, std::ostream& outputStream, const bool printSteps)
{
	if (word.size()==1 && word[0] == m_startSymbol)
	{
		outputStream << "\n" << m_startSymbol;
	}

	const std::string producedWord = word;
	m_lastWord = producedWord;
	std::vector<std::pair<char, std::string>> applicable_productions;

	for (const auto& input : m_productions | std::views::keys)
	{
		if (producedWord.find(input) != std::string::npos)
		{
			for(auto const& [key, value] : m_productions)
			{
				if (key == input)
				{
					applicable_productions.emplace_back(key, value);
				}
			}
		}
	}

	if (applicable_productions.empty())
	{
		if (!printSteps)
			outputStream << " => " << word;
		return;
	}
	if (printSteps)
	{
		outputStream << " => ";
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, applicable_productions.size() - 1);
	const std::pair<char, std::string> chosen_production = applicable_productions[distr(gen)];

	std::string new_word = ApplyRandomProduction(producedWord, chosen_production);

	if (new_word == producedWord)
	{
		return;
	}
	if (printSteps)
		outputStream << new_word;
	GenerateRandomWord(new_word, outputStream, printSteps);
	word = new_word;
}

std::string Grammar::ApplyRandomProduction(const std::string& input, std::pair<char, std::string> production)
{
	std::string producedWord = input;
	auto [inputSymbol, outputSymbol] = production;

	if (const int pos = producedWord.find(inputSymbol); pos != std::string::npos)
	{
		producedWord.replace(pos, 1, outputSymbol);
	}
	return producedWord;
}