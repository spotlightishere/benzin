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
    s16 tev_color[4];
    s16 unk_color[4];
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

typedef struct
{
    float unk[5];
} brlyt_ua2_chunk;

typedef struct
{
    u8 unk[4];
} brlyt_4b_chunk;

typedef struct
{
    u32 a;
    u32 b;
    float c;
    u32 d;
    u32 e;
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

