#include <string.h>
#include <mxml.h>

#include "types.h"
#include "xml.h"

char *                    /* O - Buffer */
get_value(mxml_node_t *node,        /* I - Node to get */
         void        *buffer,        /* I - Buffer */
     int         buflen)        /* I - Size of buffer */
{
    char        *ptr,            /* Pointer into buffer */
    *end;            /* End of buffer */
    int        len;            /* Length of node */
    mxml_node_t    *current;        /* Current node */
    
    
    ptr = (char*)buffer;
    end = (char*)buffer + buflen - 1;
    char tempbuf[4092];
    current = node->child;
    for (current = node->child; current && ptr < end; current = current->next)
    {
        if (current->type == MXML_TEXT)
        {
            if (current->value.text.whitespace)
                *ptr++ = ' ';
            
            len = (int)strlen(current->value.text.string);
            if (len > (int)(end - ptr))
                len = (int)(end - ptr);
            
            memcpy(ptr, current->value.text.string, len);
            ptr += len;
        }
        else if (current->type == MXML_OPAQUE)
        {
            len = (int)strlen(current->value.opaque);
            if (len > (int)(end - ptr))
                len = (int)(end - ptr);
            
            memcpy(ptr, current->value.opaque, len);
            ptr += len;
        }
        else if (current->type == MXML_INTEGER)
        {
            sprintf(tempbuf, "%d", current->value.integer);
            len = (int)strlen(tempbuf);
            if (len > (int)(end - ptr))
                len = (int)(end - ptr);
            
            memcpy(ptr, tempbuf, len);
            ptr += len;
        }
        else if (current->type == MXML_REAL)
        {
            sprintf(tempbuf, "%f", current->value.real);
            len = (int)strlen(tempbuf);
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
    const char *name;
    name = node->value.element.name;

    if (!strcmp(name, "xmlan"))
    {
	if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return("\n");
    }
    if (!strcmp(name, "timg"))
    {
	if (where == MXML_WS_BEFORE_OPEN)
            return("\n");
    }
    else if (!strcmp(name, "pane")) 
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t");
    }
    else if (!strcmp(name, "tag")) 
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t");
    }
    else if (!strcmp(name, "entry"))
    {
	if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t\t");
    }
    else if (!strcmp(name, "triplet") ||
             !strcmp(name, "pair"))
    {
	if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
	  return ("\n\t\t\t\t");
    }
    else if (!strcmp(name, "frame") ||
             !strcmp(name, "value") ||
             !strcmp(name, "blend") ||
             !strcmp(name, "data1") ||
             !strcmp(name, "data2") ||
             !strcmp(name, "padding"))
    {
	if (where == MXML_WS_BEFORE_OPEN)
	  return ("\n\t\t\t\t\t");
    }
    else if (!strcmp(name, "xmlyt"))
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return("\n");
    }
    else if (!strcmp(name, "tag")) 
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t");
    }
    else if (!strcmp(name, "a") ||
             !strcmp(name, "size") ||
             !strcmp(name, "entries")) 
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t");
    }
    else if (!strcmp(name, "data") ||
             !strcmp(name, "name"))
    {
        if ((where == MXML_WS_BEFORE_OPEN))
            return ("\n\t\t\t");
    }
    else if (!strcmp(name, "colors") ||
             !strcmp(name, "wrap_t") ||
             !strcmp(name, "wrap_s") ||
             !strcmp(name, "material"))
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t\t");
    }
    else if (!strcmp(name, "black_color") ||
             !strcmp(name, "white_color") ||
             !strcmp(name, "unk2") ||
             !strcmp(name, "tev_k"))
    {
        if ((where == MXML_WS_BEFORE_OPEN))
        {
            return ("\n\t\t\t\t");
        }
    }
    else if (!strcmp(name, "flags") ||
             !strcmp(name, "ua2") ||
             !strcmp(name, "ua3") ||
             !strcmp(name, "ua4") ||
             !strcmp(name, "ua5") ||
             !strcmp(name, "ua6") ||
             !strcmp(name, "ua7") ||
             !strcmp(name, "ua8") ||
             !strcmp(name, "ua9") ||
             !strcmp(name, "uaa") ||
             !strcmp(name, "uab"))
    {
        if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE))
            return ("\n\t\t\t");
    }
    /*
     * Return NULL for no added whitespace...
     */
    return (NULL);
}

