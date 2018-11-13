#include "stdafx.h"
#include "StringSorter.h"
using namespace std;

#define DELIMITER '\n'

StringSorter::StringSorter(int threadCount)
{
	this->threadCount = threadCount;
	this->threadPool = new ThreadPool(threadCount);
	this->lock = new CRITICAL_SECTION();
}

VOID StringSorter::sort(LPSTR inputFile, LPSTR outputFile)
{
	vector<string>* stringVector = getStrings(inputFile);
	vector<vector<string>*>* stringVectors = divideVector(stringVector, threadCount);
	delete stringVector;
	
	TASK *tasks = new TASK[stringVectors->size()];
	PARAM *params = new PARAM[stringVectors->size()];
	for (int i = 0; i < stringVectors->size(); i++)
	{
		params[i].sorter = this;
		params[i].stringVector = stringVectors->at(i);
		tasks[i].func = sortStringVector;
		tasks[i].param = &params[i];
		EnterCriticalSection(lock);
		runtimeThreadCount++;
		threadPool->EnqueueTask(&tasks[i]);
		LeaveCriticalSection(lock);
	}

	while (true)
	{
		EnterCriticalSection(lock);
		if (!runtimeThreadCount) { break; }
		LeaveCriticalSection(lock);
		Sleep(200);
	}

	delete[] params;
	delete[] tasks;
	vector<string> *result = mergeSortedVectors(stringVectors);
	delete stringVectors;
	writeStringsToFile(outputFile, result);
}

StringSorter::~StringSorter()
{
	threadPool->Close();
}

vector<string>* StringSorter::getStrings(LPSTR inputFile)
{
	ifstream ifstream(inputFile);
	vector<string>* result = new vector<string>();
	while (!ifstream.eof())
	{
		string str;
		getline(ifstream, str, DELIMITER);
		str += DELIMITER;
		result->push_back(str);
	}
	ifstream.close();
	return result;
}

void StringSorter::writeStringsToFile(LPSTR file, vector<string>* strings)
{
	ofstream stream(file);
	for (string str : *strings)
	{
		const char *cstr = str.c_str();
		stream.write(cstr, strlen(cstr));
	}
	stream.close();
	delete strings;
}

vector<vector<string>*>* StringSorter::divideVector(vector<string>* strings, int numOfGroups)
{
	vector<vector<string>*>* result = new vector<vector<string>*>();
	// Strings per group
	int count = strings->size() / numOfGroups;
	count += (strings->size() % numOfGroups) ? 1 : 0;
	int index = 0;
	for (int i = 0; i < numOfGroups; i++)
	{
		vector<string>* stringVector = new vector<string>();
		for (int j = 0; j < count && index < strings->size(); j++)
		{
			stringVector->push_back(strings->at(index));
			index++;
		}
		result->push_back(stringVector);
	}
	return result;
}

DWORD WINAPI StringSorter::sortStringVector(LPVOID param)
{
	PARAM *paramStruct = (PARAM*)param;
	StringSorter *sorter = paramStruct->sorter;
	vector<string> *vector = paramStruct->stringVector;
	std::sort(vector->begin(), vector->end());
	EnterCriticalSection(sorter->lock);
	sorter->runtimeThreadCount--;
	LeaveCriticalSection(sorter->lock);
	return 0;
}

vector<string>* StringSorter::mergeSortedVectors(vector<vector<string>*>* vectors)
{
	while (vectors->size() != 1)
	{
		int vectorCount = vectors->size() / 2;
		vector<vector<string>*> removedVectors;
		for (int i = 0; i < vectorCount; i++)
		{
			int j = i * 2;
			vector<string>* sourceVector = vectors->at(j);
			vector<string>* destinationVector = vectors->at(j + 1);
			for (int k = 0; k < sourceVector->size(); k++)
			{
				vector<string>::iterator iterator = destinationVector->begin();
				string str = sourceVector->at(k);
				int l = 0;
				while (str > *iterator && l < destinationVector->size())
				{
					if (l != destinationVector->size() - 1)
					{
						iterator++;
					}
					l++;
				}
				if (l == destinationVector->size())
				{
					destinationVector->push_back(str);
				}
				else
				{
					destinationVector->insert(iterator, str);
				}
			}
			removedVectors.push_back(sourceVector);
		}
		for (int i = 0; i < vectorCount; i++)
		{
			vectors->erase(vectors->begin() + i);
		}
		for (vector<string>* vector : removedVectors)
		{
			delete vector;
		}
	}
	return vectors->at(0);
}