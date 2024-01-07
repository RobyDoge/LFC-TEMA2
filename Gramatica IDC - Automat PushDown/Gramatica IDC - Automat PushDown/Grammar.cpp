﻿#include "Grammar.h"
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
    // Pasul 1 //Elimina redenumirile de tipul S->L (neterminal->neterminal) , in loc de S->L o sa avem S-> (Toate productiile unde L e membru stang)
    EliminateUnitProductions();

    // PASUL 2
    std::set<std::pair<char, std::string>> newTerminals;
    int k = 0;
    for (auto& output : m_productions | std::views::values)
    {
        //Daca membrul drept al productiei este format din 2 sau mai multe caractere
        if (output.size() >= 2)
        {
            for (const char symbol : output)
            {
                if (!IsNonTerminal(symbol))  //Inlocuim fiecare terminal cu un nonTerminal creat de noi (deocamdata se duce de la C in colo, trb schimbat da nush exact cu ce)
                {
                    char newNonTerminal = 'C' + k;
                    k++;
                    newTerminals.insert(std::make_pair(newNonTerminal, std::string(1, symbol)));
                    m_nonTerminals += newNonTerminal;
                    const auto temp = symbol;

                    // Parcurge toate perechile (cheie, valoare) în m_productions
                    for (auto& value : m_productions | std::views::values)
                    {
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
    //Adauga la productii terminalele + nonterminalele pe care le-au inlocuit spre exemplu C->a D->b etc.
    for (auto& [newNonTerminal, terminal] : newTerminals)
    {
        m_productions.emplace_back(newNonTerminal, terminal);
    }
    newTerminals.clear();
    // Pasul 3
    for (auto& firstOutput : m_productions | std::views::values)
    {
        //Daca membrul drept al productiei este format din mai mult de 2 caractere 
        if (firstOutput.size() > 2)
        {
            for (size_t i = 0; i < firstOutput.size(); ++i)
            {
                if (IsNonTerminal(firstOutput[i]) && firstOutput[i] == firstOutput[i + 1]) // Si exista 2 neterminale la fel consecutive, le inlocuim cu un NOU neterminal!(la fel ca la pasul 2)
                {
                    char newNonTerminal = 'C' + k;
                    k++;
                    std::string temp(1, firstOutput[i]);
                    temp += firstOutput[i + 1];
                    newTerminals.insert(std::make_pair(newNonTerminal, temp));
                    i--;
                    // Parcurge toate perechile (cheie, valoare) în m_productions
                    for (auto& output : m_productions | std::views::values)
                    {
                        // Verifică dacă producția conține caracterul ce trebuie înlocuit
                        size_t pos = output.find(temp);
                        while (pos != std::string::npos)
                        {
                            // Înlocuiește caracterul în producție
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
        m_nonTerminals += newNonTerminal;
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
            it = m_productions.erase(it); // Șterge producția
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
        m_newNonTerminal++;
		m_nonTerminals += m_newNonTerminal;
	}



    std::vector<std::pair<char, std::string>> newProductions;
    for (const auto& [nonTerminal, productions] : m_productions)
    {
        if (productions.size() > 1 && lambdaNonTerminals.contains(nonTerminal))
        {
            std::vector<int> uppercaseIndices;
            for (int i = 0; i < productions.size(); ++i) {
                if (IsNonTerminal(productions[i])) {
                    uppercaseIndices.push_back(i);
                }
            }

            // Calculate the total number of combinations, which is 2^count of uppercase letters
            const int totalCombinations = 1 << uppercaseIndices.size();
            std::vector<std::string> combinations;

            // Iterate through all combinations
            for (int mask = 0; mask < totalCombinations-1; ++mask) {
                std::string combination;
                for (int i = 0; i < productions.size(); ++i) {
                    // If it's not an uppercase character, always include it
                    if (!IsNonTerminal(productions[i])) {
                        combination += productions[i];
                    }
                    else {
                        // If it's an uppercase character, include it only if its corresponding bit is set in the mask
                        const int uppercaseIndex = std::distance(uppercaseIndices.begin(),
                                                                 std::ranges::find(uppercaseIndices, i));
                        if (mask & (1 << uppercaseIndex)) {
                            combination += productions[i];
                        }
                    }
                }
                newProductions.emplace_back(nonTerminal,combination);
            }

        }
    }
	m_productions.append_range(newProductions);




}


void Grammar::CreatePriority()
{
	//Creeaza un vector de perechi <prioritate, neterminal> (avem nevoie de prioritati pt FNG)
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
    //PAS 1 ... Daca avem o situatie de tipul C->A si C are prioritate mai mare aplicam lema 1, pt situatii de tip C->C lema 2
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
    //PAS 2 Luam prioritatile in ordine descrecatoare si aplicam lema 1 pentru situatii de tipul A->C (invers fata de pasul 1)
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
    //PAS 3 Aici aplicam lema 1 unde este nevoie pentru Z Productii (in cazul nostru 0 ,1 ,2 etc)
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
	// Daca avem situatii de tipul A-> cv B cv , inlocuim pe B cu toate productiile lui , daca B are 3 productii o sa avem A->cv productia1 cv , A->cv productia2 cv , A->cv productia3 cv
    //pozitia lui B e decisa de FNG (0 usually)
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
    //Daca avem situatii de tipul C->C cv (recursivitate la stanga) , ca sa spargem recursivitatea la fiecare productie a lui C adaugam inca o productie cu un Z la urma
    //Si 2 productii cu           Z-> cv , Z->cv Z  
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

        for (const auto& [inputSymbol, outputSymbols] : m_productions)
        {

            if ((std::ranges::all_of(outputSymbols, [&](const char symbol)
                                     { return generatingSymbols.contains(symbol); }) ||
                outputSymbols.empty() || // Verificăm dacă producția include simbolul vid
                (outputSymbols.size() == 1 && outputSymbols[0] == '$')) && // Verificăm simbolul "$"
	            !generatingSymbols.contains(inputSymbol))
            {
                generatingSymbols.insert(inputSymbol);
                changesMade = true;
            }
        }
    }

    // Eliminăm simbolurile care nu generează cuvinte din m_nonTerminals și m_productions
    std::erase_if(m_nonTerminals,
                  [&](const char symbol) { return !generatingSymbols.contains(symbol); });

    const auto it = std::ranges::remove_if(m_productions,
                                           [&](const auto& prod) { return !generatingSymbols.contains(prod.first); }).begin();
    m_productions.erase(it, m_productions.end());
}

void Grammar::RemoveInaccessibleSymbols() //Daca nu poate fi accesat un neterminal ,e degeaba deci il stergem
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

    // Eliminăm simbolurile inaccesibile din m_nonTerminals și m_productions
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
