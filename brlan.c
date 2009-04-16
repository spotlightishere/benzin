/*
 *  brlan.c
 *  
 *
 *  Created by Alex Marshall on 09/01/27.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mxml.h>

#include "general.h"
#include "memfile.h"
#include "types.h"
#include "brlan.h"
#include "xml.h"

#if DEBUGBRLAN == 1
#define dbgprintf	printf
#else
#define dbgprintf	//
#endif //DEBUGBRLAN

#define MAXIMUM_TAGS_SIZE		(0x1000)
#define MAXIMUM_TIMGS_SIZE		(0x1000)

fourcc tag_FourCCs[] = { "RLPA", "RLTS", "RLVI", "RLVC", "RLMC", "RLTP" };

char tag_types_list[15][24];

static size_t BRLAN_fileoffset = 0;
FILE* xmlanout;

u8 somethingIsSet = 0;

static void BRLAN_ReadDataFromMemoryX(void* destination, void* input, size_t size)
{
	u8* out = (u8*)destination;
	u8* in = ((u8*)input) + BRLAN_fileoffset;
	memcpy(out, in, size);
}

static void BRLAN_ReadDataFromMemory(void* destination, void* input, size_t size)
{
	BRLAN_ReadDataFromMemoryX(destination, input, size);
	BRLAN_fileoffset += size;
}

static void CreateGlobal_pai1(brlan_pai1_header_type2 *pai1_header, brlan_pai1_header_type1 pai1_header1,
			      brlan_pai1_header_type2 pai1_header2, int pai1_header_type)
{
	if(pai1_header_type == 1) {
		pai1_header->magic[0]		= pai1_header1.magic[0];
		pai1_header->magic[1]		= pai1_header1.magic[1];
		pai1_header->magic[2]		= pai1_header1.magic[2];
		pai1_header->magic[3]		= pai1_header1.magic[3];
		pai1_header->size		= pai1_header1.size;
		pai1_header->framesize		= pai1_header1.framesize;
		pai1_header->flags		= pai1_header1.flags;
		pai1_header->unk1		= pai1_header1.unk1;
		pai1_header->num_timgs		= pai1_header1.num_timgs;
		pai1_header->num_entries	= pai1_header1.num_entries;
		pai1_header->unk2		= 0;
		pai1_header->entry_offset	= pai1_header1.entry_offset;
	}else
		memcpy(pai1_header, &pai1_header2, sizeof(brlan_pai1_header_type2));
}

static void DisplayTagData(tag_data data, int z)
{
	u32 p1 = be32(data.part1);
	u32 p2 = be32(data.part2);
	u32 p3 = be32(data.part3);
#ifndef OLD_BRLAN_OUTSTYLE
	printf("\t\t\t<triplet>\n\t\t\t\t<frame>%.6f</frame>\n\t\t\t\t<value>%.6f</value>\n\t\t\t\t<blend>%.6f</blend>\n\t\t\t</triplet>\n", *(f32*)(&p1), *(f32*)(&p2), *(f32*)(&p3));
#endif // OLD_BRLAN_OUTSTYLE
#ifdef OLD_BRLAN_OUTSTYLE
	printf("					Triplet %d:\n", z);
	printf("						Frame number: %f\n", *(f32*)(&p1));
	printf("						Value: %f\n", *(f32*)(&p2));
	printf("						Interpolation Value: %f\n", *(f32*)(&p3));
#endif // OLD_BRLAN_OUTSTYLE
}

static int FourCCsMatch(fourcc cc1, fourcc cc2)
{
	if((cc1[0] == cc2[0]) && (cc1[1] == cc2[1]) && (cc1[2] == cc2[2]) && (cc1[3] == cc2[3]))
		return 1;
	else
		return 0;
}

static int FourCCInList(fourcc cc)
{
	int i;
	for(i = 0; i < 6; i++)
		if(FourCCsMatch(cc, tag_FourCCs[i])) return 1;
	return 0;
}

static void DisplayTagInformation(int idx, tag_header* heads, tag_entry** entriesx,
			    tag_entryinfo** entryinfosx, tag_data*** datasxx)
{
	tag_header head = heads[idx];
	tag_entry* entries = entriesx[idx];
	tag_entryinfo* entryinfos = entryinfosx[idx];
	tag_data** datas = datasxx[idx];
	int i, z;
	
#ifdef OLD_BRLAN_OUTSTYLE
	printf("		Number of entries: %u\n", head.entry_count);

// Why should we show padding? It's 0 every single time.
/*	printf("		Unk1: %02x\n", head.pad1);
	printf("		Unk2: %02x\n", head.pad2);
	printf("		Unk3: %02x\n", head.pad3); */

	printf("		Entries:\n");
#endif // OLD_BRLAN_OUTSTYLE
	for(i = 0; i < head.entry_count; i++) {
#ifndef OLD_BRLAN_OUTSTYLE
		if(short_swap_bytes(entryinfos[i].type) < 16)
			printf("\t\t<entry type=\"%s\">\n", tag_types_list[short_swap_bytes(entryinfos[i].type)]);
		else
			printf("\t\t<entry type=\"%u\">\n", short_swap_bytes(entryinfos[i].type));
#endif // OLD_BRLAN_OUTSTYLE
#ifdef OLD_BRLAN_OUTSTYLE
		printf("			Entry %u:\n", i);
		if(short_swap_bytes(entryinfos[i].type) < 16)
			printf("				Type: %s (%04x)\n", tag_types_list[short_swap_bytes(entryinfos[i].type)], short_swap_bytes(entryinfos[i].type));
		else
			printf("				Type: Unknown (%04x)\n", short_swap_bytes(entryinfos[i].type));
// User doesn't need to know the offset
//		printf("				Offset: %lu\n", be32(entries[i].offset));

// Yet again, these are always the same, and seem to maybe be some marker. don't bother to show.
/*		printf("				Unk1: %04x\n", short_swap_bytes(entryinfos[i].pad1));
		printf("				Unk2: %04x\n", short_swap_bytes(entryinfos[i].unk1));
		printf("				Unk3: %08x\n", be32(entryinfos[i].unk2)); */
		printf("				Triplet Count: %u\n", short_swap_bytes(entryinfos[i].coord_count));
		printf("				Triplets:\n");
#endif // OLD_BRLAN_OUTSTYLE
		for(z = 0; z < short_swap_bytes(entryinfos[i].coord_count); z++)
			DisplayTagData(datas[i][z], z);
#ifndef OLD_BRLAN_OUTSTYLE
		printf("\t\t</entry>\n");
#endif // OLD_BRLAN_OUTSTYLE
	}
}

