// PC080SN & PC090OJ based games

#include "tiles_generic.h"
#include "taito.h"
#include "taito_ic.h"
#include "msm5205.h"
#include "burn_ym2151.h"
#include "burn_ym2203.h"
#include "burn_gun.h"

static unsigned char OpwolfADPCM_B[0x08];
static unsigned char OpwolfADPCM_C[0x08];
static int OpWolfGunXOffset;
static int OpWolfGunYOffset;

static unsigned char DariusADPCMCommand;
static int DariusNmiEnable;
static UINT16 DariusCoinWord;

static void DariusDraw();
static void OpwolfDraw();
static void RainbowDraw();
static void JumpingDraw();
static void RastanDraw();

#define A(a, b, c, d) {a, b, (unsigned char*)(c), d}

static struct BurnInputInfo DariusInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL   , TaitoInputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL   , TaitoInputPort2 + 2, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL   , TaitoInputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL   , TaitoInputPort2 + 3, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL   , TaitoInputPort0 + 0, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL   , TaitoInputPort0 + 1, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL   , TaitoInputPort0 + 3, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL   , TaitoInputPort0 + 2, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL   , TaitoInputPort0 + 4, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , TaitoInputPort0 + 5, "p1 fire 2" },
	
	{"P2 Up"             , BIT_DIGITAL   , TaitoInputPort1 + 0, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL   , TaitoInputPort1 + 1, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL   , TaitoInputPort1 + 3, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL   , TaitoInputPort1 + 2, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL   , TaitoInputPort1 + 4, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL   , TaitoInputPort1 + 5, "p2 fire 2" },
	
	{"Reset"             , BIT_DIGITAL   , &TaitoReset        , "reset"     },
	{"Service"           , BIT_DIGITAL   , TaitoInputPort2 + 4, "service"   },
	{"Tilt"              , BIT_DIGITAL   , TaitoInputPort2 + 5, "tilt"      },
	{"Dip 1"             , BIT_DIPSWITCH , TaitoDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , TaitoDip + 1       , "dip"       },
};

STDINPUTINFO(Darius);

static struct BurnInputInfo OpwolfInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL   , TaitoInputPort0 + 0, "p1 coin"        },
	{"Start 1"           , BIT_DIGITAL   , TaitoInputPort1 + 4, "p1 start"       },
	{"Coin 2"            , BIT_DIGITAL   , TaitoInputPort0 + 1, "p2 coin"        },

	A("P1 Gun X"         , BIT_ANALOG_REL, &TaitoAnalogPort0  , "mouse x-axis"   ),
	A("P1 Gun Y"         , BIT_ANALOG_REL, &TaitoAnalogPort1  , "mouse y-axis"   ),
	{"P1 Fire 1"         , BIT_DIGITAL   , TaitoInputPort1 + 0, "mouse button 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , TaitoInputPort1 + 1, "mouse button 2" },
	
	{"Reset"             , BIT_DIGITAL   , &TaitoReset        , "reset"          },
	{"Service"           , BIT_DIGITAL   , TaitoInputPort1 + 2, "service"        },
	{"Tilt"              , BIT_DIGITAL   , TaitoInputPort1 + 3, "tilt"           },
	{"Dip 1"             , BIT_DIPSWITCH , TaitoDip + 0       , "dip"            },
	{"Dip 2"             , BIT_DIPSWITCH , TaitoDip + 1       , "dip"            },
};

STDINPUTINFO(Opwolf);

static struct BurnInputInfo RainbowInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL   , TaitoInputPort1 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL   , TaitoInputPort0 + 6, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL   , TaitoInputPort1 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL   , TaitoInputPort0 + 5, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL   , TaitoInputPort2 + 4, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL   , TaitoInputPort2 + 5, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL   , TaitoInputPort2 + 6, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , TaitoInputPort2 + 7, "p1 fire 2" },
	
	{"P2 Left"           , BIT_DIGITAL   , TaitoInputPort3 + 4, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL   , TaitoInputPort3 + 5, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL   , TaitoInputPort3 + 6, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL   , TaitoInputPort3 + 7, "p2 fire 2" },
	
	{"Reset"             , BIT_DIGITAL   , &TaitoReset        , "reset"     },
	{"Service"           , BIT_DIGITAL   , TaitoInputPort0 + 7, "service"   },
	{"Tilt"              , BIT_DIGITAL   , TaitoInputPort2 + 0, "tilt"      },
	{"Dip 1"             , BIT_DIPSWITCH , TaitoDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , TaitoDip + 1       , "dip"       },
};

STDINPUTINFO(Rainbow);

static struct BurnInputInfo JumpingInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL   , TaitoInputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL   , TaitoInputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL   , TaitoInputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL   , TaitoInputPort0 + 5, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL   , TaitoInputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL   , TaitoInputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL   , TaitoInputPort1 + 1, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , TaitoInputPort1 + 2, "p1 fire 2" },
	
	{"Reset"             , BIT_DIGITAL   , &TaitoReset        , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , TaitoDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , TaitoDip + 1       , "dip"       },
};

STDINPUTINFO(Jumping);

static struct BurnInputInfo RastanInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL   , TaitoInputPort3 + 5, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL   , TaitoInputPort3 + 3, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL   , TaitoInputPort3 + 6, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL   , TaitoInputPort3 + 4, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL   , TaitoInputPort0 + 0, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL   , TaitoInputPort0 + 1, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL   , TaitoInputPort0 + 2, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL   , TaitoInputPort0 + 3, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL   , TaitoInputPort0 + 4, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , TaitoInputPort0 + 5, "p1 fire 2" },
	
	{"P2 Up"             , BIT_DIGITAL   , TaitoInputPort1 + 0, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL   , TaitoInputPort1 + 1, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL   , TaitoInputPort1 + 2, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL   , TaitoInputPort1 + 3, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL   , TaitoInputPort1 + 4, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL   , TaitoInputPort1 + 5, "p2 fire 2" },
	
	{"Reset"             , BIT_DIGITAL   , &TaitoReset        , "reset"     },
	{"Service"           , BIT_DIGITAL   , TaitoInputPort3 + 0, "service"   },
	{"Tilt"              , BIT_DIGITAL   , TaitoInputPort3 + 2, "tilt"      },
	{"Dip 1"             , BIT_DIPSWITCH , TaitoDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , TaitoDip + 1       , "dip"       },
};

STDINPUTINFO(Rastan);

#undef A

static void DariusMakeInputs()
{
	// Reset Inputs
	TaitoInput[0] = 0xff;
	TaitoInput[1] = 0xff;
	TaitoInput[2] = 0xfc;
	TaitoInput[3] = 0xff;

	if (TaitoInputPort0[0]) TaitoInput[0] -= 0x01;
	if (TaitoInputPort0[1]) TaitoInput[0] -= 0x02;
	if (TaitoInputPort0[2]) TaitoInput[0] -= 0x04;
	if (TaitoInputPort0[3]) TaitoInput[0] -= 0x08;
	if (TaitoInputPort0[4]) TaitoInput[0] -= 0x10;
	if (TaitoInputPort0[5]) TaitoInput[0] -= 0x20;
	if (TaitoInputPort0[6]) TaitoInput[0] -= 0x40;
	if (TaitoInputPort0[7]) TaitoInput[0] -= 0x80;
	
	if (TaitoInputPort1[0]) TaitoInput[1] -= 0x01;
	if (TaitoInputPort1[1]) TaitoInput[1] -= 0x02;
	if (TaitoInputPort1[2]) TaitoInput[1] -= 0x04;
	if (TaitoInputPort1[3]) TaitoInput[1] -= 0x08;
	if (TaitoInputPort1[4]) TaitoInput[1] -= 0x10;
	if (TaitoInputPort1[5]) TaitoInput[1] -= 0x20;
	if (TaitoInputPort1[6]) TaitoInput[1] -= 0x40;
	if (TaitoInputPort1[7]) TaitoInput[1] -= 0x80;
	
	if (TaitoInputPort2[0]) TaitoInput[2] |= 0x01;
	if (TaitoInputPort2[1]) TaitoInput[2] |= 0x02;
	if (TaitoInputPort2[2]) TaitoInput[2] -= 0x04;
	if (TaitoInputPort2[3]) TaitoInput[2] -= 0x08;
	if (TaitoInputPort2[4]) TaitoInput[2] -= 0x10;
	if (TaitoInputPort2[5]) TaitoInput[2] -= 0x20;
	if (TaitoInputPort2[6]) TaitoInput[2] -= 0x40;
	if (TaitoInputPort2[7]) TaitoInput[2] -= 0x80;
}

static void OpwolfMakeInputs()
{
	// Reset Inputs
	TaitoInput[0] = 0xfc;
	TaitoInput[1] = 0xff;
	TaitoInput[2] = 0xff;
	TaitoInput[3] = 0xff;

	if (TaitoInputPort0[0]) TaitoInput[0] |= 0x01;
	if (TaitoInputPort0[1]) TaitoInput[0] |= 0x02;
	if (TaitoInputPort0[2]) TaitoInput[0] -= 0x04;
	if (TaitoInputPort0[3]) TaitoInput[0] -= 0x08;
	if (TaitoInputPort0[4]) TaitoInput[0] -= 0x10;
	if (TaitoInputPort0[5]) TaitoInput[0] -= 0x20;
	if (TaitoInputPort0[6]) TaitoInput[0] -= 0x40;
	if (TaitoInputPort0[7]) TaitoInput[0] -= 0x80;
	
	if (TaitoInputPort1[0]) TaitoInput[1] -= 0x01;
	if (TaitoInputPort1[1]) TaitoInput[1] -= 0x02;
	if (TaitoInputPort1[2]) TaitoInput[1] -= 0x04;
	if (TaitoInputPort1[3]) TaitoInput[1] -= 0x08;
	if (TaitoInputPort1[4]) TaitoInput[1] -= 0x10;
	if (TaitoInputPort1[5]) TaitoInput[1] -= 0x20;
	if (TaitoInputPort1[6]) TaitoInput[1] -= 0x40;
	if (TaitoInputPort1[7]) TaitoInput[1] -= 0x80;
	
	BurnGunMakeInputs(0, (short)TaitoAnalogPort0, (short)TaitoAnalogPort1);
	
	OpwolfCChipUpdate(TaitoInput[0], TaitoInput[1]);
}

static void OpwolfbMakeInputs()
{
	// Reset Inputs
	TaitoInput[0] = 0xfc;
	TaitoInput[1] = 0xff;
	TaitoInput[2] = 0xff;
	TaitoInput[3] = 0xff;

	if (TaitoInputPort0[0]) TaitoInput[0] |= 0x01;
	if (TaitoInputPort0[1]) TaitoInput[0] |= 0x02;
	if (TaitoInputPort0[2]) TaitoInput[0] -= 0x04;
	if (TaitoInputPort0[3]) TaitoInput[0] -= 0x08;
	if (TaitoInputPort0[4]) TaitoInput[0] -= 0x10;
	if (TaitoInputPort0[5]) TaitoInput[0] -= 0x20;
	if (TaitoInputPort0[6]) TaitoInput[0] -= 0x40;
	if (TaitoInputPort0[7]) TaitoInput[0] -= 0x80;
	
	if (TaitoInputPort1[0]) TaitoInput[1] -= 0x01;
	if (TaitoInputPort1[1]) TaitoInput[1] -= 0x02;
	if (TaitoInputPort1[2]) TaitoInput[1] -= 0x04;
	if (TaitoInputPort1[3]) TaitoInput[1] -= 0x08;
	if (TaitoInputPort1[4]) TaitoInput[1] -= 0x10;
	if (TaitoInputPort1[5]) TaitoInput[1] -= 0x20;
	if (TaitoInputPort1[6]) TaitoInput[1] -= 0x40;
	if (TaitoInputPort1[7]) TaitoInput[1] -= 0x80;
	
	BurnGunMakeInputs(0, (short)TaitoAnalogPort0, (short)TaitoAnalogPort1);
}

static void RainbowMakeInputs()
{
	// Reset Inputs
	TaitoInput[0] = 0xff;
	TaitoInput[1] = 0xfc;
	TaitoInput[2] = 0xff;
	TaitoInput[3] = 0xff;

	if (TaitoInputPort0[0]) TaitoInput[0] -= 0x01;
	if (TaitoInputPort0[1]) TaitoInput[0] -= 0x02;
	if (TaitoInputPort0[2]) TaitoInput[0] -= 0x04;
	if (TaitoInputPort0[3]) TaitoInput[0] -= 0x08;
	if (TaitoInputPort0[4]) TaitoInput[0] -= 0x10;
	if (TaitoInputPort0[5]) TaitoInput[0] -= 0x20;
	if (TaitoInputPort0[6]) TaitoInput[0] -= 0x40;
	if (TaitoInputPort0[7]) TaitoInput[0] -= 0x80;
	
	if (TaitoInputPort1[0]) TaitoInput[1] |= 0x01;
	if (TaitoInputPort1[1]) TaitoInput[1] |= 0x02;
	if (TaitoInputPort1[2]) TaitoInput[1] -= 0x04;
	if (TaitoInputPort1[3]) TaitoInput[1] -= 0x08;
	if (TaitoInputPort1[4]) TaitoInput[1] -= 0x10;
	if (TaitoInputPort1[5]) TaitoInput[1] -= 0x20;
	if (TaitoInputPort1[6]) TaitoInput[1] -= 0x40;
	if (TaitoInputPort1[7]) TaitoInput[1] -= 0x80;
	
	if (TaitoInputPort2[0]) TaitoInput[2] -= 0x01;
	if (TaitoInputPort2[1]) TaitoInput[2] -= 0x02;
	if (TaitoInputPort2[2]) TaitoInput[2] -= 0x04;
	if (TaitoInputPort2[3]) TaitoInput[2] -= 0x08;
	if (TaitoInputPort2[4]) TaitoInput[2] -= 0x10;
	if (TaitoInputPort2[5]) TaitoInput[2] -= 0x20;
	if (TaitoInputPort2[6]) TaitoInput[2] -= 0x40;
	if (TaitoInputPort2[7]) TaitoInput[2] -= 0x80;
	
	if (TaitoInputPort3[0]) TaitoInput[3] -= 0x01;
	if (TaitoInputPort3[1]) TaitoInput[3] -= 0x02;
	if (TaitoInputPort3[2]) TaitoInput[3] -= 0x04;
	if (TaitoInputPort3[3]) TaitoInput[3] -= 0x08;
	if (TaitoInputPort3[4]) TaitoInput[3] -= 0x10;
	if (TaitoInputPort3[5]) TaitoInput[3] -= 0x20;
	if (TaitoInputPort3[6]) TaitoInput[3] -= 0x40;
	if (TaitoInputPort3[7]) TaitoInput[3] -= 0x80;
	
	RainbowCChipUpdate(TaitoInput[0], TaitoInput[1], TaitoInput[2], TaitoInput[3]);
}

static void JumpingMakeInputs()
{
	// Reset Inputs
	TaitoInput[0] = 0xff;
	TaitoInput[1] = 0xff;
	TaitoInput[2] = 0xff;
	TaitoInput[3] = 0xff;

	if (TaitoInputPort0[0]) TaitoInput[0] -= 0x01;
	if (TaitoInputPort0[1]) TaitoInput[0] -= 0x02;
	if (TaitoInputPort0[2]) TaitoInput[0] -= 0x04;
	if (TaitoInputPort0[3]) TaitoInput[0] -= 0x08;
	if (TaitoInputPort0[4]) TaitoInput[0] -= 0x10;
	if (TaitoInputPort0[5]) TaitoInput[0] -= 0x20;
	if (TaitoInputPort0[6]) TaitoInput[0] -= 0x40;
	if (TaitoInputPort0[7]) TaitoInput[0] -= 0x80;
	
	if (TaitoInputPort1[0]) TaitoInput[1] -= 0x01;
	if (TaitoInputPort1[1]) TaitoInput[1] -= 0x02;
	if (TaitoInputPort1[2]) TaitoInput[1] -= 0x04;
	if (TaitoInputPort1[3]) TaitoInput[1] -= 0x08;
	if (TaitoInputPort1[4]) TaitoInput[1] -= 0x10;
	if (TaitoInputPort1[5]) TaitoInput[1] -= 0x20;
	if (TaitoInputPort1[6]) TaitoInput[1] -= 0x40;
	if (TaitoInputPort1[7]) TaitoInput[1] -= 0x80;
}

static void RastanMakeInputs()
{
	// Reset Inputs
	TaitoInput[0] = 0xff;
	TaitoInput[1] = 0xff;
	TaitoInput[2] = 0x8f;
	TaitoInput[3] = 0x1f;

	if (TaitoInputPort0[0]) TaitoInput[0] -= 0x01;
	if (TaitoInputPort0[1]) TaitoInput[0] -= 0x02;
	if (TaitoInputPort0[2]) TaitoInput[0] -= 0x04;
	if (TaitoInputPort0[3]) TaitoInput[0] -= 0x08;
	if (TaitoInputPort0[4]) TaitoInput[0] -= 0x10;
	if (TaitoInputPort0[5]) TaitoInput[0] -= 0x20;
	if (TaitoInputPort0[6]) TaitoInput[0] -= 0x40;
	if (TaitoInputPort0[7]) TaitoInput[0] -= 0x80;
	
	if (TaitoInputPort1[0]) TaitoInput[1] -= 0x01;
	if (TaitoInputPort1[1]) TaitoInput[1] -= 0x02;
	if (TaitoInputPort1[2]) TaitoInput[1] -= 0x04;
	if (TaitoInputPort1[3]) TaitoInput[1] -= 0x08;
	if (TaitoInputPort1[4]) TaitoInput[1] -= 0x10;
	if (TaitoInputPort1[5]) TaitoInput[1] -= 0x20;
	if (TaitoInputPort1[6]) TaitoInput[1] -= 0x40;
	if (TaitoInputPort1[7]) TaitoInput[1] -= 0x80;
	
	if (TaitoInputPort3[0]) TaitoInput[3] -= 0x01;
	if (TaitoInputPort3[1]) TaitoInput[3] -= 0x02;
	if (TaitoInputPort3[2]) TaitoInput[3] -= 0x04;
	if (TaitoInputPort3[3]) TaitoInput[3] -= 0x08;
	if (TaitoInputPort3[4]) TaitoInput[3] -= 0x10;
	if (TaitoInputPort3[5]) TaitoInput[3] |= 0x20;
	if (TaitoInputPort3[6]) TaitoInput[3] |= 0x40;
	if (TaitoInputPort3[7]) TaitoInput[3] |= 0x80;
}

