/*
 *  main.c
 *  Zetsubou
 *
 *  Created by Alex Marshall on 09/03/17.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "tpl.h"

int main(int argc, char *argv[])
{
	printf("Zetsubou v0.1. Written by SquidMan (Alex Marshall)\nRGB5A3 code was contributed by booto.\n\n");
	if(argc != 2) {
		printf("Invalid usage. Usage:\n\t%s <file.tpl>\n", argv[0]);
		exit(1);
	}
	FILE* fp = fopen(argv[1], "rb");
	if(fp == NULL) {
		printf("Error opening %s.\n", argv[1]);
		exit(1);
	}
	char *basename = (char*)calloc(strlen(argv[1]) + 1, 1);
	int arg1strlen = strlen(argv[1]);
	int i;
	for(i = 0; (i < arg1strlen) && (argv[1][i] != '.'); i++) basename[i] = argv[1][i];
	fseek(fp, 0, SEEK_END);
	u32 tplsize = ftell(fp);
	u8* tplbuf  = (u8*)malloc(tplsize);
	fseek(fp, 0, SEEK_SET);
	fread(tplbuf, tplsize, 1, fp);
	fclose(fp);
	int ret = TPL_ConvertToBMP(tplbuf, tplsize, basename);
	if(ret < 0) {
		printf("Error converting to BMP.\n");
		exit(1);
	}
	return 0;
}
