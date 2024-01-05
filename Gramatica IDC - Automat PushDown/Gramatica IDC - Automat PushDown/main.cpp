#include <iostream>
#include "Grammar.h"
#include <fstream>
#include "PushDownAutomaton.h"
int main()
{
	std::ifstream input("input.txt");
	Grammar grammar(input);

	std::cout << grammar << '\n';
	grammar.FNC();
	std::cout<< grammar << '\n';
	std::cout<< "Is IDC: " << std::boolalpha << grammar.IsIDC() << '\n';


	/*PushDownAutomaton pda;
	std::cout << pda;*/
	return 9;
}