// Lab4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "StringSorter.h"

#define FILE_INPUT "input.txt"
#define FILE_OUTPUT "output.txt"

int main()
{
	int threadCount;
	cout << "Enter number of threads: ";
	cin >> threadCount;
	StringSorter sorter(threadCount);
	sorter.sort(FILE_INPUT, FILE_OUTPUT);
	printf("File was succesfully sorted\n");
	getchar();
	getchar();
	return 0;
}
