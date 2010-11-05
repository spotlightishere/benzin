/*****************************************************************************
 *  tpl.c                                                                    *
 *  Part of Zetsubou                                                         *
 *  Part of Benzin                                                           *
 *  The TPL handling shiz.                                                   *
 *  Copyright (C)2009 SquidMan (Alex Marshall)       <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)      <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                             *
 *  Copyright (C)2009 comex                                                  *
 *  Copyright (C)2009 booto                                                  *
 *  Licensed under the GNU GPLv2.                                            *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>

#include "tpl.h"
#include "endian.h"

#define align(x, y) ((x) + ((x) % (y)))

int TPL_ConvertRGB565ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					if((x >= width) || (y >= height))
						continue;
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((iv++) * 2)));
					oldpixel = be16(oldpixel);
					u8 b = ((oldpixel>>11) &0x1F) *(255/0x1f);
					u8 g = ((oldpixel>>5)  &0x3F) *(255/0x3f);
					u8 r = ((oldpixel>>0)  &0x1F) *(255/0x1f);
					u8 a = 255;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					(*(u32**)bitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToRGB565(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4), 2);
	if(*tplbuf == NULL)
		return -1;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4) * 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 newpixel = 0;
					if((x >= width) || (y >= height))
						newpixel = 0;
					else {
						u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
						u8 r = (rgba >> 0)  & 0xFF;
						u8 g = (rgba >> 8)  & 0xFF;
						u8 b = (rgba >> 16) & 0xFF;
//						u8 a = (rgba >> 24)  & 0xFF;

						newpixel = ((b/(255/0x1f)) << 11) | ((g/(255/0x3f)) << 5) | ((r/(255/0x1f)) << 0);
					}
					((u16*)writebuf)[iv++] = be16(newpixel);
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
}

/*int TPL_ConvertRGBA8ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	u32 goodwidth = width;
	u32 goodheight = height;
	u32 x, y;
	u32 x1, y1;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	u8 a[16];
	u8 r[16];
	u8 g[16];
	u8 b[16];
	int i = 0;
	int z = 0;
	int iv = 0;
	int bmpslot = 0;
	for(y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					width = goodwidth;	// Lolwut, we kept corrupting, so this makes sure it isn't.
					height = goodheight;
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((x + (y * width)) * 2)));
					if((x >= width) || (y >= height)) {
						if(iv < 16) {
							a[i] = 0;
							r[i++] = 0;
						}else{
							g[i] = 0;
							b[i++] = 0;
						}
					}else{
						if(iv < 16) {
							a[i] = (oldpixel >> 8) & 0xFF;
							r[i++] = (oldpixel >> 0) & 0xFF;
						}else{
							g[i] = (oldpixel >> 8) & 0xFF;
							b[i++] = (oldpixel >> 0) & 0xFF;
						}
					}
					iv++;
					if(iv == 16) i  = 0;	// Gotta reset i every 16 "pixels"!
					if(iv >= 32) iv = 0;	// Swap every 32 "pixels"!
				}
			}
			width = goodwidth;		// Lolwut, we kept corrupting, so this makes sure it isn't.
			height = goodheight;
			for(i = 0; i < 16; i++) {
				u32 rgba = (r[i] << 0) | (g[i] << 8) | (b[i] << 16) | (a[i] << 24);
				(*(u32**)bitmapdata)[bmpslot++] = rgba;
			}
			z = 0;
			i = 0;
		}
	}
	return outsz;
}*/
/* Let's try icefire's code. */
int TPL_ConvertRGBA8ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	u32 x, y;
	u32 x1, y1;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	u8 r, g, b, a;
	int i, iv = 0;
	for(y = 0; y < height; y += 4) {
		for(x = 0; x < width; x += 4) {
			for(i = 0; i < 2; i++) {
				for(y1 = y; y1 < (y + 4); y1++) {
					for(x1 = x; x1 < (x + 4); x1++) {
						u16 oldpixel = be16( *(u16*)(tplbuf + (tplpoint + ((iv++) * 2))) );
						if((x1 >= width) || (y1 >= height))
							continue;
						
						if(i == 0) {
							a = (oldpixel >> 8) & 0xFF;
							r = (oldpixel >> 0) & 0xFF;
							(*(u32**)bitmapdata)[x1 + (y1 * width)] |= (u32)((r << 16) | (a << 24));
						} else {
							g = (oldpixel >> 8) & 0xFF;
							b = (oldpixel >> 0) & 0xFF;
							(*(u32**)bitmapdata)[x1 + (y1 * width)] |= (u32)((g << 8) | (b << 0));
						}
					}
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToRGBA8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4), 4);
	if(*tplbuf == NULL)
		return -1;
	u32 i;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4) * 4;
	i = 0;
	int z = 0;
	u32 rgba[32];
	u32 la[32];
	u32 lr[32];
	u32 lg[32];
	u32 lb[32];
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					if(x >= width) {
						//rgba[z] = 0;
						rgba[z] = rgba[z-1];
					}else if(y >= height) {
						rgba[z] = rgba[z-4];
					}else{
						rgba[z] = ((u32*)subbitmapdata)[x + (y * width)];
					}
					lr[z] = (rgba[z] >> 16)  & 0xFF;
					lg[z] = (rgba[z] >> 8)  & 0xFF;
					lb[z] = (rgba[z] >> 0) & 0xFF;
					la[z] = (rgba[z] >> 24)  & 0xFF;
					z++;
				}
			}
			if(z == 16) {	// Dump time!
				for(i = 0; i < 16; i++) {
					writebuf[iv++] = la[i];
					writebuf[iv++] = lr[i];
				}
				for(i = 0; i < 16; i++) {
					writebuf[iv++] = lg[i];
					writebuf[iv++] = lb[i];
				}
				z = 0;
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv, outsz);
	fflush(stdout);
	return outsz;
}


