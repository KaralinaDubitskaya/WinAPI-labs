#pragma once

#include "Windows.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include "ThreadPool.h"

using namespace std;

class StringSorter
{
public:
	StringSorter(int threadCount);
	VOID sort(LPSTR inputFile, LPSTR outputFile);
	~StringSorter();
private:
	static DWORD WINAPI sortStringVector(LPVOID param);

	ThreadPool *threadPool;
	int threadCount;
	INT runtimeThreadCount;
	vector<string> *getStrings(LPSTR inputFile);
	vector<vector<string>*>* divideVector(vector<string>* strings, int count);
	CRITICAL_SECTION *lock;

	struct PARAM
	{
		StringSorter *sorter;
		vector<string> *stringVector;
	};
};

