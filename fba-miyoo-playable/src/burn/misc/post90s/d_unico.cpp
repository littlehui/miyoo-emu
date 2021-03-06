#include "tiles_generic.h"
#include "burn_ym3812.h"
#include "burn_ym2151.h"
#include "msm6295.h"
#include "eeprom_93cxx.h"
//#include "burn_gun.h"

static unsigned char DrvInputPort0[8]     = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort1[8]     = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort2[8]     = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvDip[2]            = {0, 0};
static unsigned char DrvInput[3]          = {0x00, 0x00, 0x00};
static unsigned char DrvReset             = 0;
static int           DrvAnalogPort0       = 0;
static int           DrvAnalogPort1       = 0;
static int           DrvAnalogPort2       = 0;
static int           DrvAnalogPort3       = 0;

static unsigned char *Mem                 = NULL;
static unsigned char *MemEnd              = NULL;
static unsigned char *RamStart            = NULL;
static unsigned char *RamEnd              = NULL;
static unsigned char *Drv68KRom           = NULL;
static unsigned char *Drv68KRam           = NULL;
static unsigned char *DrvMSM6295ROMSrc    = NULL;
static unsigned char *DrvVideo0Ram        = NULL;
static unsigned char *DrvVideo1Ram        = NULL;
static unsigned char *DrvVideo2Ram        = NULL;
static unsigned char *DrvSpriteRam        = NULL;
static unsigned char *DrvPaletteRam       = NULL;
static unsigned char *DrvScrollRam        = NULL;
static unsigned char *DrvTiles            = NULL;
static unsigned char *DrvSprites          = NULL;
static unsigned char *DrvTempRom          = NULL;
static unsigned int  *DrvPalette          = NULL;

static int nCyclesDone[1], nCyclesTotal[1];
static int nCyclesSegment;

static UINT16 DrvScrollX0;
static UINT16 DrvScrollY0;
static UINT16 DrvScrollX1;
static UINT16 DrvScrollY1;
static UINT16 DrvScrollX2;
static UINT16 DrvScrollY2;
static unsigned char DrvOkiBank;

static unsigned int DrvNumTiles;
static unsigned int DrvNumSprites;

typedef void (*UnicoMakeInputs)();
static UnicoMakeInputs UnicoMakeInputsFunction;

static struct BurnInputInfo BurglarxInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort2 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , DrvInputPort0 + 3, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , DrvInputPort0 + 2, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , DrvInputPort0 + 1, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , DrvInputPort0 + 0, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , DrvInputPort0 + 4, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , DrvInputPort0 + 5, "p1 fire 2" },
	{"P1 Fire 3"         , BIT_DIGITAL  , DrvInputPort0 + 6, "p1 fire 3" },
	
	{"P2 Up"             , BIT_DIGITAL  , DrvInputPort1 + 3, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , DrvInputPort1 + 2, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , DrvInputPort1 + 1, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , DrvInputPort1 + 0, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , DrvInputPort1 + 4, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , DrvInputPort1 + 5, "p2 fire 2" },
	{"P2 Fire 3"         , BIT_DIGITAL  , DrvInputPort1 + 6, "p2 fire 3" },
	
	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort2 + 2, "service"   },
	{"Service 2"         , BIT_DIGITAL  , DrvInputPort2 + 3, "service 2" },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
};

STDINPUTINFO(Burglarx);

#define A(a, b, c, d) {a, b, (unsigned char*)(c), d}

static struct BurnInputInfo ZeropntInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort0 + 5, "p2 start"  },

	A("P1 Gun X"         , BIT_ANALOG_REL, &DrvAnalogPort0 , "mouse x-axis"),
	A("P1 Gun Y"         , BIT_ANALOG_REL, &DrvAnalogPort1 , "mouse y-axis"),
	{"P1 Fire 1"         , BIT_DIGITAL  , DrvInputPort1 + 0, "mouse button 1" },
	
	A("P2 Gun X"         , BIT_ANALOG_REL, &DrvAnalogPort2 , "p2 x-axis"),
	A("P2 Gun Y"         , BIT_ANALOG_REL, &DrvAnalogPort3 , "p2 y-axis"),
	{"P2 Fire 1"         , BIT_DIGITAL  , DrvInputPort1 + 1, "p2 fire 1" },
	
	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Diagnostic"        , BIT_DIGITAL  , DrvInputPort0 + 2, "diag"      },
	{"Service"           , BIT_DIGITAL  , DrvInputPort0 + 7, "service"   },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
};

STDINPUTINFO(Zeropnt);

#undef A

static inline void BurglarxClearOpposites(unsigned char* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x03) {
		*nJoystickInputs &= ~0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x0c) {
		*nJoystickInputs &= ~0x0c;
	}
}

static inline void BurglarxMakeInputs()
{
	// Reset Inputs
	DrvInput[0] = DrvInput[1] = DrvInput[2] = 0x00;

	// Compile Digital Inputs
	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvInputPort0[i] & 1) << i;
		DrvInput[1] |= (DrvInputPort1[i] & 1) << i;
		DrvInput[2] |= (DrvInputPort2[i] & 1) << i;
	}

	// Clear Opposites
	BurglarxClearOpposites(&DrvInput[0]);
	BurglarxClearOpposites(&DrvInput[1]);
}

static inline void ZeropntMakeInputs()
{
	// Reset Inputs
	DrvInput[0] = 0x00;
	DrvInput[1] = 0xff;

	// Compile Digital Inputs
	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvInputPort0[i] & 1) << i;
		DrvInput[1] -= (DrvInputPort1[i] & 1) << i;
	}
	
	//BurnGunMakeInputs(0, (short)DrvAnalogPort0, (short)DrvAnalogPort1);
	//BurnGunMakeInputs(1, (short)DrvAnalogPort2, (short)DrvAnalogPort3);
}

static inline void Zeropnt2MakeInputs()
{
	// Reset Inputs
	DrvInput[0] = 0x00;
	DrvInput[1] = 0x7f;

	// Compile Digital Inputs
	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvInputPort0[i] & 1) << i;
		DrvInput[1] -= (DrvInputPort1[i] & 1) << i;
	}
	
	//BurnGunMakeInputs(0, (short)DrvAnalogPort0, (short)DrvAnalogPort1);
	//BurnGunMakeInputs(1, (short)DrvAnalogPort2, (short)DrvAnalogPort3);
}

