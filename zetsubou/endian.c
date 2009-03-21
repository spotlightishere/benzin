/*
 *  endian.c
 *  Zetsubou
 *
 *  Created by Alex Marshall on 09/03/17.
 *  Copyright 2009 HACKERCHANNEL. All rights reserved.
 *
 *  Licensed under the terms of the GNU GPLv2.
 *
 */

#include "types.h"
#include "endian.h"

u16 be16(u16 x)
{
	return (x>>8) | 
        (x<<8);
}

u32 be32(u32 x)
{
	return (x>>24) | 
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
}

// __int64 for MSVC, "long long" for gcc
u64 be64(u64 x)
{
	return (x>>56) | 
        ((x<<40) & (u64)0x00FF000000000000LL) |
        ((x<<24) & (u64)0x0000FF0000000000LL) |
        ((x<<8)  & (u64)0x000000FF00000000LL) |
        ((x>>8)  & (u64)0x00000000FF000000LL) |
        ((x>>24) & (u64)0x0000000000FF0000LL) |
        ((x>>40) & (u64)0x000000000000FF00LL) |
        (x<<56);
}
