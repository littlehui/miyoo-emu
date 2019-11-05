#include "tmnt.h"
// TMNT - Palette
// Based on DTMNT and MAME

unsigned int TmntPal[0x400];

int TmntPalInit()
{
	TmntPalExit();
	return 0;
}

int TmntPalExit()
{
	memset(TmntPal,0,sizeof(TmntPal)); // All colors black
	return 0;
}

inline static unsigned int CalcCol(unsigned short *ps,int i)
{
	unsigned int a0,a1; int r,g,b;
	a0=ps[0]; 
	a1=ps[1];
	r =(a1&0x1f)<<3;
	g =(a0&0x03)<<6;
	g|=(a1&0xe0)>>2;
	b =(a0&0x7c)<<1;
	return BurnHighCol(r,g,b,i);
}

int TmntPalUpdate(int /*bRecalc*/)
{
	// If any colors have changed, convert them from TmntRam08 into the High palette
	int a; unsigned short *ps; unsigned int *ph;


	ps=(unsigned short *)TmntRam08; 
	ph=TmntPal; 
	for (a=0;a<0x80; a++)
	{
		int b; /*unsigned char d;*/ unsigned char m; // dirt info and mask

		for (b=0,m=1; b<8; b++,m<<=1, ps+=2,ph++)
		{
    		*ph=CalcCol(ps,(a<<3)|b); // If dirty recalculate
		}
	}
	return 0;
}

// Write a byte to the palette
void TmntPalWrite(unsigned int a,unsigned char d)
{
	a^=1; 
	a&=0xfff; // Clip to palette length
	if (TmntRam08[a]==d) 
		return; // No change
	TmntRam08[a]=d; // write byte
}
