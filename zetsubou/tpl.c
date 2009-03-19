/*
 *  tpl.c
 *  Zetsubou
 *
 *  Created by Alex Marshall on 09/03/17.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tpl.h"
#include "endian.h"

int TPL_ConvertRGB565ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	s32 iv;
	//tplpoint -= width;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	if(subbitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((iv++) * 2)));
					oldpixel = be16(oldpixel);
					u8 b = ((oldpixel >> 11) & 0x1F) << 3;
					u8 g = ((oldpixel >> 5)  & 0x3F) << 2;
					u8 r = ((oldpixel >> 0)  & 0x1F) << 3;
					u8 a = 255;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					((u32*)subbitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			*((*(u32**)bitmapdata) + ((x + (y * width)))) = ((u32*)subbitmapdata)[x + (iv * width)];
	free(subbitmapdata);
	return outsz;
}

int TPL_ConvertBitMapToRGB565(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
//	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(width * height, 2);
	if(*tplbuf == NULL)
		return -1;
	int i;
	u8* writebuf = *tplbuf;
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			for(i = 0; i < 4; i++)
				subbitmapdata[((x + (y * width)) * 4) + i] = bitmapdata[((x + (iv * width)) * 4) + i];
	u32 outsz = width * height * 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 r = (rgba >> 0)  & 0xFF;
					u8 g = (rgba >> 8)  & 0xFF;
					u8 b = (rgba >> 16) & 0xFF;
//					u8 a = (rgba >> 24)  & 0xFF;

					u16 newpixel = ((b >> 3) << 11) | ((g >> 2) << 5) | ((r >> 3) << 0);
					((u16*)writebuf)[iv++] = be16(newpixel);
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
}

int TPL_ConvertRGBA8ToBitMap(u8* tplbuf, u32 tplsize, u32 tplpoint, u8** bitmapdata, u32 width, u32 height)
{
	u32 goodwidth = width;
	u32 goodheight = height;
	u32 x, y;
	u32 x1, y1;
	*bitmapdata = (u8*)calloc(width * height, 4);
	if(*bitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	u8 ag[32];
	u8 rb[32];
	int i = 0;
	int z = 0;
	int bmpslot = 0;
	for(y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					width = goodwidth;
					height = goodheight;
					fflush(stdout);
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((x + (y * width)) * 2)));
					ag[i] = (oldpixel >> 8) & 0xFF;
					rb[i++] = (oldpixel >> 0) & 0xFF;
				}
			}
			z++;
			if(z == 2) {
				width = goodwidth;
				height = goodheight;
				for(i = 0; i < 8; i++) {
					u32 rgba = (rb[i] << 0) | (ag[8 + i] << 8) | (rb[8 + i] << 16) | (ag[i] << 24);
					(*(u32**)bitmapdata)[bmpslot++] = rgba;
				}
				for(i = 16; i < 24; i++) {
					u32 rgba = (rb[i] << 0) | (ag[8 + i] << 8) | (rb[8 + i] << 16) | (ag[i] << 24);
					(*(u32**)bitmapdata)[bmpslot++] = rgba;
				}
				z = 0;
				i = 0;
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
	*tplbuf = (u8*)calloc(width * height, 4);
	if(*tplbuf == NULL)
		return -1;
	u32 i;
	u8* writebuf = *tplbuf;
	u32 outsz = width * height * 4;
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
					rgba[z] = ((u32*)subbitmapdata)[x + (y * width)];
					lr[z] = (rgba[z] >> 0)  & 0xFF;
					lg[z] = (rgba[z] >> 8)  & 0xFF;
					lb[z] = (rgba[z] >> 16) & 0xFF;
					la[z] = (rgba[z] >> 24)  & 0xFF;
					z++;
				}
			}
			if(z == 16) {	// Dump time!
				for(i = 0; i < 16; i++) {
					writebuf[iv++] = lr[i];
					writebuf[iv++] = la[i];
				}
				for(i = 0; i < 16; i++) {
					writebuf[iv++] = lb[i];
					writebuf[iv++] = lg[i];
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
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	if(subbitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((iv++) * 2)));
					oldpixel = be16(oldpixel);
					if(oldpixel & (1 << 15)) {
						// RGB5
						u8 r = (((oldpixel >> 10) & 0x1F) * 255) / 31;
						u8 g = (((oldpixel >> 5)  & 0x1F) * 255) / 31;
						u8 b = (((oldpixel >> 0)  & 0x1F) * 255) / 31;
						u8 a = 255;
						u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
						((u32*)subbitmapdata)[x + (y * width)] = rgba;
					}else{
						// RGB4A3
						u8 r = (((oldpixel >> 11) & 0xF) * 255) / 15;
						u8 g = (((oldpixel >> 7)  & 0xF) * 255) / 15;
						u8 b = (((oldpixel >> 3)  & 0xF) * 255) / 15;
						u8 a = (((oldpixel >> 0)  & 0x7) * 255) / 7;
						u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
						((u32*)subbitmapdata)[x + (y * width)] = rgba;
					}
				}
			}
		}
	}
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			*((*(u32**)bitmapdata) + ((x + (y * width)))) = ((u32*)subbitmapdata)[x + (iv * width)];
	free(subbitmapdata);	
	return outsz;
}

int TPL_ConvertBitMapToRGB5A3(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	//	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(width * height, 2);
	if(*tplbuf == NULL)
		return -1;
	int i;
	u8* writebuf = *tplbuf;
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			for(i = 0; i < 4; i++)
				subbitmapdata[((x + (y * width)) * 4) + i] = bitmapdata[((x + (iv * width)) * 4) + i];
	u32 outsz = width * height * 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 r = (rgba >> 0)  & 0xFF;
					u8 g = (rgba >> 8)  & 0xFF;
					u8 b = (rgba >> 16) & 0xFF;
					u8 a = (rgba >> 24)  & 0xFF;
					u16 newpixel = 0;
					if(a <= 0xDA) {
						// We have enough Alpha to matter. Lets use RGB4A3!
						newpixel |= (1 << 15);
						r = ((r * 15) / 255) & 0xF;
						g = ((g * 15) / 255) & 0xF;
						b = ((b * 15) / 255) & 0xF;
						a = ((a * 7) / 255)  & 0x7;
						newpixel |= r << 11;
						newpixel |= g << 7;
						newpixel |= b << 3;
						newpixel |= a << 0;
						((u16*)writebuf)[iv++] = be16(newpixel);
					}else{
						// We don't have enough Alpha to matter. Use RGB5.
						newpixel &= ~(1 << 15);
						r = ((r * 31) / 255) & 0x1F;
						g = ((g * 31) / 255) & 0x1F;
						b = ((b * 31) / 255) & 0x1F;
						newpixel |= r << 10;
						newpixel |= g << 5;
						newpixel |= b << 0;
						((u16*)writebuf)[iv++] = be16(newpixel);
					}
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
	u8* subbitmapdata = (u8*)calloc(width * height, 8);
	if(subbitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 8) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 8); y++) {
				for(x = x1; x < (x1 + 8); x += 2) {
					u8 oldpixel = tplbuf[tplpoint + iv];
					u8 b = (oldpixel >> 4) * 255 / 15;
					u8 g = (oldpixel >> 4) * 255 / 15;
					u8 r = (oldpixel >> 4) * 255 / 15;
					u8 a = 255;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					((u32*)subbitmapdata)[x + (y * width)] = rgba;
					b = (oldpixel & 0xF) * 255 / 15;
					g = (oldpixel & 0xF) * 255 / 15;
					r = (oldpixel & 0xF) * 255 / 15;
					a = 255;
					rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					((u32*)subbitmapdata)[x + 1 + (y * width)] = rgba;
					iv++;
				}
			}
		}
	}
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			*((*(u32**)bitmapdata) + ((x + (y * width)))) = ((u32*)subbitmapdata)[x + (iv * width)];
	free(subbitmapdata);
	return outsz;
}

int TPL_ConvertBitMapToI4(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	//	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(width * height / 2, 1);
	if(*tplbuf == NULL)
		return -1;
	int i;
	u8* writebuf = *tplbuf;
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			for(i = 0; i < 4; i++)
				subbitmapdata[((x + (y * width)) * 4) + i] = bitmapdata[((x + (iv * width)) * 4) + i];
	u32 outsz = width * height / 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 8) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 8); y++) {
				for(x = x1; x < (x1 + 8); x += 2) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 i1 = (rgba >> 0) & 0xFF;
					rgba = ((u32*)subbitmapdata)[x + 1 + (y * width)];
					u8 i2 = (rgba >> 0) & 0xFF;
					
					u8 newpixel = (((i1 * 15) / 255) << 4);
					newpixel |= (((i2 * 15) / 255) & 0xF);
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
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	if(subbitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u8 oldpixel = *(u8*)(tplbuf + (tplpoint + ((iv++))));
					u8 b = ((oldpixel >> 4)  * 255) / 15;
					u8 g = ((oldpixel >> 4)  * 255) / 15;
					u8 r = ((oldpixel >> 4)  * 255) / 15;
					u8 a = ((oldpixel & 0xF) * 255) / 15;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					((u32*)subbitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			*((*(u32**)bitmapdata) + ((x + (y * width)))) = ((u32*)subbitmapdata)[x + (iv * width)];
	free(subbitmapdata);
	return outsz;
}

int TPL_ConvertBitMapToIA4(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	//	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(width * height, 1);
	if(*tplbuf == NULL)
		return -1;
	int i;
	u8* writebuf = *tplbuf;
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			for(i = 0; i < 4; i++)
				subbitmapdata[((x + (y * width)) * 4) + i] = bitmapdata[((x + (iv * width)) * 4) + i];
	u32 outsz = width * height;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 i1 = (rgba >> 0)  & 0xFF;
					u8 a1 = (rgba >> 24) & 0xFF;
					
					u8 newpixel = (((i1 * 15) / 255) << 4);
					newpixel |= (((a1 * 15) / 255) & 0xF);
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
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	if(subbitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u8 oldpixel = *(u8*)(tplbuf + (tplpoint + ((iv++) * 1)));
					u8 b = oldpixel;
					u8 g = oldpixel;
					u8 r = oldpixel;
					u8 a = 255;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					((u32*)subbitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			*((*(u32**)bitmapdata) + ((x + (y * width)))) = ((u32*)subbitmapdata)[x + (iv * width)];
	free(subbitmapdata);
	return outsz;
}

int TPL_ConvertBitMapToI8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	//	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(width * height, 1);
	if(*tplbuf == NULL)
		return -1;
	int i;
	u8* writebuf = *tplbuf;
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			for(i = 0; i < 4; i++)
				subbitmapdata[((x + (y * width)) * 4) + i] = bitmapdata[((x + (iv * width)) * 4) + i];
	u32 outsz = width * height;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 8) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 8); x++) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 i1 = (rgba >> 0) & 0xFF;
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
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	if(subbitmapdata == NULL)
		return -1;
	u32 outsz = width * height * 4;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u16 oldpixel = *(u16*)(tplbuf + (tplpoint + ((iv++) * 2)));
					u8 b = oldpixel >> 8;
					u8 g = oldpixel >> 8;
					u8 r = oldpixel >> 8;
					u8 a = oldpixel & 0xFF;
					u32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
					((u32*)subbitmapdata)[x + (y * width)] = rgba;
				}
			}
		}
	}
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			*((*(u32**)bitmapdata) + ((x + (y * width)))) = ((u32*)subbitmapdata)[x + (iv * width)];
	free(subbitmapdata);
	return outsz;
}

int TPL_ConvertBitMapToIA8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
{
	s32 x, y;
	s32 x1, y1;
	u32 iv;
	u8* subbitmapdata = (u8*)calloc(width * height, 4);
	//	u8* subbitmapdata = bitmapdata;
	if(subbitmapdata == NULL)
		return -1;
	*tplbuf = (u8*)calloc(width * height, 2);
	if(*tplbuf == NULL)
		return -1;
	int i;
	u8* writebuf = *tplbuf;
	for(iv = height - 1, y = 0; y < height; y++, iv--)
		for(x = 0; x < width; x++)
			for(i = 0; i < 4; i++)
				subbitmapdata[((x + (y * width)) * 4) + i] = bitmapdata[((x + (iv * width)) * 4) + i];
	u32 outsz = width * height * 2;
	for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u32 rgba = ((u32*)subbitmapdata)[x + (y * width)];
					u8 i1 = (rgba >> 0)  & 0xFF;
					u8 a1 = (rgba >> 24) & 0xFF;
					
					u16 newpixel = i1 << 8;
					newpixel |= a1;
					((u16*)writebuf)[iv++] = newpixel;
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
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
	char *outname = (char*)malloc(strlen(basename) + 3);
	for(i = 0; i < texcount; i++) {
		printf("Converting file %d.\n", i);
		memset(outname, 0, strlen(basename) + 3);
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
		if((be16(h.width) % 4) != 0) {
			printf("TPL width is not a multiple of four! BAD!\n");
			return -1;
		}
		if((be16(h.height) % 4) != 0) {
			printf("TPL height is not a multiple of four! BAD!\n");
			return -1;
		}
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
			default:
				printf("Unsupported: %d.\n", be32(h.format));
				ret = -1;
		}
		if(ret == -1) {
			printf("Error converting file.\n");
			return -1;
		}
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
		fwrite(bitmapdata, bi.bitmapsize, 1, out);
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
	}
	return 0;
}

int TPL_ConvertFromBMPs(const u32 count, char *bmps[], char outname[], u32 format)
{
	FILE *out = fopen(outname, "wb");
	if(out == NULL) {
		printf("Unable to open %s\n", outname);
		return -1;
	}
	printf("%d\n", count);
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
	for(i = 0; i < count; i++) {
		printf("Handling %d/%d\n", i, count);
		FILE* bmpfp = fopen(bmps[i], "rb");
/*		fseek(bmpfp, 38, SEEK_END);
		char goodtplheadstartmagic[] = {'T', 'P', 'L', 'M'};
		char tplheadstartmagic[4];
		fread(tplheadstartmagic, 4, 1, bmpfp);
		if(memcmp(goodtplheadstartmagic, tplheadstartmagic, 4) != 0) {
			printf("Bad TPL Header magic in file %s! %02x%02x%02x%02x\n", \
			       bmps[i], \
			       tplheadstartmagic[0], tplheadstartmagic[1], \
			       tplheadstartmagic[2], tplheadstartmagic[3]);
			return -1;
		}
		TPL_header h;
		fread(&h.height, 2, 1, bmpfp);
		fread(&h.width, 2, 1, bmpfp);
		fread(&h.format, 4, 1, bmpfp);
		fread(&h.offs, 4, 1, bmpfp);
		fread(&h.wrap_s, 4, 1, bmpfp);
		fread(&h.wrap_t, 4, 1, bmpfp);
		fread(&h.min, 4, 1, bmpfp);
		fread(&h.mag, 4, 1, bmpfp);
		fread(&h.lod_bias, 4, 1, bmpfp);
		fread(&h.edge_lod, 1, 1, bmpfp);
		fread(&h.min_lod, 1, 1, bmpfp);
		fread(&h.max_lod, 1, 1, bmpfp);
		fread(&h.unpacked, 1, 1, bmpfp);*/
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
		fread(bitmapdata, bi.bitmapsize, 1, bmpfp);
		u8* tplbuf;
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
		printf("Nulling h\n");
		fflush(stdout);
		memset(&h, 0, sizeof(TPL_header));
		printf("Nulled h\n");
		fflush(stdout);
		h.format = be32(format);
		h.offs = be32(12 + (8 * count) + (36 * count));
		printf("%dx%d\n", bi.width, bi.height);
		h.width = be16(bi.width);
		h.height = be16(bi.height);
		h.min = be32(TPL_MIN_FILTER_NORMAL);
		h.mag = be32(TPL_MAG_FILTER_NORMAL);
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
		printf("Closing bmpfp\n");
		fflush(stdout);
		fflush(out);
		fflush(bmpfp);
		fclose(bmpfp);
		printf("Closed bmpfp\n");
		fflush(stdout);
	}
	for(i = 0; i < count; i++) {
		printf("Writing %d. 0x%08x, 0x%08x\n", i, imagedatas[i], imagesizes[i]);
		fflush(stdout);
		fwrite(imagedatas[i], imagesizes[i], 1, out);
		fflush(out);
		free(imagedatas[i]);
	}
	fclose(out);
	return 0;
}



