
#include "burnint.h"
#include "driver.h"
extern "C" {
#include "ay8910.h"
}


static unsigned char *Mem, *Rom, *Gfx, *Mcu, *Prom;
static unsigned char DrvJoy1[8], DrvJoy2[8], DrvDips[1], DrvService, DrvReset, DrvTilt;
static unsigned short DrvAxis[2];
static unsigned int nAnalogAxis[2] = {0,0};
static short *pAY8910Buffer[3], *pFMBuffer = NULL;
static int *Palette;

static int arkanoid_bootleg_id, tetrsark = 0;
static unsigned char arkanoid_bootleg_cmd;
static unsigned char palettebank, gfxbank;
static unsigned char arkanoid_paddle_select, arkanoid_paddle_value;

enum {
	ARKUNK=0,  // unknown bootlegs
	ARKANGC,
	ARKANGC2,
	ARKBLOCK,
	ARKBLOC2,
	ARKGCBL,
	PADDLE2
};


//--------------------------------------------------------------------------------


#define A(a, b, c, d) { a, b, (unsigned char*)(c), d }

static struct BurnInputInfo DrvInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 0,	"p1 coin"  }, // 0
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p1 start" }, // 1
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 2,	"p1 fire 1"}, // 2

	A("P1 Right / left",	BIT_ANALOG_REL, DrvAxis + 0,	"mouse x-axis"),

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy2 + 0,	"p2 coin"  }, // 4
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy2 + 1,	"p2 start" }, // 5
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 2,	"p2 fire 1"}, // 6

	A("P2 Right / left",	BIT_ANALOG_REL, DrvAxis + 1,	"mouse x-axis"),

	{"Tilt",	  BIT_DIGITAL,   &DrvTilt,	"tilt"     }, // 8
	{"Service",	  BIT_DIGITAL,   &DrvService,   "diag"     }, // 9
	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    }, // a
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   }, // b
};

STDINPUTINFO(Drv);

static struct BurnInputInfo tetrsarkInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 0,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 1,	"p1 start" },
	{"P1 down",	  BIT_DIGITAL,   DrvJoy1 + 2,   "p1 down"  },
	{"P1 left",	  BIT_DIGITAL,	 DrvJoy1 + 3,   "p1 left" },
	{"P1 right",	  BIT_DIGITAL,   DrvJoy1 + 4,   "p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 6,	"p2 fire 2"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy2 + 0,	"p2 coin"  },
	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy2 + 1,	"p2 start" },
	{"P2 down",	  BIT_DIGITAL,   DrvJoy2 + 2,   "p2 down"  },
	{"P2 left",	  BIT_DIGITAL,	 DrvJoy2 + 3,   "p2 left" },
	{"P2 right",	  BIT_DIGITAL,   DrvJoy2 + 4,   "p2 right" },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 5,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy2 + 6,	"p2 fire 2"},

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
};

STDINPUTINFO(tetrsark);

static struct BurnDIPInfo arkanoidDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x7f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x0b, 0x01, 0x01, 0x01, "No"       		  },
	{0x0b, 0x01, 0x01, 0x00, "Yes"       		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "20K 60K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "20K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x20, "3"			  },
	{0x0b, 0x01, 0x20, 0x00, "5"    		  },

	{0   , 0xfe, 0   , 4   , "Coinage" 	          },
	{0x0b, 0x01, 0xc0, 0x40, "2C 1C"		  },
	{0x0b, 0x01, 0xc0, 0xc0, "1C 1C"    		  },
	{0x0b, 0x01, 0xc0, 0x80, "1C 2C"		  },
	{0x0b, 0x01, 0xc0, 0x00, "1C 6C"    		  },
};

STDDIPINFO(arkanoid);

