#pragma once
#include "Task.h"
#include "TasksQueue.h"

using namespace std;
#include <queue>

class ThreadPool
{
public:
	ThreadPool(INT threadCount);
	void enqueueTask(TASK* taskStruct);
	void Close();
	~ThreadPool();
private:
	//queue<TASK*> tasks;
	TasksQueue *tasks;
	HANDLE* threads;
	CRITICAL_SECTION lock;
	CONDITION_VARIABLE conditionVariable;
	volatile BOOL isClosed;
	INT threadCount;
	TASK* dequeueTask();
	static DWORD WINAPI ThreadFunc(LPVOID param);
};


