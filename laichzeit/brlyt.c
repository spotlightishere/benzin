/*****************************************************************************
 *  Laichzeit                                                                *
 *  Part of the Benzin Project                                               *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)      <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                             *
 *  Copyright (C)2009 comex                                                  *
 *  Copyright (C)2009 booto                                                  *
 *  All Rights Reserved, HACKERCHANNEL.                                      *
 * ------------------------------------------------------------------------- *
 *  brlyt.c                                                                  *
 *  The code for all the BRLYT stuff. Compile this!                          *
 * ------------------------------------------------------------------------- *
 *  Compilation code contributed by:                                         *
 *          megazig                                                          *
 *  Intermediation code contributed by:                                      *
 *          SquidMan                                                         *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "brlyt.h"
#include "c_oop.h"

static int AddEntryToBRLYT(BRLYT* brlyt, LYTEntry entry)
{
	return AddObjectToList((u8**)&brlyt->entries, (u32*)&brlyt->entrycnt, \
					(u8*)&entry, sizeof(LYTEntry));
}

static int AddTexCoordToPic(BRLYT* brlyt, int pic, LYTTexCoord coord)
{
	return AddObjectToList((u8**)&brlyt->entries[pic].tag.pic.texcoords, \
			       (u32*)&brlyt->entries[pic].tag.pic.texcoordcnt, \
			       (u8*)&coord, sizeof(LYTTexCoord));
}

static int AddSubToGroup(BRLYT* brlyt, int group, LYTSub sub)
{
	return AddObjectToList((u8**)&brlyt->entries[group].tag.grp.subs, \
			       (u32*)&brlyt->entries[group].tag.grp.subcnts, \
			       (u8*)&sub, sizeof(LYTSub));
}

BRLYT* LaichLyt_Start(float width, float height)
{
	BRLYT* brlyt = calloc(sizeof(BRLYT), 1);
	brlyt->width	= width;
	brlyt->height	= height;
	brlyt->entrycnt	= 0;
	brlyt->entries	= NULL;
	return brlyt;
}

int LaichLyt_StartPane(BRLYT* brlyt, char* name)
{
	LYTEntry entry;
	entry.type = LYT_TYPE_PANE;
	entry.done		= 0;
	entry.tag.pane.alpha	= 255;
	entry.tag.pane.alpha2	= 0;
	memset(entry.tag.pane.name, 0, 24);
	strncpy(entry.tag.pane.name, name, 24);
	entry.tag.pane.x	= 0;
	entry.tag.pane.y	= 0;
	entry.tag.pane.z	= 0;
	entry.tag.pane.rot_x	= 0;
	entry.tag.pane.rot_x	= 0;
	entry.tag.pane.angle	= 0;
	entry.tag.pane.xmag	= 0;
	entry.tag.pane.ymag	= 0;
	entry.tag.pane.width	= 0;
	entry.tag.pane.height	= 0;
	entry.tag.pane.done	= 0;
	return AddEntryToBRLYT(brlyt, entry);
}

int LaichLyt_StartText(BRLYT* brlyt, char* name)
{
	LYTEntry entry;
	entry.type = LYT_TYPE_TEXT;
	entry.done			= 0;
	entry.tag.text.pane.alpha	= 255;
	entry.tag.text.pane.alpha2	= 0;
	memset(entry.tag.text.pane.name, 0, 24);
	strncpy(entry.tag.text.pane.name, name, 24);
	entry.tag.text.pane.x		= 0;
	entry.tag.text.pane.y		= 0;
	entry.tag.text.pane.z		= 0;
	entry.tag.text.pane.rot_x	= 0;
	entry.tag.text.pane.rot_x	= 0;
	entry.tag.text.pane.angle	= 0;
	entry.tag.text.pane.xmag	= 0;
	entry.tag.text.pane.ymag	= 0;
	entry.tag.text.pane.width	= 0;
	entry.tag.text.pane.height	= 0;
	entry.tag.text.pane.done	= 0;
	entry.tag.text.font		= LYT_FONT_DEFAULT;
	entry.tag.text.stringlen	= 0;
	entry.tag.text.string		= NULL;
	entry.tag.text.color_fg		= RGBA8(0, 0, 0, 255);
	entry.tag.text.color_bg		= RGBA8(0, 0, 0, 0);
	entry.tag.text.done		= 0;
	return AddEntryToBRLYT(brlyt, entry);
}

int LaichLyt_StartPic(BRLYT* brlyt, char* name)
{
	LYTEntry entry;
	entry.type = LYT_TYPE_PIC;
	entry.done			= 0;
	entry.tag.pic.pane.alpha	= 255;
	entry.tag.pic.pane.alpha2	= 0;
	memset(entry.tag.pic.pane.name, 0, 24);
	strncpy(entry.tag.pic.pane.name, name, 24);
	entry.tag.pic.pane.x		= 0;
	entry.tag.pic.pane.y		= 0;
	entry.tag.pic.pane.z		= 0;
	entry.tag.pic.pane.rot_x	= 0;
	entry.tag.pic.pane.rot_x	= 0;
	entry.tag.pic.pane.angle	= 0;
	entry.tag.pic.pane.xmag		= 0;
	entry.tag.pic.pane.ymag		= 0;
	entry.tag.pic.pane.width	= 0;
	entry.tag.pic.pane.height	= 0;
	entry.tag.pic.pane.done		= 0;
	entry.tag.pic.colors[0]		= RGBA8(0, 0, 0, 255);
	entry.tag.pic.colors[1]		= RGBA8(0, 0, 0, 255);
	entry.tag.pic.colors[2]		= RGBA8(0, 0, 0, 255);
	entry.tag.pic.colors[3]		= RGBA8(0, 0, 0, 255);
	entry.tag.pic.texcoordcnt	= 0;
	entry.tag.pic.texcoords		= NULL;
	entry.tag.pic.done		= 0;
	return AddEntryToBRLYT(brlyt, entry);
}

int LaichLyt_StartWnd(BRLYT* brlyt, char* name)
{
	LYTEntry entry;
	entry.type = LYT_TYPE_WND;
	entry.done			= 0;
	entry.tag.wnd.pane.alpha	= 255;
	entry.tag.wnd.pane.alpha2	= 0;
	memset(entry.tag.wnd.pane.name, 0, 24);
	strncpy(entry.tag.wnd.pane.name, name, 24);
	entry.tag.wnd.pane.x		= 0;
	entry.tag.wnd.pane.y		= 0;
	entry.tag.wnd.pane.z		= 0;
	entry.tag.wnd.pane.rot_x	= 0;
	entry.tag.wnd.pane.rot_x	= 0;
	entry.tag.wnd.pane.angle	= 0;
	entry.tag.wnd.pane.xmag		= 0;
	entry.tag.wnd.pane.ymag		= 0;
	entry.tag.wnd.pane.width	= 0;
	entry.tag.wnd.pane.height	= 0;
	entry.tag.wnd.pane.done		= 0;
	entry.tag.wnd.colors[0]		= RGBA8(0, 0, 0, 255);
	entry.tag.wnd.colors[1]		= RGBA8(0, 0, 0, 255);
	entry.tag.wnd.colors[2]		= RGBA8(0, 0, 0, 255);
	entry.tag.wnd.colors[3]		= RGBA8(0, 0, 0, 255);
	entry.tag.wnd.count		= 0;
	entry.tag.wnd.texcoords		= NULL;
	entry.tag.wnd.done		= 0;
	return AddEntryToBRLYT(brlyt, entry);

int LaichLyt_StartMat(BRLYT* brlyt)
{
	LYTEntry entry;
	entry.type = LYT_TYPE_MAT;
	entry.done			= 0;
	entry.tag.mat.done		= 0;
	entry.tag.mat.tev_colors[0]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.tev_colors[1]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.tev_colors[2]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.tev_colors[3]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors[0]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors[1]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors[2]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors[3]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors2[0]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors2[1]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors2[2]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.unk_colors2[3]	= RGB565(0, 0, 0, 255);
	entry.tag.mat.tev_kcolor[0]	= RGBA8(0, 0, 0, 255);
	entry.tag.mat.tev_kcolor[1]	= RGBA8(0, 0, 0, 255);
	entry.tag.mat.tev_kcolor[2]	= RGBA8(0, 0, 0, 255);
	entry.tag.mat.tev_kcolor[3]	= RGBA8(0, 0, 0, 255);
	entry.tag.mat.refcnt		= 0;
	entry.tag.mat.refs		= NULL;
	entry.tag.mat.stage1cnt		= 0;
	entry.tag.mat.stage1s		= NULL;
	entry.tag.mat.stage2cnt		= 0;
	entry.tag.mat.stage2s		= NULL;
	entry.tag.mat.stage3cnt		= 0;
	entry.tag.mat.stage3s		= NULL;
	entry.tag.mat.stage4cnt		= 0;
	entry.tag.mat.stage4s		= NULL;
	entry.tag.mat.stage5cnt		= 0;
	entry.tag.mat.stage5s		= NULL;
	entry.tag.mat.stage6cnt		= 0;
	entry.tag.mat.stage6s		= NULL;
	entry.tag.mat.stage7cnt		= 0;
	entry.tag.mat.stage7s		= NULL;
	entry.tag.mat.stage8cnt		= 0;
	entry.tag.mat.stage8s		= NULL;
	entry.tag.mat.stage9cnt		= 0;
	entry.tag.mat.stage9s		= NULL;
	entry.tag.mat.stage10cnt	= 0;
	entry.tag.mat.stage10s		= NULL;
	return AddEntryToBRLYT(brlyt, entry);	
}

int LaichLyt_StartGroup(BRLYT* brlyt, char* name)
{
	LYTEntry entry;
	entry.type = LYT_TYPE_GROUP;
	entry.done			= 0;
	memset(entry.tag.grp.name, 0, 16);
	strncpy(entry.tag.grp.name, name, 16);
	entry.tag.grp.subcnts		= 0;
	entry.tag.grp.subs		= NULL;
	entry.tag.grp.done		= 0;
	return AddEntryToBRLYT(brlyt, entry);
}

static void LaichLyt_IntPaneCoords(LYTPane* pane, \
			    float x, float y, float z)
{
	pane->x = x;
	pane->y = y;
	pane->z = z;
}

static void LaichLyt_IntPaneRotation(LYTPane* pane, \
			      float rot_x, float rot_y, float angle)
{
	pane->rot_x = rot_x;
	pane->rot_y = rot_y;
	pane->angle = angle;
}

static void LaichLyt_IntPaneZoom(LYTPane* pane, \
			  float xmag, float ymag)
{
	pane->xmag = xmag;
	pane->ymag = ymag;
}

static void LaichLyt_IntPaneSize(LYTPane* pane, \
			  float width, float height)
{
	pane->width = width;
	pane->height = height;
}

static void LaichLyt_IntPaneAlpha(LYTPane* pane, \
			   u8 alpha, u8 alpha2)
{
	pane->alpha = alpha;
	pane->alpha2 = alpha2;
}

void LaichLyt_PaneCoords(BRLYT* brlyt, int pane, \
			 float x, float y, float z)
{
	LaichLyt_IntPaneCoords(&(brlyt->entries[pane].tag.pane), \
			       x, y, z);
}
void LaichLyt_PaneRotation(BRLYT* brlyt, int pane, \
			   float rot_x, float rot_y, float angle)
{
	LaichLyt_IntPaneRotation(&(brlyt->entries[pane].tag.pane), \
				 rot_x, rot_y, angle);
}
void LaichLyt_PaneZoom(BRLYT* brlyt, int pane, \
			   float xmag, float ymag)
{
	LaichLyt_IntPaneZoom(&(brlyt->entries[pane].tag.pane), \
			     xmag, ymag);
}
void LaichLyt_PaneSize(BRLYT* brlyt, int pane, \
		       float width, float height)
{
	LaichLyt_IntPaneSize(&(brlyt->entries[pane].tag.pane), \
			      width, height);
}
void LaichLyt_PaneAlpha(BRLYT* brlyt, int pane, \
			u8 alpha, u8 alpha2)
{
	LaichLyt_IntPaneAlpha(&(brlyt->entries[pane].tag.pane), \
			      alpha, alpha2);
}

void LaichLyt_TextCoords(BRLYT* brlyt, int text, \
			 float x, float y, float z)
{
	LaichLyt_IntPaneCoords(&(brlyt->entries[text].tag.text.pane), \
			       x, y, z);
}
void LaichLyt_TextRotation(BRLYT* brlyt, int text, \
			   float rot_x, float rot_y, float angle)
{
	LaichLyt_IntPaneRotation(&(brlyt->entries[text].tag.text.pane), \
				 rot_x, rot_y, angle);
}
void LaichLyt_TextZoom(BRLYT* brlyt, int text, \
		       float xmag, float ymag)
{
	LaichLyt_IntPaneZoom(&(brlyt->entries[text].tag.text.pane), \
			     xmag, ymag);
}
void LaichLyt_TextSize(BRLYT* brlyt, int text, \
		       float width, float height)
{
	LaichLyt_IntPaneSize(&(brlyt->entries[text].tag.text.pane), \
			     width, height);
}
void LaichLyt_TextAlpha(BRLYT* brlyt, int text, \
			u8 alpha, u8 alpha2)
{
	LaichLyt_IntPaneAlpha(&(brlyt->entries[text].tag.text.pane), \
			      alpha, alpha2);
}

void LaichLyt_PicCoords(BRLYT* brlyt, int pic, \
			 float x, float y, float z)
{
	LaichLyt_IntPaneCoords(&(brlyt->entries[pic].tag.pic.pane), \
			       x, y, z);
}
void LaichLyt_PicRotation(BRLYT* brlyt, int pic, \
			   float rot_x, float rot_y, float angle)
{
	LaichLyt_IntPaneRotation(&(brlyt->entries[pic].tag.pic.pane), \
				 rot_x, rot_y, angle);
}
void LaichLyt_PicZoom(BRLYT* brlyt, int pic, \
		       float xmag, float ymag)
{
	LaichLyt_IntPaneZoom(&(brlyt->entries[pic].tag.pic.pane), \
			     xmag, ymag);
}
void LaichLyt_PicSize(BRLYT* brlyt, int pic, \
		       float width, float height)
{
	LaichLyt_IntPaneSize(&(brlyt->entries[pic].tag.pic.pane), \
			     width, height);
}
void LaichLyt_PicAlpha(BRLYT* brlyt, int pic, \
			u8 alpha, u8 alpha2)
{
	LaichLyt_IntPaneAlpha(&(brlyt->entries[pic].tag.pic.pane), \
			      alpha, alpha2);
}

void LaichLyt_WndCoords(BRLYT* brlyt, int wnd, \
			 float x, float y, float z)
{
	LaichLyt_IntPaneCoords(&(brlyt->entries[wnd].tag.wnd.pane), \
			       x, y, z);
}
void LaichLyt_WndRotation(BRLYT* brlyt, int wnd, \
			   float rot_x, float rot_y, float angle)
{
	LaichLyt_IntPaneRotation(&(brlyt->entries[wnd].tag.wnd.pane), \
				 rot_x, rot_y, angle);
}
void LaichLyt_WndZoom(BRLYT* brlyt, int wnd, \
		       float xmag, float ymag)
{
	LaichLyt_IntPaneZoom(&(brlyt->entries[wnd].tag.wnd.pane), \
			     xmag, ymag);
}
void LaichLyt_WndSize(BRLYT* brlyt, int wnd, \
		       float width, float height)
{
	LaichLyt_IntPaneSize(&(brlyt->entries[wnd].tag.wnd.pane), \
			     width, height);
}
void LaichLyt_WndAlpha(BRLYT* brlyt, int wnd, \
			u8 alpha, u8 alpha2)
{
	LaichLyt_IntPaneAlpha(&(brlyt->entries[wnd].tag.wnd.pane), \
			      alpha, alpha2);
}

void LaichLyt_TextFont(BRLYT* brlyt, int text, \
			  LYTFont font)
{
	brlyt->entries[text].tag.text.font = font;
}

void LaichLyt_TextString(BRLYT* brlyt, int text, \
			    u16* string, int stringlen)
{
	brlyt->entries[text].tag.text.string = malloc(stringlen * 2);
	memcpy(brlyt->entries[text].tag.text.string, string, stringlen * 2);
	brlyt->entries[text].tag.text.stringlen = stringlen;
}

void LaichLyt_TextColors(BRLYT* brlyt, int text, \
			    u32 fg, u32 bg)
{
	brlyt->entries[text].tag.text.color_fg = fg;
	brlyt->entries[text].tag.text.color_bg = bg;
}


void LaichLyt_PicColors(BRLYT* brlyt, int pic, \
			   u32 color1, u32 color2, u32 color3, u32 color4)
{
	brlyt->entries[pic].tag.pic.colors[0]	= color1;
	brlyt->entries[pic].tag.pic.colors[1]	= color2;
	brlyt->entries[pic].tag.pic.colors[2]	= color3;
	brlyt->entries[pic].tag.pic.colors[3]	= color4;
}

void LaichLyt_PicAddTexCoord(BRLYT* brlyt, int pic, \
				float coords[8])
{
	LYTTexCoord coord;
	int i;
	for(i = 0; i < 8; i++)
		coord.coords[i]	= coords[i];
	AddTexCoordToPic(brlyt, pic, coord);
}

void LaichLyt_WndColors(BRLYT* brlyt, int wnd, \
			   u32 color1, u32 color2, u32 color3, u32 color4)
{
	brlyt->entries[wnd].tag.wnd.colors[0]	= color1;
	brlyt->entries[wnd].tag.wnd.colors[1]	= color2;
	brlyt->entries[wnd].tag.wnd.colors[2]	= color3;
	brlyt->entries[wnd].tag.wnd.colors[3]	= color4;
}

void LaichLyt_WndAddTexCoord(BRLYT* brlyt, int wnd, \
				float coords[8])
{
	LYTTexCoord coord;
	int i;
	for(i = 0; i < 8; i++)
		coord.coords[i]	= coords[i];
	AddTexCoordToWnd(brlyt, wnd, coord);
}


void LaichLyt_GroupAddSub(BRLYT* brlyt, int group, \
			     char* subname)
{

void LaichLyt_GroupAddSub(BRLYT* brlyt, int group, \
			     char* subname)
{
	LYTSub sub;
	memset(sub.name, 0, 16);
	strncpy(sub.name, subname, 16);
	AddSubToGroup(brlyt, group, sub);
}




static void LaichLyt_EndEntry(BRLYT* brlyt, int tag)
{
	switch(brlyt->entries[tag].type) {
		case LYT_TYPE_GROUP:
			brlyt->entries[tag].tag.grp.done = 1;
			break;

		case LYT_TYPE_MAT:
			brlyt->entries[tag].tag.mat.done = 1;
			break;

		case LYT_TYPE_PANE:
			brlyt->entries[tag].tag.pane.done = 1;
			break;
		
		case LYT_TYPE_TEXT:
			brlyt->entries[tag].tag.text.done = 1;
			brlyt->entries[tag].tag.text.pane.done = 1;
			break;
			
		case LYT_TYPE_PIC:
			brlyt->entries[tag].tag.pic.done = 1;
			brlyt->entries[tag].tag.pic.pane.done = 1;
			break;

		case LYT_TYPE_WND:
			brlyt->entries[tag].tag.wnd.done = 1;
			brlyt->entries[tag].tag.wnd.pane.done = 1;
			break;
			
		default:	// lolwut
			return;
	}
	brlyt->entries[tag].done = 1;
}

void LaichLyt_EndGroup(BRLYT* brlyt, int group)
{
	LaichLyt_EndEntry(brlyt, group);
}

void LaichLyt_EndMat(BRLYT* brlyt, int mat)
{
	LaichLyt_EndEntry(brlyt, mat);
}

void LaichLyt_EndWnd(BRLYT* brlyt, int wnd)
{
	LaichLyt_endEntry(brlyt, wnd);
}

void LaichLyt_EndPic(BRLYT* brlyt, int pic)
{
	LaichLyt_EndEntry(brlyt, pic);
}

void LaichLyt_EndText(BRLYT* brlyt, int text)
{
	LaichLyt_EndEntry(brlyt, text);
}

void LaichLyt_EndPane(BRLYT* brlyt, int pane)
{
	LaichLyt_EndEntry(brlyt, pane);
}

u8* LaichLyt_Compile(BRLYT* brlyt, u32* size)
{
	size = 0;
	return NULL;
}


