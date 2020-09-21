#include <kernel/hardware/interrupts.h>

#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

typedef enum {
	Ring0 = 0,
	Ring3 = 3
} RingLevel;

typedef enum {
	TS_Runnable,
	TS_Running
} TaskState;

typedef struct {
	const char* name;
	void (*entry_point)();
	u32 pid;
	TaskState state;
	RingLevel ring;
} Task;

void create_task(Task* task, void(*f)(), const char* name, RingLevel ring);

#endif