#include "tiles_generic.h"
#include "burn_ym2151.h"
// TMNT ------------------------------------------------
// Based on DTMNT and MAME
// tmnt_run.cpp
extern unsigned char bTmntRecalcPal;
extern unsigned char *TmntRam08,*TmntRam10,*TmntTile,*TmntSprite,*TmntSndRam,*K051960Ram,*PriRam;
int TmntInit();
int TmntExit();
int TmntFrame();
int TmntScan(int nAction,int *pnMin);


//MIA defines
int MiaInit();
int MiaFrame();
int MiaExit();
void MiaReset();


// tmnt_pal.cpp
extern unsigned int TmntPal[0x400];
int TmntPalInit();
int TmntPalExit();
void TmntPalWrite(unsigned int a,unsigned char d);
int TmntPalUpdate(int bRecalc);

// tmnt_til.cpp
int TmntTileDraw();

struct K051960Sprite {
	unsigned int number;
	int x,y,w,h;
	int flip;
	short pal_offset;
	int zoomy,zoomx;
};

#define SPRITEFLIPX 1
#define SPRITEFLIPY 2

void K051960Reset();

// tmnt_inp.cpp
extern unsigned char TmntAoo[0x10]; // values to return from A0000
extern unsigned char TmntCoin[4],TmntService[4],TmntStart[4],TmntAttack[4],TmntJump[4],TmntUp[4],TmntDown[4],TmntLeft[4],TmntRight[4],Tmnt2pStart[2];
extern unsigned char bInt5;
int TmntInpMake();
extern unsigned char TmntDip[3];




extern unsigned char DrvReset;
