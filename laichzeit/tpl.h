/*****************************************************************************/
/*  Laichzeit                                                                */
/*  Part of the Benzin Project                                               */
/*  Copyright (c)2009 HACKERCHANNEL. Protected under the GNU GPLv2.          */
/* ------------------------------------------------------------------------- */
/*  tpl.h                                                                    */
/*  The header file for all the TPL stuff. Include this!                     */
/* ------------------------------------------------------------------------- */
/*  Most code contributed by:                                                */
/*          SquidMan                                                         */
/*****************************************************************************/

#ifndef TPL_H
#define TPL_H

#include "types.h"

#define USE_TPL

typedef enum {
	// I4 (4 bit intensity, 8x8 tiles)
	TPL_FORMAT_I4		= 0,
	// I8 (8 bit intensity, 8x4 tiles)
	TPL_FORMAT_I8		= 1,
	// IA4 (4 bit intensity with 4 bit alpha, 8x4 tiles)
	TPL_FORMAT_IA4		= 2,
	// IA8 (8 bit intensity with 8 bit alpha, 4x4 tiles)
	TPL_FORMAT_IA8		= 3,

	// RGB565 (4x4 tiles)
	TPL_FORMAT_RGB565	= 4,
	// RGB5A3 (*) (4x4 tiles)
	TPL_FORMAT_RGB5A3	= 5,
	// RGBA8 (4x4 tiles in two cache lines - first is AR and second is GB)
	TPL_FORMAT_RGBA8	= 6,

	// CI4 (4 bit color index, 8x8 tiles)
	TPL_FORMAT_CI4		= 8,
	// CI8 (8 bit color index, 8x4 tiles)
	TPL_FORMAT_CI8		= 9,
	// CI14X2 (14 bit color index, 4x4 tiles)
	TPL_FORMAT_CI14X2	= 10,

	// CMP (S3TC compressed, 2x2 blocks of 4x4 tiles)
	TPL_FORMAT_CMP		= 14
} TPLFormat;

typedef struct {
	u16		height;			// Height of the TPL image.
	u16		width;			// Width of the TPL image.
	TPLFormat	format;			// Format of the TPL image.
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
} TPLHeader;

typedef struct {
	char		filename[256];		// Filename.
	TPLHeader	header;			// Header.
	int		done;			// Is it done?
} TPLImage;

typedef struct {
	int		imgcnt;			// How many TPLs
	TPLImage*	imgs;			// The TPLs
} TPL;

TPL*	LaichTPL_Start();
int	LaichTPL_StartImage(TPL* tpl, char* file);
void	LaichTPL_SetImageFormat(TPL* tpl, int img, TPLFormat format);
void	LaichTPL_SetImageWrap(TPL* tpl, int img, u32 s, u32 t);
void	LaichTPL_SetImageFilters(TPL* tpl, int img, u32 min, u32 mag);
void	LaichTPL_SetImageLOD(TPL* tpl, int img, \
			f32 lod_bias, u8 edge_lod, u8 min_lod, u8 max_lod);
void	LaichTPL_EndImage(TPL* tpl, int img);
u8*	LaichTPL_CompileTPL(TPL* tpl, u32* fsize);

#endif //TPL_H
