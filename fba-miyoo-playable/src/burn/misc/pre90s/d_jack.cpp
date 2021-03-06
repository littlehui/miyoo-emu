// FB Alpha Jack the Giantkiller driver module
// Based on MAME driver by Brad Oliver

#include "tiles_generic.h"
#include "bitswap.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}


static unsigned char *Mem, *Rom0, *Rom1, *Gfx, *Prom, *User;
static unsigned char DrvJoy1[8], DrvJoy2[8], DrvJoy3[8], DrvJoy4[8], DrvReset, DrvDips[2];
static short *pAY8910Buffer[3], *pFMBuffer = NULL;
static int tri_fix, joinem, loverb, suprtriv;
static int timer_rate, flip_screen;
static unsigned int *Palette, *DrvPal;
static unsigned char DrvCalcPal;
static int snd_cpu_irq;

static unsigned char soundlatch;
static int question_address, question_rom, remap_address[16];
static int joinem_snd_bit;

static struct BurnInputInfo JackInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 6,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 0,	"p1 start" },
	{"p1 Up"	, BIT_DIGITAL,   DrvJoy2 + 0,   "p1 up"    },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy2 + 1,   "p1 down", },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy2 + 2, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p1 left"  },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy3 + 1,	"p1 fire 2"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy1 + 5,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p2 start" },
	{"p2 Up"	, BIT_DIGITAL,   DrvJoy2 + 4,   "p2 up"    },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 5,   "p2 down", },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 6, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 7, 	"p2 left"  },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy4 + 0,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy4 + 1,	"p2 fire 2"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
	{"Dip 2",	  BIT_DIPSWITCH, DrvDips + 1,	"dip"	   },
};

STDINPUTINFO(Jack);

static struct BurnInputInfo ZzyzzyxxInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 0,	"p1 start" },
	{"P1 Up"      ,   BIT_DIGITAL  , DrvJoy2 + 0, 	"p1 up"    },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy2 + 1,   "p1 down", },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 1"},

	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p2 start" },
	{"P2 Up"      ,   BIT_DIGITAL  , DrvJoy2 + 4, 	"p2 Up"    },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 5,   "p2 down", },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy4 + 0,	"p2 fire 1"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
	{"Dip 2",	  BIT_DIPSWITCH, DrvDips + 1,   "dip"      },
};

STDINPUTINFO(Zzyzzyxx);

static struct BurnInputInfo FreezeInputList[] = {
	{"Coin"      ,    BIT_DIGITAL  , DrvJoy1 + 5,	"p1 coin"  },
	{"Start 1"  ,     BIT_DIGITAL  , DrvJoy1 + 0,	"p1 start" },
	{"Start 2"  ,     BIT_DIGITAL  , DrvJoy1 + 1,	"p2 start" },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy2 + 0, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy2 + 1, 	"p1 left"  },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 1,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 2"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
};

STDINPUTINFO(Freeze);

static struct BurnInputInfo SucasinoInputList[] = {
	{"Coin"      ,    BIT_DIGITAL  , DrvJoy1 + 6,	"p1 coin"  },

	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 0,	"p1 start" },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy2 + 2, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p1 left"  },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 1"},

	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p2 start" },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 6, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 7, 	"p2 left"  },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy4 + 0,	"p2 fire 1"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
};

STDINPUTINFO(Sucasino);

static struct BurnInputInfo TripoolInputList[] = {
	{"Select Game 1", BIT_DIGITAL  , DrvJoy1 + 2,   "Select Game 1"},
	{"Select Game 2", BIT_DIGITAL,   DrvJoy1 + 3,   "Select Game 2"},
	{"Select Game 3", BIT_DIGITAL,   DrvJoy1 + 4,   "Select Game 3"},

	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 6,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 0,	"p1 start" },
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy2 + 0,   "p1 up"    },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy2 + 1,   "p1 down", },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy2 + 2, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p1 left"  },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy3 + 1,	"p1 fire 2"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy1 + 5,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p2 start" },
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy2 + 4,   "p1 up"    },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 5,   "p2 down", },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 6, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 7, 	"p2 left"  },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy4 + 0,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy4 + 1,	"p2 fire 2"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
};

STDINPUTINFO(Tripool);

static struct BurnInputInfo JoinemInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy3 + 2,	"p1 start" },
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy1 + 0,   "p1 up",   },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy1 + 1,   "p1 down", },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 2, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 3, 	"p1 left"  },

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy3 + 1,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy3 + 3,	"p2 start" },
	{"P2 Up",	  BIT_DIGITAL,   DrvJoy2 + 0,   "p2 up",   },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 1,   "p2 down", },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 2, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p2 left"  },

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
	{"Dip 2",	  BIT_DIPSWITCH, DrvDips + 1,	"dip"	   },
};

STDINPUTINFO(Joinem);

static struct BurnInputInfo LoverboyInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy3 + 2,	"p1 start" },
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy1 + 0,   "p1 up",   },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy1 + 1,   "p1 down", },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 2, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 3, 	"p1 left"  },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 4,	"p1 fire 1"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy3 + 1,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy3 + 3,	"p2 start" },
	{"P2 Up",	  BIT_DIGITAL,   DrvJoy2 + 0,   "p2 up",   },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 1,   "p2 down", },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 2, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p2 left"  },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 4,	"p2 fire 1"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
};

STDINPUTINFO(Loverboy);

static struct BurnInputInfo StrivInputList[] = {
	{"Coin"      ,    BIT_DIGITAL  , DrvJoy3 + 1,	"p1 coin"  },

	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 1,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 2,	"p1 fire 2"},
	{"P1 Button 3"  , BIT_DIGITAL  , DrvJoy1 + 3,	"p1 fire 3"},
	{"P1 Button 4"  , BIT_DIGITAL  , DrvJoy1 + 4,	"p1 fire 4"},

	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 1,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy2 + 2,	"p2 fire 2"},
	{"P2 Button 3"  , BIT_DIGITAL  , DrvJoy2 + 3,	"p2 fire 3"},
	{"P2 Button 4"  , BIT_DIGITAL  , DrvJoy2 + 4,	"p2 fire 4"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,     "reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
};

STDINPUTINFO(Striv);

static struct BurnDIPInfo JackDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0x00, NULL},
	{0x12, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coin B"},
	{0x11, 0x01, 0x03, 0x01, "2 coins 1 credit"},
	{0x11, 0x01, 0x03, 0x03, "4 coins 3 credits"},
	{0x11, 0x01, 0x03, 0x00, "1 coin 1 credit"},
	{0x11, 0x01, 0x03, 0x02, "1 coin 3 credits"},

	{0   , 0xfe, 0   , 4   , "Coin A"},
	{0x11, 0x01, 0x0c, 0x00, "3 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x04, "2 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x08, "4 coins 3 credits"},
	{0x11, 0x01, 0x0c, 0x0c, "1 coin 1 credits"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x11, 0x01, 0x10, 0x00, "3"},
	{0x11, 0x01, 0x10, 0x10, "5"},

	{0   , 0xfe, 0   , 2   , "Bonus Life"},
	{0x11, 0x01, 0x20, 0x00, "Every 10000"},
	{0x11, 0x01, 0x20, 0x20, "10000 ONly"},

	{0   , 0xfe, 0   , 2   , "Difficulty"},
	{0x11, 0x01, 0x40, 0x00, "Start on Level 1"},
	{0x11, 0x01, 0x40, 0x40, "Start on Level 13"},

	{0   , 0xfe, 0   , 2   , "Bullets per Bean Collected"},
	{0x11, 0x01, 0x80, 0x00, "1"},
	{0x11, 0x01, 0x80, 0x80, "2"},

	// DSW2
	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x12, 0x01, 0x01, 0x01, "Upright"},
	{0x12, 0x01, 0x01, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Invulnerability (Cheat)"},
	{0x12, 0x01, 0x40, 0x00, "Off"},
	{0x12, 0x01, 0x40, 0x40, "On"},

	{0   , 0xfe, 0   , 4   , "255 Lives (Cheat)"},
	{0x12, 0x01, 0x80, 0x00, "Off"},
	{0x12, 0x01, 0x80, 0x80, "On"},
};

STDDIPINFO(Jack);


