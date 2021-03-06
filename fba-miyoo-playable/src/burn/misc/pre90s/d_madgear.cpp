#include "tiles_generic.h"
#include "burn_ym2203.h"
#include "msm6295.h"

static unsigned char DrvInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvDip[3]        = {0, 0, 0};
static unsigned char DrvInput[3]      = {0x00, 0x00, 0x00};
static unsigned char DrvReset         = 0;

static unsigned char *Mem                 = NULL;
static unsigned char *MemEnd              = NULL;
static unsigned char *RamStart            = NULL;
static unsigned char *RamEnd              = NULL;
static unsigned char *Drv68KRom           = NULL;
static unsigned char *Drv68KRam           = NULL;
static unsigned char *DrvZ80Rom           = NULL;
static unsigned char *DrvZ80Ram           = NULL;
static unsigned char *DrvVideoRam         = NULL;
static unsigned char *DrvSpriteRam        = NULL;
static unsigned char *DrvSpriteRamBuffer  = NULL;
static unsigned char *DrvPaletteRam       = NULL;
static unsigned char *DrvScroll1Ram       = NULL;
static unsigned char *DrvScroll2Ram       = NULL;
static unsigned char *DrvChars            = NULL;
static unsigned char *DrvBgTiles          = NULL;
static unsigned char *DrvFgTiles          = NULL;
static unsigned char *DrvSprites          = NULL;
static unsigned char *DrvTempRom          = NULL;
static unsigned int  *DrvPalette          = NULL;

static UINT16 DrvFgScrollX;
static UINT16 DrvFgScrollY;
static UINT16 DrvBgScrollX;
static UINT16 DrvBgScrollY;
static unsigned char DrvSpritePriMask;
static unsigned char DrvSpriteFlipYMask;
static unsigned char DrvZ80RomBank;
static unsigned char DrvSoundLatch;

static int nCyclesDone[2], nCyclesTotal[2];
static int nCyclesSegment;

static struct BurnInputInfo DrvInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort2 + 4, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort2 + 2, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort2 + 3, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort2 + 1, "p2 start"  },

	{"Up"                , BIT_DIGITAL  , DrvInputPort0 + 7, "p1 up"     },
	{"Down"              , BIT_DIGITAL  , DrvInputPort0 + 6, "p1 down"   },
	{"Left"              , BIT_DIGITAL  , DrvInputPort0 + 5, "p1 left"   },
	{"Right"             , BIT_DIGITAL  , DrvInputPort0 + 4, "p1 right"  },
	{"Fire 1"            , BIT_DIGITAL  , DrvInputPort0 + 3, "p1 fire 1" },
	
	{"Up (Cocktail)"     , BIT_DIGITAL  , DrvInputPort1 + 7, "p2 up"     },
	{"Down (Cocktail)"   , BIT_DIGITAL  , DrvInputPort1 + 6, "p2 down"   },
	{"Left (Cocktail)"   , BIT_DIGITAL  , DrvInputPort1 + 5, "p2 left"   },
	{"Right (Cocktail)"  , BIT_DIGITAL  , DrvInputPort1 + 4, "p2 right"  },
	{"Fire 1 (Cocktail)" , BIT_DIGITAL  , DrvInputPort1 + 3, "p2 fire 1" },

	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort2 + 7, "service"   },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
	{"Dip 3"             , BIT_DIPSWITCH, DrvDip + 2       , "dip"       },
};

STDINPUTINFO(Drv);

static struct BurnInputInfo LastduelInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort2 + 7, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort2 + 0, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort2 + 6, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort2 + 1, "p2 start"  },

	{"Up"                , BIT_DIGITAL  , DrvInputPort0 + 3, "p1 up"     },
	{"Down"              , BIT_DIGITAL  , DrvInputPort0 + 2, "p1 down"   },
	{"Left"              , BIT_DIGITAL  , DrvInputPort0 + 1, "p1 left"   },
	{"Right"             , BIT_DIGITAL  , DrvInputPort0 + 0, "p1 right"  },
	{"Fire 1"            , BIT_DIGITAL  , DrvInputPort0 + 4, "p1 fire 1" },
	{"Fire 2"            , BIT_DIGITAL  , DrvInputPort0 + 5, "p1 fire 2" },
	
	{"Up (Cocktail)"     , BIT_DIGITAL  , DrvInputPort1 + 3, "p2 up"     },
	{"Down (Cocktail)"   , BIT_DIGITAL  , DrvInputPort1 + 2, "p2 down"   },
	{"Left (Cocktail)"   , BIT_DIGITAL  , DrvInputPort1 + 1, "p2 left"   },
	{"Right (Cocktail)"  , BIT_DIGITAL  , DrvInputPort1 + 0, "p2 right"  },
	{"Fire 1 (Cocktail)" , BIT_DIGITAL  , DrvInputPort1 + 4, "p2 fire 1" },
	{"Fire 2 (Cocktail)" , BIT_DIGITAL  , DrvInputPort1 + 5, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort2 + 5, "service"   },
	{"Diagnostics"       , BIT_DIGITAL  , DrvInputPort2 + 3, "diag"      },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
	{"Dip 3"             , BIT_DIPSWITCH, DrvDip + 2       , "dip"       },
};

STDINPUTINFO(Lastduel);

static inline void DrvClearOpposites(unsigned char* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x03) {
		*nJoystickInputs &= ~0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x0c) {
		*nJoystickInputs &= ~0x0c;
	}
}

static inline void DrvMakeInputs()
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
	DrvClearOpposites(&DrvInput[0]);
	DrvClearOpposites(&DrvInput[1]);
}

static struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x10, 0xff, 0xff, 0xff, NULL                     },
	{0x11, 0xff, 0xff, 0xff, NULL                     },
	{0x12, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x10, 0x01, 0x80, 0x80, "Off"                    },
	{0x10, 0x01, 0x80, 0x00, "On"                     },
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Allow continue"         },
	{0x11, 0x01, 0x01, 0x00, "No"                     },
	{0x11, 0x01, 0x01, 0x01, "Yes"                    },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x11, 0x01, 0x02, 0x02, "Off"                    },
	{0x11, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x11, 0x01, 0x0c, 0x08, "Easy"                   },
	{0x11, 0x01, 0x0c, 0x0c, "Normal"                 },
	{0x11, 0x01, 0x0c, 0x04, "Difficult"              },
	{0x11, 0x01, 0x0c, 0x00, "Very Difficult"         },
	
	{0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x11, 0x01, 0x30, 0x30, "Upright (One Player)"   },
	{0x11, 0x01, 0x30, 0x00, "Upright (Two Players)"  },
	{0x11, 0x01, 0x30, 0x10, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x11, 0x01, 0x40, 0x00, "Off"                    },
	{0x11, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Background Music"       },
	{0x11, 0x01, 0x80, 0x00, "Off"                    },
	{0x11, 0x01, 0x80, 0x80, "On"                     },
	
	// Dip3
	{0   , 0xfe, 0   , 16  , "Coin A"                 },
	{0x12, 0x01, 0xf0, 0x20, "6 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x40, "5 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x50, "4 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x70, "3 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x10, "8 Coins 3 Plays"        },
	{0x12, 0x01, 0xf0, 0x90, "2 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x30, "5 Coins 3 Plays"        },
	{0x12, 0x01, 0xf0, 0x60, "3 Coins 2 Plays"        },
	{0x12, 0x01, 0xf0, 0xf0, "1 Coin  1 Play"         },
	{0x12, 0x01, 0xf0, 0x80, "2 Coins 3 Plays"        },
	{0x12, 0x01, 0xf0, 0xe0, "1 Coin  2 Plays"        },
	{0x12, 0x01, 0xf0, 0xd0, "1 Coin  3 Plays"        },
	{0x12, 0x01, 0xf0, 0xc0, "1 Coin  4 Plays"        },
	{0x12, 0x01, 0xf0, 0xb0, "1 Coin  5 Plays"        },
	{0x12, 0x01, 0xf0, 0xa0, "1 Coin  6 Plays"        },
	{0x12, 0x01, 0xf0, 0x00, "Freeplay"               },
	
	{0   , 0xfe, 0   , 15  , "Coin B"                 },
	{0x12, 0x01, 0x0f, 0x02, "6 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x04, "5 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x05, "4 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x07, "3 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x01, "8 Coins 3 Plays"        },
	{0x12, 0x01, 0x0f, 0x09, "2 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x03, "5 Coins 3 Plays"        },
	{0x12, 0x01, 0x0f, 0x06, "3 Coins 2 Plays"        },
	{0x12, 0x01, 0x0f, 0x0f, "1 Coin  1 Play"         },
	{0x12, 0x01, 0x0f, 0x08, "2 Coins 3 Plays"        },
	{0x12, 0x01, 0x0f, 0x0e, "1 Coin  2 Plays"        },
	{0x12, 0x01, 0x0f, 0x0d, "1 Coin  3 Plays"        },
	{0x12, 0x01, 0x0f, 0x0c, "1 Coin  4 Plays"        },
	{0x12, 0x01, 0x0f, 0x0b, "1 Coin  5 Plays"        },
	{0x12, 0x01, 0x0f, 0x0a, "1 Coin  6 Plays"        },
};

STDDIPINFO(Drv);

static struct BurnDIPInfo LastduelDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xff, NULL                     },
	{0x15, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 8   , "Coin A"                 },
	{0x13, 0x01, 0x07, 0x00, "4 Coins 1 Play"         },
	{0x13, 0x01, 0x07, 0x01, "3 Coins 1 Play"         },
	{0x13, 0x01, 0x07, 0x02, "2 Coins 1 Play"         },
	{0x13, 0x01, 0x07, 0x07, "1 Coin  1 Play"         },
	{0x13, 0x01, 0x07, 0x06, "1 Coin  2 Plays"        },
	{0x13, 0x01, 0x07, 0x05, "1 Coin  3 Plays"        },
	{0x13, 0x01, 0x07, 0x04, "1 Coin  4 Plays"        },
	{0x13, 0x01, 0x07, 0x03, "1 Coin  6 Plays"        },
	
	{0   , 0xfe, 0   , 8   , "Coin B"                 },
	{0x13, 0x01, 0x38, 0x00, "4 Coins 1 Play"         },
	{0x13, 0x01, 0x38, 0x08, "3 Coins 1 Play"         },
	{0x13, 0x01, 0x38, 0x10, "2 Coins 1 Play"         },
	{0x13, 0x01, 0x38, 0x38, "1 Coin  1 Play"         },
	{0x13, 0x01, 0x38, 0x30, "1 Coin  2 Plays"        },
	{0x13, 0x01, 0x38, 0x28, "1 Coin  3 Plays"        },
	{0x13, 0x01, 0x38, 0x20, "1 Coin  4 Plays"        },
	{0x13, 0x01, 0x38, 0x18, "1 Coin  6 Plays"        },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x14, 0x01, 0x03, 0x02, "Easy"                   },
	{0x14, 0x01, 0x03, 0x03, "Normal"                 },
	{0x14, 0x01, 0x03, 0x01, "Difficult"              },
	{0x14, 0x01, 0x03, 0x00, "Very Difficult"         },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x14, 0x01, 0x30, 0x20, "20000, 60000, 80000"    },
	{0x14, 0x01, 0x30, 0x30, "30000, 80000, 80000"    },
	{0x14, 0x01, 0x30, 0x10, "40000, 80000, 80000"    },
	{0x14, 0x01, 0x30, 0x00, "40000, 80000, 100000"   },	
	
	// Dip3
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x15, 0x01, 0x03, 0x03, "3"                      },
	{0x15, 0x01, 0x03, 0x02, "4"                      },
	{0x15, 0x01, 0x03, 0x01, "6"                      },
	{0x15, 0x01, 0x03, 0x00, "8"                      },
	
	{0   , 0xfe, 0   , 2   , "Type"                   },
	{0x15, 0x01, 0x04, 0x04, "Car"                    },
	{0x15, 0x01, 0x04, 0x00, "Plane"                  },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x15, 0x01, 0x20, 0x00, "Off"                    },
	{0x15, 0x01, 0x20, 0x20, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Allow continue"         },
	{0x15, 0x01, 0x40, 0x00, "No"                     },
	{0x15, 0x01, 0x40, 0x40, "Yes"                    },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x15, 0x01, 0x80, 0x80, "Off"                    },
	{0x15, 0x01, 0x80, 0x00, "On"                     },
	
};

STDDIPINFO(Lastduel);

static struct BurnRomInfo DrvRomDesc[] = {
	{ "mg_04.rom",     0x20000, 0xb112257d, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "mg_03.rom",     0x20000, 0xb2672465, BRF_ESS | BRF_PRG }, //	 1
	{ "mg_02.rom",     0x20000, 0x9f5ebe16, BRF_ESS | BRF_PRG }, //	 2
	{ "mg_01.rom",     0x20000, 0x1cea2af0, BRF_ESS | BRF_PRG }, //	 3
	
	{ "mg_05.rom",     0x10000, 0x2fbfc945, BRF_ESS | BRF_PRG }, //  4	Z80 Program 
	
	{ "mg_06.rom",     0x08000, 0x382ee59b, BRF_GRA },	     //  5	Characters
	
	{ "ls-12",         0x40000, 0x6c1b2c6c, BRF_GRA },	     //  6	BG Tiles
	
	{ "ls-11",         0x80000, 0x6bf81c64, BRF_GRA },	     //  7	FG Tiles
	
	{ "mg_m11.rom",    0x10000, 0xee319a64, BRF_GRA },	     //  8	Sprites
	{ "mg_m07.rom",    0x10000, 0xe5c0b211, BRF_GRA },	     //  9
	{ "mg_m12.rom",    0x10000, 0x887ef120, BRF_GRA },	     //  10
	{ "mg_m08.rom",    0x10000, 0x59709aa3, BRF_GRA },	     //  11
	{ "mg_m13.rom",    0x10000, 0xeae07db4, BRF_GRA },	     //  12
	{ "mg_m09.rom",    0x10000, 0x40ee83eb, BRF_GRA },	     //  13
	{ "mg_m14.rom",    0x10000, 0x21e5424c, BRF_GRA },	     //  14
	{ "mg_m10.rom",    0x10000, 0xb64afb54, BRF_GRA },	     //  15
	
	{ "ls-06",         0x20000, 0x88d39a5b, BRF_SND },	     //  16	Samples
	{ "ls-05",         0x20000, 0xb06e03b5, BRF_SND },	     //  17
	
	{ "63s141.14k",    0x00100, 0x7f862e1e, BRF_GRA },	     //  18	PROM (Priority)
};

STD_ROM_PICK(Drv);
STD_ROM_FN(Drv);

static struct BurnRomInfo DrvjRomDesc[] = {
	{ "mdj_04.rom",    0x20000, 0x9ebbebb1, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "mdj_03.rom",    0x20000, 0xa5579c2d, BRF_ESS | BRF_PRG }, //	 1
	{ "mg_02.rom",     0x20000, 0x9f5ebe16, BRF_ESS | BRF_PRG }, //	 2
	{ "mg_01.rom",     0x20000, 0x1cea2af0, BRF_ESS | BRF_PRG }, //	 3
	
