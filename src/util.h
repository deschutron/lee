#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <stdbool.h>
#include "numtypes.h"

#if defined(UINTPTR_MAX) && UINTPTR_MAX == 18446744073709551615UL
	#define ENV64
#elif defined(UINTPTR_MAX) && UINTPTR_MAX == 4294967295U
	#define ENV32
#else
	// let it fail a static assert check in lee.c
	#define ENV64
#endif
#define IS_LITTLE_ENDIAN ((u8)*(u32*)"\1\0\0\0")

#define stru16(x)  (*(u16*)(x))
#define stru16z(z) (*(u16*)(x "\0"))
#define stru32(x)  (*(u32*)(x))
#define stru32z(x) (*(u32*)(x "\0\0\0"))
#define stru64(x)  (*(u64*)(x))
#define stru64z(x) (*(u64*)(x "\0\0\0\0""\0\0\0"))
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define cmp(x, y) ((x) > (y) ? 1 : (x) < (y) ? -1 : 0)
void* free_from(void*, void*);
bool u32_can_be_printed(u32);
bool u32_can_be_printed_like_identifier(u32);
bool u64_can_be_printed(u64);
bool u64_can_be_printed_like_identifier(u64);
void print_rimek_byte(u8);
void print_rimek_bytes(void*, unt);

#endif