static struct BurnDIPInfo Jack2DIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0x00, NULL},
	{0x12, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coin B"},
	{0x11, 0x01, 0x03, 0x03, "4 coins 3 credits"},
	{0x11, 0x01, 0x03, 0x00, "1 coin 1 credit"},
	{0x11, 0x01, 0x03, 0x01, "1 coin 2 credits"},
	{0x11, 0x01, 0x03, 0x02, "1 coin 3 credits"},

	{0   , 0xfe, 0   , 4   , "Coin A"},
	{0x11, 0x01, 0x0c, 0x08, "3 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x04, "2 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x0c, "4 coins 3 credits"},
	{0x11, 0x01, 0x0c, 0x00, "1 coin 1 credits"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x11, 0x01, 0x10, 0x00, "3"},
	{0x11, 0x01, 0x10, 0x10, "5"},

	{0   , 0xfe, 0   , 2   , "Bonus Life"},
	{0x11, 0x01, 0x20, 0x00, "Every 10000"},
	{0x11, 0x01, 0x20, 0x20, "10000 ONly"},

	{0   , 0xfe, 0   , 2   , "Difficulty"},
	{0x11, 0x01, 0x40, 0x00, "Start on Level 1"},
	{0x11, 0x01, 0x40, 0x40, "Start on Level 13"},

	{0   , 0xfe, 0   , 2   , "Bullets per Bean Collected"},
	{0x11, 0x01, 0x80, 0x00, "1"},
	{0x11, 0x01, 0x80, 0x80, "2"},

	// DSW2
	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x12, 0x01, 0x01, 0x01, "Upright"},
	{0x12, 0x01, 0x01, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Invulnerability (Cheat)"},
	{0x12, 0x01, 0x40, 0x00, "Off"},
	{0x12, 0x01, 0x40, 0x40, "On"},

	{0   , 0xfe, 0   , 4   , "255 Lives (Cheat)"},
	{0x12, 0x01, 0x80, 0x00, "Off"},
	{0x12, 0x01, 0x80, 0x80, "On"},
};

STDDIPINFO(Jack2);

static struct BurnDIPInfo Jack3DIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0x00, NULL},
	{0x12, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coin B"},
	{0x11, 0x01, 0x03, 0x00, "1 coin 1 credit"},
	{0x11, 0x01, 0x03, 0x01, "1 coin 2 credits"},
	{0x11, 0x01, 0x03, 0x02, "1 coin 3 credits"},
	{0x11, 0x01, 0x03, 0x03, "1 coin 5 credits"},

	{0   , 0xfe, 0   , 4   , "Coin A"},
	{0x11, 0x01, 0x0c, 0x0c, "4 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x08, "3 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x04, "2 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x00, "1 coin 1 credit"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x11, 0x01, 0x10, 0x00, "3"},
	{0x11, 0x01, 0x10, 0x10, "5"},

	{0   , 0xfe, 0   , 2   , "Bonus Life"},
	{0x11, 0x01, 0x20, 0x00, "Every 10000"},
	{0x11, 0x01, 0x20, 0x20, "10000 ONly"},

	{0   , 0xfe, 0   , 2   , "Difficulty"},
	{0x11, 0x01, 0x40, 0x00, "Start on Level 1"},
	{0x11, 0x01, 0x40, 0x40, "Start on Level 13"},

	{0   , 0xfe, 0   , 2   , "Bullets per Bean Collected"},
	{0x11, 0x01, 0x80, 0x00, "1"},
	{0x11, 0x01, 0x80, 0x80, "2"},

	// DSW2
	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x12, 0x01, 0x01, 0x01, "Upright"},
	{0x12, 0x01, 0x01, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Invulnerability (Cheat)"},
	{0x12, 0x01, 0x40, 0x00, "Off"},
	{0x12, 0x01, 0x40, 0x40, "On"},

	{0   , 0xfe, 0   , 4   , "255 Lives (Cheat)"},
	{0x12, 0x01, 0x80, 0x00, "Off"},
	{0x12, 0x01, 0x80, 0x80, "On"},
};

STDDIPINFO(Jack3);

static struct BurnDIPInfo TreahuntDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0x00, NULL},
	{0x12, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coin B"},
	{0x11, 0x01, 0x03, 0x01, "2 coins 1 credit"},
	{0x11, 0x01, 0x03, 0x03, "4 coins 3 credits"},
	{0x11, 0x01, 0x03, 0x00, "1 coin 1 credit"},
	{0x11, 0x01, 0x03, 0x02, "1 coin 3 credits"},

	{0   , 0xfe, 0   , 4   , "Coin A"},
	{0x11, 0x01, 0x0c, 0x08, "3 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x04, "2 coins 1 credit"},
	{0x11, 0x01, 0x0c, 0x0c, "4 coins 3 credits"},
	{0x11, 0x01, 0x0c, 0x00, "1 coin 1 credit"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x11, 0x01, 0x10, 0x00, "3"},
	{0x11, 0x01, 0x10, 0x10, "5"},

	{0   , 0xfe, 0   , 2   , "Bonus Life"},
	{0x11, 0x01, 0x20, 0x00, "Every 10000"},
	{0x11, 0x01, 0x20, 0x20, "10000 ONly"},

	{0   , 0xfe, 0   , 2   , "Difficulty"},
	{0x11, 0x01, 0x40, 0x00, "Start on Level 1"},
	{0x11, 0x01, 0x40, 0x40, "Start on Level 6"},

	{0   , 0xfe, 0   , 2   , "Bullets per Bean Collected"},
	{0x11, 0x01, 0x80, 0x00, "5"},
	{0x11, 0x01, 0x80, 0x80, "20"},

	// DSW2
	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x12, 0x01, 0x01, 0x01, "Upright"},
	{0x12, 0x01, 0x01, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Invulnerability (Cheat)"},
	{0x12, 0x01, 0x40, 0x00, "Off"},
	{0x12, 0x01, 0x40, 0x40, "On"},

	{0   , 0xfe, 0   , 4   , "255 Lives (Cheat)"},
	{0x12, 0x01, 0x80, 0x00, "Off"},
	{0x12, 0x01, 0x80, 0x80, "On"},
};

STDDIPINFO(Treahunt);


static struct BurnDIPInfo ZzyzzyxxDIPList[]=
{
	// Default Values
	{0x0A, 0xff, 0xff, 0x00, NULL},
	{0x0B, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coinage"},
	{0x0A, 0x01, 0x03, 0x01, "2 coins 1 credit"},
	{0x0A, 0x01, 0x03, 0x03, "4 coins 3 credits"},
	{0x0A, 0x01, 0x03, 0x00, "1 coin 1 credit"},
	{0x0A, 0x01, 0x03, 0x02, "1 coin 3 credits"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x0A, 0x01, 0x04, 0x00, "3"},
	{0x0A, 0x01, 0x04, 0x04, "2"},

	{0   , 0xfe, 0   , 2   , "Credits on Reset"},
	{0x0A, 0x01, 0x08, 0x00, "Off"},
	{0x0A, 0x01, 0x08, 0x08, "On"},

	{0   , 0xfe, 0   , 2   , "Demo Sounds"},
	{0x0A, 0x01, 0x10, 0x10, "Off"},
	{0x0A, 0x01, 0x10, 0x00, "On"},

	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x0A, 0x01, 0x20, 0x20, "Upright"},
	{0x0A, 0x01, 0x20, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Free Play"},
	{0x0A, 0x01, 0x80, 0x00, "Off"},
	{0x0A, 0x01, 0x80, 0x80, "On"},

	// DSW2
	{0   , 0xfe, 0   , 2   , "Bonus Life"},
	{0x0B, 0x01, 0x03, 0x02, "None"},
	{0x0B, 0x01, 0x03, 0x00, "10000 50000"},
	{0x0B, 0x01, 0x03, 0x01, "25000 100000"},
	{0x0B, 0x01, 0x03, 0x03, "100000 300000"},

	{0   , 0xfe, 0   , 2   , "2nd Bonus Given"},
	{0x0B, 0x01, 0x04, 0x00, "No"},
	{0x0B, 0x01, 0x04, 0x04, "Yes"},

	{0   , 0xfe, 0   , 2   , "Starting Laps"},
	{0x0B, 0x01, 0x08, 0x00, "2"},
	{0x0B, 0x01, 0x08, 0x08, "3"},

	{0   , 0xfe, 0   , 2   , "Difficulty of Pleasing Lola"},
	{0x0B, 0x01, 0x10, 0x00, "Easy"},
	{0x0B, 0x01, 0x10, 0x10, "Hard"},

	{0   , 0xfe, 0   , 2   , "Show Intermissions"},
	{0x0B, 0x01, 0x20, 0x00, "No"},
	{0x0B, 0x01, 0x20, 0x20, "Yes"},

	{0   , 0xfe, 0   , 3   , "Show Intermissions"},
	{0x0B, 0x01, 0xc0, 0x00, "3 under 4000 pts"},
	{0x0B, 0x01, 0xc0, 0x80, "5 under 4000 pts"},
	{0x0B, 0x01, 0xc0, 0x40, "None"},
};

STDDIPINFO(Zzyzzyxx);

static struct BurnDIPInfo FreezeDIPList[]=
{
	// Default Values
	{0x08, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 2   , "Flip Screen"},
	{0x08, 0x01, 0x01, 0x00, "Off"},
	{0x08, 0x01, 0x01, 0x01, "On"},

	{0   , 0xfe, 0   , 2   , "Difficulty"},
	{0x08, 0x01, 0x04, 0x00, "Easy"},
	{0x08, 0x01, 0x04, 0x04, "Hard"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x08, 0x01, 0x08, 0x00, "3"},
	{0x08, 0x01, 0x08, 0x08, "5"},

	{0   , 0xfe, 0   , 4   , "Bonus Life"},
	{0x08, 0x01, 0x30, 0x00, "10000"},
	{0x08, 0x01, 0x30, 0x10, "10000 40000"},
	{0x08, 0x01, 0x30, 0x20, "10000 60000"},
	{0x08, 0x01, 0x30, 0x30, "20000 100000"},

	{0   , 0xfe, 0   , 4   , "Coinage"},
	{0x08, 0x01, 0xc0, 0x80, "2 Coins 1 Credit"},
	{0x08, 0x01, 0xc0, 0x00, "1 Coin 1 Credit"},
	{0x08, 0x01, 0xc0, 0x40, "1 Coin 2 Credits"},
	{0x08, 0x01, 0xc0, 0xc0, "Free Play"},
};

STDDIPINFO(Freeze);

static struct BurnDIPInfo SucasinoDIPList[]=
{
	// Default Values
	{0x0A, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coinage"},
	{0x0A, 0x01, 0x03, 0x00, "1 Coin 1 Credit"},
	{0x0A, 0x01, 0x03, 0x01, "1 Coin 2 Credits"},
	{0x0A, 0x01, 0x03, 0x02, "1 Coin 3 Credits"},
	{0x0A, 0x01, 0x03, 0x03, "1 Coin 4 Credits"},

	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x0A, 0x01, 0x04, 0x00, "Upright"},
	{0x0A, 0x01, 0x04, 0x04, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Flip Screen"},
	{0x0A, 0x01, 0x08, 0x00, "Off"},
	{0x0A, 0x01, 0x08, 0x08, "On"},
};

STDDIPINFO(Sucasino);

static struct BurnDIPInfo JoinemDIPList[]=
{
	// Default Values
	{0x0d, 0xff, 0xff, 0x00, NULL},
	{0x0e, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 4   , "Coin A"},
	{0x0d, 0x01, 0x03, 0x01, "2 coins 1 credit"},
	{0x0d, 0x01, 0x03, 0x03, "4 coins 3 credits"},
	{0x0d, 0x01, 0x03, 0x00, "1 coin 1 credit"},
	{0x0d, 0x01, 0x03, 0x02, "1 coin 3 credits"},

	{0   , 0xfe, 0   , 4   , "Coin B"},
	{0x0d, 0x01, 0x0c, 0x08, "3 coins 1 credit"},
	{0x0d, 0x01, 0x0c, 0x04, "2 coins 1 credit"},
	{0x0d, 0x01, 0x0c, 0x0c, "4 coins 3 credits"},
	{0x0d, 0x01, 0x0c, 0x00, "1 coin 1 credit"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x0d, 0x01, 0x10, 0x00, "2"},
	{0x0d, 0x01, 0x10, 0x10, "5"},

	// DSW2
	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x0e, 0x01, 0x01, 0x01, "Upright"},
	{0x0e, 0x01, 0x01, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Sound Check"},
	{0x0e, 0x01, 0x20, 0x00, "Off"},
	{0x0e, 0x01, 0x20, 0x20, "On"},

	{0   , 0xfe, 0   , 2   , "Infinite Lives"},
	{0x0e, 0x01, 0x80, 0x00, "2"},
	{0x0e, 0x01, 0x80, 0x80, "3"},
};

STDDIPINFO(Joinem);

static struct BurnDIPInfo LoverboyDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0x00, NULL},

	// DSW1
	{0   , 0xfe, 0   , 15   , "Coin A"},
	{0x0f, 0x01, 0x0f, 0x0c, "4 Coins 1 Credit"},
	{0x0f, 0x01, 0x0f, 0x08, "3 Coins 1 Credit"},
	{0x0f, 0x01, 0x0f, 0x0d, "4 Coins 2 Credit"},
	{0x0f, 0x01, 0x0f, 0x04, "2 Coins 1 Credit"},
	{0x0f, 0x01, 0x0f, 0x09, "3 Coins 2 Credit"},
	{0x0f, 0x01, 0x0f, 0x0e, "4 Coins 3 Credit"},
	{0x0f, 0x01, 0x0f, 0x0f, "4 Coins 4 Credit"},
	{0x0f, 0x01, 0x0f, 0x0a, "3 Coins 3 Credit"},
	{0x0f, 0x01, 0x0f, 0x05, "2 Coins 2 Credit"},
	{0x0f, 0x01, 0x0f, 0x00, "1 Coins 1 Credit"},
	{0x0f, 0x01, 0x0f, 0x0b, "3 Coins 4 Credit"},
	{0x0f, 0x01, 0x0f, 0x06, "2 Coins 3 Credit"},
	{0x0f, 0x01, 0x0f, 0x07, "2 Coins 4 Credit"},
	{0x0f, 0x01, 0x0f, 0x01, "1 Coins 2 Credit"},
	{0x0f, 0x01, 0x0f, 0x02, "1 Coins 3 Credit"},
	{0x0f, 0x01, 0x0f, 0x03, "1 Coins 4 Credit"},

	{0   , 0xfe, 0   , 2   , "Bonus"},
	{0x0f, 0x01, 0x20, 0x00, "20,000"},
	{0x0f, 0x01, 0x20, 0x20, "30,000"},

	{0   , 0xfe, 0   , 2   , "Lives"},
	{0x0f, 0x01, 0x40, 0x00, "3"},
	{0x0f, 0x01, 0x40, 0x40, "5"},

	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x0f, 0x01, 0x80, 0x80, "Upright"},
	{0x0f, 0x01, 0x80, 0x00, "Cocktail"},
};