static void ReadTagFromBRLAN(int idx, u8* brlan_file, tag_header *head, tag_entry** entries, tag_entryinfo** entryinfo, tag_data*** data)
{
	int taghead_location = BRLAN_fileoffset;
	BRLAN_ReadDataFromMemory(&head[idx], brlan_file, sizeof(tag_header));
	int i, z;
	entries[idx] = realloc(entries[idx], sizeof(tag_entry) * head[idx].entry_count);
	dbgprintf("reallocated entries[idx]\n");
	entryinfo[idx] = realloc(entryinfo[idx], sizeof(tag_entryinfo) * head[idx].entry_count);
	dbgprintf("reallocated entryinfo[idx]\n");
	if(data[idx] == NULL) {
		dbgprintf("callocating data[idx]\n");
		data[idx] = (tag_data**)malloc(sizeof(tag_data) * head[idx].entry_count);
		dbgprintf("callocated data[idx]\n");
	}else{
		dbgprintf("reallocating data[idx] %08x\n", data[idx]);
		data[idx] = (tag_data**)realloc(data[idx], sizeof(tag_data) * head[idx].entry_count);
		dbgprintf("reallocated data[idx]\n");
	}
	for(i = 0; i < head[idx].entry_count; i++) {
		dbgprintf("reading entry %d 0x%08x\n", i, &entries[idx][i]);
		BRLAN_ReadDataFromMemory(&entries[idx][i], brlan_file, sizeof(tag_entry));
		dbgprintf("read entry.\n");
	}
	for(i = 0; i < head[idx].entry_count; i++) {
		dbgprintf("i %d cnt %d\n", i, head[idx].entry_count);
		BRLAN_fileoffset = be32(entries[idx][i].offset) + taghead_location;
		dbgprintf("read entry infos. 0x%08x\n", &entryinfo[idx][i]);
		BRLAN_ReadDataFromMemory(&entryinfo[idx][i], brlan_file, sizeof(tag_entryinfo));
		dbgprintf("read entry info 0x%08x. %u\n", &data[idx][i][0], short_swap_bytes(entryinfo[idx][i].coord_count));
		data[idx][i] = realloc(NULL, sizeof(tag_data) * short_swap_bytes(entryinfo[idx][i].coord_count));
		dbgprintf("reallocated data[i].\n");
		for(z = 0; z < short_swap_bytes(entryinfo[idx][i].coord_count); z++) {
			dbgprintf("reading data 0x%08x\n", &data[idx][i][z]);
			BRLAN_ReadDataFromMemory(&data[idx][i][z], brlan_file, sizeof(tag_data));
			dbgprintf("read data.\n");
		}
	}
}

