#include "Grammar.h"
#include <fstream>
#include <numeric>
#include <random>
#include <ranges>
#include <unordered_set>
#include <map>
#include <set>

Grammar::Grammar(std::ifstream& input)
{
    std::getline(input, m_nonTerminals);
    std::getline(input, m_terminals);
    input >> m_startSymbol;
    CreateProductions(input);
    input.close();
}

std::string Grammar::GetNonTerminals() const
{
    return m_nonTerminals;
}

std::string Grammar::GetTerminals() const
{
    return m_terminals;
}

std::vector<std::pair<char, std::string>> Grammar::GetProductions() const
{
    return m_productions;
}

char Grammar::GetStartSymbol() const
{
    return m_startSymbol;
}

void Grammar::FNC()
{
    // Pasul 1
    EliminateUnitProductions();

    // PASUL 2
    std::set<std::pair<char, std::string>> newTerminals;
    int k = 0;
    for (auto& [nonTerminal, production] : m_productions)
    {
        if (production.size() >= 2)
        {
            for (size_t i = 0; i < production.size(); ++i)
            {
                if (!isNonTerminal(production[i]))
                {
                    char newNonTerminal = 'C' + k;
                    k++;
                    newTerminals.insert(std::make_pair(newNonTerminal, std::string(1, production[i])));
                    m_nonTerminals += newNonTerminal;
                    auto temp = production[i];

                    // Parcurge toate perechile (cheie, valoare) în m_productions
                    for (auto& prod : m_productions)
                    {
                        auto& key = prod.first;
                        auto& value = prod.second;

                        // Verifică dacă producția conține caracterul ce trebuie înlocuit
                        size_t pos = value.find(temp);
                        while (pos != std::string::npos)
                        {
                            // Înlocuiește caracterul în producție
                            value.replace(pos, 1, 1, newNonTerminal);
                            // Găsește următoarea apariție a caracterului în producție
                            pos = value.find(temp);
                        }
                    }
                }
            }
        }
    }
    for (auto& [newNonTerminal, terminal] : newTerminals)
    {
        m_productions.push_back(std::make_pair(newNonTerminal, terminal));
    }
    newTerminals.clear();
    // Pasul 3
    for (auto& [nonTerminal, production] : m_productions)
    {
        if (production.size() > 2)
        {
            for (size_t i = 0; i < production.size(); ++i)
            {
                if (isNonTerminal(production[i]) && production[i] == production[i + 1])
                {
                    char newNonTerminal = 'C' + k;
                    k++;
                    std::string temp(1, production[i]);
                    temp += production[i + 1];
                    newTerminals.insert(std::make_pair(newNonTerminal, temp));
                    i--;
                    // Parcurge toate perechile (cheie, valoare) în m_productions
                    for (auto& prod : m_productions)
                    {
                        auto& key = prod.first;
                        auto& value = prod.second;

                        // Verifică dacă producția conține caracterul ce trebuie înlocuit
                        size_t pos = value.find(temp);
                        while (pos != std::string::npos)
                        {
                            // Înlocuiește caracterul în producție
                            value.replace(pos, 2, 1, newNonTerminal);
                            pos = value.find(temp);
                        }
                    }
                }
            }
        }
    }
    for (auto& [newNonTerminal, terminal] : newTerminals)
    {
        m_productions.push_back(std::make_pair(newNonTerminal, terminal));
        m_nonTerminals += newNonTerminal;
    }
}

void Grammar::EliminateUnitProductions()
{
	std::vector<std::pair<char, std::string>> newProductions;
    for (const auto& [nonTerminal, productions] : m_productions)
    {
        if (productions.size() == 1 && isNonTerminal(productions[0]))
        {
            for (const auto& [nonTerminal1, productions1] : m_productions)
            {
                if (productions[0] == nonTerminal1)
                {
                    newProductions.push_back(std::make_pair(nonTerminal, productions1));
                }
            }
			m_productions.append_range(newProductions);
			newProductions.clear();
        }
    }

    auto it = m_productions.begin();
    while (it != m_productions.end()) {
        const auto& [nonTerminal, productions] = *it;
        if (productions.size() == 1 && isNonTerminal(productions[0])) {
            it = m_productions.erase(it); // Șterge producția
        }
        else {
            ++it;
        }
    }
}