STDDIPINFO(Loverboy);

static struct BurnDIPInfo StrivDIPList[]=
{
	// Default Values
	{0x0A, 0xff, 0xff, 0x80, NULL},

	// DSW1
	{0   , 0xfe, 0   , 2   , "Monitor"},
	{0x0A, 0x01, 0x02, 0x02, "Horizontal"},
	{0x0A, 0x01, 0x02, 0x00, "Vertical"},

	{0   , 0xfe, 0   , 8   , "Gaming Option Number"},
	{0x0A, 0x01, 0x05, 0x01, "2"},
	{0x0A, 0x01, 0x05, 0x05, "3"},
	{0x0A, 0x01, 0x05, 0x00, "4"},
	{0x0A, 0x01, 0x05, 0x04, "5"},
	{0x0A, 0x01, 0x05, 0x01, "4"},
	{0x0A, 0x01, 0x05, 0x05, "5"},
	{0x0A, 0x01, 0x05, 0x00, "6"},
	{0x0A, 0x01, 0x05, 0x04, "7"},

	{0   , 0xfe, 0   , 2   , "Cabinet"},
	{0x0A, 0x01, 0x08, 0x08, "Upright"},
	{0x0A, 0x01, 0x08, 0x00, "Cocktail"},

	{0   , 0xfe, 0   , 2   , "Coinage"},
	{0x0A, 0x01, 0x10, 0x00, "2 Coins 1 Credit"},
	{0x0A, 0x01, 0x10, 0x10, "1 Coin 1 Credit"},

	{0   , 0xfe, 0   , 2   , "Gaming Options"},
	{0x0A, 0x01, 0x20, 0x20, "Number of Wrong Answer"},
	{0x0A, 0x01, 0x20, 0x00, "Number of Questions"},

	{0   , 0xfe, 0   , 2   , "Show Correct Answer"},
	{0x0A, 0x01, 0x40, 0x00, "No"},
	{0x0A, 0x01, 0x40, 0x40, "Yes"},
};

STDDIPINFO(Striv);

static unsigned char timer_r(unsigned int)
{
	return ZetTotalCycles() / timer_rate;
}

static unsigned char soundlatch_r(unsigned int)
{
	return soundlatch;
}

