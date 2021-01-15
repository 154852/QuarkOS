#pragma GCC diagnostic ignored "-Wstrict-aliasing"

#include <softfloat.h>

double __floatunsidf(unsigned int i) {
	float64_t result = i32_to_f64(i);
	return *(double*) &result.v;
}

double __adddf3(double a, double b) {
	float64_t result = f64_add((float64_t) { *(uint64_t*) &a }, (float64_t) { *(uint64_t*) &b });
	return *(double*) &result.v;
}

double __muldf3(double a, double b) {
	float64_t result = f64_mul((float64_t) { *(uint64_t*) &a }, (float64_t) { *(uint64_t*) &b });
	return *(double*) &result.v;
}