	{ "mg_05.rom",     0x10000, 0x2fbfc945, BRF_ESS | BRF_PRG }, //  4	Z80 Program 
	
	{ "mg_06.rom",     0x08000, 0x382ee59b, BRF_GRA },	     //  5	Characters
	
	{ "ls-12",         0x40000, 0x6c1b2c6c, BRF_GRA },	     //  6	BG Tiles
	
	{ "ls-11",         0x80000, 0x6bf81c64, BRF_GRA },	     //  7	FG Tiles
	
	{ "mg_m11.rom",    0x10000, 0xee319a64, BRF_GRA },	     //  8	Sprites
	{ "mg_m07.rom",    0x10000, 0xe5c0b211, BRF_GRA },	     //  9
	{ "mg_m12.rom",    0x10000, 0x887ef120, BRF_GRA },	     //  10
	{ "mg_m08.rom",    0x10000, 0x59709aa3, BRF_GRA },	     //  11
	{ "mg_m13.rom",    0x10000, 0xeae07db4, BRF_GRA },	     //  12
	{ "mg_m09.rom",    0x10000, 0x40ee83eb, BRF_GRA },	     //  13
	{ "mg_m14.rom",    0x10000, 0x21e5424c, BRF_GRA },	     //  14
	{ "mg_m10.rom",    0x10000, 0xb64afb54, BRF_GRA },	     //  15
	
	{ "ls-06",         0x20000, 0x88d39a5b, BRF_SND },	     //  16	Samples
	{ "ls-05",         0x20000, 0xb06e03b5, BRF_SND },	     //  17
	
	{ "63s141.14k",    0x00100, 0x7f862e1e, BRF_GRA },	     //  18	PROM (Priority)
};

STD_ROM_PICK(Drvj);
STD_ROM_FN(Drvj);

static struct BurnRomInfo DrvuRomDesc[] = {
	{ "mdu.04",        0x20000, 0x7f7f8329, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "mdu.03",        0x20000, 0x11fa542f, BRF_ESS | BRF_PRG }, //	 1
	{ "mg_02.rom",     0x20000, 0x9f5ebe16, BRF_ESS | BRF_PRG }, //	 2
	{ "mg_01.rom",     0x20000, 0x1cea2af0, BRF_ESS | BRF_PRG }, //	 3
	
	{ "mg_05.rom",     0x10000, 0x2fbfc945, BRF_ESS | BRF_PRG }, //  4	Z80 Program 
	
	{ "06",            0x08000, 0x54bfdc02, BRF_GRA },	     //  5	Characters
	
	{ "ls-12",         0x40000, 0x6c1b2c6c, BRF_GRA },	     //  6	BG Tiles
	
	{ "ls-11",         0x80000, 0x6bf81c64, BRF_GRA },	     //  7	FG Tiles
	
	{ "mg_m11.rom",    0x10000, 0xee319a64, BRF_GRA },	     //  8	Sprites
	{ "07",            0x10000, 0x7152b212, BRF_GRA },	     //  9
	{ "mg_m12.rom",    0x10000, 0x887ef120, BRF_GRA },	     //  10
	{ "08",            0x10000, 0x72e5d525, BRF_GRA },	     //  11
	{ "mg_m13.rom",    0x10000, 0xeae07db4, BRF_GRA },	     //  12
	{ "09",            0x10000, 0x7b5175cb, BRF_GRA },	     //  13
	{ "mg_m14.rom",    0x10000, 0x21e5424c, BRF_GRA },	     //  14
	{ "10",            0x10000, 0x6db7ca64, BRF_GRA },	     //  15
	
	{ "ls-06",         0x20000, 0x88d39a5b, BRF_SND },	     //  16	Samples
	{ "ls-05",         0x20000, 0xb06e03b5, BRF_SND },	     //  17
	
	{ "63s141.14k",    0x00100, 0x7f862e1e, BRF_GRA },	     //  18	PROM (Priority)
};

STD_ROM_PICK(Drvu);
STD_ROM_FN(Drvu);

static struct BurnRomInfo Ledstrm2RomDesc[] = {
	{ "lsu-04.bin",    0x20000, 0x56a2f079, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "lsu-03.bin",    0x20000, 0x9b6408c0, BRF_ESS | BRF_PRG }, //	 1
	{ "ls-02.bin",     0x20000, 0x05c0285e, BRF_ESS | BRF_PRG }, //	 2
	{ "ls-01.bin",     0x20000, 0x8bf934dd, BRF_ESS | BRF_PRG }, //	 3
	
	{ "ls-07.bin",     0x10000, 0x98af7838, BRF_ESS | BRF_PRG }, //  4	Z80 Program 
	
	{ "ls-08.bin",     0x08000, 0x8803cf49, BRF_GRA },	     //  5	Characters
	
	{ "ls-12",         0x40000, 0x6c1b2c6c, BRF_GRA },	     //  6	BG Tiles
	
	{ "ls-11",         0x80000, 0x6bf81c64, BRF_GRA },	     //  7	FG Tiles
	
	{ "mg_m11.rom",    0x10000, 0xee319a64, BRF_GRA },	     //  8	Sprites
	{ "07",            0x10000, 0x7152b212, BRF_GRA },	     //  9
	{ "mg_m12.rom",    0x10000, 0x887ef120, BRF_GRA },	     //  10
	{ "08",            0x10000, 0x72e5d525, BRF_GRA },	     //  11
	{ "mg_m13.rom",    0x10000, 0xeae07db4, BRF_GRA },	     //  12
	{ "09",            0x10000, 0x7b5175cb, BRF_GRA },	     //  13
	{ "mg_m14.rom",    0x10000, 0x21e5424c, BRF_GRA },	     //  14
	{ "10",            0x10000, 0x6db7ca64, BRF_GRA },	     //  15
	
	{ "ls-06",         0x20000, 0x88d39a5b, BRF_SND },	     //  16	Samples
	{ "ls-05",         0x20000, 0xb06e03b5, BRF_SND },	     //  17
	
	{ "63s141.14k",    0x00100, 0x7f862e1e, BRF_GRA },	     //  18	PROM (Priority)
};

STD_ROM_PICK(Ledstrm2);
STD_ROM_FN(Ledstrm2);

static struct BurnRomInfo LastduelRomDesc[] = {
	{ "ldu-06.rom",    0x20000, 0x4228a00b, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "ldu-05.rom",    0x20000, 0x7260434f, BRF_ESS | BRF_PRG }, //	 1
	{ "ldu-04.rom",    0x10000, 0x429fb964, BRF_ESS | BRF_PRG }, //	 2
	{ "ldu-03.rom",    0x10000, 0x5aa4df72, BRF_ESS | BRF_PRG }, //	 3
	
	{ "ld_02.bin",     0x10000, 0x91834d0c, BRF_ESS | BRF_PRG }, //  4	Z80 Program 
	
	{ "ld_01.bin",     0x08000, 0xad3c6f87, BRF_GRA },	     //  5	Characters
	
	{ "ld_17.bin",     0x10000, 0x7188bfdd, BRF_GRA },	     //  6	BG Tiles
	{ "ld_18.bin",     0x10000, 0xa62af66a, BRF_GRA },	     //  7
	{ "ld_19.bin",     0x10000, 0x4b762e50, BRF_GRA },	     //  8
	{ "ld_20.bin",     0x10000, 0xb140188e, BRF_GRA },	     //  9
	
