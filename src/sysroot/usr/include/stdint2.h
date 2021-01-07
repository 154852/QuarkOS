#ifndef _STDINT2_H
#define _STDINT2_H 1
 
#include <sys/cdefs.h>
#include <stdint.h>
 
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long u64;
typedef long i64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
typedef char i8;

typedef u32 physical_addr_t;
typedef u32 linear_addr_t;
 
#ifdef __cplusplus
}
#endif
 
#endif