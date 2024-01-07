#include <iostream>
#include <fstream>
#include "Menu.h"
int main()
{
	std::ifstream input("input.txt");
	Grammar grammar(input);
	grammar.RemoveLambdaProductions();
	Menu menu;
	menu.Run(input);
	return 0;
}