static struct BurnDIPInfo DariusDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                             },
	{0x14, 0xff, 0xff, 0xff, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Auto Fire"                      },
	{0x13, 0x01, 0x02, 0x02, "Normal"                         },
	{0x13, 0x01, 0x02, 0x00, "Fast"                           },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x13, 0x01, 0x04, 0x04, "Off"                            },
	{0x13, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x13, 0x01, 0x08, 0x00, "Off"                            },
	{0x13, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x13, 0x01, 0x30, 0x00, "4 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x10, "3 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x20, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x13, 0x01, 0xc0, 0xc0, "1 Coin 2 Credits"               },
	{0x13, 0x01, 0xc0, 0x80, "1 Coin 3 Credits"               },
	{0x13, 0x01, 0xc0, 0x40, "1 Coin 4 Credits"               },
	{0x13, 0x01, 0xc0, 0x00, "1 Coin 6 Credits"               },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x14, 0x01, 0x03, 0x02, "Easy"                           },
	{0x14, 0x01, 0x03, 0x03, "Medium"                         },
	{0x14, 0x01, 0x03, 0x01, "Hard"                           },
	{0x14, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"                     },
	{0x14, 0x01, 0x0c, 0x08, "Every 600k"                     },
	{0x14, 0x01, 0x0c, 0x0c, "600k only"                      },
	{0x14, 0x01, 0x0c, 0x04, "800k only"                      },
	{0x14, 0x01, 0x0c, 0x00, "None"                           },
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x14, 0x01, 0x30, 0x30, "3"                              },
	{0x14, 0x01, 0x30, 0x20, "4"                              },
	{0x14, 0x01, 0x30, 0x10, "5"                              },
	{0x14, 0x01, 0x30, 0x00, "6"                              },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x14, 0x01, 0x80, 0x00, "Off"                            },
	{0x14, 0x01, 0x80, 0x80, "On"                             },
};

STDDIPINFO(Darius);


static struct BurnDIPInfo DariuseDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                             },
	{0x14, 0xff, 0xff, 0xff, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Auto Fire"                      },
	{0x13, 0x01, 0x02, 0x02, "Normal"                         },
	{0x13, 0x01, 0x02, 0x00, "Fast"                           },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x13, 0x01, 0x04, 0x04, "Off"                            },
	{0x13, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x13, 0x01, 0x08, 0x00, "Off"                            },
	{0x13, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x13, 0x01, 0x30, 0x10, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },
	{0x13, 0x01, 0x30, 0x00, "2 Coins 3 Credits"              },
	{0x13, 0x01, 0x30, 0x20, "1 Coin  2 Credits"              },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x13, 0x01, 0xc0, 0x40, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0xc0, 0xc0, "1 Coin  1 Credit"               },
	{0x13, 0x01, 0xc0, 0x00, "2 Coins 3 Credits"              },
	{0x13, 0x01, 0xc0, 0x80, "1 Coin  2 Credits"              },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x14, 0x01, 0x03, 0x02, "Easy"                           },
	{0x14, 0x01, 0x03, 0x03, "Medium"                         },
	{0x14, 0x01, 0x03, 0x01, "Hard"                           },
	{0x14, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"                     },
	{0x14, 0x01, 0x0c, 0x08, "Every 600k"                     },
	{0x14, 0x01, 0x0c, 0x0c, "600k only"                      },
	{0x14, 0x01, 0x0c, 0x04, "800k only"                      },
	{0x14, 0x01, 0x0c, 0x00, "None"                           },
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x14, 0x01, 0x30, 0x30, "3"                              },
	{0x14, 0x01, 0x30, 0x20, "4"                              },
	{0x14, 0x01, 0x30, 0x10, "5"                              },
	{0x14, 0x01, 0x30, 0x00, "6"                              },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x14, 0x01, 0x80, 0x00, "Off"                            },
	{0x14, 0x01, 0x80, 0x80, "On"                             },
};

STDDIPINFO(Dariuse);

static struct BurnDIPInfo DariusjDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                             },
	{0x14, 0xff, 0xff, 0xff, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Auto Fire"                      },
	{0x13, 0x01, 0x02, 0x02, "Normal"                         },
	{0x13, 0x01, 0x02, 0x00, "Fast"                           },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x13, 0x01, 0x04, 0x04, "Off"                            },
	{0x13, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x13, 0x01, 0x08, 0x00, "Off"                            },
	{0x13, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x13, 0x01, 0x30, 0x10, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },
	{0x13, 0x01, 0x30, 0x00, "2 Coins 3 Credits"              },
	{0x13, 0x01, 0x30, 0x20, "1 Coin  2 Credits"              },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x13, 0x01, 0xc0, 0x40, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0xc0, 0xc0, "1 Coin  1 Credit"               },
	{0x13, 0x01, 0xc0, 0x00, "2 Coins 3 Credits"              },
	{0x13, 0x01, 0xc0, 0x80, "1 Coin  2 Credits"              },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x14, 0x01, 0x03, 0x02, "Easy"                           },
	{0x14, 0x01, 0x03, 0x03, "Medium"                         },
	{0x14, 0x01, 0x03, 0x01, "Hard"                           },
	{0x14, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"                     },
	{0x14, 0x01, 0x0c, 0x08, "Every 600k"                     },
	{0x14, 0x01, 0x0c, 0x0c, "600k only"                      },
	{0x14, 0x01, 0x0c, 0x04, "800k only"                      },
	{0x14, 0x01, 0x0c, 0x00, "None"                           },
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x14, 0x01, 0x30, 0x30, "3"                              },
	{0x14, 0x01, 0x30, 0x20, "4"                              },
	{0x14, 0x01, 0x30, 0x10, "5"                              },
	{0x14, 0x01, 0x30, 0x00, "6"                              },
};

STDDIPINFO(Dariusj);

static struct BurnDIPInfo OpwolfDIPList[]=
{
	// Default Values
	{0x0a, 0xff, 0xff, 0xff, NULL                             },
	{0x0b, 0xff, 0xff, 0x7f, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x0a, 0x01, 0x02, 0x02, "Off"                            },
	{0x0a, 0x01, 0x02, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x0a, 0x01, 0x04, 0x04, "Off"                            },
	{0x0a, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x0a, 0x01, 0x08, 0x00, "Off"                            },
	{0x0a, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x0a, 0x01, 0x30, 0x00, "4 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x10, "3 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x20, "2 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x0a, 0x01, 0xc0, 0xc0, "1 Coin 2 Credits"               },
	{0x0a, 0x01, 0xc0, 0x80, "1 Coin 3 Credits"               },
	{0x0a, 0x01, 0xc0, 0x40, "1 Coin 4 Credits"               },
	{0x0a, 0x01, 0xc0, 0x00, "1 Coin 6 Credits"               },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x0b, 0x01, 0x03, 0x02, "Easy"                           },
	{0x0b, 0x01, 0x03, 0x03, "Medium"                         },
	{0x0b, 0x01, 0x03, 0x01, "Hard"                           },
	{0x0b, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Ammo Magazines at Start"        },
	{0x0b, 0x01, 0x0c, 0x00, "4"                              },
	{0x0b, 0x01, 0x0c, 0x04, "5"                              },
	{0x0b, 0x01, 0x0c, 0x0c, "6"                              },
	{0x0b, 0x01, 0x0c, 0x08, "7"                              },
	
	{0   , 0xfe, 0   , 2   , "Language"                       },
	{0x0b, 0x01, 0x80, 0x80, "Japanese"                       },
	{0x0b, 0x01, 0x80, 0x00, "English"                        },
};

STDDIPINFO(Opwolf);

static struct BurnDIPInfo OpwolfuDIPList[]=
{
	// Default Values
	{0x0a, 0xff, 0xff, 0xff, NULL                             },
	{0x0b, 0xff, 0xff, 0x7f, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x0a, 0x01, 0x02, 0x02, "Off"                            },
	{0x0a, 0x01, 0x02, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x0a, 0x01, 0x04, 0x04, "Off"                            },
	{0x0a, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x0a, 0x01, 0x08, 0x00, "Off"                            },
	{0x0a, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x0a, 0x01, 0x30, 0x10, "2 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },
	{0x0a, 0x01, 0x30, 0x00, "2 Coins 3 Credits"              },
	{0x0a, 0x01, 0x30, 0x20, "1 Coin  2 Credits"              },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x0a, 0x01, 0xc0, 0x40, "2 Coins 1 Credit"               },
	{0x0a, 0x01, 0xc0, 0xc0, "1 Coin  1 Credit"               },
	{0x0a, 0x01, 0xc0, 0x00, "2 Coins 3 Credits"              },
	{0x0a, 0x01, 0xc0, 0x80, "1 Coin  2 Credits"              },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x0b, 0x01, 0x03, 0x02, "Easy"                           },
	{0x0b, 0x01, 0x03, 0x03, "Medium"                         },
	{0x0b, 0x01, 0x03, 0x01, "Hard"                           },
	{0x0b, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Ammo Magazines at Start"        },
	{0x0b, 0x01, 0x0c, 0x00, "4"                              },
	{0x0b, 0x01, 0x0c, 0x04, "5"                              },
	{0x0b, 0x01, 0x0c, 0x0c, "6"                              },
	{0x0b, 0x01, 0x0c, 0x08, "7"                              },
	
	{0   , 0xfe, 0   , 2   , "Language"                       },
	{0x0b, 0x01, 0x80, 0x80, "Japanese"                       },
	{0x0b, 0x01, 0x80, 0x00, "English"                        },
};

STDDIPINFO(Opwolfu);

static struct BurnDIPInfo OpwolfbDIPList[]=
{
	// Default Values
	{0x0a, 0xff, 0xff, 0xff, NULL                             },
	{0x0b, 0xff, 0xff, 0x7f, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x0a, 0x01, 0x02, 0x02, "Off"                            },
	{0x0a, 0x01, 0x02, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x0a, 0x01, 0x04, 0x04, "Off"                            },
	{0x0a, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x0a, 0x01, 0x08, 0x00, "Off"                            },
	{0x0a, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x0a, 0x01, 0x30, 0x00, "4 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x10, "3 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x20, "2 Coins 1 Credit"               },
	{0x0a, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x0a, 0x01, 0xc0, 0xc0, "1 Coin 2 Credits"               },
	{0x0a, 0x01, 0xc0, 0x80, "1 Coin 3 Credits"               },
	{0x0a, 0x01, 0xc0, 0x40, "1 Coin 4 Credits"               },
	{0x0a, 0x01, 0xc0, 0x00, "1 Coin 6 Credits"               },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x0b, 0x01, 0x03, 0x02, "Easy"                           },
	{0x0b, 0x01, 0x03, 0x03, "Medium"                         },
	{0x0b, 0x01, 0x03, 0x01, "Hard"                           },
	{0x0b, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Ammo Magazines at Start"        },
	{0x0b, 0x01, 0x0c, 0x00, "4"                              },
	{0x0b, 0x01, 0x0c, 0x04, "5"                              },
	{0x0b, 0x01, 0x0c, 0x0c, "6"                              },
	{0x0b, 0x01, 0x0c, 0x08, "7"                              },
};

STDDIPINFO(Opwolfb);

static struct BurnDIPInfo RainbowDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xfe, NULL                             },
	{0x10, 0xff, 0xff, 0xbf, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                        },
	{0x0f, 0x01, 0x01, 0x00, "Upright"                        },
	{0x0f, 0x01, 0x01, 0x01, "Cocktail"                       },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"                    },
	{0x0f, 0x01, 0x02, 0x02, "Off"                            },
	{0x0f, 0x01, 0x02, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x0f, 0x01, 0x04, 0x04, "Off"                            },
	{0x0f, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x0f, 0x01, 0x08, 0x00, "Off"                            },
	{0x0f, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x0f, 0x01, 0x30, 0x10, "2 Coins 1 Credit"               },
	{0x0f, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },
	{0x0f, 0x01, 0x30, 0x00, "2 Coins 3 Credits"              },
	{0x0f, 0x01, 0x30, 0x20, "1 Coin  2 Credits"              },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x0f, 0x01, 0xc0, 0x40, "2 Coins 1 Credit"               },
	{0x0f, 0x01, 0xc0, 0xc0, "1 Coin  1 Credit"               },
	{0x0f, 0x01, 0xc0, 0x00, "2 Coins 3 Credits"              },
	{0x0f, 0x01, 0xc0, 0x80, "1 Coin  2 Credits"              },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x10, 0x01, 0x03, 0x02, "Easy"                           },
	{0x10, 0x01, 0x03, 0x03, "Medium"                         },
	{0x10, 0x01, 0x03, 0x01, "Hard"                           },
	{0x10, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"                     },
	{0x10, 0x01, 0x04, 0x04, "100k, 1000k"                    },
	{0x10, 0x01, 0x04, 0x00, "None"                           },
	
	{0   , 0xfe, 0   , 2   , "Complete Bonus"                 },
	{0x10, 0x01, 0x08, 0x08, "1 Up"                           },
	{0x10, 0x01, 0x08, 0x00, "100k"                           },
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x10, 0x01, 0x30, 0x10, "1"                              },
	{0x10, 0x01, 0x30, 0x00, "2"                              },
	{0x10, 0x01, 0x30, 0x30, "3"                              },
	{0x10, 0x01, 0x30, 0x20, "4"                              },
	
	{0   , 0xfe, 0   , 2   , "Language"                       },
	{0x10, 0x01, 0x40, 0x00, "English"                        },
	{0x10, 0x01, 0x40, 0x40, "Japanese"                       },
};

STDDIPINFO(Rainbow);

static struct BurnDIPInfo JumpingDIPList[]=
{
	// Default Values
	{0x09, 0xff, 0xff, 0xfe, NULL                             },
	{0x0a, 0xff, 0xff, 0xbf, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x09, 0x01, 0x08, 0x00, "Off"                            },
	{0x09, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x09, 0x01, 0x30, 0x10, "2 Coins 1 Credit"               },
	{0x09, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },
	{0x09, 0x01, 0x30, 0x00, "2 Coins 3 Credits"              },
	{0x09, 0x01, 0x30, 0x20, "1 Coin  2 Credits"              },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x09, 0x01, 0xc0, 0x40, "2 Coins 1 Credit"               },
	{0x09, 0x01, 0xc0, 0xc0, "1 Coin  1 Credit"               },
	{0x09, 0x01, 0xc0, 0x00, "2 Coins 3 Credits"              },
	{0x09, 0x01, 0xc0, 0x80, "1 Coin  2 Credits"              },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x0a, 0x01, 0x03, 0x02, "Easy"                           },
	{0x0a, 0x01, 0x03, 0x03, "Medium"                         },
	{0x0a, 0x01, 0x03, 0x01, "Hard"                           },
	{0x0a, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 2   , "Bonus Life"                     },
	{0x0a, 0x01, 0x04, 0x04, "100k, 1000k"                    },
	{0x0a, 0x01, 0x04, 0x00, "None"                           },
	
	{0   , 0xfe, 0   , 2   , "Complete Bonus"                 },
	{0x0a, 0x01, 0x08, 0x08, "1 Up"                           },
	{0x0a, 0x01, 0x08, 0x00, "100k"                           },
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x0a, 0x01, 0x30, 0x10, "1"                              },
	{0x0a, 0x01, 0x30, 0x00, "2"                              },
	{0x0a, 0x01, 0x30, 0x30, "3"                              },
	{0x0a, 0x01, 0x30, 0x20, "4"                              },
};

STDDIPINFO(Jumping);

static struct BurnDIPInfo RastanDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xfe, NULL                             },
	{0x14, 0xff, 0xff, 0xff, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                        },
	{0x13, 0x01, 0x01, 0x00, "Upright"                        },
	{0x13, 0x01, 0x01, 0x01, "Cocktail"                       },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"                    },
	{0x13, 0x01, 0x02, 0x02, "Off"                            },
	{0x13, 0x01, 0x02, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x13, 0x01, 0x04, 0x04, "Off"                            },
	{0x13, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x13, 0x01, 0x08, 0x00, "Off"                            },
	{0x13, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x13, 0x01, 0x30, 0x00, "4 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x10, "3 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x20, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x13, 0x01, 0xc0, 0xc0, "1 Coin 2 Credits"               },
	{0x13, 0x01, 0xc0, 0x80, "1 Coin 3 Credits"               },
	{0x13, 0x01, 0xc0, 0x40, "1 Coin 4 Credits"               },
	{0x13, 0x01, 0xc0, 0x00, "1 Coin 6 Credits"               },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x14, 0x01, 0x03, 0x02, "Easy"                           },
	{0x14, 0x01, 0x03, 0x03, "Medium"                         },
	{0x14, 0x01, 0x03, 0x01, "Hard"                           },
	{0x14, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"                     },
	{0x14, 0x01, 0x0c, 0x0c, "100k, 200k,  400k,  600k,  800k"},
	{0x14, 0x01, 0x0c, 0x08, "150k, 300k,  600k,  900k, 1200k"},
	{0x14, 0x01, 0x0c, 0x04, "200k, 400k,  800k, 1200k, 1600k"},
	{0x14, 0x01, 0x0c, 0x00, "250k, 500k, 1000k, 1500k, 2000k"},
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x14, 0x01, 0x30, 0x30, "3"                              },
	{0x14, 0x01, 0x30, 0x20, "4"                              },
	{0x14, 0x01, 0x30, 0x10, "5"                              },
	{0x14, 0x01, 0x30, 0x00, "6"                              },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x14, 0x01, 0x40, 0x00, "Off"                            },
	{0x14, 0x01, 0x40, 0x40, "On"                             },
};

