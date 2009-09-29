/*
 *  endian.h
 *  Zetsubou
 *
 *  Created by Alex Marshall on 09/03/17.
 *  Copyright 2009 HACKERCHANNEL. All rights reserved.
 *
 *  Licensed under the GNU GPLv2.
 */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include "types.h"

#define le16	be16
#define le32	be32
#define le64	be64

u16 be16(u16 x);
u32 be32(u32 x);
u64 be64(u64 x);

#endif //_ENDIAN_H_
