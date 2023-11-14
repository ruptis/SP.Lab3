#include <iostream>
#include <string>

#include "StringReplacer.h"

int main()
{
	const std::string test = "Hello, World!";
	std::cout << test << std::endl;

	const std::string searchString = "Hello";
	const std::string replaceString = "Adios";

	ReplaceString(searchString.c_str(), replaceString.c_str());

	std::cout << test << std::endl;
	return 0;
}
