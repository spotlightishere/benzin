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
 *  brlan.c                                                                  *
 *  The code for all the BRLAN stuff. Compile this!                          *
 * ------------------------------------------------------------------------- *
 *  Most code contributed by:                                                *
 *          SquidMan                                                         *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "brlan.h"
#include "c_oop.h"

static void AddTripletToAnimOnTagOnBRLAN(BRLAN* brlan, int tag, int anim, \
						LANTriplet triplet)
{
	AddObjectToList((u8**)&brlan->tags[tag].anims[anim].triplets, \
			(u32*)&brlan->tags[tag].anims[anim].tripletcnt, \
			(u8*)&triplet, sizeof(LANTriplet));
}

static int AddAnimToTagOnBRLAN(BRLAN* brlan, int tag, LANAnim anim)
{
	return AddObjectToList((u8**)&brlan->tags[tag].anims, \
			       (u32*)&brlan->tags[tag].animcnt, \
			       (u8*)&anim, sizeof(LANAnim));
}

static int AddTagToBRLAN(BRLAN* brlan, LANTag tag)
{
	return AddObjectToList((u8**)&brlan->tags, \
			       (u32*)&brlan->tagcnt, \
			       (u8*)&tag, sizeof(LANTag));
}

static void AddTextureToBRLAN(BRLAN* brlan, LANTex tex)
{
	AddObjectToList((u8**)&brlan->texs, \
			(u32*)&brlan->texcnt, \
			(u8*)&tex, sizeof(LANTex));
}

BRLAN* LaichAnim_Start()
{
	BRLAN* brlan	 = malloc(sizeof(BRLAN));
	brlan->framesize = 0;
	brlan->tagcnt	 = 0;
	brlan->texcnt	 = 0;
	brlan->tags	 = NULL;
	brlan->texs	 = NULL;
	return brlan;
}

void LaichAnim_AddTexture(BRLAN* brlan, char* name)
{
	LANTex tex;
	memset(tex.name, 0, 256);
	strncpy(tex.name, name, 256);
	AddTextureToBRLAN(brlan, tex);
}

int LaichAnim_StartTag(BRLAN* brlan, char* name, LANTagType type)
{
	LANTag tag;
	memset(tag.name, 0, 20);
	strncpy(tag.name, name, 20);
	tag.type	= type;
	tag.animcnt	= 0;
	tag.anims	= NULL;
	return AddTagToBRLAN(brlan, tag);
}

int LaichAnim_StartAnim(BRLAN* brlan, int tag, LANAnimType type)
{
	LANAnim anim;
	anim.type	= type;
	anim.tripletcnt	= 0;
	anim.triplets	= NULL;
	return AddAnimToTagOnBRLAN(brlan, tag, anim);
}

void LaichAnim_AddTriplet(BRLAN* brlan, int tag, int anim, \
			      f32 frame, f32 value, f32 interpolation)
{
	LANTriplet triplet;
	triplet.frame		= frame;
	triplet.value		= value;
	triplet.interpolation	= interpolation;
	AddTripletToAnimOnTagOnBRLAN(brlan, tag, anim, triplet);	
}

void LaichAnim_EndAnim(BRLAN* brlan, int tag, int anim)
{
	brlan->tags[tag].anims[anim].done = 1;
}

void LaichAnim_EndTag(BRLAN* brlan, int tag)
{
	brlan->tags[tag].done = 1;
}

u8* LaichAnim_Compile(BRLAN* brlan, u32* size);
{
	*size = 0;
	return NULL;
}