static struct BurnDIPInfo BurglarxDIPList[]=
{
	// Default Values
	{0x15, 0xff, 0xff, 0xf7, NULL                     },
	{0x16, 0xff, 0xff, 0xff, NULL                     },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Test Mode"              },
	{0x15, 0x01, 0x01, 0x01, "Off"                    },
	{0x15, 0x01, 0x01, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x15, 0x01, 0x02, 0x02, "Off"                    },
	{0x15, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x15, 0x01, 0x08, 0x08, "Off"                    },
	{0x15, 0x01, 0x08, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 8   , "Coinage"                },
	{0x15, 0x01, 0xe0, 0x00, "5 Coins 1 Credit"       },
	{0x15, 0x01, 0xe0, 0x20, "4 Coins 1 Credit"       },
	{0x15, 0x01, 0xe0, 0x40, "3 Coins 1 Credit"       },
	{0x15, 0x01, 0xe0, 0x60, "2 Coins 1 Credit"       },
	{0x15, 0x01, 0xe0, 0xe0, "1 Coin  1 Credit"       },
	{0x15, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"      },
	{0x15, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"      },
	{0x15, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"      },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x16, 0x01, 0x03, 0x02, "None"                   },
	{0x16, 0x01, 0x03, 0x03, "A"                      },
	{0x16, 0x01, 0x03, 0x01, "B"                      },
	{0x16, 0x01, 0x03, 0x00, "C"                      },
	
	{0   , 0xfe, 0   , 2   , "Starting Energy"        },
	{0x16, 0x01, 0x80, 0x00, "2"                      },
	{0x16, 0x01, 0x80, 0x80, "3"                      },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x16, 0x01, 0x30, 0x20, "Easy"                   },
	{0x16, 0x01, 0x30, 0x30, "Normal"                 },
	{0x16, 0x01, 0x30, 0x10, "Hard"                   },
	{0x16, 0x01, 0x30, 0x00, "Hardest"                },
		
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x16, 0x01, 0xc0, 0x80, "2"                      },
	{0x16, 0x01, 0xc0, 0xc0, "3"                      },
	{0x16, 0x01, 0xc0, 0x40, "4"                      },
	{0x16, 0x01, 0xc0, 0x00, "5"                      },
};

STDDIPINFO(Burglarx);

static struct BurnDIPInfo ZeropntDIPList[]=
{
	// Default Values
	{0x0d, 0xff, 0xff, 0x08, NULL                     },
	{0x0e, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x0d, 0x01, 0x02, 0x00, "Off"                    },
	{0x0d, 0x01, 0x02, 0x02, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0d, 0x01, 0x08, 0x00, "Off"                    },
	{0x0d, 0x01, 0x08, 0x08, "On"                     },
	
	{0   , 0xfe, 0   , 8   , "Coinage"                },
	{0x0d, 0x01, 0xe0, 0xe0, "5 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0xc0, "4 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0xa0, "3 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0x80, "2 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0x00, "1 Coin  1 Credit"       },
	{0x0d, 0x01, 0xe0, 0x20, "1 Coin  2 Credits"      },
	{0x0d, 0x01, 0xe0, 0x40, "1 Coin  3 Credits"      },
	{0x0d, 0x01, 0xe0, 0x60, "1 Coin  4 Credits"      },
	
	// Dip 2
	{0   , 0xfe, 0   , 3   , "Difficulty"             },
	{0x0e, 0x01, 0x30, 0x10, "Easy"                   },
	{0x0e, 0x01, 0x30, 0x00, "Normal"                 },
	{0x0e, 0x01, 0x30, 0x20, "Hard"                   },
		
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x0e, 0x01, 0xc0, 0x40, "2"                      },
	{0x0e, 0x01, 0xc0, 0x00, "3"                      },
	{0x0e, 0x01, 0xc0, 0x80, "4"                      },
	{0x0e, 0x01, 0xc0, 0xc0, "5"                      },
};

STDDIPINFO(Zeropnt);