STDDIPINFO(Rastan);

static struct BurnDIPInfo RastsagaDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xfe, NULL                             },
	{0x14, 0xff, 0xff, 0xff, NULL                             },
	
	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                        },
	{0x13, 0x01, 0x01, 0x00, "Upright"                        },
	{0x13, 0x01, 0x01, 0x01, "Cocktail"                       },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"                    },
	{0x13, 0x01, 0x02, 0x02, "Off"                            },
	{0x13, 0x01, 0x02, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"                   },
	{0x13, 0x01, 0x04, 0x04, "Off"                            },
	{0x13, 0x01, 0x04, 0x00, "On"                             },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                    },
	{0x13, 0x01, 0x08, 0x00, "Off"                            },
	{0x13, 0x01, 0x08, 0x08, "On"                             },
	
	{0   , 0xfe, 0   , 4   , "Coin A"                         },
	{0x13, 0x01, 0x30, 0x10, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0x30, 0x30, "1 Coin  1 Credit"               },
	{0x13, 0x01, 0x30, 0x00, "2 Coins 3 Credits"              },
	{0x13, 0x01, 0x30, 0x20, "1 Coin  2 Credits"              },

	{0   , 0xfe, 0   , 4   , "Coin B"                         },
	{0x13, 0x01, 0xc0, 0x40, "2 Coins 1 Credit"               },
	{0x13, 0x01, 0xc0, 0xc0, "1 Coin  1 Credit"               },
	{0x13, 0x01, 0xc0, 0x00, "2 Coins 3 Credits"              },
	{0x13, 0x01, 0xc0, 0x80, "1 Coin  2 Credits"              },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"                     },
	{0x14, 0x01, 0x03, 0x02, "Easy"                           },
	{0x14, 0x01, 0x03, 0x03, "Medium"                         },
	{0x14, 0x01, 0x03, 0x01, "Hard"                           },
	{0x14, 0x01, 0x03, 0x00, "Hardest"                        },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"                     },
	{0x14, 0x01, 0x0c, 0x0c, "100k, 200k,  400k,  600k,  800k"},
	{0x14, 0x01, 0x0c, 0x08, "150k, 300k,  600k,  900k, 1200k"},
	{0x14, 0x01, 0x0c, 0x04, "200k, 400k,  800k, 1200k, 1600k"},
	{0x14, 0x01, 0x0c, 0x00, "250k, 500k, 1000k, 1500k, 2000k"},
	
	{0   , 0xfe, 0   , 4   , "Lives"                          },
	{0x14, 0x01, 0x30, 0x30, "3"                              },
	{0x14, 0x01, 0x30, 0x20, "4"                              },
	{0x14, 0x01, 0x30, 0x10, "5"                              },
	{0x14, 0x01, 0x30, 0x00, "6"                              },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                 },
	{0x14, 0x01, 0x40, 0x00, "Off"                            },
	{0x14, 0x01, 0x40, 0x40, "On"                             },
};

STDDIPINFO(Rastsaga);

