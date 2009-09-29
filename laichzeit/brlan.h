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
 *  brlan.h                                                                  *
 *  The header file for all the BRLAN stuff. Include this!                   *
 * ------------------------------------------------------------------------- *
 *  Most code contributed by:                                                *
 *          SquidMan                                                         *
 *****************************************************************************/

#ifndef BRLAN_H
#define BRLAN_H

#include "types.h"

#define USE_BRLAN

typedef enum {
	BRLAN_TAG_RLPA = 0,		// Pane Animation (All the transforms)
	BRLAN_TAG_RLVC = 1,		// Vertex Color (Changes colors)
	BRLAN_TAG_RLTP = 2,		// Texture Picture (Changes TPL frame)
	BRLAN_TAG_RLMC = 3,		// Material Color
	BRLAN_TAG_RLTS = 4,		// Texture Shader (Defines RLTPs)
	BRLAN_TAG_RLVI = 5		// Vertex (I?)
	// TODO: Figure out the rest of the names.
} LANTagType;

typedef enum {
	BRLAN_RLPA_X_TRANSLATION = 0,
	BRLAN_RLPA_Y_TRANSLATION = 1,
	BRLAN_RLPA_Z_TRANSLATION = 2,
	BRLAN_RLPA_ROTATE	 = 3,
	BRLAN_RLPA_X_ZOOM	 = 4,
	BRLAN_RLPA_Y_ZOOM	 = 5,
	BRLAN_RLPA_WIDTH	 = 6,
	BRLAN_RLPA_HEIGHT	 = 7,
	BRLAN_RLVC_ALPHA	 = 8,
	BRLAN_RLTS_RANGE	 = 9,
	BRLAN_RLTP_CHANGE_FRAME	 = 10
	// TODO: Add moar.
} LANAnimType;

typedef struct {
	f32	frame;			// The frame.
	f32	value;			// The value.
	f32	interpolation;		// The interpolation.
} LANTriplet;

typedef struct {
	LANAnimType type;		// Type of animation.
	int	    tripletcnt;		// How many triplets.
	LANTriplet* triplets;		// The triplets.
	int	    done;		// Did we end it? (Security measure)
} LANAnim;

typedef struct {
	char	   name[20];		// Name of tag.
	u32	   flags;		// Flags.
	LANTagType type;		// Type of tag.
	int	   animcnt;		// How many animations.
	LANAnim*   anims;		// The animations.
	int	   done;		// Did we end it? (Security measure)
} LANTag;

typedef struct {
	char	name[256];		// Name of texture. Using 256 because
					// the actual size is arbitrary.
} LANTex;

typedef struct {
	int	framesize;		// How many frames to be displayed.
	int	tagcnt;			// How many tags there are.
	int	texcnt;			// How many textures there are.
	LANTag*	tags;			// The tags.
	LANTex*	texs;			// The textures.
	int	done;			// Did we end it? (Security measure)
} BRLAN;

BRLAN*	LaichAnim_Start();
void	LaichAnim_AddTexture(BRLAN* brlan, char* name);
int	LaichAnim_StartTag(BRLAN* brlan, char* name, LANTagType type);
int	LaichAnim_StartAnim(BRLAN* brlan, int tag, LANAnimType type);
void	LaichAnim_AddTriplet(BRLAN* brlan, int tag, int anim, \
			      f32 frame, f32 value, f32 interpolation);
void	LaichAnim_EndAnim(BRLAN* brlan, int tag, int anim);
void	LaichAnim_EndTag(BRLAN* brlan, int tag);
u8*	LaichAnim_Compile(BRLAN* brlan, u32* size);

#endif //BRLAN_H