static unsigned char __fastcall striv_question_r(unsigned short offset)
{
	if((offset & 0xc00) == 0x800)
	{
		remap_address[offset & 0x0f] = (offset & 0xf0) >> 4;
	}
	else if((offset & 0xc00) == 0xc00)
	{
		question_rom = offset & 7;
		question_address = (offset & 0xf8) << 7;
	}
	else
	{
		unsigned char *ROM = User;
		int real_address;

		real_address = question_address | (offset & 0x3f0) | remap_address[offset & 0x0f];

		if(offset & 0x400)
			real_address |= 0x8000 * (question_rom + 8);
		else
			real_address |= 0x8000 * question_rom;

		return ROM[real_address];
	}

	return 0;
}

void jack_paletteram_w(unsigned short offset, unsigned char data)
{
	unsigned int *pl = Palette + (offset & 0x1f);

	Rom0[offset] = data;

	DrvCalcPal = 1;

	data ^= 0xff;

	*pl  = ((data & 7) << 21) | ((data & 7) << 18) | ((data & 6) << 15);

	data >>= 3;

	*pl |= ((data & 7) << 13) | ((data & 7) << 10) | ((data & 6) <<  7);

	data >>= 3;

	*pl |= ((data & 3) <<  6) | ((data & 3) <<  4) | ((data & 3) <<  2) | (data & 3);
}

unsigned char __fastcall jack_cpu0_read(unsigned short address)
{
	unsigned char ret = 0;

	switch (address)
	{
		case 0xb500:
			return DrvDips[0];

		case 0xb501:
		{
			ret = DrvDips[1];
			if (joinem && DrvJoy2[7] && !joinem_snd_bit) ret |= 0x20;

			return ret;
		}

		case 0xb502:
		{
			for (int i = 0; i < 8; i++) ret |= DrvJoy1[i] << i;

			return ret;
		}

		case 0xb503:
		{
			for (int i = 0; i < 8; i++) ret |= DrvJoy2[i] << i;

			return ret;
		}

		case 0xb504:
		{
			for (int i = 0; i < 8; i++) ret |= DrvJoy3[i] << i;
			if (joinem || loverb) ret |= 0x40;

			return ret;
		}

		case 0xb505:
		{
			for (int i = 0; i < 8; i++) ret |= DrvJoy4[i] << i;

			return ret;
		}

		case 0xb506:
		case 0xb507:
			flip_screen = (address & 1) ^ suprtriv;
			return 0;
	}

	if (suprtriv && address >= 0xc000 && address <= 0xcfff) {
		return striv_question_r(address & 0x0fff);
	}

	return 0;
}

void __fastcall jack_cpu0_write(unsigned short address, unsigned char data)
{
	if (address >= 0xb600 && address <= 0xb61f)
	{
		jack_paletteram_w(address, data);
		return;
	}

	switch (address)
	{
		case 0xb400:
			soundlatch = data;
			snd_cpu_irq = 1;
		break;

		case 0xb506:
			flip_screen = 1;
		break;

		case 0xb700:
			flip_screen = data >> 7;
			joinem_snd_bit = data & 1;
		break;
	}
}

unsigned char __fastcall jack_in_port(unsigned short port)
{
	switch (port & 0xff)
	{
		case 0x40:
			return AY8910Read(0);
	}

	return 0;
}

void __fastcall jack_out_port(unsigned short address, unsigned char data)
{
	switch (address & 0xff)
	{
		case 0x40:
			AY8910Write(0, 1, data);
		break;

		case 0x80:
			AY8910Write(0, 0, data);
		break;
	}
}

void __fastcall jack_cpu1_write()
{
}


static int DrvDoReset()
{
	flip_screen = 0;
	DrvReset = 0;
	soundlatch = 0;

	if (loverb || joinem) {
		memset (Rom0 + 0x8000, 0, 0x1000);
	} else {
		memset ((unsigned char*)Palette, 0, 0x400);
		memset (Rom0 + 0x4000, 0, 0x1000);
	}

	memset (Rom0 + 0xb000, 0, 0x1000);
	memset (Rom1 + 0x4000, 0, 0x0400);
	memset ((unsigned char*)remap_address, 0, 0x40); 

	question_address = question_rom = 0;
	joinem_snd_bit = 0;
	soundlatch = 0;

	for (int i = 0; i < 2; i++) {
		ZetOpen(i);
		ZetReset();
		ZetClose();
	}

	AY8910Reset(0);

	return 0;
}

static int GetRoms()
{
	char* pRomName;
	struct BurnRomInfo ri;
	unsigned char *Load0 = Rom0;
	unsigned char *Load1 = Rom1;
	unsigned char *Loadg = Gfx;
	unsigned char *Loadt = User;
	int gCount = 0;

	if (!joinem && !loverb) Loadg += 0x2000;

	for (int i = 0; !BurnDrvGetRomName(&pRomName, i, 0); i++) {

		BurnDrvGetRomInfo(&ri, i);

		if ((ri.nType & 7) == 1) {
			if (ri.nLen == 0x2000)
			{
				if (BurnLoadRom(Load0, i, 1)) return 1;
				Load0 += ri.nLen;
			}
			else
			{
				if (BurnLoadRom(Load0, i, 1)) return 1;
				Load0 += ri.nLen;
	
				if (tri_fix && i == 0) {
					Load0 += 0x1000;
				}
	
				if (i == (3 - tri_fix)) Load0 += 0x8000;
			}

			continue;
		}

		if ((ri.nType & 7) == 2) {
			if (BurnLoadRom(Load1, i, 1)) return 1;
			Load1 += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 3) {
			if (BurnLoadRom(Loadg, i, 1)) return 1;
			Loadg += ri.nLen;
			if (joinem) Loadg += 0x1000;
			gCount++;

			continue;
		}

		if ((ri.nType & 7) == 4) {
			if (BurnLoadRom(Prom + 0x000, i + 0, 1)) return 1;
			if (BurnLoadRom(Prom + 0x200, i + 1, 1)) return 1;
			i++;

			// Roms are nibbles, (1/2 byte), #0 is low, #1 is high
			for (int j = 0; j < 0x200; j++) {
				Prom[j] = Prom[j] | (Prom[j + 0x200] << 4);
			}

			continue;
		}

		if ((ri.nType & 7) == 5) {
			if (BurnLoadRom(Loadt, i, 1)) return 1;
			Loadt += ri.nLen;

			continue;
		}		
	}

	// sucasino, tripool, tripoola
	if (gCount == 2) {
		memcpy (Gfx + 0x4000, Gfx + 0x3000, 0x1000);
		memset (Gfx + 0x3000, 0, 0x1000);
	} 

	return 0;
}

static void gfx_decode()
{
	unsigned char* tmp = (unsigned char*)malloc( 0x2000 * 3 );
	if (!tmp) return;

	memcpy (tmp, Gfx, 0x6000);

	static int Planes[3] = { 0, 1024*8*8, 1024*8*8*2 };
	static int YOffs[8]  = { 0, 1, 2, 3, 4, 5, 6, 7 };
	static int XOffs[8]  = { 56, 48, 40, 32, 24, 16, 8, 0 };

	GfxDecode(1024, 3, 8, 8, Planes, XOffs, YOffs, 64, tmp, Gfx);

	free (tmp);
}

