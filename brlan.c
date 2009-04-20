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

#define MAXIMUM_TAGS_SIZE		(0xf000)
#define MAXIMUM_TIMGS_SIZE		(0x1000)

fourcc tag_FourCCs[] = { "RLPA", "RLTS", "RLVI", "RLVC", "RLMC", "RLTP" };

char tag_types_list[15][24];

static size_t BRLAN_fileoffset = 0;
FILE* xmlanout;

u8 somethingElseIsSet = 0;
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

void parse_brlan(char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if(fp == NULL) {
		printf("Error! Couldn't open %s!\n", filename);
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	u32 lengthOfFile = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8 data[lengthOfFile];
	fread(data, lengthOfFile, 1, fp);

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

	BRLAN_fileoffset = 0;
	brlan_header header;
	BRLAN_ReadDataFromMemoryX(&header, data, sizeof(brlan_header));
	dbgprintf("brlan_header read to.\n");
	BRLAN_fileoffset = short_swap_bytes(header.pai1_offset);
	brlan_pai1_universal universal;
	BRLAN_ReadDataFromMemoryX(&universal, data, sizeof(brlan_pai1_universal));
	dbgprintf("pa1_universal read to.\n");
	
	int pai1_header_type;
	brlan_pai1_header_type1 pai1_header1;
	brlan_pai1_header_type2 pai1_header2;
	brlan_pai1_header_type2 pai1_header;

	if((be32(universal.flags) & (1 << 25)) >= 1) {
		pai1_header_type = 2;
		BRLAN_ReadDataFromMemory(&pai1_header2, data, sizeof(brlan_pai1_header_type2));
	} else {
		pai1_header_type = 1;
		BRLAN_ReadDataFromMemory(&pai1_header1, data, sizeof(brlan_pai1_header_type1));
	}

	CreateGlobal_pai1(&pai1_header, pai1_header1, pai1_header2, pai1_header_type);

	printf("<?xml version=\"1.0\"?>\n" \
	       "<xmlan framesize=\"%lu\" flags=\"%02x\">\n", short_swap_bytes(pai1_header.framesize), pai1_header.flags);

	int timgs = short_swap_bytes(pai1_header.num_timgs);
	printf("Number of TPL files: %d\n", timgs);

	BRLAN_fileoffset = short_swap_bytes(header.pai1_offset) + sizeof(brlan_pai1_header_type1);
	if ( pai1_header_type == 2 ) BRLAN_fileoffset += 4;
	int tableoff = BRLAN_fileoffset;
	int currtableoff = BRLAN_fileoffset;

	for(i = 0; i < timgs; i++) {
		u32 curr_timg_off = 0;
		BRLAN_ReadDataFromMemory(&curr_timg_off, data, 4);
		char timgname[256];
		memset(timgname, 0, 256);
		int z = tableoff + be32(curr_timg_off);
		int o;
		for(o = 0; data[z] != 0; timgname[o++] = data[z], z++);
			printf("\t<timg name=\"%s\" />\n", timgname);
		currtableoff += 4;
	}

	int tagcount = short_swap_bytes(pai1_header.num_entries);
	u32 taglocations[tagcount];
	BRLAN_fileoffset = be32(pai1_header.entry_offset) + short_swap_bytes(header.pai1_offset);
	BRLAN_ReadDataFromMemory(taglocations, data, tagcount * sizeof(u32));

	for(i = 0; i < tagcount; i++) {
		u8 isTriplet = 0;
		brlan_entry brlanEntry;
		tag_header tagHeader;
		tag_entry tagEntry;
		tag_entryinfo tagEntryInfo;
		BRLAN_fileoffset = be32(taglocations[i]) + short_swap_bytes(header.pai1_offset);
		u32 brlanEntryOffset = BRLAN_fileoffset;
		BRLAN_ReadDataFromMemory(&brlanEntry, data, sizeof(brlan_entry));

		u32 offsetToExtras;
		u32 setOfOffsetsToOthers[2];
		if(((be32(brlanEntry.flags) & (1 << 24)) >= 1 ) && ((be32(brlanEntry.flags) & (1<<25)) >=1)) {
			somethingElseIsSet = 1;
			BRLAN_ReadDataFromMemory(&setOfOffsetsToOthers[0], data, 4);
			BRLAN_ReadDataFromMemory(&setOfOffsetsToOthers[1], data, 4);
		}else if((be32(brlanEntry.flags) & (1 << 25)) >= 1) {
			somethingIsSet = 1;
			BRLAN_ReadDataFromMemory(&offsetToExtras, data, 4);
		}

//		if (somethingElseIsSet == 1)
//		{
//			BRLAN_ReadDataFromMemory(&tagHeader, data, sizeof(tag_header));
//			printf("\t<tag name=\"%s\" type=\"%c%c%c%c\" ", brlanEntry.name, tagHeader.magic[0], tagHeader.magic[1], tagHeader.magic[2], tagHeader.magic[3]);
//			printf("format=\"%08x\">\n", be32(brlanEntry.flags));
			
//		}

		BRLAN_ReadDataFromMemory(&tagHeader, data, sizeof(tag_header));
		printf("\t<tag name=\"%s\" type=\"%c%c%c%c\" ", brlanEntry.name, tagHeader.magic[0], tagHeader.magic[1], tagHeader.magic[2], tagHeader.magic[3]);
		if(be32(brlanEntry.flags) == 0x01000000)
		       printf("format=\"%s\">\n", "Normal");
		else
		{
			printf("format=\"%08x\">\n", be32(brlanEntry.flags));
		}

		u32 startOfEntries = BRLAN_fileoffset;
		u32 sOE = startOfEntries;
		u32 j;
		for (j=0;j<tagHeader.entry_count;j++)
		{
		BRLAN_ReadDataFromMemory(&tagEntry, data, sizeof(tag_entry));
		memcpy(&tagEntryInfo, &data[sOE - 8 + be32(tagEntry.offset)], sizeof(tag_entryinfo));

		if(short_swap_bytes(tagEntryInfo.type) < 16)
			printf("\t\t<entry type=\"%s\">\n", tag_types_list[short_swap_bytes(tagEntryInfo.type)]);
		else
			printf("\t\t<entry type=\"%u\">\n", short_swap_bytes(tagEntryInfo.type));
		if( tagEntryInfo.unk1 == 0x2) isTriplet = 1;

		int k;
		for(k=0;k<short_swap_bytes(tagEntryInfo.coord_count);k++)
		{
		if ( isTriplet == 1 )
		{
			tag_data tagData;
			memcpy(&tagData, &data[sOE - 8 + be32(tagEntry.offset) + sizeof(tag_entryinfo)+ (k*sizeof(tag_data))], sizeof(tag_data));

			u32 p1 = be32(tagData.part1);
			u32 p2 = be32(tagData.part2);
			u32 p3 = be32(tagData.part3);
			printf("\t\t\t<triplet>\n");
			printf("\t\t\t\t<frame>%.12f</frame>\n", *(f32*)(&p1));
			printf("\t\t\t\t<value>%.12f</value>\n", *(f32*)(&p2));
			printf("\t\t\t\t<blend>%.12f</blend>\n", *(f32*)(&p3));
			printf("\t\t\t</triplet>\n");
			
		} else {
			tag_data2 tagData2;
			memcpy(&tagData2, &data[sOE -8+ be32(tagEntry.offset) + sizeof(tag_entryinfo)+ (k*sizeof(tag_data2))], sizeof(tag_data2));
			u32 p1 = be32(tagData2.part1);
			u16 p2 = short_swap_bytes(tagData2.part2);
			u16 p3 = short_swap_bytes(tagData2.padding);
			printf("\t\t\t<pair>\n");
			printf("\t\t\t\t<data1>%.12f</data1>\n", *(f32*)(&p1));
			printf("\t\t\t\t<data2>%04x</data2>\n", p2);
			printf("\t\t\t\t<padding>%04x</padding>\n", p3);
			printf("\t\t\t</pair>\n");

		}
		}
		printf("\t\t</entry>\n");
		isTriplet = 0;
		}
		printf("\t</tag>\n");
		if (somethingIsSet == 1)
		{
		tag_header tagHeader;
		tag_entry tagEntry;
		tag_entryinfo tagEntryInfo;
		memcpy(&tagHeader, &data[brlanEntryOffset + be32(offsetToExtras)], sizeof(tag_header));
		printf("\t<extratag type=\"%c%c%c%c\">\n", tagHeader.magic[0], tagHeader.magic[1], tagHeader.magic[2], tagHeader.magic[3]);
		for(j=0;j<tagHeader.entry_count;j++)
		{
		memcpy(&tagEntry, &data[brlanEntryOffset + be32(offsetToExtras) + sizeof(tag_header) + (j*4)], 4);
		memcpy(&tagEntryInfo, &data[brlanEntryOffset + be32(offsetToExtras) + be32(tagEntry.offset)], sizeof(tag_entryinfo));
		
		if(short_swap_bytes(tagEntryInfo.type) < 16)
			printf("\t\t<entry type=\"%s\">\n", tag_types_list[short_swap_bytes(tagEntryInfo.type)]);
		else
			printf("\t\t<entry type=\"%u\">\n", short_swap_bytes(tagEntryInfo.type));
		if ( tagEntryInfo.unk1 == short_swap_bytes(0x0200) ) isTriplet = 1;
		int k;

		for(k=0;k<short_swap_bytes(tagEntryInfo.coord_count);k++)
		{
		if ( isTriplet == 1 )
		{
			tag_data tagData;
			memcpy(&tagData, &data[brlanEntryOffset + be32(offsetToExtras) + be32(tagEntry.offset)+ sizeof(tag_entryinfo) + (k*sizeof(tag_data))], sizeof(tag_data));
			u32 p1 = be32(tagData.part1);
			u32 p2 = be32(tagData.part2);
			u32 p3 = be32(tagData.part3);
			printf("\t\t\t<triplet>\n");
			printf("\t\t\t\t<frame>%.12f</frame>\n", *(f32*)(&p1));
			printf("\t\t\t\t<value>%.12f</value>\n", *(f32*)(&p2));
			printf("\t\t\t\t<blend>%.12f</blend>\n", *(f32*)(&p3));
			printf("\t\t\t</triplet>\n");
			
		} else {
			tag_data2 tagData2;
			memcpy(&tagData2, &data[brlanEntryOffset + be32(offsetToExtras) + be32(tagEntry.offset) + sizeof(tag_entryinfo)+ (k*sizeof(tag_data2))], sizeof(tag_data2));
			u32 p1 = be32(tagData2.part1);
			u16 p2 = short_swap_bytes(tagData2.part2);
			u16 p3 = short_swap_bytes(tagData2.padding);
			printf("\t\t\t<pair>\n");
			printf("\t\t\t\t<data1>%.12f</data1>\n", *(f32*)(&p1));
			printf("\t\t\t\t<data2>%04x</data2>\n", p2);
			printf("\t\t\t\t<padding>%04x</padding>\n", p3);
			printf("\t\t\t</pair>\n");

		}
		}
		printf("\t\t</entry>\n");
		isTriplet = 0;
		}
		printf("\t</extratag>\n");
		}
		somethingIsSet = 0;
	}

	printf("</xmlan>\n");

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
	tag_data2** data2 = NULL;
	mxml_node_t *tempnode = NULL;
	mxml_node_t *subnode = NULL;
	mxml_node_t *subsubnode = NULL;
	char temp[256];
	char temp2[256];
	char temp3[16][24];
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
		{
			data = (tag_data**)malloc(sizeof(tag_data*) * head->entry_count);
			data2 = (tag_data2**)malloc(sizeof(tag_data2*) * head->entry_count);
		} else {
			data = (tag_data**)realloc(data, sizeof(tag_data*) * head->entry_count);
			data2 = (tag_data2**)realloc(data, sizeof(tag_data2*) * head->entry_count);
		}
		data[x] = NULL;
		data2[x] = NULL;
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
		for(i = 0; (i < 17) && (strcmp(temp3[i - 1], temp2) != 0); i++);
		if(i == 17)
		{
			i = atoi(temp2);
		}
		else
		{
			i--;
		}
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
		for (i = 0, subsubnode = mxmlFindElement(subnode, subnode, "pair", NULL, NULL, MXML_DESCEND); subsubnode != NULL; subsubnode = mxmlFindElement(subsubnode, subnode, "pair", NULL, NULL, MXML_DESCEND), i++) {
			entryinfo[x].unk1 = 0x100;
			entryinfo[x].coord_count++;
			data2[x] = realloc(data2[x], sizeof(tag_data2) * entryinfo[x].coord_count);
			tempnode = mxmlFindElement(subsubnode, subsubnode, "data1", NULL, NULL, MXML_DESCEND);
			if(tempnode == NULL) {
				printf("Couldn't find attribute \"data1\"!\n");
				exit(1);
			}
			get_value(tempnode, temp, 256);
			*(f32*)(&(data2[x][i].part1)) = atof(temp);
			tempnode = mxmlFindElement(subsubnode, subsubnode, "data2", NULL, NULL, MXML_DESCEND);
			if(tempnode == NULL) {
				printf("Couldn't find attribute \"data2\"!\n");
				exit(1);
			}
			get_value(tempnode, temp, 256);
			data2[x][i].part2 = short_swap_bytes(strtoul(temp, NULL, 16));
			tempnode = mxmlFindElement(subsubnode, subsubnode, "padding", NULL, NULL, MXML_DESCEND);
			if(tempnode == NULL) {
				printf("Couldn't find attribute \"padding\"!\n");
				exit(1);
			}
			get_value(tempnode, temp, 256);
			data2[x][i].padding = short_swap_bytes(strtoul(temp, NULL, 16));
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
		fillerInt = be32(0x60);
		fwrite(&fillerInt, sizeof(u32), 1, fp);
		BRLAN_fileoffset += 4;
	}
	WriteBRLANTagHeader(head, fp);
	u32 entryloc = ftell(fp);
	u32 animLen;
	WriteBRLANTagEntries(entry, head->entry_count, fp);
	u32* entryinfolocs = (u32*)calloc(head->entry_count, sizeof(u32));
	for(x = 0; x < head->entry_count; x++) {
		entryinfolocs[x] = ftell(fp);
		if (x>0)
		{
			if ( entryinfo[x].unk1 == (0x200) )
			{
			entry[x].offset = entry[x-1].offset + sizeof(tag_entryinfo) + (entryinfo[x-1].coord_count * sizeof(tag_data));
			}
			if ( entryinfo[x].unk1 == (0x100) )
			{
				entry[x].offset = entry[x-1].offset + sizeof(tag_entryinfo) + (entryinfo[x-1].coord_count * sizeof(tag_data2));
			}
		} else {
			entry[x].offset = x * (sizeof(tag_entryinfo) + sizeof(tag_data));
		}
		WriteBRLANTagEntryinfos(entryinfo[x], fp);
		if (x==0) animLen = ftell(fp);
		if ( entryinfo[x].unk1 == 0x200 )
			WriteBRLANTagData(data[x], entryinfo[x].coord_count, fp);
		if ( entryinfo[x].unk1 == 0x100 )
		{
			tag_data2 writedata;
			int i;
			for(i = 0; i < entryinfo[x].coord_count; i++) {
				writedata.part1 = be32(data2[x][i].part1);
				writedata.part2 = (data2[x][i].part2);
				writedata.padding = (data2[x][i].padding);
				fwrite(&writedata, sizeof(tag_data2), 1, fp);
			}
		}
	}
	if ( entryinfo[0].unk1 == 0x200 )
		free(data);
	if ( entryinfo[0].unk1 == 0x100 )
		free(data2);	
	u32 oldpos = ftell(fp);
	fseek(fp, entryloc, SEEK_SET);
	for(x=0;x<head->entry_count;x++)
		entry[x].offset += sizeof(u32) * (head->entry_count) + 8;
	WriteBRLANTagEntries(entry, head->entry_count, fp);
	fseek(fp, oldpos, SEEK_SET);
	u32 filesz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	entr->anim_header_len = sizeof(brlan_entry);
	if (entr->flags & 0x02000000)
	{
		entr->anim_header_len += 4;
		u32 tempOffset = ftell(fp);
		fillerInt = be32(filesz);
		fseek(fp, fillerIntOffset, SEEK_SET);
		fwrite(&fillerInt, sizeof(u32), 1, fp);
		fseek(fp, tempOffset, SEEK_SET);
	}
	WriteBRLANEntry(entr, fp);

	mxml_node_t *addonnode = NULL;
	mxml_node_t *addonsubnode = NULL;
	mxml_node_t *addonsubsubnode = NULL;
	tag_header addonTagHeader;
	tag_entry addonTagEntry[256];
	tag_entryinfo addonTagEntryInfo[256];
	tag_data *addonTagData[1024];
	tag_data2 *addonTagData2[1024];
	char tempChar[256];
	if(entr->flags & 0x02000000)
	{
	addonnode = mxmlFindElement(node, tree, "extratag", NULL, NULL, MXML_DESCEND);
	if (addonnode != NULL)
	{
		if(mxmlElementGetAttr(addonnode, "type") != NULL)
			strcpy(temp, mxmlElementGetAttr(addonnode, "type"));
		else{
			printf("No type attribute found!\nSkipping this entry!\n");
		}
		memcpy(addonTagHeader.magic, temp, 4);
		addonTagHeader.entry_count = 0x00;
		addonTagHeader.pad1 = 0x0;
		addonTagHeader.pad2 = 0x0;
		addonTagHeader.pad3 = 0x0;
		fseek(fp, oldpos, SEEK_SET);
		fwrite(&addonTagHeader, sizeof(tag_header), 1, fp);

		mxml_node_t *entrynode;
		for(entrynode = mxmlFindElement(addonnode, addonnode, "entry", NULL, NULL, MXML_DESCEND); entrynode != NULL; entrynode = mxmlFindElement(entrynode, addonnode, "entry", NULL, NULL, MXML_DESCEND))
		{
			memset(temp, 0, 256);
			memset(temp2, 0, 256);
 			if(mxmlElementGetAttr(entrynode, "type") != NULL)
				strcpy(temp, mxmlElementGetAttr(entrynode, "type"));
			else{
				printf("No type attribute found!\nSkipping this entry!\n");
				head->entry_count--;
			}
			for(i = 0; i < strlen(temp); i++)
				temp2[i] = toupper(temp[i]);
			for(i = 0; (i < 17) && (strcmp(temp3[i - 1], temp2) != 0); i++);
			if(i == 17)
				i = atoi(temp2);
			else
				i--;
			addonTagEntryInfo[addonTagHeader.entry_count].type = short_swap_bytes(i);	
			addonTagEntryInfo[addonTagHeader.entry_count].unk1 = short_swap_bytes(0x0200);
			addonTagEntryInfo[addonTagHeader.entry_count].pad1 = 0x0000;
			addonTagEntryInfo[addonTagHeader.entry_count].unk2 = be32(0x0000000C);
			addonTagEntryInfo[addonTagHeader.entry_count].coord_count = 0x0;
	
			for(addonsubnode = mxmlFindElement(entrynode, entrynode, "triplet", NULL, NULL, MXML_DESCEND); addonsubnode != NULL; addonsubnode = mxmlFindElement(addonsubnode, entrynode, "triplet", NULL, NULL, MXML_DESCEND))
			{
				addonTagEntryInfo[addonTagHeader.entry_count].unk1 = short_swap_bytes(0x0200);
				addonTagEntryInfo[addonTagHeader.entry_count].coord_count += 1;
				u32 tC = addonTagEntryInfo[addonTagHeader.entry_count].coord_count;

				addonTagData[addonTagHeader.entry_count] = realloc(addonTagData[addonTagHeader.entry_count], sizeof(tag_data) * tC);		// S E R I O U S  H E L P  N E E D E D //
				addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "frame", NULL, NULL, MXML_DESCEND);
				if (addonsubsubnode != NULL)
				{
					get_value(addonsubsubnode, tempChar, 256);
					f32 f1 = atof(tempChar);
					addonTagData[addonTagHeader.entry_count][tC-1].part1 = *(u32*)&f1;
					addonTagData[addonTagHeader.entry_count][tC-1].part1 = int_swap_bytes(addonTagData[addonTagHeader.entry_count][tC-1].part1);
				}
				addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "value", NULL, NULL, MXML_DESCEND);
				if (addonsubsubnode != NULL)
				{
					get_value(addonsubsubnode, tempChar, 256);
					f32 f2 = atof(tempChar);
					addonTagData[addonTagHeader.entry_count][tC-1].part2 = *(u32*)&f2;
					addonTagData[addonTagHeader.entry_count][tC-1].part2 = int_swap_bytes(addonTagData[addonTagHeader.entry_count][tC-1].part2);
				}
				addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "blend", NULL, NULL, MXML_DESCEND);
				if(addonsubsubnode != NULL)
				{
					get_value(addonsubsubnode, tempChar, 256);
					f32 f3 = atof(tempChar);
					addonTagData[addonTagHeader.entry_count][tC-1].part3 = *(u32*)&f3;
					addonTagData[addonTagHeader.entry_count][tC-1].part3 = int_swap_bytes(addonTagData[addonTagHeader.entry_count][tC-1].part3);
				}
			}
			for(addonsubnode = mxmlFindElement(entrynode, entrynode, "pair", NULL, NULL, MXML_DESCEND); addonsubnode != NULL; addonsubnode = mxmlFindElement(addonsubnode, entrynode, "pair", NULL, NULL, MXML_DESCEND))
			{
				addonTagEntryInfo[addonTagHeader.entry_count].unk1 = short_swap_bytes(0x0100);
				addonTagEntryInfo[addonTagHeader.entry_count].coord_count +=1;
				u32 tC = addonTagEntryInfo[addonTagHeader.entry_count].coord_count;

				addonTagData2[addonTagHeader.entry_count] = realloc(addonTagData2[addonTagHeader.entry_count], sizeof(tag_data2) * tC);
				addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "data1", NULL, NULL, MXML_DESCEND);
				if (addonsubsubnode != NULL)
				{
					get_value(addonsubsubnode, tempChar, 256);
					f32 f1 = atof(tempChar);
					addonTagData2[addonTagHeader.entry_count][tC-1].part1 = *(u32*)&f1;
					addonTagData2[addonTagHeader.entry_count][tC-1].part1 = int_swap_bytes(addonTagData2[addonTagHeader.entry_count][tC-1].part1);
				}
				addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "data2", NULL, NULL, MXML_DESCEND);
				if (addonsubsubnode != NULL)
				{
					get_value(addonsubsubnode, tempChar, 256);
					addonTagData2[addonTagHeader.entry_count][tC-1].part2 = short_swap_bytes((short)strtoul(tempChar, NULL, 16));
				}
				addonsubsubnode = mxmlFindElement(addonsubnode, addonsubnode, "padding", NULL, NULL, MXML_DESCEND);
				if(addonsubsubnode != NULL)
				{
					get_value(addonsubsubnode, tempChar, 256);
					addonTagData2[addonTagHeader.entry_count][tC-1].padding = short_swap_bytes((short)strtoul(tempChar, NULL, 16));
				}
			}
			u32 eC = addonTagHeader.entry_count;
			u32 cC = addonTagEntryInfo[eC].coord_count;
			if (addonTagHeader.entry_count > 0)
			{
				cC = short_swap_bytes(addonTagEntryInfo[eC-1].coord_count);
				printf("cC: %08x\n", cC);
			}
			addonTagEntry[addonTagHeader.entry_count].offset = sizeof(tag_header);
			if (addonTagHeader.entry_count > 0)
			{
				if (addonTagEntryInfo[eC-1].unk1 == 0x2)
				{
					addonTagEntry[addonTagHeader.entry_count].offset = addonTagEntry[addonTagHeader.entry_count-1].offset + sizeof(tag_entryinfo) + (sizeof(tag_data) * cC);
				}
				if (addonTagEntryInfo[eC-1].unk1 == 0x1)
				{
					addonTagEntry[addonTagHeader.entry_count].offset = addonTagEntry[addonTagHeader.entry_count-1].offset + sizeof(tag_entryinfo) + (sizeof(tag_data2) * cC);				
				}
			}

			addonTagEntryInfo[eC].coord_count = short_swap_bytes(addonTagEntryInfo[eC].coord_count);
			addonTagHeader.entry_count++;
		}
		fseek(fp, oldpos, SEEK_SET);
		fwrite(&addonTagHeader, sizeof(tag_header), 1, fp);
		int l; for (l=0;l<addonTagHeader.entry_count;l++)
		{
			addonTagEntry[l].offset += ( sizeof(tag_entry) * addonTagHeader.entry_count );
			addonTagEntry[l].offset = int_swap_bytes(addonTagEntry[l].offset);
		}
		fwrite(&addonTagEntry, sizeof(tag_entry), addonTagHeader.entry_count, fp);

		for (l=0;l<addonTagHeader.entry_count;l++)
		{
			fwrite(&addonTagEntryInfo[l], sizeof(tag_entryinfo), 1, fp);
			if(addonTagEntryInfo[l].unk1 == 0x2)
			{
				fwrite(addonTagData[l], sizeof(tag_data), short_swap_bytes(addonTagEntryInfo[l].coord_count), fp);
			}
			if(addonTagEntryInfo[l].unk1 == 0x1)
			{
				fwrite(addonTagData2[l], sizeof(tag_data2), short_swap_bytes(addonTagEntryInfo[l].coord_count), fp);
			}
		}
		filesz = ftell(fp);
	}
	}

	*blobsize = filesz;
	*tagblob = (u8*)malloc(*blobsize);
	fseek(fp, 0, SEEK_SET);
	fread(*tagblob, *blobsize, 1, fp);
	free(entry);
	free(entryinfo);
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
	u32 timgStartOffset = ftell(fp);
	u32 timgOffsets[256];
	for(node = mxmlFindElement(tree, tree, "timg", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "timg", NULL, NULL, MXML_DESCEND)) {
		timgcount++;
		timgoffset = ftell(fp) + mtell(timgmem) - (4 * (timgcount + 1));
		timgOffsets[timgcount-1] = mtell(timgmem);
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
	if ((totaltimgize % 4) != 0) totaltimgize += (4 - (totaltimgize % 4));
	u32 tempoOffset = ftell(fp);
	int q;
	for(q=0;q<timgcount;q++)
	{
		timgOffsets[q] += 4 * timgcount;
		timgOffsets[q] = be32(timgOffsets[q]);
	}
	fseek(fp, timgStartOffset, SEEK_SET);
	fwrite(timgOffsets, sizeof(u32), timgcount, fp);
	u32 timgbigOffset = ftell(fp);
	fseek(fp, tempoOffset, SEEK_SET);

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

	fseek(fp, tagsbigOffset, SEEK_SET);

	tagchunksbig = (u8*)mclose(tagsmem);
	timgchunksbig = (u8*)mclose(timgmem);
	fseek(fp, timgbigOffset, SEEK_SET);
	fwrite(timgchunksbig, totaltimgize, 1, fp);
	u32 whereToGo = timgbigOffset + totaltimgize;
	fseek(fp, whereToGo, SEEK_SET);
	int j; for (j=0;j<blobcount;j++)
	{
		blobOffsets[j] += (blobcount * 4)+(4*timgcount) + totaltimgize;
		blobOffsets[j] = be32(blobOffsets[j]);
		fwrite(&blobOffsets[j], sizeof(u32), 1, fp);
	}
	fwrite(tagchunksbig, totaltagsize, 1, fp);
	paihead.num_timgs = timgcount;
	paihead.entry_offset = sizeof(brlan_pai1_header_type1) + totaltimgize + (4*paihead.num_timgs);
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

