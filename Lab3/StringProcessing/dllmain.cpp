// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

__declspec(dllexport) BOOL WINAPI ReplaceStringInVirtualMemory(const char *oldStr, const char *newStr, DWORD pid);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}