static struct BurnDIPInfo arknoidjDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x7f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x0b, 0x01, 0x01, 0x01, "No"       		  },
	{0x0b, 0x01, 0x01, 0x00, "Yes"       		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "20K 60K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "20K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x20, "3"			  },
	{0x0b, 0x01, 0x20, 0x00, "5"    		  },

	{0   , 0xfe, 0   , 2   , "Coinage" 	          },
	{0x0b, 0x01, 0x40, 0x40, "1C 1C"		  },
	{0x0b, 0x01, 0x40, 0x00, "1C 2C"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(arknoidj);

static struct BurnDIPInfo ark1ballDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x7f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x0b, 0x01, 0x01, 0x01, "No"       		  },
	{0x0b, 0x01, 0x01, 0x00, "Yes"       		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "60K 100K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "60K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x20, "1"			  },
	{0x0b, 0x01, 0x20, 0x00, "2"    		  },

	{0   , 0xfe, 0   , 2   , "Coinage" 	          },
	{0x0b, 0x01, 0x40, 0x40, "1C 1C"		  },
	{0x0b, 0x01, 0x40, 0x00, "1C 2C"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(ark1ball);

static struct BurnDIPInfo arkangcDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x7f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Ball Speed"             },
	{0x0b, 0x01, 0x01, 0x01, "Normal"       	  },
	{0x0b, 0x01, 0x01, 0x00, "Faster"      		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "20K 60K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "20K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x20, "3"			  },
	{0x0b, 0x01, 0x20, 0x00, "5"    		  },

	{0   , 0xfe, 0   , 2   , "Coinage" 	          },
	{0x0b, 0x01, 0x40, 0x40, "1C 1C"		  },
	{0x0b, 0x01, 0x40, 0x00, "1C 2C"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(arkangc);

static struct BurnDIPInfo arkangc2DIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x7f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Ball Speed"             },
	{0x0b, 0x01, 0x01, 0x01, "Slower"       	  },
	{0x0b, 0x01, 0x01, 0x00, "Normal"      		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "20K 60K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "20K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x20, "3"			  },
	{0x0b, 0x01, 0x20, 0x00, "5"    		  },

	{0   , 0xfe, 0   , 2   , "Coinage" 	          },
	{0x0b, 0x01, 0x40, 0x40, "1C 1C"		  },
	{0x0b, 0x01, 0x40, 0x00, "1C 2C"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(arkangc2);

static struct BurnDIPInfo arkgcblDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x5f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Round Select"           },
	{0x0b, 0x01, 0x01, 0x01, "Off"       		  },
	{0x0b, 0x01, 0x01, 0x00, "On"       		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "60K 100K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "60K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x00, "2"			  },
	{0x0b, 0x01, 0x20, 0x20, "3"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(arkgcbl);

static struct BurnDIPInfo paddle2DIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x5f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Round Select"           },
	{0x0b, 0x01, 0x01, 0x01, "Off"       		  },
	{0x0b, 0x01, 0x01, 0x00, "On"       		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Controls ?"             },
	{0x0b, 0x01, 0x04, 0x04, "Normal"		  },
	{0x0b, 0x01, 0x04, 0x00, "Alternate"		  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "60K 60K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "60K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x00, "2"			  },
	{0x0b, 0x01, 0x20, 0x20, "3"    		  },

	{0   , 0xfe, 0   , 2   , "Coinage" 	          },
	{0x0b, 0x01, 0x40, 0x40, "1C 1C"		  },
	{0x0b, 0x01, 0x40, 0x00, "1C 2C"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(paddle2);

static struct BurnDIPInfo arktayt2DIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x3f, NULL                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x0b, 0x01, 0x01, 0x01, "No"       		  },
	{0x0b, 0x01, 0x01, 0x00, "Yes"       		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x0b, 0x01, 0x02, 0x02, "Off"			  },
	{0x0b, 0x01, 0x02, 0x00, "On"			  },

	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x0b, 0x01, 0x08, 0x08, "Easy"     		  },
	{0x0b, 0x01, 0x08, 0x00, "Hard"		          },

	{0   , 0xfe, 0   , 2   , "Bonus Life"             },
	{0x0b, 0x01, 0x10, 0x10, "60K 60K 60K+" 	  },
	{0x0b, 0x01, 0x10, 0x00, "60K"    		  },

	{0   , 0xfe, 0   , 2   , "Lives" 	          },
	{0x0b, 0x01, 0x20, 0x20, "2"			  },
	{0x0b, 0x01, 0x20, 0x00, "3"    		  },

	{0   , 0xfe, 0   , 2   , "Coinage" 	          },
	{0x0b, 0x01, 0x40, 0x00, "2C 1C"		  },
	{0x0b, 0x01, 0x40, 0x40, "1C 1C"    		  },

	{0   , 0xfe, 0   , 2   , "Cabinet" 	          },
	{0x0b, 0x01, 0x80, 0x00, "Upright"		  },
	{0x0b, 0x01, 0x80, 0x80, "Cocktail"    		  },
};

STDDIPINFO(arktayt2);

static struct BurnDIPInfo tetrsarkDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xf0, NULL                     },

	{0   , 0xfe, 0   , 2   , "Cabinet"	          },
	{0x0f, 0x01, 0x10, 0x10, "Upright"       	  },
	{0x0f, 0x01, 0x10, 0x00, "Cocktail"    		  },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0f, 0x01, 0x20, 0x20, "Off"			  },
	{0x0f, 0x01, 0x20, 0x00, "On"			  },

	{0   , 0xfe, 0   , 4   , "Coinage" 	          },
	{0x0f, 0x01, 0xc0, 0xc0, "1C 1C"		  },
	{0x0f, 0x01, 0xc0, 0x80, "1C 2C"    		  },
	{0x0f, 0x01, 0xc0, 0x40, "1C 3C"		  },
	{0x0f, 0x01, 0xc0, 0x00, "1C 5C"    		  },
};

STDDIPINFO(tetrsark);


//--------------------------------------------------------------------------------


static unsigned char read_inputs(unsigned char port)
{
	unsigned char ret = 0;

	switch (port)
	{
		case 0x00:
			ret |= DrvJoy1[1];
			ret |= DrvJoy2[1] << 1;
			ret |= DrvService << 2;
			ret |= DrvTilt << 3;
			ret |= DrvJoy1[0] << 4;
			ret |= DrvJoy2[0] << 5;
			ret ^= 0x4f;
		break;

		case 0x01:
			ret |= DrvJoy1[2];
			ret |= DrvJoy2[2] << 2;
			ret ^= 0xff;
		break;

		case 0x02: // spinner
			nAnalogAxis[0] -= DrvAxis[0];
			ret = (~nAnalogAxis[0] >> 8) & 0xfe;
			arkanoid_paddle_value = ret;
		break;

		case 0x03: // spinner
			nAnalogAxis[1] -= DrvAxis[1];
			ret = (~nAnalogAxis[1] >> 8) & 0xfe;
		break;

		case 0x04: // dips
			ret = DrvDips[0];

			if (tetrsark)
			{
				ret |= DrvJoy2[2];
				ret |= DrvJoy2[3] << 1;
				ret |= DrvJoy2[4] << 2;
				ret |= DrvJoy2[5] << 3;
				ret ^= 0x0f;
			}
		break;

		case 0x05:
			ret |= DrvJoy1[2];
			ret |= DrvJoy1[3] << 1;
			ret |= DrvJoy1[4] << 2;
			ret |= DrvJoy1[5] << 3;
			ret |= DrvJoy1[1] << 4;
			ret |= DrvJoy2[1] << 5;
			ret |= DrvJoy1[6] << 6;
			ret |= DrvJoy2[6] << 7;
			ret ^= 0xff;
		break;
	}

	return ret;
}


//--------------------------------------------------------------------------------
// AY8910 Read ports

static unsigned char ay8910_read_port_4(unsigned int)
{
	return read_inputs(4);
}


