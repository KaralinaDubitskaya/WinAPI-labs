// StaticImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

__declspec(dllimport) BOOL WINAPI ReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid);

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

	int oldStrLen = oldStr.length();
	int newStrLen = newStr.length();

	if (oldStrLen < newStrLen)
	{
		std::cout << "Error: Length of the new string shouldn't be greater than length of the previous one.\n";
		getchar();
		return 1;
	}
	else
	{
		if (oldStrLen > newStrLen)
		{
			std::string spaces;
			for (int i = 0; i < oldStrLen - newStrLen; i++)
			{
				spaces.append(" ");
			}
			newStr.append(spaces);
		}
	}

	if (ReplaceStringInVirtualMemory(oldStr.c_str(), newStr.c_str(), pid))
	{
		std::cout << "String has been replaced successfully.\n";
	}
	else
	{
		std::cout << "Error: Cannot replace the string.\n";
	}

	getchar();
	getchar();

	return 0;
}

