#pragma once
#include "Task.h"
#include <queue>

class ThreadPool
{
public:
	ThreadPool(INT threadCount);
	VOID EnqueueTask(TASK *task);
	VOID Close();
	~ThreadPool();
private:
	INT threadCount;
	CRITICAL_SECTION lock;
	CONDITION_VARIABLE conditionVariable;
	volatile BOOL isClosed;
	HANDLE *threads;
	std::queue<TASK*> tasks;

	TASK* DequeueTask();
	static DWORD WINAPI ThreadFunc(LPVOID param);
};

