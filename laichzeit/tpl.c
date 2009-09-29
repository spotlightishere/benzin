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
 *  tpl.c                                                                    *
 *  The code for all the TPL stuff. Compile this!                            *
 * ------------------------------------------------------------------------- *
 *  Most code contributed by:                                                *
 *          SquidMan                                                         *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>

#include "types.h"
#include "tpl.h"
#include "c_oop.h"

static int AddImageToTPL(TPL* tpl, TPLImage img)
{
	return AddObjectToList((u8**)&tpl->imgs, (u32*)&tpl->imgcnt, \
			       (u8*)&img, sizeof(TPLImage));
}

TPL* LaichTPL_Start()
{
	TPL* tpl	= malloc(sizeof(TPL));
	tpl->imgcnt	= 0;
	tpl->imgs	= NULL;
	return tpl;
}

int LaichTPL_StartImage(TPL* tpl, char* file)
{
	TPLImage img;
	img.done		= 0;
	img.header.height	= 0;
	img.header.width	= 0;
	img.header.format	= TPL_FORMAT_RGBA8;
	img.header.offs		= 0;
	img.header.wrap_s	= 0;
	img.header.wrap_t	= 0;
	img.header.min		= 0;
	img.header.mag		= 0;
	img.header.lod_bias	= 0;
	img.header.edge_lod	= 0;
	img.header.min_lod	= 0;
	img.header.max_lod	= 0;
	img.header.unpacked	= 0;
	memset(img.filename, 0, 256);
	strncpy(img.filename, file, 256);
	return AddImageToTPL(tpl, img);
}

void LaichTPL_SetImageFormat(TPL* tpl, int img, TPLFormat format)
{
	tpl->imgs[img].header.format = format;
}

void LaichTPL_SetImageWrap(TPL* tpl, int img, u32 s, u32 t)
{
	tpl->imgs[img].header.wrap_s = s;
	tpl->imgs[img].header.wrap_t = t;
}

void LaichTPL_SetImageFilters(TPL* tpl, int img, u32 min, u32 mag)
{
	tpl->imgs[img].header.min = min;
	tpl->imgs[img].header.mag = mag;
}

void LaichTPL_SetImageLOD(TPL* tpl, int img, \
			      f32 lod_bias, u8 edge_lod, u8 min_lod, u8 max_lod)
{
	tpl->imgs[img].header.lod_bias = lod_bias;
	tpl->imgs[img].header.edge_lod = edge_lod;
	tpl->imgs[img].header.min_lod  = min_lod;
	tpl->imgs[img].header.max_lod  = max_lod;
}

void LaichTPL_EndImage(TPL* tpl, int img)
{
	tpl->imgs[img].done = 1;
}

static int TPL_ConvertBitMapToRGB565(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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
						
						newpixel = ((b >> 3) << 11) | ((g >> 2) << 5) | ((r >> 3) << 0);
					}
					((u16*)writebuf)[iv++] = htons(newpixel);
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
}

static int TPL_ConvertBitMapToRGBA8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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
					if((x >= width) || (y >= height))
						rgba[z] = 0;
					else
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

static int TPL_ConvertBitMapToRGB5A3(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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
						u8 a = (rgba >> 24)  & 0xFF;
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
						}else{
							// We don't have enough Alpha to matter. Use RGB5.
							newpixel &= ~(1 << 15);
							r = ((r * 31) / 255) & 0x1F;
							g = ((g * 31) / 255) & 0x1F;
							b = ((b * 31) / 255) & 0x1F;
							newpixel |= r << 10;
							newpixel |= g << 5;
							newpixel |= b << 0;
						}
					}
					((u16*)writebuf)[iv++] = htobs(newpixel);
				}
			}
		}
	}
	printf("Converted to 0x%08x bytes of 0x%08x bytes.\n", iv * 2, outsz);
	fflush(stdout);
	return outsz;
}

