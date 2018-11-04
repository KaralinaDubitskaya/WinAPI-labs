#pragma once
#include "Task.h"
#include "TasksQueue.h"

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
	TasksQueue *tasks;

	TASK* DequeueTask();
	static DWORD WINAPI ThreadFunc(LPVOID param);
};