void parse_brlan(char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if(fp == NULL) {
		printf("Error! Couldn't open %s!\n", filename);
		exit(1);
	}
#ifndef OLD_BRLAN_OUTSTYLE
	xmlanout = fopen("testout.xmlan", "wb+");
#endif // OLD_BRLAN_OUTSTYLE
	int i;
	for(i = 0; i < 16; i++)
		memset(tag_types_list[i], 0, 24);
	strcpy(tag_types_list[0], "X Translation");
	strcpy(tag_types_list[1], "Y Translation");
	strcpy(tag_types_list[2], "Z Translation");
	strcpy(tag_types_list[3], "0x03");
	strcpy(tag_types_list[4], "0x04");
	strcpy(tag_types_list[5], "Angle");
	strcpy(tag_types_list[6], "X Zoom");
	strcpy(tag_types_list[7], "Y Zoom");
	strcpy(tag_types_list[8], "Width");
	strcpy(tag_types_list[9], "Height");
	strcpy(tag_types_list[10], "0x0A");
	strcpy(tag_types_list[11], "0x0B");
	strcpy(tag_types_list[12], "0x0C");
	strcpy(tag_types_list[13], "0x0D");
	strcpy(tag_types_list[14], "0x0E");
	strcpy(tag_types_list[15], "0x0F");
	fseek(fp, 0, SEEK_END);
	int file_size = ftell(fp);
	dbgprintf("Filesize is %d\n", file_size);
	u8* brlan_file = (u8*)malloc(file_size);
	dbgprintf("brlan_file allocated\n");
	fseek(fp, 0, SEEK_SET);
	fread(brlan_file, file_size, 1, fp);
	dbgprintf("brlan_file read to.\n");
	BRLAN_fileoffset = 0;
	brlan_header header;
	BRLAN_ReadDataFromMemoryX(&header, brlan_file, sizeof(brlan_header));
	dbgprintf("brlan_header read to.\n");
	BRLAN_fileoffset = short_swap_bytes(header.pai1_offset);
	brlan_pai1_universal universal;
	BRLAN_ReadDataFromMemoryX(&universal, brlan_file, sizeof(brlan_pai1_universal));
	dbgprintf("pa1_universal read to.\n");
	
	int pai1_header_type;
	brlan_pai1_header_type1 pai1_header1;
	brlan_pai1_header_type2 pai1_header2;
	brlan_pai1_header_type2 pai1_header;

	if((be32(universal.flags) & (1 << 25)) >= 1) {
		pai1_header_type = 2;
		somethingIsSet = 1;
		BRLAN_ReadDataFromMemory(&pai1_header2, brlan_file, sizeof(brlan_pai1_header_type2));
	} else {
		pai1_header_type = 1;
		BRLAN_ReadDataFromMemory(&pai1_header1, brlan_file, sizeof(brlan_pai1_header_type1));
	}
	dbgprintf("pai1 headers gotten.\n");
	
	CreateGlobal_pai1(&pai1_header, pai1_header1, pai1_header2, pai1_header_type);
	dbgprintf("pai1 global created.\n");
	
	int tagcount = short_swap_bytes(pai1_header.num_entries);
//	int tagcount = short_swap_bytes(pai1_header.num_entries);
	dbgprintf("tagcount done.\n");
	u32 *taglocations = (u32*)calloc(tagcount, sizeof(u32));
	dbgprintf("allocated tag locations.\n");
	fourcc CCs[256];
	memset(CCs, 0, 256*4);
	BRLAN_fileoffset = be32(pai1_header.entry_offset) + short_swap_bytes(header.pai1_offset);
//	BRLAN_fileoffset = be32(pai1_header.entry_offset) + short_swap_bytes(header.pai1_offset);
	dbgprintf("allocated fourccs.\n");
	BRLAN_ReadDataFromMemory(taglocations, brlan_file, tagcount * sizeof(u32));
	dbgprintf("read tag locations.\n");
	brlan_entry *tag_entries = (brlan_entry*)calloc(tagcount, sizeof(brlan_entry));
	dbgprintf("tag entries allocated.\n");
	tag_header *intag_heads = NULL;
	dbgprintf("tag heads created.\n");
	tag_entry **intag_entries = NULL;
	dbgprintf("tag entries created.\n");
	tag_entryinfo **intag_entryinfos = NULL;
	dbgprintf("rlpa entry infos created.\n");
	tag_data ***intag_datas = NULL;
	dbgprintf("rlpa datas created.\n");
	int intag_cnt = 1;
	dbgprintf("Number of tagcount: %d\n", tagcount);
	for(i = 0; i < tagcount; i++) {
		BRLAN_fileoffset = be32(taglocations[i]) + short_swap_bytes(header.pai1_offset);
		dbgprintf("fileoffset set.\n");
		BRLAN_ReadDataFromMemory(&(tag_entries[i]), brlan_file, sizeof(brlan_entry));
		dbgprintf("got tag entry.\n");
		if((be32(tag_entries[i].flags) & (1 << 25)) >= 1) {
			somethingIsSet = 1;
			BRLAN_fileoffset += sizeof(u32);
			dbgprintf("skipped extra.\n");
		}
		fourcc magick;
		BRLAN_ReadDataFromMemoryX(magick, brlan_file, 4);
		memcpy(CCs[i], magick, 4);
		dbgprintf("read fourcc from %04x %02x%02x%02x%02x.\n", BRLAN_fileoffset, magick[0], magick[1], magick[2], magick[3]);
		if(FourCCInList(CCs[i]) == 1) {
			dbgprintf("we found a tag.\n");
			intag_heads = realloc(intag_heads, sizeof(tag_header) * intag_cnt);
			dbgprintf("reallocated tag heads.\n");
			intag_entries = realloc(intag_entries, sizeof(tag_entry) * intag_cnt * 20);
			dbgprintf("reallocated tag entries.\n");
			intag_entryinfos = realloc(intag_entryinfos, sizeof(tag_entryinfo) * intag_cnt * 20);
			dbgprintf("reallocated tag entryinfos.\n");
			intag_datas = realloc(intag_datas, sizeof(tag_data) * intag_cnt * 20 * 6);
			dbgprintf("reallocated tag datas.\n");
			if(intag_heads == NULL) {
				printf("Error allocating heads.\n");
				exit(2);
			}
			if(intag_entries == NULL) {
				printf("Error allocating entries.\n");
				exit(2);
			}
			if(intag_entryinfos == NULL) {
				printf("Error allocating entryinfos.\n");
				exit(2);
			}
			if(intag_datas == NULL) {
				printf("Error allocating datas.\n");
				exit(2);
			}
			ReadTagFromBRLAN(intag_cnt - 1, brlan_file, intag_heads, intag_entries,
					  intag_entryinfos, intag_datas);
			intag_cnt++;
		}
		dbgprintf("looping.\n");
	}
	
#ifndef OLD_BRLAN_OUTSTYLE
	printf("<?xml version=\"1.0\"?>\n" \
	       "<xmlan framesize=\"%lu\" flags=\"%02x\">\n", short_swap_bytes(pai1_header.framesize), pai1_header.flags);
#endif // OLD_BRLAN_OUTSTYLE
	int timgs = short_swap_bytes(pai1_header.num_timgs);
	printf("Number of TPL files: %d\n", timgs);
	intag_cnt = 0;
	int oldoffset = BRLAN_fileoffset;
	BRLAN_fileoffset = short_swap_bytes(header.pai1_offset) + sizeof(brlan_pai1_header_type1);
	dbgprintf("fileoffset %08x\n", BRLAN_fileoffset);
	int tableoff = BRLAN_fileoffset;
	int currtableoff = BRLAN_fileoffset;
#ifdef OLD_BRLAN_OUTSTYLE
	printf("\nTIMG entries:");
	fflush(stdout);
#endif //OLD_BRLAN_OUTSTYLE
	for(i = 0; i < timgs; i++) {
		u32 curr_timg_off = 0;
		BRLAN_ReadDataFromMemory(&curr_timg_off, brlan_file, 4);
		dbgprintf("currtimgoff %08x\n", be32(curr_timg_off));
		char timgname[256];
		memset(timgname, 0, 256);
		int z = tableoff + be32(curr_timg_off);
		dbgprintf("z %08x\n", z);
		int o;
		for(o = 0; brlan_file[z] != 0; timgname[o++] = brlan_file[z], z++);
#ifndef OLD_BRLAN_OUTSTYLE
		printf("\t<timg name=\"%s\" />\n", timgname);
#else
		printf("	Image name: %s\n", timgname);
#endif //OLD_BRLAN_OUTSTYLE
		currtableoff += 4;
	}
	BRLAN_fileoffset = oldoffset;
#ifdef OLD_BRLAN_OUTSTYLE
	printf("Parsed BRLAN! Information:\n");
	printf("Main header:\n");
	printf("	Magic: %c%c%c%c\n", header.magic[0], header.magic[1], header.magic[2], header.magic[3]);
	printf("	Filesize: %lu\n", be32(header.file_size));
	printf("		%s real file size!\n", be32(header.file_size) == file_size ? "Matches" : "Does not match");
		       
	// Not important to user, why bother.
/*	printf("	pai1 Offset: %04x\n", short_swap_bytes(header.pai1_offset));
	printf("	pai1 Count: %04x\n", short_swap_bytes(header.pai1_count)); */
	printf("\npai1 header:\n");
	printf("	Type: %d\n", pai1_header_type);
	printf("	Magic: %c%c%c%c\n", pai1_header.magic[0], pai1_header.magic[1], pai1_header.magic[2], pai1_header.magic[3]);
	printf("	Size: %lu\n", be32(pai1_header.size));
	printf("	Framesize: %u\n", short_swap_bytes(pai1_header.framesize));
	printf("	Flags: %02x\n", pai1_header.flags);
	printf("	unk1: %02x\n", pai1_header.unk1);
	printf("	Number of Textures: %u\n", short_swap_bytes(pai1_header.num_timgs));
	printf("	Number of Entries: %u\n", short_swap_bytes(pai1_header.num_entries));
	printf("	unk2: %08lx\n", be32(pai1_header.unk2));
		       
	// Not important to user, why bother.
//	printf("	Offset to Entries: 0x%08lx\n", be32(pai1_header.entry_offset));
		       
	printf("\nBRLAN entries:");
#endif // OLD_BRLAN_OUTSTYLE
	for(i = 0; i < tagcount; i++) {
#ifndef OLD_BRLAN_OUTSTYLE
		printf("\t<tag ");
		if(strlen(tag_entries[i].name) > 0)
			printf("name=\"%s\" ", tag_entries[i].name);
		printf("type=\"%c%c%c%c\" ", CCs[i][0], CCs[i][1], CCs[i][2], CCs[i][3]);
		if(be32(tag_entries[i].flags) == 0x01000000)
		       printf("format=\"%s\">\n", "Normal");
		else
		{
			somethingIsSet = 1;
			printf("format=\"%08x\">\n", be32(tag_entries[i].flags));
		}
#endif // OLD_BRLAN_OUTSTYLE

#ifdef OLD_BRLAN_OUTSTYLE
		printf("\n	Entry %u:\n", i);
		printf("		Name: %s\n", tag_entries[i].name);
		if(be32(tag_entries[i].flags) == 0x01000000)
		       printf("		Type: %s\n", "Normal");
		else
		       printf("		Type: %08x\n", be32(tag_entries[i].flags));

// Not important to user, why bother.
//		printf("		Animation Header Length: %lu\n", be32(tag_entries[i].anim_header_len));		
		printf("		FourCC: %c%c%c%c\n", CCs[i][0], CCs[i][1], CCs[i][2], CCs[i][3]);
#endif // OLD_BRLAN_OUTSTYLE
		if(FourCCInList(CCs[i]) == 1) {
			DisplayTagInformation(intag_cnt, intag_heads, intag_entries, intag_entryinfos, intag_datas);
			intag_cnt++;
		}else{
#ifdef OLD_BRLAN_OUTSTYLE
			printf("		Sorry, this type is currently unknown.\n");
#endif // OLD_BRLAN_OUTSTYLE
		}
#ifndef OLD_BRLAN_OUTSTYLE
		printf("\t</tag>\n");
#endif // OLD_BRLAN_OUTSTYLE
	}
	if (somethingIsSet)
	{
		tag_header extraTagHeader;
		tag_entry extraTagEntry;
		tag_entryinfo extraTagEntryInfo;
		tag_data extraTagData;
		//read brlan_tag_header
		BRLAN_ReadDataFromMemory(&extraTagHeader, brlan_file, sizeof(tag_header));
		//read brlan_tag_entry
		BRLAN_ReadDataFromMemory(&extraTagEntry, brlan_file, sizeof(tag_entry));
		//read brlan_tag_entryinfo
		BRLAN_ReadDataFromMemory(&extraTagEntryInfo, brlan_file, sizeof(tag_entryinfo));
		u32 p1 = be32(extraTagData.part1);
		u32 p2 = be32(extraTagData.part2);
		u32 p3 = be32(extraTagData.part3);
		printf("\t<extratag type=\"%c%c%c%c\">\n", extraTagHeader.magic[0], extraTagHeader.magic[1], extraTagHeader.magic[2], extraTagHeader.magic[3]);
		printf("\t\t<entry>\n");
		int q;for(q=0;q<short_swap_bytes(extraTagEntryInfo.coord_count);q++)
		{
			BRLAN_ReadDataFromMemory(&extraTagData, brlan_file, sizeof(tag_data));
			u32 p1 = be32(extraTagData.part1);
			u32 p2 = be32(extraTagData.part2);
			u32 p3 = be32(extraTagData.part3);

			printf("\t\t\t<triplet>\n");
			printf("\t\t\t\t<frame>%.6f</frame>\n", *(f32*)&p1);
			printf("\t\t\t\t<value>%.6f</value>\n", *(f32*)&p2);
			printf("\t\t\t\t<blend>%.6f</blend>\n", *(f32*)&p3);
			printf("\t\t\t</triplet>\n");
		}
		printf("\t\t</entry>\n");
		printf("\t</extratag>\n");
		//free(extraBytesOne);
		//free(extraBytesTwo);
	}
#ifndef OLD_BRLAN_OUTSTYLE
	printf("</xmlan>\n");
#endif // OLD_BRLAN_OUTSTYLE
	free(brlan_file);
	fclose(fp);
}