	{ "ld_28.bin",     0x10000, 0x06778248, BRF_GRA },	     //  10	FG Tiles
	{ "ld_26.bin",     0x10000, 0xb0edac81, BRF_GRA },	     //  11
	{ "ld_24.bin",     0x10000, 0x66eac4df, BRF_GRA },	     //  12
	{ "ld_22.bin",     0x10000, 0xf80f8812, BRF_GRA },	     //  13
	{ "ld_27.bin",     0x10000, 0x48c78675, BRF_GRA },	     //  14
	{ "ld_25.bin",     0x10000, 0xc541ae9a, BRF_GRA },	     //  15
	{ "ld_23.bin",     0x10000, 0xd817332c, BRF_GRA },	     //  16
	{ "ld_21.bin",     0x10000, 0xb74f0c0e, BRF_GRA },	     //  17
	
	{ "ld_09.bin",     0x10000, 0xf8fd5243, BRF_GRA },	     //  18	Sprites
	{ "ld_10.bin",     0x10000, 0xb49ad746, BRF_GRA },	     //  19
	{ "ld_11.bin",     0x10000, 0x1a0d180e, BRF_GRA },	     //  20
	{ "ld_12.bin",     0x10000, 0xb2745e26, BRF_GRA },	     //  21
	{ "ld_15.bin",     0x10000, 0x96b13bbc, BRF_GRA },	     //  22
	{ "ld_16.bin",     0x10000, 0x9d80f7e6, BRF_GRA },	     //  23
	{ "ld_13.bin",     0x10000, 0xa1a598ac, BRF_GRA },	     //  24
	{ "ld_14.bin",     0x10000, 0xedf515cc, BRF_GRA },	     //  25
	
	{ "63s141.3d",     0x00100, 0x729a1ddc, BRF_GRA },  	     //  26	PROM (Priority)
};

STD_ROM_PICK(Lastduel);
STD_ROM_FN(Lastduel);

static struct BurnRomInfo LstduelaRomDesc[] = {
	{ "06",            0x20000, 0x0e71acaf, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "05",            0x20000, 0x47a85bea, BRF_ESS | BRF_PRG }, //	 1
	{ "04",            0x10000, 0xaa4bf001, BRF_ESS | BRF_PRG }, //	 2
	{ "03",            0x10000, 0xbbaac8ab, BRF_ESS | BRF_PRG }, //	 3
	
	{ "ld_02.bin",     0x10000, 0x91834d0c, BRF_ESS | BRF_PRG }, //  4	Z80 Program 
	
	{ "ld_01.bin",     0x08000, 0xad3c6f87, BRF_GRA },	     //  5	Characters
	
	{ "ld_17.bin",     0x10000, 0x7188bfdd, BRF_GRA },	     //  6	BG Tiles
	{ "ld_18.bin",     0x10000, 0xa62af66a, BRF_GRA },	     //  7
	{ "ld_19.bin",     0x10000, 0x4b762e50, BRF_GRA },	     //  8
	{ "ld_20.bin",     0x10000, 0xb140188e, BRF_GRA },	     //  9
	
	{ "ld_28.bin",     0x10000, 0x06778248, BRF_GRA },	     //  10	FG Tiles
	{ "ld_26.bin",     0x10000, 0xb0edac81, BRF_GRA },	     //  11
	{ "ld_24.bin",     0x10000, 0x66eac4df, BRF_GRA },	     //  12
	{ "ld_22.bin",     0x10000, 0xf80f8812, BRF_GRA },	     //  13
	{ "ld_27.bin",     0x10000, 0x48c78675, BRF_GRA },	     //  14
	{ "ld_25.bin",     0x10000, 0xc541ae9a, BRF_GRA },	     //  15
	{ "ld_23.bin",     0x10000, 0xd817332c, BRF_GRA },	     //  16
	{ "ld_21.bin",     0x10000, 0xb74f0c0e, BRF_GRA },	     //  17
	
	{ "ld_09.bin",     0x10000, 0xf8fd5243, BRF_GRA },	     //  18	Sprites
	{ "ld_10.bin",     0x10000, 0xb49ad746, BRF_GRA },	     //  19
	{ "ld_11.bin",     0x10000, 0x1a0d180e, BRF_GRA },	     //  20
	{ "ld_12.bin",     0x10000, 0xb2745e26, BRF_GRA },	     //  21
	{ "ld_15.bin",     0x10000, 0x96b13bbc, BRF_GRA },	     //  22
	{ "ld_16.bin",     0x10000, 0x9d80f7e6, BRF_GRA },	     //  23
	{ "ld_13.bin",     0x10000, 0xa1a598ac, BRF_GRA },	     //  24
	{ "ld_14.bin",     0x10000, 0xedf515cc, BRF_GRA },	     //  25
	
	{ "63s141.3d",     0x00100, 0x729a1ddc, BRF_GRA },  	     //  26	PROM (Priority)
};

STD_ROM_PICK(Lstduela);
STD_ROM_FN(Lstduela);

static struct BurnRomInfo LstduelbRomDesc[] = {
	{ "ld_08.bin",     0x10000, 0x43811a96, BRF_ESS | BRF_PRG }, //  0	68000 Program Code
	{ "ld_07.bin",     0x10000, 0x63c30946, BRF_ESS | BRF_PRG }, //	 1
	{ "ld_04.bin",     0x10000, 0x46a4e0f8, BRF_ESS | BRF_PRG }, //	 2
	{ "ld_03.bin",     0x10000, 0x8d5f204a, BRF_ESS | BRF_PRG }, //	 3
	{ "ldu-04.rom",    0x10000, 0x429fb964, BRF_ESS | BRF_PRG }, //	 4
	{ "ldu-03.rom",    0x10000, 0x5aa4df72, BRF_ESS | BRF_PRG }, //	 5
	
	{ "ld_02.bin",     0x10000, 0x91834d0c, BRF_ESS | BRF_PRG }, //  6	Z80 Program 
	
	{ "ld_01.bin",     0x08000, 0xad3c6f87, BRF_GRA },	     //  7	Characters
	
	{ "ld_17.bin",     0x10000, 0x7188bfdd, BRF_GRA },	     //  8	BG Tiles
	{ "ld_18.bin",     0x10000, 0xa62af66a, BRF_GRA },	     //  9
	{ "ld_19.bin",     0x10000, 0x4b762e50, BRF_GRA },	     //  10
	{ "ld_20.bin",     0x10000, 0xb140188e, BRF_GRA },	     //  11
	
	{ "ld_28.bin",     0x10000, 0x06778248, BRF_GRA },	     //  12	FG Tiles
	{ "ld_26.bin",     0x10000, 0xb0edac81, BRF_GRA },	     //  13
	{ "ld_24.bin",     0x10000, 0x66eac4df, BRF_GRA },	     //  14
	{ "ld_22.bin",     0x10000, 0xf80f8812, BRF_GRA },	     //  15
	{ "ld_27.bin",     0x10000, 0x48c78675, BRF_GRA },	     //  16
	{ "ld_25.bin",     0x10000, 0xc541ae9a, BRF_GRA },	     //  17
	{ "ld_23.bin",     0x10000, 0xd817332c, BRF_GRA },	     //  18
	{ "ld_21.bin",     0x10000, 0xb74f0c0e, BRF_GRA },	     //  19
	
	{ "ld_09.bin",     0x10000, 0xf8fd5243, BRF_GRA },	     //  20	Sprites
	{ "ld_10.bin",     0x10000, 0xb49ad746, BRF_GRA },	     //  21
	{ "ld_11.bin",     0x10000, 0x1a0d180e, BRF_GRA },	     //  22
	{ "ld_12.bin",     0x10000, 0xb2745e26, BRF_GRA },	     //  23
	{ "ld_15.bin",     0x10000, 0x96b13bbc, BRF_GRA },	     //  24
	{ "ld_16.bin",     0x10000, 0x9d80f7e6, BRF_GRA },	     //  25
	{ "ld_13.bin",     0x10000, 0xa1a598ac, BRF_GRA },	     //  26
	{ "ld_14.bin",     0x10000, 0xedf515cc, BRF_GRA },	     //  27
	
