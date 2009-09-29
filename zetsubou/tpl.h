/*****************************************************************************
 *  tpl.h                                                                    *
 *  Part of Zetsubou                                                         *
 *  Part of Benzin                                                           *
 *  The TPL handling shiz.                                                   *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)      <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                             *
 *  Copyright (C)2009 comex                                                  *
 *  Copyright (C)2009 booto                                                  *
 *  Licensed under the GNU GPLv2.                                            *
 *****************************************************************************/

#ifndef _TPL_H_
#define _TPL_H_

#include "types.h"

// I4 (4 bit intensity, 8x8 tiles)
#define TPL_FORMAT_I4		0
// I8 (8 bit intensity, 8x4 tiles)
#define TPL_FORMAT_I8		1

// IA4 (4 bit intensity with 4 bit alpha, 8x4 tiles)
#define TPL_FORMAT_IA4		2
// IA8 (8 bit intensity with 8 bit alpha, 4x4 tiles)
#define TPL_FORMAT_IA8		3

// RGB565 (4x4 tiles)
#define TPL_FORMAT_RGB565	4
// RGB5A3 (*) (4x4 tiles)
#define TPL_FORMAT_RGB5A3	5
// RGBA8 (4x4 tiles in two cache lines - first is AR and second is GB)
#define TPL_FORMAT_RGBA8	6

// CI4 (4 bit color index, 8x8 tiles)
#define TPL_FORMAT_CI4		8
// CI8 (8 bit color index, 8x4 tiles)
#define TPL_FORMAT_CI8		9
// CI14X2 (14 bit color index, 4x4 tiles)
#define TPL_FORMAT_CI14X2	10

// CMP (S3TC compressed, 2x2 blocks of 4x4 tiles)
#define TPL_FORMAT_CMP		14

// Normal Min Filter value. (Unknown value)
#define TPL_MIN_FILTER_NORMAL	0x00010000
// Clamp Min Filter value? (Unknown value)
#define TPL_MIN_FILTER_CLAMP	0x00000001
// Normal Mag Filter value. (Unknown value)
#define TPL_MAG_FILTER_NORMAL	0x00010000
// Clamp Mag Filter value? (Unknown value)
#define TPL_MAG_FILTER_CLAMP	0x00000001

typedef struct texthead
{
	u16		height;			// Height of the TPL image.
	u16		width;			// Width of the TPL image.
	u32		format;			// Format of the TPL image. (Use #defines pl0x.)
	u32		offs;			// Offset to actual image data.
	u32		wrap_s;			// Wrap S.
	u32		wrap_t;			// Wrap T.
	u32		min;			// Min filter.
	u32		mag;			// Mag filter.
	f32		lod_bias;		// LOD Bias?
	u8		edge_lod;		// Edge LOD?
	u8		min_lod;		// Minimum LOD?
	u8		max_lod;		// Maximum LOD?
	u8		unpacked;		// Unpacked?
} TPL_header;

typedef struct bmphead
{
	u16		magic;			// 0x42 0x4D (ASCII for `BM')
	u32		size;			// The size of the file
	u16		reserved1;		// Reserved. (We'll stuff `TP' here)
	u16		reserved2;		// Reserved. (We'll stuff `LB' here)
	u32		offset;			// Offset to actual bitmap.
} TPL_BMPHeader;

typedef struct bmpv3infohead
{
	u32		headersize;		// Size of this header (40 bytes)
	s32		width;			// Width in pixels.
	s32		height;			// Height in pixels.
	u16		colorplanes;		// Number of color planes. Always 1.
	u16		bpp;			// Bits per pixel.
	u32		compression;		// Compression method. Just use 0, kthx.
	u32		bitmapsize;		// Size of the bitmap itself.
	u32		hres;			// Horizontal resolution. Just use 0, kthx.
	u32		vres;			// Vertical resolution. Just use 0, kthx.
	u32		colorpalette;		// Number of colors in palette. Just use 0, kthx.
	u32		importantcolors;	// Number of important colors. Just use 0, kthx.
} TPL_BMPInfoHeaderV3;

int TPL_ConvertToBMP(u8* tplbuf, u32 tplsize, char basename[]);
int TPL_ConvertToGD(u8* tplbuf, u32 tplsize, char basename[], u32 format);
int TPL_ConvertFromBMPs(const u32 count, char *bmps[], char outname[], u32 format);
int TPL_ConvertFromGDs(const u32 count, char *gds[], char outname[], u32 format, u32 typeformat);

#endif //_TPL_H_





