static struct BurnRomInfo DariusRomDesc[] = {
	{ "da-59.bin",     0x10000, 0x11aab4eb, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "da-58.bin",     0x10000, 0x5f71e697, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "da-61.bin",     0x10000, 0x4736aa9b, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "da-66.bin",     0x10000, 0x4ede5f56, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_31.187",    0x10000, 0xe9bb5d89, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_30.154",    0x10000, 0x9eb5e127, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	
	{ "a96_33-1.190",  0x10000, 0xff186048, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96_32-1.157",  0x10000, 0xd9719de8, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96_35-1.191",  0x10000, 0xb3280193, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96_34-1.158",  0x10000, 0xca3b2573, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },

	{ "a96_57.33",     0x10000, 0x33ceb730, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },
	
	{ "a96_56.18",     0x10000, 0x292ef55c, BRF_ESS | BRF_PRG | TAITO_Z80ROM2 },
	
	{ "a96_48.24",     0x10000, 0x39c9b3aa, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_51.47",     0x10000, 0x1bf8f0d3, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_49.25",     0x10000, 0x37a7d88a, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_52.48",     0x10000, 0x2d9b2128, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_50.26",     0x10000, 0x75d738e4, BRF_GRA | TAITO_CHARS_BYTESWAP },	
	{ "a96_53.49",     0x10000, 0x0173484c, BRF_GRA | TAITO_CHARS_BYTESWAP },
	
	{ "a96_54.143",    0x04000, 0x51c02ae2, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	{ "a96_55.144",    0x04000, 0x771e4d98, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	
	{ "a96_44.179",    0x10000, 0xbbc18878, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_45.200",    0x10000, 0x616cdd8b, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_46.180",    0x10000, 0xfec35418, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_47.201",    0x10000, 0x8df9286a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_40.177",    0x10000, 0xb699a51e, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_41.198",    0x10000, 0x97128a3a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_42.178",    0x10000, 0x7f55ee0f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_43.199",    0x10000, 0xc7cad469, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "da-62.bin",     0x10000, 0x9179862c, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "da-63.bin",     0x10000, 0xfa19cfff, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "da-64.bin",     0x10000, 0x814c676f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "da-65.bin",     0x10000, 0x14eee326, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	
	{ "a96-24.163",    0x00400, 0x0fa8be7f, BRF_OPT },
	{ "a96-25.164",    0x00400, 0x265508a6, BRF_OPT },
	{ "a96-26.165",    0x00400, 0x4891b9c0, BRF_OPT },
};

STD_ROM_PICK(Darius);
STD_ROM_FN(Darius);

static struct BurnRomInfo DariusjRomDesc[] = {
	{ "a96_29-1.185",  0x10000, 0x75486f62, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_28-1.152",  0x10000, 0xfb34d400, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_31.187",    0x10000, 0xe9bb5d89, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_30.154",    0x10000, 0x9eb5e127, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	
	{ "a96_33-1.190",  0x10000, 0xff186048, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96_32-1.157",  0x10000, 0xd9719de8, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96_35-1.191",  0x10000, 0xb3280193, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96_34-1.158",  0x10000, 0xca3b2573, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },

	{ "a96_57.33",     0x10000, 0x33ceb730, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },
	
	{ "a96_56.18",     0x10000, 0x292ef55c, BRF_ESS | BRF_PRG | TAITO_Z80ROM2 },
	
	{ "a96_48.24",     0x10000, 0x39c9b3aa, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_51.47",     0x10000, 0x1bf8f0d3, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_49.25",     0x10000, 0x37a7d88a, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_52.48",     0x10000, 0x2d9b2128, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_50.26",     0x10000, 0x75d738e4, BRF_GRA | TAITO_CHARS_BYTESWAP },	
	{ "a96_53.49",     0x10000, 0x0173484c, BRF_GRA | TAITO_CHARS_BYTESWAP },
	
	{ "a96_54.143",    0x04000, 0x51c02ae2, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	{ "a96_55.144",    0x04000, 0x771e4d98, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	
	{ "a96_44.179",    0x10000, 0xbbc18878, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_45.200",    0x10000, 0x616cdd8b, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_46.180",    0x10000, 0xfec35418, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_47.201",    0x10000, 0x8df9286a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_40.177",    0x10000, 0xb699a51e, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_41.198",    0x10000, 0x97128a3a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_42.178",    0x10000, 0x7f55ee0f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_43.199",    0x10000, 0xc7cad469, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_36.175",    0x10000, 0xaf598141, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_37.196",    0x10000, 0xb48137c8, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_38.176",    0x10000, 0xe4f3e3a7, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_39.197",    0x10000, 0xea30920f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	
	{ "a96-24.163",    0x00400, 0x0fa8be7f, BRF_OPT },
	{ "a96-25.164",    0x00400, 0x265508a6, BRF_OPT },
	{ "a96-26.165",    0x00400, 0x4891b9c0, BRF_OPT },
};

STD_ROM_PICK(Dariusj);
STD_ROM_FN(Dariusj);

static struct BurnRomInfo DariusoRomDesc[] = {
	{ "a96-29.185",    0x10000, 0xf775162b, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96-28.152",    0x10000, 0x4721d667, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_31.187",    0x10000, 0xe9bb5d89, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_30.154",    0x10000, 0x9eb5e127, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	
	{ "a96-33.190",    0x10000, 0xd2f340d2, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96-32.157",    0x10000, 0x044c9848, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96-35.191",    0x10000, 0xb8ed718b, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "a96-34.158",    0x10000, 0x7556a660, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },

	{ "a96_57.33",     0x10000, 0x33ceb730, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },
	
	{ "a96_56.18",     0x10000, 0x292ef55c, BRF_ESS | BRF_PRG | TAITO_Z80ROM2 },
	
	{ "a96_48.24",     0x10000, 0x39c9b3aa, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_51.47",     0x10000, 0x1bf8f0d3, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_49.25",     0x10000, 0x37a7d88a, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_52.48",     0x10000, 0x2d9b2128, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_50.26",     0x10000, 0x75d738e4, BRF_GRA | TAITO_CHARS_BYTESWAP },	
	{ "a96_53.49",     0x10000, 0x0173484c, BRF_GRA | TAITO_CHARS_BYTESWAP },
	
	{ "a96_54.143",    0x04000, 0x51c02ae2, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	{ "a96_55.144",    0x04000, 0x771e4d98, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	
	{ "a96_44.179",    0x10000, 0xbbc18878, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_45.200",    0x10000, 0x616cdd8b, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_46.180",    0x10000, 0xfec35418, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_47.201",    0x10000, 0x8df9286a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_40.177",    0x10000, 0xb699a51e, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_41.198",    0x10000, 0x97128a3a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_42.178",    0x10000, 0x7f55ee0f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_43.199",    0x10000, 0xc7cad469, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_36.175",    0x10000, 0xaf598141, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_37.196",    0x10000, 0xb48137c8, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_38.176",    0x10000, 0xe4f3e3a7, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_39.197",    0x10000, 0xea30920f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	
	{ "a96-24.163",    0x00400, 0x0fa8be7f, BRF_OPT },
	{ "a96-25.164",    0x00400, 0x265508a6, BRF_OPT },
	{ "a96-26.165",    0x00400, 0x4891b9c0, BRF_OPT },
};

STD_ROM_PICK(Dariuso);
STD_ROM_FN(Dariuso);

static struct BurnRomInfo DariuseRomDesc[] = {
	{ "dae-68.bin",    0x10000, 0xed721127, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "dae-67.bin",    0x10000, 0xb99aea8c, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "dae-70.bin",    0x10000, 0x54590b31, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "a96_30.154",    0x10000, 0x9eb5e127, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	
	{ "dae-72.bin",    0x10000, 0x248ca2cc, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "dae-71.bin",    0x10000, 0x65dd0403, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "dae-74.bin",    0x10000, 0x0ea31f60, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },
	{ "dae-73.bin",    0x10000, 0x27036a4d, BRF_ESS | BRF_PRG | TAITO_68KROM2_BYTESWAP },

	{ "a96_57.33",     0x10000, 0x33ceb730, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },
	
	{ "a96_56.18",     0x10000, 0x292ef55c, BRF_ESS | BRF_PRG | TAITO_Z80ROM2 },
	
	{ "a96_48.24",     0x10000, 0x39c9b3aa, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_51.47",     0x10000, 0x1bf8f0d3, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_49.25",     0x10000, 0x37a7d88a, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_52.48",     0x10000, 0x2d9b2128, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "a96_50.26",     0x10000, 0x75d738e4, BRF_GRA | TAITO_CHARS_BYTESWAP },	
	{ "a96_53.49",     0x10000, 0x0173484c, BRF_GRA | TAITO_CHARS_BYTESWAP },
	
	{ "a96_54.143",    0x04000, 0x51c02ae2, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	{ "a96_55.144",    0x04000, 0x771e4d98, BRF_GRA | TAITO_CHARSB_BYTESWAP },
	
	{ "a96_44.179",    0x10000, 0xbbc18878, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_45.200",    0x10000, 0x616cdd8b, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_46.180",    0x10000, 0xfec35418, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_47.201",    0x10000, 0x8df9286a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_40.177",    0x10000, 0xb699a51e, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_41.198",    0x10000, 0x97128a3a, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_42.178",    0x10000, 0x7f55ee0f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_43.199",    0x10000, 0xc7cad469, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_36.175",    0x10000, 0xaf598141, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_37.196",    0x10000, 0xb48137c8, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_38.176",    0x10000, 0xe4f3e3a7, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	{ "a96_39.197",    0x10000, 0xea30920f, BRF_GRA | TAITO_SPRITESA_BYTESWAP32 },
	
	{ "a96-24.163",    0x00400, 0x0fa8be7f, BRF_OPT },
	{ "a96-25.164",    0x00400, 0x265508a6, BRF_OPT },
	{ "a96-26.165",    0x00400, 0x4891b9c0, BRF_OPT },
};

STD_ROM_PICK(Dariuse);
STD_ROM_FN(Dariuse);

static struct BurnRomInfo OpwolfRomDesc[] = {
	{ "b20-05-02.40",  0x10000, 0x3ffbfe3a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-03-02.30",  0x10000, 0xfdabd8a5, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-04.39",     0x10000, 0x216b4838, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-20.29",     0x10000, 0xd244431a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b20-07.10",     0x10000, 0x45c7ace3, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b20-13.13",     0x80000, 0xf6acdab1, BRF_GRA | TAITO_CHARS },
	
	{ "b20-14.72",     0x80000, 0x89f889e5, BRF_GRA | TAITO_SPRITESA },

	{ "b20-08.21",     0x80000, 0xf3e19c64, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Opwolf);
STD_ROM_FN(Opwolf);

static struct BurnRomInfo OpwolfaRomDesc[] = {
	{ "b20-05-02.40",  0x10000, 0x3ffbfe3a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-03-02.30",  0x10000, 0xfdabd8a5, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-04.39",     0x10000, 0x216b4838, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-17",        0x10000, 0x6043188e, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b20-07.10",     0x10000, 0x45c7ace3, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b20-13.13",     0x80000, 0xf6acdab1, BRF_GRA | TAITO_CHARS },
	
	{ "b20-14.72",     0x80000, 0x89f889e5, BRF_GRA | TAITO_SPRITESA },

	{ "b20-08.21",     0x80000, 0xf3e19c64, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Opwolfa);
STD_ROM_FN(Opwolfa);

static struct BurnRomInfo OpwolfuRomDesc[] = {
	{ "b20-05-02.40",  0x10000, 0x3ffbfe3a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-03-02.30",  0x10000, 0xfdabd8a5, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b20-04.39",     0x10000, 0x216b4838, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "opwlf.29",      0x10000, 0xb71bc44c, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b20-07.10",     0x10000, 0x45c7ace3, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b20-13.13",     0x80000, 0xf6acdab1, BRF_GRA | TAITO_CHARS },
	
	{ "b20-14.72",     0x80000, 0x89f889e5, BRF_GRA | TAITO_SPRITESA },

	{ "b20-08.21",     0x80000, 0xf3e19c64, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Opwolfu);
STD_ROM_FN(Opwolfu);

static struct BurnRomInfo OpwolfbRomDesc[] = {
	{ "opwlfb.12",     0x10000, 0xd87e4405, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "opwlfb.10",     0x10000, 0x9ab6f75c, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "opwlfb.13",     0x10000, 0x61230c6e, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "opwlfb.11",     0x10000, 0x342e318d, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "opwlfb.30",     0x08000, 0x0669b94c, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "opwlfb.09",     0x08000, 0xab27a3dd, BRF_ESS | BRF_PRG | TAITO_Z80ROM2 },

	{ "opwlfb.08",     0x10000, 0x134d294e, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "opwlfb.04",     0x10000, 0xde0ca98d, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "opwlfb.06",     0x10000, 0x317d0e66, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "opwlfb.02",     0x10000, 0x6231fdd0, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "opwlfb.07",     0x10000, 0xe1c4095e, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "opwlfb.03",     0x10000, 0xccf8ba80, BRF_GRA | TAITO_CHARS_BYTESWAP },
	{ "opwlfb.05",     0x10000, 0xfd9e72c8, BRF_GRA | TAITO_CHARS_BYTESWAP },	
	{ "opwlfb.01",     0x10000, 0x0a65f256, BRF_GRA | TAITO_CHARS_BYTESWAP },

	{ "opwlfb.14",     0x10000, 0x663786eb, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "opwlfb.18",     0x10000, 0xde9ab08e, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "opwlfb.15",     0x10000, 0x315b8aa9, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "opwlfb.19",     0x10000, 0x645cf85e, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "opwlfb.16",     0x10000, 0xe01099e3, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "opwlfb.20",     0x10000, 0xd80b9cc6, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "opwlfb.17",     0x10000, 0x56fbe61d, BRF_GRA | TAITO_SPRITESA_BYTESWAP },	
	{ "opwlfb.21",     0x10000, 0x97d25157, BRF_GRA | TAITO_SPRITESA_BYTESWAP },

	{ "opwlfb.29",     0x10000, 0x05a9eac0, BRF_SND | TAITO_MSM5205_BYTESWAP },
	{ "opwlfb.25",     0x10000, 0x85b87f58, BRF_SND | TAITO_MSM5205_BYTESWAP },
	{ "opwlfb.28",     0x10000, 0x281b2175, BRF_SND | TAITO_MSM5205_BYTESWAP },
	{ "opwlfb.24",     0x10000, 0x8efc5d4d, BRF_SND | TAITO_MSM5205_BYTESWAP },
	{ "opwlfb.27",     0x10000, 0x441211a6, BRF_SND | TAITO_MSM5205_BYTESWAP },
	{ "opwlfb.23",     0x10000, 0xa874c703, BRF_SND | TAITO_MSM5205_BYTESWAP },
	{ "opwlfb.26",     0x10000, 0x86d1d42d, BRF_SND | TAITO_MSM5205_BYTESWAP },	
	{ "opwlfb.22",     0x10000, 0x9228481f, BRF_SND | TAITO_MSM5205_BYTESWAP },

};

STD_ROM_PICK(Opwolfb);
STD_ROM_FN(Opwolfb);

static struct BurnRomInfo RainbowRomDesc[] = {
	{ "b22-10-1.19",   0x10000, 0xe34a50ca, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-11-1.20",   0x10000, 0x6a31a093, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-08-1.21",   0x10000, 0x15d6e17a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-09-1.22",   0x10000, 0x454e66bc, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-03.23",     0x20000, 0x3ebb0fb8, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-04.24",     0x20000, 0x91625e7f, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b22-14.43",     0x10000, 0x113c1a5b, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b22-01.2",      0x80000, 0xb76c9168, BRF_GRA | TAITO_CHARS },

	{ "b22-01.5",      0x80000, 0x1b87ecf0, BRF_GRA | TAITO_SPRITESA },
	{ "b22-12.7",      0x10000, 0x67a76dc6, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "b22-13.6",      0x10000, 0x2fda099f, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
};

STD_ROM_PICK(Rainbow);
STD_ROM_FN(Rainbow);

static struct BurnRomInfo RainbowoRomDesc[] = {
	{ "b22-10.19",     0x10000, 0x3b013495, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-11.20",     0x10000, 0x80041a3d, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-08.21",     0x10000, 0x962fb845, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-09.22",     0x10000, 0xf43efa27, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-03.23",     0x20000, 0x3ebb0fb8, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-04.24",     0x20000, 0x91625e7f, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b22-14.43",     0x10000, 0x113c1a5b, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b22-01.2",      0x80000, 0xb76c9168, BRF_GRA | TAITO_CHARS },

	{ "b22-01.5",      0x80000, 0x1b87ecf0, BRF_GRA | TAITO_SPRITESA },
	{ "b22-12.7",      0x10000, 0x67a76dc6, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "b22-13.6",      0x10000, 0x2fda099f, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
};

STD_ROM_PICK(Rainbowo);
STD_ROM_FN(Rainbowo);

static struct BurnRomInfo RainboweRomDesc[] = {
	{ "b39-01.19",     0x10000, 0x50690880, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b39-02.20",     0x10000, 0x4dead71f, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b39-03.21",     0x10000, 0x4a4cb785, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b39-04.22",     0x10000, 0x4caa53bd, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-03.23",     0x20000, 0x3ebb0fb8, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-04.24",     0x20000, 0x91625e7f, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b22-14.43",     0x10000, 0x113c1a5b, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b22-01.2",      0x80000, 0xb76c9168, BRF_GRA | TAITO_CHARS },

	{ "b22-01.5",      0x80000, 0x1b87ecf0, BRF_GRA | TAITO_SPRITESA },
	{ "b22-12.7",      0x10000, 0x67a76dc6, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
	{ "b22-13.6",      0x10000, 0x2fda099f, BRF_GRA | TAITO_SPRITESA_BYTESWAP },
};

STD_ROM_PICK(Rainbowe);
STD_ROM_FN(Rainbowe);

static struct BurnRomInfo JumpingRomDesc[] = {
	{ "jb1_h4",        0x10000, 0x3fab6b31, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "jb1_h8",        0x10000, 0x8c878827, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "jb1_i4",        0x10000, 0x443492cf, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "jb1_i8",        0x10000, 0xed33bae1, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-03.23",     0x20000, 0x3ebb0fb8, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b22-04.24",     0x20000, 0x91625e7f, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "jb1_f89",       0x10000, 0x0810d327, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP_JUMPING },

	{ "jb1_cd67",      0x10000, 0x8527c00e, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "jb2_ic8",       0x10000, 0x65b76309, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic7",       0x10000, 0x43a94283, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic10",      0x10000, 0xe61933fb, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic9",       0x10000, 0xed031eb2, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic12",      0x10000, 0x312700ca, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic11",      0x10000, 0xde3b0b88, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic14",      0x10000, 0x9fdc6c8e, BRF_GRA | TAITO_CHARS },
	{ "jb2_ic13",      0x10000, 0x06226492, BRF_GRA | TAITO_CHARS },

	{ "jb2_ic62",      0x10000, 0x8548db6c, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic61",      0x10000, 0x37c5923b, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic60",      0x08000, 0x662a2f1e, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic78",      0x10000, 0x925865e1, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic77",      0x10000, 0xb09695d1, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic76",      0x08000, 0x41937743, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic93",      0x10000, 0xf644eeab, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic92",      0x10000, 0x3fbccd33, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_ic91",      0x08000, 0xd886c014, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_i121",      0x10000, 0x93df1e4d, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_i120",      0x10000, 0x7c4e893b, BRF_GRA | TAITO_SPRITESA },
	{ "jb2_i119",      0x08000, 0x7e1d58d8, BRF_GRA | TAITO_SPRITESA },
};

STD_ROM_PICK(Jumping);
STD_ROM_FN(Jumping);

static struct BurnRomInfo RastanRomDesc[] = {
	{ "b04-35.19",   0x10000, 0x1c91dbb1, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-37.07",   0x10000, 0xecf20bdd, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-40.20",   0x10000, 0x0930d4b3, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-39.08",   0x10000, 0xd95ade5e, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-42.21",   0x10000, 0x1857a7cb, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-43.09",   0x10000, 0xc34b9152, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b04-19.49",   0x10000, 0xee81fdd8, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b04-01.40",   0x20000, 0xcd30de19, BRF_GRA | TAITO_CHARS },
	{ "b04-03.39",   0x20000, 0xab67e064, BRF_GRA | TAITO_CHARS },
	{ "b04-02.67",   0x20000, 0x54040fec, BRF_GRA | TAITO_CHARS },
	{ "b04-04.66",   0x20000, 0x94737e93, BRF_GRA | TAITO_CHARS },

	{ "b04-05.15",   0x20000, 0xc22d94ac, BRF_GRA | TAITO_SPRITESA },
	{ "b04-07.14",   0x20000, 0xb5632a51, BRF_GRA | TAITO_SPRITESA },
	{ "b04-06.28",   0x20000, 0x002ccf39, BRF_GRA | TAITO_SPRITESA },
	{ "b04-08.27",   0x20000, 0xfeafca05, BRF_GRA | TAITO_SPRITESA },

	{ "b04-20.76",   0x10000, 0xfd1a34cc, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Rastan);
STD_ROM_FN(Rastan);

static struct BurnRomInfo RastanuRomDesc[] = {
	{ "b04-35.19",   0x10000, 0x1c91dbb1, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-37.07",   0x10000, 0xecf20bdd, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-45.20",   0x10000, 0x362812dd, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-44.08",   0x10000, 0x51cc5508, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-42.21",   0x10000, 0x1857a7cb, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-41-1.09", 0x10000, 0xbd403269, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b04-19.49",   0x10000, 0xee81fdd8, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b04-01.40",   0x20000, 0xcd30de19, BRF_GRA | TAITO_CHARS },
	{ "b04-03.39",   0x20000, 0xab67e064, BRF_GRA | TAITO_CHARS },
	{ "b04-02.67",   0x20000, 0x54040fec, BRF_GRA | TAITO_CHARS },
	{ "b04-04.66",   0x20000, 0x94737e93, BRF_GRA | TAITO_CHARS },

	{ "b04-05.15",   0x20000, 0xc22d94ac, BRF_GRA | TAITO_SPRITESA },
	{ "b04-07.14",   0x20000, 0xb5632a51, BRF_GRA | TAITO_SPRITESA },
	{ "b04-06.28",   0x20000, 0x002ccf39, BRF_GRA | TAITO_SPRITESA },
	{ "b04-08.27",   0x20000, 0xfeafca05, BRF_GRA | TAITO_SPRITESA },

	{ "b04-20.76",   0x10000, 0xfd1a34cc, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Rastanu);
STD_ROM_FN(Rastanu);

static struct BurnRomInfo Rastanu2RomDesc[] = {
	{ "rs19_38.bin", 0x10000, 0xa38ac909, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-21.7",    0x10000, 0x7c8dde9a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-23.20",   0x10000, 0x254b3dce, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-22.8",    0x10000, 0x98e8edcf, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-25.21",   0x10000, 0xd1e5adee, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-24.9",    0x10000, 0xa3dcc106, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b04-19.49",   0x10000, 0xee81fdd8, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b04-01.40",   0x20000, 0xcd30de19, BRF_GRA | TAITO_CHARS },
	{ "b04-03.39",   0x20000, 0xab67e064, BRF_GRA | TAITO_CHARS },
	{ "b04-02.67",   0x20000, 0x54040fec, BRF_GRA | TAITO_CHARS },
	{ "b04-04.66",   0x20000, 0x94737e93, BRF_GRA | TAITO_CHARS },

	{ "b04-05.15",   0x20000, 0xc22d94ac, BRF_GRA | TAITO_SPRITESA },
	{ "b04-07.14",   0x20000, 0xb5632a51, BRF_GRA | TAITO_SPRITESA },
	{ "b04-06.28",   0x20000, 0x002ccf39, BRF_GRA | TAITO_SPRITESA },
	{ "b04-08.27",   0x20000, 0xfeafca05, BRF_GRA | TAITO_SPRITESA },

	{ "b04-20.76",   0x10000, 0xfd1a34cc, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Rastanu2);
STD_ROM_FN(Rastanu2);

static struct BurnRomInfo RastsagaRomDesc[] = {
	{ "b04-38.19",   0x10000, 0xa38ac909, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-37.7",    0x10000, 0xbad60872, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-40.20",   0x10000, 0x6bcf70dc, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-39.8",    0x10000, 0x8838ecc5, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-42.21",   0x10000, 0xb626c439, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-43.9",    0x10000, 0xc928a516, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b04-19.49",   0x10000, 0xee81fdd8, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b04-01.40",   0x20000, 0xcd30de19, BRF_GRA | TAITO_CHARS },
	{ "b04-03.39",   0x20000, 0xab67e064, BRF_GRA | TAITO_CHARS },
	{ "b04-02.67",   0x20000, 0x54040fec, BRF_GRA | TAITO_CHARS },
	{ "b04-04.66",   0x20000, 0x94737e93, BRF_GRA | TAITO_CHARS },

	{ "b04-05.15",   0x20000, 0xc22d94ac, BRF_GRA | TAITO_SPRITESA },
	{ "b04-07.14",   0x20000, 0xb5632a51, BRF_GRA | TAITO_SPRITESA },
	{ "b04-06.28",   0x20000, 0x002ccf39, BRF_GRA | TAITO_SPRITESA },
	{ "b04-08.27",   0x20000, 0xfeafca05, BRF_GRA | TAITO_SPRITESA },

	{ "b04-20.76",   0x10000, 0xfd1a34cc, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Rastsaga);
STD_ROM_FN(Rastsaga);

static struct BurnRomInfo Rastsag1RomDesc[] = {
	{ "b04-14.19",   0x10000, 0xa38ac909, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-13.7",    0x10000, 0xbad60872, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-16-1.20", 0x10000, 0x00b59e60, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-15-1.8",  0x10000, 0xff9e018a, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-18-1.21", 0x10000, 0xb626c439, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },
	{ "b04-17-1.9",  0x10000, 0xc928a516, BRF_ESS | BRF_PRG | TAITO_68KROM1_BYTESWAP },

	{ "b04-19.49",   0x10000, 0xee81fdd8, BRF_ESS | BRF_PRG | TAITO_Z80ROM1 },

	{ "b04-01.40",   0x20000, 0xcd30de19, BRF_GRA | TAITO_CHARS },
	{ "b04-03.39",   0x20000, 0xab67e064, BRF_GRA | TAITO_CHARS },
	{ "b04-02.67",   0x20000, 0x54040fec, BRF_GRA | TAITO_CHARS },
	{ "b04-04.66",   0x20000, 0x94737e93, BRF_GRA | TAITO_CHARS },

	{ "b04-05.15",   0x20000, 0xc22d94ac, BRF_GRA | TAITO_SPRITESA },
	{ "b04-07.14",   0x20000, 0xb5632a51, BRF_GRA | TAITO_SPRITESA },
	{ "b04-06.28",   0x20000, 0x002ccf39, BRF_GRA | TAITO_SPRITESA },
	{ "b04-08.27",   0x20000, 0xfeafca05, BRF_GRA | TAITO_SPRITESA },

	{ "b04-20.76",   0x10000, 0xfd1a34cc, BRF_SND | TAITO_MSM5205 },
};

STD_ROM_PICK(Rastsag1);
STD_ROM_FN(Rastsag1);

static int MemIndex()
{
	unsigned char *Next; Next = TaitoMem;

	Taito68KRom1                        = Next; Next += Taito68KRom1Size;
	Taito68KRom2                        = Next; Next += Taito68KRom2Size;
	TaitoZ80Rom1                        = Next; Next += TaitoZ80Rom1Size;
	TaitoZ80Rom2                        = Next; Next += TaitoZ80Rom2Size;
	MSM5205ROM                          = Next; Next += TaitoMSM5205RomSize;
	
	TaitoRamStart                       = Next;

	Taito68KRam1                        = Next; Next += 0x018000;
	TaitoZ80Ram1                        = Next; Next += 0x001000;
	if (TaitoNumZ80s == 2) TaitoZ80Ram2 = Next; Next += 0x000800;
	TaitoPaletteRam                     = Next; Next += 0x001000;
	TaitoSpriteRam                      = Next; Next += 0x00f000;
	TaitoSharedRam                      = Next; Next += 0x002000;
	TaitoVideoRam                       = Next; Next += 0x008000;
	Taito68KRam2                        = Next; Next += 0x010000;
		
	TaitoRamEnd                         = Next;

	TaitoChars                          = Next; Next += TaitoNumChar * TaitoCharWidth * TaitoCharHeight;
	TaitoCharsB                         = Next; Next += TaitoNumCharB * TaitoCharBWidth * TaitoCharBHeight;
	TaitoSpritesA                       = Next; Next += TaitoNumSpriteA * TaitoSpriteAWidth * TaitoSpriteAHeight;
	TaitoPalette                        = (unsigned int*)Next; Next += 0x02000 * sizeof(unsigned int);

	TaitoMemEnd                         = Next;

	return 0;
}

static int DariusDoReset()
{
	TaitoDoReset();
	
	DariusADPCMCommand = 0;
	DariusNmiEnable = 0;
	DariusCoinWord = 0;
	
	return 0;
}

static int OpwolfDoReset()
{
	TaitoDoReset();
	
	memset(OpwolfADPCM_B, 0, 8);
	memset(OpwolfADPCM_C, 0, 8);
	
	return 0;
}

static void TaitoMiscCpuAReset(UINT16 d)
{
	TaitoCpuACtrl = d;
	if (!(TaitoCpuACtrl & 1)) {
		SekClose();
		SekOpen(1);
		SekReset();
		SekClose();
		SekOpen(0);
	}
}

unsigned char __fastcall Darius68K1ReadByte(unsigned int a)
{
	switch (a) {
		case 0xc00010: {
			return TaitoDip[1];
		}
		
		case 0xc00011: {
			return TaitoDip[0];
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K #1 Read byte => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Darius68K1WriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K #1 Write byte => %06X, %02X\n"), a, d);
		}
	}
}

unsigned short __fastcall Darius68K1ReadWord(unsigned int a)
{
	switch (a) {
		case 0xc00002: {
			return TC0140SYTCommRead();
		}
		
		case 0xc00008: {
			return TaitoInput[0];
		}
		
		case 0xc0000a: {
			return TaitoInput[1];
		}
		
		case 0xc0000c: {
			return TaitoInput[2];
		}
		
		case 0xc0000e: {
			return DariusCoinWord;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K #1 Read word => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Darius68K1WriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x0a0000: {
			TaitoMiscCpuAReset(d);
			return;
		}
		
		case 0x0b0000: {
			//watchdog
			return;
		}
		
		case 0xc00000: {
			TC0140SYTPortWrite(d & 0xff);
			return;
		}
		
		case 0xc00002: {
			TC0140SYTCommWrite(d & 0xff);
			return;
		}
		
		case 0xc00020:
		case 0xc00022:
		case 0xc00024:
		case 0xc00030:
		case 0xc00032:
		case 0xc00034: {
			//misc io writes
			return;
		}
		
		case 0xc00050: {
			//nop
			return;
		}
		
		case 0xc00060: {
			DariusCoinWord = d;
			return;
		}
		
		case 0xd20000:
		case 0xd20002: {
			PC080SNSetScrollY((a - 0xd20000) >> 1, d);
			return;
		}
		
		case 0xd40000:
		case 0xd40002: {
			PC080SNSetScrollX((a - 0xd40000) >> 1, d);
			return;
		}
		
		case 0xd50000: {
			PC080SNCtrlWrite((a - 0xd50000) >> 1, d);
			return;
		}
		
		case 0xdc0000: {
			//???
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K #1 Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned char __fastcall Darius68K2ReadByte(unsigned int a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K #2 Read byte => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Darius68K2WriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K #2 Write byte => %06X, %02X\n"), a, d);
		}
	}
}

unsigned short __fastcall Darius68K2ReadWord(unsigned int a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("68K #2 Read word => %06X\n"), a);
		}
	}
	
	return 0;
}

void __fastcall Darius68K2WriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0xc00050: {
			//nop
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("68K #2 Write word => %06X, %04X\n"), a, d);
		}
	}
}

unsigned char __fastcall Opwolf68KReadByte(unsigned int a)
{
	if (a >= 0x0ff000 && a <= 0x0ff7ff) {
		return OpwolfCChipDataRead((a - 0x0ff000) >> 1);
	}
	
	switch (a) {
		case 0x3e0002: {
			return TC0140SYTCommRead();
		}
	}
	
	return 0;
}

void __fastcall Opwolf68KWriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x0ff000 && a <= 0x0ff7ff) {
		OpwolfCChipDataWrite(Taito68KRom1, (a - 0x0ff000) >> 1, d);
		return;
	}

	switch (a) {
		case 0x3e0000: {
			TC0140SYTPortWrite(d);
			return;
		}
		
		case 0x3e0002: {
			TC0140SYTCommWrite(d);
			return;
		}
	}
}

unsigned short __fastcall Opwolf68KReadWord(unsigned int a)
{
	if (a >= 0x0f0000 && a <= 0x0f07ff) {
		return OpwolfCChipDataRead((a - 0x0f0000) >> 1);
	}
	
	if (a >= 0x0ff000 && a <= 0x0ff7ff) {
		return OpwolfCChipDataRead((a - 0x0ff000) >> 1);
	}
	
	switch (a) {
		case 0x3a0000: {
			int scaled = (BurnGunReturnX(0) * 320) / 256;
			return scaled + 0x15 + OpWolfGunXOffset;
		}
		
		case 0x3a0002: {
			return BurnGunReturnY(0) - 0x24 + OpWolfGunYOffset;
		}
		
		case 0x0ff802: {
			return OpwolfCChipStatusRead();
		}
	
		case 0x380000: {
			return TaitoDip[0];
		}
		
		case 0x380002: {
			return TaitoDip[1];
		}
	}
	
	return 0;
}

void __fastcall Opwolf68KWriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x0ff000 && a <= 0x0ff7ff) {
		OpwolfCChipDataWrite(Taito68KRom1, (a - 0x0ff000) >> 1, d);
		return;
	}
	
	switch (a) {
		case 0x0ff802: {
			OpwolfCChipStatusWrite();
			return;
		}
		
		case 0x0ffc00: {
			OpwolfCChipBankWrite(d);
			return;
		}
		
		case 0x380000: {
			PC090OJSpriteCtrl = (d & 0xe0) >> 5;
			return;
		}
		
		case 0x3c0000: {
			// nop
			return;
		}
		
		case 0xc20000:
		case 0xc20002: {
			PC080SNSetScrollY((a - 0xc20000) >> 1, d);
			return;
		}
		
		case 0xc40000:
		case 0xc40002: {
			PC080SNSetScrollX((a - 0xc40000) >> 1, d);
			return;
		}
		
		case 0xc50000: {
			PC080SNCtrlWrite((a - 0xc50000) >> 1, d);
			return;
		}
	}
}

unsigned char __fastcall Opwolfb68KReadByte(unsigned int a)
{
	if (a >= 0x0ff000 && a <= 0x0fffff) {
		return TaitoZ80Ram2[(a - 0x0ff000) >> 1];
	}
	
	switch (a) {
		case 0x3e0002: {
			return TC0140SYTCommRead();
		}
	}
	
	return 0;
}

void __fastcall Opwolfb68KWriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x0ff000 && a <= 0x0fffff) {
		TaitoZ80Ram2[(a - 0x0ff000) >> 1] = d ;
		return;
	}
	
	switch (a) {
		case 0x3e0000: {
			TC0140SYTPortWrite(d);
			return;
		}
		
		case 0x3e0002: {
			TC0140SYTCommWrite(d);
			return;
		}
	}
}

unsigned short __fastcall Opwolfb68KReadWord(unsigned int a)
{
	if (a >= 0x0ff000 && a <= 0x0fffff) {
		return TaitoZ80Ram2[(a - 0x0ff000) >> 1];
	}
	
	switch (a) {
		case 0x0f0008: {
			return TaitoInput[0];
		}
		
		case 0x0f000a: {
			return TaitoInput[1];
		}
		
		case 0x380000: {
			return TaitoDip[0];
		}
		
		case 0x380002: {
			return TaitoDip[1];
		}
		
		case 0x3a0000: {
			int scaled = (BurnGunReturnX(0) * 320) / 256;
			return scaled + 0x15 + OpWolfGunXOffset;
		}
		
		case 0x3a0002: {
			return BurnGunReturnY(0) - 0x24 + OpWolfGunYOffset;
		}
	}
	
	return 0;
}

void __fastcall Opwolfb68KWriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x0ff000 && a <= 0x0fffff) {
		TaitoZ80Ram2[(a - 0x0ff000) >> 1] = d & 0xff;
		return;
	}
	
	switch (a) {
		case 0x380000: {
			PC090OJSpriteCtrl = (d & 0xe0) >> 5;
			return;
		}
		
		case 0x3c0000: {
			// nop
			return;
		}
		
		case 0xc20000:
		case 0xc20002: {
			PC080SNSetScrollY((a - 0xc20000) >> 1, d);
			return;
		}
		
		case 0xc40000:
		case 0xc40002: {
			PC080SNSetScrollX((a - 0xc40000) >> 1, d);
			return;
		}
		
		case 0xc50000: {
			PC080SNCtrlWrite((a - 0xc50000) >> 1, d);
			return;
		}
	}
}

unsigned char __fastcall Rainbow68KReadByte(unsigned int a)
{
	if (a >= 0x800000 && a <= 0x8007ff) {
		return RainbowCChipRamRead((a - 0x800000) >> 1);
	}
	
	switch (a) {
		case 0x800803: {
			return RainbowCChipCtrlRead();
		}
	}
	
	return 0;
}

void __fastcall Rainbow68KWriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x800000 && a <= 0x8007ff) {
		RainbowCChipRamWrite((a - 0x800000) >> 1, d);
		return;
	}
	
	switch (a) {
		case 0x3a0001: {
			PC090OJSpriteCtrl = (d & 0xe0) >> 5;
			return;
		}
		
		case 0x3e0001: {
			TC0140SYTPortWrite(d);
			return;
		}
		
		case 0x3e0003: {
			TC0140SYTCommWrite(d);
			return;
		}
		
		case 0x800803: {
			RainbowCChipCtrlWrite(d);
			return;
		}
		
		case 0x800c01: {
			RainbowCChipBankWrite(d);
			return;
		}
	}
}

unsigned short __fastcall Rainbow68KReadWord(unsigned int a)
{
	if (a >= 0x800000 && a <= 0x8007ff) {
		return RainbowCChipRamRead((a - 0x800000) >> 1);
	}
	
	switch (a) {
		case 0x390000: {
			return TaitoDip[0];
		}
		
		case 0x3b0000: {
			return TaitoDip[1];
		}
	}
	
	return 0;
}

void __fastcall Rainbow68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x3c0000: {
			// nop
			return;
		}
		
		case 0xc20000:
		case 0xc20002: {
			PC080SNSetScrollY((a - 0xc20000) >> 1, d);
			return;
		}
		
		case 0xc40000:
		case 0xc40002: {
			PC080SNSetScrollX((a - 0xc40000) >> 1, d);
			return;
		}
		
		case 0xc50000: {
			PC080SNCtrlWrite((a - 0xc50000) >> 1, d);
			return;
		}
	}
}

unsigned char __fastcall Jumping68KReadByte(unsigned int a)
{
	switch (a) {
		case 0x401001: {
			return TaitoInput[0];
		}
		
		case 0x401003: {
			return TaitoInput[1];
		}
		
		case 0x420000: {
			// nop
			return 0;
		}
	}
	
	return 0;
}

void __fastcall Jumping68KWriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x800000 && a <= 0x80ffff) return;
	
	switch (a) {
		case 0x3a0001: {
			PC090OJSpriteCtrl = d;
			return;
		}
		
		case 0x400007: {
			TaitoSoundLatch = d;
			ZetOpen(0);
			ZetRaiseIrq(0);
			ZetClose();
			return;
		}
	}
}

unsigned short __fastcall Jumping68KReadWord(unsigned int a)
{
	switch (a) {
		case 0x400000: {
			return TaitoDip[0];
		}
		
		case 0x400002: {
			return TaitoDip[0];
		}
	}
	
	return 0;
}

void __fastcall Jumping68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x3c0000: {
			// nop
			return;
		}
		
		case 0x430000:
		case 0x430002: {
			PC080SNSetScrollY((a - 0x430000) >> 1, d);
			return;
		}
		
		case 0xc20000: {
			//nop
			return;
		}
		
		case 0xc50000: {
			//???
			return;
		}
	}
}

unsigned char __fastcall Rastan68KReadByte(unsigned int a)
{
	switch (a) {
		case 0x390001: {
			return TaitoInput[0];
		}
		
		case 0x390003: {
			return TaitoInput[1];
		}
		
		case 0x390005: {
			return TaitoInput[2];
		}
		
		case 0x390007: {
			return TaitoInput[3];
		}
		
		case 0x390009: {
			return TaitoDip[0];
		}
		
		case 0x39000b: {
			return TaitoDip[1];
		}
		
		case 0x3e0003: {
			return TC0140SYTCommRead();
		}
	}
	
	return 0;
}

void __fastcall Rastan68KWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x3e0001: {
			TC0140SYTPortWrite(d);
			return;
		}
		
		case 0x3e0003: {
			TC0140SYTCommWrite(d);
			return;
		}
	}
}

void __fastcall Rastan68KWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x350008: {
			// nop
			return;
		}
		
		case 0x380000: {
			PC090OJSpriteCtrl = (d & 0xe0) >> 5;
			return;
		}
		
		case 0x3c0000: {
			// watchdog
			return;
		}
		
		case 0xc20000:
		case 0xc20002: {
			PC080SNSetScrollY((a - 0xc20000) >> 1, d);
			return;
		}
		
		case 0xc40000:
		case 0xc40002: {
			PC080SNSetScrollX((a - 0xc40000) >> 1, d);
			return;
		}
		
		case 0xc50000: {
			PC080SNCtrlWrite((a - 0xc50000) >> 1, d);
			return;
		}
	}
}

unsigned char __fastcall DariusZ80Read(unsigned short a)
{
	switch (a) {
		case 0x9000: {
			return BurnYM2203Read(0, 0);
		}
		
		case 0x9001: {
			return BurnYM2203Read(0, 1);
		}
		
		case 0xa000: {
			return BurnYM2203Read(1, 0);
		}
		
		case 0xa001: {
			return BurnYM2203Read(1, 1);
		}
		
		case 0xb001: {
			return TC0140SYTSlaveCommRead();
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Read %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall DariusZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0x9000: {
			BurnYM2203Write(0, 0, d);
			return;
		}
		
		case 0x9001: {
			BurnYM2203Write(0, 1, d);
			return;
		}
		
		case 0xa000: {
			BurnYM2203Write(1, 0, d);
			return;
		}
		
		case 0xa001: {
			BurnYM2203Write(1, 1, d);
			return;
		}
		
		case 0xb000: {
			TC0140SYTSlavePortWrite(d);
			return;
		}
		
		case 0xb001: {
			TC0140SYTSlaveCommWrite(d);
			return;
		}
		
		case 0xc000: {
			//darius_fm0_pan
			return;
		}
		
		case 0xc400: {
			//darius_fm1_pan
			return;
		}
		
		case 0xc800: {
			//darius_psg0_pan
			return;
		}
		
		case 0xcc00: {
			//darius_psg1_pan
			return;
		}
		
		case 0xd000: {
			//darius_da_pan
			return;
		}
		
		case 0xd400: {
			bprintf(PRINT_NORMAL, _T("ADPCM Write %x\n"), d);
			DariusADPCMCommand = d;
			return;
		}
		
		case 0xd800: {
			//???
			return;
		}
		
		case 0xdc00: {
			TaitoZ80Bank = d & 0x03;			
			ZetMapArea(0x0000, 0x7fff, 0, TaitoZ80Rom1 + 0x10000 + (TaitoZ80Bank * 0x8000));
			ZetMapArea(0x0000, 0x7fff, 2, TaitoZ80Rom1 + 0x10000 + (TaitoZ80Bank * 0x8000));
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Write %04X, %02X\n"), a, d);
		}
	}
}

unsigned char __fastcall DariusZ802ReadPort(unsigned short a)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			return DariusADPCMCommand;
		}
		
		case 0x02: {
			//???
			return 0;
		}
		
		case 0x03: {
			//???
			return 0;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 #2 Port Read => %02X\n"), a);
		}
	}

	return 0;
}

void __fastcall DariusZ802WritePort(unsigned short a, unsigned char d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			DariusNmiEnable = 0;
			return;
		}
		
		case 0x01: {
			DariusNmiEnable = 1;
			return;
		}
		
		case 0x02: {
			bprintf(PRINT_NORMAL, _T("ADPCM Data Write, %x, %x\n"), d, d << 8);
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 #2 Port Write => %02X, %02X\n"), a, d);
		}
	}
}

unsigned char __fastcall OpwolfZ80Read(unsigned short a)
{
	switch (a) {
		case 0x9001: {
			return BurnYM2151ReadStatus();
		}
		
		case 0xa001: {
			return TC0140SYTSlaveCommRead();
		}
	}

	return 0;
}

void __fastcall OpwolfZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0x9000: {
			BurnYM2151SelectRegister(d);
			return;
		}
		
		case 0x9001: {
			BurnYM2151WriteRegister(d);
			return;
		}
		
		case 0xa000: {
			TC0140SYTSlavePortWrite(d);
			return;
		}
		
		case 0xa001: {
			TC0140SYTSlaveCommWrite(d);
			return;
		}
		
		case 0xb000:
		case 0xb001:
		case 0xb002:
		case 0xb003:
		case 0xb004:
		case 0xb005:
		case 0xb006: {
			int Offset = a - 0xb000;
			int Start, End;
			
			OpwolfADPCM_B[Offset] = d;
			
			if (Offset == 0x04) {
				Start = OpwolfADPCM_B[0] | (OpwolfADPCM_B[1] * 256);
				End = OpwolfADPCM_B[2] | (OpwolfADPCM_B[3] * 256);
				Start *= 16;
				End *= 16;
				MSM5205Play(Start, (End - Start) * 2, 0);
			}
			
			return;
		}
		
		case 0xc000:
		case 0xc001:
		case 0xc002:
		case 0xc003:
		case 0xc004:
		case 0xc005:
		case 0xc006: {
			int Offset = a - 0xc000;
			int Start, End;
			
			OpwolfADPCM_C[Offset] = d;
			
			if (Offset == 0x04) {
				Start = OpwolfADPCM_C[0] | (OpwolfADPCM_C[1] * 256);
				End = OpwolfADPCM_C[2] | (OpwolfADPCM_C[3] * 256);
				Start *= 16;
				End *= 16;
				MSM5205Play(Start, (End - Start) * 2, 1);
			}
			
			return;
		}
		
		case 0xd000:
		case 0xe000: {
			return;
		}
	}
}

unsigned char __fastcall RainbowZ80Read(unsigned short a)
{
	switch (a) {
		case 0x9001: {
			return BurnYM2151ReadStatus();
		}
		
		case 0xa001: {
			return TC0140SYTSlaveCommRead();
		}
	}

	return 0;
}

void __fastcall RainbowZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0x9000: {
			BurnYM2151SelectRegister(d);
			return;
		}
		
		case 0x9001: {
			BurnYM2151WriteRegister(d);
			return;
		}
		
		case 0xa000: {
			TC0140SYTSlavePortWrite(d);
			return;
		}
		
		case 0xa001: {
			TC0140SYTSlaveCommWrite(d);
			return;
		}
	}
}

unsigned char __fastcall JumpingZ80Read(unsigned short a)
{
	switch (a) {
		case 0xb000: {
			return BurnYM2203Read(0, 0);
		}
		
		case 0xb400: {
			return BurnYM2203Read(1, 0);
		}
		
		case 0xb800: {
			return TaitoSoundLatch;
		}
	}

	return 0;
}

void __fastcall JumpingZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0xb000: {
			BurnYM2203Write(0, 0, d);
			return;
		}
		
		case 0xb001: {
			BurnYM2203Write(0, 1, d);
			return;
		}
		
		case 0xb400: {
			BurnYM2203Write(1, 0, d);
			return;
		}
		
		case 0xb401: {
			BurnYM2203Write(1, 1, d);
			return;
		}
		
		case 0xbc00: {
			//nop
			return;
		}
	}
}

unsigned char __fastcall RastanZ80Read(unsigned short a)
{
	switch (a) {
		case 0x9001: {
			return BurnYM2151ReadStatus();
		}
		
		case 0xa001: {
			return TC0140SYTSlaveCommRead();
		}
	}

	return 0;
}

void __fastcall RastanZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0x9000: {
			BurnYM2151SelectRegister(d);
			return;
		}
		