	{ "63s141.3d",     0x00100, 0x729a1ddc, BRF_GRA },  	     //  28	PROM (Priority)
};

STD_ROM_PICK(Lstduelb);
STD_ROM_FN(Lstduelb);

static int MemIndex()
{
	unsigned char *Next; Next = Mem;

	Drv68KRom              = Next; Next += 0x80000;
	DrvZ80Rom              = Next; Next += 0x10000;
	MSM6295ROM             = Next; Next += 0x40000;

	RamStart               = Next;

	Drv68KRam              = Next; Next += 0x20000;
	DrvZ80Ram              = Next; Next += 0x00800;
	DrvSpriteRam           = Next; Next += 0x00800;
	DrvSpriteRamBuffer     = Next; Next += 0x00800;
	DrvVideoRam            = Next; Next += 0x02000;
	DrvPaletteRam          = Next; Next += 0x00800;
	DrvScroll1Ram          = Next; Next += 0x04000;
	DrvScroll2Ram          = Next; Next += 0x08000;

	RamEnd                 = Next;

	DrvChars               = Next; Next += 0x0800 * 8 * 8;
	DrvBgTiles             = Next; Next += 0x0800 * 16 * 16;
	DrvFgTiles             = Next; Next += 0x1000 * 16 * 16;
	DrvSprites             = Next; Next += 0x1000 * 16 * 16;
	DrvPalette             = (unsigned int*)Next; Next += 0x00800 * sizeof(unsigned int);

	MemEnd                 = Next;

	return 0;
}

static int DrvDoReset()
{
	SekOpen(0);
	SekReset();
	SekClose();
	
	ZetOpen(0);
	ZetReset();
	ZetClose();
	
	BurnYM2203Reset();

	DrvFgScrollX = 0;
	DrvFgScrollY = 0;
	DrvBgScrollX = 0;
	DrvBgScrollY = 0;
	DrvZ80RomBank = 0;
	DrvSoundLatch = 0;
	
	return 0;
}

static int MadgearDoReset()
{
	DrvDoReset();
	
	MSM6295Reset(0);
	
	return 0;
}

