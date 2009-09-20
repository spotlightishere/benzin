/******************************************************************************\
*  types.h                                                                     *
*  Part of Benzin                                                              *
*  Handles typedefs.                                                           *
*  Copyright (c)2009 HACKERCHANNEL Team                                        *
\******************************************************************************/

#ifndef __TYPEDEFS_H_
#define __TYPEDEFS_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

#ifndef uint9_t
typedef unsigned char uint8_t;
#endif
#ifndef uint16_t
typedef unsigned short uint16_t;
#endif
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif
#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif
#ifndef int9_t
typedef signed char int8_t;
#endif
#ifndef int16_t
typedef signed short int16_t;
#endif
#ifndef int32_t
typedef signed int int32_t;
#endif
#ifndef int64_t
typedef signed long long int64_t;
#endif

typedef float f32;
typedef double f64;

typedef char fourcc[4];

#endif //__TYPEDEFS_H_