int TPL_ConvertRGB5A3ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((iv++) * 2)));
					if((x >= width) || (y >= height))
						continue;
					oldpixel = be16(oldpixel);
					if(oldpixel & (1 << 15)) {
						// RGB5
						u8 b = (((oldpixel >> 10) & 0x1F) * 255) / 31;
						u8 g = (((oldpixel >> 5)  & 0x1F) * 255) / 31;
						u8 r = (((oldpixel >> 0)  & 0x1F) * 255) / 31;
						u8 a = 255;
						u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
						(*(u32**)bitmapdata)[x + (y * width)] = rgba;
					}else{
						// RGB4A3
						u8 a = (((oldpixel >> 12) & 0x7) * 255) / 7;
						u8 r = (((oldpixel >> 8)  & 0xF) * 255) / 15;
						u8 g = (((oldpixel >> 4)  & 0xF) * 255) / 15;
						u8 b = (((oldpixel >> 0)  & 0xF) * 255) / 15;
						u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
						(*(u32**)bitmapdata)[x + (y * width)] = rgba;
					}
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToRGB5A3(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4), 2);
	if(*tplbuf == NULL)
		return -1;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4) * 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 newpixel = 0;
					if((x >= width) || (y >= height))
						newpixel = 0;
					else {
						u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
						u8 r = (rgba >> 16)  & 0xFF;
						u8 g = (rgba >> 8)  & 0xFF;
						u8 b = (rgba >> 0) & 0xFF;
						u8 a = (rgba >> 24)  & 0xFF;
						if(a <= 0xDA) {
							// We have enough Alpha to matter. Lets use RGB4A3!
							newpixel &= ~(1 << 15);
							r = ((r * 15) / 255) & 0xF;
							g = ((g * 15) / 255) & 0xF;
							b = ((b * 15) / 255) & 0xF;
							a = ((a * 7)  / 255) & 0x7;
							newpixel |= a << 12;
							newpixel |= r << 8;
							newpixel |= g << 4;
							newpixel |= b << 0;
						}else{
							// We don't have enough Alpha to matter. Use RGB5.
							newpixel |= (1 << 15);
							r = ((r * 31) / 255) & 0x1F;
							g = ((g * 31) / 255) & 0x1F;
							b = ((b * 31) / 255) & 0x1F;
							newpixel |= r << 10;
							newpixel |= g << 5;
							newpixel |= b << 0;
						}
					}
					((u16*)writebuf)[iv++] = be16(newpixel);
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
}

int TPL_ConvertI4ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 8);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = align(width, 4) * align(height, 4) * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 8) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 8); y++) {
				for(x = x1; x < (x1 + 8); x += 2, iv++) {
					if((x >= width) || (y >= height))
						continue;
					u8 oldpixel = tplbuf[tplpoint + iv];
					u8 b = (oldpixel >> 4) * 255 / 15;
					u8 g = (oldpixel >> 4) * 255 / 15;
					u8 r = (oldpixel >> 4) * 255 / 15;
					u8 a = 255;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					(*(u32**)bitmapdata)[x + (y * width)] = rgba;
					b = (oldpixel & 0xF) * 255 / 15;
					g = (oldpixel & 0xF) * 255 / 15;
					r = (oldpixel & 0xF) * 255 / 15;
					a = 255;
					rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					(*(u32**)bitmapdata)[x + 1 + (y * width)] = rgba;
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToI4(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4) / 2, 1);
	if(*tplbuf == NULL)
		return -1;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4) / 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 8) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 8); y++) {
				for(x = x1; x < (x1 + 8); x += 2) {
					u8 newpixel;
					if((x >= width) || (y >= height))
						newpixel = 0;
					else {
						u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
						u8 i1 = (rgba >> 0) & 0xFF;
						rgba = ((u32*)subbitmapdata)[x + 1 + (y * width)];
						u8 i2 = (rgba >> 0) & 0xFF;
						
						newpixel = (((i1 * 15) / 255) << 4);
						newpixel |= (((i2 * 15) / 255) & 0xF);
					}
					((u8*)writebuf)[iv++] = newpixel;
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv, outsz);
	fflush(stdout);
	return outsz;
}