static int DrvInit()
{
	Mem = (unsigned char *)malloc (0x100000);
	if (Mem == NULL) {
		return 1;
	}

	pFMBuffer = (short *)malloc (nBurnSoundLen * 3 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	Rom0 = Mem + 0x00000;
	Rom1 = Mem + 0x10000;
	Gfx  = Mem + 0x20000;
	User = Mem + 0x30000;
	Prom = Mem + 0xb0000;
	Palette = (unsigned int*)(Mem + 0xc0000);
	DrvPal  = (unsigned int*)(Mem + 0xc1000);

	GetRoms();
	
	gfx_decode();

	ZetInit(2);
	ZetOpen(0);
	ZetSetReadHandler(jack_cpu0_read);
	ZetSetWriteHandler(jack_cpu0_write);

	if (joinem || loverb)
	{
		ZetMapArea(0x0000, 0x8fff, 0, Rom0 + 0x0000);
		ZetMapArea(0x0000, 0x8fff, 2, Rom0 + 0x0000);

		ZetMapArea(0x8000, 0x8fff, 1, Rom0 + 0x8000);

		ZetMapArea(0xb500, 0xb5ff, 0, Rom0 + 0xb500); // controls hack
	} else {
		ZetMapArea(0x0000, 0x3fff, 0, Rom0 + 0x0000);
		ZetMapArea(0x0000, 0x3fff, 2, Rom0 + 0x0000);

		ZetMapArea(0x4000, 0x5fff, 0, Rom0 + 0x4000);
		ZetMapArea(0x4000, 0x5fff, 1, Rom0 + 0x4000);
		ZetMapArea(0x4000, 0x5fff, 2, Rom0 + 0x4000);
	}

	ZetMapArea(0xb000, 0xb0ff, 0, Rom0 + 0xb000);
	ZetMapArea(0xb000, 0xb0ff, 1, Rom0 + 0xb000);

	ZetMapArea(0xb800, 0xbbff, 0, Rom0 + 0xb800);
	ZetMapArea(0xb800, 0xbbff, 1, Rom0 + 0xb800);	

	ZetMapArea(0xbc00, 0xbfff, 0, Rom0 + 0xbc00);
	ZetMapArea(0xbc00, 0xbfff, 1, Rom0 + 0xbc00);

	if (suprtriv)
	{
		ZetMapArea(0xd000, 0xffff, 0, Rom0 + 0xc000);
		ZetMapArea(0xd000, 0xffff, 2, Rom0 + 0xc000);
	} else {
		ZetMapArea(0xc000, 0xffff, 0, Rom0 + 0xc000);
		ZetMapArea(0xc000, 0xffff, 2, Rom0 + 0xc000);
	}

	ZetMemEnd();
	ZetClose();

	ZetOpen(1);
	ZetSetInHandler(jack_in_port);
	ZetSetOutHandler(jack_out_port);
	ZetMapArea(0x0000, 0x1fff, 0, Rom1 + 0x0000);
	ZetMapArea(0x0000, 0x1fff, 2, Rom1 + 0x0000);
	ZetMapArea(0x4000, 0x43ff, 0, Rom1 + 0x4000);
	ZetMapArea(0x4000, 0x43ff, 1, Rom1 + 0x4000);
	ZetMapArea(0x4000, 0x43ff, 2, Rom1 + 0x4000);
	ZetMemEnd();
	ZetClose();

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;

	AY8910Init(0, 1500000, nBurnSoundRate, &soundlatch_r, &timer_r, NULL, NULL);

	GenericTilesInit();

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	ZetExit();
	AY8910Exit(0);
	GenericTilesExit();

	free (pFMBuffer);
	free (Mem);

	DrvPal = Palette = NULL;
	pFMBuffer = NULL;
	Mem = Rom0 = Rom1 = Gfx = User = NULL;
	pAY8910Buffer[0] = pAY8910Buffer[1] = pAY8910Buffer[2] = NULL;

	tri_fix = 0;
	joinem = 0;
	loverb = 0;
	suprtriv = 0;

	return 0;
}


static int DrvDraw()
{
	if (DrvCalcPal)
	{
		for (int i = 0; i < 0x100; i++) {
			unsigned int col = Palette[i];
			DrvPal[i] = BurnHighCol(col >> 16, col >> 8, col, 0);
		}
		DrvCalcPal = 0;
	}

	unsigned char *sram = Rom0 + 0xb000; // sprite ram
	unsigned char *vram = Rom0 + 0xb800; // video ram
	unsigned char *cram = Rom0 + 0xbc00; // color ram
	int offs, sx, sy, num, color, flipx, flipy;

	for (offs = 0; offs < 0x400; offs++)
	{
		sx = (offs & 0x1f) << 3;
		sy = (offs >> 2) & 0xf8;

		if (joinem || loverb) {
			num = vram[offs] + ((cram[offs] & 0x03) << 8);
			color = (cram[offs] & 0x38) >> 2;
		} else {
			num = vram[offs] + ((cram[offs] & 0x18) << 5);
			color = (cram[offs] & 0x07);
		}

		if (flip_screen) {
			Render8x8Tile_FlipXY_Clip(pTransDraw, num, sx ^ 0xf8, sy ^ 0xf8, color, 2, 0, Gfx);
		} else {
			sx -= 16;
			Render8x8Tile_Clip(pTransDraw, num, sx, sy, color, 2, 0, Gfx);
		}
	}

	for (offs = 0; offs < 0x80; offs += 4)
	{
		sx = 248 - sram[offs];
		sy = sram[offs + 1];

		if (joinem || loverb) {
			num   = sram[offs + 2] + ((sram[offs + 3] & 0x01) << 8);
			color = (sram[offs + 3] & 0x38) >> 2;
		} else {
			num   = sram[offs + 2] + ((sram[offs + 3] & 0x08) << 5);
			color = (sram[offs + 3] & 0x07);
		}

		flipx = sram[offs + 3] & 0x40;
		flipy = sram[offs + 3] & 0x80;

		if (flip_screen) {
			flipx = !flipx;
			flipy = !flipy;
			sx = 248 - sx;
			sy = 248 - sy;
		}

		sx -= 16;

		if (flipy) {
			if (flipx) {
				Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, num, sx, sy, color, 2, 0, 0, Gfx);
			} else {
				Render8x8Tile_Mask_FlipY_Clip(pTransDraw, num, sx, sy, color, 2, 0, 0, Gfx);
			}
		} else {
			if (flipx) {
				Render8x8Tile_Mask_FlipX_Clip(pTransDraw, num, sx, sy, color, 2, 0, 0, Gfx);
			} else {
				Render8x8Tile_Mask_Clip(pTransDraw, num, sx, sy, color, 2, 0, 0, Gfx);
			}
		}
	}

	BurnTransferCopy(DrvPal);

	return 0;
}

static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	if (joinem || loverb) {
		for (int i = 0; i < 6; i++)
			Rom0[0xb500 + i] = jack_cpu0_read(0xb500 + i);
	}

	int nInterleave = 10;
	int nSoundBufferPos = 0;

	int nCyclesSegment;
	int nCyclesDone[2], nCyclesTotal[2];

	nCyclesTotal[0] = 3000000 / 60;
	nCyclesTotal[1] = 1500000 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run Z80 #0
		nCurrentCPU = 0;
		ZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += ZetRun(nCyclesSegment);

		if (joinem)
			if (i == (nInterleave / 3) || i == ((nInterleave / 3) * 2))
				ZetRaiseIrq(0);

		if (tri_fix && i == (nInterleave / 2)) ZetRaiseIrq(0);

		if (i == (nInterleave - 1))
		{
			if (joinem) {					// joinem
				if (!DrvJoy3[7]) ZetNmi();
			} else if (loverb) {				// loverboy
				ZetNmi();
			} else {					// other
				ZetRaiseIrq(0);
			}
		}

		ZetClose();

		// Run Z80 #1
		nCurrentCPU = 1;
		ZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (snd_cpu_irq) {
			ZetRaiseIrq(0xff);
			snd_cpu_irq = 0;
		}
		nCyclesSegment = ZetRun(nCyclesSegment);
		nCyclesDone[nCurrentCPU] += nCyclesSegment;
		ZetClose();

		// Render Sound Segment
		if (pBurnSoundOut && !suprtriv) {	// disable sound for suprtriv
			int nSample;
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			AY8910Update(0, &pAY8910Buffer[0], nSegmentLength);
			for (int n = 0; n < nSegmentLength; n++) {
				nSample  = pAY8910Buffer[0][n];
				nSample += pAY8910Buffer[1][n];
				nSample += pAY8910Buffer[2][n];

				nSample /= 4;

				if (nSample < -32768) {
					nSample = -32768;
				} else {
					if (nSample > 32767) {
						nSample = 32767;
					}
				}

				pSoundBuf[(n << 1) + 0] = nSample;
				pSoundBuf[(n << 1) + 1] = nSample;
			}
			nSoundBufferPos += nSegmentLength;
		}
	}

	// Make sure the buffer is entirely filled.
	if (pBurnSoundOut && !suprtriv) {
		int nSample;
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
			AY8910Update(0, &pAY8910Buffer[0], nSegmentLength);
			for (int n = 0; n < nSegmentLength; n++) {
				nSample  = pAY8910Buffer[0][n];
				nSample += pAY8910Buffer[1][n];
				nSample += pAY8910Buffer[2][n];

				nSample /= 4;

				if (nSample < -32768) {
					nSample = -32768;
				} else {
					if (nSample > 32767) {
						nSample = 32767;
					}
				}

				pSoundBuf[(n << 1) + 0] = nSample;
				pSoundBuf[(n << 1) + 1] = nSample;
			}
		}
	}

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}


//-------------------------------------------------------------------------------------
// Save states