static unsigned char ay8910_read_port_5(unsigned int)
{
	return read_inputs(5);
}


//--------------------------------------------------------------------------------
// Read / Write handlers


// Kludge for some bootlegs that read this address
unsigned char __fastcall arkanoid_bootleg_f002_r()
{
	unsigned char arkanoid_bootleg_val = 0x00;

	switch (arkanoid_bootleg_id)
	{
		case ARKANGC:
		case ARKBLOCK:
		case ARKBLOC2:
		case ARKANGC2:
			switch (arkanoid_bootleg_cmd)
			{
				default:
					break;
			}
			break;

		case ARKGCBL:
			switch (arkanoid_bootleg_cmd)
			{
				case 0x8a:  // Current level (fixed routine)
					arkanoid_bootleg_val = 0xa5;
					break;
				case 0xff:  // Avoid "BAD HARDWARE    " message (fixed routine)
					arkanoid_bootleg_val = 0xe2;
					break;
				default:
					break;
			}
			break;

		case PADDLE2:
			switch (arkanoid_bootleg_cmd)
			{
				case 0x24:  // Avoid bad jump to 0x0066
					arkanoid_bootleg_val = 0x9b;
					break;
				case 0x36:  // Avoid "BAD HARDWARE    " message
					arkanoid_bootleg_val = 0x2d;
					break;
				case 0x38:  // Start of levels table (fixed offset)
					arkanoid_bootleg_val = 0xf3;
					break;
				case 0x8a:  // Current level (fixed routine)
					arkanoid_bootleg_val = 0xa5;
					break;
				case 0xc3:  // Avoid bad jump to 0xf000
					arkanoid_bootleg_val = 0x1d;
					break;
				case 0xe3:  // Number of bricks left (fixed offset)
					arkanoid_bootleg_val = 0x61;
					break;
				case 0xf7:  // Avoid "U69" message
					arkanoid_bootleg_val = 0x00;
					break;
				case 0xff:  // Avoid "BAD HARDWARE    " message (fixed routine)
					arkanoid_bootleg_val = 0xe2;
					break;
				default:
					break;
			}
			break;

		default:
			break;
	}

	return arkanoid_bootleg_val;
}


// Kludge for some bootlegs that write this address
void __fastcall arkanoid_bootleg_d018_w(unsigned short, unsigned char data)
{
	arkanoid_bootleg_cmd = 0x00;

	switch (arkanoid_bootleg_id)
	{
		case ARKANGC:
		case ARKBLOCK:
			switch (data)
			{
				case 0x36:  // unneeded value : no call 0x2050, unused A and overwritten HL (0x0313 -> 0x0340)
					if (ZetPc(-1) == 0x7c47)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x38:  // unneeded value : no call 0x2050, unused A and fixed HL (0x7bd5)
					if (ZetPc(-1) == 0x7b87)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x8a:  // unneeded value : no call 0x2050, unused A and overwritten HL (0x7b77 -> 0x7c1c)
					if (ZetPc(-1) == 0x9661)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xe3:  // unneeded value : call 0x2050 but fixed A (0x00) and fixed HL (0xed83)
					if (ZetPc(-1) == 0x67e3)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xf7:  // unneeded value : 3 * 'NOP' at 0x034f + 2 * 'NOP' at 0x35b
					if (ZetPc(-1) == 0x0349)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xff:  // unneeded value : no call 0x2050, unused A and overwritten HL (0x7c4f -> 0x7d31)
					if (ZetPc(-1) == 0x9670)
						arkanoid_bootleg_cmd = 0x00;
					break;
				default:
					arkanoid_bootleg_cmd = 0x00;
					break;
			}
			break;

		case ARKANGC2:
			switch (data)
			{
				case 0x36:  // unneeded value : call 0x2050 but fixed A (0x2d)
					if (ZetPc(-1) == 0x7c4c)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x38:  // unneeded value : call 0x2050 but fixed A (0xf3)
					if (ZetPc(-1) == 0x7b87)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x88:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e3)
						arkanoid_bootleg_cmd = 0x00;
					if (ZetPc(-1) == 0x7c47)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x89:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e5)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x8a:  // unneeded value : call 0x2050 but fixed A (0xa5)
					if (ZetPc(-1) == 0x9661)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xc0:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e7)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xe3:  // unneeded value : call 0x2050 but fixed A (0x61)
					if (ZetPc(-1) == 0x67e9)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xff:  // unneeded value : call 0x2050 but fixed A (0xe2)
					if (ZetPc(-1) == 0x9670)
						arkanoid_bootleg_cmd = 0x00;
					break;
				default:
					arkanoid_bootleg_cmd = 0x00;
					break;
			}
			break;

		case ARKBLOC2:
			switch (data)
			{
				case 0x36:  // unneeded value : call 0x2050 but fixed A (0x2d)
					if (ZetPc(-1) == 0x7c4c)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x38:  // unneeded value : call 0x2050 but fixed A (0xf3)
					if (ZetPc(-1) == 0x7b87)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x88:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e3)
						arkanoid_bootleg_cmd = 0x00;
					if (ZetPc(-1) == 0x7c47)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x89:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e5)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x8a:  // unneeded value : call 0x2050 but unused HL and fixed DE (0x7c1c)
					if (ZetPc(-1) == 0x9661)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xc0:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e7)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xe3:  // unneeded value : call 0x2050 but fixed A (0x61)
					if (ZetPc(-1) == 0x67e9)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xf7:  // unneeded value : call 0x2050 but never called (check code at 0x0340)
					if (ZetPc(-1) == 0x0349)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xff:  // unneeded value : no call 0x2050, unused A and fixed HL (0x7d31)
					if (ZetPc(-1) == 0x9670)
						arkanoid_bootleg_cmd = 0x00;
					break;
				default:
					arkanoid_bootleg_cmd = 0x00;
					break;
			}
			break;

		case ARKGCBL:
			switch (data)
			{
				case 0x36:  // unneeded value : call 0x2050 but fixed A (0x2d)
					if (ZetPc(-1) == 0x7c4c)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x38:  // unneeded value : call 0x2050 but fixed A (0xf3)
					if (ZetPc(-1) == 0x7b87)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x88:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e3)
						arkanoid_bootleg_cmd = 0x00;
					if (ZetPc(-1) == 0x7c47)
						arkanoid_bootleg_cmd = 0x00;
				case 0x89:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e5)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x8a:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x9661)
						arkanoid_bootleg_cmd = data;
					break;
				case 0xc0:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e7)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xe3:  // unneeded value : call 0x2050 but fixed A (0x61)
					if (ZetPc(-1) == 0x67e9)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xf7:  // unneeded value : 3 * 'NOP' at 0x034f + 'JR NZ,$035D' at 0x35b
					if (ZetPc(-1) == 0x0349)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xff:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x9670)
						arkanoid_bootleg_cmd = data;
					break;
				default:
					arkanoid_bootleg_cmd = 0x00;
					break;
			}
			break;

		case PADDLE2:
			switch (data)
			{
				case 0x24:  // A read from 0xf002 (expected to be 0x9b)
					if (ZetPc(-1) == 0xbd7a)
						arkanoid_bootleg_cmd = data;
					break;
				case 0x36:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x7c4c)
						arkanoid_bootleg_cmd = data;
					break;
				case 0x38:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x7b87)
						arkanoid_bootleg_cmd = data;
					break;
				case 0x88:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e3)
						arkanoid_bootleg_cmd = 0x00;
					if (ZetPc(-1) == 0x7c47)
						arkanoid_bootleg_cmd = 0x00;
				case 0x89:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e5)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0x8a:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x9661)
						arkanoid_bootleg_cmd = data;
					break;
				case 0xc0:  // unneeded value : no read back
					if (ZetPc(-1) == 0x67e7)
						arkanoid_bootleg_cmd = 0x00;
					break;
				case 0xc3:  // A read from 0xf002 (expected to be 0x1d)
					if (ZetPc(-1) == 0xbd8a)
						arkanoid_bootleg_cmd = data;
					break;
				case 0xe3:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x67e9)
						arkanoid_bootleg_cmd = data;
					break;
				case 0xf7:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x0349)
						arkanoid_bootleg_cmd = data;
					break;
				case 0xff:  // call 0x2050 with A read from 0xf002 and wrong HL
					if (ZetPc(-1) == 0x9670)
						arkanoid_bootleg_cmd = data;
					break;
				default:
					arkanoid_bootleg_cmd = 0x00;
					break;
			}
			break;

		default:
			break;
	}
}

