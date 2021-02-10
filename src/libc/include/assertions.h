#ifndef _ASSERTIONS_H
#define _ASSERTIONS_H
 
#include <sys/cdefs.h>
#include <stdio.h>

#define hang while (1) {};

__attribute__((noreturn, always_inline)) inline void __assertion_fail(const char* message, unsigned int line, const char* file) {
	debugf("%s at %s, line %u\n", message, file, line);
	#ifndef __is_kernel
	printf("%s at %s, line %u\n", message, file, line);
	#endif
	hang;
}

#define assert(expression) if (!(expression)) __assertion_fail("Assertion failed: " #expression, __LINE__, __FILE__);
#define todo __assertion_fail("Not yet implemented", __LINE__, __FILE__);
#define assert_not_reached __assertion_fail("Assert not reached, reached", __LINE__, __FILE__);

#endif