static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {
		memset(&ba, 0, sizeof(ba));
		if (joinem || loverb) {
			ba.Data	  = Rom0 + 0x8000;
		} else {
			ba.Data	  = Rom0 + 0x4000;
		}
		ba.nLen	  = 0x01000;
		ba.szName = "Main Ram";
		BurnAcb(&ba);
	
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = Rom0 + 0xb000;
		ba.nLen	  = 0x01000;
		ba.szName = "Video Ram";
		BurnAcb(&ba);

		memset(&ba, 0, sizeof(ba));
		ba.Data	  = (unsigned char*)Palette;
		ba.nLen	  = 0x00400;
		ba.szName = "Palette";
		BurnAcb(&ba);

		memset(&ba, 0, sizeof(ba));
		ba.Data	  = (unsigned char*)remap_address;
		ba.nLen	  = 0x00040;
		ba.szName = "striv question addresses";
		BurnAcb(&ba);

		ZetScan(nAction);
		AY8910Scan(nAction, pnMin);

		// Scan critical driver variables
		SCAN_VAR(question_address);
		SCAN_VAR(question_rom);
		SCAN_VAR(soundlatch);
		SCAN_VAR(joinem_snd_bit);
	}

	return 0;
}


// Jack the Giantkiller (set 1)

static struct BurnRomInfo jackRomDesc[] = {
	{ "j8",           0x1000, 0xc8e73998, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "jgk.j6",       0x1000, 0x36d7810e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "jgk.j7",       0x1000, 0xb15ff3ee, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "jgk.j5",       0x1000, 0x4a63d242, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "jgk.j3",       0x1000, 0x605514a8, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "jgk.j4",       0x1000, 0xbce489b7, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "jgk.j2",       0x1000, 0xdb21bd55, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "jgk.j1",       0x1000, 0x49fffe31, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "jgk.j9",       0x1000, 0xc2dc1e00, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code

	{ "jgk.j12",      0x1000, 0xce726df0, 3 | BRF_GRA },	       //  9 Graphics
	{ "jgk.j13",      0x1000, 0x6aec2c8d, 3 | BRF_GRA },	       // 10
	{ "jgk.j11",      0x1000, 0xfd14c525, 3 | BRF_GRA },	       // 11
	{ "jgk.j10",      0x1000, 0xeab890b2, 3 | BRF_GRA },	       // 12
};

STD_ROM_PICK(jack);
STD_ROM_FN(jack);

static int jackInit()
{
	timer_rate = 128;

	return DrvInit();
}

struct BurnDriver BurnDrvjack = {
	"jack", NULL, NULL, "1982",
	"Jack the Giantkiller (set 1)\0", NULL, "Cinematronics", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, jackRomInfo, jackRomName, JackInputInfo, JackDIPInfo,
	jackInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Jack the Giantkiller (set 2)

static struct BurnRomInfo jack2RomDesc[] = {
	{ "jgk.j8",       0x1000, 0xfe229e20, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "jgk.j6",       0x1000, 0x36d7810e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "jgk.j7",       0x1000, 0xb15ff3ee, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "jgk.j5",       0x1000, 0x4a63d242, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "jgk.j3",       0x1000, 0x605514a8, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "jgk.j4",       0x1000, 0xbce489b7, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "jgk.j2",       0x1000, 0xdb21bd55, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "jgk.j1",       0x1000, 0x49fffe31, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "jgk.j9",       0x1000, 0xc2dc1e00, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code

	{ "jgk.j12",      0x1000, 0xce726df0, 3 | BRF_GRA },	       //  9 Graphics
	{ "jgk.j13",      0x1000, 0x6aec2c8d, 3 | BRF_GRA },	       // 10
	{ "jgk.j11",      0x1000, 0xfd14c525, 3 | BRF_GRA },	       // 11
	{ "jgk.j10",      0x1000, 0xeab890b2, 3 | BRF_GRA },	       // 12
};

STD_ROM_PICK(jack2);
STD_ROM_FN(jack2);

struct BurnDriver BurnDrvjack2 = {
	"jack2", "jack", NULL, "1982",
	"Jack the Giantkiller (set 2)\0", NULL, "Cinematronics", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, jack2RomInfo, jack2RomName, JackInputInfo, Jack2DIPInfo,
	jackInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Jack the Giantkiller (set 3)

static struct BurnRomInfo jack3RomDesc[] = {
	{ "jack8",        0x1000, 0x632151d2, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "jack6",        0x1000, 0xf94f80d9, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "jack7",        0x1000, 0xc830ff1e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "jack5",        0x1000, 0x8dea17e7, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "jgk.j3",       0x1000, 0x605514a8, 1 | BRF_PRG | BRF_ESS }, //  4	// bank 2
	{ "jgk.j4",       0x1000, 0xbce489b7, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "jgk.j2",       0x1000, 0xdb21bd55, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "jack1",        0x1000, 0x7e75ea3d, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "jgk.j9",       0x1000, 0xc2dc1e00, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code

	{ "jack12",       0x1000, 0x80320647, 3 | BRF_GRA },	       //  9 Graphics
	{ "jgk.j13",      0x1000, 0x6aec2c8d, 3 | BRF_GRA },	       // 10
	{ "jgk.j11",      0x1000, 0xfd14c525, 3 | BRF_GRA },	       // 11
	{ "jgk.j10",      0x1000, 0xeab890b2, 3 | BRF_GRA },	       // 12
};

STD_ROM_PICK(jack3);
STD_ROM_FN(jack3);

struct BurnDriver BurnDrvjack3 = {
	"jack3", "jack", NULL, "1982",
	"Jack the Giantkiller (set 3)\0", NULL, "Cinematronics", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, jack3RomInfo, jack3RomName, JackInputInfo, Jack3DIPInfo,
	jackInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Treasure Hunt (bootleg?)

static struct BurnRomInfo treahuntRomDesc[] = {
	{ "thunt-1.f2",   0x1000, 0x0b35858c, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "thunt-2.f3",   0x1000, 0x67305a51, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "thunt-3.4f",   0x1000, 0xd7a969c3, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "thunt-4.6f",   0x1000, 0x2483f14d, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "thunt-5.7f",   0x1000, 0xc69d5e21, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "thunt-6.7e",   0x1000, 0x11bf3d49, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "thunt-7.6e",   0x1000, 0x7c2d6279, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "thunt-8.4e",   0x1000, 0xf73b86fb, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "jgk.j9",       0x1000, 0xc2dc1e00, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code

	{ "thunt-13.a4",  0x1000, 0xe03f1f09, 3 | BRF_GRA },	       //  9 Graphics
	{ "thunt-12.a3",  0x1000, 0xda4ee9eb, 3 | BRF_GRA },	       // 10
	{ "thunt-10.a1",  0x1000, 0x51ec7934, 3 | BRF_GRA },	       // 11
	{ "thunt-11.a2",  0x1000, 0xf9781143, 3 | BRF_GRA },	       // 12
};

STD_ROM_PICK(treahunt);
STD_ROM_FN(treahunt);

static void treahunt_decode()
{
	for (int i = 0; i < 0x4000; i++)
	{
		if (i & 0x1000)
		{
			User[i] = BITSWAP08(Rom0[i], 0, 2, 5, 1, 3, 6, 4, 7);

			if (~i & 0x04) User[i] ^= 0x81;
		}
		else
		{
			User[i] = BITSWAP08(Rom0[i], 7, 2, 5, 1, 3, 6, 4, 0) ^ 0x81;
		}
	}

	ZetOpen(0);
	ZetMapArea(0x0000, 0x3fff, 2, User, Rom0);
	ZetClose();
}

static int treahuntInit()
{
	timer_rate = 128;

	int nRet = DrvInit();

	treahunt_decode();

	return nRet;
}

struct BurnDriver BurnDrvtreahunt = {
	"treahunt", "jack", NULL, "1982",
	"Treasure Hunt (bootleg?)\0", NULL, "Hara Industries", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, treahuntRomInfo, treahuntRomName, JackInputInfo, TreahuntDIPInfo,
	treahuntInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Zzyzzyxx (set 1)

static struct BurnRomInfo zzyzzyxxRomDesc[] = {
	{ "a.2f",         0x1000, 0xa9102e34, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "zzyzzyxx.b",   0x1000, 0xefa9d4c6, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "zzyzzyxx.c",   0x1000, 0xb0a365b1, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "zzyzzyxx.d",   0x1000, 0x5ed6dd9a, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "zzyzzyxx.e",   0x1000, 0x5966fdbf, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "f.7e",         0x1000, 0x12f24c68, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "g.6e",         0x1000, 0x408f2326, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "h.4e",         0x1000, 0xf8bbabe0, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "i.5a",         0x1000, 0xc7742460, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code
	{ "j.6a",         0x1000, 0x72166ccd, 2 | BRF_PRG | BRF_ESS }, //  9

