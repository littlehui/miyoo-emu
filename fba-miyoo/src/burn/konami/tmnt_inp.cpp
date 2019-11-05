#include "tmnt.h"
// TMNT - Input
// Based on DTMNT and MAME

unsigned char TmntAoo[0x10]; // values to return from A0000
unsigned char TmntCoin[4],TmntService[4],TmntStart[4],TmntAttack[4],TmntJump[4],TmntUp[4],TmntDown[4],TmntLeft[4],TmntRight[4],Tmnt2pStart[2];
static unsigned char *pPlay[4]={TmntAoo+1,TmntAoo+2,TmntAoo+3,TmntAoo+0x0a};
unsigned char TmntDip[3]={0,0,0};
int TmntInpMake()
{
	int i=0;
	memset(&TmntAoo,0,sizeof(TmntAoo));

	for (i=0;i<4;i++)
	{
		if (TmntCoin [i]) TmntAoo[0]|=0x01<<i;
		if (TmntStart[i]) *(pPlay[i])|=0x20;
		if (TmntAttack[i]) *(pPlay[i])|=0x20;
		if (TmntJump[i]) *(pPlay[i])|=0x10;
		if (TmntUp[i]) *(pPlay[i])|=0x04;
		if (TmntDown[i]) *(pPlay[i])|=0x08;
		if (TmntLeft[i]) *(pPlay[i])|=0x01;
		if (TmntRight[i])*(pPlay[i])|=0x02;
		if (Tmnt2pStart[i])*(pPlay[i])|=0x80; // add input by BisonSAS
	}

	return 0;
}