int TPL_ConvertIA4ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u8 oldpixel = *(u8*)(tplbuf + (tplpoint + ((iv++))));
					oldpixel = ~oldpixel;
					if((x >= width) || (y >= height))
						continue;
					u8 b = ((oldpixel & 0xF) * 255) / 15;
					u8 g = ((oldpixel & 0xF) * 255) / 15;
					u8 r = ((oldpixel & 0xF) * 255) / 15;
					u8 a = ((oldpixel >> 4)  * 255) / 15;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					(*(u32**)bitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToIA4(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4), 1);
	if(*tplbuf == NULL)
		return -1;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4);
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u8 newpixel;
					if((x >= width) || (y >= height))
						newpixel = 0;
					else {
						u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
						u8 i1 = (rgba >> 0)  & 0xFF;
						u8 a1 = (rgba >> 24) & 0xFF;
						
						newpixel = (((i1 * 15) / 255) & 0xF);
						newpixel |= (((a1 * 15) / 255) << 4);
					}
					((u8*)writebuf)[iv++] = newpixel;
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv, outsz);
	fflush(stdout);
	return outsz;
}

int TPL_ConvertI8ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u8 oldpixel = *(u8*)(tplbuf + (tplpoint + ((iv++) * 1)));
					if((x >= width) || (y >= height))
						continue;
					u8 b = oldpixel;
					u8 g = oldpixel;
					u8 r = oldpixel;
					u8 a = 255;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					(*(u32**)bitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToI8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4), 1);
	if(*tplbuf == NULL)
		return -1;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4);
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 i1;
					if((x >= width) || (y >= height))
						i1 = 0;
					else
						i1 = (rgba >> 0) & 0xFF;
					
					((u8*)writebuf)[iv++] = i1;
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv, outsz);
	fflush(stdout);
	return outsz;
}

int TPL_ConvertIA8ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((iv++) * 2)));
					if((x >= width) || (y >= height))
						continue;
					u8 b = oldpixel >> 8;
					u8 g = oldpixel >> 8;
					u8 r = oldpixel >> 8;
					u8 a = oldpixel & 0xFF;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					(*(u32**)bitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	return outsz;
}

int TPL_ConvertBitMapToIA8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(align(width, 4) * align(height, 4), 2);
	if(*tplbuf == NULL)
		return -1;
	u8* writebuf = *tplbuf;
	u32 outsz = align(width, 4) * align(height, 4) * 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 newpixel;
					if((x >= width) || (y >= height))
						newpixel = 0;
					else{
						u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
						u8 i1 = (rgba >> 0)  & 0xFF;
						u8 a1 = (rgba >> 24) & 0xFF;
						
						newpixel = i1 << 8;
						newpixel |= a1;
					}
					((u16*)writebuf)[iv++] = newpixel;
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
}


/* This section of code was taken from Segher's Wii.git @ http://git.infradead.org/users/segher/wii.git?a=blob_plain;f=tpl2ppm.c;hb=HEAD

 Copyright 2007,2008  Segher Boessenkool  <segher@kernel.crashing.org>
 Licensed under the terms of the GNU GPL, version 2
 http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

*/ 

#define round_up(x,n) (-(-(x) & -(n)))

static u16 avg(u16 w0, u16 w1, u16 c0, u16 c1)
{
	u16 a0, a1;
	u16 a, c;
	
	a0 = c0 >> 11;
	a1 = c1 >> 11;
	a = (w0*a0 + w1*a1) / (w0 + w1);
	c = a << 11;
	
	a0 = (c0 >> 5) & 63;
	a1 = (c1 >> 5) & 63;
	a = (w0*a0 + w1*a1) / (w0 + w1);
	c |= a << 5;
	
	a0 = c0 & 31;
	a1 = c1 & 31;
	a = (w0*a0 + w1*a1) / (w0 + w1);
	c |= a;
	
	return c;
}

int TPL_ConvertCMPToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	u32 iv = 0;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) {
			u8 pix[3];
			u16 raw;
			u16 c[4];
			int x0, x1, x2, y0, y1, y2, off;
			int ww = round_up(width, 8);
			int ix;
			u32 px;
			
			x0 = x & 3;
			x1 = (x >> 2) & 1;
			x2 = x >> 3;
			y0 = y & 3;
			y1 = (y >> 2) & 1;
			y2 = y >> 3;
			off = (8 * x1) + (16 * y1) + (32 * x2) + (4 * ww * y2);
			
			c[0] = be16(*(u16*)(tplbuf + tplpoint + off));
			c[1] = be16(*(u16*)(tplbuf + tplpoint + off + 2));
			if (c[0] > c[1]) {
				c[2] = avg(2, 1, c[0], c[1]);
				c[3] = avg(1, 2, c[0], c[1]);
			} else {
				c[2] = avg(1, 1, c[0], c[1]);
				c[3] = 0;
			}
			
			px = be32(*(u32*)(tplbuf + tplpoint + off + 4));
			ix = x0 + (4 * y0);
			raw = c[(px >> (30 - (2 * ix))) & 3];
			
			pix[0] = (raw >> 8) & 0xf8;
			pix[1] = (raw >> 3) & 0xf8;
			pix[2] = (raw << 3) & 0xf8;
			
			u32 rgba = (pix[0] << 0) | (pix[1] << 8) | (pix[2] << 16) | (255 << 24);
			(*(u32**)bitmapdata)[iv++] = rgba;
		}
	return outsz;
}

