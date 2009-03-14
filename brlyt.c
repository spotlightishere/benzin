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

#include "general.h"
#include "types.h"
#include "brlyt.h"

#ifdef DEBUGBRLYT
#define dbgprintf	printf
#else
#define dbgprintf	//
#endif //DEBUGBRLYT

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

static int FourCCsMatch(fourcc cc1, fourcc cc2)
{
	dbgprintf("FourCCs\n");
	int ret[4];
	dbgprintf("Let's go %08x %08x\n", cc1, cc2);
	ret[0] = (cc1[0] == cc2[0]);
	dbgprintf("Got zero |%02x| |%02x| %d\n", cc1[0], cc2[0], ret[0]);
	ret[1] = (cc1[1] == cc2[1]);
	dbgprintf("Got one |%02x| |%02x| %d\n", cc1[1], cc2[1], ret[1]);
	ret[2] = (cc1[2] == cc2[2]);
	dbgprintf("Got two |%02x| |%02x| %d\n", cc1[2], cc2[2], ret[2]);
	ret[3] = (cc1[3] == cc2[3]);
	dbgprintf("Got three |%02x| |%02x| %d\n", cc1[3], cc2[3], ret[3]);
	int retval;
	if(ret[0] && ret[1] && ret[2] && ret[3])
		retval = 1;
	else
		retval = 0;
	dbgprintf("Got retval %d\n", retval);
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
	int first = 0;
	int firstMask = 1;
	for (first;first<31-start+1;first++)
	{
		firstMask *= 2;
	}
	firstMask -= 1;
	first = 0;
	int secondMask = 1;

	for (first;first< 31-end;first++)
	{
		secondMask *=2;
	}
	mask = firstMask = secondMask;
	int ret = (num & mask) >> (31 - end);
	//printf("%08x, %08x, %08x, %08x\n", firstMask, secondMask, mask, ret);
	return ret;
}

char* getMaterial(int offset)
{
/*
	dbgprintf("length of materials: %08x\n", lengthOfMaterials);
	int stringOffs=0;
	char *returnPointer;
	char *ending;
	int n;
	for (n = 0; n < offset+1; n++)
	{
		char tpl = 0;
		ending = memchr(materials+stringOffs, tpl, lengthOfMaterials-stringOffs);
		int length = ending - (materials+stringOffs);
		returnPointer = materials + stringOffs;
		dbgprintf("%p\t%08x\t%p\n", ending, length, materials+stringOffs);
		stringOffs = stringOffs + length + 1;
	}
	return returnPointer;
*/
	if (offset == 0) return materials;
	char *foo = materials + strlen(materials) + 1;

	if (offset > 1)
	{
		int n;
		for (n=0; n<offset-1;n++)
			foo = foo + strlen(foo) + 1;
	}

	return foo;
}

int BRLYT_ReadEntries(u8* brlyt_file, size_t file_size, brlyt_header header, brlyt_entry* entries)
{
/*	BRLYT_fileoffset = be16(header.lyt_offset);
	brlyt_entry_header tempentry;
	int i;
	dbgprintf("curr %08x max %08x\n", BRLYT_fileoffset, file_size);
	for(i = 0; BRLYT_fileoffset < file_size; i++) {
		BRLYT_ReadDataFromMemoryX(&tempentry, brlyt_file, sizeof(brlyt_entry_header));
		BRLYT_fileoffset += be32(tempentry.length);
		dbgprintf("curr %08x max %08x\n", BRLYT_fileoffset, file_size);
	}
	int entrycount = i;
	entries = (brlyt_entry*)calloc(i, sizeof(brlyt_entry));
	dbgprintf("%08x\n", entries);
	if(entries == NULL) {
		printf("Couldn't allocate for entries!\n");
		exit(1);
	}
	BRLYT_fileoffset = be16(header.lyt_offset);
	for(i = 0; i < entrycount; i++) {
		dbgprintf("&(entries[i].realhead) = %08x\n", &(entries[i].realhead));
		BRLYT_ReadDataFromMemoryX(&(entries[i].realhead), brlyt_file, sizeof(brlyt_entry_header));
		entries[i].data_location = BRLYT_fileoffset + sizeof(brlyt_entry_header);
		BRLYT_fileoffset += be32(tempentry.length);
	}
	return entrycount;*/
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

void PrintBRLYTEntry_lyt1(brlyt_entry entry, u8* brlyt_file)
{
	brlyt_lytheader_chunk data;
	BRLYT_fileoffset = entry.data_location;
	BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_lytheader_chunk));
	printf("		Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
	printf("		a: %08x\n", be32(data.a));
	printf("		width: %f\n", float_swap_bytes(data.width));
	printf("		height: %f\n", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_grp1(brlyt_entry entry, u8* brlyt_file)
{
	brlyt_group_chunk data;
	BRLYT_fileoffset = entry.data_location;
	BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_group_chunk));
	printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
	printf("		Name: %s\n", data.name);
	printf("		Number of subs: %08x\n", be16(data.numsubs));
	printf("		unk: %08x\n", be16(data.unk));

	int offset;
	offset = 20;
	int n = 0;
	for (n;n<data.numsubs;n++)
	{
		char sub[16];
		BRLYT_ReadDataFromMemory(sub, brlyt_file, sizeof(sub));
		printf("                sub: %s\n", sub);
		offset += 16;
	}
}

