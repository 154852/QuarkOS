#include <string.h>
 
int strcmp(const char* aptr, const char* bptr) {
	const unsigned char* a = (const unsigned char*) aptr;
	const unsigned char* b = (const unsigned char*) bptr;

	size_t size = strlen(aptr);
	if (strlen(bptr) != size) return -1;

	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}