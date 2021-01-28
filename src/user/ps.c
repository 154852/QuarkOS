#include "syscall.h"
#include <stdio.h>

char* state_name(ProcessStateSC state) {
	switch (state) {
	case PSSC_NotPresent:
		return "Invis";
	case PSSC_Exitting:
		return "Exit ";
	case PSSC_Idle:
		return "Idle ";
	case PSSC_Running:
		return "Exec ";
	default:
		return "???  ";
	}
} 

int main() {
	// unsigned int pids[100];
	// printf("PID   State   %.64s\n", "Name");
	// unsigned int length = list_process_pids((int*) pids, 100);

	// ProcessInfo info;
	// for (unsigned int i = 0; i < length; i++) {
	// 	info.pid = pids[i];
	// 	proc_info(&info);

	// 	printf("%.3u   %s   %.64s\n", pids[i], state_name(info.state), info.name);
	// }

	// return 0;
}