/* End Segher's stuff. */

int TPL_ConvertToGD(u8* tplbuf, u32 tplsize, char basename[], u32 format)
{
	char formatstr[4];
	switch(format) {
		case 0:		// BMP
			return TPL_ConvertToBMP(tplbuf, tplsize, basename);
		case 1:		// PNG
			strcpy(formatstr, "png");
			break;
		default:
			printf("Unknown output format %d.\n", format);
			return -1;
	}
	u32 tplpoint = 0;
	u32 header[3];
	u32 goodmagic = be32(0x0020af30);
	memcpy(header, tplbuf + tplpoint, 12);
	tplpoint += 12;
	if(memcmp(&(header[0]), &goodmagic, 4) != 0) {
		printf("Not a valid TPL file. %08x\n", header[0]);
		return -1;
	}
	if(header[2] != be32(0x0000000c)) {
		printf("TPL Header is malformed. %08x\n", header[2]);
		return -1;
	}
	u32 TPLTextures[256][2];				// Who's gonna use more than 256 textures per TPL? Nobody.
	u32 texcount = 0;
	int i;
	for(i = 0; i < be32(header[1]); i++) {
		memcpy(TPLTextures[i], tplbuf + tplpoint, 8);
		tplpoint += 8;
		texcount++;
	}
	char *outname = malloc(strlen(basename) + 8);
	for(i = 0; i < texcount; i++) {
		printf("Converting file %d.\n", i);
		memset(outname, 0, strlen(basename) + 8);
		sprintf(outname, "%s%02x.%s", basename, i, formatstr);
		printf("Output to %s.\n", outname);
		tplpoint = be32(TPLTextures[i][0]);
		TPL_header h;
		memcpy(&h.height, tplbuf + tplpoint, 2);
		tplpoint += 2;
		memcpy(&h.width, tplbuf + tplpoint, 2);
		tplpoint += 2;
		memcpy(&h.format, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.offs, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.wrap_s, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.wrap_t, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.min, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.mag, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.lod_bias, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.edge_lod, tplbuf + tplpoint, 1);
		tplpoint++;
		memcpy(&h.min_lod, tplbuf + tplpoint, 1);
		tplpoint++;
		memcpy(&h.max_lod, tplbuf + tplpoint, 1);
		tplpoint++;
		memcpy(&h.unpacked, tplbuf + tplpoint, 1);
		tplpoint++;
		printf("Dimensions: %dx%d\n", be16(h.width), be16(h.height));
		gdImagePtr im = gdImageCreateTrueColor(be16(h.width), be16(h.height));
		gdImageAlphaBlending(im, 0);
		gdImageSaveAlpha(im, 1);
		FILE *fileout = fopen(outname, "wb");
		tplpoint = be32(h.offs);
		int ret = 0;
		u8* bitmapdata;
		printf("Format: ");
		switch(be32(h.format)) {
			case TPL_FORMAT_RGB565:
				printf("RGB565\n");
				ret = TPL_ConvertRGB565ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_RGB5A3:
				printf("RGB5A3\n");
				ret = TPL_ConvertRGB5A3ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_RGBA8:
				printf("RGBA8\n");
				ret = TPL_ConvertRGBA8ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_I4:
				printf("I4\n");
				ret = TPL_ConvertI4ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_I8:
				printf("I8\n");
				ret = TPL_ConvertI8ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_IA4:
				printf("IA4\n");
				ret = TPL_ConvertIA4ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_IA8:
				printf("IA8\n");
				ret = TPL_ConvertIA8ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			case TPL_FORMAT_CMP:
				printf("CMP\n");
				ret = TPL_ConvertCMPToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, be16(h.width), be16(h.height));
				break;
			default:
				printf("Unsupported: %d.\n", be32(h.format));
				ret = -1;
		}
		if(ret == -1) {
			printf("Error converting file.\n");
			gdImageDestroy(im);
			return -1;
		}
		int x, y;
		for(y = 0; y < be16(h.height); y++) {
			for(x = 0; x < be16(h.width); x++) {
				u32 rgba = ((u32*)bitmapdata)[x + (y * be16(h.width))];
				rgba = be32(rgba);
				u8 r = (rgba >> 8)  & 0xFF;
				u8 g = (rgba >> 16) & 0xFF;
				u8 b = (rgba >> 24) & 0xFF;
				u8 a = (255 - ((rgba >> 0) & 0xFF)) / 2;
				int clr = gdTrueColorAlpha(r, g, b, a);
				gdImageSetPixel(im, x, y, clr);
			}
		}
		gdImagePng(im, fileout);
		fclose(fileout);
		memset(outname, 0, strlen(basename) + 8);
		sprintf(outname, "%s%02x.%s", basename, i, "met");
		FILE* tplmeta = fopen(outname, "wb");
		char tplheadstartmagic[] = {'T', 'P', 'L', 'M'};
		fwrite(tplheadstartmagic, 4, 1, tplmeta);
		fwrite(&h.height,         2, 1, tplmeta);
		fwrite(&h.width,          2, 1, tplmeta);
		fwrite(&h.format,         4, 1, tplmeta);
		fwrite(&h.offs,           4, 1, tplmeta);
		fwrite(&h.wrap_s,         4, 1, tplmeta);
		fwrite(&h.wrap_t,         4, 1, tplmeta);
		fwrite(&h.min,            4, 1, tplmeta);
		fwrite(&h.mag,            4, 1, tplmeta);
		fwrite(&h.lod_bias,       4, 1, tplmeta);
		fwrite(&h.edge_lod,       1, 1, tplmeta);
		fwrite(&h.min_lod,        1, 1, tplmeta);
		fwrite(&h.max_lod,        1, 1, tplmeta);
		fwrite(&h.unpacked,       1, 1, tplmeta);
		fclose(tplmeta);
		free(bitmapdata);
		gdImageDestroy(im);
	}
	free(outname);
	return 0;
}

