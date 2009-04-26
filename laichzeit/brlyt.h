/*****************************************************************************/
/*  Laichzeit                                                                */
/*  Part of the Benzin Project                                               */
/*  Copyright (c)2009 HACKERCHANNEL. Protected under the GNU GPLv2.          */
/* ------------------------------------------------------------------------- */
/*  brlyt.h                                                                  */
/*  The header file for all the BRLYT stuff. Include this!                   */
/* ------------------------------------------------------------------------- */
/*  Compilation code contributed by:                                         */
/*          megazig                                                          */
/*  Intermediation code contributed by:                                      */
/*          SquidMan                                                         */
/*****************************************************************************/

#ifndef BRLYT_H
#define BRLYT_H

#include "types.h"

#define USE_BRLYT

#define RGBA8(w, x, y, z)\
(									      \
	(((u32)(z & 0xFF)) << 24) |					      \
	(((u32)(w & 0xFF)) << 16) |					      \
	(((u32)(x & 0xFF)) << 8)  |					      \
	(((u32)(y & 0xFF)) << 0)					      \
)

#define RGB565(w, x, y, z)\
(									      \
	(((u16)((w & 0xFF) >> 3)) << 11) |				      \
	(((u16)((x & 0xFF) >> 2)) << 6)  |				      \
	(((u16)((y & 0xFF) >> 3)) << 0)					      \
)

typedef enum
{
	LYT_FONT_DEFAULT	= 0	// Default.
} LYTFont;

typedef enum
{
	LYT_TYPE_PANE	= 0,		// Panes.
	LYT_TYPE_TEXT	= 1,		// Text.
	LYT_TYPE_PIC	= 2,		// Pictures.
	LYT_TYPE_WND	= 3,		// Window.
	LYT_TYPE_MAT	= 4,		// Materials.
	LYT_TYPE_GROUP	= 5		// Groups.
} LYTType;

typedef struct
{
	u8	alpha;			// The alpha.
	u8	alpha2;			// Always 0. Seems to deal with alpha.
	char	name[24];		// Name of pane.
	float	x;			// Starting X location.
	float	y;			// Starting Y location.
	float	z;			// Starting Z location.
	float	rot_x;			// Starting X flip rotation.
	float	rot_y;			// Starting Y flip rotation.
	float	angle;			// Starting rotation.
	float	xmag;			// Starting X zoom.
	float	ymag;			// Starting Y zoom.
	float	width;			// Starting width.
	float	height;			// Starting height.
	int	done;			// Is it done? (Safety measure)
} LYTPane;

typedef struct
{
	LYTPane pane;			// The Pane.	
	LYTFont	font;			// Which font to use.
	int	stringlen;		// How many [UTF16] letters in string.
	u16*	string;			// The string (UTF16)
	u32	color_fg;		// Foreground color.
	u32	color_bg;		// Background color.
	int	done;			// Is it done? (Safety measure)
} LYTText;

typedef struct
{
	float	coords[8];		// The coordinates.
} LYTTexCoord;

typedef struct
{
	LYTPane		pane;		// The Pane.
	u32		colors[4];	// Vertex Colors.
	u8		texcoordcnt;	// Number of Texture Coordinates.
	LYTTexCoord*	texcoords;	// Texture coordinates.
	int		done;		// Is it done? (Safety measure)
} LYTPic;

typedef struct
{
	LYTPane		pane;		// The Pane.
	u32		colors[4];	// Vertex Colors.
	u8		count;		// Number of Something.
	LYTTexCoord	texcoords;	// Texture coordinates.
	int		done;		// Is it done? (Safety measure)
} LYTWindow;

typedef struct
{
	char	name[16];		// Name of the sub.
} LYTSub;

typedef struct
{
	char	name[16];		// Name of the group.
	int	subcnts;		// How many subs?
	LYTSub*	subs;			// The subs.
	int	done;			// Is it done? (Safety measure)
} LYTGroup;

typedef struct
{
	u32	wrap_s;			// S wrapping.
	u32	wrap_t;			// T wrapping.
} LYTTexRef;

