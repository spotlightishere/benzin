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

const char *whitespace_cb(mxml_node_t *node, int where)
{
	/* code by Matt_P */
	const char *name = mxmlGetElement(node);
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
	}else if(((mxmlGetPrevSibling(node) && where == 0) || mxmlGetParent(node)) && !(where == 2 && strcmp(name, "rotate") && strcmp(name, "scale") && strcmp(name, "translate") && strcmp(name, "xmlyt") && strcmp(name, "xmlan") && strcmp(name, "pai1") && strcmp(name, "pat1") && strcmp(name, "pah1") && strcmp(name, "seconds") && strcmp(name, "entries") && strcmp(name, "triplet") && strcmp(name, "pair") && strcmp(name, "entry") && strcmp(name, "pane") && (strcmp(name, "tag") && strcmp(name, "size") && strcmp(name, "material") && strcmp(name, "vtx") && strcmp(name, "colors") && strcmp(name, "subs") && strcmp(name, "usdentry") && strcmp(name, "font") && strcmp(name, "wnd4") && strcmp(name, "wnd4mat") && strcmp(name, "set") && strcmp(name, "coordinates") && strcmp(name, "TevStage") && strcmp(name, "texture") && strcmp(name, "TextureSRT") && strcmp(name, "CoordGen") && strcmp(name, "ChanControl") && strcmp(name, "MaterialColor") && strcmp(name, "TevSwapModeTable") && strcmp(name, "IndTextureSRT") && strcmp(name, "IndTextureOrder") && strcmp(name, "AlphaCompare") && strcmp(name, "BlendMode") )) ){
		sprintf( xmlbuff , "\n%s" , tab_buffer );
	}else{
		return "";
	}
	return xmlbuff;
}
