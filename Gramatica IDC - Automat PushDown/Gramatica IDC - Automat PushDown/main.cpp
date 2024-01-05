#include <iostream>
#include "Grammar.h"
#include <fstream>
#include "PushDownAutomaton.h"
int main()
{
	std::ifstream input("input.txt");
	Grammar grammar(input);

	std::cout << grammar << std::endl;
	grammar.FNC();
	std::cout<< grammar << std::endl;
	std::cout<< "Is IDC: " << std::boolalpha << grammar.IsIDC() << std::endl;

	/*PushDownAutomaton pda;
	std::cout << pda;*/

}