typedef struct
{
	float	unk1;			// Something.
	float	unk2;			// Something.
	float	unk3;			// Something.
	float	unk4;			// Something.
	float	unk5;			// Something.
} LYTStage1;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage2;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage3;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage4;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage5;

typedef struct
{
	u32	a;			// Something.
	u32	b;			// Something.
	u32	c;			// Something.
	u32	d;			// Something.
	u32	e;			// Something.
} LYTStage6;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage7;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
	u8	unk5;			// Something.
	u8	unk6;			// Something.
	u8	unk7;			// Something.
	u8	unk8;			// Something.
	u8	unk9;			// Something.
	u8	unk10;			// Something.
	u8	unk11;			// Something.
	u8	unk12;			// Something.
	u8	unk13;			// Something.
	u8	unk14;			// Something.
	u8	unk15;			// Something.
	u8	unk16;			// Something.
} LYTStage8;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage9;

typedef struct
{
	u8	unk1;			// Something.
	u8	unk2;			// Something.
	u8	unk3;			// Something.
	u8	unk4;			// Something.
} LYTStage10;

typedef struct
{
	u16	    tev_colors[4];	// Colors.
	u16	    unk_colors[4];	// Colors.
	u16	    unk_colors2[4];	// Colors.
	u32	    tev_kcolor[4];	// Colors.
	int	    refcnt;		// Number of Texture references.
	LYTTexRef*  refs;		// The Texture references.
	int	    stage1cnt;		// Number of Stage1s.
	LYTStage1*  stage1s;		// The Stage1s.
	int	    stage2cnt;		// Number of Stage2s.
	LYTStage2*  stage2s;		// The Stage2s.
	int	    stage3cnt;		// Number of Stage3s.
	LYTStage3*  stage3s;		// The Stage3s.
	int	    stage4cnt;		// Number of Stage4s.
	LYTStage4*  stage4s;		// The Stage4s.
	int	    stage5cnt;		// Number of Stage5s.
	LYTStage5*  stage5s;		// The Stage5s.
	int	    stage6cnt;		// Number of Stage6s.
	LYTStage6*  stage6s;		// The Stage6s.
	int	    stage7cnt;		// Number of Stage7s.
	LYTStage7*  stage7s;		// The Stage7s.
	int	    stage8cnt;		// Number of Stage8s.
	LYTStage8*  stage8s;		// The Stage8s.
	int	    stage9cnt;		// Number of Stage9s.
	LYTStage9*  stage9s;		// The Stage9s.
	int	    stage10cnt;		// Number of Stage10s.
	LYTStage10* stage10s;		// The Stage10s.
	int	    done;		// Is it done? (Safety measure)
} LYTMat;

typedef union
{
	LYTPane    pane;		// Pane.
	LYTText    text;		// Text.
	LYTPic	   pic;			// Picture.
	LYTWindow  window;		// Window.
	LYTMat	   mat;			// Material.
	LYTGroup   grp;			// Group.
} LYTTag;

typedef struct
{
	LYTType	type;			// Kind of entry.
	LYTTag	tag;			// The tag.
	int	done;			// Is it done? (Safety measure)
} LYTEntry;

typedef struct
{
	float	  width;		// Width of banner.
	float	  height;		// Height of banner.
	int	  entrycnt;		// How many entries.
	LYTEntry* entries;		// The entries.
} BRLYT;

BRLYT*	LaichLyt_Start(float width, float height);
int	LaichLyt_StartPane (BRLYT* brlyt, char* name);
int	LaichLyt_StartText (BRLYT* brlyt, char* name);
int	LaichLyt_StartPic  (BRLYT* brlyt, char* name);
int	LaichLyt_StartMat  (BRLYT* brlyt);
int	LaichLyt_StartGroup(BRLYT* brlyt, char* name);

/* Pane stuff */ 
void	LaichLyt_PaneCoords(BRLYT* brlyt, int pane, \
			    float x, float y, float z);
void	LaichLyt_PaneRotation(BRLYT* brlyt, int pane, \
			    float rot_x, float rot_y, float angle);
void	LaichLyt_PaneZoom(BRLYT* brlyt, int pane, \
			    float xmag, float ymag);