int TPL_ConvertFromGDs(const u32 count, char *gds[], char outname[], u32 format, u32 typeformat)
{
	switch(typeformat) {
		case 0:		// BMP
			return TPL_ConvertFromBMPs(count, gds, outname, format);
		case 1:		// PNG
			break;
	}
	FILE *out = fopen(outname, "wb");
	if(out == NULL) {
		printf("Unable to open %s\n", outname);
		return -1;
	}
	fflush(stdout);
	u32 magic = be32(0x0020af30);
	u32 header[] = { magic, be32(count), be32(0xc) };
	fwrite(header, 4, 3, out);
	
	int i;
	for(i = 0; i < count; i++) {
		u32 offsets[] = { be32(12 + (8 * count) + (36 * i)), be32(0) };
		fwrite(offsets, 4, 2, out);
	}
	u8* imagedatas[256];
	u32 imagesizes[256];
	u32 curoff = 0;
	for(i = 0; i < count; i++) {
		printf("Handling %d/%d\n", i + 1, count);
		FILE* gdfp = fopen(gds[i], "rb");
		gdImagePtr im;
		switch(typeformat) {
			case 0:		// BMP
				return TPL_ConvertFromBMPs(count, gds, outname, format);
			case 1:		// PNG
				im = gdImageCreateFromPng(gdfp);
				break;
		}
		gdImageAlphaBlending(im, 0);
		gdImageSaveAlpha(im, 1);
		fclose(gdfp);
		u32 width = gdImageSX(im);
		u32 height = gdImageSY(im);
		u32 bitmapsize = width * height * 4;
		u8* bitmapdata = (u8*)malloc(width * height * 4);
		memset(bitmapdata, 0, width * height * 4);
		u8* tplbuf;
		u32 x, y;
		for(y = 0; y < height; y++) {
			for(x = 0; x < width; x++) {
				u32 p;
				if(x >= width || y >= height)
					p = 0;
				else
					p = gdImageGetPixel(im, x, y);
				u8 a = 254 - (2 * ((u8)gdImageAlpha(im, p)));
				if(a == 254) a++;
				u8 r = (u8)gdImageRed(im, p);
				u8 g = (u8)gdImageGreen(im, p);
				u8 b = (u8)gdImageBlue(im, p);
				u32 rgba = (r << 8) | (g << 16) | (b << 24) | (a << 0);
				rgba = be32(rgba);
				((u32*)bitmapdata)[x + (y * width)] = rgba;
			}
		}
		int ret;
		switch(format) {
			case TPL_FORMAT_RGB565:
				printf("RGB565\n");
				ret = TPL_ConvertBitMapToRGB565(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_RGB5A3:
				printf("RGB5A3\n");
				ret = TPL_ConvertBitMapToRGB5A3(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_RGBA8:
				printf("RGBA8\n");
				ret = TPL_ConvertBitMapToRGBA8(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_I4:
				printf("I4\n");
				ret = TPL_ConvertBitMapToI4(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_I8:
				printf("I8\n");
				ret = TPL_ConvertBitMapToI8(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_IA4:
				printf("IA4\n");
				ret = TPL_ConvertBitMapToIA4(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_IA8:
				printf("IA8\n");
				ret = TPL_ConvertBitMapToIA8(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_CMP:
				printf("CMP\n");
				printf("CMP is not supported as a writable format.\nPlease choose a different format.\n");
				return -1;
			default:
				printf("Unsupported: %d.\n", format);
				ret = -1;
		}
		if(ret == -1) {
			printf("Error converting file.\n");
			return -1;
		}
		imagedatas[i] = tplbuf;
		imagesizes[i] = ret;
		TPL_header h;
		fflush(stdout);
		memset(&h, 0, sizeof(TPL_header));
		fflush(stdout);
		h.format = be32(format);
		//h.offs = be32(12 + (8 * count) + (36 * count) + curoff);
		u32 temp_off = 12 + (8*count) + (36*count) + curoff;
		u32 mod = temp_off % 0x20;
		if(mod)
			mod = 0x20-mod;
		h.offs = be32(temp_off + mod);
		curoff += ret;
		printf("Dimensions: %dx%d\n", width, height);
		h.width = be16(width);
		h.height = be16(height);
		h.min = be32(TPL_MIN_FILTER_CLAMP);
		h.mag = be32(TPL_MAG_FILTER_CLAMP);
		fwrite(&h.height, 2, 1, out);
		fwrite(&h.width, 2, 1, out);
		fwrite(&h.format, 4, 1, out);
		fwrite(&h.offs, 4, 1, out);
		fwrite(&h.wrap_s, 4, 1, out);
		fwrite(&h.wrap_t, 4, 1, out);
		fwrite(&h.min, 4, 1, out);
		fwrite(&h.mag, 4, 1, out);
		fwrite(&h.lod_bias, 4, 1, out);
		fwrite(&h.edge_lod, 1, 1, out);
		fwrite(&h.min_lod, 1, 1, out);
		fwrite(&h.max_lod, 1, 1, out);
		fwrite(&h.unpacked, 1, 1, out);
		fflush(out);
	}
	// ADD PADDING TO 0x20 BYTE ALIGNMENT
	int off = ftell(out);
	off = off % 0x20;
	printf("out off: %d\n", off);
	if(off) {
		char pad[0x20];
		memset(pad, 0, 0x20);
		fwrite(pad, 1, 0x20-off, out);
	}
	for(i = 0; i < count; i++) {
		fflush(stdout);
		fwrite(imagedatas[i], imagesizes[i], 1, out);
		fflush(out);
		free(imagedatas[i]);
	}
	fclose(out);
	return 0;
}

int TPL_ConvertToBMP(u8* tplbuf, u32 tplsize, char basename[])
{
	u32 tplpoint = 0;
	u32 header[3];
	u32 goodmagic = be32(0x0020af30);
	memcpy(header, tplbuf + tplpoint, 12);
	tplpoint += 12;
	if(memcmp(&(header[0]), &goodmagic, 4) != 0) {
		printf("Not a valid TPL file. %08x\n", header[0]);
		return -1;
	}
	if(header[2] != be32(0x0000000c)) {
		printf("TPL Header is malformed. %08x\n", header[2]);
		return -1;
	}
	u32 TPLTextures[256][2];				// Who's gonna use more than 256 textures per TPL? Nobody.
	u32 texcount = 0;
	int i;
	for(i = 0; i < be32(header[1]); i++) {
		memcpy(TPLTextures[i], tplbuf + tplpoint, 8);
		tplpoint += 8;
		texcount++;
	}
	char *outname = malloc(strlen(basename) + 8);
	for(i = 0; i < texcount; i++) {
		printf("Converting file %d.\n", i);
		memset(outname, 0, strlen(basename) + 8);
		sprintf(outname, "%s%02x.bmp", basename, i);
		printf("Output to %s.\n", outname);
		tplpoint = be32(TPLTextures[i][0]);
		TPL_header h;
		memcpy(&h.height, tplbuf + tplpoint, 2);
		tplpoint += 2;
		memcpy(&h.width, tplbuf + tplpoint, 2);
		tplpoint += 2;
		memcpy(&h.format, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.offs, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.wrap_s, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.wrap_t, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.min, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.mag, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.lod_bias, tplbuf + tplpoint, 4);
		tplpoint += 4;
		memcpy(&h.edge_lod, tplbuf + tplpoint, 1);
		tplpoint++;
		memcpy(&h.min_lod, tplbuf + tplpoint, 1);
		tplpoint++;
		memcpy(&h.max_lod, tplbuf + tplpoint, 1);
		tplpoint++;
		memcpy(&h.unpacked, tplbuf + tplpoint, 1);
		tplpoint++;
		printf("Dimensions: %dx%d\n", be16(h.width), be16(h.height));
		TPL_BMPHeader b;
		memset(&b, 0, sizeof(TPL_BMPHeader));
		TPL_BMPInfoHeaderV3 bi;
		memset(&bi, 0, sizeof(TPL_BMPInfoHeaderV3));
		b.magic			= 0x4D42;
		b.size			= 0;		// Unset.
		b.reserved1		= 0;
		b.reserved2		= 0;
		b.offset		= 14 + 40;
		bi.headersize		= 40;
		bi.width		= be16(h.width);
		bi.height		= be16(h.height);
		bi.colorplanes		= 1;
		bi.bpp			= 32;
		bi.compression		= 0;
		bi.bitmapsize		= 0;		// Unset.
		bi.hres			= 0x130B;
		bi.vres			= 0x130B;
		bi.colorpalette		= 0;
		bi.importantcolors	= 0;
		tplpoint = be32(h.offs);
		int ret = 0;
		u8* bitmapdata;
		printf("Format: ");
		switch(be32(h.format)) {
			case TPL_FORMAT_RGB565:
				printf("RGB565\n");
				ret = TPL_ConvertRGB565ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_RGB5A3:
				printf("RGB5A3\n");
				ret = TPL_ConvertRGB5A3ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_RGBA8:
				printf("RGBA8\n");
				ret = TPL_ConvertRGBA8ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_I4:
				printf("I4\n");
				ret = TPL_ConvertI4ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_I8:
				printf("I8\n");
				ret = TPL_ConvertI8ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_IA4:
				printf("IA4\n");
				ret = TPL_ConvertIA4ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_IA8:
				printf("IA8\n");
				ret = TPL_ConvertIA8ToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			case TPL_FORMAT_CMP:
				printf("CMP\n");
				ret = TPL_ConvertCMPToBitMap(tplbuf, tplsize, tplpoint, &bitmapdata, bi.width, bi.height);
				break;
			default:
				printf("Unsupported: %d.\n", be32(h.format));
				ret = -1;
		}
		if(ret == -1) {
			printf("Error converting file.\n");
			return -1;
		}
		u8* subbitmapdata = (u8*)malloc(ret);
		u32 iv, x, y;
		for(iv = bi.height - 1, y = 0; y < bi.height; y++, iv--)
			for(x = 0; x < bi.width; x++)
				((u32*)subbitmapdata)[x + (y * bi.width)] = ((u32*)bitmapdata)[x + (iv * bi.width)];
		free(bitmapdata);
		bi.bitmapsize = ret;
		b.size = b.offset + bi.bitmapsize + 38;
		FILE* out = fopen(outname, "wb");
		fwrite(&(b.magic), 2, 1, out);
		fwrite(&(b.size), 4, 1, out);
		fwrite(&(b.reserved1), 2, 1, out);
		fwrite(&(b.reserved2), 2, 1, out);
		fwrite(&(b.offset), 4, 1, out);
		
		fwrite(&(bi.headersize), 4, 1, out);
		fwrite(&(bi.width), 4, 1, out);
		fwrite(&(bi.height), 4, 1, out);
		fwrite(&(bi.colorplanes), 2, 1, out);
		fwrite(&(bi.bpp), 2, 1, out);
		fwrite(&(bi.compression), 4, 1, out);
		fwrite(&(bi.bitmapsize), 4, 1, out);
		fwrite(&(bi.hres), 4, 1, out);
		fwrite(&(bi.vres), 4, 1, out);
		fwrite(&(bi.colorpalette), 4, 1, out);
		fwrite(&(bi.importantcolors), 4, 1, out);
		fwrite(subbitmapdata, bi.bitmapsize, 1, out);
		char tplheadstartmagic[] = {'T', 'P', 'L', 'M'};
		fwrite(tplheadstartmagic, 4, 1, out);
		fwrite(&h.height, 2, 1, out);
		fwrite(&h.width, 2, 1, out);
		fwrite(&h.format, 4, 1, out);
		fwrite(&h.offs, 4, 1, out);
		fwrite(&h.wrap_s, 4, 1, out);
		fwrite(&h.wrap_t, 4, 1, out);
		fwrite(&h.min, 4, 1, out);
		fwrite(&h.mag, 4, 1, out);
		fwrite(&h.lod_bias, 4, 1, out);
		fwrite(&h.edge_lod, 1, 1, out);
		fwrite(&h.min_lod, 1, 1, out);
		fwrite(&h.max_lod, 1, 1, out);
		fwrite(&h.unpacked, 1, 1, out);
		fclose(out);
		free(subbitmapdata);
	}
	free(outname);
	return 0;
}

int TPL_ConvertFromBMPs(const u32 count, char *bmps[], char outname[], u32 format)
{
	FILE *out = fopen(outname, "wb");
	if(out == NULL) {
		printf("Unable to open %s\n", outname);
		return -1;
	}
	fflush(stdout);
	u32 magic = be32(0x0020af30);
	u32 header[] = { magic, be32(count), be32(0xc) };
	fwrite(header, 4, 3, out);
	
	int i;
	for(i = 0; i < count; i++) {
		u32 offsets[] = { be32(12 + (8 * count) + (36 * i)), be32(0) };
		fwrite(offsets, 4, 2, out);
	}
	u8* imagedatas[256];
	u32 imagesizes[256];
	u32 curoff = 0;
	for(i = 0; i < count; i++) {
		printf("Handling %d/%d\n", i + 1, count);
		FILE* bmpfp = fopen(bmps[i], "rb");
		fseek(bmpfp, 0, SEEK_END);
		fseek(bmpfp, 0, SEEK_SET);
		TPL_BMPHeader b;
		fread(&b.magic, 2, 1, bmpfp);
		fread(&b.size, 4, 1, bmpfp);
		fread(&b.reserved1, 2, 1, bmpfp);
		fread(&b.reserved2, 2, 1, bmpfp);
		fread(&b.offset, 4, 1, bmpfp);
		TPL_BMPInfoHeaderV3 bi;
		fread(&bi.headersize, 4, 1, bmpfp);
		if(bi.headersize != 40) {
			printf("Bad BMPInfoV3 header size in file %s! 0x%08x 0x%08x\n", \
			       bmps[i], \
			       bi.headersize, 40);
			return -1;
		}
		fread(&bi.width, 4, 1, bmpfp);
		if((bi.width % 4) != 0) {
			printf("Width must be a multiple of 4 in file %s! 0x%08x\n", \
			       bmps[i], \
			       bi.width);
			return -1;
		}
		fread(&bi.height, 4, 1, bmpfp);
		if((bi.height % 4) != 0) {
			printf("Height must be a multiple of 4 in file %s! 0x%08x\n", \
			       bmps[i], \
			       bi.height);
			return -1;
		}
		fread(&bi.colorplanes, 2, 1, bmpfp);
		if(bi.colorplanes != 1) {
			printf("Wrong number of colorplanes in file %s! 0x%08x\n", \
			       bmps[i], \
			       bi.colorplanes);
			return -1;
		}
		fread(&bi.bpp, 2, 1, bmpfp);
		if(bi.bpp != 32) {
			printf("This version of Zetsubou only supports RGBA8 in file %s! 0x%08x\n", \
			       bmps[i], \
			       bi.bpp);
			return -1;
		}
		fread(&bi.compression, 4, 1, bmpfp);
		if(bi.compression != 0) {
			printf("No support for compression in file %s! 0x%08x\n", \
			       bmps[i], \
			       bi.compression);
			return -1;
		}
		fread(&bi.bitmapsize, 4, 1, bmpfp);
		fread(&bi.hres, 4, 1, bmpfp);
		fread(&bi.vres, 4, 1, bmpfp);
		fread(&bi.colorpalette, 4, 1, bmpfp);
		fread(&bi.importantcolors, 4, 1, bmpfp);
		fseek(bmpfp, b.offset, SEEK_SET);
		u8* bitmapdata = (u8*)malloc(bi.bitmapsize);
		u8* subbitmapdata = (u8*)malloc(bi.bitmapsize);
		fread(subbitmapdata, bi.bitmapsize, 1, bmpfp);
		u8* tplbuf;
		u32 iv, x, y, z;
		for(iv = bi.height - 1, y = 0; y < bi.height; y++, iv--)
			for(x = 0; x < bi.width; x++)
				for(z = 0; z < 4; z++)
					bitmapdata[((x + (y * bi.width)) * 4) + z] = subbitmapdata[((x + (iv * bi.width)) * 4) + z];
		free(subbitmapdata);
		int ret;
		switch(format) {
			case TPL_FORMAT_RGB565:
				printf("RGB565\n");
				ret = TPL_ConvertBitMapToRGB565(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_RGB5A3:
				printf("RGB5A3\n");
				ret = TPL_ConvertBitMapToRGB5A3(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_RGBA8:
				printf("RGBA8\n");
				ret = TPL_ConvertBitMapToRGBA8(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_I4:
				printf("I4\n");
				ret = TPL_ConvertBitMapToI4(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_I8:
				printf("I8\n");
				ret = TPL_ConvertBitMapToI8(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_IA4:
				printf("IA4\n");
				ret = TPL_ConvertBitMapToIA4(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_IA8:
				printf("IA8\n");
				ret = TPL_ConvertBitMapToIA8(bitmapdata, bi.bitmapsize, &tplbuf, bi.width, bi.height);
				break;
			case TPL_FORMAT_CMP:
				printf("CMP\n");
				printf("CMP is not supported as a writable format.\nPlease choose a different format.\n");
				return -1;
			default:
				printf("Unsupported: %d.\n", format);
				ret = -1;
		}
		if(ret == -1) {
			printf("Error converting file.\n");
			return -1;
		}
		imagedatas[i] = tplbuf;
		imagesizes[i] = ret;
		TPL_header h;
		fflush(stdout);
		memset(&h, 0, sizeof(TPL_header));
		fflush(stdout);
		h.format = be32(format);
		//h.offs = be32(12 + (8 * count) + (36 * count) + curoff);
		u32 temp_off = 12 + (8*count) + (36*count) + curoff;
		u32 mod = temp_off % 0x20;
		if(mod)
			mod = 0x20-mod;
		h.offs = be32(temp_off + mod);
		curoff += ret;
		printf("Dimensions: %dx%d\n", bi.width, bi.height);
		h.width = be16(bi.width);
		h.height = be16(bi.height);
		h.min = be32(TPL_MIN_FILTER_CLAMP);
		h.mag = be32(TPL_MAG_FILTER_CLAMP);
		fwrite(&h.height, 2, 1, out);
		fwrite(&h.width, 2, 1, out);
		fwrite(&h.format, 4, 1, out);
		fwrite(&h.offs, 4, 1, out);
		fwrite(&h.wrap_s, 4, 1, out);
		fwrite(&h.wrap_t, 4, 1, out);
		fwrite(&h.min, 4, 1, out);
		fwrite(&h.mag, 4, 1, out);
		fwrite(&h.lod_bias, 4, 1, out);
		fwrite(&h.edge_lod, 1, 1, out);
		fwrite(&h.min_lod, 1, 1, out);
		fwrite(&h.max_lod, 1, 1, out);
		fwrite(&h.unpacked, 1, 1, out);
		fflush(stdout);
		fflush(out);
		fflush(bmpfp);
		fclose(bmpfp);
		fflush(stdout);
	}
	// ADD PADDING TO 0x20 BYTE ALIGNMENT
	int off = ftell(out);
	off = off % 0x20;
	printf("out off: %d\n", off);
	if(off) {
		char pad[0x20];
		memset(pad, 0, 0x20);
		fwrite(pad, 1, 0x20-off, out);
	}
	for(i = 0; i < count; i++) {
		fflush(stdout);
		fwrite(imagedatas[i], imagesizes[i], 1, out);
		fflush(out);
		free(imagedatas[i]);
	}
	fclose(out);
	return 0;
}



