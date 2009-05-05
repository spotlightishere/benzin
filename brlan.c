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
#define dbgprintf    printf
#else
#define dbgprintf    //
#endif //DEBUGBRLAN

#define MAXIMUM_TAGS_SIZE        (0xf000)
#define MAXIMUM_TIMGS_SIZE        (0x1000)

fourcc tag_FourCCs[] = { "RLPA", "RLTS", "RLVI", "RLVC", "RLMC", "RLTP" };

char tag_types_list[15][24];
char tag_types_rlmc_list[15][24];

static size_t BRLAN_fileoffset = 0;
FILE* xmlanout;

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
        pai1_header->magic[0]        = pai1_header1.magic[0];
        pai1_header->magic[1]        = pai1_header1.magic[1];
        pai1_header->magic[2]        = pai1_header1.magic[2];
        pai1_header->magic[3]        = pai1_header1.magic[3];
        pai1_header->size        = pai1_header1.size;
        pai1_header->framesize        = pai1_header1.framesize;
        pai1_header->flags        = pai1_header1.flags;
        pai1_header->unk1        = pai1_header1.unk1;
        pai1_header->num_timgs        = pai1_header1.num_timgs;
        pai1_header->num_entries    = pai1_header1.num_entries;
        pai1_header->unk2        = 0;
        pai1_header->entry_offset    = pai1_header1.entry_offset;
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