static struct BurnDIPInfo Zeropnt2DIPList[]=
{
	// Default Values
	{0x0d, 0xff, 0xff, 0xff, NULL                     },
	{0x0e, 0xff, 0xff, 0xfd, NULL                     },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x0d, 0x01, 0x01, 0x01, "Off"                    },
	{0x0d, 0x01, 0x01, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Coins to Continue"      },
	{0x0d, 0x01, 0x02, 0x00, "1"                      },
	{0x0d, 0x01, 0x02, 0x02, "2"                      },
	
	{0   , 0xfe, 0   , 3   , "Gun Reloading"          },
	{0x0d, 0x01, 0x0c, 0x08, "No"                     },
	{0x0d, 0x01, 0x0c, 0x04, "Yes"                    },
	{0x0d, 0x01, 0x0c, 0x0c, "Factory Setting"        },
	
	{0   , 0xfe, 0   , 2   , "Language"               },
	{0x0d, 0x01, 0x10, 0x10, "English"                },
	{0x0d, 0x01, 0x10, 0x00, "Japanese"               },
	
	{0   , 0xfe, 0   , 8   , "Coinage"                },
	{0x0d, 0x01, 0xe0, 0x00, "5 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0x20, "4 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0x40, "3 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0x60, "2 Coins 1 Credit"       },
	{0x0d, 0x01, 0xe0, 0xe0, "1 Coin  1 Credit"       },
	{0x0d, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"      },
	{0x0d, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"      },
	{0x0d, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"      },
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Korean Language"        },
	{0x0e, 0x01, 0x01, 0x01, "Off"                    },
	{0x0e, 0x01, 0x01, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0e, 0x01, 0x02, 0x02, "Off"                    },
	{0x0e, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 5   , "Lives"                  },
	{0x0e, 0x01, 0x1c, 0x10, "2"                      },
	{0x0e, 0x01, 0x1c, 0x0c, "3"                      },
	{0x0e, 0x01, 0x1c, 0x1c, "4"                      },
	{0x0e, 0x01, 0x1c, 0x18, "5"                      },
	{0x0e, 0x01, 0x1c, 0x14, "6"                      },

	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x0e, 0x01, 0xc0, 0x80, "Easy"                   },
	{0x0e, 0x01, 0xc0, 0xc0, "Normal"                 },
	{0x0e, 0x01, 0xc0, 0x40, "Hard"                   },
	{0x0e, 0x01, 0xc0, 0x00, "Hardest"                },	
};

STDDIPINFO(Zeropnt2);

static struct BurnRomInfo BurglarxRomDesc[] = {
	{ "bx-rom2.pgm",   0x80000, 0xf81120c8, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "bx-rom3.pgm",   0x80000, 0x080b4e82, BRF_ESS | BRF_PRG }, //	 1
	
	{ "bx-rom4",       0x80000, 0xf74ce31f, BRF_GRA },	     //  2	Sprites
	{ "bx-rom10",      0x80000, 0x6f56ca23, BRF_GRA },	     //  3
	{ "bx-rom9",       0x80000, 0x33f29d79, BRF_GRA },	     //  4
	{ "bx-rom8",       0x80000, 0x24367092, BRF_GRA },	     //  5
	{ "bx-rom7",       0x80000, 0xaff6bdea, BRF_GRA },	     //  6
	{ "bx-rom6",       0x80000, 0x246afed2, BRF_GRA },	     //  7
	{ "bx-rom11",      0x80000, 0x898d176a, BRF_GRA },	     //  8
	{ "bx-rom5",       0x80000, 0xfdee1423, BRF_GRA },	     //  9
	
	{ "bx-rom14",      0x80000, 0x30413373, BRF_GRA },	     //  10	Layers
	{ "bx-rom18",      0x80000, 0x8e7fc99f, BRF_GRA },	     //  11
	{ "bx-rom19",      0x80000, 0xd40eabcd, BRF_GRA },	     //  12
	{ "bx-rom15",      0x80000, 0x78833c75, BRF_GRA },	     //  13
	{ "bx-rom17",      0x80000, 0xf169633f, BRF_GRA },	     //  14
	{ "bx-rom12",      0x80000, 0x71eb160f, BRF_GRA },	     //  15
	{ "bx-rom13",      0x80000, 0xda34bbb5, BRF_GRA },	     //  16
	{ "bx-rom16",      0x80000, 0x55b28ef9, BRF_GRA },	     //  17
	
	{ "bx-rom1.snd",   0x80000, 0x8ae67138, BRF_SND },	     //  18	Samples
};

STD_ROM_PICK(Burglarx);
STD_ROM_FN(Burglarx);

static struct BurnRomInfo ZeropntRomDesc[] = {
	{ "zero_2.bin",    0x080000, 0x1e599509, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "zero_3.bin",    0x080000, 0x588aeef7, BRF_ESS | BRF_PRG }, //  1
	
	{ "zpobjz01.bin",  0x200000, 0x1f2768a3, BRF_GRA },	     //  2	Sprites
	{ "zpobjz02.bin",  0x200000, 0xde34f33a, BRF_GRA },	     //  3
	{ "zpobjz03.bin",  0x200000, 0xd7a657f7, BRF_GRA },	     //  4
	{ "zpobjz04.bin",  0x200000, 0x3aec2f8d, BRF_GRA },	     //  5
	
	{ "zpscrz06.bin",  0x200000, 0xe1e53cf0, BRF_GRA },	     //  6	Layers
	{ "zpscrz05.bin",  0x200000, 0x0d7d4850, BRF_GRA },	     //  7
	{ "zpscrz07.bin",  0x200000, 0xbb178f32, BRF_GRA },	     //  8
	{ "zpscrz08.bin",  0x200000, 0x672f02e5, BRF_GRA },	     //  9
	
	{ "zero_1.bin",    0x080000, 0xfd2384fa, BRF_SND },	     //  10	Samples
};

STD_ROM_PICK(Zeropnt);
STD_ROM_FN(Zeropnt);

static struct BurnRomInfo ZeropntaRomDesc[] = {
	{ "zpa2.bin",      0x080000, 0x285fbca3, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "zpa3.bin",      0x080000, 0xad7b3129, BRF_ESS | BRF_PRG }, //  1
	
	{ "zpobjz01.bin",  0x200000, 0x1f2768a3, BRF_GRA },	     //  2	Sprites
	{ "zpobjz02.bin",  0x200000, 0xde34f33a, BRF_GRA },	     //  3
	{ "zpobjz03.bin",  0x200000, 0xd7a657f7, BRF_GRA },	     //  4
	{ "zpobjz04.bin",  0x200000, 0x3aec2f8d, BRF_GRA },	     //  5
	
	{ "zpscrz06.bin",  0x200000, 0xe1e53cf0, BRF_GRA },	     //  6	Layers
	{ "zpscrz05.bin",  0x200000, 0x0d7d4850, BRF_GRA },	     //  7
	{ "zpscrz07.bin",  0x200000, 0xbb178f32, BRF_GRA },	     //  8
	{ "zpscrz08.bin",  0x200000, 0x672f02e5, BRF_GRA },	     //  9
	
	{ "zero_1.bin",    0x080000, 0xfd2384fa, BRF_SND },	     //  10	Samples
};

STD_ROM_PICK(Zeropnta);
STD_ROM_FN(Zeropnta);

static struct BurnRomInfo Zeropnt2RomDesc[] = {
	{ "d16-d31.4",     0x100000, 0x48314fdb, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "d0-d15.3",      0x100000, 0x5ec4151e, BRF_ESS | BRF_PRG }, //  1
	
	{ "db0db1zp.209",  0x400000, 0x474b460c, BRF_GRA },	     //  2	Sprites
	{ "db2db3zp.210",  0x400000, 0x0a1d0a88, BRF_GRA },	     //  3
	{ "db4db5zp.211",  0x400000, 0x227169dc, BRF_GRA },	     //  4
	{ "db6db7zp.212",  0x400000, 0xa6306cdb, BRF_GRA },	     //  5
	
	{ "a0-a1zp.205",   0x400000, 0xf7ca9c0e, BRF_GRA },	     //  6	Layers
	{ "a2-a3zp.206",   0x400000, 0x0581c8fe, BRF_GRA },	     //  7
	{ "a4-a5zp.208",   0x400000, 0xddd091ef, BRF_GRA },	     //  8
	{ "a6-a7zp.207",   0x400000, 0x3fd46113, BRF_GRA },	     //  9
	
	{ "uzp2-1.bin",    0x080000, 0xed0966ed, BRF_SND },	     //  10	Samples (Oki 1)
	
	{ "uzp2-2.bin",    0x040000, 0xdb8cb455, BRF_SND },	     //  11	Samples (Oki 2)
};

STD_ROM_PICK(Zeropnt2);
STD_ROM_FN(Zeropnt2);

static int MemIndex()
{
	unsigned char *Next; Next = Mem;

	Drv68KRom              = Next; Next += 0x100000;
	MSM6295ROM             = Next; Next += 0x040000;
	DrvMSM6295ROMSrc       = Next; Next += 0x080000;

	RamStart               = Next;

	Drv68KRam              = Next; Next += 0x014000;
	DrvVideo0Ram           = Next; Next += 0x004000;
	DrvVideo1Ram           = Next; Next += 0x004000;
	DrvVideo2Ram           = Next; Next += 0x004000;
	DrvSpriteRam           = Next; Next += 0x000800;
	DrvPaletteRam          = Next; Next += 0x008000;

	RamEnd                 = Next;

	DrvTiles               = Next; Next += DrvNumTiles * 16 * 16;
	DrvSprites             = Next; Next += DrvNumSprites * 16 * 16;
	DrvPalette             = (unsigned int*)Next; Next += 0x02000 * sizeof(unsigned int);

	MemEnd                 = Next;

	return 0;
}

static int Zeropnt2MemIndex()
{
	unsigned char *Next; Next = Mem;

	Drv68KRom              = Next; Next += 0x200000;
	MSM6295ROM             = Next; Next += 0x140000;
	DrvMSM6295ROMSrc       = Next; Next += 0x080000;

	RamStart               = Next;

	Drv68KRam              = Next; Next += 0x024000;
	DrvVideo0Ram           = Next; Next += 0x004000;
	DrvVideo1Ram           = Next; Next += 0x004000;
	DrvVideo2Ram           = Next; Next += 0x004000;
	DrvSpriteRam           = Next; Next += 0x000800;
	DrvPaletteRam          = Next; Next += 0x008000;
	DrvScrollRam           = Next; Next += 0x000018;

	RamEnd                 = Next;

	DrvTiles               = Next; Next += DrvNumTiles * 16 * 16;
	DrvSprites             = Next; Next += DrvNumSprites * 16 * 16;
	DrvPalette             = (unsigned int*)Next; Next += 0x02000 * sizeof(unsigned int);

	MemEnd                 = Next;

	return 0;
}

static int DrvDoReset()
{
	SekOpen(0);
	SekReset();
	SekClose();
	
	BurnYM3812Reset();
	MSM6295Reset(0);
	
	DrvScrollX0 = 0;
	DrvScrollY0 = 0;
	DrvScrollX1 = 0;
	DrvScrollY1 = 0;
	DrvScrollX2 = 0;
	DrvScrollY2 = 0;
	DrvOkiBank = 0;	
	
	return 0;
}

static int Zeropnt2DoReset()
{
	SekOpen(0);
	SekReset();
	SekClose();
	
	EEPROMReset();
	
	BurnYM2151Reset();
	MSM6295Reset(0);
	MSM6295Reset(1);
	
	DrvOkiBank = 0;
	
	return 0;
}

unsigned char __fastcall Burglarx68KReadByte(unsigned int a)
{
	switch (a) {
		case 0x800000: {
			return 0xff - DrvInput[1];
		}
		
		case 0x800001: {
			return 0xff - DrvInput[0];
		}
		
		case 0x800019: {
			return 0xff - DrvInput[2];
		}
		
		case 0x80001a: {
			return DrvDip[0];
		}
		
		case 0x80001c: {
			return DrvDip[1];
		}
		
		case 0x800189: {
			return MSM6295ReadStatus(0);
		}
		
		case 0x80018c: {
			return BurnYM3812Read(0);
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read byte => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Burglarx68KWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x800189: {
			MSM6295Command(0, d);
			return;
		}
		
		case 0x80018a: {
			BurnYM3812Write(1, d);
			return;
		}
		
		case 0x80018c: {
			BurnYM3812Write(0, d);
			return;
		}
		
		case 0x80018e: {
			DrvOkiBank = d & 1;
			memcpy(MSM6295ROM + 0x00000, DrvMSM6295ROMSrc + (0x40000 * DrvOkiBank), 0x40000);
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write byte => %06X, %02X\n"), a, d);
		}
	}
}

unsigned short __fastcall Burglarx68KReadWord(unsigned int a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read word => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Burglarx68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x800030: {
			// NOP???
			return;
		}
		
		case 0x80010c: {
			DrvScrollX0 = d & 0x3ff;
			return;
		}
		
		case 0x80010e: {
			DrvScrollY0 = d & 0x3ff;
			return;
		}
		
		case 0x800110: {
			DrvScrollY2 = d & 0x3ff;
			return;
		}
	
		case 0x800114: {
			DrvScrollX2 = d & 0x3ff;
			return;
		}
		
		case 0x800116: {
			DrvScrollX1 = d & 0x3ff;
			return;
		}
		
		case 0x800120: {
			DrvScrollY1 = d & 0x3ff;
			return;
		}
		
		case 0x8001e0: {
			// IRQ Ack???
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned char __fastcall Zeropnt68KReadByte(unsigned int a)
{
	switch (a) {
		case 0x800018: {
			return DrvInput[1];
		}
		
		case 0x800019: {
			return DrvInput[0];
		}
		
		case 0x80001a: {
			return DrvDip[0];
		}
		
		case 0x80001c: {
			return DrvDip[1];
		}
		
/*		case 0x800170: {
			int y = BurnGunReturnY(1);
			
			y = 0x18 + ((y * 0xe0) / 0xff);
			
			return ((y & 0xff) ^ (GetCurrentFrame() & 1));
		}
		
		case 0x800174: {
			int x = BurnGunReturnX(1);
			
			x = x * 384 / 256;
			if (x < 0x160) {
				x = 0x30 + (x * 0xd0 / 0x15f);
			} else {
				x = ((x - 0x160) * 0x20) / 0x1f;
			}
			
			return ((x & 0xff) ^ (GetCurrentFrame() & 1));
		}
		
		case 0x800178: {
			int y = BurnGunReturnY(0);
			
			y = 0x18 + ((y * 0xe0) / 0xff);
			
			return ((y & 0xff) ^ (GetCurrentFrame() & 1));
		}
		
		case 0x80017c: {
			int x = BurnGunReturnX(0);
			
			x = x * 384 / 256;
			if (x < 0x160) {
				x = 0x30 + (x * 0xd0 / 0x15f);
			} else {
				x = ((x - 0x160) * 0x20) / 0x1f;
			}
			
			return ((x & 0xff) ^ (GetCurrentFrame() & 1));
		
		}
*/		
		case 0x800189: {
			return MSM6295ReadStatus(0);
		}
		
		case 0x80018c: {
			return BurnYM3812Read(0);
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read byte => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Zeropnt68KWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x800189: {
			MSM6295Command(0, d);
			return;
		}
		
		case 0x80018a: {
			BurnYM3812Write(1, d);
			return;
		}
		
		case 0x80018c: {
			BurnYM3812Write(0, d);
			return;
		}
		
		case 0x80018e: {
			DrvOkiBank = d & 1;
			memcpy(MSM6295ROM + 0x20000, DrvMSM6295ROMSrc + 0x20000 + (0x20000 * DrvOkiBank), 0x20000);
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write byte => %06X, %02X\n"), a, d);
		}
	}
}

unsigned short __fastcall Zeropnt68KReadWord(unsigned int a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read word => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Zeropnt68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x800030: {
			// NOP???
			return;
		}
		
		case 0x80010c: {
			DrvScrollX0 = d & 0x3ff;
			return;
		}
		
		case 0x80010e: {
			DrvScrollY0 = d & 0x3ff;
			return;
		}
		
		case 0x800110: {
			DrvScrollY2 = d & 0x3ff;
			return;
		}
	
		case 0x800114: {
			DrvScrollX2 = d & 0x3ff;
			return;
		}
		
		case 0x800116: {
			DrvScrollX1 = d & 0x3ff;
			return;
		}
		
		case 0x800120: {
			DrvScrollY1 = d & 0x3ff;
			return;
		}
		
		case 0x8001e0: {
			// IRQ Ack???
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned char __fastcall Zeropnt268KReadByte(unsigned int a)
{
	switch (a) {
		case 0x800019: {
			return DrvInput[0];
		}
		
		case 0x800025: {
			return MSM6295ReadStatus(0);
		}
		
		case 0x80002d: {
			return BurnYM2151ReadStatus();
		}
		
		case 0x800031: {
			return MSM6295ReadStatus(1);
		}
		
/*		case 0x800140: {
			int y = BurnGunReturnY(1);
			
			y = 0x18 + ((y * 0xe0) / 0xff);
			
			return ((y & 0xff) ^ (GetCurrentFrame() & 1)) + 0x08;
		}
		
		case 0x800144: {
			int x = BurnGunReturnX(1);
			
			x = x * 384 / 256;
			if (x < 0x160) {
				x = 0x30 + (x * 0xd0 / 0x15f);
			} else {
				x = ((x - 0x160) * 0x20) / 0x1f;
			}
			
			return ((x & 0xff) ^ (GetCurrentFrame() & 1)) - 0x08;
		}
		
		case 0x800148: {
			int y = BurnGunReturnY(0);
			
			y = 0x18 + ((y * 0xe0) / 0xff);
			
			return ((y & 0xff) ^ (GetCurrentFrame() & 1)) + 0x08;
		}
		
		case 0x80014c: {
			int x = BurnGunReturnX(0);
			
			x = x * 384 / 256;
			if (x < 0x160) {
				x = 0x30 + (x * 0xd0 / 0x15f);
			} else {
				x = ((x - 0x160) * 0x20) / 0x1f;
			}
			
			return ((x & 0xff) ^ (GetCurrentFrame() & 1)) - 0x08;
		}
*/		
		case 0x800150: {
			return DrvDip[0];
		}
		
		case 0x800154: {
			return DrvDip[1];
		}
		
		case 0x80015c: {
			return DrvInput[1] | ((EEPROMRead() & 1) << 8);
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read byte => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Zeropnt268KWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x800025: {
			MSM6295Command(0, d);
			return;
		}
		
		case 0x800029: {
			BurnYM2151SelectRegister(d);
			return;
		}
		
		case 0x80002d: {
			BurnYM2151WriteRegister(d);
			return;
		}
		
		case 0x800031: {
			MSM6295Command(1, d);
			return;
		}
		
		case 0x800034: {
			DrvOkiBank = (d & 3) % 4;
			memcpy(MSM6295ROM + 0x20000, DrvMSM6295ROMSrc + 0x20000 + (0x20000 * DrvOkiBank), 0x20000);
			return;
		}
		
		case 0x800039: {
			// LEDs
			return;
		}
		
		case 0x8001f0: {
			EEPROMWrite(d & 2, d & 1, d & 4);
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write byte => %06X, %02X\n"), a, d);
		}
	}
}

unsigned short __fastcall Zeropnt268KReadWord(unsigned int a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read word => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Zeropnt268KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x80010c: {
			UINT16* ScrollRam = (UINT16*)DrvScrollRam;
			ScrollRam[0] = d;
			return;
		}
		
		case 0x80010e: {
			UINT16* ScrollRam = (UINT16*)DrvScrollRam;
			ScrollRam[1] = d;
			return;
		}
		
		case 0x800110: {
			UINT16* ScrollRam = (UINT16*)DrvScrollRam;
			ScrollRam[2] = d;
			return;
		}
		
		case 0x800114: {
			UINT16* ScrollRam = (UINT16*)DrvScrollRam;
			ScrollRam[4] = d;
			return;
		}
		
		case 0x800116: {
			UINT16* ScrollRam = (UINT16*)DrvScrollRam;
			ScrollRam[5] = d;
			return;
		}
		
		case 0x800120: {
			UINT16* ScrollRam = (UINT16*)DrvScrollRam;
			ScrollRam[10] = d;
			return;
		}
		
		case 0x8001e0: {
			// IRQ Ack???
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned int __fastcall Zeropnt268KReadLong(unsigned int a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read long => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Zeropnt268KWriteLong(unsigned int a, unsigned int d)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write long => %06X, %08X\n"), a, d);
		}
	}
}

static int BurglarxSynchroniseStream(int nSoundRate)
{
	return (long long)SekTotalCycles() * nSoundRate / 16000000;
}

static int BurglarxTilePlaneOffsets[8] = { 0x1800008, 0x1800000, 0x1000008, 0x1000000, 0x0800008, 0x0800000, 0x0000008, 0x0000000 };
static int ZeropntTilePlaneOffsets[8]  = { 0x3000008, 0x3000000, 0x2000008, 0x2000000, 0x1000008, 0x1000000, 0x0000008, 0x0000000 };
static int Zeropnt2TilePlaneOffsets[8] = { 0x6000008, 0x6000000, 0x4000008, 0x4000000, 0x2000008, 0x2000000, 0x0000008, 0x0000000 };
static int TileXOffsets[16]            = { 0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 18, 19, 20, 21, 22, 23 };
static int TileYOffsets[16]            = { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480 };

static int BurglarxInit()
{
	int nRet = 0, nLen;
	
	DrvNumTiles = 0x4000;
	DrvNumSprites = 0x4000;
	
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	DrvTempRom = (unsigned char *)malloc(0x400000);

	// Load 68000 Program Roms
	nRet = BurnLoadRom(Drv68KRom + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Drv68KRom + 0x000000, 1, 2); if (nRet != 0) return 1;
	
	// Load and decode the tiles
	nRet = BurnLoadRom(DrvTempRom + 0x000000, 10, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x000001, 11, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x100000, 12, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x100001, 13, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x200000, 14, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x200001, 15, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x300000, 16, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x300001, 17, 2); if (nRet != 0) return 1;
	for (int i = 0; i < 0x400000; i++) {
		DrvTempRom[i] ^= 0xff;
	}	
	GfxDecode(0x4000, 8, 16, 16, BurglarxTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x200, DrvTempRom, DrvTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x400000);
	nRet = BurnLoadRom(DrvTempRom + 0x000000,  2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x000001,  3, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x100000,  4, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x100001,  5, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x200000,  6, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x200001,  7, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x300000,  8, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x300001,  9, 2); if (nRet != 0) return 1;
	for (int i = 0; i < 0x400000; i++) {
		DrvTempRom[i] ^= 0xff;
	}	
	GfxDecode(0x4000, 8, 16, 16, BurglarxTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x200, DrvTempRom, DrvSprites);
	
	// Load Sample Roms
	nRet = BurnLoadRom(DrvMSM6295ROMSrc + 0x00000, 18, 1); if (nRet != 0) return 1;
	memcpy(MSM6295ROM, DrvMSM6295ROMSrc, 0x40000);
	
	free(DrvTempRom);
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Drv68KRom           , 0x000000, 0x0fffff, SM_ROM);
	SekMapMemory(DrvVideo1Ram        , 0x904000, 0x907fff, SM_RAM);
	SekMapMemory(DrvVideo2Ram        , 0x908000, 0x90bfff, SM_RAM);
	SekMapMemory(DrvVideo0Ram        , 0x90c000, 0x90ffff, SM_RAM);
	SekMapMemory(Drv68KRam + 0x10000 , 0x920000, 0x923fff, SM_RAM);
	SekMapMemory(DrvSpriteRam        , 0x930000, 0x9307ff, SM_RAM);
	SekMapMemory(DrvPaletteRam       , 0x940000, 0x947fff, SM_RAM);
	SekMapMemory(Drv68KRam           , 0xff0000, 0xffffff, SM_RAM);
	SekSetReadWordHandler(0, Burglarx68KReadWord);
	SekSetWriteWordHandler(0, Burglarx68KWriteWord);
	SekSetReadByteHandler(0, Burglarx68KReadByte);
	SekSetWriteByteHandler(0, Burglarx68KWriteByte);
	SekClose();
	
	BurnYM3812Init(3579545, NULL, &BurglarxSynchroniseStream);
	BurnTimerAttachSek(16000000);
	
	// Setup the OKIM6295 emulation
	MSM6295Init(0, 1056000 / 132, 100.0, 1);
	
	GenericTilesInit();
	
	UnicoMakeInputsFunction = BurglarxMakeInputs;

	// Reset the driver
	DrvDoReset();

	return 0;
}

static int ZeropntInit()
{
	int nRet = 0, nLen;
	
	DrvNumTiles = 0x8000;
	DrvNumSprites = 0x8000;
	
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	DrvTempRom = (unsigned char *)malloc(0x800000);

	// Load 68000 Program Roms
	nRet = BurnLoadRom(Drv68KRom + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Drv68KRom + 0x000000, 1, 2); if (nRet != 0) return 1;
	
	// Load and decode the tiles
	nRet = BurnLoadRom(DrvTempRom + 0x000000, 6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x200000, 7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x400000, 8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x600000, 9, 1); if (nRet != 0) return 1;
	for (int i = 0; i < 0x800000; i++) {
		DrvTempRom[i] ^= 0xff;
	}	
	GfxDecode(0x8000, 8, 16, 16, ZeropntTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x200, DrvTempRom, DrvTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x800000);
	nRet = BurnLoadRom(DrvTempRom + 0x000000,  2, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x200000,  3, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x400000,  4, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x600000,  5, 1); if (nRet != 0) return 1;
	for (int i = 0; i < 0x800000; i++) {
		DrvTempRom[i] ^= 0xff;
	}	
	GfxDecode(0x8000, 8, 16, 16, ZeropntTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x200, DrvTempRom, DrvSprites);
	
	// Load Sample Roms
	nRet = BurnLoadRom(DrvMSM6295ROMSrc + 0x00000, 10, 1); if (nRet != 0) return 1;
	memcpy(MSM6295ROM, DrvMSM6295ROMSrc, 0x40000);
	
	free(DrvTempRom);
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Drv68KRom           , 0x000000, 0x0fffff, SM_ROM);
	SekMapMemory(DrvVideo1Ram        , 0x904000, 0x907fff, SM_RAM);
	SekMapMemory(DrvVideo2Ram        , 0x908000, 0x90bfff, SM_RAM);
	SekMapMemory(DrvVideo0Ram        , 0x90c000, 0x90ffff, SM_RAM);
	SekMapMemory(Drv68KRam + 0x10000 , 0x920000, 0x923fff, SM_RAM);
	SekMapMemory(DrvSpriteRam        , 0x930000, 0x9307ff, SM_RAM);
	SekMapMemory(DrvPaletteRam       , 0x940000, 0x947fff, SM_RAM);
	SekMapMemory(Drv68KRam           , 0xef0000, 0xefffff, SM_RAM);
	SekSetReadWordHandler(0, Zeropnt68KReadWord);
	SekSetWriteWordHandler(0, Zeropnt68KWriteWord);
	SekSetReadByteHandler(0, Zeropnt68KReadByte);
	SekSetWriteByteHandler(0, Zeropnt68KWriteByte);
	SekClose();
	
	BurnYM3812Init(3579545, NULL, &BurglarxSynchroniseStream);
	BurnTimerAttachSek(16000000);
	
	// Setup the OKIM6295 emulation
	MSM6295Init(0, 1056000 / 132, 100.0, 1);
	
	GenericTilesInit();
	
	//BurnGunInit(2, true);
	
	UnicoMakeInputsFunction = ZeropntMakeInputs;

	// Reset the driver
	DrvDoReset();

	return 0;
}

static int Zeropnt2Init()
{
	int nRet = 0, nLen;
	
	DrvNumTiles = 0x10000;
	DrvNumSprites = 0x10000;
	
	// Allocate and Blank all required memory
	Mem = NULL;
	Zeropnt2MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	Zeropnt2MemIndex();

	DrvTempRom = (unsigned char *)malloc(0x1000000);

	// Load 68000 Program Roms
	nRet = BurnLoadRom(Drv68KRom + 0x000000, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Drv68KRom + 0x000001, 1, 2); if (nRet != 0) return 1;
	
	for (int i = 0; i < 0x200000; i+= 4) {
		int t = Drv68KRom[i + 1];
		Drv68KRom[i + 1] = Drv68KRom[i + 2];
		Drv68KRom[i + 2] = t;
	}
	
	// Load and decode the tiles
	nRet = BurnLoadRom(DrvTempRom + 0x000000, 6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x400000, 7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x800000, 8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0xc00000, 9, 1); if (nRet != 0) return 1;
	for (int i = 0; i < 0x1000000; i++) {
		DrvTempRom[i] ^= 0xff;
	}	
	GfxDecode(0x10000, 8, 16, 16, Zeropnt2TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x200, DrvTempRom, DrvTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x1000000);
	nRet = BurnLoadRom(DrvTempRom + 0x000000,  2, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x400000,  3, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x800000,  4, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0xc00000,  5, 1); if (nRet != 0) return 1;
	for (int i = 0; i < 0x1000000; i++) {
		DrvTempRom[i] ^= 0xff;
	}	
	GfxDecode(0x10000, 8, 16, 16, Zeropnt2TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x200, DrvTempRom, DrvSprites);
	
	// Load Sample Roms
	nRet = BurnLoadRom(DrvMSM6295ROMSrc + 0x00000, 10, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(MSM6295ROM + 0x100000, 11, 1); if (nRet != 0) return 1;
	memcpy(MSM6295ROM + 0x000000, DrvMSM6295ROMSrc, 0x40000);
	
	free(DrvTempRom);
	
	// Setup the 68000 emulation
	SekInit(0, 0x68EC020);
	SekOpen(0);
	SekMapMemory(Drv68KRom           , 0x000000, 0x1fffff, SM_ROM);
	SekMapMemory(DrvVideo1Ram        , 0x904000, 0x907fff, SM_RAM);
	SekMapMemory(DrvVideo2Ram        , 0x908000, 0x90bfff, SM_RAM);
	SekMapMemory(DrvVideo0Ram        , 0x90c000, 0x90ffff, SM_RAM);
	SekMapMemory(Drv68KRam + 0x10000 , 0x920000, 0x923fff, SM_RAM);
	SekMapMemory(DrvSpriteRam        , 0x930000, 0x9307ff, SM_RAM);
	SekMapMemory(DrvPaletteRam       , 0x940000, 0x947fff, SM_RAM);
	SekMapMemory(Drv68KRam           , 0xfe0000, 0xffffff, SM_RAM);
	SekSetReadWordHandler(0, Zeropnt268KReadWord);
	SekSetWriteWordHandler(0, Zeropnt268KWriteWord);
	SekSetReadByteHandler(0, Zeropnt268KReadByte);
	SekSetWriteByteHandler(0, Zeropnt268KWriteByte);
	SekSetReadLongHandler(0, Zeropnt268KReadLong);
	SekSetWriteLongHandler(0, Zeropnt268KWriteLong);
	SekClose();
	
	EEPROMInit(1024, 8);
	
	BurnYM2151Init(3579545, 25.0);
	
	MSM6295Init(0, 1056000 / 132, 100.0, 1);
	MSM6295Init(1, 3960000 / 132, 100.0, 1);
	
	GenericTilesInit();
	
	//BurnGunInit(2, true);
	
	// Reset the driver
	Zeropnt2DoReset();

	return 0;
}

static int DrvExit()
{
	SekExit();
	
	BurnYM3812Exit();
	MSM6295Exit(0);
	
	GenericTilesExit();
	//BurnGunExit();
	
	DrvScrollX0 = 0;
	DrvScrollY0 = 0;
	DrvScrollX1 = 0;
	DrvScrollY1 = 0;
	DrvScrollX2 = 0;
	DrvScrollY2 = 0;
	DrvOkiBank = 0;
	
	DrvNumTiles = 0;
	DrvNumSprites = 0;
	
	UnicoMakeInputsFunction = NULL;
	
	free(Mem);
	Mem = NULL;

	return 0;
}

static int Zeropnt2Exit()
{
	BurnYM2151Exit();
	MSM6295Exit(1);
	
	EEPROMExit();
	
	return DrvExit();
}

static void DrvRenderSprites(int PriorityDraw)
{
	for (int Offset = 0; Offset < 0x400; Offset += 4) {
		int x, xStart, xEnd, xInc;
		UINT16 *SpriteRam = (UINT16*)DrvSpriteRam;
		
		int sx = SpriteRam[Offset + 0];
		int sy = SpriteRam[Offset + 1];
		int Code = SpriteRam[Offset + 2];
		int Attr = SpriteRam[Offset + 3];
		
		int xFlip = Attr & 0x20;
		int yFlip = Attr & 0x40;
		
		int xDim = ((Attr >> 8) & 0x0f) + 1;
		int Priority = ((Attr >> 12) & 0x03);
		
		if (Priority != PriorityDraw) continue;
		
		sx -= 0x3f;
		sy -= 0x0e;
		
		sx = (sx & 0x1ff) - (sx & 0x200);
		sy = (sy & 0x1ff) - (sy & 0x200);
		
		if (xFlip) {
			xStart = sx + (xDim - 1) * 16;
			xEnd = sx - 16;
			xInc = -16;
		} else {
			xStart = sx;
			xEnd = sx + (xDim * 16);
			xInc = 16;
		}
		
		for (x = xStart; x != xEnd; x += xInc) {
			if (x > 16 && x < 368 && sy > 16 && sy < 208) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				}
			}
		}
	}
}

static void Zeropnt2RenderSprites(int PriorityDraw)
{
	for (int Offset = 0; Offset < 0x200; Offset += 2) {
		int x, xStart, xEnd, xInc;
		UINT32 *SpriteRam = (UINT32*)DrvSpriteRam;
		
		int sx = SpriteRam[Offset + 0] & 0xffff;
		int sy = SpriteRam[Offset + 0] >> 16;
		int Code = SpriteRam[Offset + 1] & 0xffff;
		int Attr = SpriteRam[Offset + 1] >> 16;
		
		int xFlip = Attr & 0x20;
		int yFlip = Attr & 0x40;
		
		int xDim = ((Attr >> 8) & 0x0f) + 1;
		int Priority = ((Attr >> 12) & 0x03);
		
		if (Priority != PriorityDraw) continue;
		
		sx -= 0x3f;
		sy -= 0x0e;
		
		sx = (sx & 0x1ff) - (sx & 0x200);
		sy = (sy & 0x1ff) - (sy & 0x200);
		
		if (xFlip) {
			xStart = sx + (xDim - 1) * 16;
			xEnd = sx - 16;
			xInc = -16;
		} else {
			xStart = sx;
			xEnd = sx + (xDim * 16);
			xInc = 16;
		}
		
		for (x = xStart; x != xEnd; x += xInc) {
			if (x > 16 && x < 368 && sy > 16 && sy < 208) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Code++, x, sy, Attr & 0x1f, 8, 0, 0, DrvSprites);
					}
				}
			}
		}
	}
}

static void DrvRenderLayer(int Layer)
{
	int mx, my, Attr, Code, Colour, x, y, TileIndex = 0, Flip, xFlip, yFlip;
	
	UINT16 ScrollX = DrvScrollX0;
	UINT16 ScrollY = DrvScrollY0;	
	UINT16 *VideoRam = (UINT16*)DrvVideo0Ram;
	
	if (Layer == 1) {
		ScrollX = DrvScrollX1;
		ScrollY = DrvScrollY1;
		VideoRam = (UINT16*)DrvVideo1Ram;
	}
	
	if (Layer == 2) {
		ScrollX = DrvScrollX2;
		ScrollY = DrvScrollY2;
		VideoRam = (UINT16*)DrvVideo2Ram;
	}
	
	for (my = 0; my < 64; my++) {
		for (mx = 0; mx < 64; mx++) {
			Attr = VideoRam[2 * TileIndex + 1];
			Code = VideoRam[2 * TileIndex + 0];
			Colour = Attr & 0x1f;
			Flip = Attr >> 5;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			
			x = 16 * mx;
			y = 16 * my;
			
			x -= ScrollX;
			y -= ScrollY;
			if (x < -16) x += 1024;
			if (y < -16) y += 1024;
			
			y -= 15;
			if (Layer == 0) x -= 0x32;
			if (Layer == 1) x -= 0x30;
			if (Layer == 2) x -= 0x2e;
			
			if (x > 16 && x < 368 && y > 16 && y < 208) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					} else {
						Render16x16Tile_Mask(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
					}
				}
			}
			
			TileIndex++;
		}
	}
}

static void Zeropnt2RenderLayer(int Layer)
{
	int mx, my, Attr, Colour, x, y, TileIndex = 0, Flip, xFlip, yFlip;
	UINT32 Code;
	
	UINT32 *VideoRam = (UINT32*)DrvVideo0Ram;
	UINT32 *ScrollRam = (UINT32*)DrvScrollRam;
	
	UINT16 ScrollX = ScrollRam[0] & 0xffff;
	UINT16 ScrollY = ScrollRam[0] >> 16;
	
	if (Layer == 1) {
		ScrollX = ScrollRam[2] >> 16;
		ScrollY = ScrollRam[5] & 0xffff;
		VideoRam = (UINT32*)DrvVideo1Ram;
	}
	
	if (Layer == 2) {
		ScrollX = ScrollRam[2] & 0xffff;
		ScrollY = ScrollRam[1] & 0xffff;
		VideoRam = (UINT32*)DrvVideo2Ram;
	}
	
	ScrollX &= 0x3ff;
	ScrollY &= 0x3ff;
	
	for (my = 0; my < 64; my++) {
		for (mx = 0; mx < 64; mx++) {
			Code = VideoRam[TileIndex];
			Attr = Code >> 16;
			Code &= 0xffff;
			Colour = Attr & 0x1f;
			Flip = Attr >> 5;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			
			x = 16 * mx;
			y = 16 * my;
			
			x -= ScrollX;
			y -= ScrollY;
			if (x < -16) x += 1024;
			if (y < -16) y += 1024;
			
			y -= 15;
			if (Layer == 0) x -= 0x32;
			if (Layer == 1) x -= 0x30;
			if (Layer == 2) x -= 0x2e;
			
			if (Code) {
				if (x > 16 && x < 368 && y > 16 && y < 208) {
					if (xFlip) {
						if (yFlip) {
							Render16x16Tile_Mask_FlipXY(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						} else {
							Render16x16Tile_Mask_FlipX(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						}
					} else {
						if (yFlip) {
							Render16x16Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						} else {
							Render16x16Tile_Mask(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						}
					}
				} else {
					if (xFlip) {
						if (yFlip) {
							Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						} else {
							Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						}
					} else {
						if (yFlip) {
							Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						} else {
							Render16x16Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 8, 0, 0, DrvTiles);
						}
					}
				}
			}
			
			TileIndex++;
		}
	}
}

static void DrvCalcPalette()
{
	UINT16 Data1, Data2;
	UINT16 *PaletteRam = (UINT16*)DrvPaletteRam;
	
	for (int i = 0; i < 0x4000; i += 2) {
		 Data1 = PaletteRam[i & ~1];
		 Data2 = PaletteRam[i |  1];
		 
		 DrvPalette[i >> 1] = BurnHighCol((Data1 >> 8) & 0xfc, (Data1 >> 0) & 0xfc, (Data2 >> 8) & 0xfc, 0);
	}
}

static void Zeropnt2CalcPalette()
{
	UINT32 *PaletteRam = (UINT32*)DrvPaletteRam;
	UINT32 rgb0;
	
	for (int i = 0; i < 0x2000; i++) {
		 rgb0 = PaletteRam[i];
		 
		 DrvPalette[i] = BurnHighCol((rgb0 >> 8) & 0xfc, (rgb0 >> 0) & 0xfc, (rgb0 >> 24) & 0xfc, 0);
	}
}

static void DrvDraw()
{
	BurnTransferClear();
	DrvCalcPalette();
	
	for (int i = 0; i < nScreenHeight * nScreenWidth; i++) {
		pTransDraw[i] = 0x1f00;	
	}
	
	DrvRenderLayer(0);
	DrvRenderSprites(0);
	DrvRenderLayer(1);
	DrvRenderSprites(1);
	DrvRenderLayer(2);
	DrvRenderSprites(2);
	DrvRenderSprites(3);
	
	BurnTransferCopy(DrvPalette);
	
/*	for (int i = 0; i < nBurnGunNumPlayers; i++) {
		BurnGunDrawTarget(i, BurnGunX[i] >> 8, BurnGunY[i] >> 8);
	}
*/
}

static void Zeropnt2Draw()
{
	BurnTransferClear();
	Zeropnt2CalcPalette();
	
	for (int i = 0; i < nScreenHeight * nScreenWidth; i++) {
		pTransDraw[i] = 0x1f00;	
	}
	
	Zeropnt2RenderSprites(3);
	Zeropnt2RenderLayer(0);
	Zeropnt2RenderSprites(2);
	Zeropnt2RenderLayer(1);
	Zeropnt2RenderSprites(1);
	Zeropnt2RenderLayer(2);	
	Zeropnt2RenderSprites(0);
	
	BurnTransferCopy(DrvPalette);
	
/*	for (int i = 0; i < nBurnGunNumPlayers; i++) {
		BurnGunDrawTarget(i, BurnGunX[i] >> 8, BurnGunY[i] >> 8);
	}
*/
}

static int DrvFrame()
{
	if (DrvReset) DrvDoReset();

	UnicoMakeInputsFunction();

	nCyclesTotal[0] = 16000000 / 60;
	nCyclesDone[0] = 0;

	SekNewFrame();
	
	SekOpen(0);
	BurnTimerEndFrame(nCyclesTotal[0]);
	SekSetIRQLine(2, SEK_IRQSTATUS_AUTO);
	BurnYM3812Update(nBurnSoundLen);
	MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);	
	SekClose();
	
	if (pBurnDraw) DrvDraw();

	return 0;
}

static int Zeropnt2Frame()
{
	int nInterleave = 10;
	int nSoundBufferPos = 0;
	
	if (DrvReset) Zeropnt2DoReset();

	Zeropnt2MakeInputs();

	nCyclesTotal[0] = 12800000 / 60;
	nCyclesDone[0] = 0;

	SekNewFrame();
	
	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run 68000
		nCurrentCPU = 0;
		SekOpen(0);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		if (i == 9) SekSetIRQLine(2, SEK_IRQSTATUS_AUTO);
		SekClose();

		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen / nInterleave;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			BurnYM2151Render(pSoundBuf, nSegmentLength);
			MSM6295Render(0, pSoundBuf, nSegmentLength);
			MSM6295Render(1, pSoundBuf, nSegmentLength);
			nSoundBufferPos += nSegmentLength;
		}	
	}
	
	if (pBurnSoundOut) {
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);

		if (nSegmentLength) {
			BurnYM2151Render(pSoundBuf, nSegmentLength);
			MSM6295Render(0, pSoundBuf, nSegmentLength);
			MSM6295Render(1, pSoundBuf, nSegmentLength);
		}
	}
	
	if (pBurnDraw) Zeropnt2Draw();

	return 0;
}

