#include "stdafx.h"
#include "ThreadPool.h"

#define SPIN_COUNT 4000

ThreadPool::ThreadPool(INT threadCount)
{
	//InitializeCriticalSectionAndSpinCount(&lock, SPIN_COUNT);
	//InitializeConditionVariable(&conditionVariable);
	isClosed = false;

	this->threadCount = threadCount;
	this->threads = new HANDLE[threadCount];
	for (int i = 0; i < threadCount; i++)
	{
		threads[i] = CreateThread(NULL, 0, ThreadFunc, this, 0, NULL);
	}
}

VOID ThreadPool::EnqueueTask(TASK *task)
{
	//EnterCriticalSection(&lock);
	tasks.push(task);
	//WakeAllConditionVariable(&conditionVariable);
	//LeaveCriticalSection(&lock);
}

VOID ThreadPool::Close()
{
	if (!isClosed)
	{
		//EnterCriticalSection(&lock);
		//if (!isClosed)
		{
			isClosed = true;
			//WakeAllConditionVariable(&conditionVariable);

			for (int i = 0; i < threadCount; i++)
			{
				WaitForSingleObject(threads[i], NULL);
				CloseHandle(threads[i]);
			}
			delete[] threads;
		}
		//LeaveCriticalSection(&lock);	
	}
}

ThreadPool::~ThreadPool()
{
	if (!isClosed)
	{
		Close();
	}
}

TASK* ThreadPool::DequeueTask()
{
	//EnterCriticalSection(&lock);

	while (tasks.size() == 0 && !isClosed)
	{
		// The queue is empty - sleep so consumers can put tasks
		//SleepConditionVariableCS(&conditionVariable, &lock, INFINITE);
		Sleep(500);
	}

	TASK *task = NULL;
	if (tasks.size() != 0)
	{
		task = tasks.front();
		tasks.pop();
	}

	//LeaveCriticalSection(&lock);
	return task;
}

DWORD WINAPI ThreadPool::ThreadFunc(LPVOID param)
{
	ThreadPool *threadPool = (ThreadPool*)param;

	while (!threadPool->isClosed)
	{
		TASK *task = threadPool->DequeueTask();
		if (!task)
		{
			break;
		}
		else
		{
			task->func(task->param);
		}
	}

	return 0;
}