void WriteBRLANTagHeader(tag_header* head, FILE* fp)
{
	fwrite(head, sizeof(tag_header), 1, fp);
}

void WriteBRLANTagEntries(tag_entry* entry, u8 count, FILE* fp)
{
	tag_entry writeentry;
	int i;
	for(i = 0; i < count; i++) {
		writeentry.offset = be32(entry[i].offset);
		fwrite(&writeentry, sizeof(tag_entry), 1, fp);
	}
}

void WriteBRLANTagEntryinfos(tag_entryinfo entryinfo, FILE* fp)
{
	tag_entryinfo writeentryinfo;
	writeentryinfo.type = short_swap_bytes(entryinfo.type);
	writeentryinfo.unk1 = short_swap_bytes(entryinfo.unk1);
	writeentryinfo.coord_count = short_swap_bytes(entryinfo.coord_count);
	writeentryinfo.pad1 = short_swap_bytes(entryinfo.pad1);
	writeentryinfo.unk2 = be32(entryinfo.unk2);
	fwrite(&writeentryinfo, sizeof(tag_entryinfo), 1, fp);
}

void WriteBRLANTagData(tag_data* data, u16 count, FILE* fp)
{
	tag_data writedata;
	int i;
	for(i = 0; i < count; i++) {
		writedata.part1 = be32(data[i].part1);
		writedata.part2 = be32(data[i].part2);
		writedata.part3 = be32(data[i].part3);
		fwrite(&writedata, sizeof(tag_data), 1, fp);
	}
}

void WriteBRLANEntry(brlan_entry *entr, FILE* fp)
{
	brlan_entry writeentr;
	memset(writeentr.name, 0, 20);
	strncpy(writeentr.name, entr->name, 20);
	writeentr.flags = be32(entr->flags);
	writeentr.anim_header_len = be32(entr->anim_header_len);
	fwrite(&writeentr, sizeof(brlan_entry), 1, fp);
}