	{ "n.1c",         0x1000, 0x4f64538d, 3 | BRF_GRA },	       // 10 Graphics
	{ "m.1d",         0x1000, 0x217b1402, 3 | BRF_GRA },	       // 11
	{ "k.1b",         0x1000, 0xb8b2b8cc, 3 | BRF_GRA },	       // 12
	{ "l.1a",         0x1000, 0xab421a83, 3 | BRF_GRA },	       // 13
};

STD_ROM_PICK(zzyzzyxx);
STD_ROM_FN(zzyzzyxx);

static int zzyzzyxxInit()
{
	timer_rate = 1;

	return DrvInit();
}

struct BurnDriver BurnDrvzzyzzyxx = {
	"zzyzzyxx", NULL, NULL, "1982",
	"Zzyzzyxx (set 1)\0", NULL, "Cinematronics + Advanced Microcomputer Systems", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, zzyzzyxxRomInfo, zzyzzyxxRomName, ZzyzzyxxInputInfo, ZzyzzyxxDIPInfo,
	zzyzzyxxInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Zzyzzyxx (set 2)

static struct BurnRomInfo zzyzzyx2RomDesc[] = {
	{ "a.2f",         0x1000, 0xa9102e34, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "b.3f",         0x1000, 0x4277beab, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "c.4f",         0x1000, 0x72ac99e1, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "d.6f",         0x1000, 0x7c7eec2b, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "e.7f",         0x1000, 0xcffc4a68, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "f.7e",         0x1000, 0x12f24c68, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "g.6e",         0x1000, 0x408f2326, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "h.4e",         0x1000, 0xf8bbabe0, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "i.5a",         0x1000, 0xc7742460, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code
	{ "j.6a",         0x1000, 0x72166ccd, 2 | BRF_PRG | BRF_ESS }, //  9

	{ "n.1c",         0x1000, 0x4f64538d, 3 | BRF_GRA },	       // 10 Graphics
	{ "m.1d",         0x1000, 0x217b1402, 3 | BRF_GRA },	       // 11
	{ "k.1b",         0x1000, 0xb8b2b8cc, 3 | BRF_GRA },	       // 12
	{ "l.1a",         0x1000, 0xab421a83, 3 | BRF_GRA },	       // 13
};

STD_ROM_PICK(zzyzzyx2);
STD_ROM_FN(zzyzzyx2);

struct BurnDriver BurnDrvzzyzzyx2 = {
	"zzyzzyx2", "zzyzzyxx", NULL, "1982",
	"Zzyzzyxx (set 2)\0", NULL, "Cinematronics + Advanced Microcomputer Systems", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, zzyzzyx2RomInfo, zzyzzyx2RomName, ZzyzzyxxInputInfo, ZzyzzyxxDIPInfo,
	zzyzzyxxInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Brix

static struct BurnRomInfo brixRomDesc[] = {
	{ "a",            0x1000, 0x050e0d70, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "b",            0x1000, 0x668118ae, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "c",            0x1000, 0xff5ed6cf, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "d",            0x1000, 0xc3ae45a9, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "e",            0x1000, 0xdef99fa9, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "f",            0x1000, 0xdde717ed, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "g",            0x1000, 0xadca02d8, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "h",            0x1000, 0xbc3b878c, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "i.5a",         0x1000, 0xc7742460, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code
	{ "j.6a",         0x1000, 0x72166ccd, 2 | BRF_PRG | BRF_ESS }, //  9

	{ "n",            0x1000, 0x8064910e, 3 | BRF_GRA },	       // 10 Graphics
	{ "m.1d",         0x1000, 0x217b1402, 3 | BRF_GRA },	       // 11
	{ "k",            0x1000, 0xc7d7e2a0, 3 | BRF_GRA },	       // 12
	{ "l.1a",         0x1000, 0xab421a83, 3 | BRF_GRA },	       // 13
};

STD_ROM_PICK(brix);
STD_ROM_FN(brix);

struct BurnDriver BurnDrvbrix = {
	"brix", "zzyzzyxx", NULL, "1982",
	"Brix\0", NULL, "Cinematronics + Advanced Microcomputer Systems", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, brixRomInfo, brixRomName, ZzyzzyxxInputInfo, ZzyzzyxxDIPInfo,
	zzyzzyxxInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Freeze

static struct BurnRomInfo freezeRomDesc[] = {
	{ "freeze.f2",    0x1000, 0x0a431665, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "freeze.f3",    0x1000, 0x1189b8ad, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "freeze.f4",    0x1000, 0x10c4a5ea, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "freeze.f5",    0x1000, 0x16024c53, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "freeze.f7",    0x1000, 0xea0b0765, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "freeze.e7",    0x1000, 0x1155c00b, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "freeze.e5",    0x1000, 0x95c18d75, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "freeze.e4",    0x1000, 0x7e8f5afc, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "freeze.a1",    0x1000, 0x7771f5b9, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code

	{ "freeze.5a",    0x1000, 0x6c8a98a0, 3 | BRF_GRA },	       //  9 Graphics
	{ "freeze.3a",    0x1000, 0x6d2125e4, 3 | BRF_GRA },	       // 10
	{ "freeze.1a",    0x1000, 0x3a7f2fa9, 3 | BRF_GRA },	       // 11
	{ "freeze.2a",    0x1000, 0xdd70ddd6, 3 | BRF_GRA },	       // 12
};

STD_ROM_PICK(freeze);
STD_ROM_FN(freeze);

struct BurnDriver BurnDrvfreeze = {
	"freeze", NULL, NULL, "1984",
	"Freeze\0", NULL, "Cinematronics", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, freezeRomInfo, freezeRomName, FreezeInputInfo, FreezeDIPInfo,
	jackInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Super Casino

static struct BurnRomInfo sucasinoRomDesc[] = {
	{ "1",       	  0x1000, 0xe116e979, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2",      	  0x1000, 0x2a2635f5, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "3",       	  0x1000, 0x69864d90, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "4",       	  0x1000, 0x174c9373, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "5",       	  0x1000, 0x115bcb1e, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "6",       	  0x1000, 0x434caa17, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "7",       	  0x1000, 0x67c68b82, 1 | BRF_PRG | BRF_ESS }, //  6
	{ "8",       	  0x1000, 0xf5b63006, 1 | BRF_PRG | BRF_ESS }, //  7

	{ "9",       	  0x1000, 0x67cf8aec, 2 | BRF_PRG | BRF_ESS }, //  8 Z80 #1 Code

	{ "11",      	  0x1000, 0xf92c4c5b, 3 | BRF_GRA },	       //  9 Graphics
	{ "10",      	  0x1000, 0x3b0783ce, 3 | BRF_GRA },	       // 10
};

STD_ROM_PICK(sucasino);
STD_ROM_FN(sucasino);

struct BurnDriver BurnDrvsucasino = {
	"sucasino", NULL, NULL, "1984",
	"Super Casino\0", NULL, "Data Amusement", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, sucasinoRomInfo, sucasinoRomName, SucasinoInputInfo, SucasinoDIPInfo,
	jackInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Tri-Pool (Casino Tech)

static struct BurnRomInfo tripoolRomDesc[] = {
	{ "tri73a.bin",   0x1000, 0x96893aa7, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "tri62a.bin",   0x1000, 0x3299dc65, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "tri52b.bin",   0x1000, 0x27ef765e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "tri33c.bin",   0x1000, 0xd7ef061d, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "tri45c.bin",   0x1000, 0x51b813b1, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "tri25d.bin",   0x1000, 0x8e64512d, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "tri13d.bin",   0x1000, 0xad268e9b, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "trisnd.bin",   0x1000, 0x945c4b8b, 2 | BRF_PRG | BRF_ESS }, //  7 Z80 #1 Code

	{ "tri105a.bin",  0x1000, 0x366a753c, 3 | BRF_GRA },	       //  8 Graphics
	{ "tri93a.bin",   0x1000, 0x35213782, 3 | BRF_GRA },	       //  9
};

STD_ROM_PICK(tripool);
STD_ROM_FN(tripool);

static int tripoolInit()
{
	tri_fix = 1;
	timer_rate = 128;

	return DrvInit();
}

struct BurnDriver BurnDrvtripool = {
	"tripool", NULL, NULL, "1981",
	"Tri-Pool (Casino Tech)\0", NULL, "Noma (Casino Tech license)", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, tripoolRomInfo, tripoolRomName, TripoolInputInfo, NULL,
	tripoolInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Tri-Pool (Costal Games)

static struct BurnRomInfo tripoolaRomDesc[] = {
	{ "tri73a.bin",   0x1000, 0x96893aa7, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "tri62a.bin",   0x1000, 0x3299dc65, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "tri52b.bin",   0x1000, 0x27ef765e, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "tri33c.bin",   0x1000, 0xd7ef061d, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "tri45c.bin",   0x1000, 0x51b813b1, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "tri25d.bin",   0x1000, 0x8e64512d, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "tp1ckt",       0x1000, 0x72ec43a3, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "trisnd.bin",   0x1000, 0x945c4b8b, 2 | BRF_PRG | BRF_ESS }, //  7 Z80 #1 Code

