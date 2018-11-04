#pragma once

#include "Task.h"

class TasksQueue
{
public:
	TasksQueue();
	VOID push(TASK *task);
	TASK* pop();
	TASK* front();
	~TasksQueue();
private:
	TASK *tasks = NULL;
	volatile long count;
	volatile long front;
	CRITICAL_SECTION lock;
};