static int TPL_ConvertBitMapToI4(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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

static int TPL_ConvertBitMapToIA4(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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
						
						newpixel = (((i1 * 15) / 255) << 4);
						newpixel |= (((a1 * 15) / 255) & 0xF);
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

static int TPL_ConvertBitMapToI8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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

static int TPL_ConvertBitMapToIA8(u8* bitmapdata, u32 bitmapsize, u8** tplbuf, u32 width, u32 height)
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

u8* LaichTPL_CompileTPL(TPL* tpl, u32* fsize)
{
	FILE *out = fopen("tpl.tmp", "wb");
	if(out == NULL)
		return NULL;
	fflush(stdout);
	u32 magic = htobl(0x0020af30);
	u32 header[] = { magic, htobl(tpl->imgcnt), htobl(0xc) };
	fwrite(header, 4, 3, out);
		
	int i;
	for(i = 0; i < tpl->imgcnt; i++) {
		u32 offsets[] = { htobl(12 + (8 * tpl->imgcnt) + (36 * i)), htobl(0) };
		fwrite(offsets, 4, 2, out);
	}
	u8* imagedatas[256];
	u32 imagesizes[256];
	for(i = 0; i < tpl->imgcnt; i++) {
		FILE* gdfp = fopen(tpl->imgs[i].filename, "rb");
		gdImagePtr im = gdImageCreateFromPng(gdfp);
		gdImageAlphaBlending(im, 0);
		gdImageSaveAlpha(im, 1);
		fclose(gdfp);
		u32 width = tpl->imgs[i].header.width;
		u32 height = tpl->imgs[i].header.height;
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
				u8 a = 254 - 2*((u8)gdImageAlpha(im, p));
				if(a == 254) a++;
				u8 r = (u8)gdImageRed(im, p);
				u8 g = (u8)gdImageGreen(im, p);
				u8 b = (u8)gdImageBlue(im, p);
				u32 rgba = (r << 8) | (g << 16) | (b << 24) | (a << 0);
				rgba = htobl(rgba);
				((u32*)bitmapdata)[x + (y * width)] = rgba;
			}
		}
		int ret;
		switch(tpl->imgs[i].header.format) {
			case TPL_FORMAT_RGB565:
				ret = TPL_ConvertBitMapToRGB565(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_RGB5A3:
				ret = TPL_ConvertBitMapToRGB5A3(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_RGBA8:
				ret = TPL_ConvertBitMapToRGBA8(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_I4:
				ret = TPL_ConvertBitMapToI4(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_I8:
				ret = TPL_ConvertBitMapToI8(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_IA4:
				ret = TPL_ConvertBitMapToIA4(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_IA8:
				ret = TPL_ConvertBitMapToIA8(bitmapdata, bitmapsize, &tplbuf, width, height);
				break;
			case TPL_FORMAT_CMP:
				ret = -1;
				break;
			default:
				ret = -1;
		}
		if(ret == -1) {
			return NULL;
		}
		imagedatas[i] = tplbuf;
		imagesizes[i] = ret;
		fflush(stdout);
		tpl->imgs[i].header.format	= htobl(tpl->imgs[i].header.format);
		tpl->imgs[i].header.offs	= htobl(12 + (8 * tpl->imgcnt) + (36 * tpl->imgcnt));
		printf("Dimensions: %dx%d\n", width, height);
		tpl->imgs[i].header.width	= htobs(tpl->imgs[i].header.width);
		tpl->imgs[i].header.height	= htobs(tpl->imgs[i].header.height);
		tpl->imgs[i].header.min		= htobl(tpl->imgs[i].header.min);
		tpl->imgs[i].header.mag		= htobl(tpl->imgs[i].header.mag);
		tpl->imgs[i].header.wrap_s	= htobl(tpl->imgs[i].header.wrap_s);
		tpl->imgs[i].header.wrap_t	= htobl(tpl->imgs[i].header.wrap_t);
		tpl->imgs[i].header.lod_bias	= htobl(tpl->imgs[i].header.lod_bias);
		fwrite(&tpl->imgs[i].header.height,	2, 1, out);
		fwrite(&tpl->imgs[i].header.width,	2, 1, out);
		fwrite(&tpl->imgs[i].header.format,	4, 1, out);
		fwrite(&tpl->imgs[i].header.offs,	4, 1, out);
		fwrite(&tpl->imgs[i].header.wrap_s,	4, 1, out);
		fwrite(&tpl->imgs[i].header.wrap_t,	4, 1, out);
		fwrite(&tpl->imgs[i].header.min,	4, 1, out);
		fwrite(&tpl->imgs[i].header.mag,	4, 1, out);
		fwrite(&tpl->imgs[i].header.lod_bias,	4, 1, out);
		fwrite(&tpl->imgs[i].header.edge_lod,	1, 1, out);
		fwrite(&tpl->imgs[i].header.min_lod,	1, 1, out);
		fwrite(&tpl->imgs[i].header.max_lod,	1, 1, out);
		fwrite(&tpl->imgs[i].header.unpacked,	1, 1, out);
		fflush(out);
	}
	for(i = 0; i < tpl->imgcnt; i++) {
		fwrite(imagedatas[i], imagesizes[i], 1, out);
		fflush(out);
		free(imagedatas[i]);
	}
	fclose(out);
	out = fopen("tpl.tmp", "rb");
	fseek(out, 0, SEEK_END);
	u32 filesize = ftell(out);
	u8* filedata = malloc(filesize);
	fseek(out, 0, SEEK_SET);
	fread(filedata, filesize, 1, out);
	fflush(out);
	fclose(out);
	remove("tpl.tmp");
	if(fsize != NULL)
		*fsize = filesize;
	return filedata;
}



