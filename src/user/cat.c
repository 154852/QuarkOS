#include <stdio.h>
#include <syscall.h>

char buffer[1024];

int main(int argc, char** argv) {	
	if (argc == 1) {
		printf("usage: cat file\n");
		exit(1);
	}

	int fd = open(argv[1], FILE_FLAG_R);

	if (fd == -EFILENOTFOUND) {
		printf("cat: %s: not such file or directory\n", argv[1]);
		exit(1);
	}

	int len = read(fd, buffer, sizeof(buffer) - 1);
	buffer[len] = 0;
	puts(buffer);

	return 0;
}