		case 0x9001: {
			BurnYM2151WriteRegister(d);
			return;
		}
	
		case 0xa000: {
			TC0140SYTSlavePortWrite(d);
			return;
		}
		
		case 0xa001: {
			TC0140SYTSlaveCommWrite(d);
			return;
		}
		
		case 0xb000: {
			UINT8 *Rom = MSM5205ROM;
			int Len = 0x10000;
			int Start = d << 8;
			int End;

			// search for end
			End = (Start + 3) & ~3;
			while (End < Len && *((unsigned int *)(&Rom[End])) != 0x08080808) End += 4;

			MSM5205Play(Start, (End - Start) * 2,0);
		}
		
		case 0xc000: {
			return;
		}
		
		case 0xd000: {
			return;
		}
	}
}

unsigned char __fastcall OpwolfbCChipSubZ80Read(unsigned short a)
{
	switch (a) {
		case 0x8800: {
			return TaitoInput[0];
		}
		
		case 0x9800: {
			return TaitoInput[0];
		}
	}

	return 0;
}

void __fastcall OpwolfbCChipSubZ80Write(unsigned short a, unsigned char)
{
	switch (a) {
		case 0x9000:
		case 0xa000: {
			//nop
			return;
		}
	}
}

static void TaitoYM2151IRQHandler(int Irq)
{
	if (Irq) {
		ZetSetIRQLine(0xff, ZET_IRQSTATUS_ACK);
	} else {
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	}
}