void PrintBRLYTEntry_txl1(brlyt_entry entry, u8* brlyt_file)
{
	brlyt_numoffs_chunk data;
	BRLYT_fileoffset = entry.data_location;
	BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
	printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
	printf("                num: %08x\n", be16(data.num));
	printf("                offs: %08x\n", be16(data.offs));
	int pos = 4;
	pos += data.offs;
	int bpos = pos;
	int n = 0;
	for (n;n<be16(data.num);n++)
	{
                brlyt_offsunk_chunk data2;
                BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_offsunk_chunk));
                //int data2.offset              //int data2.unk
                printf("                offset: %08x\n", be32(data2.offset));
                printf("                unk: %08x\n", be32(data2.unk));
                int tempLocation = BRLYT_fileoffset;
                BRLYT_fileoffset = entry.data_location + bpos + be32(data2.offset);
		int toRead = (be32(entry.length) + entry.data_location - 8) - BRLYT_fileoffset;
		char nameRead[toRead];
                BRLYT_ReadDataFromMemory(nameRead, brlyt_file, sizeof(nameRead));
                //char nameRead[toRead] the name of the tpls null terminated between
		char tpl = 0;
		char *ending = memchr(nameRead, tpl, toRead);
		int end = ending - nameRead;
		char name[end+1];
		memcpy(name, nameRead, sizeof(name));
		printf("                name: %s\n", name);
                BRLYT_fileoffset = tempLocation;
		int oldsize = sizeof(name);
		dbgprintf("size of materials before: %08x\n", sizeof(materials));
		int newSize = lengthOfMaterials+sizeof(name);
		materials = realloc(materials, newSize);
		numberOfMaterials += 1;
		memcpy(materials+lengthOfMaterials, name, sizeof(name));
		dbgprintf("mats: %s\tnamsize: %08x\tmatsize: %08x\tnewsize: %08x\n", materials+lengthOfMaterials, sizeof(name), lengthOfMaterials, newSize);
		lengthOfMaterials = newSize;
	}
}

void PrintBRLYTEntry_fnl1(brlyt_entry entry, u8* brlyt_file)
{
        brlyt_numoffs_chunk data;
        BRLYT_fileoffset = entry.data_location;
        BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
	printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        printf("                num: %08x\n", be16(data.num));
        printf("                offs: %08x\n", be16(data.offs));
        int pos = 4;
        pos += data.offs;
        int bpos = pos;
        int n = 0;
	for (n;n<be16(data.num);n++)
        {
                brlyt_offsunk_chunk data2;
                BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_offsunk_chunk));
                //int data2.offset              //int data2.unk
                printf("                offset: %08x\n", be32(data2.offset));
                printf("                unk: %08x\n", be32(data2.unk));
                int tempLocation = BRLYT_fileoffset;
                BRLYT_fileoffset = entry.data_location + bpos + be32(data2.offset);
                int toRead = (be32(entry.length) + entry.data_location - 8) - BRLYT_fileoffset;
                char nameRead[toRead];
                BRLYT_ReadDataFromMemory(nameRead, brlyt_file, sizeof(nameRead));
                //char nameRead[toRead] the name of the tpls null terminated between
                char tpl = 0;
                char *ending = memchr(nameRead, tpl, toRead);
                int end = ending - nameRead;
                char name[end+1];
                memcpy(name, nameRead, sizeof(name));
                printf("                name: %s\n", name);
                BRLYT_fileoffset = tempLocation;
        }
}