void Grammar::CreatePriority()
{
    int k = 0;
    for (const auto& nonTerminal : m_nonTerminals)
    {
        m_priority.push_back(std::make_pair(k++, nonTerminal));
    }
}

int Grammar::GetPriority(char symbol) {
	for (const auto& [priority, nonTerminal] : m_priority)
	{
		if (nonTerminal == symbol)
		{
			return priority;
		}
	}
	return -1;
}

void Grammar::FNG()
{
    //PAS 1
	CreatePriority();
    bool noChangesMade = true;
    while (noChangesMade)
    {
		noChangesMade = false;
        for (auto& prod : m_productions)
        {
            if (GetPriority(prod.first) > GetPriority(prod.second[0]) && isNonTerminal(prod.second[0])&& prod.first!='Z' && prod.second[0]!='Z')
            {
			    FngLema1(prod, 0);
                noChangesMade = true;
                break;
            }
		    else if (GetPriority(prod.first) == GetPriority(prod.second[0]) && isNonTerminal(prod.second[0]) && prod.first != 'Z' && prod.second[0] != 'Z')
		    {
			    FngLema2(prod);
                noChangesMade = true;
				break;
		    }
        }
    }
    m_productions.erase(std::unique(m_productions.begin(), m_productions.end()), m_productions.end());
}
void Grammar::RemoveDuplicates(std::vector<std::pair<char, std::string>>& input)
{
    input.erase(std::unique(input.begin(), input.end()), input.end());

}
void Grammar::FngLema1(std::pair<char, std::string>& production, size_t BPos)
{
    char first = production.first;
    std::string right = production.second;
    char B = right[BPos];
	std::vector<std::pair<char, std::string>> newProductions;
    for (const auto& [nonTerminal, productions] : m_productions)
    {
        if (nonTerminal == B)
        {
            newProductions.push_back(std::make_pair(first, right.substr(0, BPos) + productions + right.substr(BPos + 1, right.size())));
        }
    }
    m_productions.erase(std::remove(m_productions.begin(), m_productions.end(), production), m_productions.end());
	m_productions.append_range(newProductions);
}

void Grammar::FngLema2(std::pair<char, std::string>& production)
{
    char first = production.first;
    std::string right = production.second;
    std::pair<char, std::string> newProduction1 = std::make_pair('Z', right.substr(1, right.size()));
    m_productions.push_back(newProduction1);
    std::pair<char, std::string> newProduction2 = std::make_pair('Z', right.substr(1, right.size()) + 'Z');
    m_productions.push_back(newProduction2);

    m_productions.erase(std::remove(m_productions.begin(), m_productions.end(), production), m_productions.end());
    for (auto& [nonTerminal, productions] : m_productions)
    {
        if (nonTerminal == first)
        {
            m_productions.push_back(std::make_pair(nonTerminal, productions+'Z'));
        }
    }
	if (!isNonTerminal('Z')) m_nonTerminals += 'Z';
}

bool Grammar::isNonTerminal(char symbol)
{
    return std::find(m_nonTerminals.begin(), m_nonTerminals.end(), symbol) != m_nonTerminals.end();
}

void Grammar::RemoveNonGeneratingSymbols()
{
    std::unordered_set<char> generatingSymbols;

    // Inițializăm cu terminalele care pot genera cuvinte
    for (char symbol : m_terminals)
    {
        if (symbol != '$')
            generatingSymbols.insert(symbol);
    }

    bool changesMade = true;
    while (changesMade)
    {
        changesMade = false;

        for (const auto& prod : m_productions)
        {
            auto& inputSymbol = prod.first;
            auto& outputSymbols = prod.second;

            if ((std::all_of(outputSymbols.begin(), outputSymbols.end(), [&](char symbol)
                { return generatingSymbols.find(symbol) != generatingSymbols.end(); }) ||
                outputSymbols.empty() || // Verificăm dacă producția include simbolul vid
                (outputSymbols.size() == 1 && outputSymbols[0] == '$')) && // Verificăm simbolul "$"
                generatingSymbols.find(inputSymbol) == generatingSymbols.end())
            {
                generatingSymbols.insert(inputSymbol);
                changesMade = true;
            }
        }
    }

    // Eliminăm simbolurile care nu generează cuvinte din m_nonTerminals și m_productions
    m_nonTerminals.erase(std::remove_if(m_nonTerminals.begin(), m_nonTerminals.end(),
        [&](char symbol) { return generatingSymbols.find(symbol) == generatingSymbols.end(); }),
        m_nonTerminals.end());

    auto it = std::remove_if(m_productions.begin(), m_productions.end(),
        [&](const auto& prod) { return generatingSymbols.find(prod.first) == generatingSymbols.end(); });
    m_productions.erase(it, m_productions.end());
}

