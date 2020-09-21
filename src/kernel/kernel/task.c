#include <stdio.h>
#include <kernel/task.h>
#include <string.h>

static u32 next_pid = 0;

void create_task(Task* task, void(*f)(), const char* name, RingLevel ring) {
	task->name = name;
	task->entry_point = f;
	task->pid  = next_pid++;
	task->state = TS_Runnable;
	task->ring = ring;
}