void PrintBRLYTEntry_pan1(brlyt_entry entry, u8* brlyt_file)
{
	brlyt_pane_chunk data;
	BRLYT_fileoffset = entry.data_location;
	BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        printf("                flag1: %08x\n", data.flag1);
        printf("                flag2: %08x\n", data.flag2);
        printf("                alpha: %08x\n", data.alpha);
        printf("                alpha2: %08x\n", data.alpha2);
        printf("                name: %s\n", data.name);
        printf("                x: %f\n", be32(data.x));
        printf("                y: %f\n", be32(data.y));
        printf("                z: %f\n", be32(data.z));
        printf("                flip_x: %f\n", float_swap_bytes(data.flip_x));
        printf("                flip_y: %f\n", float_swap_bytes(data.flip_y));
        printf("                angle: %f\n", float_swap_bytes(data.angle));
        printf("                xmag: %f\n", float_swap_bytes(data.xmag));
        printf("                ymag: %f\n", float_swap_bytes(data.ymag));
        printf("                width: %f\n", float_swap_bytes(data.width));
        printf("                height: %f\n", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_wnd1(brlyt_entry entry, u8* brlyt_file)
{
        brlyt_pane_chunk data;
        BRLYT_fileoffset = entry.data_location;
        BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        printf("                flag1: %08x\n", data.flag1);
        printf("                flag2: %08x\n", data.flag2);
        printf("                alpha: %08x\n", data.alpha);
        printf("                alpha2: %08x\n", data.alpha2);
        printf("                name: %s\n", data.name);
        printf("                x: %f\n", be32(data.x));
        printf("                y: %f\n", be32(data.y));
        printf("                z: %f\n", be32(data.z));
        printf("                flip_x: %f\n", float_swap_bytes(data.flip_x));
        printf("                flip_y: %f\n", float_swap_bytes(data.flip_y));
        printf("                angle: %f\n", float_swap_bytes(data.angle));
        printf("                xmag: %f\n", float_swap_bytes(data.xmag));
        printf("                ymag: %f\n", float_swap_bytes(data.ymag));
        printf("                width: %f\n", float_swap_bytes(data.width));
        printf("                height: %f\n", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_bnd1(brlyt_entry entry, u8* brlyt_file)
{
	dbgprintf("entry length: %08x\n", be32(entry.length));
	int w;
	for (w=0;w<be32(entry.length);w++) dbgprintf("byte %x: %02x\n", w, brlyt_file[w]);

        brlyt_pane_chunk data;
        BRLYT_fileoffset = entry.data_location;
        BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        printf("                flag1: %08x\n", data.flag1);
        printf("                flag2: %08x\n", data.flag2);
        printf("                alpha: %08x\n", data.alpha);
        printf("                alpha2: %08x\n", data.alpha2);
        printf("                name: %s\n", data.name);
        printf("                x: %f\n", be32(data.x));
        printf("                y: %f\n", be32(data.y));
        printf("                z: %f\n", be32(data.z));
        printf("                flip_x: %f\n", float_swap_bytes(data.flip_x));
        printf("                flip_y: %f\n", float_swap_bytes(data.flip_y));
        printf("                angle: %f\n", float_swap_bytes(data.angle));
        printf("                xmag: %f\n", float_swap_bytes(data.xmag));
        printf("                ymag: %f\n", float_swap_bytes(data.ymag));
        printf("                width: %f\n", float_swap_bytes(data.width));
        printf("                height: %f\n", float_swap_bytes(data.height));
}

void PrintBRLYTEntry_pic1(brlyt_entry entry, u8* brlyt_file)
{
        brlyt_pane_chunk data;
        BRLYT_fileoffset = entry.data_location;
        BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        printf("                flag1: %08x\n", data.flag1);
	printf("                flag2: %08x\n", data.flag2);
	printf("                alpha: %08x\n", data.alpha);
	printf("                alpha2: %08x\n", data.alpha2);
	printf("                name: %s\n", data.name);
	printf("                x: %f\n", be32(data.x));
	printf("                y: %f\n", be32(data.y));
	printf("                z: %f\n", be32(data.z));
	printf("                flip_x: %f\n", float_swap_bytes(data.flip_x));
	printf("                flip_y: %f\n", float_swap_bytes(data.flip_y));
	printf("                angle: %f\n", float_swap_bytes(data.angle));
	printf("                xmag: %f\n", float_swap_bytes(data.xmag));
	printf("                ymag: %f\n", float_swap_bytes(data.ymag));
	printf("                width: %f\n", float_swap_bytes(data.width));
	printf("                height: %f\n", float_swap_bytes(data.height));
	brlyt_pic_chunk data2;
	BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_pic_chunk));
	printf("                vtx_colors: %u,%u,%u,%u\n", be32(data2.vtx_colors[0]), be32(data2.vtx_colors[1]), be32(data2.vtx_colors[2]), be32(data2.vtx_colors[3]));
	printf("                mat_off: %08x\n", be16(data2.mat_off));
	printf("                num_texcoords: %08x\n", data2.num_texcoords);
	printf("                padding: %08x\n", data2.padding);
	int n = 0;
	for (n;n<data2.num_texcoords;n++)
	{
		float texcoords[8];		// I think that's what that means 
		BRLYT_ReadDataFromMemory(texcoords, brlyt_file, sizeof(texcoords));
        	printf("                tex coords: %f,%f,%f,%f,%f,%f,%f,%f\n", float_swap_bytes(texcoords[0]), float_swap_bytes(texcoords[1]), float_swap_bytes(texcoords[2]), float_swap_bytes(texcoords[3]), float_swap_bytes(texcoords[4]), float_swap_bytes(texcoords[5]), float_swap_bytes(texcoords[6]), float_swap_bytes(texcoords[7]));
	}
}

void PrintBRLYTEntry_txt1(brlyt_entry entry, u8* brlyt_file)
{
        brlyt_pane_chunk data;
        BRLYT_fileoffset = entry.data_location;
        BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_pane_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        printf("                flag1: %08x\n", data.flag1);
        printf("                flag2: %08x\n", data.flag2);
        printf("                alpha: %08x\n", data.alpha);
        printf("                alpha2: %08x\n", data.alpha2);
        printf("                name: %s\n", data.name);
        printf("                x: %f\n", be32(data.x));
        printf("                y: %f\n", be32(data.y));
        printf("                z: %f\n", be32(data.z));
        printf("                flip_x: %f\n", float_swap_bytes(data.flip_x));
        printf("                flip_y: %f\n", float_swap_bytes(data.flip_y));
        printf("                angle: %f\n", float_swap_bytes(data.angle));
        printf("                xmag: %f\n", float_swap_bytes(data.xmag));
        printf("                ymag: %f\n", float_swap_bytes(data.ymag));
        printf("                width: %f\n", float_swap_bytes(data.width));
        printf("                height: %f\n", float_swap_bytes(data.height));
	brlyt_text_chunk data2;
	BRLYT_ReadDataFromMemory(&data2, brlyt_file, sizeof(brlyt_text_chunk));
	printf("                len1: %08x\n", be16(data2.len1));
	printf("                len2: %08x\n", be16(data2.len2));
	printf("                mat_off: %08x\n", be16(data2.mat_off));
	printf("                font_idx: %08x\n", be16(data2.font_idx));
	printf("                unk4: %08x\n", data2.unk4);
	printf("                pad[3]: %08x\n", data2.pad[0], data2.pad[1], data2.pad[2]);
	printf("                name_offs: %08x\n", be32(data2.name_offs));
	printf("                color1: %08x\n", be32(data2.color1));
	printf("                color2: %08x\n", be32(data2.color2));
	printf("                len1: %08x\n", be16(data2.len1));
        printf("                font_size_x: %f\n", float_swap_bytes(data2.font_size_x));
        printf("                font_size_y: %f\n", float_swap_bytes(data2.font_size_y));
        printf("                char_space: %f\n", float_swap_bytes(data2.char_space));
        printf("                line_space: %f\n", float_swap_bytes(data2.line_space));
}

void PrintBRLYTEntry_mat1(brlyt_entry entry, u8* brlyt_file)
{
	brlyt_numoffs_chunk data;
	BRLYT_fileoffset = entry.data_location;
	BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
	printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
	printf("                num: %08x\n", be16(data.num));
	printf("                offs: %08x\n", be16(data.offs));
	int n = 0;
	for (n;n<be16(data.num);n++)
	{
		int offset;
		BRLYT_ReadDataFromMemory(&offset, brlyt_file, sizeof(offset));
		printf("                offset: %08x\n", be32(offset));
		int tempDataLocation = BRLYT_fileoffset;
		BRLYT_fileoffset = entry.data_location + be32(offset) - 8;
		brlyt_material_chunk data3;
		BRLYT_ReadDataFromMemory(&data3, brlyt_file, sizeof(brlyt_material_chunk));
		printf("                name: %s\n", data3.name);
		printf("                tev_color: %#x,%#x,%#x,%#x\n", be16(data3.tev_color[0]), be16(data3.tev_color[1]), be16(data3.tev_color[2]), be16(data3.tev_color[3]));
		printf("                unk_color: %#x,%#x,%#x,%#x\n", be16(data3.unk_color[0]), be16(data3.unk_color[1]), be16(data3.unk_color[2]), be16(data3.unk_color[3]));
		printf("                unk_color_2: %#x,%#x,%#x,%#x\n", be16(data3.unk_color_2[0]), be16(data3.unk_color_2[1]), be16(data3.unk_color_2[2]), be16(data3.unk_color_2[3]));
		printf("                tev_kcolor: %#x,%#x,%#x,%#x\n", be32(data3.tev_kcolor[0]), be32(data3.tev_kcolor[1]), be32(data3.tev_kcolor[2]), be32(data3.tev_kcolor[3]));
		printf("                flags: %08x\n", be32(data3.flags));
		
		//more junk to do with bit masks and flags
		//mat_texref = get_array(chunk, mpos, bit_extract(data3.flags, 28,31), 4, 'texref');
		unsigned int flaggs = be32(data3.flags);
		printf("                bitmask: %08x\n", bit_extract(flaggs, 28, 31));
		int n = 0;
		for (n;n<bit_extract(flaggs, 28,31);n++)
		{
			brlyt_texref_chunk data4;
			BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_texref_chunk));
			printf("                 texoffs: %08x\n", be16(data4.tex_offs));
			printf("                 wrap_s: %08x\n", data4.wrap_s);
			printf("                 wrap_t: %08x\n", data4.wrap_t);
			dbgprintf("                 name: %s\n", getMaterial(be32(data4.tex_offs)));
		}

//		# 0x14 * flags[24-27], followed by
                n = 0;
                for (n;n<bit_extract(flaggs, 24,27);n++)
                {
                        brlyt_ua2_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_ua2_chunk));
                        printf("                 ua2: %08x, %08x, %08x, %08x, %08x\n", float_swap_bytes(data4.unk[0]), float_swap_bytes(data4.unk[1]), float_swap_bytes(data4.unk[2]), float_swap_bytes(data4.unk[3]), float_swap_bytes(data4.unk[4]));
                        //pos += item_size;
                }
		//# 4*flags[20-23], followed by
                n = 0;
                for (n;n<bit_extract(flaggs, 20,23);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 ua3: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
		//# Changing ua3 things
		//# 1st --> disappears.
		//# 2nd --> no visible effect.
		//# 3rd --> disappears.
		//# 4th --> no visible effect.

		//# 4 * flags[6]
                n = 0;
                for (n;n<bit_extract(flaggs, 6,100);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 ua4: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
		//# 4 * flags[4]
                n = 0;
                for (n;n<bit_extract(flaggs, 4,100);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 ua5: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
		//# 4 * flags[19]
                n = 0;
                for (n;n<bit_extract(flaggs, 19,100);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 ua6: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
                n = 0;
                for (n;n<bit_extract(flaggs, 17,18);n++)
                {
                        brlyt_ua7_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_ua7_chunk));
                        printf("                 ua7 a: %08x\n", be32(data4.a));
			printf("                 ua7 b: %08x\n", be32(data4.b));
			printf("                 ua7 c: %f\n", float_swap_bytes(data4.c));
			printf("                 ua7 d: %08x\n", be32(data4.d));
			printf("                 ua7 e: %08x\n", be32(data4.e));
                        //pos += item_size;
                }
		//# 4 * flags[14-16]
                n = 0;
                for (n;n<bit_extract(flaggs, 14,16);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 ua8: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
		//# 0x10 * flags[9-13]
                n = 0;
                for (n;n<bit_extract(flaggs, 9,13);n++)
                {
                        brlyt_10b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_10b_chunk));
                        printf("                 ua8: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3], data4.unk[4], data4.unk[5], data4.unk[6], data4.unk[7], data4.unk[8], data4.unk[9], data4.unk[10], data4.unk[11], data4.unk[12], data4.unk[13], data4.unk[14], data4.unk[15]);
                        //pos += item_size;
                }
		//# 4 * flags[8], these are bytes btw
                n = 0;
                for (n;n<bit_extract(flaggs, 20,23);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 uaa: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
		//# 4 * flags[7]
                n = 0;
                for (n;n<bit_extract(flaggs, 20,23);n++)
                {
                        brlyt_4b_chunk data4;
                        //get_opt(chunk, pos, True, item_size, item_type);
                        BRLYT_ReadDataFromMemory(&data4, brlyt_file, sizeof(brlyt_4b_chunk));
                        printf("                 uab: %08x, %08x, %08x, %08x\n", data4.unk[0], data4.unk[1], data4.unk[2], data4.unk[3]);
                        //pos += item_size;
                }
//		if n < vars['num'] - 1
//		{
//			next_offset, = struct.unpack('>I', chunk[pos+4:pos+8])
//			if next_offset - 8 != mpos:
//				mat['~_insane'] = next_offset - 8 - mpos //# Extra shit we di        dn't parse :(
//		}
// 		mat['unk_bit_5'] = bit_extract(flags, 5)
//		mat['unk_bits_0_3'] = bit_extract(flags, 0, 3) //# Overwritten by stu        ff
//		vars['materials'].append(mat)
//		pos += 4

		BRLYT_fileoffset = tempDataLocation;		
	}
}

void PrintBRLYTEntry_gre1(brlyt_entry entry, u8* brlyt_file)
{
	//brlyt_numoffs_chunk data;
        //BRLYT_fileoffset = entry.data_location;
        //BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        //printf("                num: %08x\n", be16(data.num));
	//group end info
}

void PrintBRLYTEntry_grs1(brlyt_entry entry, u8* brlyt_file)
{
	//brlyt_numoffs_chunk data;
        //BRLYT_fileoffset = entry.data_location;
        //BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        //printf("                num: %08x\n", be16(data.num));
        //group start info
}

void PrintBRLYTEntry_pae1(brlyt_entry entry, u8* brlyt_file)
{
	//brlyt_numoffs_chunk data;
        //BRLYT_fileoffset = entry.data_location;
        //BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        //printf("                num: %08x\n", be16(data.num));
        //panel end info
}

void PrintBRLYTEntry_pas1(brlyt_entry entry, u8* brlyt_file)
{
	//brlyt_numoffs_chunk data;
        //BRLYT_fileoffset = entry.data_location;
        //BRLYT_ReadDataFromMemory(&data, brlyt_file, sizeof(brlyt_numoffs_chunk));
        printf("                Type: %c%c%c%c\n", entry.magic[0], entry.magic[1], entry.magic[2], entry.magic[3]);
        //printf("                num: %08x\n", be16(data.num));
        //panel start info
}

void PrintBRLYTEntries(brlyt_entry *entries, int entrycnt, u8* brlyt_file)
{
	dbgprintf("Printing BRLYT Entries");
	int i;
	
	for(i = 0; i < entrycnt; i++) {
		printf("\n	Index %d (@%08x):\n", i, entries[i].data_location - 8);
		if((FourCCsMatch(entries[i].magic, pan1_magic) == 1)) {
			dbgprintf("pan1\n");
			PrintBRLYTEntry_pan1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, txt1_magic) == 1)) {
			dbgprintf("txt1\n");
			PrintBRLYTEntry_txt1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, pic1_magic) == 1)) {
			dbgprintf("pic1\n");
			PrintBRLYTEntry_pic1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, wnd1_magic) == 1)) {
			dbgprintf("wnd1\n");
			PrintBRLYTEntry_wnd1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, bnd1_magic) == 1)) {
			dbgprintf("bnd1\n");
			PrintBRLYTEntry_bnd1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, lyt1_magic) == 1)) {
			dbgprintf("lyt1\n");
			PrintBRLYTEntry_lyt1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, grp1_magic) == 1)) {
			dbgprintf("grp1\n");
			PrintBRLYTEntry_grp1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, txl1_magic) == 1)) {
			dbgprintf("txl1\n");
			PrintBRLYTEntry_txl1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, fnl1_magic) == 1)) {
			dbgprintf("img\n");
			PrintBRLYTEntry_fnl1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, mat1_magic) == 1)) {
			dbgprintf("mat1\n");
			PrintBRLYTEntry_mat1(entries[i], brlyt_file);
		}else if((FourCCsMatch(entries[i].magic, gre1_magic) == 1)) {
			dbgprintf("gre1\n");
			PrintBRLYTEntry_gre1(entries[i], brlyt_file);
                }else if((FourCCsMatch(entries[i].magic, grs1_magic) == 1)) {
                        dbgprintf("grs1\n");
                        PrintBRLYTEntry_grs1(entries[i], brlyt_file);
                }else if((FourCCsMatch(entries[i].magic, pae1_magic) == 1)) {
                        dbgprintf("pae1\n");
                        PrintBRLYTEntry_pae1(entries[i], brlyt_file);
                }else if((FourCCsMatch(entries[i].magic, pas1_magic) == 1)) {
                        dbgprintf("pas1\n");
                        PrintBRLYTEntry_pas1(entries[i], brlyt_file);
		}else
			printf("		Unknown tag (%c%c%c%c)!\n",entries[i].magic[0],entries[i].magic[1],entries[i].magic[2],entries[i].magic[3]);
	}
}

