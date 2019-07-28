#include "tmnt.h"
// Missing in Action - Driver

static struct BurnInputInfo MIAInputList[]=
{
  {"p1 coin" ,	BIT_DIGITAL, TmntCoin+0		,"p1 coin"},
  {"p2 coin"  , BIT_DIGITAL, TmntCoin+1, "p2 coin"},
  {"p3 coin"  , BIT_DIGITAL, TmntCoin+2, "p3 coin"},
  {"p4 coin"  , BIT_DIGITAL, TmntCoin+3, "p4 coin"},
  {"p1 start" , BIT_DIGITAL, TmntStart+0,"p1 start"},
  {"p2 start" , BIT_DIGITAL, TmntStart+1,"p2 start"},
  {"p3 start" , BIT_DIGITAL, TmntStart+2,"p3 start"},
  {"p4 start" , BIT_DIGITAL, TmntStart+3,"p4 start"},
};

STDINPUTINFO(MIA);

//=============================
//Missing in Action (Version T)
//=============================

// Count possible zip names and (if pszName!=NULL) return them

// Rom information
static struct BurnRomInfo MiaRomDesc[]=
{
  {"808t20.h17",0x20000, 0x6f0acb1d, 0x10}, //  0 68000 code (even)
  {"808t21.j17",0x20000, 0x42a30416, 0x10}, //  1            (odd)
 
  {"808e12.f28",0x10000, 0xd62f1fde,    1}, //  4 tile bitmaps
  {"808e13.h28",0x10000, 0x1fa708f4,    1}, //  5
  {"808e22.i28",0x10000, 0x73d758f6,    1}, //  2
  {"808e23.k28",0x10000, 0x8ff08b21,    1}, //  3

  {"808d17.j4",0x80000, 0xd1299082,    1}, //  6
  {"808d15.h4",0x80000, 0x2b22a6b6,    1}, //  7

  {"808a18.f16",0x0100, 0xeb95aede,    1}, //  7
 {"808d01.d4",0x20000, 0xfd4d37c0,    1}, //  7
 
 {"808e03.f4",0x08000, 0x3d93a7cd,    1}, //  7
};

STD_ROM_PICK(Mia);
STD_ROM_FN(Mia);


struct BurnDriverD BurnDrvMia={
	"mia",NULL,NULL,"1989",
	"Missing in Action (Version T)\0",NULL,"Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING,4,HARDWARE_MISC_PRE90S,
	NULL,MiaRomInfo,MiaRomName,MIAInputInfo, NULL,
	MiaInit,MiaExit,MiaFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3  
};

