/******************************************************************************
 *  xml.c                                                                     *
 *  Part of Benzin                                                            *
 *  Handles some XML stuff.                                                   *
 *  Copyright (C)2009 SquidMan (Alex Marshall)        <SquidMan72@gmail.com>  *
 *  Copyright (C)2009 megazig  (Stephen Simpson)       <megazig@hotmail.com>  *
 *  Copyright (C)2009 Matt_P   (Matthew Parlane)                              *
 *  Copyright (C)2009 comex                                                   *
 *  Copyright (C)2009 booto                                                   *
 *  All Rights Reserved, HACKERCHANNEL.                                       *
 ******************************************************************************/

#include <stdbool.h>
#include <string.h>
#include <mxml.h>

#include "types.h"
#include "xml.h"

char xmlbuff[4096];

char *								/* O - Buffer */
get_value(mxml_node_t *node,		/* I - Node to get */
		void        *buffer,		/* I - Buffer */
		int         buflen)			/* I - Size of buffer */
{
	char		*ptr,				/* Pointer into buffer */
	*end;							/* End of buffer */
	int			len;				/* Length of node */
	mxml_node_t	*current;			/* Current node */
    
    
	ptr = (char*)buffer;
	end = (char*)buffer + buflen - 1;
	char tempbuf[4092];

	for (current = mxmlGetNextSibling(node); current && ptr < end; current = mxmlGetNextSibling(current)) {
		mxml_type_t currentType = mxmlGetType(current);
		if (currentType == MXML_TEXT) {
			int whitespace;			
			const char* value = mxmlGetText(current, &whitespace);
			if (whitespace)
				*ptr++ = ' ';

			len = strlen(value);
			if (len > (int)(end - ptr))
				len = (int)(end - ptr);

			memcpy(ptr, value, len);
			ptr += len;
		} else if (currentType == MXML_OPAQUE) {
			const char* value = mxmlGetOpaque(current);
			len = strlen(value);
			if (len > (int)(end - ptr))
				len = (int)(end - ptr);

			memcpy(ptr, value, len);
			ptr += len;
		}
		else if (currentType == MXML_INTEGER)
		{
			sprintf(tempbuf, "%d", mxmlGetInteger(current));
			len = strlen(tempbuf);
			if (len > (int)(end - ptr))
				len = (int)(end - ptr);

			memcpy(ptr, tempbuf, len);
			ptr += len;
		} else if (currentType == MXML_REAL) {
			sprintf(tempbuf, "%f", mxmlGetReal(current));
			len = strlen(tempbuf);
			if (len > (int)(end - ptr))
				len = (int)(end - ptr);
            
			memcpy(ptr, tempbuf, len);
			ptr += len;
		}
	}
	*ptr = 0;
	return buffer;
}

// When names within tabKeyNames are encountered,
// we add a tab for their values within our XML output.
char *tabKeyNames[] = {
	"rotate", "scale", "translate",
	"xmlyt", "xmlan", "pai1",
	"pat1", "pah1", "seconds",
	"entries", "triplet", "pair",
	"entry", "pane", "tag",
	"size", "material", "vtx",
	"colors", "subs", "usdentry",
	"font", "wnd4", "wnd4mat",
	"set", "coordinates", "TevStage",
	"texture", "TextureSRT", "CoordGen",
	"ChanControl", "MaterialColor",
	"TevSwapModeTable", "IndTextureSRT",
	"IndTextureOrder", "AlphaCompare",
	"BlendMode", 0};

bool shouldHaveTabKey(mxml_node_t* node) {
 	const char* name = mxmlGetElement(node);
 	int i = 0;
 	while (tabKeyNames[i]) {
 	 	if (strcmp(name, tabKeyNames[i]) == 0) {
 	 	 	return true;
 	 	} else {
 	 	 	i++;
 	 	}
 	}

 	return false;
}

const char *whitespace_cb(mxml_node_t *node, int where)
{
	/* code by Matt_P */
	mxml_node_t * temp = node;
	char tab_buffer[25];
	memset( tab_buffer , 0 , 15 );
	int depth = 0;
	while(temp){
		depth++;
		tab_buffer[depth - 1] = '\t';
		temp = mxmlGetParent(temp);
	}
	if (where == 3 || where == 1){
		return "";
	} else if (((mxmlGetPrevSibling(node) && where == 0) || mxmlGetParent(node)) && !(where == 2 && !shouldHaveTabKey(node))) {
		sprintf( xmlbuff , "\n%s" , tab_buffer );
	} else {
		return "";
	}
	return xmlbuff;
}