static int DrvScan(int nAction, int *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {			// Return minimum compatible version
		*pnMin = 0x029671;
	}

	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = RamStart;
		ba.nLen	  = RamEnd-RamStart;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SekScan(nAction);
		MSM6295Scan(0, nAction);

		// Scan critical driver variables
		SCAN_VAR(nCyclesDone);
		SCAN_VAR(nCyclesSegment);
		SCAN_VAR(DrvDip);
		SCAN_VAR(DrvInput);
		SCAN_VAR(DrvOkiBank);
	}

	return 0;
}

static int BurglarxScan(int nAction, int *pnMin)
{
	DrvScan(nAction, pnMin);
	
	if (nAction & ACB_DRIVER_DATA) {
		BurnYM3812Scan(nAction, pnMin);
	}
	
	if (nAction & ACB_WRITE) {
		memcpy(MSM6295ROM + 0x00000, DrvMSM6295ROMSrc + (0x40000 * DrvOkiBank), 0x40000);
	}
	
	return 0;
}

static int ZeropntScan(int nAction, int *pnMin)
{
	DrvScan(nAction, pnMin);
	
	if (nAction & ACB_DRIVER_DATA) {
		BurnYM3812Scan(nAction, pnMin);
	}
	
	if (nAction & ACB_WRITE) {
		memcpy(MSM6295ROM + 0x20000, DrvMSM6295ROMSrc + 0x20000 + (0x20000 * DrvOkiBank), 0x20000);
	}
	
	return 0;
}

