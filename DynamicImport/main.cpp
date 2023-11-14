#include <iostream>
#include <windows.h>

typedef void (*ReplaceStringFunc)(const char*, const char*);

int main()
{
	const HMODULE dll = LoadLibraryA("StringReplacer.dll");
	if (dll == nullptr)
		return 1;

	const auto replaceStringFunction = reinterpret_cast<ReplaceStringFunc>(GetProcAddress(dll, "ReplaceString"));

	if (replaceStringFunction == nullptr)
		return 1;

	const std::string test = "Hello, World!";
	std::cout << test << std::endl;

	const std::string searchString = "Hello";
	const std::string replaceString = "Adios";

	replaceStringFunction(searchString.c_str(), replaceString.c_str());

	std::cout << test << std::endl;
	return 0;
}