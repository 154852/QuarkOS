#include <stdio.h>

int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		debugf("%s", argv[i]);
		if (i != argc - 1) debugf(" ");
	}
	debugf("\n");
}