static int Zeropnt2Scan(int nAction, int *pnMin)
{
	DrvScan(nAction, pnMin);
	
	EEPROMScan(nAction, pnMin);
	
	if (nAction & ACB_DRIVER_DATA) {
		BurnYM2151Scan(nAction);
		MSM6295Scan(1, nAction);
	}
	
	if (nAction & ACB_WRITE) {
		memcpy(MSM6295ROM + 0x20000, DrvMSM6295ROMSrc + 0x20000 + (0x20000 * DrvOkiBank), 0x20000);
	}
	
	return 0;
}

struct BurnDriver BurnDrvBurglarx = {
	"burglarx", NULL, NULL, "1997",
	"Burglar X\0", NULL, "Unico", "Unico",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_MISC,
	NULL, BurglarxRomInfo, BurglarxRomName, BurglarxInputInfo, BurglarxDIPInfo,
	BurglarxInit, DrvExit, DrvFrame, NULL, BurglarxScan,
	0, 384, 224, 4, 3
};

struct BurnDriver BurnDrvZeropnt = {
	"zeropnt", NULL, NULL, "1998",
	"Zero Point (set 1)\0", NULL, "Unico", "Unico",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_MISC,
	NULL, ZeropntRomInfo, ZeropntRomName, ZeropntInputInfo, ZeropntDIPInfo,
	ZeropntInit, DrvExit, DrvFrame, NULL, ZeropntScan,
	0, 384, 224, 4, 3
};

struct BurnDriver BurnDrvZeropnta = {
	"zeropnta", "zeropnt", NULL, "1998",
	"Zero Point (set 2)\0", NULL, "Unico", "Unico",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_MISC,
	NULL, ZeropntaRomInfo, ZeropntaRomName, ZeropntInputInfo, ZeropntDIPInfo,
	ZeropntInit, DrvExit, DrvFrame, NULL, ZeropntScan,
	0, 384, 224, 4, 3
};

struct BurnDriver BurnDrvZeropnt2 = {
	"zeropnt2", NULL, NULL, "1999",
	"Zero Point 2\0", NULL, "Unico", "Unico",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_MISC,
	NULL, Zeropnt2RomInfo, Zeropnt2RomName, ZeropntInputInfo, Zeropnt2DIPInfo,
	Zeropnt2Init, Zeropnt2Exit, Zeropnt2Frame, NULL, Zeropnt2Scan,
	0, 384, 224, 4, 3
};
