#include <iostream>
#include "Grammar.h"
#include <fstream>

int main()
{
	std::ifstream input("input.txt");
	Grammar grammar(input);

	std::cout << grammar << std::endl;
	grammar.SimplifyGrammar();
	std::cout<< grammar << std::endl;
	std::cout<< "Is IDC: " << std::boolalpha << grammar.IsIDC() << std::endl;

}