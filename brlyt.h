/*
 *  brlyt.h
 *
 *
 *  Created by Alex Marshall on 09/01/27.
 *  Updated by Stephen Simpson on 09/03/11.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BRLYT_H_
#define BRLYT_H_

#include "types.h"

typedef struct
{
    fourcc        magic;            // RLYT
    u32        unk1;            // 0xFEFF0008
    u32        filesize;        // The filesize of the brlyt.
    u16        lyt_offset;        // Offset to the lyt1 section.
    u16        unk2;            // Number of sections.
} brlyt_header;

typedef struct
{
    fourcc        magic;            // The magic.
    u32        length;            // How long the entry is.
} brlyt_entry_header;

typedef struct
{
    fourcc        magic;            // The magic.
    u32        length;            // How long the entry is.
    u32        data_location;        // The location of the data in the BRLYT file.
} brlyt_entry;

typedef struct
{
    unsigned char flag1;
    unsigned char flag2;
    unsigned char alpha;
    unsigned char alpha2;
    char name[24];
    float x;
    float y;
    float z;
    float flip_x;
    float flip_y;
    float angle;
    float xmag;
    float ymag;
    float width;
    float height;
} brlyt_pane_chunk;

typedef struct
{
    u16 len1;
    u16 len2;
    u16 mat_off;
    u16 font_idx;
    u8 unk4;
    u8 pad[3];    // [0, 0, 0]
    u32 name_offs;
    u32 color1;
    u32 color2;
    float font_size_x;
    float font_size_y;
    float char_space;
    float line_space;
} brlyt_text_chunk;

typedef struct
{
    u32 vtx_colors[4];    // [4294967295L, 4294967295L, 4294967295L, 4294967295L]
    u16 mat_off;
    u8 num_texcoords;
    u8 padding;        // 0
} brlyt_pic_chunk;

typedef struct
{
	float unk1[4];		// all 0x00000000
	u8 count;		// 0x01
	u8 padding[3];		// 0x00 0x00 0x00
	u32 offset1;		// 0x00000068   offset to 0xffffffff's
	u32 offset2;		// 0x0000007c	offset to last 8bytes of wnd1
} brlyt_wnd;

typedef struct			// pointed at by offset1
{
	u32 unk1[4];		// all 0xffffffff
} brlyt_wnd1;

typedef struct
{
	u16 unk1[6];		// 0x0019 0x0000 0x0000 0x0080 0x001a 0x0000
} brlyt_wnd2ish;

typedef struct
{
	u16 unk1;		// 0x0000
	u16 unk2;		// 0x0100
} brlyt_wnd2;

typedef struct
{
	float texcoords[8];	// 0x00 0x00 3f800000 0x00 0x00 3f800000 3f800000 3f800000
} brlyt_wnd3;

typedef struct			// pointed to by offset2
{
	u32 offset;		// offset to something
} brlyt_wnd4;

typedef struct
{
	u32 unk1;		// material number ??
} brlyt_wnd4_mat;

typedef struct			// 0003-0005
{
	u32 unk1[2];		// 0xb4 0xb8
	u16 unk2[8];		// 0x0012 0x0000 0x0013 0x0000 0x0014 0x0500 0x0015 0x0200
} brlyt_wnd_addon2;

// !!	bytes 0-7 header
// !!	bytes 8-b flags/alpha
// !!	bytes c-23 name
// !!	floats 24-4b
// !	floats 4c-5b
// !	byte 5c count
// !	bytes 60-63 offset	// 4 8 and c are words  12 is a byte following that is tex coords
// !	byte 64-67 offset
//	<count> words that are an offset to a struct of (short, bytes) short is a mat offset


typedef struct
{
    char a;
    char pad[3];
    float width;
    float height;
} brlyt_lytheader_chunk;

typedef struct
{
    char name[16];
    u16 numsubs;
    u16 unk;
} brlyt_group_chunk;

typedef struct
{
    u16 num;
    u16 offs;
} brlyt_numoffs_chunk;

typedef struct
{
    char name[20];
    s16 black_color[4];
    s16 white_color[4];
    s16 unk_color_2[4];
    u32 tev_kcolor[4];
    u32 flags;
} brlyt_material_chunk;

typedef struct
{
    int offset;
    int unk;
} brlyt_offsunk_chunk;

typedef struct
{
    u16 tex_offs;
    u8 wrap_s;
    u8 wrap_t;
} brlyt_texref_chunk;

typedef struct		// useful for texture offset and wrapping
{			// 1st number is x offset; 2nd number is y offset
    float unk[5];	// 3rd number is currently unknown
} brlyt_ua2_chunk;	// 4th number is number of horiz. spans; 5th is number of vert. span

typedef struct
{
    u8 unk[4];
} brlyt_4b_chunk;

typedef struct
{
    u32 a;
    u32 b;
    float c;
    float d;  // found to be floating point  * 04/22 *
    float e;  // found to be floating point  * 04/22 *
} brlyt_ua7_chunk;

typedef struct
{
    u8 unk[16];
} brlyt_10b_chunk;

void swapBytes(unsigned char* char1, unsigned char* char2);
int bitExtraction(unsigned int num, unsigned int start, unsigned int end);

void parse_brlyt(char *filename);
void make_brlyt(char* infile, char* outfile);

#endif //BRLYT_H_

