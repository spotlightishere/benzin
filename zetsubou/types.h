/*****************************************************************************
 *  types.h                                                                  *
 *  Part of Zetsubou                                                         *
 *  Part of Benzin                                                           *
 *  The typedefs stuff.                                                      *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)      <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                             *
 *  Copyright (C)2009 comex                                                  *
 *  Copyright (C)2009 booto                                                  *
 *  Licensed under the GNU GPLv2.                                            *
 *****************************************************************************/

#ifndef __TYPEDEFS_H_
#define __TYPEDEFS_H_

typedef unsigned char			u8;
typedef unsigned short			u16;
typedef unsigned int			u32;
typedef unsigned long long		u64;
typedef volatile unsigned char		vu8;
typedef volatile unsigned short		vu16;
typedef volatile unsigned int		vu32;
typedef volatile unsigned long long	vu64;

typedef signed char			s8;
typedef signed short			s16;
typedef signed int			s32;
typedef signed long long		s64;
typedef volatile signed char		vs8;
typedef volatile signed short		vs16;
typedef volatile signed int		vs32;
typedef volatile signed long long	vs64;

typedef float				f32;
typedef double				f64;
typedef volatile float			vf32;
typedef volatile double			vf64;

typedef char fourcc[4];

#endif //__TYPEDEFS_H_