u32 create_entries_from_xml(mxml_node_t *tree, mxml_node_t *node, brlan_entry *entr, tag_header* head, u8** tagblob, u32* blobsize)
{
	tag_entry* entry = NULL;
	tag_entryinfo* entryinfo = NULL;
	tag_data** data = NULL;
	mxml_node_t *tempnode = NULL;
	mxml_node_t *subnode = NULL;
	mxml_node_t *subsubnode = NULL;
	char temp[256];
	char temp2[256];
	char temp3[15][24];
	int i, x;

	for(i = 0; i < 16; i++)
		memset(temp3[i], 0, 24);
	for(x = 0; x < 16; x++)
		for(i = 0; i < strlen(tag_types_list[x]); i++)
			temp3[x][i] = toupper(tag_types_list[x][i]);
	head->entry_count = 0;
	subnode = node;
	for (x = 0, subnode = mxmlFindElement(subnode, node, "entry", NULL, NULL, MXML_DESCEND); subnode != NULL; subnode = mxmlFindElement(subnode, node, "entry", NULL, NULL, MXML_DESCEND), x++) {
		head->entry_count++;
		entry = realloc(entry, sizeof(tag_entry) * head->entry_count);
		entryinfo = realloc(entryinfo, sizeof(tag_entryinfo) * head->entry_count);
		if(data == NULL)
			data = (tag_data**)malloc(sizeof(tag_data*) * head->entry_count);
		else
			data = (tag_data**)realloc(data, sizeof(tag_data*) * head->entry_count);
		data[x] = NULL;
		memset(temp, 0, 256);
		memset(temp2, 0, 256);
 		if(mxmlElementGetAttr(subnode, "type") != NULL)
			strcpy(temp, mxmlElementGetAttr(subnode, "type"));
		else{
			printf("No type attribute found!\nSkipping this entry!\n");
			head->entry_count--;
			continue;
		}
		for(i = 0; i < strlen(temp); i++)
			temp2[i] = toupper(temp[i]);
		for(i = 0; (i < 16) && (strcmp(temp3[i - 1], temp2) != 0); i++);
		if(i == 16)
			i = atoi(temp2);
		else
			i--;
		entry[x].offset = 0;
		entryinfo[x].type = i;
		entryinfo[x].unk1 = 0x0200;
		entryinfo[x].pad1 = 0x0000;
		entryinfo[x].unk2 = 0x0000000C;
		entryinfo[x].coord_count = 0;
		subsubnode = subnode;
		for (i = 0, subsubnode = mxmlFindElement(subsubnode, subnode, "triplet", NULL, NULL, MXML_DESCEND); subsubnode != NULL; subsubnode = mxmlFindElement(subsubnode, subnode, "triplet", NULL, NULL, MXML_DESCEND), i++) {
			entryinfo[x].coord_count++;
			data[x] = realloc(data[x], sizeof(tag_data) * entryinfo[x].coord_count);
			tempnode = mxmlFindElement(subsubnode, subsubnode, "frame", NULL, NULL, MXML_DESCEND);
			if(tempnode == NULL) {
				printf("Couldn't find attribute \"frame\"!\n");
				exit(1);
			}
			get_value(tempnode, temp, 256);
			*(f32*)(&(data[x][i].part1)) = atof(temp);
			tempnode = mxmlFindElement(subsubnode, subsubnode, "value", NULL, NULL, MXML_DESCEND);
			if(tempnode == NULL) {
				printf("Couldn't find attribute \"value\"!\n");
				exit(1);
			}
			get_value(tempnode, temp, 256);
			*(f32*)(&(data[x][i].part2)) = atof(temp);
			tempnode = mxmlFindElement(subsubnode, subsubnode, "blend", NULL, NULL, MXML_DESCEND);
			if(tempnode == NULL) {
				printf("Couldn't find attribute \"blend\"!\n");
				exit(1);
			}
			get_value(tempnode, temp, 256);
			*(f32*)(&(data[x][i].part3)) = atof(temp);
		}
	}
	FILE* fp = fopen("temp.blan", "wb+");
	if(fp == NULL) {
		printf("Couldn't open temporary temp.blan file\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_SET);
	entr->anim_header_len = 0;
	WriteBRLANEntry(entr, fp);
	u32 fillerIntOffset = ftell(fp);
	u32 fillerInt = be32(0x60);
	if (entr->flags & 0x02000000)
	{
		fillerInt = be32(0x60);	// sizeof(wholeFile) - 
		fwrite(&fillerInt, sizeof(u32), 1, fp);
		BRLAN_fileoffset += 4;
	}
	WriteBRLANTagHeader(head, fp);
	u32 entryloc = ftell(fp);
	WriteBRLANTagEntries(entry, head->entry_count, fp);
	u32* entryinfolocs = (u32*)calloc(head->entry_count, sizeof(u32));
	for(x = 0; x < head->entry_count; x++) {
		entryinfolocs[x] = ftell(fp);
		entry[x].offset = entryinfolocs[x] - sizeof(brlan_entry);
		WriteBRLANTagEntryinfos(entryinfo[x], fp);
		WriteBRLANTagData(data[x], entryinfo[x].coord_count, fp);
	}
	u32 oldpos = ftell(fp);
	fseek(fp, entryloc, SEEK_SET);
	for(x=0;x<head->entry_count;x++)
		entry[x].offset -= sizeof(u32) * (head->entry_count - 1);
	WriteBRLANTagEntries(entry, head->entry_count, fp);
	fseek(fp, oldpos, SEEK_SET);
	u32 filesz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char tagRLPA[4] = {'R','L','P','A' };
	char tagRLMC[4] = {'R','L','M','C' };
	char tagRLVC[4] = {'R','L','V','C' };
	char tagRLVI[4] = {'R','L','V','I' };
	if (memcmp(&head->magic, tagRLPA, 4) == 0) 
	{
		entr->anim_header_len = sizeof(brlan_entry);
	} else if ((memcmp(&head->magic, tagRLMC, 4) == 0) || (memcmp(&head->magic, tagRLVC, 4) == 0)) {
		entr->anim_header_len = (head->entry_count * (sizeof(tag_entry) + sizeof(tag_data) + sizeof(tag_entryinfo)));
	} else {
		entr->anim_header_len = sizeof(tag_header) + (sizeof(tag_entry) * head->entry_count);
	}
	if (entr->flags & 0x02000000)
	{
		entr->anim_header_len += 4;
		u32 tempOffset = ftell(fp);
		fillerInt = be32(filesz);
		fseek(fp, fillerIntOffset, SEEK_SET);
		fwrite(&fillerInt, sizeof(u32), 1, fp);
		BRLAN_fileoffset += 4;
		fseek(fp, tempOffset, SEEK_SET);
	}
	WriteBRLANEntry(entr, fp);

	// write the final tag if flag & 0x02000000
	 mxml_node_t *addonnode = NULL;
	 mxml_node_t *addonsubnode = NULL;
	 mxml_node_t *addonsubsubnode = NULL;
	 tag_header addonTagHeader;
	 tag_entry addonTagEntry;
	 tag_entryinfo addonTagEntryInfo;
	 tag_data *addonTagData;
//	tag_data addonTagData;
	 char tempChar[256];
	 addonnode = mxmlFindElement(tree, tree, "extratag", NULL, NULL, MXML_DESCEND);
	 if (addonnode != NULL)
	 {
		printf("in the box\n");
		if(mxmlElementGetAttr(addonnode, "type") != NULL)
			strcpy(temp, mxmlElementGetAttr(addonnode, "type"));
		else{
			printf("No type attribute found!\nSkipping this entry!\n");
		}
		printf("addon magic: %c%c%c%c\n", temp[0], temp[1], temp[2], temp[3]);
		//addonTagHeader.magic = temp;
		memcpy(addonTagHeader.magic, temp, 4);
		printf("addon tagheader magic: %s\n", addonTagHeader.magic);
		addonTagHeader.entry_count = 0x01;
		addonTagHeader.pad1 = 0x0;
		addonTagHeader.pad2 = 0x0;
		addonTagHeader.pad3 = 0x0;
		addonTagEntry.offset = be32(0x0c);
		addonTagEntryInfo.type = short_swap_bytes(0x10);
		addonTagEntryInfo.unk1 = short_swap_bytes(0x0200);
		addonTagEntryInfo.pad1 = 0x0000;
		addonTagEntryInfo.unk2 = be32(0x0000000C);
		addonTagEntryInfo.coord_count = 0x0;
	
		for(addonsubnode = mxmlFindElement(addonnode, addonnode, "triplet", NULL, NULL, MXML_DESCEND); addonsubnode != NULL; addonsubnode = mxmlFindElement(addonsubnode, addonnode, "triplet", NULL, NULL, MXML_DESCEND))
		{
			addonTagEntryInfo.coord_count +=1;
			u8 tC = addonTagEntryInfo.coord_count;
			addonTagData = realloc(addonTagData, sizeof(tag_data) * tC);
			addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "frame", NULL, NULL, MXML_DESCEND);
			if (addonsubsubnode != NULL)
			{
				get_value(addonsubsubnode, tempChar, 256);
				f32 f1 = atof(tempChar);
				addonTagData[tC-1].part1 = *(u32*)&f1;
				addonTagData[tC-1].part1 = int_swap_bytes(addonTagData[tC-1].part1);

//				addonTagData.part1 = *(u32*)&f1;
//				addonTagData.part1 = int_swap_bytes(addonTagData.part1);
			}
	
			addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "value", NULL, NULL, MXML_DESCEND);
			if (addonsubsubnode != NULL)
			{
				get_value(addonsubsubnode, tempChar, 256);
				f32 f2 = atof(tempChar);
				addonTagData[tC-1].part2 = *(u32*)&f2;
				addonTagData[tC-1].part2 = int_swap_bytes(addonTagData[tC-1].part2);

//				addonTagData.part2 = *(u32*)&f2;
//				addonTagData.part2 = int_swap_bytes(addonTagData.part2);
			}
	
			addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "blend", NULL, NULL, MXML_DESCEND);
			if(addonsubsubnode != NULL)
			{
				get_value(addonsubsubnode, tempChar, 256);
				f32 f3 = atof(tempChar);
				printf("f3: %f\n", f3);
				addonTagData[tC-1].part3 = *(u32*)&f3;
				addonTagData[tC-1].part3 = int_swap_bytes(addonTagData[tC-1].part3);

//				addonTagData.part3 = *(u32*)&f3;
//				addonTagData.part3 = int_swap_bytes(addonTagData.part3);
			}
		}
		fseek(fp, oldpos, SEEK_SET);
	//	WriteBRLANTagHeader(addonTagHeader, fp);
		fwrite(&addonTagHeader, sizeof(tag_header), 1, fp);
	//	WriteBRLANTagEntry(addonTagEntry, fp);
		fwrite(&addonTagEntry, sizeof(tag_entry), 1, fp);
	//	WriteBRLANTagInfo(addonTagEntryInfo, fp);
		addonTagEntryInfo.coord_count = short_swap_bytes(addonTagEntryInfo.coord_count);
		fwrite(&addonTagEntryInfo, sizeof(tag_entryinfo), 1, fp);
	//	WriteExtraBytes(byte, fp);
//		u32 extraOne = 0x0;
//		fwrite(&extraOne, 4, 1, fp);
	//	WriteBRLANTagData(addonTagData, fp);
		fwrite(addonTagData, sizeof(tag_data), short_swap_bytes(addonTagEntryInfo.coord_count), fp);
//		fwrite(&addonTagData, sizeof(tag_data), 1, fp);
	//	WriteBRLANTwoExtraBytes(extraBytes, fp);
//		u32 extraTwo[2] = {0x0, 0x0};
//		fwrite(extraTwo, 8, 1, fp);
		filesz = ftell(fp);
	}

	*blobsize = filesz;
	*tagblob = (u8*)malloc(*blobsize);
	fseek(fp, 0, SEEK_SET);
	fread(*tagblob, *blobsize, 1, fp);
	free(entry);
	free(entryinfo);
	free(data);
	fclose(fp);
	remove("temp.blan");
	return filesz;
}

