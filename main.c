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
    u8 type = 3;
    char *ext;

    if (argc < 4)
    {
        printf("Please use ./benzin r brlytfilename.brlyt xmlytfilename.xmlyt\n");
        printf("                - OR - \n");
        printf("Please use ./benzin m xmlytfilename.xmlyt brlytfilename.brlyt\n");
        exit(1);
    }
    if ((strcmp(argv[1], "r") == 0) || (strcmp(argv[1], "p") == 0))
    {
        ext = strrchr(argv[2], '.');
    } else if ((strcmp(argv[1], "m") == 0) || (strcmp(argv[1], "w") == 0))
    {
        if(argc < 4)
        {
            printf("Arguments not setup correctly\n");
            printf("Please  type ./benzin to get possible arguments\n");
            exit(1);
        }
        ext = strrchr(argv[3], '.');
    } else {
        printf("Please use ./benzin r brlytfilename.brlyt\n");
        printf("                - OR - \n");
        printf("Please use ./benzin m xmlytfilename.xmlyt brlytfilename.brlyt\n");
        exit(1);
    }
    if(!ext)
    {
        printf("No extension on file\n");
        printf("To see possible arguments type ./benzin\n");
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
        }
        if(type > 2)
        {
            printf("Unknown file extension\n");
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
    if((argv[1][0] == 'r') || (argv[1][0] == 'p')) {
if(type == 2)
{
        parse_brlan(argv[brlanargread], argv[3]);
}
if(type == 1)
{
        parse_brlyt(argv[brlytargread], argv[3]);
}
    }else if((argv[1][0] == 'm') || (argv[1][0] == 'w')) {
if(type == 2)
{
        make_brlan(argv[brlanargmake], argv[brlanargdestmake]);
}
if(type == 1)
{
        make_brlyt(argv[brlytargmake], argv[brlytargdestmake]);
}
    }
    return 0;
}

