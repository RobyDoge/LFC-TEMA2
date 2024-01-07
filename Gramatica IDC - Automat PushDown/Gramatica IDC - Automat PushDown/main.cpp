#include <iostream>
#include <fstream>
#include "Menu.h"
int main()
{
	std::ifstream input("input.txt");
	Menu menu;
	menu.Run(input);
	return 0;
}