void Grammar::RemoveInaccessibleSymbols()
{
    std::unordered_set<char> accessibleSymbols;
    std::unordered_set<char> newAccessibleSymbols;

    newAccessibleSymbols.insert(m_startSymbol);

    accessibleSymbols.insert(newAccessibleSymbols.begin(), newAccessibleSymbols.end());

    while (!newAccessibleSymbols.empty())
    {
        char symbol = *newAccessibleSymbols.begin();
        newAccessibleSymbols.erase(newAccessibleSymbols.begin());

        for (const auto& [left, right] : m_productions)
        {
            if (left == symbol)
            {
                for (char VNonT : right)
                {
                    if (isNonTerminal(VNonT) &&
                        accessibleSymbols.find(VNonT) == accessibleSymbols.end())
                    {
                        newAccessibleSymbols.insert(VNonT);
                        accessibleSymbols.insert(VNonT);
                    }
                }
            }
        }
    }

    // Eliminăm simbolurile inaccesibile din m_nonTerminals și m_productions
    m_nonTerminals.erase(std::remove_if(m_nonTerminals.begin(), m_nonTerminals.end(),
        [&](char symbol) { return accessibleSymbols.find(symbol) == accessibleSymbols.end(); }),
        m_nonTerminals.end());

    auto it = std::remove_if(m_productions.begin(), m_productions.end(),
        [&](const auto& prod) { return accessibleSymbols.find(prod.first) == accessibleSymbols.end(); });
    m_productions.erase(it, m_productions.end());
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
        m_productions.emplace_back(line[0], line.substr(2));
    }
}

std::ostream& operator<<(std::ostream& out, const Grammar& grammar)
{
    out << "Grammar: G = ({";
    for (const auto& nonTerminal : grammar.m_nonTerminals)
    {
        out << nonTerminal;
        if (nonTerminal != grammar.m_nonTerminals.back())
        {
            out << ",";
        }
    }

    out << "},{";
    for (const auto& terminal : grammar.m_terminals)
    {
        out << terminal;
        if (terminal != grammar.m_terminals.back())
        {
            out << ",";
        }
    }
    out << "}," << grammar.m_startSymbol << ",P)"
        << ".\nP containing the following productions:\n";
    for (const auto& [inputSymbol, outputSymbols] : grammar.m_productions)
    {
        out << inputSymbol;
        out << " -> ";
        out << outputSymbols;
        out << std::endl;
    }
    return out;
}

std::string Grammar::GetLastWord()
{
    return m_lastWord;
}

void Grammar::SimplifyGrammar()
{
    EliminateUnitProductions();
    RemoveNonGeneratingSymbols();
    RemoveInaccessibleSymbols();
}

void Grammar::GenerateRandomWord(std::string& word, std::ostream& outputStream, const bool printSteps)
{
    if (word.size() == 1 && word[0] == m_startSymbol)
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
            for (auto const& prod : m_productions)
            {
                if (prod.first == input)
                {
                    applicable_productions.push_back(prod);
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
    if (outputSymbol == "$")
    {
        if (const int pos = producedWord.find(inputSymbol); pos != std::string::npos)
        {
            producedWord.erase(pos, 1);
        }
    }
    else
        if (const int pos = producedWord.find(inputSymbol); pos != std::string::npos)
        {
            producedWord.replace(pos, 1, outputSymbol);
        }
    return producedWord;
}
