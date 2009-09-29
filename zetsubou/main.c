/*****************************************************************************
 *  main.c                                                                   *
 *  Part of Zetsubou                                                         *
 *  Part of Benzin                                                           *
 *  The Main Code.                                                           *
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
#include <ctype.h>

#include "types.h"
#include "tpl.h"

void bad_args(char progname[])
{
	printf("Invalid usage. Usage:\n\t%s r[bmp|png] <file.tpl>\nOr:\n\t%s w[bmp|png] <type> <file1.bmp> [file2.bmp...] <out.tpl>\n", progname, progname);
	exit(1);
}

int main(int argc, char *argv[])
{
	printf("Zetsubou v0.1. Written by SquidMan (Alex Marshall)\nFormat information from YAGCD. RGB5A3 code by booto. CMP code by segher.\nLightly based off of gentpl by comex.\n\n");
	if(argc < 3)
		bad_args(argv[0]);
	char type = toupper(argv[1][0]);
	char fmt = toupper(argv[1][1]);
	if(type == 'R') {	// Read TPL.
		if(argc != 3)
			bad_args(argv[0]);
		FILE* fp = fopen(argv[2], "rb");
		if(fp == NULL) {
			printf("Error opening %s.\n", argv[2]);
			exit(1);
		}
		char *basename = strdup(argv[2]);
		char *last_dot = strrchr(basename, '.');
		if(last_dot) *last_dot = 0;
		fseek(fp, 0, SEEK_END);
		u32 tplsize = ftell(fp);
		u8* tplbuf  = (u8*)malloc(tplsize);
		fseek(fp, 0, SEEK_SET);
		fread(tplbuf, tplsize, 1, fp);
		fclose(fp);
		int ret;
		int outfmt;
		if(fmt == 'B')
			outfmt = 0;
		else if(fmt == 'P')
			outfmt = 1;
		else
			bad_args(argv[0]);
		ret = TPL_ConvertToGD(tplbuf, tplsize, basename, outfmt);
		free(tplbuf);
		free(basename);
		if(ret < 0) {
			printf("Error converting from TPL.\n");
			exit(1);
		}
	}else if(type == 'W') {	// Write TPL.
		if(argc < 5)
			bad_args(argv[0]);
		char** names = (char**)calloc(sizeof(char*), argc - 4);
		int i;
		for(i = 0; i < argc - 4; i++)
			names[i] = argv[i + 3];
		int format;
		char formatstr[256];
		memset(formatstr, 0, 256);
		for(i = 0; i < strlen(argv[2]); i++) formatstr[i] = toupper(argv[2][i]);
		if(strcmp(formatstr, "I4") == 0) format = TPL_FORMAT_I4;
		else if(strcmp(formatstr, "I8") == 0) format = TPL_FORMAT_I8;
		else if(strcmp(formatstr, "IA4") == 0) format = TPL_FORMAT_IA4;
		else if(strcmp(formatstr, "IA8") == 0) format = TPL_FORMAT_IA8;
		else if(strcmp(formatstr, "RGB565") == 0) format = TPL_FORMAT_RGB565;
		else if(strcmp(formatstr, "RGB5A3") == 0) format = TPL_FORMAT_RGB5A3;
		else if(strcmp(formatstr, "RGBA8") == 0) format = TPL_FORMAT_RGBA8;
		else if(strcmp(formatstr, "CI4") == 0) format = TPL_FORMAT_CI4;
		else if(strcmp(formatstr, "CI8") == 0) format = TPL_FORMAT_CI8;
		else if(strcmp(formatstr, "CI14X2") == 0) format = TPL_FORMAT_CI14X2;
		else if(strcmp(formatstr, "CMP") == 0) format = TPL_FORMAT_CMP;
		int ret;
		int outfmt;
		if(fmt == 'B')
			outfmt = 0;
		else if(fmt == 'P')
			outfmt = 1;
		else
			bad_args(argv[0]);
		ret = TPL_ConvertFromGDs(argc - 4, names, argv[argc - 1], format, outfmt);
		if(ret < 0) {
			printf("Error converting to TPL.\n");
			exit(1);
		}
	}
	return 0;
}
