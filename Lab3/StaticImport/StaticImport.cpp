// StaticImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

__declspec(dllimport) BOOL ReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid);

int main()
{
	int pid = -1;
	printf("Enter the process id:\n");
	scanf_s("%d", &pid);

	std::string oldStr;
	std::string newStr;
	std::cout << "Enter string to replace:\n";
	std::cin >> oldStr;
	std::cout << "Enter string by which replace:\n";
	std::cin >> newStr;

	if (oldStr.length() != newStr.length())
	{
		std::cout << "Error: Length of the strings should be equal.\n";
		getchar();
		return 1;
	}

	ReplaceStringInVirtualMemory(oldStr.c_str, newStr.c_str, pid);
	std::cout << "String was replaced successfully.\n";

	getchar();

	return 0;
}

