// DynamicImport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define DLL_NAME "C:\\Users\\Èðèíà\\Desktop\\ÎÑÈÑÏ\\Lab3\\Debug\\StringProcessing.dll"
#define PROC_NAME "ReplaceStringInVirtualMemory"

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

	HMODULE module = LoadLibrary(TEXT(DLL_NAME));

	typedef BOOL TReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid);
	TReplaceStringInVirtualMemory* pReplaceStringInVirtualMemory = 
		(TReplaceStringInVirtualMemory*)GetProcAddress(module, PROC_NAME);

	if (pReplaceStringInVirtualMemory(oldStr.c_str(), newStr.c_str(), pid))
	{
		std::cout << "String has been replaced successfully.\n";
	}
	else
	{
		std::cout << "Error: Cannot replace the string.\n";
	}

	FreeLibrary(module);
	getchar();
	getchar();

    return 0;
}