unsigned char __fastcall Madgear68KReadByte(unsigned int a)
{
	switch (a) {
		case 0xfc4000: {
			return DrvDip[0];
		}
		
		case 0xfc4001: {
			return DrvDip[1];
		}
		
		case 0xfc4002: {
			return DrvDip[2];
		}
	
		case 0xfc4004: {
			return 0xff - DrvInput[0];
		}
		
		case 0xfc4005: {
			return 0xff - DrvInput[1];
		}
		
		case 0xfc4006: {
			return 0xff - DrvInput[2];
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read byte => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Madgear68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0xfc4000: {
			// flip
			return;
		}
		
		case 0xfc4002: {
			DrvSoundLatch = d & 0xff;
			return;
		}
		
		case 0xfd0000: {
			DrvFgScrollY = d & 0x1ff;
			return;
		}		
		
		case 0xfd0002: {
			DrvFgScrollX = d & 0x3ff;
			return;
		}
		
		case 0xfd0004: {
			DrvBgScrollY = d & 0x1ff;
			return;
		}
		
		case 0xfd0006: {
			DrvBgScrollX = d & 0x3ff;
			return;
		}
		
		case 0xfd0008:
		case 0xfd000e: {
			// ???
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned char __fastcall MadgearZ80Read(unsigned short a)
{
	switch (a) {
		case 0xf000: {
			return BurnYM2203Read(0, 0);
		}
		
		case 0xf002: {
			return BurnYM2203Read(1, 0);
		}
		
		case 0xf006: {
			return DrvSoundLatch;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall MadgearZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0xf000: {
			BurnYM2203Write(0, 0, d);
			return;
		}
		
		case 0xf001: {
			BurnYM2203Write(0, 1, d);
			return;
		}
		
		case 0xf002: {
			BurnYM2203Write(1, 0, d);
			return;
		}
		
		case 0xf003: {
			BurnYM2203Write(1, 1, d);
			return;
		}
		
		case 0xf004: {
			MSM6295Command(0, d);
			return;
		}
		
		case 0xf00a: {
			DrvZ80RomBank = d & 1;
			ZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom + 0x8000 + DrvZ80RomBank * 0x4000 );
			ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom + 0x8000 + DrvZ80RomBank * 0x4000 );
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Write => %04X, %02X\n"), a, d);
		}
	}
}

void __fastcall Lastduel68KWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0xfc4001: {
			// flip
			return;
		}
		
		case 0xfc4003: {
			DrvSoundLatch = d & 0xff;
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write byte => %06X, %02X\n"), a, d);
		}
	}
}

unsigned short __fastcall Lastduel68KReadWord(unsigned int a)
{
	switch (a) {
		case 0xfc4000: {
			return 0xffff - ((DrvInput[1] << 8) | DrvInput[0]);
		}
		
		case 0xfc4002: {
			return 0xffff - DrvInput[2];
		}
		
		case 0xfc4004: {
			return (DrvDip[1] << 8) | DrvDip[0];
		}
		
		case 0xfc4006: {
			return DrvDip[2];
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Read Word => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Lastduel68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0xfc0000:
		case 0xfc0002: {
			// NOP
			return;
		}
		
		case 0xfc8000: {
			DrvFgScrollY = d & 0x1ff;
			return;
		}		
		
		case 0xfc8002: {
			DrvFgScrollX = d & 0x3ff;
			return;
		}
		
		case 0xfc8004: {
			DrvBgScrollY = d & 0x1ff;
			return;
		}
		
		case 0xfc8006: {
			DrvBgScrollX = d & 0x3ff;
			return;
		}
		
		case 0xfc8008:
		case 0xfc800e: {
			// ???
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned char __fastcall LastduelZ80Read(unsigned short a)
{
	switch (a) {
		case 0xe800: {
			return BurnYM2203Read(0, 0);
		}
		
		case 0xf000: {
			return BurnYM2203Read(1, 0);
		}
		
		case 0xf800: {
			return DrvSoundLatch;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall LastduelZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0xe800: {
			BurnYM2203Write(0, 0, d);
			return;
		}
		
		case 0xe801: {
			BurnYM2203Write(0, 1, d);
			return;
		}
		
		case 0xf000: {
			BurnYM2203Write(1, 0, d);
			return;
		}
		
		case 0xf001: {
			BurnYM2203Write(1, 1, d);
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Write => %04X, %02X\n"), a, d);
		}
	}
}

inline static void DrvYM2203IRQHandler(int, int nStatus)
{
	if (nStatus & 1) {
		ZetSetIRQLine(0xff, ZET_IRQSTATUS_ACK);
	} else {
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	}
}

inline static int DrvSynchroniseStream(int nSoundRate)
{
	return (long long)ZetTotalCycles() * nSoundRate / 3579545;
}

inline static double DrvGetTime()
{
	return (double)ZetTotalCycles() / 3579545;
}

static int CharPlaneOffsets[2]     = { 4, 0 };
static int CharXOffsets[8]         = { 0, 1, 2, 3, 8, 9, 10, 11 };
static int CharYOffsets[8]         = { 0, 16, 32, 48, 64, 80, 96, 112 };
static int BgTilePlaneOffsets[4]   = { 12, 8, 4, 0 };
static int FgTilePlaneOffsets[4]   = { 4, 12, 0, 8 };
static int TileXOffsets[16]        = { 0, 1, 2, 3, 16, 17, 18, 19, 512, 513, 514, 515, 528, 529, 530, 531 };
static int TileYOffsets[16]        = { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480 };
static int SpritePlaneOffsets[4]   = { 0, 0x100000, 0x200000, 0x300000 };
static int SpriteXOffsets[16]      = { 0, 1, 2, 3, 4, 5, 6, 7, 128, 129, 130, 131, 132, 133, 134, 135 };
static int SpriteYOffsets[16]      = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };
static int LdFgTilePlaneOffsets[4] = { 4, 0, 0x200004, 0x200000 };
static int LdBgTilePlaneOffsets[4] = { 4, 0, 0x100004, 0x100000 };
static int LdTileXOffsets[16]      = { 0, 1, 2, 3, 8, 9, 10, 11, 256, 257, 258, 259, 264, 265, 266, 267 };
static int LdTileYOffsets[16]      = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };

static int DrvInit()
{
	int nRet = 0, nLen;

	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	DrvTempRom = (unsigned char *)malloc(0x80000);

	// Load 68000 Program Roms
	nRet = BurnLoadRom(Drv68KRom + 0x00001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Drv68KRom + 0x00000, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Drv68KRom + 0x40001, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Drv68KRom + 0x40000, 3, 2); if (nRet != 0) return 1;
	
	// Load Z80 Program Rom
	nRet = BurnLoadRom(DrvZ80Rom, 4, 1); if (nRet != 0) return 1;
		
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 5, 1); if (nRet != 0) return 1;
	GfxDecode(0x0800, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	
	// Load and decode the bg tiles
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom, 6, 1); if (nRet != 0) return 1;
	GfxDecode(0x0800, 4, 16, 16, BgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x400, DrvTempRom, DrvBgTiles);
	
	// Load and decode the fg tiles
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom, 7, 1); if (nRet != 0) return 1;
	GfxDecode(0x1000, 4, 16, 16, FgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x400, DrvTempRom, DrvFgTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 10, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x40000, 12, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x50000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x60000, 14, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x70000, 15, 1); if (nRet != 0) return 1;
	GfxDecode(0x1000, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x100, DrvTempRom, DrvSprites);
	
	// Load the samples
	nRet = BurnLoadRom(MSM6295ROM + 0x00000, 16, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(MSM6295ROM + 0x20000, 17, 1); if (nRet != 0) return 1;
	
	free(DrvTempRom);
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Drv68KRom           , 0x000000, 0x07ffff, SM_ROM);
	SekMapMemory(DrvSpriteRam        , 0xfc1800, 0xfc1fff, SM_RAM);
	SekMapMemory(DrvVideoRam         , 0xfc8000, 0xfc9fff, SM_RAM);
	SekMapMemory(DrvPaletteRam       , 0xfcc000, 0xfcc7ff, SM_RAM);
	SekMapMemory(DrvScroll1Ram       , 0xfd4000, 0xfd7fff, SM_RAM);
	SekMapMemory(DrvScroll2Ram       , 0xfd8000, 0xfdffff, SM_RAM);
	SekMapMemory(Drv68KRam           , 0xff0000, 0xffffff, SM_RAM);
	SekSetWriteWordHandler(0, Madgear68KWriteWord);
	SekSetReadByteHandler(0, Madgear68KReadByte);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(MadgearZ80Read);
	ZetSetWriteHandler(MadgearZ80Write);
	ZetMapArea(0x0000, 0x7fff, 0, DrvZ80Rom              );
	ZetMapArea(0x0000, 0x7fff, 2, DrvZ80Rom              );
	ZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom + 0x8000     );
	ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom + 0x8000     );
	ZetMapArea(0xd000, 0xd7ff, 0, DrvZ80Ram              );
	ZetMapArea(0xd000, 0xd7ff, 1, DrvZ80Ram              );
	ZetMapArea(0xd000, 0xd7ff, 2, DrvZ80Ram              );
	ZetMemEnd();
	ZetClose();
	
	GenericTilesInit();
	DrvSpritePriMask = 0x10;
	DrvSpriteFlipYMask = 0x80;
	
	BurnYM2203Init(2, 3579545, &DrvYM2203IRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
	BurnTimerAttachZet(3579545);
	
	MSM6295Init(0, 7575, 98, 1);

	// Reset the driver
	MadgearDoReset();

	return 0;
}

static int LastduelInit()
{
	int nRet = 0, nLen, nRomOffset = 0, Lstduelb = 0;
	
	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "lstduelb")) Lstduelb = 1;
	if (Lstduelb) nRomOffset = 2;

	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	DrvTempRom = (unsigned char *)malloc(0x80000);

	// Load 68000 Program Roms
	if (!Lstduelb) {
		nRet = BurnLoadRom(Drv68KRom + 0x00001, 0, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x00000, 1, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x40001, 2, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x40000, 3, 2); if (nRet != 0) return 1;
	} else {
		nRet = BurnLoadRom(Drv68KRom + 0x00001, 0, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x00000, 1, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x20001, 2, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x20000, 3, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x40001, 4, 2); if (nRet != 0) return 1;
		nRet = BurnLoadRom(Drv68KRom + 0x40000, 5, 2); if (nRet != 0) return 1;
	}
	
	// Load Z80 Program Rom
	nRet = BurnLoadRom(DrvZ80Rom, 4 + nRomOffset, 1); if (nRet != 0) return 1;
		
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 5 + nRomOffset, 1); if (nRet != 0) return 1;
	GfxDecode(0x0800, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	
	// Load and decode the bg tiles
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 6 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 7 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 8 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 9 + nRomOffset, 1); if (nRet != 0) return 1;
	GfxDecode(0x0800, 4, 16, 16, LdBgTilePlaneOffsets, LdTileXOffsets, LdTileYOffsets, 0x200, DrvTempRom, DrvBgTiles);
	
	// Load and decode the fg tiles
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 10 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 11 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 12 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 13 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x40000, 14 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x50000, 15 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x60000, 16 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x70000, 17 + nRomOffset, 1); if (nRet != 0) return 1;
	GfxDecode(0x1000, 4, 16, 16, LdFgTilePlaneOffsets, LdTileXOffsets, LdTileYOffsets, 0x200, DrvTempRom, DrvFgTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 18 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 19 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 20 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 21 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x40000, 22 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x50000, 23 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x60000, 24 + nRomOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x70000, 25 + nRomOffset, 1); if (nRet != 0) return 1;
	GfxDecode(0x1000, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x100, DrvTempRom, DrvSprites);
	
	free(DrvTempRom);
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Drv68KRom           , 0x000000, 0x05ffff, SM_ROM);
	SekMapMemory(DrvSpriteRam        , 0xfc0800, 0xfc0fff, SM_RAM);
	SekMapMemory(DrvVideoRam         , 0xfcc000, 0xfcdfff, SM_RAM);
	SekMapMemory(DrvScroll1Ram       , 0xfd0000, 0xfd3fff, SM_RAM);
	SekMapMemory(DrvScroll2Ram       , 0xfd4000, 0xfd7fff, SM_RAM);
	SekMapMemory(DrvPaletteRam       , 0xfd8000, 0xfd87ff, SM_RAM);
	SekMapMemory(Drv68KRam           , 0xfe0000, 0xffffff, SM_RAM);
	SekSetReadWordHandler(0, Lastduel68KReadWord);
	SekSetWriteWordHandler(0, Lastduel68KWriteWord);
	SekSetWriteByteHandler(0, Lastduel68KWriteByte);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(LastduelZ80Read);
	ZetSetWriteHandler(LastduelZ80Write);
	ZetMapArea(0x0000, 0xdfff, 0, DrvZ80Rom              );
	ZetMapArea(0x0000, 0xdfff, 2, DrvZ80Rom              );
	ZetMapArea(0xe000, 0xe7ff, 0, DrvZ80Ram              );
	ZetMapArea(0xe000, 0xe7ff, 1, DrvZ80Ram              );
	ZetMapArea(0xe000, 0xe7ff, 2, DrvZ80Ram              );
	ZetMemEnd();
	ZetClose();
	
	GenericTilesInit();
	DrvSpritePriMask = 0x00;
	DrvSpriteFlipYMask = 0x40;
	
	BurnYM2203Init(2, 3579545, &DrvYM2203IRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
	BurnTimerAttachZet(3579545);
	
	// Reset the driver
	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	SekExit();
	ZetExit();
	
	BurnYM2203Exit();
	MSM6295Exit(0);
	
	GenericTilesExit();
	
	DrvFgScrollX = 0;
	DrvFgScrollY = 0;
	DrvBgScrollX = 0;
	DrvBgScrollY = 0;
	DrvSpritePriMask = 0;
	DrvSpriteFlipYMask = 0;
	DrvZ80RomBank = 0;
	DrvSoundLatch = 0;
	
	free(Mem);
	Mem = NULL;

	return 0;
}

