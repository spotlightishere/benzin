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
 *  c_oop.c                                                                  *
 *  Object orientation shit for C. Compile this!                             *
 * ------------------------------------------------------------------------- *
 *  Code contributed by:                                                     *
 *          SquidMan                                                         *
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "c_oop.h"

int AddObjectToList(u8** listv, u32* listcnt, u8* object, u32 size)
{
	*listcnt++;
	u8* list = *(u8**)listv;
	u8* entries = calloc(size, *listcnt);
	if(list != NULL) {
		memcpy(entries, list, size * (*listcnt - 1));
		free(list);
	}
	memcpy(&entries[(*listcnt - 1) * size], object, size);
	*(u8**)listv = entries;
	return *listcnt - 1;
}

