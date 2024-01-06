#include <iostream>
#include "Grammar.h"
#include <fstream>
#include "PushDownAutomaton.h"
int main()
{
	std::ifstream input("test.txt");
	Grammar grammar(input);
	PushDownAutomaton pda(grammar);
	std::cout << pda;
	std::string word{ "abab" };
	std::cout << "Word: " << word << " is " << (pda.CheckWord(word) ? "accepted" : "rejected") << '\n';
	return 9;
}