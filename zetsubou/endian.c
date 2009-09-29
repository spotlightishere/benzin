/*****************************************************************************
 *  endian.c                                                                 *
 *  Part of Zetsubou                                                         *
 *  Part of Benzin                                                           *
 *  Handles endianness.                                                      *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)      <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                             *
 *  Copyright (C)2009 comex                                                  *
 *  Copyright (C)2009 booto                                                  *
 *  Licensed under the GNU GPLv2.                                            *
 *****************************************************************************/

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