void parse_brlyt(char *filename)
{
	materials = (u8*)malloc(12);
	numberOfMaterials = 0;
	lengthOfMaterials = 0;
	FILE* fp = fopen(filename, "rb");
	if(fp == NULL) {
		printf("Error! Couldn't open %s!\n", filename);
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	dbgprintf("Filesize is %d\n", file_size);
	u8* brlyt_file = (u8*)malloc(file_size);
	dbgprintf("brlyt_file allocated\n");
	fseek(fp, 0, SEEK_SET);
	fread(brlyt_file, file_size, 1, fp);
	dbgprintf("brlyt_file read to.\n");
	BRLYT_fileoffset = 0;
	brlyt_header header;
	BRLYT_ReadDataFromMemory(&header, brlyt_file, sizeof(brlyt_header));
	BRLYT_CheckHeaderSanity(header, file_size);
	brlyt_entry *entries;
	BRLYT_fileoffset = be16(header.lyt_offset);
	brlyt_entry_header tempentry;
	int i;
	dbgprintf("curr %08x max %08x\n", BRLYT_fileoffset, file_size);
	for(i = 0; BRLYT_fileoffset < file_size; i++) {
		BRLYT_ReadDataFromMemoryX(&tempentry, brlyt_file, sizeof(brlyt_entry_header));
		BRLYT_fileoffset += be32(tempentry.length);
		dbgprintf("curr %08x max %08x\n", BRLYT_fileoffset, file_size);
	}
	int entrycount = i;
	entries = (brlyt_entry*)calloc(entrycount, sizeof(brlyt_entry));
	dbgprintf("%08x\n", entries);
	if(entries == NULL) {
		printf("Couldn't allocate for entries!\n");
		exit(1);
	}
	BRLYT_fileoffset = be16(header.lyt_offset);
	for(i = 0; i < entrycount; i++) {
		dbgprintf("&(entries[i]) = %08x\n", &(entries[i]));
		BRLYT_ReadDataFromMemoryX(&tempentry, brlyt_file, sizeof(brlyt_entry_header));
		memcpy(entries[i].magic, tempentry.magic, 4);
		entries[i].length = tempentry.length;
		entries[i].data_location = BRLYT_fileoffset + sizeof(brlyt_entry_header);
		BRLYT_fileoffset += be32(tempentry.length);
	}	
//	int entrycnt = BRLYT_ReadEntries(brlyt_file, file_size, header, entries);
	dbgprintf("%08x\n", entries);
	printf("Parsed BRLYT! Information:\n");
	printf("Main header:\n");
	printf("	Magic: %c%c%c%c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
	printf("	Unk1: %08x\n", be32(header.unk1));
	printf("	Filesize: %lu\n", be32(header.filesize));
	printf("		%s real file size!\n", be32(header.filesize) == file_size ? "Matches" : "Does not match");
	printf("	Offset to lyt1: %04x\n", be16(header.lyt_offset));
	printf("	Unk2: %04x\n", be16(header.unk2));
	printf("\nBRLYT Entries:");
	PrintBRLYTEntries(entries, entrycount, brlyt_file);
}

void make_brlyt(char* infile, char* outfile)
{
}