static void TaitoYM2203IRQHandler(int, int nStatus)
{
	if (nStatus & 1) {
		ZetSetIRQLine(0xFF, ZET_IRQSTATUS_ACK);
	} else {
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	}
}

inline static int TaitoSynchroniseStream(int nSoundRate)
{
	return (long long)(ZetTotalCycles() * nSoundRate / 4000000);
}

inline static double TaitoGetTime()
{
	return (double)ZetTotalCycles() / 4000000;
}

static void RainbowBankSwitch(unsigned int, unsigned int Data)
{
	TaitoZ80Bank = (Data - 1) & 3;
	
	ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000 + (TaitoZ80Bank * 0x4000));
	ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000 + (TaitoZ80Bank * 0x4000));
}

static void RastanBankSwitch(unsigned int, unsigned int Data)
{
	Data &= 3;
	if (Data != 0) {
		TaitoZ80Bank = Data - 1;
	
		ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000 + (TaitoZ80Bank * 0x4000));
		ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000 + (TaitoZ80Bank * 0x4000));
	}
}

static int DariusCharPlaneOffsets[4]     = { 0, 1, 2, 3 };
static int DariusCharXOffsets[8]         = { 0, 4, 8, 12, 16, 20, 24, 28 };
static int DariusCharYOffsets[8]         = { 0, 32, 64, 96, 128, 160, 192, 224 };
static int DariusCharBPlaneOffsets[2]    = { 0, 8 };
static int DariusCharBXOffsets[8]        = { 0, 1, 2, 3, 4, 5, 6, 7 };
static int DariusCharBYOffsets[8]        = { 0, 16, 32, 48, 64, 80, 96, 112 };
static int DariusSpritePlaneOffsets[4]   = { 24, 8, 16, 0 };
static int DariusSpriteXOffsets[16]      = { 0, 1, 2, 3, 4, 5, 6, 7, 256, 257, 258, 259, 260, 261, 262, 263 };
static int DariusSpriteYOffsets[16]      = { 0, 32, 64, 96, 128, 160, 192, 224, 512, 544, 576, 608, 640, 672, 704, 736 };
static int OpwolfbCharPlaneOffsets[4]    = { 0, 1, 2, 3 };
static int OpwolfbCharXOffsets[8]        = { 0, 4, 8, 12, 16, 20, 24, 28 };
static int OpwolfbCharYOffsets[8]        = { 0, 32, 64, 96, 128, 160, 192, 224 };
static int OpwolfbSpritePlaneOffsets[4]  = { 0, 1, 2, 3 };
static int OpwolfbSpriteXOffsets[16]     = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60 };
static int OpwolfbSpriteYOffsets[16]     = { 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 };
static int RainbowCharPlaneOffsets[4]    = { 0, 1, 2, 3 };
static int RainbowCharXOffsets[8]        = { 8, 12, 0, 4, 24, 28, 16, 20 };
static int RainbowCharYOffsets[8]        = { 0, 32, 64, 96, 128, 160, 192, 224 };
static int RainbowSpritePlaneOffsets[4]  = { 0, 1, 2, 3 };
static int RainbowSpriteXOffsets[16]     = { 8, 12, 0, 4, 24, 28, 16, 20, 40, 44, 32, 36, 56, 60, 48, 52 };
static int RainbowSpriteYOffsets[16]     = { 0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960 };
static int JumpingCharPlaneOffsets[4]    = { 0, 0x20000*8, 0x40000*8, 0x60000*8 };
static int JumpingCharXOffsets[8]        = { 0, 1, 2, 3, 4, 5, 6, 7 };
static int JumpingCharYOffsets[8]        = { 0, 8, 16, 24, 32, 40, 48, 56 };
static int JumpingSpritePlaneOffsets[4]  = { 0x78000*8, 0x50000*8, 0x28000*8, 0 };
static int JumpingSpriteXOffsets[16]     = { 0, 1, 2, 3, 4, 5, 6, 7, 128, 129, 130, 131, 132, 133, 134, 135 };
static int JumpingSpriteYOffsets[16]     = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };
static int RastanCharPlaneOffsets[4]     = { 0, 1, 2, 3 };
static int RastanCharXOffsets[8]         = { 0, 4, 0x200000, 0x200004, 8, 12, 0x200008, 0x20000c };
static int RastanCharYOffsets[8]         = { 0, 16, 32, 48, 64, 80, 96, 112 };
static int RastanSpritePlaneOffsets[4]   = { 0, 1, 2, 3 };
static int RastanSpriteXOffsets[16]      = { 0, 4, 0x200000, 0x200004, 8, 12, 0x200008, 0x20000c, 16, 20, 0x200010, 0x200014, 24, 28, 0x200018, 0x20001c };
static int RastanSpriteYOffsets[16]      = { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480 };

static int DariusInit()
{
	int nLen;
	
	TaitoCharModulo = 0x100;
	TaitoCharNumPlanes = 4;
	TaitoCharWidth = 8;
	TaitoCharHeight = 8;
	TaitoCharPlaneOffsets = DariusCharPlaneOffsets;
	TaitoCharXOffsets = DariusCharXOffsets;
	TaitoCharYOffsets = DariusCharYOffsets;
	TaitoNumChar = 0x3000;
	
	TaitoCharBModulo = 0x80;
	TaitoCharBNumPlanes = 2;
	TaitoCharBWidth = 8;
	TaitoCharBHeight = 8;
	TaitoCharBPlaneOffsets = DariusCharBPlaneOffsets;
	TaitoCharBXOffsets = DariusCharBXOffsets;
	TaitoCharBYOffsets = DariusCharBYOffsets;
	TaitoNumCharB = 0x800;
	
	TaitoSpriteAModulo = 0x400;
	TaitoSpriteANumPlanes = 4;
	TaitoSpriteAWidth = 16;
	TaitoSpriteAHeight = 16;
	TaitoSpriteAPlaneOffsets = DariusSpritePlaneOffsets;
	TaitoSpriteAXOffsets = DariusSpriteXOffsets;
	TaitoSpriteAYOffsets = DariusSpriteYOffsets;
	TaitoNumSpriteA = 0x1800;
	
	TaitoNum68Ks = 2;
	TaitoNumZ80s = 2;
	TaitoNumYM2203 = 2;
	TaitoNumMSM5205 = 1;
		
	TaitoLoadRoms(0);
	
	if (strcmp(BurnDrvGetTextA(DRV_NAME), "darius") != 0)  Taito68KRom1Size = 0x60000;	
	TaitoZ80Rom1Size = 0x30000;
	
	// Allocate and Blank all required memory
	TaitoMem = NULL;
	MemIndex();
	nLen = TaitoMemEnd - (unsigned char *)0;
	if ((TaitoMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(TaitoMem, 0, nLen);
	MemIndex();
	
	if (TaitoLoadRoms(1)) return 1;
	
	if (strcmp(BurnDrvGetTextA(DRV_NAME), "darius") != 0)  {
		memcpy(Taito68KRom1 + 0x40000, Taito68KRom1 + 0x20000, 0x20000);
		memset(Taito68KRom1 + 0x20000, 0xff, 0x20000);
	}
	
	for (int i = 3; i >= 0; i--) {
		memcpy(TaitoZ80Rom1 + 0x8000 * i + 0x10000, TaitoZ80Rom1             , 0x4000);
		memcpy(TaitoZ80Rom1 + 0x8000 * i + 0x14000, TaitoZ80Rom1 + 0x4000 * i, 0x4000);
	}
	
	MSM5205ROM = TaitoZ80Rom1;
	
	PC080SNInit(TaitoNumChar, -16, 0, 0, 1);
	TC0140SYTInit();
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Taito68KRom1           , 0x000000, 0x05ffff, SM_ROM);
	SekMapMemory(Taito68KRam1           , 0x080000, 0x08ffff, SM_RAM);
	SekMapMemory(PC080SNRam             , 0xd00000, 0xd0ffff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0xd80000, 0xd80fff, SM_RAM);
	SekMapMemory(TaitoSpriteRam         , 0xe00100, 0xe00fff, SM_RAM);
	SekMapMemory(TaitoSharedRam         , 0xe01000, 0xe02fff, SM_RAM);
	SekMapMemory(TaitoVideoRam          , 0xe08000, 0xe0ffff, SM_RAM);
	SekMapMemory(Taito68KRam1 + 0x10000 , 0xe10000, 0xe10fff, SM_RAM);
	SekSetReadByteHandler(0, Darius68K1ReadByte);
	SekSetWriteByteHandler(0, Darius68K1WriteByte);
	SekSetReadWordHandler(0, Darius68K1ReadWord);	
	SekSetWriteWordHandler(0, Darius68K1WriteWord);
	SekClose();
	
	SekInit(1, 0x68000);
	SekOpen(1);
	SekMapMemory(Taito68KRom2           , 0x000000, 0x03ffff, SM_ROM);
	SekMapMemory(Taito68KRam2           , 0x040000, 0x04ffff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0xd80000, 0xd80fff, SM_RAM);
	SekMapMemory(TaitoSpriteRam         , 0xe00100, 0xe00fff, SM_RAM);
	SekMapMemory(TaitoSharedRam         , 0xe01000, 0xe02fff, SM_RAM);
	SekMapMemory(TaitoVideoRam          , 0xe08000, 0xe0ffff, SM_RAM);
	SekSetReadByteHandler(0, Darius68K2ReadByte);
	SekSetWriteByteHandler(0, Darius68K2WriteByte);
	SekSetReadWordHandler(0, Darius68K2ReadWord);	
	SekSetWriteWordHandler(0, Darius68K2WriteWord);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(2);
	ZetOpen(0);
	ZetSetReadHandler(DariusZ80Read);
	ZetSetWriteHandler(DariusZ80Write);
	ZetMapArea(0x0000, 0x7fff, 0, TaitoZ80Rom1               );
	ZetMapArea(0x0000, 0x7fff, 2, TaitoZ80Rom1               );
	ZetMapArea(0x8000, 0x8fff, 0, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 1, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 2, TaitoZ80Ram1               );
	ZetMemEnd();
	ZetClose();
	
	ZetOpen(1);
	ZetSetInHandler(DariusZ802ReadPort);
	ZetSetOutHandler(DariusZ802WritePort);
	ZetMapArea(0x0000, 0xffff, 0, TaitoZ80Rom2               );
	ZetMapArea(0x0000, 0xffff, 2, TaitoZ80Rom2               );
	ZetMemEnd();
	ZetClose();
	
	BurnYM2203Init(2, 4000000, TaitoYM2203IRQHandler, TaitoSynchroniseStream, TaitoGetTime, 0);
	BurnTimerAttachZet(8000000 / 2);
	
	MSM5205Init(0, 8000, 100, 1);
	
	GenericTilesInit();
	
	TaitoDrawFunction = DariusDraw;
	TaitoMakeInputsFunction = DariusMakeInputs;
	TaitoIrqLine = 4;
	
	nTaitoCyclesTotal[0] = (16000000 / 2) / 60;
	nTaitoCyclesTotal[1] = (16000000 / 2) / 60;
	nTaitoCyclesTotal[2] = (8000000 / 2) / 60;
	nTaitoCyclesTotal[3] = (8000000 / 2) / 60;

	// Reset the driver
	TaitoResetFunction = DariusDoReset;
	TaitoResetFunction();

	return 0;
}

static int OpwolfInit()
{
	int nLen;
	
	TaitoCharModulo = 0x100;
	TaitoCharNumPlanes = 4;
	TaitoCharWidth = 8;
	TaitoCharHeight = 8;
	TaitoCharPlaneOffsets = RainbowCharPlaneOffsets;
	TaitoCharXOffsets = RainbowCharXOffsets;
	TaitoCharYOffsets = RainbowCharYOffsets;
	TaitoNumChar = 0x4000;
	
	TaitoSpriteAModulo = 0x400;
	TaitoSpriteANumPlanes = 4;
	TaitoSpriteAWidth = 16;
	TaitoSpriteAHeight = 16;
	TaitoSpriteAPlaneOffsets = RainbowSpritePlaneOffsets;
	TaitoSpriteAXOffsets = RainbowSpriteXOffsets;
	TaitoSpriteAYOffsets = RainbowSpriteYOffsets;
	TaitoNumSpriteA = 0x1000;
	
	TaitoNum68Ks = 1;
	TaitoNumZ80s = 1;
	TaitoNumYM2151 = 1;
	TaitoNumMSM5205 = 2;
		
	TaitoLoadRoms(0);
	
	// Allocate and Blank all required memory
	TaitoMem = NULL;
	MemIndex();
	nLen = TaitoMemEnd - (unsigned char *)0;
	if ((TaitoMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(TaitoMem, 0, nLen);
	MemIndex();
	
	if (TaitoLoadRoms(1)) return 1;
	
	PC080SNInit(TaitoNumChar, 0, 8, 0, 0);
	PC090OJInit(TaitoNumSpriteA, 0, 8, 0);
	TC0140SYTInit();
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Taito68KRom1           , 0x000000, 0x03ffff, SM_ROM);
	SekMapMemory(Taito68KRam1           , 0x100000, 0x107fff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0x200000, 0x200fff, SM_RAM);
	SekMapMemory(PC080SNRam             , 0xc00000, 0xc0ffff, SM_RAM);
	SekMapMemory(Taito68KRam1 + 0x8000  , 0xc10000, 0xc1ffff, SM_RAM);
	SekMapMemory(PC090OJRam             , 0xd00000, 0xd03fff, SM_RAM);
	SekSetReadByteHandler(0, Opwolf68KReadByte);
	SekSetWriteByteHandler(0, Opwolf68KWriteByte);
	SekSetReadWordHandler(0, Opwolf68KReadWord);	
	SekSetWriteWordHandler(0, Opwolf68KWriteWord);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(OpwolfZ80Read);
	ZetSetWriteHandler(OpwolfZ80Write);
	ZetMapArea(0x0000, 0x3fff, 0, TaitoZ80Rom1               );
	ZetMapArea(0x0000, 0x3fff, 2, TaitoZ80Rom1               );
	ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x8000, 0x8fff, 0, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 1, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 2, TaitoZ80Ram1               );
	ZetMemEnd();
	ZetClose();
	
	BurnYM2151Init(4000000, 50.0);
	BurnYM2151SetIrqHandler(&TaitoYM2151IRQHandler);
	BurnYM2151SetPortHandler(&RainbowBankSwitch);
	
	MSM5205Init(0, 8000, 100, 1);
	MSM5205Init(1, 8000, 100, 1);
	
	GenericTilesInit();
	
	BurnGunInit(1, true);
	
	TaitoDrawFunction = OpwolfDraw;
	TaitoMakeInputsFunction = OpwolfMakeInputs;
	TaitoIrqLine = 5;
	
	nTaitoCyclesTotal[0] = 12000000 / 60;
	nTaitoCyclesTotal[1] = 4000000 / 60;
	
	UINT16 *Rom = (UINT16*)Taito68KRom1;
	OpWolfGunXOffset = 0xec - (Rom[0x03ffb0 / 2] & 0xff);
	OpWolfGunYOffset = 0x1c - (Rom[0x03ffae / 2] & 0xff);
	int Region = Rom[0x03fffe / 2] & 0xff;
	OpwolfCChipInit(Region);

	// Reset the driver
	TaitoResetFunction = OpwolfDoReset;
	TaitoResetFunction();

	return 0;
}

static int OpwolfbInit()
{
	int nLen;
	
	TaitoCharModulo = 0x100;
	TaitoCharNumPlanes = 4;
	TaitoCharWidth = 8;
	TaitoCharHeight = 8;
	TaitoCharPlaneOffsets = OpwolfbCharPlaneOffsets;
	TaitoCharXOffsets = OpwolfbCharXOffsets;
	TaitoCharYOffsets = OpwolfbCharYOffsets;
	TaitoNumChar = 0x4000;
	
	TaitoSpriteAModulo = 0x400;
	TaitoSpriteANumPlanes = 4;
	TaitoSpriteAWidth = 16;
	TaitoSpriteAHeight = 16;
	TaitoSpriteAPlaneOffsets = OpwolfbSpritePlaneOffsets;
	TaitoSpriteAXOffsets = OpwolfbSpriteXOffsets;
	TaitoSpriteAYOffsets = OpwolfbSpriteYOffsets;
	TaitoNumSpriteA = 0x1000;
	
	TaitoNum68Ks = 1;
	TaitoNumZ80s = 2;
	TaitoNumYM2151 = 1;
	TaitoNumMSM5205 = 2;
	
	TaitoLoadRoms(0);
	
	// Allocate and Blank all required memory
	TaitoMem = NULL;
	MemIndex();
	nLen = TaitoMemEnd - (unsigned char *)0;
	if ((TaitoMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(TaitoMem, 0, nLen);
	MemIndex();
	
	if (TaitoLoadRoms(1)) return 1;
	
	PC080SNInit(TaitoNumChar, 0, 8, 0, 0);
	PC090OJInit(TaitoNumSpriteA, 0, 8, 0);
	TC0140SYTInit();
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Taito68KRom1           , 0x000000, 0x03ffff, SM_ROM);
	SekMapMemory(Taito68KRam1           , 0x100000, 0x107fff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0x200000, 0x200fff, SM_RAM);
	SekMapMemory(PC080SNRam             , 0xc00000, 0xc0ffff, SM_RAM);
	SekMapMemory(Taito68KRam1 + 0x8000  , 0xc10000, 0xc1ffff, SM_RAM);
	SekMapMemory(PC090OJRam             , 0xd00000, 0xd03fff, SM_RAM);
	SekSetReadByteHandler(0, Opwolfb68KReadByte);
	SekSetWriteByteHandler(0, Opwolfb68KWriteByte);
	SekSetReadWordHandler(0, Opwolfb68KReadWord);	
	SekSetWriteWordHandler(0, Opwolfb68KWriteWord);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(2);
	ZetOpen(0);
	ZetSetReadHandler(OpwolfZ80Read);
	ZetSetWriteHandler(OpwolfZ80Write);
	ZetMapArea(0x0000, 0x3fff, 0, TaitoZ80Rom1               );
	ZetMapArea(0x0000, 0x3fff, 2, TaitoZ80Rom1               );
	ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x8000, 0x8fff, 0, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 1, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 2, TaitoZ80Ram1               );
	ZetMemEnd();
	ZetClose();
	
	ZetOpen(1);
	ZetSetReadHandler(OpwolfbCChipSubZ80Read);
	ZetSetWriteHandler(OpwolfbCChipSubZ80Write);
	ZetMapArea(0x0000, 0x7fff, 0, TaitoZ80Rom2               );
	ZetMapArea(0x0000, 0x7fff, 2, TaitoZ80Rom2               );
	ZetMapArea(0xc000, 0xc7ff, 0, TaitoZ80Ram2               );
	ZetMapArea(0xc000, 0xc7ff, 1, TaitoZ80Ram2               );
	ZetMapArea(0xc000, 0xc7ff, 2, TaitoZ80Ram2               );
	ZetMemEnd();
	ZetClose();
	
	BurnYM2151Init(4000000, 50.0);
	BurnYM2151SetIrqHandler(&TaitoYM2151IRQHandler);
	BurnYM2151SetPortHandler(&RainbowBankSwitch);
	
	MSM5205Init(0, 8000, 100, 1);
	MSM5205Init(1, 8000, 100, 1);
	
	GenericTilesInit();
	
	BurnGunInit(1, true);
	
	TaitoDrawFunction = OpwolfDraw;
	TaitoMakeInputsFunction = OpwolfbMakeInputs;
	TaitoIrqLine = 5;
	
	nTaitoCyclesTotal[0] = 12000000 / 60;
	nTaitoCyclesTotal[1] = 4000000 / 60;
	nTaitoCyclesTotal[2] = 4000000 / 60;
	
	OpWolfGunXOffset = -2;
	OpWolfGunYOffset = 17;

	// Reset the driver
	TaitoResetFunction = OpwolfDoReset;
	TaitoResetFunction();

	return 0;
}

static int RainbowInit()
{
	int nLen;
	
	TaitoCharModulo = 0x100;
	TaitoCharNumPlanes = 4;
	TaitoCharWidth = 8;
	TaitoCharHeight = 8;
	TaitoCharPlaneOffsets = RainbowCharPlaneOffsets;
	TaitoCharXOffsets = RainbowCharXOffsets;
	TaitoCharYOffsets = RainbowCharYOffsets;
	TaitoNumChar = 0x4000;
	
	TaitoSpriteAModulo = 0x400;
	TaitoSpriteANumPlanes = 4;
	TaitoSpriteAWidth = 16;
	TaitoSpriteAHeight = 16;
	TaitoSpriteAPlaneOffsets = RainbowSpritePlaneOffsets;
	TaitoSpriteAXOffsets = RainbowSpriteXOffsets;
	TaitoSpriteAYOffsets = RainbowSpriteYOffsets;
	TaitoNumSpriteA = 0x1400;
	
	TaitoNum68Ks = 1;
	TaitoNumZ80s = 1;
	TaitoNumYM2151 = 1;
	
	TaitoLoadRoms(0);
	
	// Allocate and Blank all required memory
	TaitoMem = NULL;
	MemIndex();
	nLen = TaitoMemEnd - (unsigned char *)0;
	if ((TaitoMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(TaitoMem, 0, nLen);
	MemIndex();
	
	if (TaitoLoadRoms(1)) return 1;
	
	PC080SNInit(TaitoNumChar, 0, 16, 0, 0);
	PC090OJInit(TaitoNumSpriteA, 0, 16, 0);
	TC0140SYTInit();
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Taito68KRom1           , 0x000000, 0x07ffff, SM_ROM);
	SekMapMemory(Taito68KRam1           , 0x10c000, 0x10ffff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0x200000, 0x200fff, SM_RAM);
	SekMapMemory(Taito68KRam1 + 0x4000  , 0x201000, 0x203fff, SM_RAM);
	SekMapMemory(PC080SNRam             , 0xc00000, 0xc0ffff, SM_RAM);
	SekMapMemory(PC090OJRam             , 0xd00000, 0xd03fff, SM_RAM);
	SekSetReadByteHandler(0, Rainbow68KReadByte);
	SekSetWriteByteHandler(0, Rainbow68KWriteByte);
	SekSetReadWordHandler(0, Rainbow68KReadWord);	
	SekSetWriteWordHandler(0, Rainbow68KWriteWord);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(RainbowZ80Read);
	ZetSetWriteHandler(RainbowZ80Write);
	ZetMapArea(0x0000, 0x3fff, 0, TaitoZ80Rom1               );
	ZetMapArea(0x0000, 0x3fff, 2, TaitoZ80Rom1               );
	ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x8000, 0x8fff, 0, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 1, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 2, TaitoZ80Ram1               );
	ZetMemEnd();
	ZetClose();
	
	BurnYM2151Init(16000000 / 4, 50.0);
	BurnYM2151SetIrqHandler(&TaitoYM2151IRQHandler);
	BurnYM2151SetPortHandler(&RainbowBankSwitch);
	
	GenericTilesInit();
	
	TaitoDrawFunction = RainbowDraw;
	TaitoMakeInputsFunction = RainbowMakeInputs;
	TaitoIrqLine = 4;
	
	nTaitoCyclesTotal[0] = (16000000 / 2) / 60;
	nTaitoCyclesTotal[1] = (16000000 / 4) / 60;
	
	int CChipVer = 0;
	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "rainbowe")) CChipVer = 1;
	RainbowCChipInit(CChipVer);
		
	// Reset the driver
	TaitoResetFunction = TaitoDoReset;
	TaitoResetFunction();

	return 0;
}