void BRLAN_CreateXMLTag(tag_header tagHeader, void* data, u32 offset, mxml_node_t *pane)
{
    BRLAN_fileoffset = offset;
    BRLAN_ReadDataFromMemory(&tagHeader, data, sizeof(tag_header));

    mxml_node_t *tag;
    tag = mxmlNewElement(pane, "tag"); mxmlElementSetAttrf(tag, "type", "%c%c%c%c", tagHeader.magic[0], tagHeader.magic[1], tagHeader.magic[2], tagHeader.magic[3]);
    printf("\t\t<tag type=\"%c%c%c%c\">\n", tagHeader.magic[0], tagHeader.magic[1], tagHeader.magic[2], tagHeader.magic[3]);

    tag_entryinfo tagEntryInfo;
    u32 tagentrylocations[tagHeader.entry_count];
    BRLAN_ReadDataFromMemory(&tagentrylocations, data, tagHeader.entry_count * sizeof(u32));
    u32 i, j;
    for ( i = 0; i < tagHeader.entry_count; i++)
    {
        mxml_node_t *entry;
        BRLAN_fileoffset = offset + be32(tagentrylocations[i]);
        BRLAN_ReadDataFromMemory(&tagEntryInfo, data, sizeof(tag_entryinfo));
        if(short_swap_bytes(tagEntryInfo.type) < 16)
	{
            char type_rlmc[4] = {'R', 'L', 'M', 'C'};
            if(memcmp(tagHeader.magic, type_rlmc, 4) == 0)
            {
                entry = mxmlNewElement(tag, "entry"); mxmlElementSetAttrf(entry, "type", "%s", tag_types_rlmc_list[short_swap_bytes(tagEntryInfo.type)]);
                printf("\t\t\t<entry type=\"%s\">\n", tag_types_rlmc_list[short_swap_bytes(tagEntryInfo.type)]);
            } else {
                entry = mxmlNewElement(tag, "entry"); mxmlElementSetAttrf(entry, "type", "%s", tag_types_list[short_swap_bytes(tagEntryInfo.type)]);
                printf("\t\t\t<entry type=\"%s\">\n", tag_types_list[short_swap_bytes(tagEntryInfo.type)]);
            }
        } else {
            entry = mxmlNewElement(tag, "entry"); mxmlElementSetAttrf(entry, "type", "%u", short_swap_bytes(tagEntryInfo.type));
            printf("\t\t\t<entry type=\"%u\">\n", short_swap_bytes(tagEntryInfo.type));
        }

        for( j = 0; j < short_swap_bytes(tagEntryInfo.coord_count); j++)
        {
            if ( tagEntryInfo.unk1 == 0x2 )
            {
                mxml_node_t *triplet;
                mxml_node_t *frame;
                mxml_node_t *value;
                mxml_node_t *blend;
                tag_data tagData;
                BRLAN_ReadDataFromMemory(&tagData, data, sizeof(tag_data));

                u32 p1 = be32(tagData.part1);
                u32 p2 = be32(tagData.part2);
                u32 p3 = be32(tagData.part3);
                triplet = mxmlNewElement(entry, "triplet");
                printf("\t\t\t\t<triplet>\n");
                frame = mxmlNewElement(triplet, "frame"); mxmlNewTextf(frame, 0, "%.15f", *(f32*)(&p1));
                printf("\t\t\t\t\t<frame>%.15f</frame>\n", *(f32*)(&p1));
                value = mxmlNewElement(triplet, "value"); mxmlNewTextf(value, 0, "%.15f", *(f32*)(&p2));
                printf("\t\t\t\t\t<value>%.15f</value>\n", *(f32*)(&p2));
                blend = mxmlNewElement(triplet, "blend"); mxmlNewTextf(blend, 0, "%.15f", *(f32*)(&p3));
                printf("\t\t\t\t\t<blend>%.15f</blend>\n", *(f32*)(&p3));
                printf("\t\t\t\t</triplet>\n");
            } else {
                tag_data2 tagData2;
                BRLAN_ReadDataFromMemory(&tagData2, data, sizeof(tag_data2));

                mxml_node_t *pair;
                mxml_node_t *data1;
                mxml_node_t *data2;
                mxml_node_t *padding;
                u32 p1 = be32(tagData2.part1);
                u16 p2 = short_swap_bytes(tagData2.part2);
                u16 p3 = short_swap_bytes(tagData2.padding);
                pair = mxmlNewElement(entry, "pair");
                printf("\t\t\t\t<pair>\n");
                data1 = mxmlNewElement(pair, "data1"); mxmlNewTextf(data1, 0, "%.15f", *(f32*)(&p1));
                printf("\t\t\t\t\t<data1>%.15f</data1>\n", *(f32*)(&p1));
                data2 = mxmlNewElement(pair, "data2"); mxmlNewTextf(data2, 0, "%.15f", *(f32*)(&p2));
                printf("\t\t\t\t\t<data2>%04x</data2>\n", p2);
                padding = mxmlNewElement(pair, "padding"); mxmlNewTextf(padding, 0, "%04x", p3);
                printf("\t\t\t\t\t<padding>%04x</padding>\n", p3);
                printf("\t\t\t\t</pair>\n");
            }
        }
        printf("\t\t\t</entry>\n");
    }
    printf("\t\t</tag>\n");
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

    int i, j;
    for(i = 0; i < 16; i++)
    {
        memset(tag_types_list[i], 0, 24);
        memset(tag_types_rlmc_list[i], 0, 24);
    }
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

    strcpy(tag_types_rlmc_list[0], "0x00");
    strcpy(tag_types_rlmc_list[1], "0x01");
    strcpy(tag_types_rlmc_list[2], "0x02");
    strcpy(tag_types_rlmc_list[3], "0x03");
    strcpy(tag_types_rlmc_list[4], "Blackcolor R");
    strcpy(tag_types_rlmc_list[5], "Blackcolor G");
    strcpy(tag_types_rlmc_list[6], "Blackcolor B");
    strcpy(tag_types_rlmc_list[7], "Blackcolor A");
    strcpy(tag_types_rlmc_list[8], "Whitecolor R");
    strcpy(tag_types_rlmc_list[9], "Whitecolor G");
    strcpy(tag_types_rlmc_list[10], "Whitecolor B");
    strcpy(tag_types_rlmc_list[11], "Whitecolor A");
    strcpy(tag_types_rlmc_list[12], "0x0C");
    strcpy(tag_types_rlmc_list[13], "0x0D");
    strcpy(tag_types_rlmc_list[14], "0x0E");
    strcpy(tag_types_rlmc_list[15], "0x0F");

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

    const char *whitespace_cb(mxml_node_t *node, int where)
    {
      const char *name;
      name = node->value.element.name;

      if (!strcmp(name, "xmlan"))
      {
	if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return("\n");
      }
      if (!strcmp(name, "timg"))
      {
	if (where == MXML_WS_BEFORE_OPEN)
            return("\n");
      }
      else if (!strcmp(name, "pane")) 
      {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t");
      }
      else if (!strcmp(name, "tag")) 
      {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t");
      }
      else if (!strcmp(name, "entry"))
      {
	if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t\t");
      }
      else if (!strcmp(name, "triplet") ||
               !strcmp(name, "pair"))
      {
	if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
	  return ("\n\t\t\t\t");
      }
      else if (!strcmp(name, "frame") ||
               !strcmp(name, "value") ||
               !strcmp(name, "blend") ||
               !strcmp(name, "data1") ||
               !strcmp(name, "data2") ||
               !strcmp(name, "padding"))
      {
	if (where == MXML_WS_BEFORE_OPEN)
	  return ("\n\t\t\t\t\t");
      }

     /*
      * Return NULL for no added whitespace...
      */
      return (NULL);
    }

    FILE *xmlFile;
    xmlFile = fopen("testy.xmlan", "w");
    mxml_node_t *xml;
    mxml_node_t *xmlan;
    xml = mxmlNewXML("1.0");
    xmlan = mxmlNewElement(xml, "xmlan");
    mxmlElementSetAttrf(xmlan, "framesize", "%lu", (long unsigned int)short_swap_bytes(pai1_header.framesize));
    mxmlElementSetAttrf(xmlan, "flags", "%02x", pai1_header.flags);

    printf("<?xml version=\"1.0\"?>\n" \
           "<xmlan framesize=\"%lu\" flags=\"%02x\">\n", (long unsigned int)short_swap_bytes(pai1_header.framesize), pai1_header.flags);

    int timgs = short_swap_bytes(pai1_header.num_timgs);

    BRLAN_fileoffset = short_swap_bytes(header.pai1_offset) + sizeof(brlan_pai1_header_type1);
    if ( pai1_header_type == 2 ) BRLAN_fileoffset += 4;
    int tableoff = BRLAN_fileoffset;
    int currtableoff = BRLAN_fileoffset;

    mxml_node_t *timg;
    for( i = 0; i < timgs; i++) {
        u32 curr_timg_off = 0;
        BRLAN_ReadDataFromMemory(&curr_timg_off, data, 4);
        char timgname[256];
        memset(timgname, 0, 256);
        int z = tableoff + be32(curr_timg_off);
        for( j = 0; data[z] != 0; timgname[j++] = data[z], z++);
        {
            printf("\t<timg name=\"%s\" />\n", timgname);
            timg = mxmlNewElement(xmlan, "timg");
            mxmlElementSetAttrf(timg, "name", "%s", timgname);
        }
        currtableoff += 4;
    }

    int tagcount = short_swap_bytes(pai1_header.num_entries);
    u32 taglocations[tagcount];
    BRLAN_fileoffset = be32(pai1_header.entry_offset) + short_swap_bytes(header.pai1_offset);
    BRLAN_ReadDataFromMemory(taglocations, data, tagcount * sizeof(u32));

    for( i = 0; i < tagcount; i++) {
        brlan_entry brlanEntry;
        tag_header tagHeader;
        tag_entry tagEntry;
        tag_entryinfo tagEntryInfo;
        BRLAN_fileoffset = be32(taglocations[i]) + short_swap_bytes(header.pai1_offset);
        u32 brlanEntryOffset = BRLAN_fileoffset;
        BRLAN_ReadDataFromMemory(&brlanEntry, data, sizeof(brlan_entry));

        mxml_node_t *pane;
        pane = mxmlNewElement(xmlan, "pane"); mxmlElementSetAttrf(pane, "name", "%s", brlanEntry.name); mxmlElementSetAttrf(pane, "type", "%u", brlanEntry.is_material);
        printf("\t<pane name=\"%s\" type=\"%u\">\n", brlanEntry.name, brlanEntry.is_material);

        u32 entrylocations[brlanEntry.num_tags];
        BRLAN_ReadDataFromMemory(entrylocations, data, brlanEntry.num_tags * sizeof(u32));
        for ( j = 0; j < brlanEntry.num_tags; j++)
        {
            BRLAN_CreateXMLTag(tagHeader, data, brlanEntryOffset + be32(entrylocations[j]), pane);
        }
         printf("\t</pane>\n");
     }
    printf("</xmlan>\n");
    mxmlSaveFile(xml, xmlFile, whitespace_cb);
    fclose(xmlFile);
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

void WriteBRLANOffsets(u32* offsets, u32 number, FILE* fp)
{
    u32 tempoffsets[number];
    int i;
    for(i=0;i<number;i++) tempoffsets[i] = be32(offsets[i]);
    fwrite(tempoffsets, number * sizeof(u32), 1, fp);
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
    writeentr.num_tags = entr->num_tags;
    writeentr.is_material = entr->is_material;
    writeentr.pad[0] = 0x0;
    writeentr.pad[1] = 0x0;
    fwrite(&writeentr, sizeof(brlan_entry), 1, fp);
}

void create_entries_from_xml(mxml_node_t *tree, mxml_node_t *node, brlan_entry *entr, tag_header* head, FILE* fp)
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

    char tag_type[256];
    if(mxmlElementGetAttr(node, "type") != NULL)
        strcpy(tag_type, mxmlElementGetAttr(node, "type"));

    char rlmc_type[5] = {'R', 'L', 'M', 'C'};

    for(i = 0; i < 16; i++)
        memset(temp3[i], 0, 24);
    for(x = 0; x < 16; x++)
        if(memcmp(tag_type, rlmc_type, 4) == 0)
        {
            for(i = 0; i < strlen(tag_types_rlmc_list[x]); i++)
            {
                temp3[x][i] = toupper(tag_types_rlmc_list[x][i]);
            }
         } else {
            for(i = 0; i < strlen(tag_types_list[x]); i++)
            {
                temp3[x][i] = toupper(tag_types_list[x][i]);
            }
        }
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

    head->pad1 = 0x0; head->pad2 = 0x0; head->pad3 = 0x0;
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
    for( x = 0; x < head->entry_count; x++)
        entry[x].offset += sizeof(u32) * (head->entry_count) + 8;
    WriteBRLANTagEntries(entry, head->entry_count, fp);
    fseek(fp, oldpos, SEEK_SET);
    free(entry);
    free(entryinfo);
}

