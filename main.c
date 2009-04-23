#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brlan.h"
#include "brlyt.h"
#include "types.h"
#include "endian.h"
#include "general.h"

#ifdef DEBUGMAIN
#define dbgprintf    printf
#else
#define dbgprintf    //
#endif //DEBUGMAIN

int main(int argc, char* argv[])
{
    
u8 type;
char *ext;

if (strcmp(argv[1], "r") == 0 )
{
    ext = strrchr(argv[2], '.');
}
if (strcmp(argv[1], "m") == 0)
{
    ext = strrchr(argv[3], '.');
}
if(!ext)
{
    printf("no extension");
    exit(1);
}
else
{
char* temp = ext;
    if(strcmp(++ext, "brlyt") == 0)
    {
        type = 1;
    }
    ext = temp;
    if(strcmp(++ext, "brlan") == 0)
    {
        type = 2;
    } else {
        printf("unknown file extension");
        exit(1);
    }
}

    int brlanargread;
    int brlytargread;
    int brlanargmake;
    int brlytargmake;
    int brlanargdestmake;
    int brlytargdestmake;
    int reqargs;
    int currentarg = 2;
    char helpstrmake[256];
    char helpstrread[256];
    sprintf(helpstrmake, "%s m", argv[0]);
    sprintf(helpstrread, "%s r", argv[0]);
if(type == 2)
{
    brlanargread = currentarg;
    strcat(helpstrread, " <*.brlan>");
    brlanargmake = currentarg++;
    strcat(helpstrmake, " <*.xmlan>");
}
if(type == 1)
{
    brlytargread = currentarg;
    strcat(helpstrread, " <*.brlyt>");
    brlytargmake = currentarg++;
    strcat(helpstrmake, " <*.xmlyt>");
}
    reqargs = currentarg;
if(type == 2)
{
    brlanargdestmake = currentarg++;
    strcat(helpstrmake, " <out.brlan>");
}
if(type == 1)
{
    brlytargdestmake = currentarg++;
    strcat(helpstrmake, " <out.brlyt>");
}

    printf(INFORMATION_TEXT);
    if(argc < reqargs) {
        printf("Invalid arguments. Use like:\n\t%s\n\t\tor:\n\t%s\n", helpstrread, helpstrmake);
        exit(1);
    }
    if(argv[1][0] == 'r') {
if(type == 2)
{
        parse_brlan(argv[brlanargread]);
}
if(type == 1)
{
        parse_brlyt(argv[brlytargread]);
}
    }else if(argv[1][0] == 'm') {
if(type == 2)
{
        dbgprintf("1:%d 2:%d f1:%s f2:%s\n", brlanargmake, brlanargdestmake, argv[brlanargmake], argv[brlanargdestmake]);
        make_brlan(argv[brlanargmake], argv[brlanargdestmake]);
}
if(type == 1)
{
        dbgprintf("1:%d 2:%d f1:%s f2:%s\n", brlytargmake, brlytargdestmake, argv[brlytargmake], argv[brlytargdestmake]);
        make_brlyt(argv[brlytargmake], argv[brlytargdestmake]);
}
    }
    return 0;
}


















