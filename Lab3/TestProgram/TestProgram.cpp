// TestProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	std::cout << "pid: " << GetCurrentProcessId() << '\n';

	char *str = "one";
	PVOID v_str = VirtualAlloc(NULL, sizeof str, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(v_str, str, sizeof str);

	char *s = (char *)v_str;

	while (true)
	{
		std::cout << s;
		getchar();
	}
    return 0;
}