void create_tag_from_xml(mxml_node_t *tree, mxml_node_t *node, u8** tagblob, u32* blobsize)
{
	tag_header head;
	brlan_entry entr;
	char temp[256];
	memset(entr.name, 0, 20);
	if(mxmlElementGetAttr(node, "name") != NULL)
		strcpy(entr.name, mxmlElementGetAttr(node, "name"));
	else{
	}
	if(mxmlElementGetAttr(node, "type") != NULL)
		strcpy(temp, mxmlElementGetAttr(node, "type"));
	else{
		printf("No type attribute found!\nQuitting!\n");
		exit(1);
	}
	head.magic[0] = temp[0];
	head.magic[1] = temp[1];
	head.magic[2] = temp[2];
	head.magic[3] = temp[3];
	head.pad1 = 0;
	head.pad2 = 0;
	head.pad3 = 0;
	memset(temp, 0, 256);
	if(mxmlElementGetAttr(node, "format") != NULL)
		strcpy(temp, mxmlElementGetAttr(node, "format"));
	else{
		printf("No format attribute found!\nQuitting!\n");
		exit(1);
	}
	int x;
	for(x = 0; x < strlen(temp); x++)
		temp[x] = toupper(temp[x]);
	if(strcmp(temp, "NORMAL") == 0)
		entr.flags = 0x01000000;
	else
//		entr.flags = atoi(temp);
		entr.flags = strtoul(temp, NULL, 16);
	create_entries_from_xml(tree, node, &entr, &head, tagblob, blobsize);
}

