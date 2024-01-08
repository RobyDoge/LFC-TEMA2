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
    
    EliminateUnitProductions();

    
    std::set<std::pair<char, std::string>> newTerminals;
    int k = 0;
    for (auto& output : m_productions | std::views::values)
    {
        if (output.size() >= 2)
        {
            for (const char symbol : output)
            {
                if (!IsNonTerminal(symbol))  
                {
                    char newNonTerminal = m_newNonTerminal;
                    newTerminals.insert(std::make_pair(newNonTerminal, std::string(1, symbol)));
                    m_nonTerminals += newNonTerminal;

					m_newNonTerminal++;
                    const auto temp = symbol;

                    
                    for (auto& value : m_productions | std::views::values)
                    {
                    
                        size_t pos = value.find(temp);
                        while (pos != std::string::npos)
                        {
                    
                            value.replace(pos, 1, 1, newNonTerminal);
                    
                            pos = value.find(temp);
                        }
                    }
                }
            }
        }
    }
    
    for (auto& [newNonTerminal, terminal] : newTerminals)
    {
        m_productions.emplace_back(newNonTerminal, terminal);
    }
    newTerminals.clear();
    
    for (auto& firstOutput : m_productions | std::views::values)
    {
    
        while (firstOutput.size() > 2)
        {
            for (size_t i = 1; i < firstOutput.size(); ++i)
            {
                if (IsNonTerminal(firstOutput[i]) && IsNonTerminal(firstOutput[i+1])) 
                {
                    char newNonTerminal = m_newNonTerminal;
                    m_nonTerminals += newNonTerminal;
					m_newNonTerminal++;
                    std::string temp(1, firstOutput[i]);
                    temp += firstOutput[i + 1];
                    newTerminals.insert(std::make_pair(newNonTerminal, temp));
                    i--;
    
                    for (auto& output : m_productions | std::views::values)
                    {
  
                        size_t pos = output.find(temp);
                        while (pos != std::string::npos)
                        {
  
                            output.replace(pos, 2, 1, newNonTerminal);
                            pos = output.find(temp);
                        }
                    }
                }
            }
        }
    }
    for (auto& [newNonTerminal, terminal] : newTerminals)
    {
        m_productions.emplace_back(newNonTerminal, terminal);
    }
}

void Grammar::EliminateUnitProductions()
{
	std::vector<std::pair<char, std::string>> newProductions;
    for (const auto& [nonTerminal, productions] : m_productions)
    {
        if (productions.size() == 1 && IsNonTerminal(productions[0]))
        {
            for (const auto& [nonTerminal1, productions1] : m_productions)
            {
                if (productions[0] == nonTerminal1)
                {
                    newProductions.emplace_back(nonTerminal, productions1);
                }
            }
			m_productions.append_range(newProductions);
			newProductions.clear();
        }
    }

    auto it = m_productions.begin();
    while (it != m_productions.end()) {
	    if (const auto& [nonTerminal, productions] = *it; 
            productions.size() == 1 && IsNonTerminal(productions[0])) {
            it = m_productions.erase(it); 
        }
        else {
            ++it;
        }
    }
}

void Grammar::RemoveLambdaProductions()
{
    std::unordered_set<char> lambdaNonTerminals;

    for (const auto& it : m_productions)
    {
		auto [nonTerminal, productions] = it;
        if (productions.size() == 1 && productions[0]=='$')
        {
			lambdaNonTerminals.insert(nonTerminal);
            std::erase(m_productions, it);
        }
    }

	if (lambdaNonTerminals.contains(m_startSymbol))
	{
		m_productions.emplace_back(m_newNonTerminal, "$");
        m_productions.emplace_back(m_newNonTerminal, std::string(1,m_startSymbol));
		m_startSymbol = m_newNonTerminal;
		m_nonTerminals += m_newNonTerminal;
        m_newNonTerminal++;
	}

    std::vector<std::pair<char, std::string>> newProductions;
    for (const auto& [nonTerminal, productions] : m_productions)
    {
        if (productions.size() > 1 && lambdaNonTerminals.contains(nonTerminal))
        {
            std::vector<int> uppercaseIndices;
            for (int i = 0; i < productions.size(); ++i)
            {
                if (IsNonTerminal(productions[i])) 
                {
                    uppercaseIndices.push_back(i);
                }
            }

            
            const int totalCombinations = 1 << uppercaseIndices.size();
            std::vector<std::string> combinations;

            
            for (int mask = 0; mask < totalCombinations-1; ++mask) {
                std::string combination;
                for (int i = 0; i < productions.size(); ++i) {
            
                    if (!IsNonTerminal(productions[i])) {
                        combination += productions[i];
                    }
                    else {
            
                        const int uppercaseIndex = std::distance(uppercaseIndices.begin(),
                                                                 std::ranges::find(uppercaseIndices, i));
                        if (mask & (1 << uppercaseIndex)) {
                            combination += productions[i];
                        }
                    }
                }
                if(!combination.empty())
                newProductions.push_back(std::make_pair(nonTerminal,combination));
            }

        }
    }
	m_productions.append_range(newProductions);
}


void Grammar::CreatePriority()
{
	
    int k = 0;
    for (const auto& nonTerminal : m_nonTerminals)
    {
        m_priority.emplace_back(k++, nonTerminal);
    }
}