static int MadgearExit()
{
	MSM6295Exit(0);
	DrvExit();

	return 0;
}

inline static unsigned int CalcCol(unsigned short nColour)
{
	static const unsigned char ztable[16] = { 0x0, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11 };
	int i, r, g, b;

	i = ztable[(nColour >> 0) & 15];
	r = ((nColour >> 12) & 15) * i;
	g = ((nColour >>  8) & 15) * i;
	b = ((nColour >>  4) & 15) * i;
	
	return BurnHighCol(r, g, b, 0);
}

static void DrvCalcPalette()
{
	int i;
	unsigned short* ps;
	unsigned int* pd;

	for (i = 0, ps = (unsigned short*)DrvPaletteRam, pd = DrvPalette; i < 0x800; i++, ps++, pd++) {
		*pd = CalcCol(*ps);
	}
}

static void DrvRenderBgLayer()
{
	int mx, my, Code, Colour, x, y, TileIndex, Flip, xFlip, yFlip;
	
	UINT16 *VideoRam = (UINT16*)DrvScroll2Ram;
	
	for (mx = 0; mx < 32; mx++) {
		for (my = 0; my < 64; my++) {
			TileIndex = (my * 32) + mx;
			Code = VideoRam[TileIndex] & 0x1fff;
			Colour = VideoRam[TileIndex + 0x800];
			Flip = (Colour & 0x60) >> 5;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			Colour &= 0x0f;
			
			y = 16 * mx;
			x = 16 * my;
			
			x -= DrvBgScrollX;
			y -= DrvBgScrollY;
			if (x < -16) x += 1024;
			if (y < -16) y += 512;
			x -= 64;
			y -= 8;

			if (x > 16 && x < 368 && y > 16 && y < 224) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_FlipXY(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile_FlipX(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_FlipY(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile_FlipX_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_FlipY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				}
			}
		}
	}
}

static void DrvRenderFgLayer(int Priority)
{
	int mx, my, Code, Colour, x, y, TileIndex, Split, Flip, xFlip, yFlip;
	
	UINT16 *VideoRam = (UINT16*)DrvScroll1Ram;
	
	for (mx = 0; mx < 32; mx++) {
		for (my = 0; my < 64; my++) {
			TileIndex = (my * 32) + mx;
			Code = VideoRam[TileIndex] & 0x1fff;
			Colour = VideoRam[TileIndex + 0x800];
			Flip = (Colour & 0x60) >> 5;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			Split = (Colour & 0x10) >> 4;
			Colour &= 0x0f;
			
			if (Split != Priority) continue;
			
			y = 16 * mx;
			x = 16 * my;
			
			x -= DrvFgScrollX;
			y -= DrvFgScrollY;
			if (x < -16) x += 1024;
			if (y < -16) y += 512;
			x -= 64;
			y -= 8;

			if (x > 16 && x < 368 && y > 16 && y < 224) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 4, 0x0f, 0x100, DrvFgTiles);
					}
				}
			}
		}
	}
}

static void LastduelRenderBgLayer()
{
	int mx, my, Code, Colour, x, y, TileIndex = 0, Flip, xFlip, yFlip;
	
	UINT16 *VideoRam = (UINT16*)DrvScroll2Ram;
	
	for (my = 0; my < 64; my++) {
		for (mx = 0; mx < 64; mx++) {
			Code = VideoRam[2 * TileIndex] & 0x1fff;
			Colour = VideoRam[2 * TileIndex + 1];
			Flip = (Colour & 0x60) >> 5;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			Colour &= 0x0f;
			
			x = 16 * mx;
			y = 16 * my;
			
			x -= DrvBgScrollX;
			y -= DrvBgScrollY;
			if (x < -16) x += 1024;
			if (y < -16) y += 1024;
			x -= 64;
			y -= 8;

			if (x > 16 && x < 368 && y > 16 && y < 224) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_FlipXY(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile_FlipX(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_FlipY(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile_FlipX_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_FlipY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					} else {
						Render16x16Tile_Clip(pTransDraw, Code, x, y, Colour, 4, 0, DrvBgTiles);
					}
				}
			}
			
			TileIndex++;
		}
	}
}

static void LastduelRenderFgLayer(int Priority)
{
	int mx, my, Code, Colour, x, y, TileIndex = 0, Split, Flip, xFlip, yFlip;
	
	UINT16 *VideoRam = (UINT16*)DrvScroll1Ram;
	
	for (my = 0; my < 64; my++) {
		for (mx = 0; mx < 64; mx++) {
			Code = VideoRam[2 * TileIndex] & 0x1fff;
			Colour = VideoRam[2 * TileIndex + 1];
			Flip = (Colour & 0x60) >> 5;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			Split = (Colour & 0x80) >> 7;
			Colour &= 0x0f;
			
			if (Split != Priority) continue;
			
			x = 16 * mx;
			y = 16 * my;
			
			x -= DrvFgScrollX;
			y -= DrvFgScrollY;
			if (x < -16) x += 1024;
			if (y < -16) y += 1024;
			x -= 64;
			y -= 8;

			if (x > 16 && x < 368 && y > 16 && y < 224) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0x100, DrvFgTiles);
					}
				}
			}
			
			TileIndex++;
		}
	}
}

