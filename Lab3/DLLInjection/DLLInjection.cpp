// DLLInjection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define DLL_NAME "C:\\Users\\Èðèíà\\Desktop\\ÎÑÈÑÏ\\Lab3\\Debug\\StringProcessing.dll"
#define PROC_NAME "ReplaceStringInVirtualMemoryInjection"

BOOL InjectSearchProcessingDll(int pid, std::string oldStr, std::string newStr);

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

	InjectSearchProcessingDll(pid, oldStr, newStr);
	
	getchar();
	getchar();

	return 0;
}

BOOL InjectSearchProcessingDll(int pid, std::string oldStr, std::string newStr)
{
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (process == NULL)
	{
		std::cout << "Error: Cannot open the process.\n";
		return false;
	}

	LPVOID funcPointer = (LPVOID)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	if (funcPointer == NULL)
	{
		std::cout << "Error: Cannot load the procedure address from kernel32.dll.\n";
		return false;
	}

	LPVOID alloc = VirtualAllocEx(process, 0, strlen(DLL_NAME), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (alloc == NULL)
	{
		std::cout << "Error: Cannot allocate virtual memory.\n";
		return false;
	}

	if (!WriteProcessMemory(process, alloc, DLL_NAME, strlen(DLL_NAME), NULL))
	{
		std::cout << "Error: Cannot write to the virtual memory of the process.\n";
		return false;
	}

	HANDLE remoteThread = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)funcPointer, (LPVOID)alloc, 0, 0);
	if (remoteThread == NULL)
	{
		std::cout << "Error: Cannot create remote thread in the process.\n";
		return false;
	}

	std::cout << "Dll was successfully injected.\n";
	WaitForSingleObject(remoteThread, INFINITE);
	CloseHandle(remoteThread);

	HMODULE hModule = LoadLibrary(TEXT(DLL_NAME));
	funcPointer = (LPVOID)GetProcAddress(hModule, PROC_NAME);
	if (funcPointer == NULL)
	{
		std::cout << "Error: Cannot load the procedure address from StringProcessing.dll.\n";
		return false;
	}

	struct PARAMETER {
		std::string oldStr;
		std::string newStr;
		int pid;
	};

	PARAMETER m_parameter;

	m_parameter.oldStr = oldStr;
	m_parameter.newStr = newStr;
	m_parameter.pid = pid;

	alloc = VirtualAllocEx(process, NULL, sizeof PARAMETER, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (alloc == NULL)
	{
		std::cout << "Error: Cannot allocate virtual memory.\n";
		return false;
	}

	if (!WriteProcessMemory(process, alloc, &m_parameter, sizeof(m_parameter), NULL))
	{
		std::cout << "Error: Cannot write to the virtual memory of the process.\n";
		return false;
	}

	remoteThread = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)funcPointer, (LPVOID)alloc, 0, 0);
	if (remoteThread == NULL)
	{
		std::cout << "Error: Cannot create remote thread in the process.\n";
		return false;
	}

	CloseHandle(process);
	WaitForSingleObject(remoteThread, INFINITE);
	std::cout << "String was replaced.\n";
	CloseHandle(remoteThread);

	return true;
}

