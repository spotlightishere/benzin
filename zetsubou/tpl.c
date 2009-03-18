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
/*	for(y1 = 0; y1 < height; y1 += 4) {
		for(x1 = 0; x1 < width; x1 += 4) {
			for(y = y1; y < (y1 + 4); y++) {
				for(x = x1; x < (x1 + 4); x++) {
					u32 oldpixel = *(u32*)(tplbuf + (tplpoint + ((x + (y * width)) * 4)));
					oldpixel = be32(oldpixel);
					ag[i] = (oldpixel >> 16);
					rb[i++] = (oldpixel >> 0);
				}
			}
		}
	}
*/
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
//					u32 rgba = (rb[i] << 24) | (ag[8 + i] << 16) | (rb[8 + i] << 8) | (ag[i] << 0);
					(*(u32**)bitmapdata)[bmpslot++] = rgba;
					printf("%08X\n", rgba);
				}
				for(i = 16; i < 24; i++) {
					u32 rgba = (rb[i] << 0) | (ag[8 + i] << 8) | (rb[8 + i] << 16) | (ag[i] << 24);
//					u32 rgba = (rb[i] << 24) | (ag[8 + i] << 16) | (rb[8 + i] << 8) | (ag[i] << 0);
					(*(u32**)bitmapdata)[bmpslot++] = rgba;
					printf("%08X\n", rgba);
				}
				printf("\n");
				z = 0;
				i = 0;
			}
		}
	}
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
					u8 oldpixel = *(u8*)(tplbuf + (tplpoint + ((iv++) * 1)));
					u8 b = oldpixel >> 4;
					u8 g = oldpixel >> 4;
					u8 r = oldpixel >> 4;
					u8 a = oldpixel & 0xF;
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
//	u32 genstuff[] = {htonl(1), htonl(0xc), htonl(0x14), 0};
	if(header[2] != be32(0x0000000c)) {
		printf("TPL Header is malformed. %08x\n", header[2]);
		return -1;
	}
	u32 TPLTextures[32][2];				// Who's gonna use more than 32 textures per TPL? Nobody.
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