void	LaichLyt_PaneSize(BRLYT* brlyt, int pane, \
			    float width, float height);
void	LaichLyt_PaneAlpha(BRLYT* brlyt, int pane, \
			    u8 alpha, u8 alpha2);

/* Panes exist in Text and Pic tags, so we gotta create those too!*/ 
void	LaichLyt_TextCoords(BRLYT* brlyt, int text, \
			    float x, float y, float z);
void	LaichLyt_TextRotation(BRLYT* brlyt, int text, \
			      float rot_x, float rot_y, float angle);
void	LaichLyt_TextZoom(BRLYT* brlyt, int text, \
			  float xmag, float ymag);
void	LaichLyt_TextSize(BRLYT* brlyt, int text, \
			  float width, float height);
void	LaichLyt_TextAlpha(BRLYT* brlyt, int text, \
			   u8 alpha, u8 alpha2);
void	LaichLyt_PicCoords(BRLYT* brlyt, int pic, \
			    float x, float y, float z);
void	LaichLyt_PicRotation(BRLYT* brlyt, int pic, \
			      float rot_x, float rot_y, float angle);
void	LaichLyt_PicZoom(BRLYT* brlyt, int pic, \
			  float xmag, float ymag);
void	LaichLyt_PicSize(BRLYT* brlyt, int pic, \
			  float width, float height);
void	LaichLyt_PicAlpha(BRLYT* brlyt, int pic, \
			   u8 alpha, u8 alpha2);

/* Text stuff */ 
void	LaichLyt_TextFont(BRLYT* brlyt, int text, \
			    LYTFont font);
void	LaichLyt_TextString(BRLYT* brlyt, int text, \
			    u16* string, int stringlen);
void	LaichLyt_TextColors(BRLYT* brlyt, int text, \
			    u32 fg, u32 bg);

/* Pic stuff */
void	LaichLyt_PicColors(BRLYT* brlyt, int pic, \
			   u32 color1, u32 color2, u32 color3, u32 color4);
void	LaichLyt_PicAddTexCoord(BRLYT* brlyt, int pic, \
			   float coords[8]);
			   
/* Group stuff */
void	LaichLyt_GroupAddSub(BRLYT* brlyt, int group, \
				char* subname);

/* Material stuff */
void	LaichLyt_MatColors(BRLYT* brlyt, int mat, \
			   u16 tev_colors[4], u16 unk_colors[4], \
			   u16 unk_colors2[4], u32 tev_kcolor[4]);
void	LaichLyt_MatAddTexRef(BRLYT* brlyt, int mat, \
			   u32 wrap_s, u32 wrap_t);
void	LaichLyt_MatAddStage1(BRLYT* brlyt, int mat, \
			   float unk1, float unk2, float unk3, \
			   float unk4, float unk5);
void	LaichLyt_MatAddStage2(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);
void	LaichLyt_MatAddStage3(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);
void	LaichLyt_MatAddStage4(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);
void	LaichLyt_MatAddStage5(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);
void	LaichLyt_MatAddStage6(BRLYT* brlyt, int mat, \
			   u32 a, u32 b, u32 c, u32 d, u32 e);
void	LaichLyt_MatAddStage7(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);
void	LaichLyt_MatAddStage8(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4, \
			   u8 unk5, u8 unk6, u8 unk7, u8 unk8, \
			   u8 unk9, u8 unk10, u8 unk11, u8 unk12, \
			   u8 unk13, u8 unk14, u8 unk15, u8 unk16);
void	LaichLyt_MatAddStage9(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);
void	LaichLyt_MatAddStage10(BRLYT* brlyt, int mat, \
			   u8 unk1, u8 unk2, u8 unk3, u8 unk4);

void	LaichLyt_EndGroup(BRLYT* brlyt, int group);
void	LaichLyt_EndMat  (BRLYT* brlyt, int mat  );
void	LaichLyt_EndPic  (BRLYT* brlyt, int pic  );
void	LaichLyt_EndText (BRLYT* brlyt, int text );
void	LaichLyt_EndPane (BRLYT* brlyt, int pane );
u8*	LaichLyt_Compile (BRLYT* brlyt, u32* size);


#endif //BRLYT_H