static void DrvRenderSprites(int Priority)
{
	int Offset;
	UINT16 *SpriteRam = (UINT16*)DrvSpriteRamBuffer;
	
	for (Offset = 0x400 - 4; Offset >= 0; Offset -= 4) {
		int Attr, sy, sx, xFlip, yFlip, Code, Colour;
		
		Attr = SpriteRam[Offset + 1];
		if (DrvSpritePriMask) {
			if (Priority == 1 && (Attr & DrvSpritePriMask)) continue;
			if (Priority == 0 && !(Attr & DrvSpritePriMask)) continue;
		}
		
		Code = SpriteRam[Offset] & 0xfff;
		sx = SpriteRam[Offset + 3] & 0x1ff;
		sy = SpriteRam[Offset + 2] & 0x1ff;
		if (sy > 0x100) sy -= 0x200;
		
		xFlip = Attr & 0x20;
		yFlip = Attr & DrvSpriteFlipYMask;
		Colour = Attr & 0x0f;
		
		sx -= 64;
		sy -= 8;
		
		if (sx > 16 && sx < 368 && sy > 16 && sy < 224) {
			if (xFlip) {
				if (yFlip) {
					Render16x16Tile_Mask_FlipXY(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				} else {
					Render16x16Tile_Mask_FlipX(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				}
			} else {
				if (yFlip) {
					Render16x16Tile_Mask_FlipY(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				} else {
					Render16x16Tile_Mask(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				}
			}
		} else {
			if (xFlip) {
				if (yFlip) {
					Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				} else {
					Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				}
			} else {
				if (yFlip) {
					Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				} else {
					Render16x16Tile_Mask_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0x0f, 0x200, DrvSprites);
				}
			}
		}
	}
}

static void DrvRenderCharLayer()
{
	int mx, my, Code, Colour, x, y, yFlip, TileIndex = 0;
	
	UINT16 *VideoRam = (UINT16*)DrvVideoRam;

	for (my = 0; my < 32; my++) {
		for (mx = 0; mx < 64; mx++) {
			yFlip = 0;
			Code = VideoRam[TileIndex];
			Colour = Code >> 12;
			if (Code & 0x800) yFlip = 1;
			Code &= 0x7ff;
			
			x = 8 * mx;
			y = 8 * my;
			
			x -= 64;
			y -= 8;
			
			if (x > 0 && x < 376 && y > 8 && y < 232) {
				if (yFlip) {
					Render8x8Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 2, 3, 0x300, DrvChars);
				} else {
					Render8x8Tile_Mask(pTransDraw, Code, x, y, Colour, 2, 3, 0x300, DrvChars);
				}
			} else {
				if (yFlip) {
					Render8x8Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 2, 3, 0x300, DrvChars);
				} else {
					Render8x8Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 2, 3, 0x300, DrvChars);
				}
			}

			TileIndex++;
		}
	}
}

static void DrvDraw()
{
	BurnTransferClear();
	DrvCalcPalette();
	DrvRenderBgLayer();
	DrvRenderFgLayer(0);
	DrvRenderSprites(0);
	DrvRenderFgLayer(1);
	DrvRenderSprites(1);
	DrvRenderCharLayer();
	BurnTransferCopy(DrvPalette);
}

static void LastduelDraw()
{
	BurnTransferClear();
	DrvCalcPalette();
	LastduelRenderBgLayer();
	LastduelRenderFgLayer(0);
	DrvRenderSprites(0);
	LastduelRenderFgLayer(1);
	DrvRenderCharLayer();
	BurnTransferCopy(DrvPalette);
}

static int DrvFrame()
{
	int nInterleave = 10;

	if (DrvReset) MadgearDoReset();

	DrvMakeInputs();

	nCyclesTotal[0] = 10000000 / 60;
	nCyclesTotal[1] = 3579545 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	SekNewFrame();
	ZetNewFrame();
	
	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run 68000
		nCurrentCPU = 0;
		SekOpen(0);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		if (i == 3 || i == 6) SekSetIRQLine(6, SEK_IRQSTATUS_AUTO);
		SekClose();
	}
	
	ZetOpen(0);
	BurnTimerEndFrame(nCyclesTotal[1]);
	BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
	ZetClose();
	MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	
	if (pBurnDraw) DrvDraw();
	
	SekOpen(0);
	SekSetIRQLine(5, SEK_IRQSTATUS_AUTO);
	SekClose();
	
	memcpy(DrvSpriteRamBuffer, DrvSpriteRam, 0x800);

	return 0;
}

static int LastduelFrame()
{
	int nInterleave = 10;

	if (DrvReset) DrvDoReset();

	DrvMakeInputs();

	nCyclesTotal[0] = 10000000 / 60;
	nCyclesTotal[1] = 3579545 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	SekNewFrame();
	ZetNewFrame();
	
	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run 68000
		nCurrentCPU = 0;
		SekOpen(0);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		if (i == 3 || i == 6) SekSetIRQLine(4, SEK_IRQSTATUS_AUTO);
		SekClose();
	}
		
	ZetOpen(0);
	BurnTimerEndFrame(nCyclesTotal[1] - nCyclesDone[1]);
	BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
	ZetClose();
	
	if (pBurnDraw) LastduelDraw();
	
	SekOpen(0);
	SekSetIRQLine(2, SEK_IRQSTATUS_AUTO);
	SekClose();
	
	memcpy(DrvSpriteRamBuffer, DrvSpriteRam, 0x800);

	return 0;
}

static int DrvScan(int nAction, int *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {			// Return minimum compatible version
		*pnMin = 0x029672;
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
		ZetScan(nAction);			// Scan Z80
		BurnYM2203Scan(nAction, pnMin);

		// Scan critical driver variables
		SCAN_VAR(nCyclesDone);
		SCAN_VAR(nCyclesSegment);
		SCAN_VAR(DrvZ80RomBank);
		SCAN_VAR(DrvSoundLatch);
		SCAN_VAR(DrvFgScrollX);
		SCAN_VAR(DrvFgScrollY);
		SCAN_VAR(DrvBgScrollX);
		SCAN_VAR(DrvBgScrollY);
		SCAN_VAR(DrvDip);
		SCAN_VAR(DrvInput);
	}

	return 0;
}

static int MadgearScan(int nAction, int *pnMin)
{
	int i = DrvScan(nAction, pnMin);
	
	if (i == 0) {
		if (nAction & ACB_DRIVER_DATA) {
			MSM6295Scan(0, nAction);
		}
	
		if (nAction & ACB_WRITE) {
			ZetOpen(0);
			ZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom + 0x8000 + DrvZ80RomBank * 0x4000 );
			ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom + 0x8000 + DrvZ80RomBank * 0x4000 );
			ZetClose();
		}
	}
	
	return i;
}

struct BurnDriver BurnDrvLastduel = {
	"lastduel", NULL, NULL, "1988",
	"Last Duel (US set 1)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, LastduelRomInfo, LastduelRomName, LastduelInputInfo, LastduelDIPInfo,
	LastduelInit, DrvExit, LastduelFrame, NULL, DrvScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};

struct BurnDriver BurnDrvLstduela = {
	"lstduela", "lastduel", NULL, "1988",
	"Last Duel (US set 2)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, LstduelaRomInfo, LstduelaRomName, LastduelInputInfo, LastduelDIPInfo,
	LastduelInit, DrvExit, LastduelFrame, NULL, DrvScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};

struct BurnDriver BurnDrvLstduelb = {
	"lstduelb", "lastduel", NULL, "1988",
	"Last Duel (bootleg)\0", NULL, "bootleg", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_BOOTLEG, 2, HARDWARE_MISC_PRE90S,
	NULL, LstduelbRomInfo, LstduelbRomName, LastduelInputInfo, LastduelDIPInfo,
	LastduelInit, DrvExit, LastduelFrame, NULL, DrvScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};

struct BurnDriver BurnDrvMadgear = {
	"madgear", NULL, NULL, "1989",
	"Mad Gear (US)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, DrvRomInfo, DrvRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, MadgearExit, DrvFrame, NULL, MadgearScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};

struct BurnDriver BurnDrvMadgearj = {
	"madgearj", "madgear", NULL, "1989",
	"Mad Gear (Japan)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, DrvjRomInfo, DrvjRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, MadgearExit, DrvFrame, NULL, MadgearScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};

struct BurnDriver BurnDrvMadgearu = {
	"ledstorm", "madgear", NULL, "1988",
	"Led Storm (US)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, DrvuRomInfo, DrvuRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, MadgearExit, DrvFrame, NULL, MadgearScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};

struct BurnDriver BurnDrvLedstrm2 = {
	"ledstrm2", "madgear", NULL, "1988",
	"Led Storm Rally 2011 (US)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S,
	NULL, Ledstrm2RomInfo, Ledstrm2RomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, MadgearExit, DrvFrame, NULL, MadgearScan,
	0, NULL, NULL, NULL, NULL, 240, 384, 3, 4
};