	{ "tri105a.bin",  0x1000, 0x366a753c, 3 | BRF_GRA },	       //  8 Graphics
	{ "tri93a.bin",   0x1000, 0x35213782, 3 | BRF_GRA },	       //  9
};

STD_ROM_PICK(tripoola);
STD_ROM_FN(tripoola);

struct BurnDriver BurnDrvtripoola = {
	"tripoola", "tripool", NULL, "1981",
	"Tri-Pool (Costal Games)\0", NULL, "Noma (Costal Games license)", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
	NULL, tripoolaRomInfo, tripoolaRomName, TripoolInputInfo, NULL,
	tripoolInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


// Joinem

static struct BurnRomInfo joinemRomDesc[] = {
	{ "join1.r0",    0x2000, 0xb5b2e2cc, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "join2.r2",    0x2000, 0xbcf140e6, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "join3.r4",    0x2000, 0xfe04e4d4, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "join7.s0",    0x1000, 0xbb8a7814, 2 | BRF_PRG | BRF_ESS }, //  3 Z80 #1 Code

	{ "join4.p3",    0x1000, 0x4964c82c, 3 | BRF_GRA },	      //  4 Graphics
	{ "join5.p2",    0x1000, 0xae78fa89, 3 | BRF_GRA },	      //  5
	{ "join6.p1",    0x1000, 0x2b533261, 3 | BRF_GRA },	      //  6

	{ "l82s129.11n", 0x0100, 0x7b724211, 4 | BRF_GRA },	      //  7 Color Proms
	{ "h82s129.12n", 0x0100, 0x2e81c5ff, 4 | BRF_GRA },	      //  8
};

STD_ROM_PICK(joinem);
STD_ROM_FN(joinem);

static void joinem_palette_init()
{
	for (int i = 0; i < 0x100; i++)
	{
		int bit0,bit1,bit2,r,g,b;

		bit0 = (Prom[i] >> 0) & 1;
		bit1 = (Prom[i] >> 1) & 1;
		bit2 = (Prom[i] >> 2) & 1;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		bit0 = (Prom[i] >> 3) & 1;
		bit1 = (Prom[i] >> 4) & 1;
		bit2 = (Prom[i] >> 5) & 1;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		bit0 = 0;
		bit1 = (Prom[i] >> 6) & 1;
		bit2 = (Prom[i] >> 7) & 1;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		Palette[i] = (r << 16) | (g << 8) | b;
	}
}

static int joinemInit()
{
	joinem = 1;
	timer_rate = 1;

	int nRet = DrvInit();

	joinem_palette_init();

	return nRet;
}

struct BurnDriver BurnDrvjoinem = {
	"joinem", NULL, NULL, "1986",
	"Joinem\0", NULL, "Global Corporation", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, joinemRomInfo, joinemRomName, JoinemInputInfo, JoinemDIPInfo,
	joinemInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 240, 3, 4
};


// Lover Boy

static struct BurnRomInfo loverboyRomDesc[] = {
	{ "lover.r0",  0x2000, 0xffec4e41, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "lover.r2",  0x2000, 0x04052262, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "lover.r4",  0x2000, 0xce5f3b49, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "lover.r6",  0x1000, 0x839d79b7, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "lover.s0",  0x1000, 0xec38111c, 2 | BRF_PRG | BRF_ESS }, //  4 Z80 #1 Code

	{ "lover.p3",  0x2000, 0x1a519c8f, 3 | BRF_GRA },	    //  5 Graphics
	{ "lover.p2",  0x2000, 0xe465372f, 3 | BRF_GRA },	    //  6
	{ "lover.p1",  0x2000, 0xcda0d87e, 3 | BRF_GRA },	    //  7

	{ "color.n11", 0x0200, 0xcf4a16ae, 4 | BRF_GRA },	    //  8 Color Proms
	{ "color.n12", 0x0200, 0x4b11ac21, 4 | BRF_GRA },	    //  9
};

STD_ROM_PICK(loverboy);
STD_ROM_FN(loverboy);

static int loverboyInit()
{
	loverb = 1;
	timer_rate = 16;

	int nRet = DrvInit();

	// Hack (Protection?)
	Rom0[0x12] = 0x9d;
	Rom0[0x13] = 0x01;

	joinem_palette_init();

	return nRet;
}

struct BurnDriver BurnDrvloverboy = {
	"loverboy", NULL, NULL, "1983",
	"Lover Boy\0", NULL, "G.T Enterprise Inc", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, loverboyRomInfo, loverboyRomName, LoverboyInputInfo, LoverboyDIPInfo,
	loverboyInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 240, 3, 4
};


// Super Triv

static struct BurnRomInfo strivRomDesc[] = {
	{ "pr1.f2",       0x1000, 0xdcf5da6e, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "pr2.4f",       0x1000, 0x921610ba, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "pr3.5f",       0x1000, 0xc36f0e21, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "pr4.6f",       0x1000, 0x0dc98a97, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "bc3.7e",       0x1000, 0x83f03885, 1 | BRF_PRG | BRF_ESS }, //  4
	{ "bc2.6e",       0x1000, 0x75f18361, 1 | BRF_PRG | BRF_ESS }, //  5
	{ "bc1.5e",       0x1000, 0x0d150385, 1 | BRF_PRG | BRF_ESS }, //  6

	{ "snd.5a",       0x1000, 0xb7ddf84f, 2 | BRF_PRG | BRF_ESS }, //  7 Z80 #1 Code

	{ "chr0.1a",      0x1000, 0x8f60229b, 3 | BRF_GRA },	       //  8 Graphics
	{ "chr2.4a",      0x1000, 0x8f982a9c, 3 | BRF_GRA },	       //  9
	{ "chr3.5a",      0x1000, 0x8f982a9c, 3 | BRF_GRA },	       // 10
	{ "chr1.2a",      0x1000, 0x7ad4358e, 3 | BRF_GRA },	       // 11

	{ "rom.u6",       0x8000, 0xa32d7a28, 5 | BRF_PRG | BRF_ESS }, // 12 Question ROMs
	{ "rom.u7",       0x8000, 0xbc44ae18, 5 | BRF_PRG | BRF_ESS }, // 13
	{ "tbfd2.u8",     0x8000, 0x9572984a, 5 | BRF_PRG | BRF_ESS }, // 14
	{ "tbfd3.u9",     0x8000, 0xd904a2f1, 5 | BRF_PRG | BRF_ESS }, // 15
	{ "tbfl0.u10",    0x8000, 0x680264a2, 5 | BRF_PRG | BRF_ESS }, // 16
	{ "tbfl1.u11",    0x8000, 0x33e99d00, 5 | BRF_PRG | BRF_ESS }, // 17
	{ "tbfl2.u12",    0x8000, 0x2e7a941f, 5 | BRF_PRG | BRF_ESS }, // 18
	{ "tbft0.u13",    0x8000, 0x7d2e5e89, 5 | BRF_PRG | BRF_ESS }, // 19
	{ "tbft1.u14",    0x8000, 0xd36246cf, 5 | BRF_PRG | BRF_ESS }, // 20
	{ "tbfd1.u15",    0x8000, 0x745db398, 5 | BRF_PRG | BRF_ESS }, // 21

	{ "tbfd0.u21",    0x2000, 0x15b83099, 0 | BRF_OPT },	       // 22 Junk
};

STD_ROM_PICK(striv);
STD_ROM_FN(striv);

static int strivInit()
{
	suprtriv = 1;
	timer_rate = 128;

	int nRet = DrvInit();

	for (int i = 0; i < 0x4000; i++)
	{
		if (i & 0x1000)
		{
			if (i & 4)
				Rom0[i] = BITSWAP08(Rom0[i],7,2,5,1,3,6,4,0) ^ 1;
			else
				Rom0[i] = BITSWAP08(Rom0[i],0,2,5,1,3,6,4,7) ^ 0x81;
		}
		else
		{
			if (i & 4)
				Rom0[i] = BITSWAP08(Rom0[i],7,2,5,1,3,6,4,0) ^ 1;
			else
				Rom0[i] = BITSWAP08(Rom0[i],0,2,5,1,3,6,4,7);
		}
	}

	return nRet;
}

struct BurnDriver BurnDrvstriv = {
	"striv", NULL, NULL, "1985",
	"Super Triv\0", "No sound", "Hara Industries", "Jack the Giantkiller",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, strivRomInfo, strivRomName, StrivInputInfo, StrivDIPInfo,
	strivInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, &DrvCalcPal,
	224, 256, 3, 4
};


