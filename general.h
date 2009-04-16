/*
 *  general.h
 *  
 *
 *  Created by Alex Marshall on 09/01/30.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#define BENZIN_VERSION_MAJOR		0
#define BENZIN_VERSION_MINOR		1
#define BENZIN_VERSION_BUILD		576
#define BENZIN_VERSION_OTHER		"WIP"

#define INFORMATION_TEXT		\
"Benzin %d.%d.%d%s.\n" \
"Written by SquidMan (Alex Marshall), comex, and megazig.\n" \
"(c) 2009 HACKERCHANNEL\n", BENZIN_VERSION_MAJOR, BENZIN_VERSION_MINOR, BENZIN_VERSION_BUILD, BENZIN_VERSION_OTHER

#define USE_BRLAN
//#define USE_BRLYT

//#define OLD_BRLAN_OUTSTYLE
//#define OLD_BRLYT_OUTSTYLE

//#define DEBUGBRLAN			1
//#define DEBUGMAIN			1
//#define DEBUGBRLYT			1

#define fatal(x)	printf(x); exit(1)

// WTF is this doing here?
/*void LZ77Decompress(void *indata, size_t dlen, u8* ret)
{
	int inp = 0;
	u8* data = (u8*)calloc(dlen + 16, 1);
	memcpy(data, indata, dlen);
	int i;
	u8 bitmask;
	ret = (u8*)calloc(dlen, 256);		// HORRIBLY INEFFICIENT.
						// FIGURE SOMETHING ELSE OUT
	int retoff = 0;
	
	while(inp < dlen) {
		bitmask = data[inp];
		inp++;
		
		for(i = 0; i < 8; i++) {
			if(bitmask & 0x80) {
				u8 rep = data[inp];
				u8 repLength = (rep >> 4) + 3;
				inp++;
				u16 repOff = data[inp] | ((rep & 0x0F) << 8);
				inp++;
				
				if(repOff <= retoff) {
					fatal("Offset too high!\nQuitting\n");
				}
				
				while(repLength > 0) {
					ret[retoff] = ret[(retoff - repOff) - 1];
					repLength--;
					retoff++;
				}
			}else{
				ret[retoff] = data[inp];
				retoff++;
				inp++;
			}
			bitmask = bitmask << 1;
		}
	}
}
*/