void create_tag_from_xml(mxml_node_t *tree, mxml_node_t *node, FILE* fp)
{
    brlan_entry entr;
    entr.num_tags = 0x0;
    entr.is_material = 0x0;
    entr.pad[0] = 0x0;
    entr.pad[1] = 0x0;

    char temp[256];
    memset(entr.name, 0, 20);
    if(mxmlElementGetAttr(node, "name") != NULL)
        strcpy(entr.name, mxmlElementGetAttr(node, "name"));
    else{
    }
    if(mxmlElementGetAttr(node, "type") != NULL) {
        strcpy(temp, mxmlElementGetAttr(node, "type"));
        entr.is_material = (u8)atoi(temp);
    } else {
    }

    
    
    tag_header head;
    mxml_node_t *tagnode;
    for(tagnode = mxmlFindElement(node, node, "tag", NULL, NULL, MXML_DESCEND); tagnode != NULL; tagnode = mxmlFindElement(tagnode, node, "tag", NULL, NULL, MXML_DESCEND))
    {
        entr.num_tags++;
    }
    u32 brlanentryoffset = ftell(fp);
    WriteBRLANEntry(&entr, fp);
    
    u32 entrylocations[entr.num_tags];
    int i; for ( i = 0; i < entr.num_tags; i++) entrylocations[i] = brlanentryoffset;
    WriteBRLANOffsets(entrylocations, entr.num_tags, fp);
    
    entr.num_tags = 0x0;
    for(tagnode = mxmlFindElement(node, node, "tag", NULL, NULL, MXML_DESCEND); tagnode != NULL; tagnode = mxmlFindElement(tagnode, node, "tag", NULL, NULL, MXML_DESCEND))
    {
        entrylocations[entr.num_tags] = ftell(fp) - entrylocations[entr.num_tags];
        if(mxmlElementGetAttr(tagnode, "type") != NULL)
            strcpy(temp, mxmlElementGetAttr(tagnode, "type"));
        else {
            printf("No type attribute found!\nQuitting!\n");
            exit(1);
        }
        for ( i = 0; i < 4; i++) head.magic[i] = temp[i];
        create_entries_from_xml(node, tagnode, &entr, &head, fp);    
        entr.num_tags++;
    }
    u32 tempOffset = ftell(fp);
    fseek(fp, brlanentryoffset + sizeof(brlan_entry), SEEK_SET);
    WriteBRLANOffsets(entrylocations, entr.num_tags, fp);
    fseek(fp, tempOffset, SEEK_SET);
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
    {
        memset(tag_types_list[i], 0, 24);
        memset(tag_types_rlmc_list[i], 0, 24);
    }
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

    strcpy(tag_types_rlmc_list[0], "0x00");
    strcpy(tag_types_rlmc_list[1], "0x01");
    strcpy(tag_types_rlmc_list[2], "0x02");
    strcpy(tag_types_rlmc_list[3], "0x03");
    strcpy(tag_types_rlmc_list[4], "Blackcolor R");
    strcpy(tag_types_rlmc_list[5], "Blackcolor G");
    strcpy(tag_types_rlmc_list[6], "Blackcolor B");
    strcpy(tag_types_rlmc_list[7], "Blackcolor A");
    strcpy(tag_types_rlmc_list[8], "Whitecolor R");
    strcpy(tag_types_rlmc_list[9], "Whitecolor G");
    strcpy(tag_types_rlmc_list[10], "Whitecolor B");
    strcpy(tag_types_rlmc_list[11], "Whitecolor A");
    strcpy(tag_types_rlmc_list[12], "0x0C");
    strcpy(tag_types_rlmc_list[13], "0x0D");
    strcpy(tag_types_rlmc_list[14], "0x0E");
    strcpy(tag_types_rlmc_list[15], "0x0F");

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
    
    u32 timgnumber = 0x0;
    for(node = mxmlFindElement(tree, tree, "timg", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "timg", NULL, NULL, MXML_DESCEND))
    {
        timgnumber++;
    }
    u32 imageoffsets[timgnumber];
    u32 imagefileoffset = ftell(fp);
    for( i = 0; i < timgnumber; i++) imageoffsets[i] = imagefileoffset;
    WriteBRLANOffsets(imageoffsets, timgnumber, fp);
    
    for(node = mxmlFindElement(tree, tree, "timg", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "timg", NULL, NULL, MXML_DESCEND)) {
        timgcount++;
        imageoffsets[timgcount-1] = ftell(fp) - imageoffsets[timgcount-1];
        if(mxmlElementGetAttr(node, "name") != NULL)
            strcpy(temp, mxmlElementGetAttr(node, "name"));
        else{
            printf("No name attribute found!\n");
            exit(1);
        }
        fwrite(temp, strlen(temp) + 1, 1, fp);
        totaltimgize += strlen(temp) + 1;
    }
    if ((totaltimgize % 4) != 0)
    {
        u8 blank[3] = {0x0, 0x0, 0x0};
        fwrite(blank, (4 - (totaltimgize % 4)), 1, fp);
        totaltimgize += (4 - (totaltimgize % 4));
    }
    u32 tempoOffset = ftell(fp);
    fseek(fp, imagefileoffset, SEEK_SET);
    WriteBRLANOffsets(imageoffsets, timgnumber, fp);
    fseek(fp, tempoOffset, SEEK_SET);

    u32 panecount = 0x0;
    for(node = mxmlFindElement(tree, tree, "pane", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "pane", NULL, NULL, MXML_DESCEND))
    {
        panecount++;
    }
    u32 paneoffsets[panecount];
    u32 tagoffset = ftell(fp);
    for(i = 0; i < panecount; i++) paneoffsets[i] = 0x10;
    WriteBRLANOffsets(paneoffsets, panecount, fp);
    
    for(node = mxmlFindElement(tree, tree, "pane", NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlFindElement(node, tree, "pane", NULL, NULL, MXML_DESCEND))
    {
        blobcount++;
        paneoffsets[blobcount-1] = ftell(fp) - paneoffsets[blobcount-1];
        create_tag_from_xml(tree, node, fp);
        totaltagsize = ftell(fp) - tagoffset;
    }
    u32 fileSize = ftell(fp);
    fseek(fp, tagoffset, SEEK_SET);
    WriteBRLANOffsets(paneoffsets, blobcount, fp);
    

    paihead.num_timgs = timgcount;
    paihead.entry_offset = sizeof(brlan_pai1_header_type1) + totaltimgize + (4*paihead.num_timgs);
    paihead.num_entries = blobcount;
    fseek(fp, 0, SEEK_END);
    paihead.size = fileSize - rlanhead.pai1_offset;
    rlanhead.file_size = fileSize;
    fseek(fp, 0, SEEK_SET);
    WriteBRLANHeader(rlanhead, fp);
    WriteBRLANPaiHeader(paihead, fp);
}

void make_brlan(char* infile, char* outfile)
{
    printf("Parsing XMLAN @ %s.\n", infile);
    write_brlan(infile, outfile);
}