static int JumpingInit()
{
	int nLen;
	
	TaitoCharModulo = 0x40;
	TaitoCharNumPlanes = 4;
	TaitoCharWidth = 8;
	TaitoCharHeight = 8;
	TaitoCharPlaneOffsets = JumpingCharPlaneOffsets;
	TaitoCharXOffsets = JumpingCharXOffsets;
	TaitoCharYOffsets = JumpingCharYOffsets;
	TaitoNumChar = 16384;
	
	TaitoSpriteAModulo = 0x100;
	TaitoSpriteANumPlanes = 4;
	TaitoSpriteAWidth = 16;
	TaitoSpriteAHeight = 16;
	TaitoSpriteAPlaneOffsets = JumpingSpritePlaneOffsets;
	TaitoSpriteAXOffsets = JumpingSpriteXOffsets;
	TaitoSpriteAYOffsets = JumpingSpriteYOffsets;
	TaitoSpriteAInvertRom = 1;
	TaitoNumSpriteA = 5120;
	
	TaitoNum68Ks = 1;
	TaitoNumZ80s = 1;
	TaitoNumYM2203 = 2;
	
	TaitoLoadRoms(0);
	
	// Allocate and Blank all required memory
	TaitoMem = NULL;
	MemIndex();
	nLen = TaitoMemEnd - (unsigned char *)0;
	if ((TaitoMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(TaitoMem, 0, nLen);
	MemIndex();
	
	if (TaitoLoadRoms(1)) return 1;
	
	PC080SNInit(TaitoNumChar, 0, 16, 1, 0);
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Taito68KRom1           , 0x000000, 0x09ffff, SM_ROM);
	SekMapMemory(Taito68KRam1           , 0x10c000, 0x10ffff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0x200000, 0x200fff, SM_RAM);
	SekMapMemory(Taito68KRam1 + 0x4000  , 0x201000, 0x203fff, SM_RAM);
	SekMapMemory(TaitoSpriteRam         , 0x440000, 0x4407ff, SM_RAM);
	SekMapMemory(PC080SNRam             , 0xc00000, 0xc0ffff, SM_RAM);
	SekMapMemory(TaitoSpriteRam + 0x800 , 0xd00000, 0xd01fff, SM_RAM);
	SekSetReadByteHandler(0, Jumping68KReadByte);
	SekSetWriteByteHandler(0, Jumping68KWriteByte);
	SekSetReadWordHandler(0, Jumping68KReadWord);	
	SekSetWriteWordHandler(0, Jumping68KWriteWord);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(JumpingZ80Read);
	ZetSetWriteHandler(JumpingZ80Write);
	ZetMapArea(0x0000, 0x7fff, 0, TaitoZ80Rom1               );
	ZetMapArea(0x0000, 0x7fff, 2, TaitoZ80Rom1               );
	ZetMapArea(0x8000, 0x8fff, 0, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 1, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 2, TaitoZ80Ram1               );
	ZetMapArea(0xc000, 0xffff, 0, TaitoZ80Rom1 + 0xc000      );
	ZetMapArea(0xc000, 0xffff, 2, TaitoZ80Rom1 + 0xc000      );
	ZetMemEnd();
	ZetClose();
	
	BurnYM2203Init(2, 3579545, NULL, TaitoSynchroniseStream, TaitoGetTime, 0);
	BurnTimerAttachZet(4000000);
	
	GenericTilesInit();
	
	TaitoDrawFunction = JumpingDraw;
	TaitoMakeInputsFunction = JumpingMakeInputs;
	TaitoIrqLine = 4;
	PC080SNSetFgTransparentPen(0x0f);
	
	nTaitoCyclesTotal[0] = 8000000 / 60;
	nTaitoCyclesTotal[1] = 4000000 / 60;
	
	// Reset the driver
	TaitoResetFunction = TaitoDoReset;
	TaitoResetFunction();

	return 0;
}

static int RastanInit()
{
	int nLen;
	
	TaitoCharModulo = 0x80;
	TaitoCharNumPlanes = 4;
	TaitoCharWidth = 8;
	TaitoCharHeight = 8;
	TaitoCharPlaneOffsets = RastanCharPlaneOffsets;
	TaitoCharXOffsets = RastanCharXOffsets;
	TaitoCharYOffsets = RastanCharYOffsets;
	TaitoNumChar = 0x4000;
	
	TaitoSpriteAModulo = 0x200;
	TaitoSpriteANumPlanes = 4;
	TaitoSpriteAWidth = 16;
	TaitoSpriteAHeight = 16;
	TaitoSpriteAPlaneOffsets = RastanSpritePlaneOffsets;
	TaitoSpriteAXOffsets = RastanSpriteXOffsets;
	TaitoSpriteAYOffsets = RastanSpriteYOffsets;
	TaitoNumSpriteA = 0x1000;
	
	TaitoNum68Ks = 1;
	TaitoNumZ80s = 1;
	TaitoNumYM2151 = 1;
	TaitoNumMSM5205 = 1;
	
	TaitoLoadRoms(0);
	
	// Allocate and Blank all required memory
	TaitoMem = NULL;
	MemIndex();
	nLen = TaitoMemEnd - (unsigned char *)0;
	if ((TaitoMem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(TaitoMem, 0, nLen);
	MemIndex();
	
	if (TaitoLoadRoms(1)) return 1;
	
	PC080SNInit(TaitoNumChar, 0, 8, 0, 0);
	PC090OJInit(TaitoNumSpriteA, 0, 8, 0);
	TC0140SYTInit();
	
	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(Taito68KRom1           , 0x000000, 0x05ffff, SM_ROM);
	SekMapMemory(Taito68KRam1           , 0x10c000, 0x10ffff, SM_RAM);
	SekMapMemory(TaitoPaletteRam        , 0x200000, 0x200fff, SM_RAM);
	SekMapMemory(PC080SNRam             , 0xc00000, 0xc0ffff, SM_RAM);
	SekMapMemory(PC090OJRam             , 0xd00000, 0xd03fff, SM_RAM);
	SekSetReadByteHandler(0, Rastan68KReadByte);
	SekSetWriteByteHandler(0, Rastan68KWriteByte);
	SekSetWriteWordHandler(0, Rastan68KWriteWord);
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetSetReadHandler(RastanZ80Read);
	ZetSetWriteHandler(RastanZ80Write);
	ZetMapArea(0x0000, 0x3fff, 0, TaitoZ80Rom1               );
	ZetMapArea(0x0000, 0x3fff, 2, TaitoZ80Rom1               );
	ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000      );
	ZetMapArea(0x8000, 0x8fff, 0, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 1, TaitoZ80Ram1               );
	ZetMapArea(0x8000, 0x8fff, 2, TaitoZ80Ram1               );
	ZetMemEnd();
	ZetClose();
	
	BurnYM2151Init(16000000 / 4, 50.0);
	BurnYM2151SetIrqHandler(&TaitoYM2151IRQHandler);
	BurnYM2151SetPortHandler(&RastanBankSwitch);
	
	MSM5205Init(0,8000, 60, 1);
	
	GenericTilesInit();
	
	TaitoDrawFunction = RastanDraw;
	TaitoMakeInputsFunction = RastanMakeInputs;
	TaitoIrqLine = 5;
	
	nTaitoCyclesTotal[0] = (16000000 / 2) / 60;
	nTaitoCyclesTotal[1] = (16000000 / 4) / 60;
		
	// Reset the driver
	TaitoResetFunction = TaitoDoReset;
	TaitoResetFunction();

	return 0;
}

static int TaitoMiscExit()
{
	memset(OpwolfADPCM_B, 0, 8);
	memset(OpwolfADPCM_C, 0, 8);
	
	OpWolfGunXOffset = 0;
	OpWolfGunYOffset = 0;
	
	DariusADPCMCommand = 0;
	DariusNmiEnable = 0;
	DariusCoinWord = 0;
	
	return TaitoExit();
}

static inline unsigned char pal4bit(unsigned char bits)
{
	bits &= 0x0f;
	return (bits << 4) | bits;
}

static inline unsigned char pal5bit(unsigned char bits)
{
	bits &= 0x1f;
	return (bits << 3) | (bits >> 2);
}

inline static unsigned int CalcCol(unsigned short nColour)
{
	int r, g, b;

	r = pal5bit(nColour >>  0);
	g = pal5bit(nColour >>  5);
	b = pal5bit(nColour >> 10);

	return BurnHighCol(r, g, b, 0);
}

inline static unsigned int OpwolfCalcCol(unsigned short nColour)
{
	int r, g, b;

	r = pal4bit(nColour >> 8);
	g = pal4bit(nColour >> 4);
	b = pal4bit(nColour >> 0);

	return BurnHighCol(r, g, b, 0);
}

inline static unsigned int JumpingCalcCol(unsigned short nColour)
{
	int r, g, b;

	r = pal4bit(nColour >> 0);
	g = pal4bit(nColour >> 4);
	b = pal4bit(nColour >> 8);

	return BurnHighCol(r, g, b, 0);
}

static void TaitoMiscCalcPalette()
{
	int i;
	unsigned short* ps;
	unsigned int* pd;

	for (i = 0, ps = (unsigned short*)TaitoPaletteRam, pd = TaitoPalette; i < 0x0800; i++, ps++, pd++) {
		*pd = CalcCol(*ps);
	}
}

static void OpwolfCalcPalette()
{
	int i;
	unsigned short* ps;
	unsigned int* pd;

	for (i = 0, ps = (unsigned short*)TaitoPaletteRam, pd = TaitoPalette; i < 0x0800; i++, ps++, pd++) {
		*pd = OpwolfCalcCol(*ps);
	}
}

static void JumpingCalcPalette()
{
	int i;
	unsigned short* ps;
	unsigned int* pd;

	for (i = 0, ps = (unsigned short*)TaitoPaletteRam, pd = TaitoPalette; i < 0x0800; i++, ps++, pd++) {
		*pd = JumpingCalcCol(*ps);
	}
}

static void DariusDrawSprites(int PriorityDraw)
{
	int Offset, sx, sy;
	UINT16 Code, Data;
	UINT8 xFlip, yFlip, Colour, Priority;
	
	UINT16 *SpriteRam = (UINT16*)TaitoSpriteRam;

	for (Offset = 0xf000 / 2 - 4; Offset >= 0; Offset -= 4)
	{
		Code = SpriteRam[Offset+ 2 ] &0x1fff;

		if (Code) {
			Data = SpriteRam[Offset + 0];
			sy = (256 - Data) & 0x1ff;

			Data = SpriteRam[Offset + 1];
			sx = Data & 0x3ff;

			Data = SpriteRam[Offset + 2];
			xFlip = ((Data & 0x4000) >> 14);
			yFlip = ((Data & 0x8000) >> 15);

			Data = SpriteRam[Offset + 3];
			Priority = (Data & 0x80) >> 7;
			if (Priority != PriorityDraw) continue;
			
			Colour = (Data & 0x7f);

			if (sx > 900) sx -= 1024;
 			if (sy > 400) sy -= 512;
 			
 			sy -= 16;
 			
 			if (sx > 16 && sx < (nScreenWidth < 16) && sy > 16 && sy < (nScreenHeight - 16)) {
 				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					}
				}
 			} else {
	 			if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, TaitoSpritesA);
					}
				}
 			}			
		}
	}
}

