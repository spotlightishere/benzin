/******************************************************************************
 *  endian.h                                                                  *
 *  Part of Benzin                                                            *
 *  Handles endianness.                                                       *
 *  Copyright (C)2009 SquidMan (Alex Marshall)        <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)       <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                              *
 *  Copyright (C)2009 comex                                                   *
 *  Copyright (C)2009 booto                                                   *
 *  All Rights Reserved, HACKERCHANNEL.                                       *
 ******************************************************************************/

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include "types.h"

u16 be16(u16 x);

u32 be32(u32 x);

u16 short_swap_bytes(u16 short1);

u32 int_swap_bytes(u32 int1);

// __int64 for MSVC, "long long" for gcc
u64 be64(u64 x);

#endif //_ENDIAN_H_