void WriteBRLANHeader(brlan_header rlanhead, FILE* fp)
{
	brlan_header writehead;
	writehead.magic[0] = rlanhead.magic[0];
	writehead.magic[1] = rlanhead.magic[1];
	writehead.magic[2] = rlanhead.magic[2];
	writehead.magic[3] = rlanhead.magic[3];
	writehead.unk1 = be32(rlanhead.unk1);
	writehead.file_size = be32(rlanhead.file_size);
	writehead.pai1_offset = short_swap_bytes(rlanhead.pai1_offset);
	writehead.pai1_count = short_swap_bytes(rlanhead.pai1_count);
	fwrite(&writehead, sizeof(brlan_header), 1, fp);
}

void WriteBRLANPaiHeader(brlan_pai1_header_type1 paihead, FILE* fp)
{
	brlan_pai1_header_type1 writehead;
	writehead.magic[0] = paihead.magic[0];
	writehead.magic[1] = paihead.magic[1];
	writehead.magic[2] = paihead.magic[2];
	writehead.magic[3] = paihead.magic[3];
	writehead.size = be32(paihead.size);
	writehead.framesize = short_swap_bytes(paihead.framesize);
	writehead.flags = paihead.flags;
	writehead.unk1 = paihead.unk1;
	writehead.num_timgs = short_swap_bytes(paihead.num_timgs);
	writehead.num_entries = short_swap_bytes(paihead.num_entries);
	writehead.entry_offset = be32(paihead.entry_offset);
	fwrite(&writehead, sizeof(brlan_pai1_header_type1), 1, fp);
}

