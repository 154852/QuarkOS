#include <stdio.h>
#include <syscall.h>

#define ENTRY_COUNT 256

DirEntry entries[ENTRY_COUNT];
int main(int argc, char** argv) {
	int count;

	if (argc == 1) {
		count = read_dir("/", entries, ENTRY_COUNT);
		if (count == -EFILENOTFOUND) {
			printf("ls: /: No such file or directory\n");
			exit(1);
		}
	} else {
		count = read_dir(argv[1], entries, ENTRY_COUNT);
		if (count == -EFILENOTFOUND) {
			printf("ls: %s: No such file or directory\n", argv[1]);
			exit(1);
		}
	}

	for (int i = 0; i < count; i++) {
		switch (entries[i].type) {
			case FT_Socket: {
				printf("%s=\n", entries[i].name);
				break;
			}
			case FT_File: {
				printf("%s\n", entries[i].name);
				break;
			}
			case FT_Directory: {
				printf("%s/\n", entries[i].name);
				break;
			}
		}
	}

	return 0;
}