int Grammar::GetPriority(const char symbol) {
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
    
	CreatePriority();
    bool noChangesMade = true;
    while (noChangesMade)
    {
        noChangesMade = false;
        for (auto& prod : m_productions)
        {
            if (GetPriority(prod.first) > GetPriority(prod.second[0]) && IsNonTerminal(prod.second[0]) )
            {
                FngLema1(prod, 0);
                noChangesMade = true;
                break;
            }
            else if (GetPriority(prod.first) == GetPriority(prod.second[0]) && IsNonTerminal(prod.second[0]) )
            {
                FngLema2(prod);
                noChangesMade = true;
                break;
            }
        }
    }
    
	int k = m_priority.size() - 2;
     noChangesMade = true;
    while (k>=0)
    {
        noChangesMade = false;
        for (auto& prod : m_productions)
        {
            if (GetPriority(prod.first) == k && GetPriority(prod.first) < GetPriority(prod.second[0]))
            {
                FngLema1(prod, 0);
                noChangesMade = true;
                break;
            }
     
        }
        if (!noChangesMade)
        k--;
    }
    
    noChangesMade = true;
    while (noChangesMade)
    {
        noChangesMade = false;
        for (auto& prod : m_productions)
        {
			if (std::isdigit(prod.first) && IsNonTerminal(prod.second[0]))
            {
                FngLema1(prod, 0);
                noChangesMade = true;
                break;
            }
        }
    }

}

void Grammar::FngLema1(const std::pair<char, std::string>& production, const size_t BPos)
{
	
    
    char first = production.first;
	const std::string right = production.second;
	const char B = right[BPos];
	std::vector<std::pair<char, std::string>> newProductions;
    for (const auto& [nonTerminal, productions] : m_productions)
    {
        if (nonTerminal == B)
        {
            newProductions.emplace_back(first, right.substr(0, BPos) + productions + right.substr(BPos + 1, right.size()));
        }
    }
    std::erase(m_productions, production);
	m_productions.append_range(newProductions);
}

void Grammar::FngLema2(const std::pair<char, std::string>& production)
{
    
      
    const char first = production.first;
    const std::string right = production.second;
    std::erase(m_productions, production);
    const std::pair<char, std::string> newProduction1 = std::make_pair(m_newNonTerminal, right.substr(1, right.size()));
    m_productions.push_back(newProduction1);
    const std::pair<char, std::string> newProduction2 = std::make_pair(m_newNonTerminal, right.substr(1, right.size()) + m_newNonTerminal);
    m_productions.push_back(newProduction2);
    for (auto& [nonTerminal, productions] : m_productions)
    {
        if (nonTerminal == first)
        {
            m_productions.emplace_back(nonTerminal, productions+ m_newNonTerminal);
        }
    }

    m_nonTerminals += m_newNonTerminal;
    m_newNonTerminal++;

}

bool Grammar::IsNonTerminal(const char symbol)
{
    return std::ranges::find(m_nonTerminals, symbol) != m_nonTerminals.end();
}

void Grammar::RemoveNonGeneratingSymbols()
{
    std::unordered_set<char> generatingSymbols;

    
    for (char symbol : m_terminals)
    {
        if (symbol != '$')
            generatingSymbols.insert(symbol);
    }

    bool changesMade = true;
    while (changesMade)
    {
        changesMade = false;

        for (const auto& [inputSymbol, outputSymbols] : m_productions)
        {

            if ((std::ranges::all_of(outputSymbols, [&](const char symbol)
                                     { return generatingSymbols.contains(symbol); }) ||
                outputSymbols.empty() || 
                (outputSymbols.size() == 1 && outputSymbols[0] == '$')) && 
	            !generatingSymbols.contains(inputSymbol))
            {
                generatingSymbols.insert(inputSymbol);
                changesMade = true;
            }
        }
    }

    
    std::erase_if(m_nonTerminals,
                  [&](const char symbol) { return !generatingSymbols.contains(symbol); });

    const auto it = std::ranges::remove_if(m_productions,
                                           [&](const auto& prod) { return !generatingSymbols.contains(prod.first); }).begin();
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
	    const char symbol = *newAccessibleSymbols.begin();
        newAccessibleSymbols.erase(newAccessibleSymbols.begin());

        for (const auto& [left, right] : m_productions)
        {
            if (left == symbol)
            {
                for (char VNonT : right)
                {
                    if (IsNonTerminal(VNonT) &&
	                    !accessibleSymbols.contains(VNonT))
                    {
                        newAccessibleSymbols.insert(VNonT);
                        accessibleSymbols.insert(VNonT);
                    }
                }
            }
        }
    }

    
    std::erase_if(m_nonTerminals,
                  [&](const char symbol) { return !accessibleSymbols.contains(symbol); });

    const auto it = std::ranges::remove_if(m_productions,
                                           [&](const auto& prod) { return !accessibleSymbols.contains(prod.first); }).begin();
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
        out << '\n';
    }
    return out;
}

std::string Grammar::GetLastWord()
{
    return m_lastWord;
}

void Grammar::SimplifyGrammar()
{
    RemoveLambdaProductions();
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