void write_brlan(char *infile, char* outfile)
{
	int i;
	for(i = 0; i < 16; i++)
		memset(tag_types_list[i], 0, 24);
	strcpy(tag_types_list[0], "X Translation");
	strcpy(tag_types_list[1], "Y Translation");
	strcpy(tag_types_list[2], "Z Translation");
	strcpy(tag_types_list[3], "0x03");
	strcpy(tag_types_list[4], "0x04");
	strcpy(tag_types_list[5], "Angle");
	strcpy(tag_types_list[6], "X Zoom");
	strcpy(tag_types_list[7], "Y Zoom");
	strcpy(tag_types_list[8], "Width");
	strcpy(tag_types_list[9], "Height");
	strcpy(tag_types_list[10], "0x0A");
	strcpy(tag_types_list[11], "0x0B");
	strcpy(tag_types_list[12], "0x0C");
	strcpy(tag_types_list[13], "0x0D");
	strcpy(tag_types_list[14], "0x0E");
	strcpy(tag_types_list[15], "0x0F");
	FILE* fpx = fopen(infile, "r");
	if(fpx == NULL) {
		printf("xmlan couldn't be opened!\n");
		exit(1);
	}
	mxml_node_t *hightree = mxmlLoadFile(NULL, fpx, MXML_TEXT_CALLBACK);
	if(hightree == NULL) {
		printf("Couldn't open hightree!\n");
		exit(1);
	}
	mxml_node_t *tree = mxmlFindElement(hightree, hightree, "xmlan", NULL, NULL, MXML_DESCEND);
	if(hightree == NULL) {
		printf("Couldn't get tree!\n");
		exit(1);
	}
	mxml_node_t *node;
	FILE* fp = fopen(outfile, "wb+");
	if(fpx == NULL) {
		printf("destination brlan couldn't be opened!\n");
		exit(1);
	}
	u8* tagblob;
	u32 blobsize;
	u16 blobcount = 0;
	u32 bloboffset;
	brlan_header rlanhead;
	rlanhead.magic[0] = 'R';
	rlanhead.magic[1] = 'L';
	rlanhead.magic[2] = 'A';
	rlanhead.magic[3] = 'N';
	rlanhead.unk1 = 0xFEFF0008;
	rlanhead.file_size = 0;
	rlanhead.pai1_offset = sizeof(brlan_header);
	rlanhead.pai1_count = 1;
	WriteBRLANHeader(rlanhead, fp);
	brlan_pai1_header_type1 paihead;
	paihead.magic[0] = 'p';
	paihead.magic[1] = 'a';
	paihead.magic[2] = 'i';
	paihead.magic[3] = '1';
	paihead.size = 0;
	char temp[256];
	if(mxmlElementGetAttr(tree, "framesize") != NULL)
		strcpy(temp, mxmlElementGetAttr(tree, "framesize"));
	else{
		printf("No framesize attribute found!\nDefaulting to 20.");
		strcpy(temp, "20");
	}	
	paihead.framesize = atoi(temp);
//	paihead.flags = 1;			// NOT ALWAYS 1
	memset(temp, 0, 256);
	if(mxmlElementGetAttr(tree, "flags") != NULL)
	{
		strcpy(temp, mxmlElementGetAttr(tree, "flags"));
	} else {
		printf("No flags attribute found!\nDefaulting to 1.");
		paihead.flags = 1;
	}
	paihead.flags = atoi(temp);
	paihead.unk1 = 0;
	paihead.num_timgs = 0;
	paihead.num_entries = 0;
	paihead.entry_offset = sizeof(brlan_pai1_header_type1);
	WriteBRLANPaiHeader(paihead, fp);
	// Do header stuff here...
	u8* tagchunksbig = (u8*)calloc(MAXIMUM_TAGS_SIZE, 1);
	MEMORY* tagsmem = mopen(tagchunksbig, MAXIMUM_TAGS_SIZE, 3);
	u32 totaltagsize = 0;

	u32 headerOffset;
	u32 tagsbigOffset;
	u32 blobOffsets[256];
	u8* timgblob;
	u32 timgsize;
	u16 timgcount = 0;
	u32 timgoffset;
	u8* timgchunksbig = (u8*)calloc(MAXIMUM_TIMGS_SIZE, 1);
	MEMORY* timgmem = mopen(timgchunksbig, MAXIMUM_TIMGS_SIZE, 3);
	u32 totaltimgize = 0;
	for(node = mxmlFindElement(tree, tree, "timg", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "timg", NULL, NULL, MXML_DESCEND)) {
		timgcount++;
		timgoffset = ftell(fp) + mtell(timgmem) - (4 * (timgcount + 1));
		timgoffset = be32(timgoffset);
		fwrite(&timgoffset, sizeof(u32), 1, fp);
		if(mxmlElementGetAttr(node, "name") != NULL)
			strcpy(temp, mxmlElementGetAttr(node, "name"));
		else{
			printf("No name attribute found!\n");
			exit(1);
		}
		mwrite(temp, strlen(temp) + 1, 1, timgmem);
		totaltimgize += strlen(temp) + 1;
	}
	headerOffset = ftell(fp);
	for(node = mxmlFindElement(tree, tree, "tag", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "tag", NULL, NULL, MXML_DESCEND)) {
		blobcount++;
		bloboffset = ftell(fp) + mtell(tagsmem) - (4 * (blobcount + 1));
		blobOffsets[blobcount-1] = sizeof(brlan_pai1_header_type1) +mtell(tagsmem);
		bloboffset = be32(bloboffset);
		fwrite(&bloboffset, sizeof(u32), 1, fp);
		create_tag_from_xml(tree, node, &tagblob, &blobsize);
		mwrite(tagblob, blobsize, 1, tagsmem);
		totaltagsize += blobsize;
	}
	tagsbigOffset = ftell(fp);
	fseek(fp, headerOffset, SEEK_SET);
	int j; for (j=0;j<blobcount;j++)
	{
		blobOffsets[j] += (blobcount * 4);
		blobOffsets[j] = be32(blobOffsets[j]);
		fwrite(&blobOffsets[j], sizeof(u32), 1, fp);
	}
	fseek(fp, tagsbigOffset, SEEK_SET);
/*
	//new home of the extra tag code
	// write the final tag if flag & 0x02000000
	 mxml_node_t *addonnode = NULL;
	 mxml_node_t *addonsubnode = NULL;
	 mxml_node_t *addonsubsubnode = NULL;
	 tag_header addonTagHeader;
	 tag_entry addonTagEntry;
	 tag_entryinfo addonTagEntryInfo;
	 tag_data addonTagData;
	 char tempChar[256];
	 addonnode = mxmlFindElement(tree, tree, "extratag", NULL, NULL, MXML_DESCEND);
	 if (addonnode != NULL)
	 {
		printf("in the second box\n");
		if(mxmlElementGetAttr(addonnode, "type") != NULL)
			strcpy(temp, mxmlElementGetAttr(addonnode, "type"));
		else{
			printf("No type attribute found!\nSkipping this entry!\n");
		}
		//addonTagHeader.magic = temp;
		memcpy(addonTagHeader.magic, temp, 4);
		addonTagHeader.entry_count = 0x01;
		addonTagHeader.pad1 = 0x0;
		addonTagHeader.pad2 = 0x0;
		addonTagHeader.pad3 = 0x0;
		addonTagEntry.offset = be32(0x0c);
		addonTagEntryInfo.type = short_swap_bytes(0x2);
		addonTagEntryInfo.unk1 = short_swap_bytes(0x0200);
		addonTagEntryInfo.pad1 = 0x0000;
		addonTagEntryInfo.unk2 = be32(0x0000000C);
		addonTagEntryInfo.coord_count = short_swap_bytes(0x01);
	
		for(addonsubnode = mxmlFindElement(addonnode, addonnode, "triplet", NULL, NULL, MXML_DESCEND); addonsubnode != NULL; addonsubnode = mxmlFindElement(addonsubnode, addonnode, "triplet", NULL, NULL, MXML_DESCEND))
		{
			addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "frame", NULL, NULL, MXML_DESCEND);
			if (addonsubsubnode != NULL)
			{
				get_value(addonsubsubnode, tempChar, 256);
				addonTagData.part1 = float_swap_bytes(atof(tempChar));
			}
	
			addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "value", NULL, NULL, MXML_DESCEND);
			if (addonsubsubnode != NULL)
			{
				get_value(addonsubsubnode, tempChar, 256);
				addonTagData.part2 = float_swap_bytes(atof(tempChar));
			}
	
			addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "blend", NULL, NULL, MXML_DESCEND);
			if(addonsubsubnode != NULL)
			{
				get_value(addonsubsubnode, tempChar, 256);
				addonTagData.part3 = float_swap_bytes(atof(tempChar));
			}
		}
		fseek(fp, oldpos, SEEK_SET);
	//	WriteBRLANTagHeader(addonTagHeader, fp);
		fwrite(&addonTagHeader, sizeof(tag_header), 1, fp);
	//	WriteBRLANTagEntry(addonTagEntry, fp);
		fwrite(&addonTagEntry, sizeof(tag_entry), 1, fp);
	//	WriteBRLANTagInfo(addonTagEntryInfo, fp);
		fwrite(&addonTagEntryInfo, sizeof(tag_entryinfo), 1, fp);
	//	WriteExtraBytes(byte, fp);
		u32 extraOne = 0x0;
		fwrite(&extraOne, 4, 1, fp);
	//	WriteBRLANTagData(addonTagData, fp);
		fwrite(&addonTagData, sizeof(tag_data), 1, fp);
	//	WriteBRLANTwoExtraBytes(extraBytes, fp);
		u32 extraTwo[2] = {0x0, 0x0};
		fwrite(extraTwo, 8, 1, fp);
		filesz = ftell(fp);
	}
*/

	tagchunksbig = (u8*)mclose(tagsmem);
	timgchunksbig = (u8*)mclose(timgmem);
	fwrite(timgchunksbig, totaltimgize, 1, fp);
	fwrite(tagchunksbig, totaltagsize, 1, fp);
	paihead.num_timgs = timgcount;
	paihead.entry_offset = sizeof(brlan_pai1_header_type1) + totaltimgize;
	paihead.num_entries = blobcount;
	fseek(fp, 0, SEEK_END);
	paihead.size = ftell(fp) - rlanhead.pai1_offset;
	rlanhead.file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	WriteBRLANHeader(rlanhead, fp);
	WriteBRLANPaiHeader(paihead, fp);
}

void make_brlan(char* infile, char* outfile)
{
	printf("Parsing XMLAN @ %s.\n", infile);
	write_brlan(infile, outfile);
}

