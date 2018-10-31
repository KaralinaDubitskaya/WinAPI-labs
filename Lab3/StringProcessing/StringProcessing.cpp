// StringProcessing.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#define BUFFER_SIZE 89120000

void ReplaceString(HANDLE hProc, LPVOID lpAddress, PBYTE pSource,
	INT buffSize, const char *oldStr, const char *newStr);
BOOL ReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid);

// Replace all occurences of the string (oldStr) with the new one (newStr) located in
// the virtual memory of the specified process 
BOOL ReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid)
{
	// Length of the new string should be equal to the old one
	if (strlen(oldStr) != strlen(newStr)) { return false; }

	// Get the process handle
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
	if (hProc == NULL) { return false; }

	// A pointer to the base address of the region of pages 
	LPCVOID lpPageAddress = (LPCVOID)0;

	// Info about a range of pages in the virtual memory of the process
	MEMORY_BASIC_INFORMATION vmInfo;

	while (VirtualQueryEx(hProc, lpPageAddress, &vmInfo, sizeof(vmInfo)) != 0)
	{
		INT numOfIterations = vmInfo.RegionSize / BUFFER_SIZE + (vmInfo.RegionSize % BUFFER_SIZE != 0 ? 1 : 0);
		LPVOID lpAddress = vmInfo.BaseAddress;

		for (int i = 0; i < numOfIterations; i++)
		{
			PBYTE pBuffer = new BYTE[BUFFER_SIZE];
			SIZE_T numOfBytesRead = 0;
			INT bufferSize = (i < numOfIterations - 1) ? BUFFER_SIZE : (vmInfo.RegionSize % BUFFER_SIZE);

			ReadProcessMemory(hProc, lpAddress, pBuffer, bufferSize, &numOfBytesRead);
			if (numOfBytesRead > 0)
			{
				ReplaceString(hProc, lpAddress, pBuffer, bufferSize, oldStr, newStr);
			}
			lpAddress = (PBYTE)lpAddress + bufferSize;

			delete pBuffer;
		}

		lpPageAddress = lpAddress;
	}

	DWORD l = GetLastError();

	CloseHandle(hProc);
}


// Replace all occurences of the string (oldStr) with the new one (newStr) located in
// the specified virtual memory area copied to pSource which starts at the address 
// lpAddress in the specified process (hProc)
void ReplaceString(HANDLE hProc, LPVOID lpAddress, PBYTE pSource, 
	INT buffSize, const char *oldStr, const char *newStr)
{
	std::string strBuffer((char *)pSource);
	INT offset = 0;

	// Position of the string in the buffer
	INT position = std::string::npos;

	// Check all occurences
	while ((position = strBuffer.find(oldStr[0], offset)) != std::string::npos)
	{
		PBYTE pStringAddress = (PBYTE)lpAddress + position;
		PBYTE pBuffer = new BYTE[strlen(oldStr)];
		SIZE_T numOfBytesRead = 0;
		ReadProcessMemory(hProc, pStringAddress, pBuffer, strlen(oldStr), &numOfBytesRead);
		if (numOfBytesRead > 0)
		{
			std::string readString((char *)pBuffer);
			if (readString.find(oldStr) != std::string::npos)
			{
				// Previous access protection
				DWORD prevProtect;
				// Change the protection of the region that contains string
				if (VirtualProtectEx(hProc, pStringAddress, strlen(newStr), PAGE_READWRITE, &prevProtect))
				{
					WriteProcessMemory(hProc, pStringAddress, newStr, strlen(newStr), NULL);
				}
			}
		}
		delete pBuffer;
		offset = position + 1;
	}
}