unsigned char __fastcall arkanoid_bootleg_d008_r()
{
	switch (arkanoid_bootleg_id)
	{
		case ARKANGC:
		case ARKBLOCK:
			return 0x00;

		case ARKANGC2:
			return 0x02;

		case ARKBLOC2:
			return (arkanoid_paddle_value < 0x40) << 5;

		case ARKGCBL:
			return ((arkanoid_paddle_value < 0x40) << 5) | 0x02;

		case PADDLE2:
			return ((arkanoid_paddle_value < 0x40) << 5) | 0x0f;

		default:
			return 0x00;
	}

	return 0;
}


static void __fastcall arkanoid_d008_w(unsigned short, unsigned char data)
{
	// bits 0 and 1 flip X and Y

	// bit 2 selects the input paddle
	arkanoid_paddle_select = data & 0x04;

	// bit 3 is coin lockout (but not the service coin)

	// bit 4 is unknown

	// bits 5 and 6 control gfx bank and palette bank. They are used together
	// so it's impossible to know which is which.

	gfxbank = (data & 0x20) >> 5;

	palettebank = (data & 0x40) >> 6;

	// bit 7 is unknown
}


unsigned char __fastcall arkanoid_read_byte(unsigned short a)
{
	switch (a)
	{
		case 0xd001:
			return AY8910Read(0);

		case 0xd008:
			return arkanoid_bootleg_d008_r();

		case 0xd00c: // arkanoid_68705_input_0_r, input_port_0_r (boot)
			return read_inputs(0);

		case 0xd010:
			return read_inputs(1);

		case 0xd018: // arkanoid_z80_mcu_r, input_port_2_r (boot)
			return read_inputs(2);

		case 0xf002:
			return arkanoid_bootleg_f002_r();
	}

	return 0;
}

void __fastcall arkanoid_write_byte(unsigned short a, unsigned char d)
{
	switch (a)
	{
		case 0xd000: // ay8910_control
		case 0xd001: // ay8910_write_port
			AY8910Write(0, a & 1, d);
		break;

		case 0xd008:
			arkanoid_d008_w(0, d);
		break;
	
		case 0xd010: // watchdog
		break;

		case 0xd018: // arkanoid_z80_mcu_w
			arkanoid_bootleg_d018_w(0, d);
		break;
	}
}


//--------------------------------------------------------------------------------
// Initilizing functions


