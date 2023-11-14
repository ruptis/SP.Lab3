#include <iostream>

int main()
{
	const std::string test = "Hello, World!";
	std::cout << test << std::endl;
	std::cin.get();
	std::cout << test << std::endl;
	std::cin.get();
	return 0;
}