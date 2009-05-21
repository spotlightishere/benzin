/*
 *  brlyt.c
 *  
 *
 *  Created by Alex Marshall on 09/01/27.
 *  Updated by Stephen Simpson on 09/03/11.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mxml.h>

#include "general.h"
#include "types.h"
#include "brlyt.h"
#include "memory.h"
#include "xml.h"
#include "endian.h"

#ifdef DEBUGBRLYT
#define dbgprintf    printf
#else
#define dbgprintf    //
#endif //DEBUGBRLYT

#define MAXIMUM_TAGS_SIZE        (0x1000)

char pic1_magic[] = "pic1";
char pan1_magic[] = "pan1";
char bnd1_magic[] = "bnd1";
char wnd1_magic[] = "wnd1";
char lyt1_magic[] = "lyt1";
char grp1_magic[] = "grp1";
char txl1_magic[] = "txl1";
char mat1_magic[] = "mat1";
char fnl1_magic[] = "fnl1";
char txt1_magic[] = "txt1";
char gre1_magic[] = "gre1";
char grs1_magic[] = "grs1";
char pae1_magic[] = "pae1";
char pas1_magic[] = "pas1";

static size_t BRLYT_fileoffset = 0;

char *materials;
int numberOfMaterials;
int lengthOfMaterials;
char *textures;
int numberOfTextures;
int lengthOfTextures;

static int FourCCsMatch(fourcc cc1, fourcc cc2)
{
    int ret[4];
    ret[0] = (cc1[0] == cc2[0]);
    ret[1] = (cc1[1] == cc2[1]);
    ret[2] = (cc1[2] == cc2[2]);
    ret[3] = (cc1[3] == cc2[3]);
    int retval;
    if(ret[0] && ret[1] && ret[2] && ret[3])
        retval = 1;
    else
        retval = 0;
    return retval;
}

static void BRLYT_ReadDataFromMemoryX(void* destination, void* input, size_t size)
{
    u8* out = (u8*)destination;
    u8* in = ((u8*)input) + BRLYT_fileoffset;
    memcpy(out, in, size);
}

static void BRLYT_ReadDataFromMemory(void* destination, void* input, size_t size)
{
    BRLYT_ReadDataFromMemoryX(destination, input, size);
    BRLYT_fileoffset += size;
}

float float_swap_bytes(float float1)
{
    unsigned char *float1c; float1c = (unsigned char*)&float1;

    unsigned char charTemp = 0x00;
    charTemp = float1c[0]; float1c[0] = float1c[3]; float1c[3] = charTemp;
    charTemp = 0x00;
    charTemp = float1c[1]; float1c[1] = float1c[2]; float1c[2] = charTemp;

    float *newFloat; newFloat = (float*)float1c;
    return *newFloat;
}

unsigned int bit_extract(unsigned int num, unsigned int start, unsigned int end)
{
    if (end == 100) end = start;
    //simple bitmask, figure out when awake
    //unsigned int mask = (2**(31 - start + 1) - 1) - (2**(31 - end) - 1)
    unsigned int mask;
    int first;
    int firstMask = 1;
    for (first=0;first<31-start+1;first++)
    {
        firstMask *= 2;
    }
    firstMask -= 1;
    int secondMask = 1;

    for (first=0;first< 31-end;first++)
    {
        secondMask *=2;
    }
    secondMask -= 1;
    mask = firstMask - secondMask;
    int ret = (num & mask) >> (31 - end);
    return ret;
}

char* getMaterial(u16 offset)
{

    if (offset == 0) return materials;
    char *mat = materials + strlen(materials) + 1;

    if (offset > 1)
    {
        int n;
        for (n=1; n<offset;n++)
            mat = mat + strlen(mat) + 1;
    }
    return mat;
}

char* getTexture(u16 offset)
{

    if (offset == 0) return textures;
    char *tex = textures + strlen(textures) + 1;

    if (offset > 1)
    {
        int n;
        for (n=1; n<offset;n++)
            tex = tex + strlen(tex) + 1;
    }
    return tex;
}

u16 findMatOffset(char *mats)
{
    int isEqual = 1;
    char* mat = materials;
    int i;
    for (i = 0; isEqual != 0; )
    {
        isEqual = strcmp(mats, mat);
        if (isEqual != 0) i++;
        mat = mat + strlen(mat) + 1;
    }
    return i;
}

u16 findTexOffset(char *tex)
{
    int isEqual = 1;
    char* texs = textures;
    int i;
    for (i = 0; isEqual != 0; )
    {
        isEqual = strcmp(tex, texs);
        if (isEqual != 0) i++;
        texs = texs + strlen(texs) + 1;
    }
    return i;
}

int BRLYT_ReadEntries(u8* brlyt_file, size_t file_size, brlyt_header header, brlyt_entry* entries)
{
	return 0;
}

void BRLYT_CheckHeaderSanity(brlyt_header header, size_t filesize)
{
    if((header.magic[0] != 'R') || (header.magic[1] != 'L') || (header.magic[2] != 'Y') || (header.magic[3] != 'T')) {
        printf("BRLYT magic doesn't match! %c%c%c%c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
        exit(1);
    }
    if(filesize != be32(header.filesize)) {
        printf("BRLYT filesize doesn't match!\n");
        exit(1);
    }
}

void PrintBRLYTEntry_lyt1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *a;
    mxml_node_t *size;
    mxml_node_t *width;
    mxml_node_t *height;
    brlyt_lytheader_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_lytheader_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    a = mxmlNewElement(tag, "a"); mxmlNewTextf(a, 0, "%02x", data.a);
    size = mxmlNewElement(tag, "size");
    width = mxmlNewElement(size, "width"); mxmlNewTextf(width, 0, "%f", float_swap_bytes(data.width));
    height = mxmlNewElement(size, "height"); mxmlNewTextf(height, 0,"%f", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_grp1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *subs;
    mxml_node_t *sub;
    brlyt_group_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_group_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    mxmlElementSetAttrf(tag, "name", "%s", data.name);

    if (short_swap_bytes(data.numsubs) > 0) subs = mxmlNewElement(tag, "subs");

    int offset;
    offset = 20;
    int n;
    for (n=0;n<short_swap_bytes(data.numsubs);n++)
    {
        char subb[16];
        BRLYT_ReadDataFromMemory(subb, brlyt_file, sizeof(subb));
        sub = mxmlNewElement(subs, "sub"); mxmlNewTextf(sub, 0, "%s", subb);
        offset += 16;
    }
}

void PrintBRLYTEntry_txl1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *entries;
    mxml_node_t *naame;
    brlyt_numoffs_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    entries = mxmlNewElement(tag, "entries");
    int pos = 4;
    pos += data.offs;
    int bpos = pos;
    int n;
    for (n=0;n<short_swap_bytes(data.num);n++)
    {
        brlyt_offsunk_chunk data2;
        BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_offsunk_chunk));
        int tempLocation = BRLYT_fileoffset;
        BRLYT_fileoffset = entry.data_location + bpos + be32(data2.offset);
        int toRead = (be32(entry.length) + entry.data_location - 8) - BRLYT_fileoffset;
        char nameRead[toRead];
        BRLYT_ReadDataFromMemory(nameRead, brlyt_file, sizeof(nameRead));

        char tpl = 0;
        char *ending = memchr(nameRead, tpl, toRead);
        int end = ending - nameRead;
        char name[end+1];
        memcpy(name, nameRead, sizeof(name));
        naame = mxmlNewElement(entries, "name"); mxmlNewTextf(naame, 0, "%s", name);
        BRLYT_fileoffset = tempLocation;
        int newSize = lengthOfTextures+sizeof(name);
        textures = realloc(textures, newSize);
        numberOfTextures += 1;
        memcpy(textures+lengthOfTextures, name, sizeof(name));
        lengthOfTextures = newSize;
    }
}

void PrintBRLYTEntry_fnl1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *entries;
    mxml_node_t *naame;
    brlyt_numoffs_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    entries = mxmlNewElement(tag, "entries");
    int pos = 4;
    pos += data.offs;
    int bpos = pos;
    int n;
    for (n=0;n<short_swap_bytes(data.num);n++)
    {
        brlyt_offsunk_chunk data2;
        BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_offsunk_chunk));
        int tempLocation = BRLYT_fileoffset;
        BRLYT_fileoffset = entry.data_location + bpos + be32(data2.offset);
        int toRead = (be32(entry.length) + entry.data_location - 8) - BRLYT_fileoffset;
        char nameRead[toRead];
        BRLYT_ReadDataFromMemory(nameRead, brlyt_file, sizeof(nameRead));

        char tpl = 0;
        char *ending = memchr(nameRead, tpl, toRead);
        int end = ending - nameRead;
        char name[end+1];
        memcpy(name, nameRead, sizeof(name));
        naame = mxmlNewElement(entries, "name"); mxmlNewTextf(naame, 0, "%s", name);
        BRLYT_fileoffset = tempLocation;
    }
}

void PrintBRLYTEntry_pan1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *flags1, *alpha1;
    mxml_node_t *coords, *x, *y, *z;
    mxml_node_t *flip, *rotate, *zoom;
    mxml_node_t *size, *width, *height;
    brlyt_pane_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    mxmlElementSetAttrf(tag, "name", "%s", data.name);
    flags1 = mxmlNewElement(tag, "flags"); mxmlNewTextf(flags1, 0, "%02x-%02x", data.flag1, data.flag2);
    alpha1 = mxmlNewElement(tag, "alpha"); mxmlNewTextf(alpha1, 0, "%02x-%02x", data.alpha, data.alpha2);
    coords = mxmlNewElement(tag, "coords");
    x = mxmlNewElement(coords, "x"); mxmlNewTextf(x, 0, "%.20f", float_swap_bytes(data.x));
    y = mxmlNewElement(coords, "y"); mxmlNewTextf(y, 0, "%.20f", float_swap_bytes(data.y));
    z = mxmlNewElement(coords, "z"); mxmlNewTextf(z, 0, "%.20f", float_swap_bytes(data.z));
    flip = mxmlNewElement(tag, "flip");
    x = mxmlNewElement(flip, "x"); mxmlNewTextf(x, 0, "%.20f", float_swap_bytes(data.flip_x));
    y = mxmlNewElement(flip, "y"); mxmlNewTextf(y, 0, "%.20f", float_swap_bytes(data.flip_y));
    rotate = mxmlNewElement(tag, "rotate"); mxmlNewTextf(rotate, 0, "%.20f", float_swap_bytes(data.angle));
    zoom = mxmlNewElement(tag, "zoom");
    x = mxmlNewElement(zoom, "x"); mxmlNewTextf(x, 0, "%.10f", float_swap_bytes(data.xmag));
    y = mxmlNewElement(zoom, "y"); mxmlNewTextf(y, 0, "%.10f", float_swap_bytes(data.ymag));
    size = mxmlNewElement(tag, "size");
    width = mxmlNewElement(size, "width"); mxmlNewTextf(width, 0, "%f", float_swap_bytes(data.width));
    height = mxmlNewElement(size, "height"); mxmlNewTextf(height, 0, "%f", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_wnd1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *flags1, *alpha1;
    mxml_node_t *coords, *x, *y, *z;
    mxml_node_t *flip, *rotate, *zoom;
    mxml_node_t *size, *width, *height;
    brlyt_pane_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    mxmlElementSetAttrf(tag, "name", "%s", data.name);
    flags1 = mxmlNewElement(tag, "flags"); mxmlNewTextf(flags1, 0, "%02x-%02x", data.flag1, data.flag2);
    alpha1 = mxmlNewElement(tag, "alpha"); mxmlNewTextf(alpha1, 0, "%02x-%02x", data.alpha, data.alpha2);
    coords = mxmlNewElement(tag, "coords");
    x = mxmlNewElement(coords, "x"); mxmlNewTextf(x, 0, "%.20f", float_swap_bytes(data.x));
    y = mxmlNewElement(coords, "y"); mxmlNewTextf(y, 0, "%.20f", float_swap_bytes(data.y));
    z = mxmlNewElement(coords, "z"); mxmlNewTextf(z, 0, "%.20f", float_swap_bytes(data.z));
    flip = mxmlNewElement(tag, "flip");
    x = mxmlNewElement(flip, "x"); mxmlNewTextf(x, 0, "%f", float_swap_bytes(data.flip_x));
    y = mxmlNewElement(flip, "y"); mxmlNewTextf(y, 0, "%f", float_swap_bytes(data.flip_y));
    rotate = mxmlNewElement(tag, "rotate"); mxmlNewTextf(rotate, 0, "%f", float_swap_bytes(data.angle));
    zoom = mxmlNewElement(tag, "zoom");
    x = mxmlNewElement(zoom, "x"); mxmlNewTextf(x, 0, "%.10f", float_swap_bytes(data.xmag));
    y = mxmlNewElement(zoom, "y"); mxmlNewTextf(y, 0, "%.10f", float_swap_bytes(data.ymag));
    size = mxmlNewElement(tag, "size");
    width = mxmlNewElement(size, "width"); mxmlNewTextf(width, 0, "%f", float_swap_bytes(data.width));
    height = mxmlNewElement(size, "height"); mxmlNewTextf(height, 0, "%f", float_swap_bytes(data.height));

    mxml_node_t *wndd, *unkk, *count, *offset1, *offset2;
    int i;
    brlyt_wnd wndy;
    BRLYT_ReadDataFromMemory(&wndy, brlyt_file, sizeof(brlyt_wnd));
    wndd = mxmlNewElement(tag, "wnd");
    for(i=0;i<4;i++)
    {
        unkk = mxmlNewElement(wndd, "unk1"); mxmlNewTextf(unkk, 0, "%f", float_swap_bytes(wndy.unk1[i]));
    }
    count = mxmlNewElement(wndd, "count"); mxmlNewTextf(count, 0, "%02x", wndy.count);
    offset1 = mxmlNewElement(wndd, "offset1"); mxmlNewTextf(offset1, 0, "%08x", be32(wndy.offset1));
    offset2 = mxmlNewElement(wndd, "offset2"); mxmlNewTextf(offset2, 0, "%08x", be32(wndy.offset2));

    mxml_node_t *wnddd, *unkkk;
    brlyt_wnd1 wndy1;
    BRLYT_ReadDataFromMemory(&wndy1, brlyt_file, sizeof(brlyt_wnd1));
    wnddd = mxmlNewElement(tag, "wnd1");
    for(i=0;i<4;i++)
    {
        unkkk = mxmlNewElement(wnddd, "unk1"); mxmlNewTextf(unkkk, 0, "%08x", int_swap_bytes(wndy1.unk1[i]));
    }

    mxml_node_t *wndddd, *unkkkk, *unkkkk2;
    brlyt_wnd2 wndy2;
    BRLYT_ReadDataFromMemory(&wndy2, brlyt_file, sizeof(brlyt_wnd2));
    wndddd = mxmlNewElement(tag, "wnd2");
    unkkkk = mxmlNewElement(wndddd, "unk1"); mxmlNewTextf(unkkkk, 0, "%04x", short_swap_bytes(wndy2.unk1));
    unkkkk2 = mxmlNewElement(wndddd, "unk2"); mxmlNewTextf(unkkkk2, 0, "%04x", short_swap_bytes(wndy2.unk2));

    mxml_node_t *wnddddd, *texcoord;
    if(wndy.offset2 == be32(0x0000009c))
    {
        brlyt_wnd3 wndy3;
        BRLYT_ReadDataFromMemory(&wndy3, brlyt_file, sizeof(brlyt_wnd3));
        wnddddd = mxmlNewElement(tag, "wnd3");
        for(i=0;i<8;i++)
        {
            texcoord = mxmlNewElement(wnddddd, "texcoord"); mxmlNewTextf(texcoord, 0, "%f", float_swap_bytes(wndy3.texcoords[i]));
        }
    }

    mxml_node_t *wndddddd, *offsett;
    brlyt_wnd4 wndy4;
    for(i=0;i<wndy.count;i++)
    {
        BRLYT_ReadDataFromMemory(&wndy4, brlyt_file, sizeof(brlyt_wnd4));
        wndddddd = mxmlNewElement(tag, "wnd4");
        offsett = mxmlNewElement(wndddddd, "offset"); mxmlNewTextf(offsett, 0, "%08x", be32(wndy4.offset));
    }

    mxml_node_t *wndmat, *matrl;
    brlyt_wnd4_mat wndy4mat;
    for(i=0;i<wndy.count;i++)
    {
        BRLYT_ReadDataFromMemory(&wndy4mat, brlyt_file, sizeof(brlyt_wnd4_mat));
        wndmat = mxmlNewElement(tag, "wnd4mat");
        matrl = mxmlNewElement(wndmat, "material"); mxmlNewTextf(matrl, 0, "%08x", be32(wndy4mat.unk1));
    }
}

void PrintBRLYTEntry_bnd1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *flags1, *alpha1;
    mxml_node_t *coords, *x, *y, *z;
    mxml_node_t *flip, *rotate, *zoom;
    mxml_node_t *size, *width, *height;
    brlyt_pane_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    mxmlElementSetAttrf(tag, "name", "%s", data.name);
    flags1 = mxmlNewElement(tag, "flags"); mxmlNewTextf(flags1, 0, "%02x-%02x", data.flag1, data.flag2);
    alpha1 = mxmlNewElement(tag, "alpha"); mxmlNewTextf(alpha1, 0, "%02x-%02x", data.alpha, data.alpha2);
    coords = mxmlNewElement(tag, "coords");
    x = mxmlNewElement(coords, "x"); mxmlNewTextf(x, 0, "%.25f", float_swap_bytes(data.x));
    y = mxmlNewElement(coords, "y"); mxmlNewTextf(y, 0, "%.25f", float_swap_bytes(data.y));
    z = mxmlNewElement(coords, "z"); mxmlNewTextf(z, 0, "%.25f", float_swap_bytes(data.z));
    flip = mxmlNewElement(tag, "flip");
    x = mxmlNewElement(flip, "x"); mxmlNewTextf(x, 0, "%f", float_swap_bytes(data.flip_x));
    y = mxmlNewElement(flip, "y"); mxmlNewTextf(y, 0, "%f", float_swap_bytes(data.flip_y));
    rotate = mxmlNewElement(tag, "rotate"); mxmlNewTextf(rotate, 0, "%f", float_swap_bytes(data.angle));
    zoom = mxmlNewElement(tag, "zoom");
    x = mxmlNewElement(zoom, "x"); mxmlNewTextf(x, 0, "%.10f", float_swap_bytes(data.xmag));
    y = mxmlNewElement(zoom, "y"); mxmlNewTextf(y, 0, "%.10f", float_swap_bytes(data.ymag));
    size = mxmlNewElement(tag, "size");
    width = mxmlNewElement(size, "width"); mxmlNewTextf(width, 0, "%f", float_swap_bytes(data.width));
    height = mxmlNewElement(size, "height"); mxmlNewTextf(height, 0, "%f", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_pic1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *flags1, *alpha1;
    mxml_node_t *coords, *x, *y, *z;
    mxml_node_t *flip, *rotate, *zoom;
    mxml_node_t *size, *width, *height;
    brlyt_pane_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
    brlyt_pic_chunk data2;
    BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_pic_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    mxmlElementSetAttrf(tag, "name", "%s", data.name);
    flags1 = mxmlNewElement(tag, "flags"); mxmlNewTextf(flags1, 0, "%02x-%02x", data.flag1, data.flag2);
    alpha1 = mxmlNewElement(tag, "alpha"); mxmlNewTextf(alpha1, 0, "%02x-%02x", data.alpha, data.alpha2);
    coords = mxmlNewElement(tag, "coords");
    x = mxmlNewElement(coords, "x"); mxmlNewTextf(x, 0, "%.20f", float_swap_bytes(data.x));
    y = mxmlNewElement(coords, "y"); mxmlNewTextf(y, 0, "%.20f", float_swap_bytes(data.y));
    z = mxmlNewElement(coords, "z"); mxmlNewTextf(z, 0, "%.20f", float_swap_bytes(data.z));
    flip = mxmlNewElement(tag, "flip");
    x = mxmlNewElement(flip, "x"); mxmlNewTextf(x, 0, "%f", float_swap_bytes(data.flip_x));
    y = mxmlNewElement(flip, "y"); mxmlNewTextf(y, 0, "%f", float_swap_bytes(data.flip_y));
    rotate = mxmlNewElement(tag, "rotate"); mxmlNewTextf(rotate, 0, "%f", float_swap_bytes(data.angle));
    zoom = mxmlNewElement(tag, "zoom");
    x = mxmlNewElement(zoom, "x"); mxmlNewTextf(x, 0, "%.10f", float_swap_bytes(data.xmag));
    y = mxmlNewElement(zoom, "y"); mxmlNewTextf(y, 0, "%.10f", float_swap_bytes(data.ymag));
    size = mxmlNewElement(tag, "size");
    width = mxmlNewElement(size, "width"); mxmlNewTextf(width, 0, "%f", float_swap_bytes(data.width));
    height = mxmlNewElement(size, "height"); mxmlNewTextf(height, 0, "%f", float_swap_bytes(data.height));
    mxml_node_t *material, *color, *vtx, *coordinates, *set, *coord;
    material = mxmlNewElement(tag, "material"); mxmlElementSetAttrf(material, "name", "%s", getMaterial(short_swap_bytes(data2.mat_off)));
    color = mxmlNewElement(tag, "colors");
    int n; for (n=0;n<4;n++)
    {
        vtx = mxmlNewElement(color, "vtx"); mxmlNewTextf(vtx, 0, "%#08X", be32(data2.vtx_colors[n]));
    }
    coordinates = mxmlNewElement(tag, "coordinates");
    for (n=0;n<data2.num_texcoords;n++)
    {
        float texcoords[8];
        BRLYT_ReadDataFromMemory(texcoords, brlyt_file, sizeof(texcoords));
        int i;
        set = mxmlNewElement(coordinates, "set");
        for(i = 0; i < 8; i++)
        {
            coord = mxmlNewElement(set, "coord"); mxmlNewTextf(coord, 0, "%f", float_swap_bytes(texcoords[i]));
        }
    }
}

void PrintBRLYTEntry_txt1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *flags1, *alpha1;
    mxml_node_t *coords, *x, *y, *z;
    mxml_node_t *flip, *rotate, *zoom;
    mxml_node_t *size, *width, *height;
    brlyt_pane_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    mxmlElementSetAttrf(tag, "name", "%s", data.name);
    flags1 = mxmlNewElement(tag, "flags"); mxmlNewTextf(flags1, 0, "%02x-%02x", data.flag1, data.flag2);
    alpha1 = mxmlNewElement(tag, "alpha"); mxmlNewTextf(alpha1, 0, "%02x-%02x", data.alpha, data.alpha2);
    coords = mxmlNewElement(tag, "coords");
    x = mxmlNewElement(coords, "x"); mxmlNewTextf(x, 0, "%.16f", float_swap_bytes(data.x));
    y = mxmlNewElement(coords, "y"); mxmlNewTextf(y, 0, "%.16f", float_swap_bytes(data.y));
    z = mxmlNewElement(coords, "z"); mxmlNewTextf(z, 0, "%.16f", float_swap_bytes(data.z));
    flip = mxmlNewElement(tag, "flip");
    x = mxmlNewElement(flip, "x"); mxmlNewTextf(x, 0, "%f", float_swap_bytes(data.flip_x));
    y = mxmlNewElement(flip, "y"); mxmlNewTextf(y, 0, "%f", float_swap_bytes(data.flip_y));
    rotate = mxmlNewElement(tag, "rotate"); mxmlNewTextf(rotate, 0, "%f", float_swap_bytes(data.angle));
    zoom = mxmlNewElement(tag, "zoom");
    x = mxmlNewElement(zoom, "x"); mxmlNewTextf(x, 0, "%.10f", float_swap_bytes(data.xmag));
    y = mxmlNewElement(zoom, "y"); mxmlNewTextf(y, 0, "%.10f", float_swap_bytes(data.ymag));
    size = mxmlNewElement(tag, "size");
    width = mxmlNewElement(size, "width"); mxmlNewTextf(width, 0, "%.20f", float_swap_bytes(data.width));
    height = mxmlNewElement(size, "height"); mxmlNewTextf(height, 0, "%.20f", float_swap_bytes(data.height));
    mxml_node_t *length, *font, *xsize, *ysize, *charsize, *linesize, *unkk, *color, *text;
    brlyt_text_chunk data2;
    BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_text_chunk));
    unsigned char texty[short_swap_bytes(data2.len2)];
    memcpy(texty, &brlyt_file[BRLYT_fileoffset], short_swap_bytes(data2.len2));
    length = mxmlNewElement(tag, "length"); mxmlNewTextf(length, 0, "%04x-%04x", short_swap_bytes(data2.len2), short_swap_bytes(data2.len2));
    font = mxmlNewElement(tag, "font"); mxmlElementSetAttrf(font, "index", "%d", short_swap_bytes(data2.font_idx));
    xsize = mxmlNewElement(font, "xsize"); mxmlNewTextf(xsize, 0, "%f", float_swap_bytes(data2.font_size_x));
    ysize = mxmlNewElement(font, "ysize"); mxmlNewTextf(ysize, 0, "%f", float_swap_bytes(data2.font_size_y));
    charsize = mxmlNewElement(font, "charsize"); mxmlNewTextf(charsize, 0, "%f", float_swap_bytes(data2.char_space));
    linesize = mxmlNewElement(font, "linesize"); mxmlNewTextf(linesize, 0, "%f", float_swap_bytes(data2.line_space));
    unkk = mxmlNewElement(font, "unk"); mxmlNewTextf(unkk, 0, "%02x", data2.unk4);
    color = mxmlNewElement(tag, "color"); mxmlNewTextf(color, 0, "%08x-%08x", be32(data2.color1), be32(data2.color2));
//    int q; for(q=0;q<short_swap_bytes(data2.len2);q++) printf("%02x", texty[q]);    // S T U P I D   U T F 1 6    T E X T
    unsigned char textbuffer[4096];
    int q; for(q=0;q<short_swap_bytes(data2.len2);q++) sprintf((char*)&textbuffer[q*2], "%02x", texty[q]);
    text = mxmlNewElement(tag, "text"); mxmlNewTextf(text, 0, "%s", textbuffer);
}

void PrintBRLYTEntry_mat1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxml_node_t *entries, *colors, *forecolor, *backcolor, *unk2, *tev_k, *flags;
    brlyt_numoffs_chunk data;
    BRLYT_fileoffset = entry.data_location;
    BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
    int n = 0;
    for (n=0;n<short_swap_bytes(data.num);n++)
    {
        int offset;
        BRLYT_ReadDataFromMemory(&offset, brlyt_file, sizeof(offset));
        int tempDataLocation = BRLYT_fileoffset;
        BRLYT_fileoffset = entry.data_location + be32(offset) - 8;
        brlyt_material_chunk data3;
        BRLYT_ReadDataFromMemory(&data3, brlyt_file, sizeof(brlyt_material_chunk));

        unsigned int flaggs = be32(data3.flags);
        entries = mxmlNewElement(tag, "entries"); mxmlElementSetAttrf(entries, "name",  "%s", data3.name);
        colors = mxmlNewElement(entries, "colors");
        int i; for (i=0;i<4;i++)
        {
            forecolor = mxmlNewElement(colors, "forecolor");
            mxmlNewTextf(forecolor, 0, "%d", short_swap_bytes(data3.forecolor[i]));
        }
        for (i=0;i<4;i++)
        {
            backcolor = mxmlNewElement(colors, "backcolor");
            mxmlNewTextf(backcolor, 0, "%d", short_swap_bytes(data3.backcolor[i]));
        }
        for (i=0;i<4;i++)
        {
            unk2 = mxmlNewElement(colors, "unk2");
            mxmlNewTextf(unk2, 0, "%d", short_swap_bytes(data3.unk_color_2[i]));
        }
        for (i=0;i<4;i++)
        {
            tev_k = mxmlNewElement(colors, "tev_k");
            mxmlNewTextf(tev_k, 0, "%#08x", be32(data3.tev_kcolor[i]));
        }
        flags = mxmlNewElement(entries, "flags"); mxmlNewTextf(flags, 0, "%08x", be32(data3.flags));

        int newSize = lengthOfMaterials+strlen(data3.name)+1;
        materials = realloc(materials, newSize);
        numberOfMaterials += 1;
        memcpy(materials+lengthOfMaterials, data3.name, 1+strlen(data3.name));
        lengthOfMaterials = newSize;

        mxml_node_t *texture, *wrap_t, *wrap_s;
        int n = 0;
        for (n=0;n<bit_extract(flaggs, 28,31);n++)
        {
            brlyt_texref_chunk data4;
            BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_texref_chunk));
            int tplOffset = short_swap_bytes(data4.tex_offs);
            texture = mxmlNewElement(entries, "texture"); mxmlElementSetAttrf(texture, "name", "%s", getTexture(tplOffset));
            wrap_s = mxmlNewElement(texture, "wrap_s"); mxmlNewTextf(wrap_s, 0, "%02x", data4.wrap_s);
            wrap_t = mxmlNewElement(texture, "wrap_t"); mxmlNewTextf(wrap_t, 0, "%02x", data4.wrap_t);
        }

        mxml_node_t *tex_coords, *dataa;
//        # 0x14 * flags[24-27], followed by
                n = 0;
                for (n=0;n<bit_extract(flaggs, 24,27);n++)
                {
                        brlyt_tex_coords data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_tex_coords));
                        tex_coords = mxmlNewElement(entries, "tex_coords");
                        dataa = mxmlNewElement(tex_coords, "x1"); mxmlNewTextf(dataa, 0, "%.10f", float_swap_bytes(data4.x1));
                        dataa = mxmlNewElement(tex_coords, "y1"); mxmlNewTextf(dataa, 0, "%.10f", float_swap_bytes(data4.y1));
                        dataa = mxmlNewElement(tex_coords, "angle"); mxmlNewTextf(dataa, 0, "%.10f", float_swap_bytes(data4.angle));
                        dataa = mxmlNewElement(tex_coords, "x2"); mxmlNewTextf(dataa, 0, "%.10f", float_swap_bytes(data4.x2));
                        dataa = mxmlNewElement(tex_coords, "y2"); mxmlNewTextf(dataa, 0, "%.10f", float_swap_bytes(data4.y2));
                }
                
                mxml_node_t *ua3;
        //# 4*flags[20-23], followed by
                n = 0;
                for (n=0;n<bit_extract(flaggs, 20,23);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        ua3 = mxmlNewElement(entries, "ua3");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(ua3, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        }
                }
        //# Changing ua3 things
        //# 1st --> disappears.
        //# 2nd --> no visible effect.
        //# 3rd --> disappears.
        //# 4th --> no visible effect.

                mxml_node_t *ua4;
        //# 4 * flags[6]
                n = 0;
                for (n=0;n<bit_extract(flaggs, 6,100);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        ua4 = mxmlNewElement(entries, "ua4");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(ua4, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        }
                }
                
                mxml_node_t *ua5;
        //# 4 * flags[4]
                n = 0;
                for (n=0;n<bit_extract(flaggs, 4,100);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        ua5 = mxmlNewElement(entries, "ua5");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(ua5, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        }                        
                }
                
                mxml_node_t *ua6;
        //# 4 * flags[19]
                n = 0;
                for (n=0;n<bit_extract(flaggs, 19,100);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        ua6 = mxmlNewElement(entries, "ua6");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(ua6, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        }                        
                }
                
                mxml_node_t *ua7, *a, *b, *c, *d, *e;
                n = 0;
                for (n=0;n<bit_extract(flaggs, 17,18);n++)
                {
                        brlyt_ua7_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_ua7_chunk));
                        ua7 = mxmlNewElement(entries, "ua7");
                        a = mxmlNewElement(ua7, "a"); mxmlNewTextf(a, 0, "%08x", be32(data4.a));
                        b = mxmlNewElement(ua7, "b"); mxmlNewTextf(b, 0, "%08x", be32(data4.b));
                        c = mxmlNewElement(ua7, "c"); mxmlNewTextf(c, 0, "%f", float_swap_bytes(data4.c));
                        d = mxmlNewElement(ua7, "d"); mxmlNewTextf(d, 0, "%f", float_swap_bytes(data4.d));
                        e = mxmlNewElement(ua7, "e"); mxmlNewTextf(e, 0, "%f", float_swap_bytes(data4.e));
                }
                
                mxml_node_t *ua8;
        //# 4 * flags[14-16]
                n = 0;
                for (n=0;n<bit_extract(flaggs, 14,16);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        ua8 = mxmlNewElement(entries, "ua8");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(ua8, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        } 
                }
                
                mxml_node_t *ua9;
        //# 0x10 * flags[9-13]
                n = 0;
                for (n=0;n<bit_extract(flaggs, 9,13);n++)
                {
                        brlyt_10b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_10b_chunk));
                        ua9 = mxmlNewElement(entries, "ua9");
                        int j; for (j=0;j<16;j++)
                        {
                            dataa = mxmlNewElement(ua9, "data");
                            mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        }
                }
                
                mxml_node_t *uaa;
        //# 4 * flags[8], these are bytes btw
                n = 0;
                for (n=0;n<bit_extract(flaggs, 8,8);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        uaa = mxmlNewElement(entries, "uaa");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(uaa, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        } 
                }

                mxml_node_t *uab;
        //# 4 * flags[7]
                n = 0;
                for (n=0;n<bit_extract(flaggs, 7,7);n++)
                {
                        brlyt_4b_chunk data4;
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        uab = mxmlNewElement(entries, "uab");
                        int j; for (j=0;j<4;j++)
                        {
                            dataa = mxmlNewElement(uab, "data"); mxmlNewTextf(dataa, 0, "%02x", data4.unk[j]);
                        } 
                }
        BRLYT_fileoffset = tempDataLocation;        
    }
}

void PrintBRLYTEntry_gre1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
}

void PrintBRLYTEntry_grs1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
}

void PrintBRLYTEntry_pae1(brlyt_entry entry, u8* brlyt_file, mxml_node_t *tag)
{
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
}

void PrintBRLYTEntry_pas1(brlyt_entry entry, u8* brlyt_file, mxml_node_t* tag)
{
    mxmlElementSetAttrf(tag, "type", "%c%c%c%c", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
}

void PrintBRLYTEntries(brlyt_entry *entries, int entrycnt, u8* brlyt_file, mxml_node_t *xmlyt)
{
    int i;
    mxml_node_t *tag;    
    for(i = 0; i < entrycnt; i++) {
        tag = mxmlNewElement(xmlyt, "tag");
        if((FourCCsMatch(entries[i].magic, pan1_magic) == 1)) {
            PrintBRLYTEntry_pan1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, txt1_magic) == 1)) {
            PrintBRLYTEntry_txt1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, pic1_magic) == 1)) {
            PrintBRLYTEntry_pic1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, wnd1_magic) == 1)) {
            PrintBRLYTEntry_wnd1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, bnd1_magic) == 1)) {
            PrintBRLYTEntry_bnd1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, lyt1_magic) == 1)) {
            PrintBRLYTEntry_lyt1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, grp1_magic) == 1)) {
            PrintBRLYTEntry_grp1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, txl1_magic) == 1)) {
            PrintBRLYTEntry_txl1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, fnl1_magic) == 1)) {
            PrintBRLYTEntry_fnl1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, mat1_magic) == 1)) {
            PrintBRLYTEntry_mat1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, gre1_magic) == 1)) {
            PrintBRLYTEntry_gre1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, grs1_magic) == 1)) {
            PrintBRLYTEntry_grs1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, pae1_magic) == 1)) {
            PrintBRLYTEntry_pae1(entries[i], brlyt_file, tag);
        }else if((FourCCsMatch(entries[i].magic, pas1_magic) == 1)) {
            PrintBRLYTEntry_pas1(entries[i], brlyt_file, tag);
        }else{
        }
    }
}

void parse_brlyt(char *filename, char *filenameout)
{
    materials = (char*)malloc(12);
    numberOfMaterials = 0;
    lengthOfMaterials = 0;
    textures = (char*)malloc(12);
    numberOfTextures = 0;
    lengthOfTextures = 0;
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL) {
        printf("Error! Couldn't open %s!\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    u8* brlyt_file = (u8*)malloc(file_size);
    fseek(fp, 0, SEEK_SET);
    fread(brlyt_file, file_size, 1, fp);
    fclose(fp);
    BRLYT_fileoffset = 0;
    brlyt_header header;
    BRLYT_ReadDataFromMemory(&header, brlyt_file, sizeof(brlyt_header));
    BRLYT_CheckHeaderSanity(header, file_size);
    brlyt_entry *entries;
    BRLYT_fileoffset = short_swap_bytes(header.lyt_offset);
    brlyt_entry_header tempentry;
    int i;
    for(i = 0; BRLYT_fileoffset < file_size; i++) {
        BRLYT_ReadDataFromMemoryX(&tempentry, brlyt_file, sizeof(brlyt_entry_header));
        BRLYT_fileoffset += be32(tempentry.length);
    }
    int entrycount = i;
    entries = (brlyt_entry*)calloc(entrycount, sizeof(brlyt_entry));
    if(entries == NULL) {
        printf("Couldn't allocate for entries!\n");
        exit(1);
    }
    BRLYT_fileoffset = short_swap_bytes(header.lyt_offset);
    for(i = 0; i < entrycount; i++) {
        BRLYT_ReadDataFromMemoryX(&tempentry, brlyt_file, sizeof(brlyt_entry_header));
        memcpy(entries[i].magic, tempentry.magic, 4);
        entries[i].length = tempentry.length;
        entries[i].data_location = BRLYT_fileoffset + sizeof(brlyt_entry_header);
        BRLYT_fileoffset += be32(tempentry.length);
    }    


    FILE *xmlFile;
    xmlFile = fopen(filenameout, "w");
    mxml_node_t *xml;
    mxml_node_t *xmlyt;
    xml = mxmlNewXML("1.0");
    xmlyt = mxmlNewElement(xml, "xmlyt");
    mxmlElementSetAttrf(xmlyt, "version", "%d.%d.%d%s", BENZIN_VERSION_MAJOR, BENZIN_VERSION_MINOR, BENZIN_VERSION_BUILD, BENZIN_VERSION_OTHER);
    PrintBRLYTEntries(entries, entrycount, brlyt_file, xmlyt);
    mxmlSaveFile(xml, xmlFile, whitespace_cb);
    fclose(xmlFile);
    mxmlDelete(xml);
    free(entries);
    free(brlyt_file);
    free(materials);
    free(textures);
}

void WriteBRLYTEntry(mxml_node_t *tree, mxml_node_t *node, u8** tagblob, u32* blobsize, char temp[4], FILE* fp, unsigned int *fileOffset)
{
    unsigned int startOfChunk = *fileOffset;

    char lyt1[4] = {'l', 'y', 't', '1'};
    char txl1[4] = {'t', 'x', 'l', '1'};
    char fnl1[4] = {'f', 'n', 'l', '1'};
    char mat1[4] = {'m', 'a', 't', '1'};
    char pan1[4] = {'p', 'a', 'n', '1'};
    char wnd1[4] = {'w', 'n', 'd', '1'};
    char bnd1[4] = {'b', 'n', 'd', '1'};
    char pic1[4] = {'p', 'i', 'c', '1'};
    char txt1[4] = {'t', 'x', 't', '1'};
    char grp1[4] = {'g', 'r', 'p', '1'};
    char grs1[4] = {'g', 'r', 's', '1'};
    char gre1[4] = {'g', 'r', 'e', '1'};
    char pas1[4] = {'p', 'a', 's', '1'};
    char pae1[4] = {'p', 'a', 'e', '1'};

    if ( memcmp(temp, lyt1, sizeof(lyt1)) == 0)
    {
        brlyt_lytheader_chunk lytheader;

        mxml_node_t *subnode = mxmlFindElement(node , node , "a", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempChar[4];
            get_value(subnode, tempChar, 256);
            lytheader.a = atoi(tempChar);
            lytheader.pad[0]=0;lytheader.pad[1]=0;lytheader.pad[2]=0;
        }
        subnode = mxmlFindElement(node, node, "size", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode = mxmlFindElement(subnode , subnode , "width", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempChar[4];
                get_value(valnode, tempChar, 256);
                *(float*)(&(lytheader.width)) = atof(tempChar);
            }
            valnode = mxmlFindElement(subnode , subnode ,"height", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempChar[4];
                get_value(valnode, tempChar, 256);
                
                *(float*)(&(lytheader.height)) = atof(tempChar);
            }
        }
        lytheader.pad[0] = 0; lytheader.pad[1] = 0; lytheader.pad[2] = 0;
        lytheader.width = float_swap_bytes(lytheader.width);
        lytheader.height = float_swap_bytes(lytheader.height);
        fwrite(&lytheader, sizeof(brlyt_header), 1, fp);
        *fileOffset = *fileOffset + sizeof(lytheader);
    }
    if ( memcmp(temp, txl1, sizeof(txl1)) == 0)
    {
        int numoffsOffset = ftell(fp);

        brlyt_numoffs_chunk chunk;
        chunk.num = 0;
        chunk.offs = 0;
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);

        char *names;
        names = malloc(sizeof(names));
        int lengthOfNames = 0;
        mxml_node_t *subnode = mxmlFindElement(node, node, "entries", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            int numEntries = 0;
        
            int oldNameLength;
            int baseOffset = 0;

            u32 *offsunks;

            offsunks = malloc(sizeof(u32));

            brlyt_offsunk_chunk chunk2;
            mxml_node_t *valnode;
            for(valnode = mxmlFindElement(subnode, subnode, "name", NULL, NULL, MXML_DESCEND); valnode != NULL; valnode = mxmlFindElement(valnode, subnode, "name", NULL, NULL, MXML_DESCEND)) {
                if (valnode != NULL)
                {
                    char tempSub[256];
                    get_value(valnode, tempSub, 256);

                    oldNameLength = lengthOfNames;
                    lengthOfNames += strlen(tempSub);

                    offsunks = realloc(offsunks, (numEntries + 1)*(2 * sizeof(u32)));
                    if (offsunks == NULL) printf("NULLed by a realloc\n");

                    offsunks[(numEntries*2)+0] = baseOffset;
                    offsunks[(numEntries*2)+1] = 0;

                    *fileOffset = *fileOffset + sizeof(chunk2);
                    numEntries++;
                    
                    baseOffset += (strlen(tempSub) + 1);

                    names = realloc(names, 1 + sizeof(char) * lengthOfNames);
                    strcpy(&names[oldNameLength], tempSub);
                    char nuller = '\0';
                    memcpy(&names[lengthOfNames], &nuller, sizeof(char));
                    lengthOfNames += 1;
                }
            }
            chunk.num = short_swap_bytes(numEntries);
            chunk.offs = 0;
            fseek(fp, numoffsOffset, SEEK_SET);
            fwrite(&chunk, sizeof(chunk), 1, fp);

            int i;
            for(i=0;i<numEntries;i++) {
                offsunks[(i*2)+0] = be32(offsunks[(i*2)+0] + (numEntries * 8));
                offsunks[i*2+1] = 0;    // MIGHT NOT BE 0 ? //

                fwrite(&offsunks[i*2], sizeof(u32), 1, fp);
                fwrite(&offsunks[i*2+1], sizeof(u32), 1, fp);
            }
            free(offsunks);
        }
        fseek(fp, *fileOffset, SEEK_SET);
        fwrite(names, lengthOfNames, 1, fp);
        *fileOffset = *fileOffset + lengthOfNames;
        if ((*fileOffset % 4) > 0)
        {
            u8 toAdd = 4-(*fileOffset % 4);
            char nuller[3] = {'\0', '\0', '\0'};
            fwrite(&nuller, sizeof(char), toAdd, fp);
            *fileOffset = *fileOffset + toAdd;
        }
        textures = malloc(lengthOfNames * sizeof(char));
        memcpy(textures, names, lengthOfNames * sizeof(char));
        free(names);
    }
    if ( memcmp(temp, fnl1, sizeof(fnl1)) == 0)
    {
        brlyt_numoffs_chunk chunk;
        chunk.num = 0;
        chunk.offs = 0;
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);

        char *names;
        names = malloc(sizeof(char));
        int lengthOfNames = 0;
        mxml_node_t *subnode = mxmlFindElement(node, node, "entries", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            int numEntries = 0;

            int oldNameLength;
            int baseOffset = 0;
            int numoffsOffset = *fileOffset - 4;

            u32 *offsunks;
            offsunks = malloc(sizeof(u32));

            brlyt_offsunk_chunk chunk2;
            mxml_node_t *valnode;
            for(valnode = mxmlFindElement(subnode, subnode, "name", NULL, NULL, MXML_DESCEND); valnode != NULL; valnode = mxmlFindElement(valnode, subnode, "name", NULL, NULL, MXML_DESCEND)) {
                if (valnode != NULL)
                {
                    char tempSub[256];
                    get_value(valnode, tempSub, 256);

                    oldNameLength = lengthOfNames;
                    lengthOfNames += strlen(tempSub);

                    offsunks = realloc(offsunks, (numEntries + 1)*(2 * sizeof(u32)));
                    offsunks[(numEntries*2)+0] = baseOffset;
                    offsunks[(numEntries*2)+1] = 0;

                    *fileOffset = *fileOffset + sizeof(chunk2);
                    numEntries++;
                    
                    baseOffset += (strlen(tempSub) + 1);
                    names = realloc(names, 1 + sizeof(char) * lengthOfNames);
                    memcpy(&names[oldNameLength], tempSub, strlen(tempSub));
                    char nuller = '\0';
                    memcpy(&names[lengthOfNames], &nuller, sizeof(char));
                    lengthOfNames += 1;
                }
            }
            chunk.num = short_swap_bytes(numEntries);
            chunk.offs = 0;
            fseek(fp, numoffsOffset, SEEK_SET);
            fwrite(&chunk, sizeof(chunk), 1, fp);

            int i;
            for(i=0;i<numEntries;i++) {
                offsunks[(i*2)+0] = be32(offsunks[(i*2)+0] + (numEntries * 8));
                offsunks[i*2+1] = 0;

                fwrite(&offsunks[i*2], sizeof(u32), 1, fp);
                fwrite(&offsunks[i*2+1], sizeof(u32), 1, fp);
            }
            free(offsunks);
        }
        fseek(fp, *fileOffset, SEEK_SET);
        fwrite(names, lengthOfNames, 1, fp);
        *fileOffset = *fileOffset + lengthOfNames;
        if ((*fileOffset % 4) > 0)
        {
            u8 toAdd = 4-(*fileOffset % 4);
            char nuller[3] = {'\0', '\0', '\0'};
            fwrite(&nuller, sizeof(char), toAdd, fp);
            *fileOffset = *fileOffset + toAdd;
        }
        free(names);
    }
    if ( memcmp(temp, mat1, sizeof(mat1)) == 0)
    {
        int numberOfEntries = 0;
        int actualNumber = 0;
        brlyt_numoffs_chunk numchunk;
        brlyt_material_chunk chunk;
        mxml_node_t *subnode;
        for(subnode=mxmlFindElement(node, node, "entries", NULL, NULL, MXML_DESCEND);subnode!=NULL;subnode=mxmlFindElement(subnode, node, "entries", NULL, NULL, MXML_DESCEND))
            numberOfEntries++;

        numchunk.num = short_swap_bytes(numberOfEntries);
        numchunk.offs = 0;
        fwrite(&numchunk, sizeof(numchunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(numchunk);

        int *offsets = calloc(numberOfEntries, sizeof(int));
        int offsetsOffset = ftell(fp);
        int materialOffset = 0;
        actualNumber = numberOfEntries;
        numberOfEntries = 0;
        int matSize = 0;
        int initialOffset = sizeof(numchunk) + 8 + (actualNumber * sizeof(int));

        offsets[numberOfEntries] = be32(matSize + initialOffset);
        fwrite(&offsets[0], sizeof(int), actualNumber, fp);
        *fileOffset = *fileOffset + (actualNumber * 4);

        for(subnode=mxmlFindElement(node,node,"entries",NULL,NULL,MXML_DESCEND);subnode!=NULL;subnode=mxmlFindElement(subnode,node,"entries",NULL,NULL,MXML_DESCEND))
        {
            numberOfEntries += 1;
            char temp[256];
            if(mxmlElementGetAttr(subnode, "name") != NULL)
                strcpy(temp, mxmlElementGetAttr(subnode, "name"));
            else{
                printf("No name attribute found!\nQuitting!\n");
                exit(1);
            }
            memset(chunk.name, 0, 20);
            strcpy(chunk.name, temp);

            materials = realloc(materials, lengthOfMaterials + strlen(chunk.name) + 1);
            memcpy(&materials[lengthOfMaterials], chunk.name, strlen(chunk.name) + 1);
            lengthOfMaterials += (1 + strlen(chunk.name));
            numberOfMaterials += 1;

            mxml_node_t *setnode;
            setnode = mxmlFindElement(subnode, subnode, "colors", NULL, NULL, MXML_DESCEND);
            if (setnode != NULL)
            {
                mxml_node_t *valnode;
                int colorNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "forecolor", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "forecolor", NULL, NULL, MXML_DESCEND) )
                {
                    char tempCoord[256];
                    get_value(valnode, tempCoord, 256);
                        
                    chunk.forecolor[colorNumber] = strtol(tempCoord, NULL, 10);
                    chunk.forecolor[colorNumber] = short_swap_bytes(chunk.forecolor[colorNumber]);

                    colorNumber+=1;
                }
                colorNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "backcolor", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "backcolor", NULL, NULL, MXML_DESCEND) )
                {
                    char tempCoord[256];
                    get_value(valnode, tempCoord, 256);
                    
                    chunk.backcolor[colorNumber] = strtol(tempCoord, NULL, 10);
                    chunk.backcolor[colorNumber] = short_swap_bytes(chunk.backcolor[colorNumber]);

                    colorNumber+=1;
                }
                colorNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "unk2", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "unk2", NULL, NULL, MXML_DESCEND) )
                {
                    char tempCoord[256];
                    get_value(valnode, tempCoord, 256);
                        
                    chunk.unk_color_2[colorNumber] = strtol(tempCoord, NULL, 10);
                    chunk.unk_color_2[colorNumber] = short_swap_bytes(chunk.unk_color_2[colorNumber]);

                    colorNumber+=1;
                }
                colorNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "tev_k", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "tev_k", NULL, NULL, MXML_DESCEND) )
                {
                    char tempCoord[256];
                    get_value(valnode, tempCoord, 256);
                    chunk.tev_kcolor[colorNumber] = be32(strtoul(tempCoord, NULL, 16));
                    colorNumber+=1;
                }
            }    
            setnode = mxmlFindElement(subnode,subnode,"flags",NULL,NULL,MXML_DESCEND);
            if (setnode != NULL)
            {
                char tempCoord[256];
                get_value(setnode, tempCoord, 256);
                chunk.flags = be32(strtoul(tempCoord, NULL, 16));
            }

            fwrite(&chunk, sizeof(chunk), 1, fp);
            *fileOffset = *fileOffset + sizeof(chunk);
            matSize += sizeof(chunk);

            brlyt_texref_chunk chunkTexRef;
            for(setnode = mxmlFindElement(subnode, subnode, "texture", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "texture", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                char temp[256];
                if(mxmlElementGetAttr(setnode, "name") != NULL)
                    strcpy(temp, mxmlElementGetAttr(setnode, "name"));
                else{
                    printf("No name attribute found!\nQuitting!\n");
                    exit(1);
                }
                chunkTexRef.tex_offs = short_swap_bytes(findTexOffset(temp));

                valnode=mxmlFindElement(setnode, setnode, "wrap_s", NULL, NULL, MXML_DESCEND);

                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunkTexRef.wrap_s = strtoul(tempCoord, NULL, 16);

                valnode=mxmlFindElement(setnode, setnode, "wrap_t", NULL, NULL, MXML_DESCEND);

                get_value(valnode, tempCoord, 256);
                chunkTexRef.wrap_t = strtoul(tempCoord, NULL, 16);

                fwrite(&chunkTexRef, sizeof(chunkTexRef), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkTexRef);
                matSize += sizeof(chunkTexRef);
            }

            brlyt_tex_coords chunkTexCoords;
            for(setnode = mxmlFindElement(subnode, subnode, "tex_coords", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "tex_coords", NULL, NULL, MXML_DESCEND))
            {
                char tempCoord[256];
                mxml_node_t *valnode;
                valnode=mxmlFindElement(setnode, setnode, "x1", NULL, NULL, MXML_DESCEND);
		memset(tempCoord, 0, 256);
                get_value(valnode, tempCoord, 256);
                chunkTexCoords.x1 = float_swap_bytes(atof(tempCoord));
                valnode=mxmlFindElement(setnode, setnode, "y1", NULL, NULL, MXML_DESCEND);
		memset(tempCoord, 0, 256);
                get_value(valnode, tempCoord, 256);
                chunkTexCoords.y1 = float_swap_bytes(atof(tempCoord));
                valnode=mxmlFindElement(setnode, setnode, "angle", NULL, NULL, MXML_DESCEND);
		memset(tempCoord, 0, 256);
                get_value(valnode, tempCoord, 256);
                chunkTexCoords.angle = float_swap_bytes(atof(tempCoord));
                valnode=mxmlFindElement(setnode, setnode, "x2", NULL, NULL, MXML_DESCEND);
		memset(tempCoord, 0, 256);
                get_value(valnode, tempCoord, 256);
                chunkTexCoords.x2 = float_swap_bytes(atof(tempCoord));
                valnode=mxmlFindElement(setnode, setnode, "y2", NULL, NULL, MXML_DESCEND);
		memset(tempCoord, 0, 256);
                get_value(valnode, tempCoord, 256);
                chunkTexCoords.y2 = float_swap_bytes(atof(tempCoord));
                fwrite(&chunkTexCoords, sizeof(chunkTexCoords), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkTexCoords);
                matSize += sizeof(chunkTexCoords);
            }
            brlyt_4b_chunk chunkUa3;
            for(setnode = mxmlFindElement(subnode, subnode, "ua3", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua3", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                    char tempCoord[256];
                    get_value(valnode, tempCoord, 256);
                    chunkUa3.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                    dataNumber+=1;
                }
                fwrite(&chunkUa3, sizeof(chunkUa3), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa3);
                matSize += sizeof(chunkUa3);
            }
            brlyt_4b_chunk chunkUa4;
            for(setnode = mxmlFindElement(subnode, subnode, "ua4", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua4", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa4.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUa4, sizeof(chunkUa4), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa4);
                matSize += sizeof(chunkUa4);
            }
            brlyt_4b_chunk chunkUa5;
            for(setnode = mxmlFindElement(subnode, subnode, "ua5", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua5", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa5.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUa5, sizeof(chunkUa5), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa5);
                matSize += sizeof(chunkUa5);
            }
            brlyt_4b_chunk chunkUa6;
            for(setnode = mxmlFindElement(subnode, subnode, "ua6", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua6", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa6.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUa6, sizeof(chunkUa6), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa6);
                matSize += sizeof(chunkUa6);
            }
            brlyt_ua7_chunk chunkUa7;
            for(setnode = mxmlFindElement(subnode, subnode, "ua7", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua7", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                valnode=mxmlFindElement(setnode, setnode, "a", NULL, NULL, MXML_DESCEND);
                if ( valnode != NULL )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa7.a = be32(strtoul(tempCoord, NULL, 16));
                }
                valnode=mxmlFindElement(setnode, setnode, "b", NULL, NULL, MXML_DESCEND);
                if ( valnode != NULL )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa7.b = be32(strtoul(tempCoord, NULL, 16));
                }
                valnode=mxmlFindElement(setnode, setnode, "c", NULL, NULL, MXML_DESCEND);
                if ( valnode != NULL )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa7.c = float_swap_bytes(atof(tempCoord));
                }
                valnode=mxmlFindElement(setnode, setnode, "d", NULL, NULL, MXML_DESCEND);
                if ( valnode != NULL )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa7.d = float_swap_bytes(atof(tempCoord));
                }
                valnode=mxmlFindElement(setnode, setnode, "e", NULL, NULL, MXML_DESCEND);
                if ( valnode != NULL )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa7.e = float_swap_bytes(atof(tempCoord));
                }
                fwrite(&chunkUa7, sizeof(chunkUa7), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa7);
                matSize += sizeof(chunkUa7);
            }
            brlyt_4b_chunk chunkUa8;
            for(setnode = mxmlFindElement(subnode, subnode, "ua8", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua8", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa8.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUa8, sizeof(chunkUa8), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa8);
                matSize += sizeof(chunkUa8);
            }
            brlyt_10b_chunk chunkUa9;
            for (setnode = mxmlFindElement(subnode, subnode, "ua9", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "ua9", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUa9.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUa9, sizeof(chunkUa9), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUa9);
                matSize += sizeof(chunkUa9);
            }
            brlyt_4b_chunk chunkUaa;
            for(setnode = mxmlFindElement(subnode, subnode, "uaa", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "uaa", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUaa.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUaa, sizeof(chunkUaa), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUaa);
                matSize += sizeof(chunkUaa);
            }
            brlyt_4b_chunk chunkUab;
            for(setnode = mxmlFindElement(subnode, subnode, "uab", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "uab", NULL, NULL, MXML_DESCEND))
            {
                mxml_node_t *valnode;
                int dataNumber = 0;
                for (valnode=mxmlFindElement(setnode, setnode, "data", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "data", NULL, NULL, MXML_DESCEND) )
                {
                        char tempCoord[256];
                        get_value(valnode, tempCoord, 256);
                        chunkUab.unk[dataNumber] = strtoul(tempCoord, NULL, 16);
                        dataNumber+=1;
                }
                fwrite(&chunkUab, sizeof(chunkUab), 1, fp);
                *fileOffset = *fileOffset + sizeof(chunkUab);
                matSize += sizeof(chunkUab);
            }
            if (numberOfEntries < actualNumber)
            {
                offsets[numberOfEntries] = be32(matSize + initialOffset);
                materialOffset = ftell(fp);
                fseek(fp, offsetsOffset+(numberOfEntries * sizeof(int)), SEEK_SET);
                fwrite(&offsets[numberOfEntries], sizeof(int), 1, fp);
                fseek(fp, materialOffset, SEEK_SET);
                initialOffset += matSize;
                matSize = 0;
            }
        }
        free(offsets);
    }
    if ( memcmp(temp, pan1, sizeof(pan1)) == 0)
    {
         brlyt_pane_chunk chunk;

        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\nQuitting!\n");
            exit(1);
        }
        memset(chunk.name, 0, 24);
        strcpy(chunk.name, temp);

        mxml_node_t *subnode = mxmlFindElement(node, node, "coords", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.y = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "z", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.z = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "flip", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_y = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "zoom", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.xmag = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.ymag = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "size", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "width", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.width = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "height", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.height = float_swap_bytes(atof(tempCoord));
            }
        }
        
        subnode = mxmlFindElement(node, node, "flags", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.flag1 = strtol(tempCoord, NULL, 16);
            chunk.flag2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "alpha", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.alpha = strtol(tempCoord, NULL, 16);
            chunk.alpha2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "rotate", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.angle = float_swap_bytes(atof(tempCoord));
        }
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);
    }
    if ( memcmp(temp, bnd1, sizeof(bnd1)) == 0)
    {
         brlyt_pane_chunk chunk;

        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\nQuitting!\n");
            exit(1);
        }
        memset(chunk.name, 0, 24 * sizeof(char));
        strcpy(chunk.name, temp);

        mxml_node_t *subnode = mxmlFindElement(node, node, "coords", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.y = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "z", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.z = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "flip", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_y = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "zoom", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.xmag = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.ymag = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "size", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "width", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.width = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "height", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.height = float_swap_bytes(atof(tempCoord));
            }
        }
        
        subnode = mxmlFindElement(node, node, "flags", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.flag1 = strtol(tempCoord, NULL, 16);
            chunk.flag2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "alpha", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.alpha = strtol(tempCoord, NULL, 16);
            chunk.alpha2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "rotate", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.angle = float_swap_bytes(atof(tempCoord));
        }
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);
    }
    if ( memcmp(temp, wnd1, sizeof(wnd1)) == 0)
    {
         brlyt_pane_chunk chunk;

        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\nQuitting!\n");
            exit(1);
        }
        memset(chunk.name, 0, 24 * sizeof(char));
        strcpy(chunk.name, temp);

        mxml_node_t *subnode = mxmlFindElement(node, node, "coords", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.y = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "z", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.z = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "flip", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_y = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "zoom", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.xmag = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.ymag = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "size", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "width", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.width = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "height", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.height = float_swap_bytes(atof(tempCoord));
            }
        }
        
        subnode = mxmlFindElement(node, node, "flags", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.flag1 = strtol(tempCoord, NULL, 16);
            chunk.flag2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "alpha", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.alpha = strtol(tempCoord, NULL, 16);
            chunk.alpha2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "rotate", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.angle = float_swap_bytes(atof(tempCoord));
        }
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);

        int i;
        brlyt_wnd wndy;
        subnode = mxmlFindElement(node, node, "wnd", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            i = 0;
            mxml_node_t *subsubnode;
            for(subsubnode = mxmlFindElement(subnode, subnode, "unk1", NULL, NULL, MXML_DESCEND); subsubnode != NULL; subsubnode = mxmlFindElement(subsubnode, subnode, "unk1", NULL, NULL, MXML_DESCEND))
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy.unk1[i] = float_swap_bytes(atof(tempCoord));
                i++;
            }
            subsubnode = mxmlFindElement(subnode, subnode, "count", NULL, NULL, MXML_DESCEND);
            if(subsubnode != NULL)
            {
                
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy.count = (u8)(strtoul(tempCoord, NULL, 16));
            }
            wndy.padding[0] = 0x0;
            wndy.padding[1] = 0x0;
            wndy.padding[2] = 0x0;

            subsubnode = mxmlFindElement(subnode, subnode, "offset1", NULL, NULL, MXML_DESCEND);
            if(subsubnode != NULL)
            {

                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy.offset1 = be32(strtoul(tempCoord, NULL, 16));
            }
            subsubnode = mxmlFindElement(subnode, subnode, "offset2", NULL, NULL, MXML_DESCEND);
            if(subsubnode != NULL)
            {

                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy.offset2 = be32(strtoul(tempCoord, NULL, 16));
            }
            fwrite(&wndy, sizeof(brlyt_wnd), 1, fp);
            *fileOffset = *fileOffset + sizeof(brlyt_wnd);
        }

        brlyt_wnd1 wndy1;
        subnode = mxmlFindElement(node, node, "wnd1", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            i=0;
            mxml_node_t *subsubnode;
            for(subsubnode = mxmlFindElement(subnode, subnode, "unk1", NULL, NULL, MXML_DESCEND); subsubnode != NULL; subsubnode = mxmlFindElement(subsubnode, subnode, "unk1", NULL, NULL, MXML_DESCEND))
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy1.unk1[i] = be32(strtoul(tempCoord, NULL, 16));
                i++;
            }
            fwrite(&wndy1, sizeof(brlyt_wnd1), 1, fp);
            *fileOffset = *fileOffset + sizeof(brlyt_wnd1);
        }

        brlyt_wnd2 wndy2;
        subnode = mxmlFindElement(node, node, "wnd2", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *subsubnode;
            subsubnode = mxmlFindElement(subnode, subnode, "unk1", NULL, NULL, MXML_DESCEND);
            if (subnode != NULL)
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy2.unk1 = short_swap_bytes(strtoul(tempCoord, NULL, 16));
            }
            subsubnode = mxmlFindElement(subnode, subnode, "unk2", NULL, NULL, MXML_DESCEND);
            if (subsubnode != NULL)
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy2.unk2 = short_swap_bytes(strtoul(tempCoord, NULL, 16));
            }
            fwrite(&wndy2, sizeof(brlyt_wnd2), 1, fp);
            *fileOffset = *fileOffset + sizeof(brlyt_wnd2);
        }

        brlyt_wnd3 wndy3;
        subnode = mxmlFindElement(node, node, "wnd3", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            i=0;
            mxml_node_t *subsubnode;
            for(subsubnode = mxmlFindElement(subnode, subnode, "texcoord", NULL, NULL, MXML_DESCEND); subsubnode != NULL; subsubnode = mxmlFindElement(subsubnode, subnode, "texcoord", NULL, NULL, MXML_DESCEND))
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy3.texcoords[i] = float_swap_bytes(atof(tempCoord));
                i++;
            }
            fwrite(&wndy3, sizeof(brlyt_wnd3), 1, fp);
            *fileOffset = *fileOffset + sizeof(brlyt_wnd3);
        }

        brlyt_wnd4 wndy4;
        for(subnode = mxmlFindElement(node, node, "wnd4", NULL, NULL, MXML_DESCEND); subnode != NULL; subnode = mxmlFindElement(subnode, node, "wnd4", NULL, NULL, MXML_DESCEND))
        {
            i=0;
            mxml_node_t *subsubnode;
            subsubnode = mxmlFindElement(subnode, subnode, "offset", NULL, NULL, MXML_DESCEND);
            if (subsubnode != NULL)
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy4.offset = be32(strtoul(tempCoord, NULL, 16));
                i++;
            }
            fwrite(&wndy4, sizeof(brlyt_wnd4), 1, fp);
            *fileOffset = *fileOffset + sizeof(brlyt_wnd4);
        }
        brlyt_wnd4_mat wndy4mat;
        for(subnode = mxmlFindElement(node, node, "wnd4mat", NULL, NULL, MXML_DESCEND); subnode != NULL; subnode = mxmlFindElement(subnode, node, "wnd4mat", NULL, NULL, MXML_DESCEND))
        {
            i=0;
            mxml_node_t *subsubnode;
            subsubnode = mxmlFindElement(subnode, subnode, "material", NULL, NULL, MXML_DESCEND);
            if (subsubnode != NULL)
            {
                char tempCoord[256];
                get_value(subsubnode, tempCoord, 256);
                wndy4mat.unk1 = be32(strtoul(tempCoord, NULL, 16));
                i++;
            }
            fwrite(&wndy4mat, sizeof(brlyt_wnd4_mat), 1, fp);
            *fileOffset = *fileOffset + sizeof(brlyt_wnd4_mat);
        }
    }
    if ( memcmp(temp, txt1, sizeof(txt1)) == 0)
    {
        brlyt_pane_chunk chunk;
        brlyt_text_chunk chunk2;

        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\nQuitting!\n");
            exit(1);
        }
        memset(chunk.name, 0, 24 * sizeof(char));
        strcpy(chunk.name, temp);
        chunk2.mat_off = short_swap_bytes(findMatOffset(temp));

        mxml_node_t *subnode = mxmlFindElement(node, node, "coords", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.y = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "z", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.z = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "flip", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_y = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "zoom", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.xmag = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.ymag = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "size", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "width", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.width = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "height", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.height = float_swap_bytes(atof(tempCoord));
            }
        }
        
        subnode = mxmlFindElement(node, node, "flags", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.flag1 = strtol(tempCoord, NULL, 16);
            chunk.flag2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "alpha", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.alpha = strtol(tempCoord, NULL, 16);
            chunk.alpha2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "rotate", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.angle = float_swap_bytes(atof(tempCoord));
        }

        subnode = mxmlFindElement(node, node, "font", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char temp[256];
            if(mxmlElementGetAttr(subnode, "index") != NULL)
                strcpy(temp, mxmlElementGetAttr(subnode, "index"));
            else{
                printf("No index attribute found!\nQuitting!\n");
                exit(1);
            }
            chunk2.font_idx = atoi(temp);
            chunk2.font_idx = short_swap_bytes(chunk2.font_idx);

            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "xsize", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk2.font_size_x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "ysize", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk2.font_size_y = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "charsize", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk2.char_space = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "linesize", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk2.line_space = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "unk", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk2.unk4 = strtol(tempCoord, NULL, 16);
            }
        }

        subnode = mxmlFindElement(node, node, "length", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk2.len1 = strtoul(tempCoord, NULL, 16);
            chunk2.len1 = short_swap_bytes(chunk2.len1);
            chunk2.len2 = strtoul(tempCoord+5, NULL, 16);
            chunk2.len2 = short_swap_bytes(chunk2.len2);
        }
        subnode = mxmlFindElement(node, node, "color", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk2.color1 = be32(strtoul(tempCoord, NULL, 16));
            chunk2.color2 = be32(strtoul(tempCoord+9, NULL, 16));
        }
        u16 textyLength = short_swap_bytes(chunk2.len2);
        if ((textyLength % 4) != 0)
            textyLength += (4 - (textyLength % 4));
        unsigned char texty[textyLength + 1];
    unsigned char tempy[textyLength*2];
        subnode = mxmlFindElement(node, node, "text", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            unsigned char tempCoord[8184];
            get_value(subnode, tempCoord, 8184);
            memcpy(tempy, tempCoord, short_swap_bytes(chunk2.len2)*2);
            u32 w; for (w=0;w<short_swap_bytes(chunk2.len2);w++)
            {
                u8 temp[3];
                temp[0] = tempy[w*2+0];
                temp[1] = tempy[w*2+1];
                temp[2] = 0x0;
                //printf("w: %d, temp: %s\n", w, temp);
                texty[w] = (u8)strtoul((char*)temp, NULL, 16);
            }
        }
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);

        chunk2.name_offs=be32(0x74);
        chunk2.pad[0]= 0;chunk2.pad[1]=0;chunk2.pad[2]=0;
        fwrite(&chunk2, sizeof(chunk2), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk2);

    int q;for (q=short_swap_bytes(chunk2.len2);q<textyLength;q++)
        {
            q++;
            texty[q] = 0x00;
        }
        fwrite(texty, textyLength, 1, fp);
        *fileOffset = *fileOffset + textyLength;
    }
    if ( memcmp(temp, pic1, sizeof(pic1)) == 0)
    {
        brlyt_pane_chunk chunk;

        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\nQuitting!\n");
            exit(1);
        }
        memset(chunk.name, 0, 24 * sizeof(char));
        strcpy(chunk.name, temp);

        mxml_node_t *subnode = mxmlFindElement(node, node, "coords", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.y = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "z", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.z = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "flip", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_x = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.flip_y = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "zoom", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "x", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.xmag = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "y", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.ymag = float_swap_bytes(atof(tempCoord));
            }
        }
        subnode = mxmlFindElement(node, node, "size", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            valnode = mxmlFindElement(subnode, subnode, "width", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.width = float_swap_bytes(atof(tempCoord));
            }
            valnode = mxmlFindElement(subnode, subnode, "height", NULL, NULL, MXML_DESCEND);
            if (valnode != NULL)
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk.height = float_swap_bytes(atof(tempCoord));
            }
        }
        
        subnode = mxmlFindElement(node, node, "flags", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.flag1 = strtol(tempCoord, NULL, 16);
            chunk.flag2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "alpha", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.alpha = strtol(tempCoord, NULL, 16);
            chunk.alpha2 = strtol(&(tempCoord[3]), NULL, 16);
        }
        subnode = mxmlFindElement(node, node, "rotate", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char tempCoord[256];
            get_value(subnode, tempCoord, 256);
            chunk.angle = float_swap_bytes(atof(tempCoord));
        }

        brlyt_pic_chunk chunk2;
        subnode = mxmlFindElement(node, node, "material", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            char temp[256];
            if(mxmlElementGetAttr(subnode, "name") != NULL)
                strcpy(temp, mxmlElementGetAttr(subnode, "name"));
            else{
                printf("No name attribute found!\nQuitting!\n");
                exit(1);
            }
            chunk2.mat_off = short_swap_bytes(findMatOffset(temp));
        }
        subnode = mxmlFindElement(node, node, "colors", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            int i=0;
            for(valnode = mxmlFindElement(subnode, subnode, "vtx", NULL, NULL, MXML_DESCEND); valnode != NULL; valnode = mxmlFindElement(valnode, subnode, "vtx", NULL, NULL, MXML_DESCEND))
            {
                char tempCoord[256];
                get_value(valnode, tempCoord, 256);
                chunk2.vtx_colors[i] = be32(strtoul(&(tempCoord[2]), NULL, 16));
                i++;
            }
        }
        u32 numberOfPicCoords = 0;
        u32 sets = 1;
        float* picCoords = malloc(8 * sizeof(float));
        subnode = mxmlFindElement(node, node, "coordinates", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *setnode;
            for (setnode = mxmlFindElement(subnode, subnode, "set", NULL, NULL, MXML_DESCEND); setnode != NULL; setnode = mxmlFindElement(setnode, subnode, "set", NULL, NULL, MXML_DESCEND))
            {
                picCoords = realloc(picCoords, sizeof(float) * 8 * sets);
                mxml_node_t *valnode;
                for (valnode=mxmlFindElement(setnode, setnode, "coord", NULL, NULL, MXML_DESCEND) ; valnode != NULL  ; valnode=mxmlFindElement(valnode, setnode, "coord", NULL, NULL, MXML_DESCEND) )
                {
                        
                        char tempCoord[256];
                        int j; for (j=0;j<256;j++) tempCoord[j]=0;
                        get_value(valnode, tempCoord, 256);
                        float tempCoordGotten = atof(tempCoord);
                        float coordGotten = float_swap_bytes(tempCoordGotten);
                        memcpy(&picCoords[numberOfPicCoords], &coordGotten, sizeof(float));
                        numberOfPicCoords++;
                }
                sets++;
            }
        }
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);
        chunk2.num_texcoords = (sets - 1);
        chunk2.padding = 0;
        fwrite(&chunk2, sizeof(chunk2), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk2);
        fwrite(picCoords, numberOfPicCoords * sizeof(float), 1, fp);
        *fileOffset = *fileOffset + (sizeof(float) * numberOfPicCoords);
        free(picCoords);
    }
    if ( memcmp(temp, grp1, sizeof(grp1)) == 0)
    {
        brlyt_group_chunk chunk;
        chunk.unk = 0;

        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\nQuitting!\n");
            exit(1);
        }
        memset(chunk.name, 0, 16 * sizeof(char));
        strcpy(chunk.name, temp);
        int numSubs = 0;
        char *subs;
        subs=malloc(0);
        u32 subsLength = 0;

        mxml_node_t *subnode = mxmlFindElement(node, node, "subs", NULL, NULL, MXML_DESCEND);
        if (subnode != NULL)
        {
            mxml_node_t *valnode;
            for(valnode = mxmlFindElement(subnode, subnode, "sub", NULL, NULL, MXML_DESCEND); valnode != NULL; valnode = mxmlFindElement(valnode, subnode, "sub", NULL, NULL, MXML_DESCEND)) {
            if (valnode != NULL)
            {
                    u32 oldSubsLength = subsLength;
                    char tempSub[256];
                    get_value(valnode, tempSub, 256);
                    subsLength += 16;
                    u8 stringLength = strlen(tempSub) + 1;
                    subs = realloc(subs, 1 + sizeof(char) * subsLength);
                    strcpy(&subs[oldSubsLength], tempSub);
                    for (stringLength=0;stringLength<17;stringLength++)
                        subs[oldSubsLength+stringLength] = 0x00;
                    numSubs++;
                }
            }
        }
        chunk.numsubs = short_swap_bytes(numSubs);
        fwrite(&chunk, sizeof(chunk), 1, fp);
        *fileOffset = *fileOffset + sizeof(chunk);
        fwrite(subs, sizeof(char) * subsLength, 1, fp);
        *fileOffset = *fileOffset + subsLength;
        if (subsLength > 0) free(subs);
    }
    if ( memcmp(temp, grs1, sizeof(grs1)) == 0)
    {

    }
    if ( memcmp(temp, gre1, sizeof(gre1)) == 0)
    {

    }
    if ( memcmp(temp, pas1, sizeof(pas1)) == 0)
    {

    }
    if ( memcmp(temp, pae1, sizeof(pae1)) == 0)
    {

    }

    *blobsize = *fileOffset - startOfChunk;
}

void WriteBRLYTHeader(brlyt_header rlythead, FILE* fp)
{
    brlyt_header writehead;
    writehead.magic[0] = rlythead.magic[0];
    writehead.magic[1] = rlythead.magic[1];
    writehead.magic[2] = rlythead.magic[2];
    writehead.magic[3] = rlythead.magic[3];
    writehead.unk1 = be32(rlythead.unk1);
    writehead.filesize = be32(rlythead.filesize);
    writehead.lyt_offset = short_swap_bytes(rlythead.lyt_offset);
    writehead.unk2 = short_swap_bytes(rlythead.unk2);

    fwrite(&writehead, sizeof(brlyt_header), 1, fp);
}

void write_brlyt(char *infile, char *outfile)
{
    unsigned int fileOffset = 0;
    FILE* fpx = fopen(infile, "r");
    if(fpx == NULL) {
        printf("xmlyt couldn't be opened!\n");
        exit(1);
    }
    mxml_node_t *hightree = mxmlLoadFile(NULL, fpx, MXML_TEXT_CALLBACK);
    if(hightree == NULL) {
        printf("Couldn't open hightree!\n");
        exit(1);
    }
    mxml_node_t *tree = mxmlFindElement(hightree, hightree, "xmlyt", NULL, NULL, MXML_DESCEND);
    if(hightree == NULL) {
        printf("Couldn't get tree!\n");
        exit(1);
    }
    mxml_node_t *node;
    FILE* fp = fopen(outfile, "wb+");
    if(fp == NULL) {
        printf("destination brlyt couldn't be opened!\n");
        exit(1);
    }

    u8* tagblob;
    u32 blobsize;
    u16 blobcount = 0;
    brlyt_header rlythead;
    rlythead.magic[0] = 'R';
    rlythead.magic[1] = 'L';
    rlythead.magic[2] = 'Y';
    rlythead.magic[3] = 'T';
    rlythead.unk1 = 0xFEFF0008;
    rlythead.filesize = 0;
    rlythead.lyt_offset = sizeof(brlyt_header);
    rlythead.unk2 = 1;
    WriteBRLYTHeader(rlythead, fp);

    fileOffset += sizeof(brlyt_header);

    u32 numberOfEntries;

    for(node = mxmlFindElement(tree, tree, "tag", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "tag", NULL, NULL, MXML_DESCEND)) {

        char tempType[4];
        if(mxmlElementGetAttr(node, "type") != NULL)
            strcpy(tempType, mxmlElementGetAttr(node, "type"));
        else{
            printf("No type attribute found!\n");
            exit(1);
        }

        unsigned int chunkHeaderOffset = fileOffset;

        brlyt_entry_header chunk;
        chunk.magic[0]=tempType[0];
        chunk.magic[1]=tempType[1];
        chunk.magic[2]=tempType[2];
        chunk.magic[3]=tempType[3];
        chunk.length = 0;

        fwrite(&chunk, sizeof(chunk), 1, fp);

        fileOffset += sizeof(chunk);

        WriteBRLYTEntry(tree, node, &tagblob, &blobsize, tempType, fp, &fileOffset);

        chunk.length = be32(blobsize + sizeof(chunk));

        fseek(fp, chunkHeaderOffset, SEEK_SET);
        fwrite(&chunk, sizeof(chunk), 1, fp);
        fseek(fp, fileOffset, SEEK_SET);

        numberOfEntries++;

        blobcount++;

    }

    rlythead.filesize = fileOffset;
    rlythead.unk2 = blobcount;
    fseek(fp, 0, SEEK_SET);
    WriteBRLYTHeader(rlythead, fp);

    fclose(fpx);
    fclose(fp);
}

void make_brlyt(char* infile, char* outfile)
{
    printf("\x1b[33mParsing XMLYT @ \x1b[0m%s.\n", infile);
    write_brlyt(infile, outfile);
    free(materials);
    free(textures);
    printf("\x1b[34mParsing XMLYT @ \x1b[0m%s complete.\n", infile);
}