static void arkanoid_palette_init()
{
	int i;

	for (i = 0; i < 0x200; i++)
	{
		int bit0,bit1,bit2,bit3,r,g,b;

		// red component
		bit0 = (Prom[i + 0x000] >> 0) & 0x01;
		bit1 = (Prom[i + 0x000] >> 1) & 0x01;
		bit2 = (Prom[i + 0x000] >> 2) & 0x01;
		bit3 = (Prom[i + 0x000] >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		// green component
		bit0 = (Prom[i + 0x200] >> 0) & 0x01;
		bit1 = (Prom[i + 0x200] >> 1) & 0x01;
		bit2 = (Prom[i + 0x200] >> 2) & 0x01;
		bit3 = (Prom[i + 0x200] >> 3) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		// blue component
		bit0 = (Prom[i + 0x400] >> 0) & 0x01;
		bit1 = (Prom[i + 0x400] >> 1) & 0x01;
		bit2 = (Prom[i + 0x400] >> 2) & 0x01;
		bit3 = (Prom[i + 0x400] >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		Palette[i] = (r << 16) | (g << 8) | b;
	}
}

static int GetRoms()
{
	char* pRomName;
	struct BurnRomInfo ri;
	unsigned char *RomLoad = Rom;
	unsigned char *McuLoad = Mcu;
	unsigned char *GfxLoad = Gfx;
	unsigned char *PromLoad = Prom;

	for (int i = 0; !BurnDrvGetRomName(&pRomName, i, 0); i++) {

		BurnDrvGetRomInfo(&ri, i);

		if ((ri.nType & 7) == 1) {
			if (BurnLoadRom(RomLoad, i, 1)) return 1;
			RomLoad += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 2) {
			if (BurnLoadRom(McuLoad, i, 1)) return 1;
			McuLoad += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 3) {
			if (BurnLoadRom(GfxLoad, i, 1)) return 1;
			GfxLoad += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 4) {
			if (BurnLoadRom(PromLoad, i, 1)) return 1;
			PromLoad += ri.nLen;

			continue;
		}
	}

	// Convert graphics (combine bitplanes)
	{
		unsigned char a, b, c;
		unsigned char *tmp = (unsigned char*)malloc(0x40000);
		for (int i = 0; i < 0x40000; i++)
		{
			a = (Gfx[(i >> 3) + 0x00000] >> (i & 7)) & 1;
			b = (Gfx[(i >> 3) + 0x08000] >> (i & 7)) & 1;
			c = (Gfx[(i >> 3) + 0x10000] >> (i & 7)) & 1;

			tmp[i] = (c << 2) | (b << 1) | a;
		}

		memcpy (Gfx, tmp, 0x40000);
		free (tmp);
	}

	return 0;
}

static int DrvDoReset()
{
	DrvReset = 0;
	memset (Rom + 0xc000, 0, 0x0400);
	memset (Rom + 0xe000, 0, 0x1000);

	nAnalogAxis[0] = nAnalogAxis[1] = 0;
	palettebank = 0;
	gfxbank = 0;
	arkanoid_paddle_select = 0;
	arkanoid_paddle_value = 0;

	ZetOpen(0);
	ZetReset();
	ZetClose();

	AY8910Reset(0);

	return 0;
}


static int DrvInit()
{
	Mem = (unsigned char*)malloc(0x10000 + 0x40000 + 0x800 + 0x600 + 0x800);
	if (Mem == NULL) {
		return 1;
	}

	pFMBuffer = (short *)malloc (nBurnSoundLen * 3 * sizeof(short));
	if (pFMBuffer == NULL) {
		return 1;
	}

	Rom  = Mem + 0x00000;
	Gfx  = Mem + 0x10000;
	Mcu  = Mem + 0x50000;
	Prom = Mem + 0x50800;
	Palette = (int*)(Mem + 0x50e00);

	if (GetRoms()) return 1;

	arkanoid_palette_init();

	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(arkanoid_read_byte);
	ZetSetWriteHandler(arkanoid_write_byte);
	ZetMapArea(0x0000, 0xbfff, 0, Rom + 0x0000);
	ZetMapArea(0x0000, 0xbfff, 2, Rom + 0x0000);
	ZetMapArea(0xc000, 0xc7ff, 0, Rom + 0xc000);
	ZetMapArea(0xc000, 0xc7ff, 1, Rom + 0xc000);
	ZetMapArea(0xc000, 0xc7ff, 2, Rom + 0xc000);
	ZetMapArea(0xe000, 0xefff, 0, Rom + 0xe000);
	ZetMapArea(0xe000, 0xefff, 1, Rom + 0xe000);
	ZetMapArea(0xe000, 0xefff, 2, Rom + 0xe000);
	ZetMapArea(0xf000, 0xffff, 2, Rom + 0xf000);
	ZetMemEnd();
	ZetClose();

//	MDRV_CPU_ADD_TAG("mcu", M68705, 3000000/M68705_CLOCK_DIVIDER) // 3 Mhz

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;

	AY8910Init(0, 1500000, nBurnSoundRate, &ay8910_read_port_5, &ay8910_read_port_4, NULL, NULL);

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	ZetExit();
	AY8910Exit(0);

	free (Mem);
	free (pFMBuffer);

	tetrsark = 0;

	return 0;
}


//--------------------------------------------------------------------------------
// Drawing routine


static inline void draw_tile(int sy, int sx, int num, int color, int transp)
{
	unsigned char *src = Gfx + num;

	for (int x = sx + 7; x >= sx; x--)
	{
		for (int y = sy + 7; y >= sy; y--, src++)
		{
			if (!src[0] && transp) continue;
			if (y > 255 || y < 0 || x > 239 || x < 16) continue;

			int pxl = Palette[color | src[0]];

			PutPix(pBurnDraw + ((y << 8) + x) * nBurnBpp, BurnHighCol(pxl >> 16, pxl >> 8, pxl, 0));
		}
	}
}

static int DrvDraw()
{
	unsigned char *vram = Rom + 0xe000;
	unsigned char *sram = Rom + 0xe800;

	// draw background
	for (int offs = 0; offs < 0x800; offs += 2)
	{
		int num = vram[offs + 1] + ((vram[offs] & 0x07) << 8) + 2048 * gfxbank;
		int color = ((vram[offs] & 0xf8) >> 3) + 32 * palettebank;

		int sy = (((offs >> 1) & 0x1f) << 3);
		int sx = (((offs >> 6) & 0x1f) << 3) ^ 0xf8;

		draw_tile(sy, sx, num << 6, color << 3, 0);
	}

	// draw foreground
	for (int offs = 0; offs < 0x3f; offs += 4)
	{
		int num = (sram[offs + 3] + ((sram[offs + 2] & 0x03) << 8) + 1024 * gfxbank) << 1;
		int color = ((sram[offs + 2] & 0xf8) >> 3) + 32 * palettebank;

		int sy = sram[offs];
		int sx = sram[offs + 1];

		draw_tile(sy, sx + 0, (num | 1) << 6, color << 3, 1);
		draw_tile(sy, sx + 8, (num | 0) << 6, color << 3, 1);
	}

	return 0;
}


static int DrvFrame()
{
	if (DrvReset) DrvDoReset();

	int nSoundBufferPos = 0;

	ZetOpen(0);
	ZetRun(6000000 / 60);
	ZetRaiseIrq(0xff);
	ZetClose();

	if (pBurnSoundOut) {
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

	if (pBurnDraw) DrvDraw();

	return 0;
}


//--------------------------------------------------------------------------------
// Savestates


static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x029521;
	}

	if (nAction & ACB_VOLATILE) {		// Scan volatile ram		
		memset(&ba, 0, sizeof(ba));

		ba.Data	  = Rom + 0xc000;
		ba.nLen	  = 0x0800;
		ba.szName = "Main Ram";
		BurnAcb(&ba);

		ba.Data	  = Rom + 0xe000;
		ba.nLen	  = 0x1000;
		ba.szName = "Video Ram";
		BurnAcb(&ba);

		ZetScan(nAction);			// Scan Z80
		AY8910Scan(nAction, pnMin);		// Scan AY8910

		// Scan critical driver variables
		SCAN_VAR(nAnalogAxis[0]);
		SCAN_VAR(nAnalogAxis[1]);
		SCAN_VAR(arkanoid_bootleg_cmd);
		SCAN_VAR(palettebank);
		SCAN_VAR(gfxbank);
		SCAN_VAR(arkanoid_paddle_select);
		SCAN_VAR(arkanoid_paddle_value);
	}

	return 0;
}


//--------------------------------------------------------------------------------
// Game drivers


// Arkanoid (World)

static struct BurnRomInfo arkanoidRomDesc[] = {
	{ "a75-01-1.rom", 0x8000, 0x5bcda3b0, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "a75-11.rom",   0x8000, 0xeafd7191, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "arkanoid.uc",  0x0800, 0x515d77b6, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  3 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  4
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  5

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8
};

STD_ROM_PICK(arkanoid);
STD_ROM_FN(arkanoid);

struct BurnDriver BurnDrvarkanoid = {
	"arkanoid", NULL, NULL, "1986",
	"Arkanoid (World)\0", "Unsupported MCU", "Taito Corporation Japan", "Arkanoid",
	NULL, NULL, NULL, NULL,
	0, 1, HARDWARE_MISC_PRE90S,
	NULL, arkanoidRomInfo, arkanoidRomName, DrvInputInfo, arkanoidDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (US)

static struct BurnRomInfo arknoiduRomDesc[] = {
	{ "a75-19.bin",   0x8000, 0xd3ad37d7, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "a75-18.bin",   0x8000, 0xcdc08301, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "arknoidu.uc",  0x0800, 0xde518e47, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  3 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  4
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  5

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8
};

STD_ROM_PICK(arknoidu);
STD_ROM_FN(arknoidu);

struct BurnDriverD BurnDrvarknoidu = {
	"arknoidu", "arkanoid", NULL, "1986",
	"Arkanoid (US)\0", "Unsupported MCU", "Taito America Corporation (Romstar license)", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_CLONE, 1, HARDWARE_MISC_PRE90S,
	NULL, arknoiduRomInfo, arknoiduRomName, DrvInputInfo, arkanoidDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (US, older)

static struct BurnRomInfo arknoiuoRomDesc[] = {
	{ "a75-01-1.rom", 0x8000, 0x5bcda3b0, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "a75-10.rom",   0x8000, 0xa1769e15, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "arkanoid.uc",  0x0800, 0x515d77b6, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  3 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  4
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  5

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8
};

STD_ROM_PICK(arknoiuo);
STD_ROM_FN(arknoiuo);

struct BurnDriverD BurnDrvarknoiuo = {
	"arknoiuo", "arkanoid", NULL, "1986",
	"Arkanoid (US, older)\0", "Unsupported MCU", "Taito America Corporation (Romstar license)", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_CLONE, 1, HARDWARE_MISC_PRE90S,
	NULL, arknoiuoRomInfo, arknoiuoRomName, DrvInputInfo, arkanoidDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Tournament Arkanoid (US)

static struct BurnRomInfo arkatourRomDesc[] = {
	{ "t_ark1.bin",   0x8000, 0xe3b8faf5, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "t_ark2.bin",   0x8000, 0x326aca4d, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "arkatour.uc",  0x0800, 0xd3249559, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "t_ark3.bin",   0x8000, 0x5ddea3cf, 3 | BRF_GRA },	       //  3 Graphics
	{ "t_ark4.bin",   0x8000, 0x5fcf2e85, 3 | BRF_GRA },	       //  4
	{ "t_ark5.bin",   0x8000, 0x7b76b192, 3 | BRF_GRA },	       //  5

	{ "07.bpr",       0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "08.bpr",       0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "09.bpr",       0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8
};

STD_ROM_PICK(arkatour);
STD_ROM_FN(arkatour);

struct BurnDriverD BurnDrvarkatour = {
	"arkatour", "arkanoid", NULL, "1987",
	"Tournament Arkanoid (US)\0", "Unsupported MCU", "Taito America Corporation (Romstar license)", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_CLONE, 1, HARDWARE_MISC_PRE90S,
	NULL, arkatourRomInfo, arkatourRomName, DrvInputInfo, arkanoidDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (Japan)

static struct BurnRomInfo arknoidjRomDesc[] = {
	{ "a75-21.rom",   0x8000, 0xbf0455fc, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "a75-22.rom",   0x8000, 0x3a2688d3, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "arknoidj.uc",  0x0800, 0x0a4abef6, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  3 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  4
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  5

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8
};

STD_ROM_PICK(arknoidj);
STD_ROM_FN(arknoidj);

struct BurnDriverD BurnDrvarknoidj = {
	"arknoidj", "arkanoid", NULL, "1986",
	"Arkanoid (Japan)\0", "Unsupported MCU", "Taito Corporation", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_CLONE, 1, HARDWARE_MISC_PRE90S,
	NULL, arknoidjRomInfo, arknoidjRomName, DrvInputInfo, arknoidjDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (bootleg with MCU)

static struct BurnRomInfo arkmcublRomDesc[] = {
	{ "e1.6d",        0x8000, 0xdd4f2b72, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "e2.6f",        0x8000, 0xbbc33ceb, 1 | BRF_ESS | BRF_PRG }, //  1

	// MCU from the World early version ('arkanoid'), so the game is playable
	{ "arkmcubl.uc",  0x0800, 0x515d77b6, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  3 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  4
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  5

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8

	// What is this MCU supposed to do? 
	{ "68705p3.6i",   0x0800, 0x389a8cfb, 0 | BRF_ESS | BRF_PRG }, //  9 Another MCU?
};

STD_ROM_PICK(arkmcubl);
STD_ROM_FN(arkmcubl);

struct BurnDriverD BurnDrvarkmcubl = {
	"arkmcubl", "arkanoid", NULL, "1986",
	"Arkanoid (bootleg with MCU)\0", "Unsupported MCU", "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkmcublRomInfo, arkmcublRomName, DrvInputInfo, arknoidjDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid Arkanoid (bootleg with MCU, harder)

static struct BurnRomInfo ark1ballRomDesc[] = {
	{ "a-1.7d",       0x8000, 0xdd4f2b72, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "2palline.7f",  0x8000, 0xed6b62ab, 1 | BRF_ESS | BRF_PRG }, //  1

	// MCU from the World early version ('arkanoid'), so the game is playable
	{ "ark1ball.uc",  0x0800, 0x515d77b6, 2 | BRF_ESS | BRF_PRG }, //  2 M68705 MCU

	{ "a-3.3a",       0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  3 Graphics
	{ "a-4.3d",       0x8000, 0x71fae199, 3 | BRF_GRA },	       //  4
	{ "a-5.3f",       0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  5

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  6 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  7
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  8
};

STD_ROM_PICK(ark1ball);
STD_ROM_FN(ark1ball);

struct BurnDriverD BurnDrvark1ball = {
	"ark1ball", "arkanoid", NULL, "1986",
	"Arkanoid (bootleg with MCU, harder)\0", "Unsupported MCU", "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, ark1ballRomInfo, ark1ballRomName, DrvInputInfo, ark1ballDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (Game Corporation bootleg, set 1)

static struct BurnRomInfo arkangcRomDesc[] = {
	{ "arkgc.1",      0x8000, 0xc54232e6, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "arkgc.2",      0x8000, 0x9f0d4754, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(arkangc);
STD_ROM_FN(arkangc);

static int arkangcInit()
{
	arkanoid_bootleg_id = ARKANGC;

	return DrvInit();
}

struct BurnDriver BurnDrvarkangc = {
	"arkangc", "arkanoid", NULL, "1986",
	"Arkanoid (Game Corporation bootleg, set 1)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkangcRomInfo, arkangcRomName, DrvInputInfo, arkangcDIPInfo,
	arkangcInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (Game Corporation bootleg, set 2)

static struct BurnRomInfo arkangc2RomDesc[] = {
	{ "1.81",         0x8000, 0xbd6eb996, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "2.82",         0x8000, 0x29dbe452, 1 | BRF_ESS | BRF_PRG }, //  1
 
	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(arkangc2);
STD_ROM_FN(arkangc2);

static int arkangc2Init()
{
	arkanoid_bootleg_id = ARKANGC2;

	return DrvInit();
}

struct BurnDriver BurnDrvarkangc2 = {
	"arkangc2", "arkanoid", NULL, "1986",
	"Arkanoid (Game Corporation bootleg, set 2)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkangc2RomInfo, arkangc2RomName, DrvInputInfo, arkangc2DIPInfo,
	arkangc2Init, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Block (Game Corporation bootleg, set 1)

static struct BurnRomInfo arkblockRomDesc[] = {
	{ "ark-6.bin",    0x8000, 0x0be015de, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "arkgc.2",      0x8000, 0x9f0d4754, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(arkblock);
STD_ROM_FN(arkblock);

static int arkblockInit()
{
	arkanoid_bootleg_id = ARKBLOCK;

	return DrvInit();
}

struct BurnDriver BurnDrvarkblock = {
	"arkblock", "arkanoid", NULL, "1986",
	"Block (Game Corporation bootleg, set 1)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkblockRomInfo, arkblockRomName, DrvInputInfo, arkangcDIPInfo,
	arkblockInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Block (Game Corporation bootleg, set 2)

static struct BurnRomInfo arkbloc2RomDesc[] = {
	{ "block01.bin",  0x8000, 0x5be667e1, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "block02.bin",  0x8000, 0x4f883ef1, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(arkbloc2);
STD_ROM_FN(arkbloc2);

static int arkbloc2Init()
{
	arkanoid_bootleg_id = ARKBLOC2;

	return DrvInit();
}

struct BurnDriver BurnDrvarkbloc2 = {
	"arkbloc2", "arkanoid", NULL, "1986",
	"Block (Game Corporation bootleg, set 2)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkbloc2RomInfo, arkbloc2RomName, DrvInputInfo, arkangcDIPInfo,
	arkbloc2Init, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (bootleg on Block hardware)

static struct BurnRomInfo arkgcblRomDesc[] = {
	{ "arkanunk.1",   0x8000, 0xb0f73900, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "arkanunk.2",   0x8000, 0x9827f297, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(arkgcbl);
STD_ROM_FN(arkgcbl);

static int arkgcblInit()
{
	arkanoid_bootleg_id = ARKGCBL;

	return DrvInit();
}

struct BurnDriver BurnDrvarkgcbl = {
	"arkgcbl", "arkanoid", NULL, "1986",
	"Arkanoid (bootleg on Block hardware)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkgcblRomInfo, arkgcblRomName, DrvInputInfo, arkgcblDIPInfo,
	arkgcblInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Paddle 2 (bootleg on Block hardware)

static struct BurnRomInfo paddle2RomDesc[] = {
	{ "paddle2.16",   0x8000, 0xa286333c, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "paddle2.17",   0x8000, 0x04c2acb5, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "a75-03.rom",   0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "a75-04.rom",   0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "a75-05.rom",   0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(paddle2);
STD_ROM_FN(paddle2);

static int paddle2Init()
{
	arkanoid_bootleg_id = PADDLE2;

	return DrvInit();
}

struct BurnDriver BurnDrvpaddle2 = {
	"paddle2", "arkanoid", NULL, "1986",
	"Paddle 2 (bootleg on Block hardware)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, paddle2RomInfo, paddle2RomName, DrvInputInfo, paddle2DIPInfo,
	paddle2Init, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (Tayto bootleg)

static struct BurnRomInfo arkataytRomDesc[] = {
	{ "ic81-v.3f",    0x8000, 0x154e2c6f, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "ic82-w.5f",    0x8000, 0x4fa8cefa, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "1-ic33.2c",    0x8000, 0x038b74ba, 3 | BRF_GRA },	       //  2 Graphics
	{ "2-ic34.3c",    0x8000, 0x71fae199, 3 | BRF_GRA },	       //  3
	{ "3-ic35.5c",    0x8000, 0xc76374e2, 3 | BRF_GRA },	       //  4

	{ "ic73.11e",     0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "ic74.12e",     0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "ic75.13e",     0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(arkatayt);
STD_ROM_FN(arkatayt);

struct BurnDriver BurnDrvarkatayt = {
	"arkatayt", "arkanoid", NULL, "1986",
	"Arkanoid (Tayto bootleg)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arkataytRomInfo, arkataytRomName, DrvInputInfo, arknoidjDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Arkanoid (Tayto bootleg, harder)

static struct BurnRomInfo arktayt2RomDesc[] = {
	{ "ic81.3f",    0x8000, 0x6e0a2b6f, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "ic82.5f",    0x8000, 0x5a97dd56, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "1-ic33.2c",  0x8000, 0x038b74ba, 3 | BRF_GRA },	     //  2 Graphics
	{ "2-ic34.3c",  0x8000, 0x71fae199, 3 | BRF_GRA },	     //  3
	{ "3-ic35.5c",  0x8000, 0xc76374e2, 3 | BRF_GRA },	     //  4

	{ "ic73.11e",   0x0200, 0x0af8b289, 4 | BRF_GRA },	     //  5 Color Proms
	{ "ic74.12e",   0x0200, 0xabb002fb, 4 | BRF_GRA },	     //  6
	{ "ic75.13e",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	     //  7
};

STD_ROM_PICK(arktayt2);
STD_ROM_FN(arktayt2);

struct BurnDriver BurnDrvarktayt2 = {
	"arktayt2", "arkanoid", NULL, "1986",
	"Arkanoid (Tayto bootleg, harder)\0", NULL, "bootleg", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 1, HARDWARE_MISC_PRE90S,
	NULL, arktayt2RomInfo, arktayt2RomName, DrvInputInfo, arktayt2DIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};


// Tetris (D.R. Korea)

static struct BurnRomInfo tetrsarkRomDesc[] = {
	{ "ic17.1",       0x8000, 0x1a505eda, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 Code
	{ "ic16.2",       0x8000, 0x157bc4df, 1 | BRF_ESS | BRF_PRG }, //  1

	{ "ic64.3",       0x8000, 0xc3e9b290, 3 | BRF_GRA },	       //  2 Graphics
	{ "ic63.4",       0x8000, 0xde9a368f, 3 | BRF_GRA },	       //  3
	{ "ic62.5",       0x8000, 0xc8e80a00, 3 | BRF_GRA },	       //  4

	{ "a75-07.bpr",   0x0200, 0x0af8b289, 4 | BRF_GRA },	       //  5 Color Proms
	{ "a75-08.bpr",   0x0200, 0xabb002fb, 4 | BRF_GRA },	       //  6
	{ "a75-09.bpr",   0x0200, 0xa7c6c277, 4 | BRF_GRA },	       //  7
};

STD_ROM_PICK(tetrsark);
STD_ROM_FN(tetrsark);

static int tetrsarkInit()
{
	tetrsark = 1;

	int nRet = DrvInit();

	for (int i = 0; i < 0x8000; i++)
	{
		Rom[i] ^= 0x94;
	}

	return nRet;
}

struct BurnDriverD BurnDrvtetrsark = {
	"tetrsark", NULL, NULL, "198?",
	"Tetris (D.R. Korea)\0", "Wrong colors", "D.R. Korea", "Arkanoid",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 1, HARDWARE_MISC_PRE90S,
	NULL, tetrsarkRomInfo, tetrsarkRomName, tetrsarkInputInfo, tetrsarkDIPInfo,
	tetrsarkInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 256, 4, 3
};