static void DariusDrawCharLayer()
{
	int mx, my, Code, Attr, Colour, x, y, Flip, xFlip, yFlip, TileIndex = 0;
	
	UINT16 *VideoRam = (UINT16*)TaitoVideoRam;

	for (my = 0; my < 64; my++) {
		for (mx = 0; mx < 128; mx++) {
			Code = VideoRam[TileIndex + 0x2000] & (TaitoNumCharB - 1);
			Attr = VideoRam[TileIndex + 0x0000];
			
			Colour = (Attr & 0xff) << 2;
			Flip = (Attr & 0xc000) >> 14;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			
			x = 8 * mx;
			y = 8 * my;
			
			if (x > 8 && x < (nScreenWidth - 8) && y > 8 && y < (nScreenHeight - 8)) {
				if (xFlip) {
					if (yFlip) {
						Render8x8Tile_Mask_FlipXY(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					} else {
						Render8x8Tile_Mask_FlipX(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					}
				} else {
					if (yFlip) {
						Render8x8Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					} else {
						Render8x8Tile_Mask(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					}
				}
			} else {			
				if (xFlip) {
					if (yFlip) {
						Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					} else {
						Render8x8Tile_Mask_FlipX_Clip(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					}
				} else {
					if (yFlip) {
						Render8x8Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					} else {
						Render8x8Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 2, 0, 0, TaitoCharsB);
					}
				}
			}

			TileIndex++;
		}
	}
}

static void JumpingDrawSprites()
{
	int SpriteColBank = (PC090OJSpriteCtrl & 0xe0) >> 1;
	
	for (int Offs = 0x400 - 8; Offs >= 0; Offs -= 8) {
		UINT16 *SpriteRam = (UINT16*)TaitoSpriteRam;
		int Tile = SpriteRam[Offs];
		if (Tile < 5120) {
			int sx, sy, Colour, Data1, xFlip, yFlip;

			sy = ((SpriteRam[Offs + 1] - 0xfff1) ^ 0xffff) & 0x1ff;
  			if (sy > 400) sy = sy - 512;
			sx = (SpriteRam[Offs + 2] - 0x38) & 0x1ff;
			if (sx > 400) sx = sx - 512;

			Data1 = SpriteRam[Offs + 3];
			Colour = (SpriteRam[Offs + 4] & 0x0f) | SpriteColBank;
			xFlip = Data1 & 0x40;
			yFlip = Data1 & 0x80;
			
			sy += 1;
			sy -= 16;
			
			if (sx > 16 && sx < (nScreenWidth - 16) && sy > 16 && sy < (nScreenHeight - 16)) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, Tile, sx, sy, Colour, 4, 15, 0, TaitoSpritesA);
					}
				}
			}
		}
	}
}

static void DariusDraw()
{
	BurnTransferClear();
	TaitoMiscCalcPalette();
	PC080SNDrawBgLayer(1, TaitoChars);
	DariusDrawSprites(0);
	PC080SNDrawFgLayer(TaitoChars);
	DariusDrawSprites(1);
	DariusDrawCharLayer();
	BurnTransferCopy(TaitoPalette);
}

static void OpwolfDraw()
{
	BurnTransferClear();
	OpwolfCalcPalette();
	PC080SNDrawBgLayer(1, TaitoChars);
	PC090OJDrawSprites(TaitoSpritesA);
	PC080SNDrawFgLayer(TaitoChars);
	BurnTransferCopy(TaitoPalette);
	
	for (int i = 0; i < nBurnGunNumPlayers; i++) {
		BurnGunDrawTarget(i, BurnGunX[i] >> 8, BurnGunY[i] >> 8);
	}
}

static void RainbowDraw()
{
	BurnTransferClear();
	TaitoMiscCalcPalette();
	PC080SNDrawBgLayer(1, TaitoChars);
	PC090OJDrawSprites(TaitoSpritesA);
	PC080SNDrawFgLayer(TaitoChars);
	BurnTransferCopy(TaitoPalette);
}

static void JumpingDraw()
{
	BurnTransferClear();
	JumpingCalcPalette();
	PC080SNOverrideFgScroll(16, 0);
	PC080SNDrawBgLayer(1, TaitoChars);
	JumpingDrawSprites();	
	PC080SNDrawFgLayer(TaitoChars);
	BurnTransferCopy(TaitoPalette);
}

static void RastanDraw()
{
	BurnTransferClear();
	TaitoMiscCalcPalette();
	PC080SNDrawBgLayer(1, TaitoChars);
	PC080SNDrawFgLayer(TaitoChars);
	PC090OJDrawSprites(TaitoSpritesA);
	BurnTransferCopy(TaitoPalette);
}

static int TaitoMiscFrame()
{
	int nInterleave = 10;
	int nSoundBufferPos = 0;

	if (TaitoReset) TaitoResetFunction();

	TaitoMakeInputsFunction();
	
	nTaitoCyclesDone[0] = nTaitoCyclesDone[1] = nTaitoCyclesDone[2] = 0;

	SekNewFrame();
	ZetNewFrame();
		
	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run 68000 # 1
		nCurrentCPU = 0;
		SekOpen(0);
		nNext = (i + 1) * nTaitoCyclesTotal[nCurrentCPU] / nInterleave;
		nTaitoCyclesSegment = nNext - nTaitoCyclesDone[nCurrentCPU];
		nTaitoCyclesDone[nCurrentCPU] += SekRun(nTaitoCyclesSegment);
		if (i == 9) SekSetIRQLine(TaitoIrqLine, SEK_IRQSTATUS_AUTO);
		SekClose();
		
		// Run Z80
		if (TaitoNumZ80s >= 1) {
			nCurrentCPU = 1;
			ZetOpen(0);
			nNext = (i + 1) * nTaitoCyclesTotal[nCurrentCPU] / nInterleave;
			nTaitoCyclesSegment = nNext - nTaitoCyclesDone[nCurrentCPU];
			nTaitoCyclesSegment = ZetRun(nTaitoCyclesSegment);
			nTaitoCyclesDone[nCurrentCPU] += nTaitoCyclesSegment;
			ZetClose();
		}
		
		if (TaitoNumZ80s == 2) {
			nCurrentCPU = 2;
			ZetOpen(1);
			nNext = (i + 1) * nTaitoCyclesTotal[nCurrentCPU] / nInterleave;
			nTaitoCyclesSegment = nNext - nTaitoCyclesDone[nCurrentCPU];
			nTaitoCyclesSegment = ZetRun(nTaitoCyclesSegment);
			nTaitoCyclesDone[nCurrentCPU] += nTaitoCyclesSegment;
			if (i == 9) ZetRaiseIrq(0);
			ZetClose();
		}
		
		// Render sound segment
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen / nInterleave;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (TaitoNumZ80s >= 1) ZetOpen(0);
			if (TaitoNumYM2151) BurnYM2151Render(pSoundBuf, nSegmentLength);
			for (int j = 0; j < TaitoNumMSM5205; j++) {
				MSM5205Render(j, pSoundBuf, nSegmentLength);
			}
			if (TaitoNumZ80s >= 1) ZetClose();
			nSoundBufferPos += nSegmentLength;
		}
	}
	
	// Make sure the buffer is entirely filled.
	if (pBurnSoundOut) {
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
			if (TaitoNumZ80s >= 1) ZetOpen(0);
			if (TaitoNumYM2151) BurnYM2151Render(pSoundBuf, nSegmentLength);
			for (int j = 0; j < TaitoNumMSM5205; j++) {
				MSM5205Render(j, pSoundBuf, nSegmentLength);
			}
			if (TaitoNumZ80s >= 1) ZetClose();
		}
	}
	
	if (pBurnDraw) TaitoDrawFunction();
	
	return 0;
}

static int DariusFrame()
{
	int nInterleave = 10;

	if (TaitoReset) TaitoResetFunction();

	TaitoMakeInputsFunction();
	
	nTaitoCyclesDone[0] = nTaitoCyclesDone[1] = nTaitoCyclesDone[2] = nTaitoCyclesDone[3] = 0;

	SekNewFrame();
	ZetNewFrame();
		
	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run 68000 # 1
		nCurrentCPU = 0;
		SekOpen(0);
		nNext = (i + 1) * nTaitoCyclesTotal[nCurrentCPU] / nInterleave;
		nTaitoCyclesSegment = nNext - nTaitoCyclesDone[nCurrentCPU];
		nTaitoCyclesDone[nCurrentCPU] += SekRun(nTaitoCyclesSegment);
		if (i == nInterleave - 1) SekSetIRQLine(TaitoIrqLine, SEK_IRQSTATUS_AUTO);
		SekClose();
		
		// Run 68000 # 2
		if ((TaitoCpuACtrl & 0x01)) {
			nCurrentCPU = 1;
			SekOpen(1);
			nNext = (i + 1) * nTaitoCyclesTotal[nCurrentCPU] / nInterleave;
			nTaitoCyclesSegment = nNext - nTaitoCyclesDone[nCurrentCPU];
			nTaitoCyclesDone[nCurrentCPU] += SekRun(nTaitoCyclesSegment);
			if (i == nInterleave - 1) SekSetIRQLine(TaitoIrqLine, SEK_IRQSTATUS_AUTO);
			SekClose();
		}
	}
	
	ZetOpen(0);
	BurnTimerEndFrame(nTaitoCyclesTotal[2] - nTaitoCyclesDone[2]);
	BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
	ZetClose();
	
	ZetOpen(1);
	ZetRun(nTaitoCyclesTotal[3] - nTaitoCyclesDone[3]);
	if (DariusNmiEnable) {
		ZetNmi();
		DariusNmiEnable = 0;
	}
	ZetClose();
	
	for (int i = 0; i < TaitoNumMSM5205; i++) {
		MSM5205Render(i, pBurnSoundOut, nBurnSoundLen);
	}
	
	if (pBurnDraw) TaitoDrawFunction();
	
	return 0;
}

static int JumpingFrame()
{
	if (TaitoReset) TaitoResetFunction();

	TaitoMakeInputsFunction();
	
	nTaitoCyclesDone[0] = nTaitoCyclesDone[1] = 0;

	SekNewFrame();
	ZetNewFrame();
		
	SekOpen(0);
	SekRun(nTaitoCyclesTotal[0]);
	SekSetIRQLine(TaitoIrqLine, SEK_IRQSTATUS_AUTO);
	SekClose();
	
	ZetOpen(0);
	BurnTimerEndFrame(nTaitoCyclesTotal[1] - nTaitoCyclesDone[1]);
	BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
	ZetClose();
	
	if (pBurnDraw) TaitoDrawFunction();
	
	return 0;
}

static int TaitoMiscScan(int nAction, int *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {			// Return minimum compatible version
		*pnMin = 0x029683;
	}
	
	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = TaitoRamStart;
		ba.nLen	  = TaitoRamEnd-TaitoRamStart;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}
	
	TaitoICScan(nAction);
	
	if (nAction & ACB_DRIVER_DATA) {
		SekScan(nAction);
		ZetScan(nAction);

		if (TaitoNumYM2151) BurnYM2151Scan(nAction);
		if (TaitoNumYM2203) BurnYM2203Scan(nAction, pnMin);
		for (int i = 0; i < TaitoNumMSM5205; i++) {
			MSM5205Scan(i, nAction);
		}
		
		BurnGunScan();
				
		SCAN_VAR(TaitoInput);
		SCAN_VAR(TaitoAnalogPort0);
		SCAN_VAR(TaitoAnalogPort1);
		SCAN_VAR(TaitoZ80Bank);
		SCAN_VAR(TaitoSoundLatch);
		SCAN_VAR(OpwolfADPCM_B);
		SCAN_VAR(OpwolfADPCM_C);
		SCAN_VAR(nTaitoCyclesDone);
		SCAN_VAR(nTaitoCyclesSegment);
		SCAN_VAR(DariusADPCMCommand);
		SCAN_VAR(DariusNmiEnable);
		SCAN_VAR(DariusCoinWord);
		SCAN_VAR(PC090OJSpriteCtrl);	// for jumping
	}
	
	if (nAction & ACB_WRITE && TaitoZ80Bank) {
		ZetOpen(0);
		ZetMapArea(0x4000, 0x7fff, 0, TaitoZ80Rom1 + 0x4000 + (TaitoZ80Bank * 0x4000));
		ZetMapArea(0x4000, 0x7fff, 2, TaitoZ80Rom1 + 0x4000 + (TaitoZ80Bank * 0x4000));
		ZetClose();
	}
	
	return 0;
}

struct BurnDriver BurnDrvDarius = {
	"darius", NULL, NULL, "1986",
	"Darius (World)\0", NULL, "Taito Corporation Japan", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_TAITO_MISC,
	NULL, DariusRomInfo, DariusRomName, DariusInputInfo, DariusDIPInfo,
	DariusInit, TaitoMiscExit, DariusFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 864, 224, 12, 3
};

struct BurnDriver BurnDrvDariusj = {
	"dariusj", "darius", NULL, "1986",
	"Darius (Japan)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, DariusjRomInfo, DariusjRomName, DariusInputInfo, DariusjDIPInfo,
	DariusInit, TaitoMiscExit, DariusFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 864, 224, 12, 3
};

struct BurnDriver BurnDrvDariuso = {
	"dariuso", "darius", NULL, "1986",
	"Darius (Japan old version)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, DariusoRomInfo, DariusoRomName, DariusInputInfo, DariusjDIPInfo,
	DariusInit, TaitoMiscExit, DariusFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 864, 224, 12, 3
};

struct BurnDriver BurnDrvDariuse = {
	"dariuse", "darius", NULL, "1986",
	"Darius (Extra) (Japan)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, DariuseRomInfo, DariuseRomName, DariusInputInfo, DariuseDIPInfo,
	DariusInit, TaitoMiscExit, DariusFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 864, 224, 12, 3
};

struct BurnDriver BurnDrvOpwolf = {
	"opwolf", NULL, NULL, "1987",
	"Operation Wolf (World, set 1)\0", NULL, "Taito Corporation Japan", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_TAITO_MISC,
	NULL, OpwolfRomInfo, OpwolfRomName, OpwolfInputInfo, OpwolfDIPInfo,
	OpwolfInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvOpwolfa = {
	"opwolfa", "opwolf", NULL, "1987",
	"Operation Wolf (World, set 2)\0", NULL, "Taito Corporation Japan", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, OpwolfaRomInfo, OpwolfaRomName, OpwolfInputInfo, OpwolfDIPInfo,
	OpwolfInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvOpwolfu = {
	"opwolfu", "opwolf", NULL, "1987",
	"Operation Wolf (US)\0", NULL, "Taito America Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, OpwolfuRomInfo, OpwolfuRomName, OpwolfInputInfo, OpwolfuDIPInfo,
	OpwolfInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvOpwolfb = {
	"opwolfb", "opwolf", NULL, "1987",
	"Operation Bear\0", NULL, "bootleg", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_TAITO_MISC,
	NULL, OpwolfbRomInfo, OpwolfbRomName, OpwolfInputInfo, OpwolfbDIPInfo,
	OpwolfbInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvRainbow = {
	"rainbow", NULL, NULL, "1987",
	"Rainbow Islands (new version)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_TAITO_MISC,
	NULL, RainbowRomInfo, RainbowRomName, RainbowInputInfo, RainbowDIPInfo,
	RainbowInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvRainbowo = {
	"rainbowo", "rainbow", NULL, "1987",
	"Rainbow Islands (old version)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, RainbowoRomInfo, RainbowoRomName, RainbowInputInfo, RainbowDIPInfo,
	RainbowInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvRainbowe = {
	"rainbowe", "rainbow", NULL, "1988",
	"Rainbow Islands (Extra)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, RainboweRomInfo, RainboweRomName, RainbowInputInfo, RainbowDIPInfo,
	RainbowInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvJumping = {
	"jumping", "rainbow", NULL, "1989",
	"Jumping\0", NULL, "bootleg", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_TAITO_MISC,
	NULL, JumpingRomInfo, JumpingRomName, JumpingInputInfo, JumpingDIPInfo,
	JumpingInit, TaitoMiscExit, JumpingFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvRastan = {
	"rastan", NULL, NULL, "1987",
	"Rastan (World)\0", NULL, "Taito Corporation Japan", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_TAITO_MISC,
	NULL, RastanRomInfo, RastanRomName, RastanInputInfo, RastanDIPInfo,
	RastanInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvRastanu = {
	"rastanu", "rastan", NULL, "1987",
	"Rastan (US set 1)\0", NULL, "Taito America Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, RastanuRomInfo, RastanuRomName, RastanInputInfo, RastsagaDIPInfo,
	RastanInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvRastanu2 = {
	"rastanu2", "rastan", NULL, "1987",
	"Rastan (US set 2)\0", NULL, "Taito America Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, Rastanu2RomInfo, Rastanu2RomName, RastanInputInfo, RastsagaDIPInfo,
	RastanInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvRastsaga = {
	"rastsaga", "rastan", NULL, "1987",
	"Rastan Saga (Japan)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, RastsagaRomInfo, RastsagaRomName, RastanInputInfo, RastsagaDIPInfo,
	RastanInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};

struct BurnDriver BurnDrvRastsag1 = {
	"rastsag1", "rastan", NULL, "1987",
	"Rastan Saga (Japan Rev 1)\0", NULL, "Taito Corporation", "Taito Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_TAITO_MISC,
	NULL, Rastsag1RomInfo, Rastsag1RomName, RastanInputInfo, RastsagaDIPInfo,
	RastanInit, TaitoMiscExit, TaitoMiscFrame, NULL, TaitoMiscScan,
	0, NULL, NULL, NULL, NULL, 320, 240, 4, 3
};
