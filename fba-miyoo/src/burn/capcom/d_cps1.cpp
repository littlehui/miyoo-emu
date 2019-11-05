#include "cps.h"

#define CPS1_68K_PROGRAM_BYTESWAP	1
#define CPS1_68K_PROGRAM_NO_BYTESWAP	2
#define CPS1_Z80_PROGRAM		3
#define CPS1_TILES			4
#define CPS1_OKIM6295_SAMPLES		5
#define CPS1_QSOUND_SAMPLES		6
#define CPS1_PIC			7

//#define INCLUDE_HACKS			1
//#define INCLUDE_DUPLICATES		1

// Input Definitions

static struct BurnInputInfo NTFOInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Shot"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Shot"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(NTFO);

static struct BurnInputInfo ThreeWondersInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Button 1"      , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Button 2"      , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Button 1"      , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Button 2"      , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(ThreeWonders);

static struct BurnInputInfo CaptcommInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"P3 Coin"          , BIT_DIGITAL  , CpsInp177+6, "p3 coin"   },
	{"P3 Start"         , BIT_DIGITAL  , CpsInp177+7, "p3 start"  },
	{"P3 Up"            , BIT_DIGITAL  , CpsInp177+3, "p3 up"     },
	{"P3 Down"          , BIT_DIGITAL  , CpsInp177+2, "p3 down"   },
	{"P3 Left"          , BIT_DIGITAL  , CpsInp177+1, "p3 left"   },
	{"P3 Right"         , BIT_DIGITAL  , CpsInp177+0, "p3 right"  },
	{"P3 Attack"        , BIT_DIGITAL  , CpsInp177+4, "p3 fire 1" },
	{"P3 Jump"          , BIT_DIGITAL  , CpsInp177+5, "p3 fire 2" },

	{"P4 Coin"          , BIT_DIGITAL  , CpsInp179+6, "p4 coin"   },
	{"P4 Start"         , BIT_DIGITAL  , CpsInp179+7, "p4 start"  },
	{"P4 Up"            , BIT_DIGITAL  , CpsInp179+3, "p4 up"     },
	{"P4 Down"          , BIT_DIGITAL  , CpsInp179+2, "p4 down"   },
	{"P4 Left"          , BIT_DIGITAL  , CpsInp179+1, "p4 left"   },
	{"P4 Right"         , BIT_DIGITAL  , CpsInp179+0, "p4 right"  },
	{"P4 Attack"        , BIT_DIGITAL  , CpsInp179+4, "p4 fire 1" },
	{"P4 Jump"          , BIT_DIGITAL  , CpsInp179+5, "p4 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Captcomm);

static struct BurnInputInfo CawingInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Shot"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Shot"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Cawing);

static struct BurnInputInfo CawingbInputList[] = {
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp008+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp008+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Shot"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp008+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp008+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Shot"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp008+6, "diag"      },
 	{"Service"          , BIT_DIGITAL  , CpsInp008+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Cawingb);

static struct BurnInputInfo Cworld2jInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Answer 1"      , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Answer 2"      , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Answer 3"      , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Answer 4"      , BIT_DIGITAL  , CpsInp001+7, "p1 fire 4" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Answer 1"      , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Answer 2"      , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Answer 3"      , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Answer 4"      , BIT_DIGITAL  , CpsInp000+7, "p2 fire 4" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Cworld2j);

static struct BurnInputInfo DinoInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0 , "p1 coin"  },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4 , "p1 start" },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3 , "p1 up"    },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2 , "p1 down"  },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1 , "p1 left"  },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0 , "p1 right" },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4 , "p1 fire 1"},
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5 , "p1 fire 2"},

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1 , "p2 coin"  },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5 , "p2 start" },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3 , "p2 up"    },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2 , "p2 down"  },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1 , "p2 left"  },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0 , "p2 right" },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4 , "p2 fire 1"},
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5 , "p2 fire 2"},

	{"P3 Coin"          , BIT_DIGITAL  , CpsInpc001+6, "p3 coin"  },
	{"P3 Start"         , BIT_DIGITAL  , CpsInpc001+7, "p3 start" },
	{"P3 Up"            , BIT_DIGITAL  , CpsInpc001+3, "p3 up"    },
	{"P3 Down"          , BIT_DIGITAL  , CpsInpc001+2, "p3 down"  },
	{"P3 Left"          , BIT_DIGITAL  , CpsInpc001+1, "p3 left"  },
	{"P3 Right"         , BIT_DIGITAL  , CpsInpc001+0, "p3 right" },
	{"P3 Attack"        , BIT_DIGITAL  , CpsInpc001+4, "p3 fire 1"},
	{"P3 Jump"          , BIT_DIGITAL  , CpsInpc001+5, "p3 fire 2"},

	{"Reset"            , BIT_DIGITAL  , &CpsReset   ,  "reset"   },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6 ,  "diag"    },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2 ,  "service" },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E     ,  "dip"     },
};

STDINPUTINFO(Dino);

static struct BurnInputInfo DinohInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0 , "p1 coin"  },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4 , "p1 start" },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3 , "p1 up"    },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2 , "p1 down"  },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1 , "p1 left"  },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0 , "p1 right" },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4 , "p1 fire 1"},
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5 , "p1 fire 2"},
	{"P1 Bomb"          , BIT_DIGITAL  , CpsInp001+6 , "p1 fire 3"},

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1 , "p2 coin"  },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5 , "p2 start" },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3 , "p2 up"    },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2 , "p2 down"  },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1 , "p2 left"  },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0 , "p2 right" },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4 , "p2 fire 1"},
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5 , "p2 fire 2"},
	{"P2 Bomb"          , BIT_DIGITAL  , CpsInp000+6 , "p2 fire 3"},

	{"P3 Coin"          , BIT_DIGITAL  , CpsInp177+6, "p3 coin"   },
	{"P3 Start"         , BIT_DIGITAL  , CpsInp177+7, "p3 start"  },
	{"P3 Up"            , BIT_DIGITAL  , CpsInp177+3, "p3 up"     },
	{"P3 Down"          , BIT_DIGITAL  , CpsInp177+2, "p3 down"   },
	{"P3 Left"          , BIT_DIGITAL  , CpsInp177+1, "p3 left"   },
	{"P3 Right"         , BIT_DIGITAL  , CpsInp177+0, "p3 right"  },
	{"P3 Attack"        , BIT_DIGITAL  , CpsInp177+4, "p3 fire 1" },
	{"P3 Jump"          , BIT_DIGITAL  , CpsInp177+5, "p3 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset   ,  "reset"   },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6 ,  "diag"    },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2 ,  "service" },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E     ,  "dip"     },
};

STDINPUTINFO(Dinoh);

static struct BurnInputInfo DynwarInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack Left"   , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Attack Right"  , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack Left"   , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Attack Right"  , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Dynwar);

static struct BurnInputInfo FfightInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Ffight);

#define A(a, b, c, d) {a, b, (unsigned char*)(c), d}

static struct BurnInputInfo ForgottnInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL,    CpsInp018+0, "p1 coin"  },
	{"P1 Start"         , BIT_DIGITAL,    CpsInp018+4, "p1 start" },
	{"P1 Up"            , BIT_DIGITAL,    CpsInp001+3, "p1 up"    },
	{"P1 Down"          , BIT_DIGITAL,    CpsInp001+2, "p1 down"  },
	{"P1 Left"          , BIT_DIGITAL,    CpsInp001+1, "p1 left"  },
	{"P1 Right"         , BIT_DIGITAL,    CpsInp001+0, "p1 right" },
	{"P1 Attack"        , BIT_DIGITAL,    CpsInp001+4, "p1 fire 1"},
	A("P1 Turn"         , BIT_ANALOG_REL, &CpsInp055,  "p1 z-axis"),

	{"P2 Coin"          , BIT_DIGITAL,    CpsInp018+1, "p2 coin"  },
	{"P2 Start"         , BIT_DIGITAL,    CpsInp018+5, "p2 start" },
	{"P2 Up"            , BIT_DIGITAL,    CpsInp000+3, "p2 up"    },
	{"P2 Down"          , BIT_DIGITAL,    CpsInp000+2, "p2 down"  },
	{"P2 Left"          , BIT_DIGITAL,    CpsInp000+1, "p2 left"  },
	{"P2 Right"         , BIT_DIGITAL,    CpsInp000+0, "p2 right" },
	{"P2 Attack"        , BIT_DIGITAL,    CpsInp000+4, "p2 fire 1"},
	A("P2 Turn"         , BIT_ANALOG_REL, &CpsInp05d,  "p2 z-axis"),

	{"Reset"            , BIT_DIGITAL,    &CpsReset,   "reset"    },
	{"Service"          , BIT_DIGITAL,    CpsInp018+2, "service"  },

	{"Dip A"            , BIT_DIPSWITCH,  &Cpi01A    , "dip"      },
	{"Dip B"            , BIT_DIPSWITCH,  &Cpi01C    , "dip"      },
	{"Dip C"            , BIT_DIPSWITCH,  &Cpi01E    , "dip"      },
};

#undef A

STDINPUTINFO(Forgottn);

static struct BurnInputInfo GhoulsInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Fire"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	
	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Fire"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Ghouls);

static struct BurnInputInfo KnightsInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"P3 Coin"          , BIT_DIGITAL  , CpsInp177+6, "p3 coin"   },
	{"P3 Start"         , BIT_DIGITAL  , CpsInp177+7, "p3 start"  },
	{"P3 Up"            , BIT_DIGITAL  , CpsInp177+3, "p3 up"     },
	{"P3 Down"          , BIT_DIGITAL  , CpsInp177+2, "p3 down"   },
	{"P3 Left"          , BIT_DIGITAL  , CpsInp177+1, "p3 left"   },
	{"P3 Right"         , BIT_DIGITAL  , CpsInp177+0, "p3 right"  },
	{"P3 Attack"        , BIT_DIGITAL  , CpsInp177+4, "p3 fire 1" },
	{"P3 Jump"          , BIT_DIGITAL  , CpsInp177+5, "p3 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Knights);

static struct BurnInputInfo KodInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"P3 Coin"          , BIT_DIGITAL  , CpsInp177+6, "p3 coin"   },
	{"P3 Start"         , BIT_DIGITAL  , CpsInp177+7, "p3 start"  },
	{"P3 Up"            , BIT_DIGITAL  , CpsInp177+3, "p3 up"     },
	{"P3 Down"          , BIT_DIGITAL  , CpsInp177+2, "p3 down"   },
	{"P3 Left"          , BIT_DIGITAL  , CpsInp177+1, "p3 left"   },
	{"P3 Right"         , BIT_DIGITAL  , CpsInp177+0, "p3 right"  },
	{"P3 Attack"        , BIT_DIGITAL  , CpsInp177+4, "p3 fire 1" },
	{"P3 Jump"          , BIT_DIGITAL  , CpsInp177+5, "p3 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Kod);

static struct BurnInputInfo KodhInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"P3 Coin"          , BIT_DIGITAL  , CpsInpc000+6, "p3 coin"   },
	{"P3 Start"         , BIT_DIGITAL  , CpsInpc000+7, "p3 start"  },
	{"P3 Up"            , BIT_DIGITAL  , CpsInpc000+3, "p3 up"     },
	{"P3 Down"          , BIT_DIGITAL  , CpsInpc000+2, "p3 down"   },
	{"P3 Left"          , BIT_DIGITAL  , CpsInpc000+1, "p3 left"   },
	{"P3 Right"         , BIT_DIGITAL  , CpsInpc000+0, "p3 right"  },
	{"P3 Attack"        , BIT_DIGITAL  , CpsInpc000+4, "p3 fire 1" },
	{"P3 Jump"          , BIT_DIGITAL  , CpsInpc000+5, "p3 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Kodh);

static struct BurnInputInfo MegamanInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Fire"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Select"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Fire"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Select"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Megaman);

static struct BurnInputInfo MercsInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Shot"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Shot"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"P3 Coin"          , BIT_DIGITAL  , CpsInp177+6, "p3 coin"   },
	{"P3 Start"         , BIT_DIGITAL  , CpsInp177+7, "p3 start"  },
	{"P3 Up"            , BIT_DIGITAL  , CpsInp177+3, "p3 up"     },
	{"P3 Down"          , BIT_DIGITAL  , CpsInp177+2, "p3 down"   },
	{"P3 Left"          , BIT_DIGITAL  , CpsInp177+1, "p3 left"   },
	{"P3 Right"         , BIT_DIGITAL  , CpsInp177+0, "p3 right"  },
	{"P3 Shot"          , BIT_DIGITAL  , CpsInp177+4, "p3 fire 1" },
	{"P3 Special"       , BIT_DIGITAL  , CpsInp177+5, "p3 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Mercs);

static struct BurnInputInfo MswordInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Msword);

static struct BurnInputInfo MtwinsInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Mtwins);

static struct BurnInputInfo NemoInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Nemo);

static struct BurnInputInfo Pang3InputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Shot 1"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Shot 2"        , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Shot 1"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Shot 2"        , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Pang3);

static struct BurnInputInfo PnickjInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Turn 1"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Turn 2"        , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Turn 1"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Turn 2"        , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Pnickj);

static struct BurnInputInfo PunisherInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Punisher);

static struct BurnInputInfo QadInputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Answer 1"      , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Answer 2"      , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Answer 3"      , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Answer 4"      , BIT_DIGITAL  , CpsInp001+7, "p1 fire 4" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Answer 1"      , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Answer 2"      , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Answer 3"      , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Answer 4"      , BIT_DIGITAL  , CpsInp000+7, "p2 fire 4" },
	
	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Qad);

static struct BurnInputInfo Qtono2InputList[] =
{
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Answer 1"      , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Answer 2"      , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Answer 3"      , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Answer 4"      , BIT_DIGITAL  , CpsInp001+7, "p1 fire 4" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Answer 1"      , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Answer 2"      , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Answer 3"      , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Answer 4"      , BIT_DIGITAL  , CpsInp000+7, "p2 fire 4" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Qtono2);

static struct BurnInputInfo Sf2InputList[] = {
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Weak Punch"    , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Medium Punch"  , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Strong Punch"  , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Weak Kick"     , BIT_DIGITAL  , CpsInp177+0, "p1 fire 4" },
	{"P1 Medium Kick"   , BIT_DIGITAL  , CpsInp177+1, "p1 fire 5" },
	{"P1 Strong Kick"   , BIT_DIGITAL  , CpsInp177+2, "p1 fire 6" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Weak Punch"    , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Medium Punch"  , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Strong Punch"  , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Weak Kick"     , BIT_DIGITAL  , CpsInp177+4, "p2 fire 4" },
	{"P2 Medium Kick"   , BIT_DIGITAL  , CpsInp177+5, "p2 fire 5" },
	{"P2 Strong Kick"   , BIT_DIGITAL  , CpsInp177+6, "p2 fire 6" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Sf2);

static struct BurnInputInfo Sf2ueInputList[] = {
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Weak Punch"    , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Medium Punch"  , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Strong Punch"  , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Weak Kick"     , BIT_DIGITAL  , CpsInp1fd+0, "p1 fire 4" },
	{"P1 Medium Kick"   , BIT_DIGITAL  , CpsInp1fd+1, "p1 fire 5" },
	{"P1 Strong Kick"   , BIT_DIGITAL  , CpsInp1fd+2, "p1 fire 6" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Weak Punch"    , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Medium Punch"  , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Strong Punch"  , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Weak Kick"     , BIT_DIGITAL  , CpsInp1fd+4, "p2 fire 4" },
	{"P2 Medium Kick"   , BIT_DIGITAL  , CpsInp1fd+5, "p2 fire 5" },
	{"P2 Strong Kick"   , BIT_DIGITAL  , CpsInp1fd+6, "p2 fire 6" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Sf2ue);

static struct BurnInputInfo Sf2yycInputList[] = {
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp019+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp019+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Weak Punch"    , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Medium Punch"  , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Strong Punch"  , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Weak Kick"     , BIT_DIGITAL  , CpsInp176+0, "p1 fire 4" },
	{"P1 Medium Kick"   , BIT_DIGITAL  , CpsInp176+1, "p1 fire 5" },
	{"P1 Strong Kick"   , BIT_DIGITAL  , CpsInp176+2, "p1 fire 6" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp019+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp019+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Weak Punch"    , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Medium Punch"  , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Strong Punch"  , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Weak Kick"     , BIT_DIGITAL  , CpsInp176+4, "p2 fire 4" },
	{"P2 Medium Kick"   , BIT_DIGITAL  , CpsInp176+5, "p2 fire 5" },
	{"P2 Strong Kick"   , BIT_DIGITAL  , CpsInp176+6, "p2 fire 6" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp019+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp019+2, "service"   },
	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Sf2yyc);

static struct BurnInputInfo Sf2m1InputList[] = {
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
	{"P1 Weak Punch"    , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
	{"P1 Medium Punch"  , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
	{"P1 Strong Punch"  , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
	{"P1 Weak Kick"     , BIT_DIGITAL  , CpsInp012+0, "p1 fire 4" },
	{"P1 Medium Kick"   , BIT_DIGITAL  , CpsInp012+1, "p1 fire 5" },
	{"P1 Strong Kick"   , BIT_DIGITAL  , CpsInp012+2, "p1 fire 6" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
	{"P2 Weak Punch"    , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
	{"P2 Medium Punch"  , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
	{"P2 Strong Punch"  , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
	{"P2 Weak Kick"     , BIT_DIGITAL  , CpsInp012+4, "p2 fire 4" },
	{"P2 Medium Kick"   , BIT_DIGITAL  , CpsInp012+5, "p2 fire 5" },
	{"P2 Strong Kick"   , BIT_DIGITAL  , CpsInp012+6, "p2 fire 6" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Sf2m1);

static struct BurnInputInfo Sf2m3InputList[] = {
	{"P1 Coin"          , BIT_DIGITAL  , CpsInp029+0, "p1 coin"   },
	{"P1 Start"         , BIT_DIGITAL  , CpsInp029+4, "p1 start"  },
	{"P1 Up"            , BIT_DIGITAL  , CpsInp011+3, "p1 up"     },
	{"P1 Down"          , BIT_DIGITAL  , CpsInp011+2, "p1 down"   },
	{"P1 Left"          , BIT_DIGITAL  , CpsInp011+1, "p1 left"   },
	{"P1 Right"         , BIT_DIGITAL  , CpsInp011+0, "p1 right"  },
	{"P1 Weak Punch"    , BIT_DIGITAL  , CpsInp011+4, "p1 fire 1" },
	{"P1 Medium Punch"  , BIT_DIGITAL  , CpsInp011+5, "p1 fire 2" },
	{"P1 Strong Punch"  , BIT_DIGITAL  , CpsInp011+6, "p1 fire 3" },
	{"P1 Weak Kick"     , BIT_DIGITAL  , CpsInp186+0, "p1 fire 4" },
	{"P1 Medium Kick"   , BIT_DIGITAL  , CpsInp186+1, "p1 fire 5" },
	{"P1 Strong Kick"   , BIT_DIGITAL  , CpsInp186+2, "p1 fire 6" },

	{"P2 Coin"          , BIT_DIGITAL  , CpsInp029+1, "p2 coin"   },
	{"P2 Start"         , BIT_DIGITAL  , CpsInp029+5, "p2 start"  },
	{"P2 Up"            , BIT_DIGITAL  , CpsInp010+3, "p2 up"     },
	{"P2 Down"          , BIT_DIGITAL  , CpsInp010+2, "p2 down"   },
	{"P2 Left"          , BIT_DIGITAL  , CpsInp010+1, "p2 left"   },
	{"P2 Right"         , BIT_DIGITAL  , CpsInp010+0, "p2 right"  },
	{"P2 Weak Punch"    , BIT_DIGITAL  , CpsInp010+4, "p2 fire 1" },
	{"P2 Medium Punch"  , BIT_DIGITAL  , CpsInp010+5, "p2 fire 2" },
	{"P2 Strong Punch"  , BIT_DIGITAL  , CpsInp010+6, "p2 fire 3" },
	{"P2 Weak Kick"     , BIT_DIGITAL  , CpsInp186+4, "p2 fire 4" },
	{"P2 Medium Kick"   , BIT_DIGITAL  , CpsInp186+5, "p2 fire 5" },
	{"P2 Strong Kick"   , BIT_DIGITAL  , CpsInp186+6, "p2 fire 6" },

	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
	{"Diagnostic"       , BIT_DIGITAL  , CpsInp029+6, "diag"      },
	{"Service"          , BIT_DIGITAL  , CpsInp029+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Sf2m3);

static struct BurnInputInfo SfzchInputList[] =
{
	{"P1 Pause"         , BIT_DIGITAL  , CpsInp018+2, ""          },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Weak Punch"    , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Medium Punch"  , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Strong Punch"  , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
 	{"P1 Weak Kick"     , BIT_DIGITAL  , CpsInp001+7, "p1 fire 4" },
 	{"P1 Medium Kick"   , BIT_DIGITAL  , CpsInp018+0, "p1 fire 5" },
 	{"P1 Strong Kick"   , BIT_DIGITAL  , CpsInp018+6, "p1 fire 6" },

 	{"P2 Pause"         , BIT_DIGITAL  , CpsInp018+3, ""          },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Weak Punch"    , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Medium Punch"  , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Strong Punch"  , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },
 	{"P2 Weak Kick"     , BIT_DIGITAL  , CpsInp000+7, "p2 fire 4" },
 	{"P2 Medium Kick"   , BIT_DIGITAL  , CpsInp018+1, "p2 fire 5" },
 	{"P2 Strong Kick"   , BIT_DIGITAL  , CpsInp018+7, "p2 fire 6" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
};

STDINPUTINFO(Sfzch);

static struct BurnInputInfo SlammastInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Pin"           , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Pin"           , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

 	{"P3 Coin"          , BIT_DIGITAL  , CpsInpc001+6, "p3 coin"  },
 	{"P3 Start"         , BIT_DIGITAL  , CpsInpc001+7, "p3 start" },
 	{"P3 Up"            , BIT_DIGITAL  , CpsInpc001+3, "p3 up"    },
 	{"P3 Down"          , BIT_DIGITAL  , CpsInpc001+2, "p3 down"  },
 	{"P3 Left"          , BIT_DIGITAL  , CpsInpc001+1, "p3 left"  },
 	{"P3 Right"         , BIT_DIGITAL  , CpsInpc001+0, "p3 right" },
 	{"P3 Attack"        , BIT_DIGITAL  , CpsInpc001+4, "p3 fire 1"},
 	{"P3 Jump"          , BIT_DIGITAL  , CpsInpc001+5, "p3 fire 2"},
 	{"P3 Pin"           , BIT_DIGITAL  , CpsInp001+7 , "p3 fire 3"},

 	{"P4 Coin"          , BIT_DIGITAL  , CpsInpc003+6, "p4 coin"  },
 	{"P4 Start"         , BIT_DIGITAL  , CpsInpc003+7, "p4 start" },
 	{"P4 Up"            , BIT_DIGITAL  , CpsInpc003+3, "p4 up"    },
 	{"P4 Down"          , BIT_DIGITAL  , CpsInpc003+2, "p4 down"  },
 	{"P4 Left"          , BIT_DIGITAL  , CpsInpc003+1, "p4 left"  },
 	{"P4 Right"         , BIT_DIGITAL  , CpsInpc003+0, "p4 right" },
 	{"P4 Attack"        , BIT_DIGITAL  , CpsInpc003+4, "p4 fire 1"},
 	{"P4 Jump"          , BIT_DIGITAL  , CpsInpc003+5, "p4 fire 2"},
 	{"P4 Pin"           , BIT_DIGITAL  , CpsInp000+7 , "p4 fire 3"},

 	{"Reset"            , BIT_DIGITAL  , &CpsReset   , "reset"    },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6 , "diag"     },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2 , "service"  },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E     , "dip"      },
};

STDINPUTINFO(Slammast);

static struct BurnInputInfo StriderInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset  , "reset"     },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Strider);

static struct BurnInputInfo UnsquadInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Shot"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Special"       , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Shot"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Special"       , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },

};

STDINPUTINFO(Unsquad);

static struct BurnInputInfo VarthInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Shot"          , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Bomb"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Shot"          , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Bomb"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Varth);

static struct BurnInputInfo WillowInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },
 	{"P1 Fire 3"        , BIT_DIGITAL  , CpsInp001+6, "p1 fire 3" },
 
 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },
 	{"P2 Fire 3"        , BIT_DIGITAL  , CpsInp000+6, "p2 fire 3" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Willow);

static struct BurnInputInfo WofInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

 	{"P3 Coin"          , BIT_DIGITAL  , CpsInpc001+6, "p3 coin"  },
 	{"P3 Start"         , BIT_DIGITAL  , CpsInpc001+7, "p3 start" },
 	{"P3 Up"            , BIT_DIGITAL  , CpsInpc001+3, "p3 up"    },
 	{"P3 Down"          , BIT_DIGITAL  , CpsInpc001+2, "p3 down"  },
 	{"P3 Left"          , BIT_DIGITAL  , CpsInpc001+1, "p3 left"  },
 	{"P3 Right"         , BIT_DIGITAL  , CpsInpc001+0, "p3 right" },
 	{"P3 Attack"        , BIT_DIGITAL  , CpsInpc001+4, "p3 fire 1"},
 	{"P3 Jump"          , BIT_DIGITAL  , CpsInpc001+5, "p3 fire 2"},

 	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Wof);

static struct BurnInputInfo WofhfhInputList[] =
{
 	{"P1 Coin"          , BIT_DIGITAL  , CpsInp018+0, "p1 coin"   },
 	{"P1 Start"         , BIT_DIGITAL  , CpsInp018+4, "p1 start"  },
 	{"P1 Up"            , BIT_DIGITAL  , CpsInp001+3, "p1 up"     },
 	{"P1 Down"          , BIT_DIGITAL  , CpsInp001+2, "p1 down"   },
 	{"P1 Left"          , BIT_DIGITAL  , CpsInp001+1, "p1 left"   },
 	{"P1 Right"         , BIT_DIGITAL  , CpsInp001+0, "p1 right"  },
 	{"P1 Attack"        , BIT_DIGITAL  , CpsInp001+4, "p1 fire 1" },
 	{"P1 Jump"          , BIT_DIGITAL  , CpsInp001+5, "p1 fire 2" },

 	{"P2 Coin"          , BIT_DIGITAL  , CpsInp018+1, "p2 coin"   },
 	{"P2 Start"         , BIT_DIGITAL  , CpsInp018+5, "p2 start"  },
 	{"P2 Up"            , BIT_DIGITAL  , CpsInp000+3, "p2 up"     },
 	{"P2 Down"          , BIT_DIGITAL  , CpsInp000+2, "p2 down"   },
 	{"P2 Left"          , BIT_DIGITAL  , CpsInp000+1, "p2 left"   },
 	{"P2 Right"         , BIT_DIGITAL  , CpsInp000+0, "p2 right"  },
 	{"P2 Attack"        , BIT_DIGITAL  , CpsInp000+4, "p2 fire 1" },
 	{"P2 Jump"          , BIT_DIGITAL  , CpsInp000+5, "p2 fire 2" },

 	{"P3 Coin"          , BIT_DIGITAL  , CpsInp177+6, "p3 coin"   },
 	{"P3 Start"         , BIT_DIGITAL  , CpsInp177+7, "p3 start"  },
 	{"P3 Up"            , BIT_DIGITAL  , CpsInp177+3, "p3 up"     },
 	{"P3 Down"          , BIT_DIGITAL  , CpsInp177+2, "p3 down"   },
 	{"P3 Left"          , BIT_DIGITAL  , CpsInp177+1, "p3 left"   },
 	{"P3 Right"         , BIT_DIGITAL  , CpsInp177+0, "p3 right"  },
 	{"P3 Attack"        , BIT_DIGITAL  , CpsInp177+4, "p3 fire 1" },
 	{"P3 Jump"          , BIT_DIGITAL  , CpsInp177+5, "p3 fire 2" },

 	{"Reset"            , BIT_DIGITAL  , &CpsReset,   "reset"     },
 	{"Diagnostic"       , BIT_DIGITAL  , CpsInp018+6, "diag"      },
 	{"Service"          , BIT_DIGITAL  , CpsInp018+2, "service"   },
 	{"Dip A"            , BIT_DIPSWITCH, &Cpi01A    , "dip"       },
 	{"Dip B"            , BIT_DIPSWITCH, &Cpi01C    , "dip"       },
 	{"Dip C"            , BIT_DIPSWITCH, &Cpi01E    , "dip"       },
};

STDINPUTINFO(Wofhfh);

// Dip Switch Definitions

#define CPS1_COINAGE_1(dipval) \
	{0     , 0xfe, 0   , 8   , "Coin A"                 }, \
	{dipval, 0x01, 0x07, 0x07, "4 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x06, "3 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x05, "2 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x00, "1 Coin  1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x01, "1 Coin  2 Credits"      }, \
	{dipval, 0x01, 0x07, 0x02, "1 Coin  3 Credits"      }, \
	{dipval, 0x01, 0x07, 0x03, "1 Coin  4 Credits"      }, \
	{dipval, 0x01, 0x07, 0x04, "1 Coin  6 Credits"      }, \
							       \
	{0     , 0xfe, 0   , 8   , "Coin B"                 }, \
	{dipval, 0x01, 0x38, 0x38, "4 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x30, "3 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x28, "2 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x00, "1 Coin  1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x08, "1 Coin  2 Credits"      }, \
	{dipval, 0x01, 0x38, 0x10, "1 Coin  3 Credits"      }, \
	{dipval, 0x01, 0x38, 0x18, "1 Coin  4 Credits"      }, \
	{dipval, 0x01, 0x38, 0x20, "1 Coin  6 Credits"      }, 
	
#define CPS1_COINAGE_2(dipval) \
	{0     , 0xfe, 0   , 8   , "Coinage"                }, \
	{dipval, 0x01, 0x07, 0x07, "4 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x06, "3 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x05, "2 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x00, "1 Coin  1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x01, "1 Coin  2 Credits"      }, \
	{dipval, 0x01, 0x07, 0x02, "1 Coin  3 Credits"      }, \
	{dipval, 0x01, 0x07, 0x03, "1 Coin  4 Credits"      }, \
	{dipval, 0x01, 0x07, 0x04, "1 Coin  6 Credits"      }, 
	
#define CPS1_COINAGE_3(dipval) \
	{0     , 0xfe, 0   , 8   , "Coin A"                 }, \
	{dipval, 0x01, 0x07, 0x06, "4 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x05, "3 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x04, "2 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x07, "2 Coins 1 Credit (1 to cont)"}, \
	{dipval, 0x01, 0x07, 0x00, "1 Coin  1 Credit"       }, \
	{dipval, 0x01, 0x07, 0x01, "1 Coin  2 Credits"      }, \
	{dipval, 0x01, 0x07, 0x02, "1 Coin  3 Credits"      }, \
	{dipval, 0x01, 0x07, 0x03, "1 Coin  4 Credits"      }, \
							       \
	{0     , 0xfe, 0   , 8   , "Coin B"                 }, \
	{dipval, 0x01, 0x38, 0x30, "4 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x28, "3 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x20, "2 Coins 1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x38, "2 Coins 1 Credit (1 to cont)"}, \
	{dipval, 0x01, 0x38, 0x00, "1 Coin  1 Credit"       }, \
	{dipval, 0x01, 0x38, 0x08, "1 Coin  2 Credits"      }, \
	{dipval, 0x01, 0x38, 0x10, "1 Coin  3 Credits"      }, \
	{dipval, 0x01, 0x38, 0x18, "1 Coin  4 Credits"      }, 
	
#define CPS1_DIFFICULTY_1(dipval) \
	{0   , 0xfe, 0   , 8   , "Difficulty"               }, \
	{dipval, 0x01, 0x07, 0x00, "1 (Easiest)"            }, \
	{dipval, 0x01, 0x07, 0x01, "2"                      }, \
	{dipval, 0x01, 0x07, 0x02, "3"                      }, \
	{dipval, 0x01, 0x07, 0x03, "4 (Normal)"             }, \
	{dipval, 0x01, 0x07, 0x04, "5"                      }, \
	{dipval, 0x01, 0x07, 0x05, "6"                      }, \
	{dipval, 0x01, 0x07, 0x06, "7"                      }, \
	{dipval, 0x01, 0x07, 0x07, "8 (Hardest)"            },
	
#define CPS1_DIFFICULTY_2(dipval) \
	{0   , 0xfe, 0   , 8   , "Difficulty"               }, \
	{dipval, 0x01, 0x07, 0x03, "1 (Easiest)"            }, \
	{dipval, 0x01, 0x07, 0x02, "2"                      }, \
	{dipval, 0x01, 0x07, 0x01, "3"                      }, \
	{dipval, 0x01, 0x07, 0x00, "4 (Normal)"             }, \
	{dipval, 0x01, 0x07, 0x04, "5"                      }, \
	{dipval, 0x01, 0x07, 0x05, "6"                      }, \
	{dipval, 0x01, 0x07, 0x06, "7"                      }, \
	{dipval, 0x01, 0x07, 0x07, "8 (Hardest)"            },

static struct BurnDIPInfo NTFODIPList[]=
{
	// Defaults
	{0x12, 0xff, 0xff, 0x00, NULL                     },
	{0x13, 0xff, 0xff, 0x03, NULL                     },
	{0x14, 0xff, 0xff, 0x60, NULL                     },

	// Dip A
	CPS1_COINAGE_1(0x12)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x12, 0x01, 0x40, 0x00, "Off"                    },
	{0x12, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x13)
	
	{0   , 0xfe, 0   , 4   , "Level Up Timer"         },
	{0x13, 0x01, 0x18, 0x00, "More Slowly"            },
	{0x13, 0x01, 0x18, 0x08, "Slowly"                 },
	{0x13, 0x01, 0x18, 0x10, "Quickly"                },
	{0x13, 0x01, 0x18, 0x18, "More Quickly"           },

	{0   , 0xfe, 0   , 4   , "Bullet's Speed"         },
	{0x13, 0x01, 0x60, 0x00, "Very Slow"              },
	{0x13, 0x01, 0x60, 0x20, "Slow"                   },
	{0x13, 0x01, 0x60, 0x40, "Fast"                   },
	{0x13, 0x01, 0x60, 0x60, "Very Fast"              },

	{0   , 0xfe, 0   , 2   , "Initital Vitality"      },
	{0x13, 0x01, 0x80, 0x00, "3 bars"                 },
	{0x13, 0x01, 0x80, 0x80, "4 bars"                 },
	
	// Dip C
	{0   , 0xfe, 0   , 2   , "Throttle Game Speed"    },
	{0x14, 0x01, 0x01, 0x01, "Off"                    },
	{0x14, 0x01, 0x01, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x14, 0x01, 0x04, 0x00, "Off"                    },
	{0x14, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x14, 0x01, 0x08, 0x00, "Off"                    },
	{0x14, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x14, 0x01, 0x10, 0x00, "Off"                    },
	{0x14, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x14, 0x01, 0x20, 0x00, "Off"                    },
	{0x14, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x14, 0x01, 0x40, 0x00, "Off"                    },
	{0x14, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x14, 0x01, 0x80, 0x00, "Game"                   },
	{0x14, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(NTFO);

static struct BurnDIPInfo ThreeWondersDIPList[]=
{
	// Defaults
	{0x13, 0xff, 0xff, 0x00, NULL                     },
	{0x14, 0xff, 0xff, 0x65, NULL                     },
	{0x15, 0xff, 0xff, 0x66, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x13)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x13, 0x01, 0x40, 0x00, "Off"                    },
	{0x13, 0x01, 0x40, 0x40, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x13, 0x01, 0x80, 0x00, "Off"                    },
	{0x13, 0x01, 0x80, 0x80, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 4   , "Action Lives"           },
	{0x14, 0x01, 0x03, 0x00, "1"                      },
	{0x14, 0x01, 0x03, 0x01, "2"                      },
	{0x14, 0x01, 0x03, 0x02, "3"                      },
	{0x14, 0x01, 0x03, 0x03, "5"                      },

	{0   , 0xfe, 0   , 4   , "Action Game Level"      },
	{0x14, 0x01, 0x0c, 0x00, "Easy"                   },
	{0x14, 0x01, 0x0c, 0x04, "Normal"                 },
	{0x14, 0x01, 0x0c, 0x08, "Hard"                   },
	{0x14, 0x01, 0x0c, 0x0c, "Hardest"                },

	{0   , 0xfe, 0   , 4   , "Shooting Lives"         },
	{0x14, 0x01, 0x30, 0x00, "1"                      },
	{0x14, 0x01, 0x30, 0x10, "2"                      },
	{0x14, 0x01, 0x30, 0x20, "3"                      },
	{0x14, 0x01, 0x30, 0x30, "5"                      },

	{0   , 0xfe, 0   , 4   , "Shooting Game Level"    },
	{0x14, 0x01, 0xc0, 0x00, "Easy"                   },
	{0x14, 0x01, 0xc0, 0x40, "Normal"                 },
	{0x14, 0x01, 0xc0, 0x80, "Hard"                   },
	{0x14, 0x01, 0xc0, 0xc0, "Hardest"                },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Puzzle Lives"           },
	{0x15, 0x01, 0x03, 0x00, "1"                      },
	{0x15, 0x01, 0x03, 0x01, "2"                      },
	{0x15, 0x01, 0x03, 0x02, "3"                      },
	{0x15, 0x01, 0x03, 0x03, "5"                      },

	{0   , 0xfe, 0   , 4   , "Puzzle Game Level"      },
	{0x15, 0x01, 0x0c, 0x00, "Easy"                   },
	{0x15, 0x01, 0x0c, 0x04, "Normal"                 },
	{0x15, 0x01, 0x0c, 0x08, "Hard"                   },
	{0x15, 0x01, 0x0c, 0x0c, "Hardest"                },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x15, 0x01, 0x10, 0x00, "Off"                    },
	{0x15, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x15, 0x01, 0x20, 0x00, "Off"                    },
	{0x15, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x15, 0x01, 0x40, 0x00, "Off"                    },
	{0x15, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x15, 0x01, 0x80, 0x00, "Game"                   },
	{0x15, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(ThreeWonders);

static struct BurnDIPInfo CaptcommDIPList[]=
{
	// Defaults
	{0x23, 0xff, 0xff, 0x00, NULL                     },
	{0x24, 0xff, 0xff, 0x0b, NULL                     },
	{0x25, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x23)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x23, 0x01, 0x40, 0x00, "Off"                    },
	{0x23, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x24)

	{0   , 0xfe, 0   , 4   , "Difficulty 2"           },
	{0x24, 0x01, 0x18, 0x00, "Easy"                   },
	{0x24, 0x01, 0x18, 0x08, "Normal"                 },
	{0x24, 0x01, 0x18, 0x10, "Hard"                   },
	{0x24, 0x01, 0x18, 0x18, "Hardest"                },

	{0   , 0xfe, 0   , 4   , "Play Mode"              },
	{0x24, 0x01, 0xc0, 0x80, "1 Player"               },
	{0x24, 0x01, 0xc0, 0x00, "2 Player"               },
	{0x24, 0x01, 0xc0, 0x40, "3 Player"               },
	{0x24, 0x01, 0xc0, 0xc0, "4 Player"               },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x25, 0x01, 0x03, 0x03, "1"                      },
	{0x25, 0x01, 0x03, 0x00, "2"                      },
	{0x25, 0x01, 0x03, 0x01, "3"                      },
	{0x25, 0x01, 0x03, 0x02, "4"                      },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x25, 0x01, 0x04, 0x00, "Off"                    },
	{0x25, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x25, 0x01, 0x08, 0x00, "Off"                    },
	{0x25, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x25, 0x01, 0x10, 0x00, "Off"                    },
	{0x25, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x25, 0x01, 0x20, 0x00, "Off"                    },
	{0x25, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x25, 0x01, 0x40, 0x00, "Off"                    },
	{0x25, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x25, 0x01, 0x80, 0x00, "Game"                   },
	{0x25, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Captcomm);

static struct BurnDIPInfo CawingDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x03, NULL                     },
	{0x17, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x15)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x15, 0x01, 0x40, 0x00, "Off"                    },
	{0x15, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 8   , "Difficulty (Enemys strength)"},
	{0x16, 0x01, 0x07, 0x00, "1 (Easiest)"            },
	{0x16, 0x01, 0x07, 0x01, "2"                      },
	{0x16, 0x01, 0x07, 0x02, "3"                      },
	{0x16, 0x01, 0x07, 0x03, "4 (Normal)"             },
	{0x16, 0x01, 0x07, 0x04, "5"                      },
	{0x16, 0x01, 0x07, 0x05, "6"                      },
	{0x16, 0x01, 0x07, 0x06, "7"                      },
	{0x16, 0x01, 0x07, 0x07, "8 (Hardest)"            },

	{0   , 0xfe, 0   , 4   , "Difficulty (Players strength)"},
	{0x16, 0x01, 0x18, 0x08, "Easy"                   },
	{0x16, 0x01, 0x18, 0x00, "Normal"                 },
	{0x16, 0x01, 0x18, 0x10, "Difficult"              },
	{0x16, 0x01, 0x18, 0x18, "Very Difficult"         },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x17, 0x01, 0x04, 0x00, "Off"                    },
	{0x17, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x17, 0x01, 0x08, 0x00, "Off"                    },
	{0x17, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Sound"                  },
	{0x17, 0x01, 0x20, 0x00, "Off"                    },
	{0x17, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x00, "Off"                    },
	{0x17, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Cawing);

static struct BurnDIPInfo Cworld2jDIPList[]=
{
	// Defaults
	{0x0f, 0xff, 0xff, 0x00, NULL                     },
	{0x10, 0xff, 0xff, 0x01, NULL                     },
	{0x11, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x0f)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x0f, 0x01, 0x40, 0x00, "Off"                    },
	{0x0f, 0x01, 0x40, 0x40, "On"                     },

	{0   , 0xfe, 0   , 2   , "Extended Test Mode"     },
	{0x0f, 0x01, 0x80, 0x00, "Off"                    },
	{0x0f, 0x01, 0x80, 0x80, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x10, 0x01, 0x07, 0x01, "0"                      },
	{0x10, 0x01, 0x07, 0x02, "1"                      },
	{0x10, 0x01, 0x07, 0x03, "2"                      },
	{0x10, 0x01, 0x07, 0x04, "3"                      },
	{0x10, 0x01, 0x07, 0x05, "4"                      },

	{0   , 0xfe, 0   , 3   , "Extend"                 },
	{0x10, 0x01, 0x18, 0x18, "D"                      },
	{0x10, 0x01, 0x18, 0x08, "E"                      },
	{0x10, 0x01, 0x18, 0x00, "N"                      },

	{0   , 0xfe, 0   , 5   , "Lives"                  },
	{0x10, 0x01, 0xe0, 0xe0, "1"                      },
	{0x10, 0x01, 0xe0, 0x60, "2"                      },
	{0x10, 0x01, 0xe0, 0x00, "3"                      },
	{0x10, 0x01, 0xe0, 0x40, "4"                      },
	{0x10, 0x01, 0xe0, 0x20, "5"                      },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x11, 0x01, 0x04, 0x00, "Off"                    },
	{0x11, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x11, 0x01, 0x08, 0x00, "Off"                    },
	{0x11, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x11, 0x01, 0x10, 0x00, "Off"                    },
	{0x11, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x11, 0x01, 0x20, 0x00, "Off"                    },
	{0x11, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x11, 0x01, 0x40, 0x00, "Off"                    },
	{0x11, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x11, 0x01, 0x80, 0x00, "Game"                   },
	{0x11, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Cworld2j);

static struct BurnDIPInfo DinoDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1b, 0x01, 0x08, 0x00, "Off"                    },
	{0x1b, 0x01, 0x08, 0x08, "On"                     },
};

STDDIPINFO(Dino);

static struct BurnDIPInfo DinohDIPList[]=
{
	// Defaults
	{0x1d, 0xff, 0xff, 0x00, NULL                     },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1d, 0x01, 0x08, 0x00, "Off"                    },
	{0x1d, 0x01, 0x08, 0x08, "On"                     },
};

STDDIPINFO(Dinoh);

static struct BurnDIPInfo DynwarDIPList[]=
{
	// Defaults
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
        CPS1_COINAGE_3(0x14)

        {0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x14, 0x01, 0x80, 0x00, "Off"                    },
	{0x14, 0x01, 0x80, 0x80, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_2(0x15)
	
	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x16, 0x01, 0x01, 0x00, "Off"                    },
	{0x16, 0x01, 0x01, 0x01, "On"                     },

	{0   , 0xfe, 0   , 2   , "Turbo Mode"             },
	{0x16, 0x01, 0x02, 0x00, "Off"                    },
	{0x16, 0x01, 0x02, 0x02, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x16, 0x01, 0x10, 0x00, "Off"                    },
	{0x16, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x16, 0x01, 0x20, 0x20, "Off"                    },
	{0x16, 0x01, 0x20, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x16, 0x01, 0x40, 0x40, "Off"                    },
	{0x16, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x16, 0x01, 0x80, 0x00, "Game"                   },
	{0x16, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Dynwar);

static struct BurnDIPInfo FfightDIPList[]=
{
	// Defaults
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	{0x15, 0xff, 0xff, 0x0b, NULL                     },
	{0x16, 0xff, 0xff, 0x60, NULL                     },
		
	// Dip A
	CPS1_COINAGE_1(0x14)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x14, 0x01, 0x40, 0x00, "Off"                    },
	{0x14, 0x01, 0x40, 0x40, "On"                     },
		
	// Dip B
	CPS1_DIFFICULTY_1(0x15)

	{0   , 0xfe, 0   , 4   , "Difficulty 2"           },
	{0x15, 0x01, 0x18, 0x00, "Easy"                   },
	{0x15, 0x01, 0x18, 0x08, "Normal"                 },
	{0x15, 0x01, 0x18, 0x10, "Hard"                   },
	{0x15, 0x01, 0x18, 0x18, "Hardest"                },

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x15, 0x01, 0x60, 0x00, "100k"                   },
	{0x15, 0x01, 0x60, 0x20, "200k"                   },
	{0x15, 0x01, 0x60, 0x40, "100k and every 200k"    },
	{0x15, 0x01, 0x60, 0x60, "None"                   },
	
	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x16, 0x01, 0x03, 0x03, "1"                      },
	{0x16, 0x01, 0x03, 0x00, "2"                      },
	{0x16, 0x01, 0x03, 0x01, "3"                      },
	{0x16, 0x01, 0x03, 0x02, "4"                      },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x16, 0x01, 0x04, 0x00, "Off"                    },
	{0x16, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x16, 0x01, 0x08, 0x00, "Off"                    },
	{0x16, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x16, 0x01, 0x10, 0x00, "Off"                    },
	{0x16, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x16, 0x01, 0x20, 0x00, "Off"                    },
	{0x16, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x16, 0x01, 0x40, 0x00, "Off"                    },
	{0x16, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x16, 0x01, 0x80, 0x00, "Game"                   },
	{0x16, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Ffight);

static struct BurnDIPInfo ForgottnDIPList[]=
{
	// Defaults
	{0x12, 0xff, 0xff, 0x00, NULL                     },
	{0x13, 0xff, 0xff, 0x03, NULL                     },
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x12)

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x12, 0x01, 0x40, 0x40, "Off"                    },
	{0x12, 0x01, 0x40, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x12, 0x01, 0x80, 0x00, "Off"                    },
	{0x12, 0x01, 0x80, 0x80, "On"                     },
	
	// Dip B
	CPS1_DIFFICULTY_1(0x13)
	
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x13, 0x01, 0x40, 0x00, "Off"                    },
	{0x13, 0x01, 0x40, 0x40, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x13, 0x01, 0x80, 0x00, "Off"                    },
	{0x13, 0x01, 0x80, 0x80, "On"                     },
};

STDDIPINFO(Forgottn);

static struct BurnDIPInfo GhoulsDIPList[]=
{
	// Defaults
	{0x12, 0xff, 0xff, 0x00, NULL                     },
	{0x13, 0xff, 0xff, 0x00, NULL                     },
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x12)

	{0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x12, 0x01, 0xc0, 0x00, "Upright 1 Player"       },
	{0x12, 0x01, 0xc0, 0x40, "Upright 2 Players"      },
	{0x12, 0x01, 0xc0, 0xc0, "Cocktail"               },

	// Dip B
	CPS1_DIFFICULTY_2(0x13)

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x13, 0x01, 0x30, 0x10, "10k, 30k and every 30k" },
	{0x13, 0x01, 0x30, 0x20, "20k, 50k and every 70k" },
	{0x13, 0x01, 0x30, 0x00, "30k, 60k and every 70k" },
	{0x13, 0x01, 0x30, 0x30, "40k, 70k and every 80k" },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x03, 0x00, "3"                      },
	{0x14, 0x01, 0x03, 0x01, "4"                      },
	{0x14, 0x01, 0x03, 0x02, "5"                      },
	{0x14, 0x01, 0x03, 0x03, "6"                      },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x14, 0x01, 0x10, 0x00, "Off"                    },
	{0x14, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x14, 0x01, 0x40, 0x40, "Off"                    },
	{0x14, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x14, 0x01, 0x80, 0x00, "Game"                   },
	{0x14, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Ghouls);

static struct BurnDIPInfo GhoulsuDIPList[]=
{
	// Defaults
	{0x12, 0xff, 0xff, 0x00, NULL                     },
	{0x13, 0xff, 0xff, 0x00, NULL                     },
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x12)

	{0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x12, 0x01, 0xc0, 0x00, "Upright 1 Player"       },
	{0x12, 0x01, 0xc0, 0x40, "Upright 2 Players"      },
	{0x12, 0x01, 0xc0, 0xc0, "Cocktail"               },
	
	// Dip B
	CPS1_DIFFICULTY_2(0x13)

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x13, 0x01, 0x30, 0x10, "10k, 30k and every 30k" },
	{0x13, 0x01, 0x30, 0x20, "20k, 50k and every 70k" },
	{0x13, 0x01, 0x30, 0x00, "30k, 60k and every 70k" },
	{0x13, 0x01, 0x30, 0x30, "40k, 70k and every 80k" },
	
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x13, 0x01, 0x80, 0x00, "Off"                    },
	{0x13, 0x01, 0x80, 0x80, "On"                     },
	
	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x03, 0x03, "2"                      },
	{0x14, 0x01, 0x03, 0x00, "3"                      },
	{0x14, 0x01, 0x03, 0x01, "4"                      },
	{0x14, 0x01, 0x03, 0x02, "5"                      },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x14, 0x01, 0x10, 0x00, "Off"                    },
	{0x14, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x14, 0x01, 0x40, 0x40, "Off"                    },
	{0x14, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x14, 0x01, 0x80, 0x00, "Game"                   },
	{0x14, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Ghoulsu);

static struct BurnDIPInfo DaimakaiDIPList[]=
{
	// Defaults
	{0x12, 0xff, 0xff, 0x00, NULL                     },
	{0x13, 0xff, 0xff, 0x00, NULL                     },
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x12)

	{0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x12, 0x01, 0xc0, 0x00, "Upright 1 Player"       },
	{0x12, 0x01, 0xc0, 0x40, "Upright 2 Players"      },
	{0x12, 0x01, 0xc0, 0xc0, "Cocktail"               },

	// Dip B
	CPS1_DIFFICULTY_2(0x13)

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x13, 0x01, 0x30, 0x10, "10k, 30k and every 30k" },
	{0x13, 0x01, 0x30, 0x20, "20k, 50k and every 70k" },
	{0x13, 0x01, 0x30, 0x00, "30k, 60k and every 70k" },
	{0x13, 0x01, 0x30, 0x30, "40k, 70k and every 80k" },
	
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x13, 0x01, 0x80, 0x00, "Off"                    },
	{0x13, 0x01, 0x80, 0x80, "On"                     },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x14, 0x01, 0x03, 0x00, "3"                      },
	{0x14, 0x01, 0x03, 0x01, "4"                      },
	{0x14, 0x01, 0x03, 0x02, "5"                      },
	{0x14, 0x01, 0x03, 0x03, "6"                      },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x14, 0x01, 0x10, 0x00, "Off"                    },
	{0x14, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x14, 0x01, 0x40, 0x40, "Off"                    },
	{0x14, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x14, 0x01, 0x80, 0x00, "Game"                   },
	{0x14, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Daimakai);

static struct BurnDIPInfo KnightsDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x03, NULL                     },
	{0x1d, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x1b)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x1b, 0x01, 0x40, 0x00, "Off"                    },
	{0x1b, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 8   , "Enemys Attack Frequency"},
	{0x1c, 0x01, 0x07, 0x00, "1 (Easiest)"            },
	{0x1c, 0x01, 0x07, 0x01, "2"                      },
	{0x1c, 0x01, 0x07, 0x02, "3"                      },
	{0x1c, 0x01, 0x07, 0x03, "4 (Normal)"             },
	{0x1c, 0x01, 0x07, 0x04, "5"                      },
	{0x1c, 0x01, 0x07, 0x05, "6"                      },
	{0x1c, 0x01, 0x07, 0x06, "7"                      },
	{0x1c, 0x01, 0x07, 0x07, "8 (Hardest)"            },

	{0   , 0xfe, 0   , 8   , "Enemys Attack Power"    },
	{0x1c, 0x01, 0x38, 0x38, "1 (Easiest)"            },
	{0x1c, 0x01, 0x38, 0x30, "2"                      },
	{0x1c, 0x01, 0x38, 0x28, "3"                      },
	{0x1c, 0x01, 0x38, 0x00, "4 (Normal)"             },
	{0x1c, 0x01, 0x38, 0x08, "5"                      },
	{0x1c, 0x01, 0x38, 0x10, "6"                      },
	{0x1c, 0x01, 0x38, 0x18, "7"                      },
	{0x1c, 0x01, 0x38, 0x20, "8 (Hardest)"            },

	{0   , 0xfe, 0   , 2   , "Coin Slots"             },
	{0x1c, 0x01, 0x40, 0x40, "1"                      },
	{0x1c, 0x01, 0x40, 0x00, "3"                      },

	{0   , 0xfe, 0   , 2   , "Play Mode"              },
	{0x1c, 0x01, 0x80, 0x80, "2P"                     },
	{0x1c, 0x01, 0x80, 0x00, "3P"                     },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x1d, 0x01, 0x03, 0x03, "1"                      },
	{0x1d, 0x01, 0x03, 0x00, "2"                      },
	{0x1d, 0x01, 0x03, 0x01, "3"                      },
	{0x1d, 0x01, 0x03, 0x02, "4"                      },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x1d, 0x01, 0x04, 0x00, "Off"                    },
	{0x1d, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1d, 0x01, 0x08, 0x00, "Off"                    },
	{0x1d, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x1d, 0x01, 0x10, 0x00, "Off"                    },
	{0x1d, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x1d, 0x01, 0x20, 0x00, "Off"                    },
	{0x1d, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x1d, 0x01, 0x40, 0x00, "Off"                    },
	{0x1d, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x1d, 0x01, 0x80, 0x00, "Game"                   },
	{0x1d, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Knights);

static struct BurnDIPInfo KodDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x03, NULL                     },
	{0x1d, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x1b)

	{0   , 0xfe, 0   , 2   , "Coin Slots"             },
	{0x1b, 0x01, 0x08, 0x08, "1"                      },
	{0x1b, 0x01, 0x08, 0x00, "3"                      },

	{0   , 0xfe, 0   , 2   , "Play Mode"              },
	{0x1b, 0x01, 0x10, 0x10, "2 Players"              },
	{0x1b, 0x01, 0x10, 0x00, "3 Players"              },

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x1b, 0x01, 0x40, 0x00, "Off"                    },
	{0x1b, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x1c)

	{0   , 0xfe, 0   , 8   , "Lives"                  },
	{0x1c, 0x01, 0x38, 0x08, "1"                      },
	{0x1c, 0x01, 0x38, 0x00, "2"                      },
	{0x1c, 0x01, 0x38, 0x10, "3"                      },
	{0x1c, 0x01, 0x38, 0x18, "4"                      },
	{0x1c, 0x01, 0x38, 0x20, "5"                      },
	{0x1c, 0x01, 0x38, 0x28, "6"                      },
	{0x1c, 0x01, 0x38, 0x30, "7"                      },
	{0x1c, 0x01, 0x38, 0x38, "8"                      },

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x1c, 0x01, 0xc0, 0x40, "80k and every 400k"     },
	{0x1c, 0x01, 0xc0, 0x00, "160k and every 450k"    },
	{0x1c, 0x01, 0xc0, 0x80, "200k and every 450k"    },
	{0x1c, 0x01, 0xc0, 0xc0, "None"                   },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x1d, 0x01, 0x04, 0x00, "Off"                    },
	{0x1d, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1d, 0x01, 0x08, 0x00, "Off"                    },
	{0x1d, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x1d, 0x01, 0x10, 0x00, "Off"                    },
	{0x1d, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x1d, 0x01, 0x20, 0x00, "Off"                    },
	{0x1d, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x1d, 0x01, 0x40, 0x00, "Off"                    },
	{0x1d, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x1d, 0x01, 0x80, 0x00, "Game"                   },
	{0x1d, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Kod);

static struct BurnDIPInfo KodjDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x03, NULL                     },
	{0x1d, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x1b)

	{0   , 0xfe, 0   , 2   , "Coin Slots"             },
	{0x1b, 0x01, 0x08, 0x08, "1"                      },
	{0x1b, 0x01, 0x08, 0x00, "3"                      },

	{0   , 0xfe, 0   , 2   , "Play Mode"              },
	{0x1b, 0x01, 0x10, 0x10, "2 Players"              },
	{0x1b, 0x01, 0x10, 0x00, "3 Players"              },

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x1b, 0x01, 0x40, 0x00, "Off"                    },
	{0x1b, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x1c)

	{0   , 0xfe, 0   , 8   , "Lives"                  },
	{0x1c, 0x01, 0x38, 0x08, "1"                      },
	{0x1c, 0x01, 0x38, 0x00, "2"                      },
	{0x1c, 0x01, 0x38, 0x10, "3"                      },
	{0x1c, 0x01, 0x38, 0x18, "4"                      },
	{0x1c, 0x01, 0x38, 0x20, "5"                      },
	{0x1c, 0x01, 0x38, 0x28, "6"                      },
	{0x1c, 0x01, 0x38, 0x30, "7"                      },
	{0x1c, 0x01, 0x38, 0x38, "8"                      },

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x1c, 0x01, 0xc0, 0x40, "80k and every 400k"     },
	{0x1c, 0x01, 0xc0, 0x80, "160k and every 450k"    },
	{0x1c, 0x01, 0xc0, 0x00, "200k and every 450k"    },
	{0x1c, 0x01, 0xc0, 0xc0, "None"                   },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x1d, 0x01, 0x04, 0x00, "Off"                    },
	{0x1d, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1d, 0x01, 0x08, 0x00, "Off"                    },
	{0x1d, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x1d, 0x01, 0x10, 0x00, "Off"                    },
	{0x1d, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x1d, 0x01, 0x20, 0x00, "Off"                    },
	{0x1d, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x1d, 0x01, 0x40, 0x00, "Off"                    },
	{0x1d, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x1d, 0x01, 0x80, 0x00, "Game"                   },
	{0x1d, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Kodj);

static struct BurnDIPInfo MegamanDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x01, NULL                     },
	{0x17, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	{0   , 0xfe, 0   , 19  , "Coinage"                },
	{0x15, 0x01, 0x1f, 0x00, "1 Coin  1 Credit"       },
	{0x15, 0x01, 0x1f, 0x01, "1 Coin  2 Credits"      },
	{0x15, 0x01, 0x1f, 0x02, "1 Coin  3 Credits"      },
	{0x15, 0x01, 0x1f, 0x03, "1 Coin  4 Credits"      },
	{0x15, 0x01, 0x1f, 0x04, "1 Coin  5 Credits"      },
	{0x15, 0x01, 0x1f, 0x05, "1 Coin  6 Credits"      },
	{0x15, 0x01, 0x1f, 0x06, "1 Coin  7 Credits"      },
	{0x15, 0x01, 0x1f, 0x07, "1 Coin  8 Credits"      },
	{0x15, 0x01, 0x1f, 0x08, "1 Coin  9 Credits"      },
	{0x15, 0x01, 0x1f, 0x09, "2 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0a, "3 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0b, "4 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0c, "5 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0d, "6 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0e, "7 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0f, "8 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x10, "9 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x11, "2 Coins Start 1 Coin Continue"},
	{0x15, 0x01, 0x1f, 0x12, "Free Play"              },

	{0   , 0xfe, 0   , 4   , "Coin Slots"             },
	{0x15, 0x01, 0x60, 0x40, "1, Common"              },
	{0x15, 0x01, 0x60, 0x00, "2, Common"              },
	{0x15, 0x01, 0x60, 0x20, "2, Individual"          },

	// Dip B
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x16, 0x01, 0x03, 0x00, "Easy"                   },
	{0x16, 0x01, 0x03, 0x01, "Normal"                 },
	{0x16, 0x01, 0x03, 0x02, "Hard"                   },
	{0x16, 0x01, 0x03, 0x03, "Hardest"                },

	{0   , 0xfe, 0   , 4   , "Time"                   },
	{0x16, 0x01, 0x0c, 0x00, "100"                    },
	{0x16, 0x01, 0x0c, 0x04, "90"                     },
	{0x16, 0x01, 0x0c, 0x08, "70"                     },
	{0x16, 0x01, 0x0c, 0x0c, "60"                     },

	{0   , 0xfe, 0   , 2   , "Voice"                  },
	{0x16, 0x01, 0x40, 0x40, "Off"                    },
	{0x16, 0x01, 0x40, 0x00, "On"                     },
	
	// Dip C
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x17, 0x01, 0x01, 0x00, "Off"                    },
	{0x17, 0x01, 0x01, 0x01, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x17, 0x01, 0x02, 0x02, "Off"                    },
	{0x17, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x04, 0x04, "Off"                    },
	{0x17, 0x01, 0x04, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },	
};

STDDIPINFO(Megaman);

static struct BurnDIPInfo RockmanjDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x01, NULL                     },
	{0x17, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	{0   , 0xfe, 0   , 19  , "Coinage"                },
	{0x15, 0x01, 0x1f, 0x00, "1 Coin  1 Credit"       },
	{0x15, 0x01, 0x1f, 0x01, "1 Coin  2 Credits"      },
	{0x15, 0x01, 0x1f, 0x02, "1 Coin  3 Credits"      },
	{0x15, 0x01, 0x1f, 0x03, "1 Coin  4 Credits"      },
	{0x15, 0x01, 0x1f, 0x04, "1 Coin  5 Credits"      },
	{0x15, 0x01, 0x1f, 0x05, "1 Coin  6 Credits"      },
	{0x15, 0x01, 0x1f, 0x06, "1 Coin  7 Credits"      },
	{0x15, 0x01, 0x1f, 0x07, "1 Coin  8 Credits"      },
	{0x15, 0x01, 0x1f, 0x08, "1 Coin  9 Credits"      },
	{0x15, 0x01, 0x1f, 0x09, "2 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0a, "3 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0b, "4 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0c, "5 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0d, "6 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0e, "7 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x0f, "8 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x10, "9 Coins 1 Credit"       },
	{0x15, 0x01, 0x1f, 0x11, "2 Coins Start 1 Coin Continue"},
	{0x15, 0x01, 0x1f, 0x12, "Free Play"              },

	{0   , 0xfe, 0   , 4   , "Coin Slots"             },
	{0x15, 0x01, 0x60, 0x40, "1, Common"              },
	{0x15, 0x01, 0x60, 0x00, "2, Common"              },
	{0x15, 0x01, 0x60, 0x20, "2, Individual"          },

	// Dip B
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x16, 0x01, 0x03, 0x00, "Easy"                   },
	{0x16, 0x01, 0x03, 0x01, "Normal"                 },
	{0x16, 0x01, 0x03, 0x02, "Hard"                   },
	{0x16, 0x01, 0x03, 0x03, "Hardest"                },

	{0   , 0xfe, 0   , 4   , "Time"                   },
	{0x16, 0x01, 0x0c, 0x00, "100"                    },
	{0x16, 0x01, 0x0c, 0x04, "90"                     },
	{0x16, 0x01, 0x0c, 0x08, "70"                     },
	{0x16, 0x01, 0x0c, 0x0c, "60"                     },
	
	// Dip C
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x17, 0x01, 0x01, 0x00, "Off"                    },
	{0x17, 0x01, 0x01, 0x01, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x17, 0x01, 0x02, 0x02, "Off"                    },
	{0x17, 0x01, 0x02, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x04, 0x04, "Off"                    },
	{0x17, 0x01, 0x04, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },	
};

STDDIPINFO(Rockmanj);

static struct BurnDIPInfo MercsDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x03, NULL                     },
	{0x1d, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x1b)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x1b, 0x01, 0x40, 0x00, "Off"                    },
	{0x1b, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x1c)

	{0   , 0xfe, 0   , 2   , "Coin Slots"             },
	{0x1c, 0x01, 0x08, 0x08, "1"                      },
	{0x1c, 0x01, 0x08, 0x00, "3"                      },

	{0   , 0xfe, 0   , 2   , "Play Mode"              },
	{0x1c, 0x01, 0x10, 0x10, "2 Player"               },
	{0x1c, 0x01, 0x10, 0x00, "3 Player"               },
	
	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1d, 0x01, 0x08, 0x00, "Off"                    },
	{0x1d, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x1d, 0x01, 0x10, 0x00, "Off"                    },
	{0x1d, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x1d, 0x01, 0x20, 0x00, "Off"                    },
	{0x1d, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x1d, 0x01, 0x40, 0x00, "Off"                    },
	{0x1d, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x1d, 0x01, 0x80, 0x00, "Off"                    },
	{0x1d, 0x01, 0x80, 0x80, "On"                     },
};

STDDIPINFO(Mercs);

static struct BurnDIPInfo MswordDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x03, NULL                     },
	{0x17, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x15)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x15, 0x01, 0x40, 0x00, "Off"                    },
	{0x15, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 8   , "Players vitality consumption"},
	{0x16, 0x01, 0x07, 0x00, "1 (Easiest)"            },
	{0x16, 0x01, 0x07, 0x01, "2"                      },
	{0x16, 0x01, 0x07, 0x02, "3"                      },
	{0x16, 0x01, 0x07, 0x03, "4 (Normal)"             },
	{0x16, 0x01, 0x07, 0x04, "5"                      },
	{0x16, 0x01, 0x07, 0x05, "6"                      },
	{0x16, 0x01, 0x07, 0x06, "7"                      },
	{0x16, 0x01, 0x07, 0x07, "8 (Hardest)"            },

	{0   , 0xfe, 0   , 8   , "Level 2"                },
	{0x16, 0x01, 0x38, 0x18, "1 (Easiest)"            },
	{0x16, 0x01, 0x38, 0x10, "2"                      },
	{0x16, 0x01, 0x38, 0x08, "3"                      },
	{0x16, 0x01, 0x38, 0x00, "4 (Normal)"             },
	{0x16, 0x01, 0x38, 0x20, "5"                      },
	{0x16, 0x01, 0x38, 0x28, "6"                      },
	{0x16, 0x01, 0x38, 0x30, "7"                      },
	{0x16, 0x01, 0x38, 0x38, "8 (Hardest)"            },

	{0   , 0xfe, 0   , 2   , "Stage Select"           },
	{0x16, 0x01, 0x40, 0x00, "Off"                    },
	{0x16, 0x01, 0x40, 0x40, "On"                     },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Vitality"               },
	{0x17, 0x01, 0x03, 0x03, "1"                      },
	{0x17, 0x01, 0x03, 0x00, "2"                      },
	{0x17, 0x01, 0x03, 0x01, "3 (2 when continue)"    },
	{0x17, 0x01, 0x03, 0x02, "4 (3 when continue)"    },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x17, 0x01, 0x04, 0x00, "Off"                    },
	{0x17, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x17, 0x01, 0x08, 0x00, "Off"                    },
	{0x17, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x17, 0x01, 0x20, 0x00, "Off"                    },
	{0x17, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x00, "Off"                    },
	{0x17, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Msword);

static struct BurnDIPInfo MtwinsDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x03, NULL                     },
	{0x17, 0xff, 0xff, 0x60, NULL                     },

	// Dip A
	CPS1_COINAGE_1(0x15)

	// Dip B
	CPS1_DIFFICULTY_1(0x16)

	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x16, 0x01, 0x38, 0x28, "1"                      },
	{0x16, 0x01, 0x38, 0x20, "2"                      },
	{0x16, 0x01, 0x38, 0x30, "3"                      },
	{0x16, 0x01, 0x38, 0x38, "4"                      },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x17, 0x01, 0x04, 0x00, "Off"                    },
	{0x17, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x17, 0x01, 0x08, 0x00, "Off"                    },
	{0x17, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x17, 0x01, 0x20, 0x00, "Off"                    },
	{0x17, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x00, "Off"                    },
	{0x17, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Mtwins);

static struct BurnDIPInfo NemoDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x03, NULL                     },
	{0x17, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x15)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x15, 0x01, 0x40, 0x00, "Off"                    },
	{0x15, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x16)

	{0   , 0xfe, 0   , 3   , "Life Bar"               },
	{0x16, 0x01, 0x18, 0x18, "Minimum"                },
	{0x16, 0x01, 0x18, 0x00, "Medium"                 },
	{0x16, 0x01, 0x18, 0x10, "Maximum"                },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x17, 0x01, 0x03, 0x01, "1"                      },
	{0x17, 0x01, 0x03, 0x00, "2"                      },
	{0x17, 0x01, 0x03, 0x02, "3"                      },
	{0x17, 0x01, 0x03, 0x03, "4"                      },
	
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x17, 0x01, 0x04, 0x00, "Off"                    },
	{0x17, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x17, 0x01, 0x08, 0x00, "Off"                    },
	{0x17, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x17, 0x01, 0x20, 0x00, "Off"                    },
	{0x17, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x00, "Off"                    },
	{0x17, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Nemo);

static struct BurnDIPInfo Pang3DIPList[]=
{
	// Defaults
	{0x13, 0xff, 0xff, 0x00, NULL                     },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x13, 0x01, 0x08, 0x00, "Off"                    },
	{0x13, 0x01, 0x08, 0x08, "On"                     },
};

STDDIPINFO(Pang3);

static struct BurnDIPInfo PnickjDIPList[]=
{
	// Defaults
	{0x13, 0xff, 0xff, 0x00, NULL                     },
	{0x14, 0xff, 0xff, 0x03, NULL                     },
	{0x15, 0xff, 0xff, 0x20, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x13)

	{0   , 0xfe, 0   , 2   , "Coin Slots"             },
	{0x13, 0x01, 0x08, 0x00, "1"                      },
	{0x13, 0x01, 0x08, 0x08, "2"                      },
	
	// Dip B
	CPS1_DIFFICULTY_1(0x14)

	{0   , 0xfe, 0   , 4   , "Vs. Play Mode"          },
	{0x14, 0x01, 0xc0, 0x00, "1 Game  Match"          },
	{0x14, 0x01, 0xc0, 0x40, "3 Games Match"          },
	{0x14, 0x01, 0xc0, 0x80, "5 Games Match"          },
	{0x14, 0x01, 0xc0, 0xc0, "7 Games Match"          },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x15, 0x01, 0x08, 0x00, "Off"                    },
	{0x15, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x15, 0x01, 0x10, 0x00, "Off"                    },
	{0x15, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x15, 0x01, 0x20, 0x00, "Off"                    },
	{0x15, 0x01, 0x20, 0x20, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x15, 0x01, 0x80, 0x00, "Game"                   },
	{0x15, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Pnickj);

static struct BurnDIPInfo PunisherDIPList[]=
{
	// Defaults
	{0x13, 0xff, 0xff, 0x00, NULL                     },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x13, 0x01, 0x08, 0x00, "Off"                    },
	{0x13, 0x01, 0x08, 0x08, "On"                     },
};

STDDIPINFO(Punisher);

static struct BurnDIPInfo QadDIPList[]=
{
	// Defaults
	{0x0f, 0xff, 0xff, 0x00, NULL                     },
	{0x10, 0xff, 0xff, 0x0b, NULL                     },
	{0x11, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x0f)
	
	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x0f, 0x01, 0x40, 0x00, "Off"                    },
	{0x0f, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 5   , "Difficulty"             },
	{0x10, 0x01, 0x07, 0x01, "Easiest"                },
	{0x10, 0x01, 0x07, 0x02, "Easy"                   },
	{0x10, 0x01, 0x07, 0x03, "Normal"                 },
	{0x10, 0x01, 0x07, 0x04, "Hard"                   },
	{0x10, 0x01, 0x07, 0x05, "Hardest"                },

	{0   , 0xfe, 0   , 4   , "Wisdom of Enemy"        },
	{0x10, 0x01, 0x18, 0x00, "Easy"                   },
	{0x10, 0x01, 0x18, 0x08, "Normal"                 },
	{0x10, 0x01, 0x18, 0x10, "Hard"                   },
	{0x10, 0x01, 0x18, 0x18, "Hardest"                },

	{0   , 0xfe, 0   , 5   , "Lives"                  },
	{0x10, 0x01, 0xe0, 0x80, "1"                      },
	{0x10, 0x01, 0xe0, 0x60, "2"                      },
	{0x10, 0x01, 0xe0, 0x40, "3"                      },
	{0x10, 0x01, 0xe0, 0x20, "4"                      },
	{0x10, 0x01, 0xe0, 0x00, "5"                      },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x11, 0x01, 0x04, 0x00, "Off"                    },
	{0x11, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x11, 0x01, 0x08, 0x00, "Off"                    },
	{0x11, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x11, 0x01, 0x10, 0x00, "Off"                    },
	{0x11, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x11, 0x01, 0x20, 0x20, "Off"                    },
	{0x11, 0x01, 0x20, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x11, 0x01, 0x40, 0x40, "Off"                    },
	{0x11, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x11, 0x01, 0x80, 0x00, "Game"                   },
	{0x11, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Qad);

static struct BurnDIPInfo QadjDIPList[]=
{
	// Defaults
	{0x0f, 0xff, 0xff, 0x00, NULL                     },
	{0x10, 0xff, 0xff, 0x00, NULL                     },
	{0x11, 0xff, 0xff, 0x20, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x0f)
	
	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x0f, 0x01, 0x40, 0x00, "Off"                    },
	{0x0f, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	{0   , 0xfe, 0   , 5   , "Difficulty"             },
	{0x10, 0x01, 0x07, 0x00, "0"                      },
	{0x10, 0x01, 0x07, 0x01, "1"                      },
	{0x10, 0x01, 0x07, 0x02, "2"                      },
	{0x10, 0x01, 0x07, 0x03, "3"                      },
	{0x10, 0x01, 0x07, 0x04, "4"                      },

	{0   , 0xfe, 0   , 3   , "Lives"                  },
	{0x10, 0x01, 0xe0, 0x40, "1"                      },
	{0x10, 0x01, 0xe0, 0x20, "2"                      },
	{0x10, 0x01, 0xe0, 0x00, "3"                      },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x11, 0x01, 0x04, 0x00, "Off"                    },
	{0x11, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x11, 0x01, 0x08, 0x00, "Off"                    },
	{0x11, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x11, 0x01, 0x10, 0x00, "Off"                    },
	{0x11, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x11, 0x01, 0x20, 0x00, "Off"                    },
	{0x11, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x11, 0x01, 0x40, 0x40, "Off"                    },
	{0x11, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x11, 0x01, 0x80, 0x00, "Game"                   },
	{0x11, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Qadj);

static struct BurnDIPInfo Qtono2DIPList[]=
{
	// Defaults
	{0x0f, 0xff, 0xff, 0x00, NULL                     },
	{0x10, 0xff, 0xff, 0x03, NULL                     },
	{0x11, 0xff, 0xff, 0x20, NULL                     },
	
	// Dip A
	CPS1_COINAGE_2(0x0f)
	
	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x0f, 0x01, 0x40, 0x00, "Off"                    },
	{0x0f, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x10)

	{0   , 0xfe, 0   , 5   , "Lives"                  },
	{0x10, 0x01, 0xe0, 0x80, "1"                      },
	{0x10, 0x01, 0xe0, 0x60, "2"                      },
	{0x10, 0x01, 0xe0, 0x00, "3"                      },
	{0x10, 0x01, 0xe0, 0x40, "4"                      },
	{0x10, 0x01, 0xe0, 0x20, "5"                      },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Infinite Lives"         },
	{0x11, 0x01, 0x02, 0x00, "Off"                    },
	{0x11, 0x01, 0x02, 0x02, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x11, 0x01, 0x04, 0x00, "Off"                    },
	{0x11, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x11, 0x01, 0x08, 0x00, "Off"                    },
	{0x11, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x11, 0x01, 0x10, 0x00, "Off"                    },
	{0x11, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x11, 0x01, 0x20, 0x00, "Off"                    },
	{0x11, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x11, 0x01, 0x40, 0x40, "Off"                    },
	{0x11, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x11, 0x01, 0x80, 0x00, "Game"                   },
	{0x11, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Qtono2);

static struct BurnDIPInfo Sf2DIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x03, NULL                     },
	{0x1d, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x1b)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x1b, 0x01, 0x40, 0x00, "Off"                    },
	{0x1b, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x1c)

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
        {0x1d, 0x01, 0x04, 0x00, "Off"                    },
        {0x1d, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
        {0x1d, 0x01, 0x08, 0x00, "Off"                    },
        {0x1d, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
        {0x1d, 0x01, 0x10, 0x00, "Off"                    },
        {0x1d, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
        {0x1d, 0x01, 0x20, 0x00, "Off"                    },
        {0x1d, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
        {0x1d, 0x01, 0x40, 0x00, "Off"                    },
        {0x1d, 0x01, 0x40, 0x40, "On"                     },
        
        {0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x1d, 0x01, 0x80, 0x00, "Game"                   },
	{0x1d, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Sf2);

static struct BurnDIPInfo Sf2jDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x03, NULL                     },
	{0x1d, 0xff, 0xff, 0x60, NULL                     },

	// Dip A
	CPS1_COINAGE_1(0x1b)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x1b, 0x01, 0x40, 0x00, "Off"                    },
	{0x1b, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x1c)

	{0   , 0xfe, 0   , 2   , "2 Players Game"         },
	{0x1c, 0x01, 0x08, 0x00, "1 Credit/No Continue"   },
	{0x1c, 0x01, 0x08, 0x08, "2 Credits/Winner Continue"},

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
        {0x1d, 0x01, 0x04, 0x00, "Off"                    },
        {0x1d, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
        {0x1d, 0x01, 0x08, 0x00, "Off"                    },
        {0x1d, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
        {0x1d, 0x01, 0x10, 0x00, "Off"                    },
        {0x1d, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
        {0x1d, 0x01, 0x20, 0x00, "Off"                    },
        {0x1d, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
        {0x1d, 0x01, 0x40, 0x00, "Off"                    },
        {0x1d, 0x01, 0x40, 0x40, "On"                     },
        
        {0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x1d, 0x01, 0x80, 0x00, "Game"                   },
	{0x1d, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Sf2j);

static struct BurnDIPInfo SlammastDIPList[]=
{
	// Defaults
	{0x23, 0xff, 0xff, 0x00, NULL                     },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x23, 0x01, 0x08, 0x08, "Off"                    },
	{0x23, 0x01, 0x08, 0x00, "On"                     },
};

STDDIPINFO(Slammast);

static struct BurnDIPInfo StriderDIPList[]=
{
	// Defaults
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x14)

	{0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x14, 0x01, 0xc0, 0x00, "Upright 1P"             },
	{0x14, 0x01, 0xc0, 0x40, "Upright 2P"             },
	{0x14, 0x01, 0xc0, 0xc0, "Cocktail"               },

	// Dip B
	CPS1_DIFFICULTY_2(0x15)

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x15, 0x01, 0x30, 0x00, "20k, 40k and every 60k" },
	{0x15, 0x01, 0x30, 0x10, "30k, 50k and every 70k" },
	{0x15, 0x01, 0x30, 0x20, "20k & 60k only"         },
	{0x15, 0x01, 0x30, 0x30, "30k & 60k only"         },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x16, 0x01, 0x03, 0x03, "2"                      },
	{0x16, 0x01, 0x03, 0x00, "3"                      },
	{0x16, 0x01, 0x03, 0x01, "4"                      },
	{0x16, 0x01, 0x03, 0x02, "5"                      },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x16, 0x01, 0x04, 0x00, "Off"                    },
	{0x16, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x16, 0x01, 0x08, 0x00, "Off"                    },
	{0x16, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x16, 0x01, 0x10, 0x00, "Off"                    },
	{0x16, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Sound"                  },
	{0x16, 0x01, 0x20, 0x20, "Off"                    },
	{0x16, 0x01, 0x20, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x16, 0x01, 0x40, 0x40, "Off"                    },
	{0x16, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x16, 0x01, 0x80, 0x00, "Game"                   },
	{0x16, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Strider);

static struct BurnDIPInfo StridruaDIPList[]=
{
	// Defaults
	{0x14, 0xff, 0xff, 0x00, NULL                     },
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x14)

	{0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x14, 0x01, 0xc0, 0x00, "Upright 1P"             },
	{0x14, 0x01, 0xc0, 0x40, "Upright 2P"             },
	{0x14, 0x01, 0xc0, 0xc0, "Cocktail"               },

	// Dip B
	CPS1_DIFFICULTY_2(0x15)
	
	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x15, 0x01, 0x08, 0x00, "Off"                    },
	{0x15, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x15, 0x01, 0x30, 0x00, "20k, 40k and every 60k" },
	{0x15, 0x01, 0x30, 0x10, "30k, 50k and every 70k" },
	{0x15, 0x01, 0x30, 0x20, "20k & 60k only"         },
	{0x15, 0x01, 0x30, 0x30, "30k & 60k only"         },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x16, 0x01, 0x03, 0x03, "2"                      },
	{0x16, 0x01, 0x03, 0x00, "3"                      },
	{0x16, 0x01, 0x03, 0x01, "4"                      },
	{0x16, 0x01, 0x03, 0x02, "5"                      },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x16, 0x01, 0x04, 0x00, "Off"                    },
	{0x16, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x16, 0x01, 0x08, 0x00, "Off"                    },
	{0x16, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x16, 0x01, 0x10, 0x00, "Off"                    },
	{0x16, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Sound"                  },
	{0x16, 0x01, 0x20, 0x20, "Off"                    },
	{0x16, 0x01, 0x20, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x16, 0x01, 0x40, 0x40, "Off"                    },
	{0x16, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x16, 0x01, 0x80, 0x00, "Game"                   },
	{0x16, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Stridrua);

static struct BurnDIPInfo UnsquadDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x03, NULL                     },
	{0x17, 0xff, 0xff, 0x60, NULL                     },

	// Dip A
        CPS1_COINAGE_3(0x15)

	// Dip B
	CPS1_DIFFICULTY_1(0x16)
	
	{0   , 0xfe, 0   , 4   , "Damage"                 },
	{0x16, 0x01, 0x18, 0x08, "Small"                  },
	{0x16, 0x01, 0x18, 0x00, "Normal"                 },
	{0x16, 0x01, 0x18, 0x10, "Big"                    },
	{0x16, 0x01, 0x18, 0x18, "Biggest"                },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x17, 0x01, 0x04, 0x00, "Off"                    },
	{0x17, 0x01, 0x04, 0x04, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x17, 0x01, 0x08, 0x00, "Off"                    },
	{0x17, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip"                   },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x17, 0x01, 0x20, 0x00, "Off"                    },
	{0x17, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x00, "Off"                    },
	{0x17, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Unsquad);

static struct BurnDIPInfo VarthDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x0b, NULL                     },
	{0x17, 0xff, 0xff, 0x60, NULL                     },
	
	// Dip A
	CPS1_COINAGE_1(0x15)

	{0   , 0xfe, 0   , 2   , "2C to Start, 1 to Cont" },
	{0x15, 0x01, 0x40, 0x00, "Off"                    },
	{0x15, 0x01, 0x40, 0x40, "On"                     },

	// Dip B
	CPS1_DIFFICULTY_1(0x16)

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x16, 0x01, 0x18, 0x00, "600k and every 1400k"   },
	{0x16, 0x01, 0x18, 0x08, "600k, 2000k and every 4500k"},
	{0x16, 0x01, 0x18, 0x10, "1200k, 3500k"           },
	{0x16, 0x01, 0x18, 0x18, "2000k only"             },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x17, 0x01, 0x03, 0x01, "1"                      },
	{0x17, 0x01, 0x03, 0x02, "2"                      },
	{0x17, 0x01, 0x03, 0x00, "3"                      },
	{0x17, 0x01, 0x03, 0x03, "4"                      },

	{0   , 0xfe, 0   , 2   , "Free Play"              },
	{0x17, 0x01, 0x04, 0x00, "Off"                    },
	{0x17, 0x01, 0x04, 0x04, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x17, 0x01, 0x08, 0x00, "Off"                    },
	{0x17, 0x01, 0x08, 0x08, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sound"             },
	{0x17, 0x01, 0x20, 0x00, "Off"                    },
	{0x17, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x00, "Off"                    },
	{0x17, 0x01, 0x40, 0x40, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Varth);

static struct BurnDIPInfo WillowDIPList[]=
{
	// Defaults
	{0x15, 0xff, 0xff, 0x00, NULL                     },
	{0x16, 0xff, 0xff, 0x00, NULL                     },
	{0x17, 0xff, 0xff, 0x00, NULL                     },

	// Dip A
        CPS1_COINAGE_3(0x15)

        {0   , 0xfe, 0   , 3   , "Cabinet"                },
	{0x15, 0x01, 0xc0, 0x00, "Upright 1P"             },
	{0x15, 0x01, 0xc0, 0x40, "Upright 2P"             },
	{0x15, 0x01, 0xc0, 0xc0, "Cocktail"               },
	
	// Dip B
	CPS1_DIFFICULTY_2(0x16)

	{0   , 0xfe, 0   , 4   , "Nando Speed"            },
	{0x16, 0x01, 0x18, 0x08, "Slow"                   },
	{0x16, 0x01, 0x18, 0x00, "Normal"                 },	
	{0x16, 0x01, 0x18, 0x10, "Fast"                   },
	{0x16, 0x01, 0x18, 0x18, "Very Fast"              },

	{0   , 0xfe, 0   , 2   , "Stage Magic Continue"   },
	{0x16, 0x01, 0x80, 0x00, "Off"                    },
	{0x16, 0x01, 0x80, 0x80, "On"                     },

	// Dip C
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x17, 0x01, 0x03, 0x01, "1"                      },
	{0x17, 0x01, 0x03, 0x00, "2"                      },
	{0x17, 0x01, 0x03, 0x02, "3"                      },
	{0x17, 0x01, 0x03, 0x03, "4"                      },

	{0   , 0xfe, 0   , 4   , "Vitality"               },
	{0x17, 0x01, 0x0c, 0x0c, "2"                      },
	{0x17, 0x01, 0x0c, 0x00, "3"                      },
	{0x17, 0x01, 0x0c, 0x04, "4"                      },
	{0x17, 0x01, 0x0c, 0x08, "5"                      },

	{0   , 0xfe, 0   , 2   , "Screen"                 },
	{0x17, 0x01, 0x10, 0x00, "Off"                    },
	{0x17, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x17, 0x01, 0x20, 0x20, "Off"                    },
	{0x17, 0x01, 0x20, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x17, 0x01, 0x40, 0x40, "Off"                    },
	{0x17, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Game Mode"              },
	{0x17, 0x01, 0x80, 0x00, "Game"                   },
	{0x17, 0x01, 0x80, 0x80, "Test"                   },
};

STDDIPINFO(Willow);

static struct BurnDIPInfo WofDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },

	// Dip C
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x1b, 0x01, 0x08, 0x08, "Off"                    },
	{0x1b, 0x01, 0x08, 0x00, "On"                     },
};

STDDIPINFO(Wof);

static struct BurnDIPInfo WofhfhDIPList[]=
{
	// Defaults
	{0x1b, 0xff, 0xff, 0x00, NULL                     },
	{0x1c, 0xff, 0xff, 0x13, NULL                     },
	{0x1d, 0xff, 0xff, 0x00, NULL                     },
	
	// Dip A
	{0   , 0xfe, 0   , 4   , "Coin A"                 },
	{0x1b, 0x01, 0x03, 0x00, "1 Coin 1 Credit"        },
	{0x1b, 0x01, 0x03, 0x01, "1 Coin 2 Credits"       },
	{0x1b, 0x01, 0x03, 0x02, "1 Coin 3 Credits"       },
	{0x1b, 0x01, 0x03, 0x03, "1 Coin 4 Credits"       },
	
	// Dip B
	{0   , 0xfe, 0   , 8   , "Difficulty"             },
	{0x1c, 0x01, 0x07, 0x00, "Extra Easy"             },
	{0x1c, 0x01, 0x07, 0x01, "Very Easy"              },
	{0x1c, 0x01, 0x07, 0x02, "Easy"                   },
	{0x1c, 0x01, 0x07, 0x03, "Normal"                 },
	{0x1c, 0x01, 0x07, 0x04, "Hard"                   },
	{0x1c, 0x01, 0x07, 0x05, "Very Hard"              },
	{0x1c, 0x01, 0x07, 0x06, "Extra Hard"             },
	{0x1c, 0x01, 0x07, 0x07, "Hardest"                },
	
	{0   , 0xfe, 0   , 8   , "Lives"                  },
	{0x1c, 0x01, 0x70, 0x70, "Start 4 Continue 5"     },
	{0x1c, 0x01, 0x70, 0x60, "Start 3 Continue 4"     },
	{0x1c, 0x01, 0x70, 0x50, "Start 2 Continue 3"     },
	{0x1c, 0x01, 0x70, 0x40, "Start 1 Continue 2"     },
	{0x1c, 0x01, 0x70, 0x30, "Start 4 Continue 4"     },
	{0x1c, 0x01, 0x70, 0x20, "Start 3 Continue 3"     },
	{0x1c, 0x01, 0x70, 0x10, "Start 2 Continue 2"     },
	{0x1c, 0x01, 0x70, 0x00, "Start 1 Continue 1"     },
	
	// Dip C
	{0   , 0xfe, 0   , 3   , "Coin Slots"             },
	{0x1d, 0x01, 0x03, 0x02, "2 Players 1 Shooter"    },
	{0x1d, 0x01, 0x03, 0x01, "3 Players 1 Shooter"    },
	{0x1d, 0x01, 0x03, 0x00, "3 Players 3 Shooters"   },
};

STDDIPINFO(Wofhfh);

// Rom Definitions

static struct BurnRomInfo NTFODrvRomDesc[] = {
	{ "41e_30.rom",    0x020000, 0x9deb1e75, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41e_35.rom",    0x020000, 0xd63942b3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41e_31.rom",    0x020000, 0xdf201112, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41e_36.rom",    0x020000, 0x816a818f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41_32.rom",     0x080000, 0x4e9648ca, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
 
	{ "41_gfx5.rom",   0x080000, 0x01d1cb11, BRF_GRA | CPS1_TILES },
	{ "41_gfx7.rom",   0x080000, 0xaeaa3509, BRF_GRA | CPS1_TILES },
	{ "41_gfx1.rom",   0x080000, 0xff77985a, BRF_GRA | CPS1_TILES },
	{ "41_gfx3.rom",   0x080000, 0x983be58f, BRF_GRA | CPS1_TILES },

	{ "41_09.rom",     0x010000, 0x0f9d8527, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "41_18.rom",     0x020000, 0xd1f15aeb, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "41_19.rom",     0x020000, 0x15aec3a6, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(NTFODrv);
STD_ROM_FN(NTFODrv);

static struct BurnRomInfo NTFOJDrvRomDesc[] = {
	{ "4136.bin",      0x20000, 0x7fbd42ab, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "4142.bin",      0x20000, 0xc7781f89, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "4137.bin",      0x20000, 0xc6464b0b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "4143.bin",      0x20000, 0x440fc0b5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41_32.rom",     0x80000, 0x4e9648ca, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "41_gfx5.rom",   0x80000, 0x01d1cb11, BRF_GRA | CPS1_TILES },
	{ "41_gfx7.rom",   0x80000, 0xaeaa3509, BRF_GRA | CPS1_TILES },
	{ "41_gfx1.rom",   0x80000, 0xff77985a, BRF_GRA | CPS1_TILES },
	{ "41_gfx3.rom",   0x80000, 0x983be58f, BRF_GRA | CPS1_TILES },

	{ "41_09.rom",     0x10000, 0x0f9d8527, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "41_18.rom",     0x20000, 0xd1f15aeb, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "41_19.rom",     0x20000, 0x15aec3a6, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(NTFOJDrv);
STD_ROM_FN(NTFOJDrv);

static struct BurnRomInfo ThreeWondersRomDesc[] = {
	{ "rte_30a.11f",   0x020000, 0xef5b8b33, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rte_35a.11h",   0x020000, 0x7d705529, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rte_31a.12f",   0x020000, 0x32835e5e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rte_36a.12h",   0x020000, 0x7637975f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rt_28a.9f",     0x020000, 0x054137c8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rt_33a.9h",     0x020000, 0x7264cb1b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rte_29a.10f",   0x020000, 0xcddaa919, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rte_34a.10h",   0x020000, 0xed52e7e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "rt-5m.7a",      0x080000, 0x86aef804, BRF_GRA | CPS1_TILES },
	{ "rt-7m.9a",      0x080000, 0x4f057110, BRF_GRA | CPS1_TILES },
	{ "rt-1m.3a",      0x080000, 0x902489d0, BRF_GRA | CPS1_TILES },
	{ "rt-3m.5a",      0x080000, 0xe35ce720, BRF_GRA | CPS1_TILES },
	{ "rt-6m.8a",      0x080000, 0x13cb0e7c, BRF_GRA | CPS1_TILES },
	{ "rt-8m.10a",     0x080000, 0x1f055014, BRF_GRA | CPS1_TILES },
	{ "rt-2m.4a",      0x080000, 0xe9a034f4, BRF_GRA | CPS1_TILES },
	{ "rt-4m.6a",      0x080000, 0xdf0eea8b, BRF_GRA | CPS1_TILES },

	{ "rt_09.12b",     0x010000, 0xabfca165, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "rt_18.11c",     0x020000, 0x26b211ab, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "rt_19.12c",     0x020000, 0xdbe64ad0, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(ThreeWonders);
STD_ROM_FN(ThreeWonders);

static struct BurnRomInfo Wonder3uRomDesc[] = {
	{ "3wonders.30",   0x020000, 0x0b156fd8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3wonders.35",   0x020000, 0x57350bf4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3wonders.31",   0x020000, 0x0e723fcc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3wonders.36",   0x020000, 0x523a45dc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rt_28a.9f",     0x020000, 0x054137c8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rt_33a.9h",     0x020000, 0x7264cb1b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3wonders.29",   0x020000, 0x37ba3e20, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3wonders.34",   0x020000, 0xf99f46c0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "rt-5m.7a",      0x080000, 0x86aef804, BRF_GRA | CPS1_TILES },
	{ "rt-7m.9a",      0x080000, 0x4f057110, BRF_GRA | CPS1_TILES },
	{ "rt-1m.3a",      0x080000, 0x902489d0, BRF_GRA | CPS1_TILES },
	{ "rt-3m.5a",      0x080000, 0xe35ce720, BRF_GRA | CPS1_TILES },
	{ "rt-6m.8a",      0x080000, 0x13cb0e7c, BRF_GRA | CPS1_TILES },
	{ "rt-8m.10a",     0x080000, 0x1f055014, BRF_GRA | CPS1_TILES },
	{ "rt-2m.4a",      0x080000, 0xe9a034f4, BRF_GRA | CPS1_TILES },
	{ "rt-4m.6a",      0x080000, 0xdf0eea8b, BRF_GRA | CPS1_TILES },

	{ "rt_09.12b",     0x010000, 0xabfca165, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "rt_18.11c",     0x020000, 0x26b211ab, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "rt_19.12c",     0x020000, 0xdbe64ad0, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Wonder3u)
STD_ROM_FN(Wonder3u)

static struct BurnRomInfo Wonder3RomDesc[] = {
	{ "rtj_36.12f",    0x020000, 0xe3741247, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rtj_42.12h",    0x020000, 0xb4baa117, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rtj_37.13f",    0x020000, 0xa1f677b0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rtj_43.13h",    0x020000, 0x85337a47, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rt_34.10f",     0x020000, 0x054137c8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rt_40.10h",     0x020000, 0x7264cb1b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rtj_35.11f",    0x020000, 0xe72f9ea3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "rtj_41.11h",    0x020000, 0xa11ee998, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "rt_09.4b",      0x020000, 0x2c40e480, BRF_GRA | CPS1_TILES },
	{ "rt_01.4a",      0x020000, 0x3e11f8cd, BRF_GRA | CPS1_TILES },
	{ "rt_13.9b",      0x020000, 0x51009117, BRF_GRA | CPS1_TILES },
	{ "rt_05.9a",      0x020000, 0x283fd470, BRF_GRA | CPS1_TILES },
	{ "rt_24.5e",      0x020000, 0xee4484ce, BRF_GRA | CPS1_TILES },
	{ "rt_17.5c",      0x020000, 0xe5dcddeb, BRF_GRA | CPS1_TILES },
	{ "rt_38.8h",      0x020000, 0xb2940c2d, BRF_GRA | CPS1_TILES },
	{ "rt_32.8f",      0x020000, 0x08e2b758, BRF_GRA | CPS1_TILES },
	{ "rt_10.5b",      0x020000, 0xe3f3ff94, BRF_GRA | CPS1_TILES },
	{ "rt_02.5a",      0x020000, 0xfcffd73c, BRF_GRA | CPS1_TILES },
	{ "rt_14.10b",     0x020000, 0x5c546d9a, BRF_GRA | CPS1_TILES },
	{ "rt_06.10a",     0x020000, 0xd9650bc4, BRF_GRA | CPS1_TILES },
	{ "rt_25.7e",      0x020000, 0x11b28831, BRF_GRA | CPS1_TILES },
	{ "rt_18.7c",      0x020000, 0xce1afb7c, BRF_GRA | CPS1_TILES },
	{ "rt_39.9h",      0x020000, 0xea7ac9ee, BRF_GRA | CPS1_TILES },
	{ "rt_33.9f",      0x020000, 0xd6a99384, BRF_GRA | CPS1_TILES },
	{ "rt_11.7b",      0x020000, 0x04f3c298, BRF_GRA | CPS1_TILES },
	{ "rt_03.7a",      0x020000, 0x98087e08, BRF_GRA | CPS1_TILES },
	{ "rt_15.11b",     0x020000, 0xb6aba565, BRF_GRA | CPS1_TILES },
	{ "rt_07.11a",     0x020000, 0xc62defa1, BRF_GRA | CPS1_TILES },
	{ "rt_26.8e",      0x020000, 0x532f542e, BRF_GRA | CPS1_TILES },
	{ "rt_19.8c",      0x020000, 0x1f0f72bd, BRF_GRA | CPS1_TILES },
	{ "rt_28.10e",     0x020000, 0x6064e499, BRF_GRA | CPS1_TILES },
	{ "rt_21.10c",     0x020000, 0x20012ddc, BRF_GRA | CPS1_TILES },
	{ "rt_12.8b",      0x020000, 0xe54664cc, BRF_GRA | CPS1_TILES },
	{ "rt_04.8a",      0x020000, 0x4d7b9a1a, BRF_GRA | CPS1_TILES },
	{ "rt_16.12b",     0x020000, 0x37c96cfc, BRF_GRA | CPS1_TILES },
	{ "rt_08.12a",     0x020000, 0x75f4975b, BRF_GRA | CPS1_TILES },
	{ "rt_27.9e",      0x020000, 0xec6edc0f, BRF_GRA | CPS1_TILES },
	{ "rt_20.9c",      0x020000, 0x4fe52659, BRF_GRA | CPS1_TILES },
	{ "rt_29.11e",     0x020000, 0x8fa77f9f, BRF_GRA | CPS1_TILES },
	{ "rt_22.11c",     0x020000, 0x228a0d4a, BRF_GRA | CPS1_TILES },	

	{ "rt_23.13b",     0x010000, 0x7d5a77a7, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "rt_30.12c",     0x020000, 0x26b211ab, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "rt_31.13c",     0x020000, 0xdbe64ad0, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Wonder3)
STD_ROM_FN(Wonder3)

static struct BurnRomInfo CaptcommRomDesc[] = {
	{ "cce_23d.rom",   0x080000, 0x19c58ece, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cc_22d.rom",    0x080000, 0xa91949b7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cc_24d.rom",    0x020000, 0x680e543f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cc_28d.rom",    0x020000, 0x8820039f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "gfx_01.rom",    0x080000, 0x7261d8ba, BRF_GRA | CPS1_TILES },
	{ "gfx_03.rom",    0x080000, 0x6a60f949, BRF_GRA | CPS1_TILES },
	{ "gfx_02.rom",    0x080000, 0x00637302, BRF_GRA | CPS1_TILES },
	{ "gfx_04.rom",    0x080000, 0xcc87cf61, BRF_GRA | CPS1_TILES },
	{ "gfx_05.rom",    0x080000, 0x28718bed, BRF_GRA | CPS1_TILES },
	{ "gfx_07.rom",    0x080000, 0xd4acc53a, BRF_GRA | CPS1_TILES },
	{ "gfx_06.rom",    0x080000, 0x0c69f151, BRF_GRA | CPS1_TILES },
	{ "gfx_08.rom",    0x080000, 0x1f9ebb97, BRF_GRA | CPS1_TILES },

	{ "cc_09.rom",     0x010000, 0x698e8b58, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cc_18.rom",     0x020000, 0x6de2c2db, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "cc_19.rom",     0x020000, 0xb99091ae, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Captcomm)
STD_ROM_FN(Captcomm)

static struct BurnRomInfo CaptcomuRomDesc[] = {
	{ "23b",           0x080000, 0x03da44fd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "22c",           0x080000, 0x9b82a052, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "24b",           0x020000, 0x84ff99b2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "28b",           0x020000, 0xfbcec223, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "gfx_01.rom",    0x080000, 0x7261d8ba, BRF_GRA | CPS1_TILES },
	{ "gfx_03.rom",    0x080000, 0x6a60f949, BRF_GRA | CPS1_TILES },
	{ "gfx_02.rom",    0x080000, 0x00637302, BRF_GRA | CPS1_TILES },
	{ "gfx_04.rom",    0x080000, 0xcc87cf61, BRF_GRA | CPS1_TILES },
	{ "gfx_05.rom",    0x080000, 0x28718bed, BRF_GRA | CPS1_TILES },
	{ "gfx_07.rom",    0x080000, 0xd4acc53a, BRF_GRA | CPS1_TILES },
	{ "gfx_06.rom",    0x080000, 0x0c69f151, BRF_GRA | CPS1_TILES },
	{ "gfx_08.rom",    0x080000, 0x1f9ebb97, BRF_GRA | CPS1_TILES },

	{ "cc_09.rom",     0x010000, 0x698e8b58, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cc_18.rom",     0x020000, 0x6de2c2db, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "cc_19.rom",     0x020000, 0xb99091ae, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Captcomu)
STD_ROM_FN(Captcomu)

static struct BurnRomInfo CaptcomjRomDesc[] = {
	{ "cc23.bin",      0x080000, 0x5b482b62, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cc22.bin",      0x080000, 0x0fd34195, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cc24.bin",      0x020000, 0x3a794f25, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cc28.bin",      0x020000, 0xfc3c2906, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "gfx_01.rom",    0x080000, 0x7261d8ba, BRF_GRA | CPS1_TILES },
	{ "gfx_03.rom",    0x080000, 0x6a60f949, BRF_GRA | CPS1_TILES },
	{ "gfx_02.rom",    0x080000, 0x00637302, BRF_GRA | CPS1_TILES },
	{ "gfx_04.rom",    0x080000, 0xcc87cf61, BRF_GRA | CPS1_TILES },
	{ "gfx_05.rom",    0x080000, 0x28718bed, BRF_GRA | CPS1_TILES },
	{ "gfx_07.rom",    0x080000, 0xd4acc53a, BRF_GRA | CPS1_TILES },
	{ "gfx_06.rom",    0x080000, 0x0c69f151, BRF_GRA | CPS1_TILES },
	{ "gfx_08.rom",    0x080000, 0x1f9ebb97, BRF_GRA | CPS1_TILES },

	{ "cc_09.rom",     0x010000, 0x698e8b58, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cc_18.rom",     0x020000, 0x6de2c2db, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "cc_19.rom",     0x020000, 0xb99091ae, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Captcomj)
STD_ROM_FN(Captcomj)

static struct BurnRomInfo CaptcombRomDesc[] = {
	{ "25.bin",        0x080000, 0xcb71ed7a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "27.bin",        0x080000, 0x47cb2e87, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "24.bin",        0x040000, 0x79794279, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "26.bin",        0x040000, 0xb01077ba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "c91e-01.bin",   0x100000, 0xf863071c, BRF_GRA | CPS1_TILES },
	{ "c91e-02.bin",   0x100000, 0x4b03c308, BRF_GRA | CPS1_TILES },
	{ "c91e-03.bin",   0x100000, 0x3383ea96, BRF_GRA | CPS1_TILES },
	{ "c91e-04.bin",   0x100000, 0xb8e1f4cf, BRF_GRA | CPS1_TILES },

	{ "l.bin",         0x010000, 0x698e8b58, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "c91e-05.bin",   0x040000, 0x096115fb, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Captcomb)
STD_ROM_FN(Captcomb)

static struct BurnRomInfo CawingRomDesc[] = {
	{ "cae_30a.11f",   0x020000, 0x91fceacd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_35a.11h",   0x020000, 0x3ef03083, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_31a.12f",   0x020000, 0xe5b75caf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_36a.12h",   0x020000, 0xc73fd713, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ca-32m.8h",     0x080000, 0x0c4837d4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ca-5m.7a",      0x080000, 0x66d4cc37, BRF_GRA | CPS1_TILES },
	{ "ca-7m.9a",      0x080000, 0xb6f896f2, BRF_GRA | CPS1_TILES },
	{ "ca-1m.3a",      0x080000, 0x4d0620fd, BRF_GRA | CPS1_TILES },
	{ "ca-3m.5a",      0x080000, 0x0b0341c3, BRF_GRA | CPS1_TILES },

	{ "ca_9.12b",      0x010000, 0x96fe7485, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ca_18.11c",     0x020000, 0x4a613a2c, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ca_19.12c",     0x020000, 0x74584493, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cawing)
STD_ROM_FN(Cawing)

static struct BurnRomInfo Cawingr1RomDesc[] = {
	{ "cae_30.11f",    0x020000, 0x23305cd5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_35.11h",    0x020000, 0x69419113, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_31.12f",    0x020000, 0x9008dfb3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_36.12h",    0x020000, 0x4dbf6f8e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ca-32m.8h",     0x080000, 0x0c4837d4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ca-5m.7a",      0x080000, 0x66d4cc37, BRF_GRA | CPS1_TILES },
	{ "ca-7m.9a",      0x080000, 0xb6f896f2, BRF_GRA | CPS1_TILES },
	{ "ca-1m.3a",      0x080000, 0x4d0620fd, BRF_GRA | CPS1_TILES },
	{ "ca-3m.5a",      0x080000, 0x0b0341c3, BRF_GRA | CPS1_TILES },

	{ "ca_9.12b",      0x010000, 0x96fe7485, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ca_18.11c",     0x020000, 0x4a613a2c, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ca_19.12c",     0x020000, 0x74584493, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cawingr1)
STD_ROM_FN(Cawingr1)

static struct BurnRomInfo CawinguRomDesc[] = {
	{ "cae_30a.11f",   0x020000, 0x91fceacd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cau_35a.11h",   0x020000, 0xf090d9b2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_31a.12f",   0x020000, 0xe5b75caf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cae_36a.12h",   0x020000, 0xc73fd713, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ca-32m.8h",     0x080000, 0x0c4837d4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ca-5m.7a",      0x080000, 0x66d4cc37, BRF_GRA | CPS1_TILES },
	{ "ca-7m.9a",      0x080000, 0xb6f896f2, BRF_GRA | CPS1_TILES },
	{ "ca-1m.3a",      0x080000, 0x4d0620fd, BRF_GRA | CPS1_TILES },
	{ "ca-3m.5a",      0x080000, 0x0b0341c3, BRF_GRA | CPS1_TILES },

	{ "ca_9.12b",      0x010000, 0x96fe7485, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ca_18.11c",     0x020000, 0x4a613a2c, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ca_19.12c",     0x020000, 0x74584493, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cawingu)
STD_ROM_FN(Cawingu)

static struct BurnRomInfo CawingjRomDesc[] = {
	{ "caj_36a.12f",   0x020000, 0x91fceacd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_42a.12h",   0x020000, 0x039f8362, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_37a.13f",   0x020000, 0xe5b75caf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_43a.13h",   0x020000, 0xc73fd713, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_34.10f",    0x020000, 0x51ea57f4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_40.10h",    0x020000, 0x2ab71ae1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_35.11f",    0x020000, 0x01d71973, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "caj_41.11h",    0x020000, 0x3a43b538, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "caj_09.4b",     0x020000, 0x41b0f9a6, BRF_GRA | CPS1_TILES },
	{ "caj_01.4a",     0x020000, 0x1002d0b8, BRF_GRA | CPS1_TILES },
	{ "caj_13.9b",     0x020000, 0x6f3948b2, BRF_GRA | CPS1_TILES },
	{ "caj_05.9a",     0x020000, 0x207373d7, BRF_GRA | CPS1_TILES },
	{ "caj_24.5e",     0x020000, 0xe356aad7, BRF_GRA | CPS1_TILES },
	{ "caj_17.5c",     0x020000, 0x540f2fd8, BRF_GRA | CPS1_TILES },
	{ "caj_38.8h",     0x020000, 0x2464d4ab, BRF_GRA | CPS1_TILES },
	{ "caj_32.8f",     0x020000, 0x9b5836b3, BRF_GRA | CPS1_TILES },
	{ "caj_10.5b",     0x020000, 0xbf8a5f52, BRF_GRA | CPS1_TILES },
	{ "caj_02.5a",     0x020000, 0x125b018d, BRF_GRA | CPS1_TILES },
	{ "caj_14.10b",    0x020000, 0x8458e7d7, BRF_GRA | CPS1_TILES },
	{ "caj_06.10a",    0x020000, 0xcf80e164, BRF_GRA | CPS1_TILES },
	{ "caj_25.7e",     0x020000, 0xcdd0204d, BRF_GRA | CPS1_TILES },
	{ "caj_18.7c",     0x020000, 0x29c1d4b1, BRF_GRA | CPS1_TILES },
	{ "caj_39.9h",     0x020000, 0xeea23b67, BRF_GRA | CPS1_TILES },
	{ "caj_33.9f",     0x020000, 0xdde3891f, BRF_GRA | CPS1_TILES },

	{ "caj_23.13b",    0x010000, 0x96fe7485, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "caj_30.12c",    0x020000, 0x4a613a2c, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "caj_31.13c",    0x020000, 0x74584493, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cawingj)
STD_ROM_FN(Cawingj)

static struct BurnRomInfo CawingbRomDesc[] = {
	{ "2.16d",         0x080000, 0x8125d3f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "1.19d",         0x080000, 0xb19b10ce, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ca-5m.7a",      0x080000, 0x66d4cc37, BRF_GRA | CPS1_TILES },
	{ "ca-7m.9a",      0x080000, 0xb6f896f2, BRF_GRA | CPS1_TILES },
	{ "ca-1m.3a",      0x080000, 0x4d0620fd, BRF_GRA | CPS1_TILES },
	{ "ca-3m.5a",      0x080000, 0x0b0341c3, BRF_GRA | CPS1_TILES },

	{ "ca_9.12b",      0x010000, 0x96fe7485, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ca_18.11c",     0x020000, 0x4a613a2c, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ca_19.12c",     0x020000, 0x74584493, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cawingb)
STD_ROM_FN(Cawingb)

static struct BurnRomInfo Cworld2jRomDesc[] = {
	{ "q536.bin",      0x020000, 0x38a08099, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q542.bin",      0x020000, 0x4d29b3a4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q537.bin",      0x020000, 0xeb547ebc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q543.bin",      0x020000, 0x3ef65ea8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q534.bin",      0x020000, 0x7fcc1317, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q540.bin",      0x020000, 0x7f14b7b4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q535.bin",      0x020000, 0xabacee26, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "q541.bin",      0x020000, 0xd3654067, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "q509.bin",      0x020000, 0x48496d80, BRF_GRA | CPS1_TILES },
	{ "q501.bin",      0x020000, 0xc5453f56, BRF_GRA | CPS1_TILES },
	{ "q513.bin",      0x020000, 0xc741ac52, BRF_GRA | CPS1_TILES },
	{ "q505.bin",      0x020000, 0x143e068f, BRF_GRA | CPS1_TILES },
	{ "q524.bin",      0x020000, 0xb419d139, BRF_GRA | CPS1_TILES },
	{ "q517.bin",      0x020000, 0xbd3b4d11, BRF_GRA | CPS1_TILES },
	{ "q538.bin",      0x020000, 0x9c24670c, BRF_GRA | CPS1_TILES },
	{ "q532.bin",      0x020000, 0x3ef9c7c2, BRF_GRA | CPS1_TILES },
	{ "q510.bin",      0x020000, 0x119e5e93, BRF_GRA | CPS1_TILES },
	{ "q502.bin",      0x020000, 0xa2cadcbe, BRF_GRA | CPS1_TILES },
	{ "q514.bin",      0x020000, 0xa8755f82, BRF_GRA | CPS1_TILES },
	{ "q506.bin",      0x020000, 0xc92a91fc, BRF_GRA | CPS1_TILES },
	{ "q525.bin",      0x020000, 0x979237cb, BRF_GRA | CPS1_TILES },
	{ "q518.bin",      0x020000, 0xc57da03c, BRF_GRA | CPS1_TILES },
	{ "q539.bin",      0x020000, 0xa5839b25, BRF_GRA | CPS1_TILES },
	{ "q533.bin",      0x020000, 0x04d03930, BRF_GRA | CPS1_TILES },

	{ "q523.bin",      0x010000, 0xe14dc524, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "q530.bin",      0x020000, 0xd10c1b68, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "q531.bin",      0x020000, 0x7d17e496, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cworld2j)
STD_ROM_FN(Cworld2j)

static struct BurnRomInfo DinoRomDesc[] = {
	{ "cde_23a.rom",   0x080000, 0x8f4e585e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cde_22a.rom",   0x080000, 0x9278aa12, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cde_21a.rom",   0x080000, 0x66d23de2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd_gfx01.rom",  0x080000, 0x8da4f917, BRF_GRA | CPS1_TILES },
	{ "cd_gfx03.rom",  0x080000, 0x6c40f603, BRF_GRA | CPS1_TILES },
	{ "cd_gfx02.rom",  0x080000, 0x09c8fc2d, BRF_GRA | CPS1_TILES },
	{ "cd_gfx04.rom",  0x080000, 0x637ff38f, BRF_GRA | CPS1_TILES },
	{ "cd_gfx05.rom",  0x080000, 0x470befee, BRF_GRA | CPS1_TILES },
	{ "cd_gfx07.rom",  0x080000, 0x22bfb7a3, BRF_GRA | CPS1_TILES },
	{ "cd_gfx06.rom",  0x080000, 0xe7599ac4, BRF_GRA | CPS1_TILES },
	{ "cd_gfx08.rom",  0x080000, 0x211b4b15, BRF_GRA | CPS1_TILES },

	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Dino)
STD_ROM_FN(Dino)

static struct BurnRomInfo DinojRomDesc[] = {
	{ "cdj-23a.8f",    0x080000, 0x5f3ece96, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cdj-22a.7f",    0x080000, 0xa0d8de29, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cde_21a.rom",   0x080000, 0x66d23de2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd_gfx01.rom",  0x080000, 0x8da4f917, BRF_GRA | CPS1_TILES },
	{ "cd_gfx03.rom",  0x080000, 0x6c40f603, BRF_GRA | CPS1_TILES },
	{ "cd_gfx02.rom",  0x080000, 0x09c8fc2d, BRF_GRA | CPS1_TILES },
	{ "cd_gfx04.rom",  0x080000, 0x637ff38f, BRF_GRA | CPS1_TILES },
	{ "cd_gfx05.rom",  0x080000, 0x470befee, BRF_GRA | CPS1_TILES },
	{ "cd_gfx07.rom",  0x080000, 0x22bfb7a3, BRF_GRA | CPS1_TILES },
	{ "cd_gfx06.rom",  0x080000, 0xe7599ac4, BRF_GRA | CPS1_TILES },
	{ "cd_gfx08.rom",  0x080000, 0x211b4b15, BRF_GRA | CPS1_TILES },

	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Dinoj)
STD_ROM_FN(Dinoj)

static struct BurnRomInfo DinouRomDesc[] = {
	{ "cdu.23a",       0x080000, 0x7c2543cd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cdu.22a",       0x080000, 0xfab740a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cde_21a.rom",   0x080000, 0x66d23de2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd_gfx01.rom",  0x080000, 0x8da4f917, BRF_GRA | CPS1_TILES },
	{ "cd_gfx03.rom",  0x080000, 0x6c40f603, BRF_GRA | CPS1_TILES },
	{ "cd_gfx02.rom",  0x080000, 0x09c8fc2d, BRF_GRA | CPS1_TILES },
	{ "cd_gfx04.rom",  0x080000, 0x637ff38f, BRF_GRA | CPS1_TILES },
	{ "cd_gfx05.rom",  0x080000, 0x470befee, BRF_GRA | CPS1_TILES },
	{ "cd_gfx07.rom",  0x080000, 0x22bfb7a3, BRF_GRA | CPS1_TILES },
	{ "cd_gfx06.rom",  0x080000, 0xe7599ac4, BRF_GRA | CPS1_TILES },
	{ "cd_gfx08.rom",  0x080000, 0x211b4b15, BRF_GRA | CPS1_TILES },

	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Dinou)
STD_ROM_FN(Dinou)

static struct BurnRomInfo DinopicRomDesc[] = {
	{ "5.bin",         0x080000, 0x96dfcbf1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3.bin",         0x080000, 0x13dfeb08, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "7.bin",         0x080000, 0x6133f349, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "2.bin",         0x080000, 0x0e4058ba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "4.bin",         0x080000, 0xf3c2c98d, BRF_GRA | CPS1_TILES },
	{ "8.bin",         0x080000, 0xd574befc, BRF_GRA | CPS1_TILES },
	{ "9.bin",         0x080000, 0x55ef0adc, BRF_GRA | CPS1_TILES },
	{ "6.bin",         0x080000, 0xcc0805fc, BRF_GRA | CPS1_TILES },
	{ "13.bin",        0x080000, 0x1371f714, BRF_GRA | CPS1_TILES },
	{ "12.bin",        0x080000, 0xb284c4a7, BRF_GRA | CPS1_TILES },
	{ "11.bin",        0x080000, 0xb7ad3394, BRF_GRA | CPS1_TILES },
	{ "10.bin",        0x080000, 0x88847705, BRF_GRA | CPS1_TILES },
	
	{ "pic16c57-rp",   0x002d4c, 0x5a6d393c, BRF_PRG | CPS1_PIC },

	{ "1.bin",         0x080000, 0x7d921309, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Dinopic)
STD_ROM_FN(Dinopic)

static struct BurnRomInfo Dinopic2RomDesc[] = {
	{ "27c4000-m12481.bin",    0x080000, 0x96dfcbf1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "27c4000-m12374r-2.bin", 0x080000, 0x13dfeb08, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "27c4000-m12374r-3.bin", 0x080000, 0x6133f349, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "27c4000-m12374r-1.bin", 0x080000, 0x0e4058ba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "27c4000-m12481-4.bin",  0x080000, 0xf3c2c98d, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-3.bin",  0x080000, 0x417a4816, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-2.bin",  0x080000, 0x55ef0adc, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-1.bin",  0x080000, 0xcc0805fc, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-8.bin",  0x080000, 0x1371f714, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-7.bin",  0x080000, 0xb284c4a7, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-6.bin",  0x080000, 0xb7ad3394, BRF_GRA | CPS1_TILES },
	{ "27c4000-m12481-5.bin",  0x080000, 0x88847705, BRF_GRA | CPS1_TILES },
	
	{ "pic16c57-xt.hex",       0x0026cc, 0xa6a5eac4, BRF_PRG | CPS1_PIC },

	{ "27c4000-m12623.bin",    0x080000, 0x7d921309, BRF_SND | CPS1_OKIM6295_SAMPLES },
	
	{ "cat93c46p.bin",         0x000080, 0xd49fa351, BRF_OPT },
	{ "gal20v8a-1.bin",        0x000157, 0xcd99ca47, BRF_OPT },
	{ "gal20v8a-2.bin",        0x000157, 0x60d016b9, BRF_OPT },
	{ "gal20v8a-3.bin",        0x000157, 0x049b7f4f, BRF_OPT },
	{ "palce16v8h-1.bin",      0x000117, 0x48253c66, BRF_OPT },
	{ "palce16v8h-2.bin",      0x000117, 0x9ae375ba, BRF_OPT },
};

STD_ROM_PICK(Dinopic2)
STD_ROM_FN(Dinopic2)

static struct BurnRomInfo DinohRomDesc[] = {
	{ "cda_23h.rom",   0x080000, 0x8e2a9cf0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cda_22h.rom",   0x080000, 0xf72cd219, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cda_21h.rom",   0x080000, 0xbc275b76, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cda_20h.rom",   0x080000, 0x8987c975, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd_gfx01.rom",  0x080000, 0x8da4f917, BRF_GRA | CPS1_TILES },
	{ "cd_gfx03.rom",  0x080000, 0x6c40f603, BRF_GRA | CPS1_TILES },
	{ "cd_gfx02.rom",  0x080000, 0x09c8fc2d, BRF_GRA | CPS1_TILES },
	{ "cd_gfx04.rom",  0x080000, 0x637ff38f, BRF_GRA | CPS1_TILES },
	{ "cd_gfx05.rom",  0x080000, 0x470befee, BRF_GRA | CPS1_TILES },
	{ "cd_gfx07.rom",  0x080000, 0x22bfb7a3, BRF_GRA | CPS1_TILES },
	{ "cd_gfx06.rom",  0x080000, 0xe7599ac4, BRF_GRA | CPS1_TILES },
	{ "cd_gfx08.rom",  0x080000, 0x211b4b15, BRF_GRA | CPS1_TILES },
	
	// This doesn't work at all without the Q-Sound data present - does it really use Q-Sound??
	
	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Dinoh)
STD_ROM_FN(Dinoh)

static struct BurnRomInfo DinohaRomDesc[] = {
	{ "kl2-r1.800",    0x100000, 0x4c70dca7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "kl2-l2.800",    0x100000, 0xc6ae7338, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd-a.160",      0x200000, 0x7e4f9fb3, BRF_GRA | CPS1_TILES },
	{ "cd-b.160",      0x200000, 0x89532d85, BRF_GRA | CPS1_TILES },
	
	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Dinoha)
STD_ROM_FN(Dinoha)

static struct BurnRomInfo DinohbRomDesc[] = {
	{ "cd-d.800",      0x100000, 0x2a7b2915, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cd-e.800",      0x100000, 0xe8370226, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd-a.160",      0x200000, 0x7e4f9fb3, BRF_GRA | CPS1_TILES },
	{ "cd-b.160",      0x200000, 0x89532d85, BRF_GRA | CPS1_TILES },
};

STD_ROM_PICK(Dinohb)
STD_ROM_FN(Dinohb)

static struct BurnRomInfo DynwarRomDesc[] = {
	{ "30.11f",        0x020000, 0xf9ec6d68, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "35.11h",        0x020000, 0xe41fff2f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "31.12f",        0x020000, 0xe3de76ff, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "36.12h",        0x020000, 0x7a13cfbf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tkm-9.8h",      0x080000, 0x93654bcf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tkm-5.7a",      0x080000, 0xf64bb6a0, BRF_GRA | CPS1_TILES },
	{ "tkm-8.9a",      0x080000, 0x21fe6274, BRF_GRA | CPS1_TILES },
	{ "tkm-6.3a",      0x080000, 0x0bf228cb, BRF_GRA | CPS1_TILES },
	{ "tkm-7.5a",      0x080000, 0x1255dfb1, BRF_GRA | CPS1_TILES },
	{ "tkm-1.8a",      0x080000, 0x44f7661e, BRF_GRA | CPS1_TILES },
	{ "tkm-4.10a",     0x080000, 0xa54c515d, BRF_GRA | CPS1_TILES },
	{ "tkm-2.4a",      0x080000, 0xca5c687c, BRF_GRA | CPS1_TILES },
	{ "tkm-3.6a",      0x080000, 0xf9fe6591, BRF_GRA | CPS1_TILES },

	{ "tke_17.12b",    0x010000, 0xb3b79d4f, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tke_18.11c",    0x020000, 0xac6e307d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "tke_19.12c",    0x020000, 0x068741db, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Dynwar)
STD_ROM_FN(Dynwar)

static struct BurnRomInfo DynwaruRomDesc[] = {
	{ "36",            0x020000, 0x895991d1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "42",            0x020000, 0xc898d2e8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "37",            0x020000, 0xb228d58c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "43",            0x020000, 0x1a14375a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "34.bin",        0x020000, 0x8f663d00, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "40.bin",        0x020000, 0x1586dbf3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "35.bin",        0x020000, 0x9db93d7a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41.bin",        0x020000, 0x1aae69a4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "09.bin",        0x020000, 0xc3e83c69, BRF_GRA | CPS1_TILES },
	{ "01.bin",        0x020000, 0x187b2886, BRF_GRA | CPS1_TILES },
	{ "13.bin",        0x020000, 0x0273d87d, BRF_GRA | CPS1_TILES },
	{ "05.bin",        0x020000, 0x339378b8, BRF_GRA | CPS1_TILES },
	{ "24.bin",        0x020000, 0xc6909b6f, BRF_GRA | CPS1_TILES },
	{ "17.bin",        0x020000, 0x2e2f8320, BRF_GRA | CPS1_TILES },
	{ "38.bin",        0x020000, 0xcd7923ed, BRF_GRA | CPS1_TILES },
	{ "32.bin",        0x020000, 0x21a0a453, BRF_GRA | CPS1_TILES },
	{ "10.bin",        0x020000, 0xff28f8d0, BRF_GRA | CPS1_TILES },
	{ "02.bin",        0x020000, 0xcc83c02f, BRF_GRA | CPS1_TILES },
	{ "14",            0x020000, 0x58d9b32f, BRF_GRA | CPS1_TILES },
	{ "06.bin",        0x020000, 0x6f9edd75, BRF_GRA | CPS1_TILES },
	{ "25.bin",        0x020000, 0x152ea74a, BRF_GRA | CPS1_TILES },
	{ "18.bin",        0x020000, 0x1833f932, BRF_GRA | CPS1_TILES },
	{ "39.bin",        0x020000, 0xbc09b360, BRF_GRA | CPS1_TILES },
	{ "33.bin",        0x020000, 0x89de1533, BRF_GRA | CPS1_TILES },
	{ "11.bin",        0x020000, 0x29eaf490, BRF_GRA | CPS1_TILES },
	{ "03.bin",        0x020000, 0x7bf51337, BRF_GRA | CPS1_TILES },
	{ "15.bin",        0x020000, 0xd36cdb91, BRF_GRA | CPS1_TILES },
	{ "07.bin",        0x020000, 0xe04af054, BRF_GRA | CPS1_TILES },
	{ "26.bin",        0x020000, 0x07fc714b, BRF_GRA | CPS1_TILES },
	{ "19.bin",        0x020000, 0x7114e5c6, BRF_GRA | CPS1_TILES },
	{ "28.bin",        0x020000, 0xaf62bf07, BRF_GRA | CPS1_TILES },
	{ "21.bin",        0x020000, 0x523f462a, BRF_GRA | CPS1_TILES },
	{ "12.bin",        0x020000, 0x38652339, BRF_GRA | CPS1_TILES },
	{ "04.bin",        0x020000, 0x4951bc0f, BRF_GRA | CPS1_TILES },
	{ "16.bin",        0x020000, 0x381608ae, BRF_GRA | CPS1_TILES },
	{ "08.bin",        0x020000, 0xb475d4e9, BRF_GRA | CPS1_TILES },
	{ "27.bin",        0x020000, 0xa27e81fa, BRF_GRA | CPS1_TILES },
	{ "20.bin",        0x020000, 0x002796dc, BRF_GRA | CPS1_TILES },
	{ "29.bin",        0x020000, 0x6b41f82d, BRF_GRA | CPS1_TILES },
	{ "22.bin",        0x020000, 0x52145369, BRF_GRA | CPS1_TILES },

	{ "23.bin",        0x010000, 0xb3b79d4f, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "30",            0x020000, 0xac6e307d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "31",            0x020000, 0x068741db, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Dynwaru)
STD_ROM_FN(Dynwaru)

static struct BurnRomInfo DynwarjRomDesc[] = {
	{ "36.bin",        0x020000, 0x1a516657, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "42.bin",        0x020000, 0x12a290a0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "37.bin",        0x020000, 0x932fc943, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "43.bin",        0x020000, 0x872ad76d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "34.bin",        0x020000, 0x8f663d00, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "40.bin",        0x020000, 0x1586dbf3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "35.bin",        0x020000, 0x9db93d7a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "41.bin",        0x020000, 0x1aae69a4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "09.bin",        0x020000, 0xc3e83c69, BRF_GRA | CPS1_TILES },
	{ "01.bin",        0x020000, 0x187b2886, BRF_GRA | CPS1_TILES },
	{ "13.bin",        0x020000, 0x0273d87d, BRF_GRA | CPS1_TILES },
	{ "05.bin",        0x020000, 0x339378b8, BRF_GRA | CPS1_TILES },
	{ "24.bin",        0x020000, 0xc6909b6f, BRF_GRA | CPS1_TILES },
	{ "17.bin",        0x020000, 0x2e2f8320, BRF_GRA | CPS1_TILES },
	{ "38.bin",        0x020000, 0xcd7923ed, BRF_GRA | CPS1_TILES },
	{ "32.bin",        0x020000, 0x21a0a453, BRF_GRA | CPS1_TILES },
	{ "10.bin",        0x020000, 0xff28f8d0, BRF_GRA | CPS1_TILES },
	{ "02.bin",        0x020000, 0xcc83c02f, BRF_GRA | CPS1_TILES },
	{ "14.bin",        0x020000, 0x18fb232c, BRF_GRA | CPS1_TILES },
	{ "06.bin",        0x020000, 0x6f9edd75, BRF_GRA | CPS1_TILES },
	{ "25.bin",        0x020000, 0x152ea74a, BRF_GRA | CPS1_TILES },
	{ "18.bin",        0x020000, 0x1833f932, BRF_GRA | CPS1_TILES },
	{ "39.bin",        0x020000, 0xbc09b360, BRF_GRA | CPS1_TILES },
	{ "33.bin",        0x020000, 0x89de1533, BRF_GRA | CPS1_TILES },
	{ "11.bin",        0x020000, 0x29eaf490, BRF_GRA | CPS1_TILES },
	{ "03.bin",        0x020000, 0x7bf51337, BRF_GRA | CPS1_TILES },
	{ "15.bin",        0x020000, 0xd36cdb91, BRF_GRA | CPS1_TILES },
	{ "07.bin",        0x020000, 0xe04af054, BRF_GRA | CPS1_TILES },
	{ "26.bin",        0x020000, 0x07fc714b, BRF_GRA | CPS1_TILES },
	{ "19.bin",        0x020000, 0x7114e5c6, BRF_GRA | CPS1_TILES },
	{ "28.bin",        0x020000, 0xaf62bf07, BRF_GRA | CPS1_TILES },
	{ "21.bin",        0x020000, 0x523f462a, BRF_GRA | CPS1_TILES },
	{ "12.bin",        0x020000, 0x38652339, BRF_GRA | CPS1_TILES },
	{ "04.bin",        0x020000, 0x4951bc0f, BRF_GRA | CPS1_TILES },
	{ "16.bin",        0x020000, 0x381608ae, BRF_GRA | CPS1_TILES },
	{ "08.bin",        0x020000, 0xb475d4e9, BRF_GRA | CPS1_TILES },
	{ "27.bin",        0x020000, 0xa27e81fa, BRF_GRA | CPS1_TILES },
	{ "20.bin",        0x020000, 0x002796dc, BRF_GRA | CPS1_TILES },
	{ "29.bin",        0x020000, 0x6b41f82d, BRF_GRA | CPS1_TILES },
	{ "22.bin",        0x020000, 0x52145369, BRF_GRA | CPS1_TILES },

	{ "23.bin",        0x010000, 0xb3b79d4f, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "30.bin",        0x020000, 0x7e5f6cb4, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "31.bin",        0x020000, 0x4a30c737, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Dynwarj)
STD_ROM_FN(Dynwarj)

static struct BurnRomInfo FfightRomDesc[] = {
	{ "ff_36.11f",     0x020000, 0xf9a5ce83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff_42.11h",     0x020000, 0x65f11215, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff_37.12f",     0x020000, 0xe1033784, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffe_43.12h",    0x020000, 0x995e968a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff-32m.8h",     0x080000, 0xc747696e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ff-5m.7a",      0x080000, 0x9c284108, BRF_GRA | CPS1_TILES },
	{ "ff-7m.9a",      0x080000, 0xa7584dfb, BRF_GRA | CPS1_TILES },
	{ "ff-1m.3a",      0x080000, 0x0b605e44, BRF_GRA | CPS1_TILES },
	{ "ff-3m.5a",      0x080000, 0x52291cd2, BRF_GRA | CPS1_TILES },

	{ "ff_09.12b",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ff_18.11c",     0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ff_19.12c",     0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffight)
STD_ROM_FN(Ffight)

static struct BurnRomInfo FfightuRomDesc[] = {
	{ "ff_36.11f",     0x020000, 0xf9a5ce83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff_42.11h",     0x020000, 0x65f11215, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff_37.12f",     0x020000, 0xe1033784, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff43.rom",      0x020000, 0x4ca65947, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff-32m.8h",     0x080000, 0xc747696e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ff-5m.7a",      0x080000, 0x9c284108, BRF_GRA | CPS1_TILES },
	{ "ff-7m.9a",      0x080000, 0xa7584dfb, BRF_GRA | CPS1_TILES },
	{ "ff-1m.3a",      0x080000, 0x0b605e44, BRF_GRA | CPS1_TILES },
	{ "ff-3m.5a",      0x080000, 0x52291cd2, BRF_GRA | CPS1_TILES },

	{ "ff_09.12b",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ff_18.11c",     0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ff_19.12c",     0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffightu)
STD_ROM_FN(Ffightu)

static struct BurnRomInfo FfightuaRomDesc[] = {
	{ "ffu_36.11f",    0x020000, 0xe2a48af9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffu_42.11h",    0x020000, 0xf4bb480e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffu_37.12f",    0x020000, 0xc371c667, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffu_43.12h",    0x020000, 0x2f5771f9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff-32m.8h",     0x080000, 0xc747696e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ff-5m.7a",      0x080000, 0x9c284108, BRF_GRA | CPS1_TILES },
	{ "ff-7m.9a",      0x080000, 0xa7584dfb, BRF_GRA | CPS1_TILES },
	{ "ff-1m.3a",      0x080000, 0x0b605e44, BRF_GRA | CPS1_TILES },
	{ "ff-3m.5a",      0x080000, 0x52291cd2, BRF_GRA | CPS1_TILES },

	{ "ff_09.12b",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ff_18.11c",     0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ff_19.12c",     0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffightua)
STD_ROM_FN(Ffightua)

static struct BurnRomInfo FfightubRomDesc[] = {
	{ "ffu30",         0x020000, 0xed988977, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffu35",         0x020000, 0x07bf1c21, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffu31",         0x020000, 0xdba5a476, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffu36",         0x020000, 0x4d89f542, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff-32m.8h",     0x080000, 0xc747696e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ff-5m.7a",      0x080000, 0x9c284108, BRF_GRA | CPS1_TILES },
	{ "ff-7m.9a",      0x080000, 0xa7584dfb, BRF_GRA | CPS1_TILES },
	{ "ff-1m.3a",      0x080000, 0x0b605e44, BRF_GRA | CPS1_TILES },
	{ "ff-3m.5a",      0x080000, 0x52291cd2, BRF_GRA | CPS1_TILES },

	{ "ff_09.12b",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ff_18.11c",     0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ff_19.12c",     0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffightub)
STD_ROM_FN(Ffightub)

static struct BurnRomInfo FfightjRomDesc[] = {
	{ "ff36.bin",      0x020000, 0xf9a5ce83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff42.bin",      0x020000, 0x65f11215, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff37.bin",      0x020000, 0xe1033784, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff43.bin",      0x020000, 0xb6dee1c3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_34.10f",    0x020000, 0x0c8dc3fc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_40.10h",    0x020000, 0x8075bab9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_35.11f",    0x020000, 0x4a934121, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_41.11h",    0x020000, 0x2af68154, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ffj_09.4b",     0x020000, 0x5b116d0d, BRF_GRA | CPS1_TILES },
	{ "ffj_01.4a",     0x020000, 0x815b1797, BRF_GRA | CPS1_TILES },
	{ "ffj_13.9b",     0x020000, 0x8721a7da, BRF_GRA | CPS1_TILES },
	{ "ffj_05.9a",     0x020000, 0xd0fcd4b5, BRF_GRA | CPS1_TILES },
	{ "ffj_24.5e",     0x020000, 0xa1ab607a, BRF_GRA | CPS1_TILES },
	{ "ffj_17.5c",     0x020000, 0x2dc18cf4, BRF_GRA | CPS1_TILES },
	{ "ffj_38.8h",     0x020000, 0x6535a57f, BRF_GRA | CPS1_TILES },
	{ "ffj_32.8f",     0x020000, 0xc8bc4a57, BRF_GRA | CPS1_TILES },
	{ "ffj_10.5b",     0x020000, 0x624a924a, BRF_GRA | CPS1_TILES },
	{ "ffj_02.5a",     0x020000, 0x5d91f694, BRF_GRA | CPS1_TILES },
	{ "ffj_14.10b",    0x020000, 0x0a2e9101, BRF_GRA | CPS1_TILES },
	{ "ffj_06.10a",    0x020000, 0x1c18f042, BRF_GRA | CPS1_TILES },
	{ "ffj_25.7e",     0x020000, 0x6e8181ea, BRF_GRA | CPS1_TILES },
	{ "ffj_18.7c",     0x020000, 0xb19ede59, BRF_GRA | CPS1_TILES },
	{ "ffj_39.9h",     0x020000, 0x9416b477, BRF_GRA | CPS1_TILES },
	{ "ffj_33.9f",     0x020000, 0x7369fa07, BRF_GRA | CPS1_TILES },

	{ "ff_23.13c",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ffj_30.12e",    0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ffj_31.13e",    0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffightj)
STD_ROM_FN(Ffightj)

static struct BurnRomInfo Ffightj1RomDesc[] = {
	{ "ff30-36.rom",   0x020000, 0x088ed1c9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff35-42.rom",   0x020000, 0xc4c491e6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff31-37.rom",   0x020000, 0x708557ff, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ff36-43.rom",   0x020000, 0xc004004a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_34.10f",    0x020000, 0x0c8dc3fc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_40.10h",    0x020000, 0x8075bab9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_35.11f",    0x020000, 0x4a934121, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_41.11h",    0x020000, 0x2af68154, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ffj_09.4b",     0x020000, 0x5b116d0d, BRF_GRA | CPS1_TILES },
	{ "ffj_01.4a",     0x020000, 0x815b1797, BRF_GRA | CPS1_TILES },
	{ "ffj_13.9b",     0x020000, 0x8721a7da, BRF_GRA | CPS1_TILES },
	{ "ffj_05.9a",     0x020000, 0xd0fcd4b5, BRF_GRA | CPS1_TILES },
	{ "ffj_24.5e",     0x020000, 0xa1ab607a, BRF_GRA | CPS1_TILES },
	{ "ffj_17.5c",     0x020000, 0x2dc18cf4, BRF_GRA | CPS1_TILES },
	{ "ffj_38.8h",     0x020000, 0x6535a57f, BRF_GRA | CPS1_TILES },
	{ "ffj_32.8f",     0x020000, 0xc8bc4a57, BRF_GRA | CPS1_TILES },
	{ "ffj_10.5b",     0x020000, 0x624a924a, BRF_GRA | CPS1_TILES },
	{ "ffj_02.5a",     0x020000, 0x5d91f694, BRF_GRA | CPS1_TILES },
	{ "ffj_14.10b",    0x020000, 0x0a2e9101, BRF_GRA | CPS1_TILES },
	{ "ffj_06.10a",    0x020000, 0x1c18f042, BRF_GRA | CPS1_TILES },
	{ "ffj_25.7e",     0x020000, 0x6e8181ea, BRF_GRA | CPS1_TILES },
	{ "ffj_18.7c",     0x020000, 0xb19ede59, BRF_GRA | CPS1_TILES },
	{ "ffj_39.9h",     0x020000, 0x9416b477, BRF_GRA | CPS1_TILES },
	{ "ffj_33.9f",     0x020000, 0x7369fa07, BRF_GRA | CPS1_TILES },

	{ "ff_23.13c",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ffj_30.12e",    0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ffj_31.13e",    0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffightj1)
STD_ROM_FN(Ffightj1)

static struct BurnRomInfo Ffightj2RomDesc[] = {
	{ "ffj_36.12f",    0x020000, 0xe2a48af9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_42.12h",    0x020000, 0xf4bb480e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_37.13f",    0x020000, 0xc371c667, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_43.13h",    0x020000, 0x6f81f194, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_34.10f",    0x020000, 0x0c8dc3fc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_40.10h",    0x020000, 0x8075bab9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_35.11f",    0x020000, 0x4a934121, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ffj_41.11h",    0x020000, 0x2af68154, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ffj_09.4b",     0x020000, 0x5b116d0d, BRF_GRA | CPS1_TILES },
	{ "ffj_01.4a",     0x020000, 0x815b1797, BRF_GRA | CPS1_TILES },
	{ "ffj_13.9b",     0x020000, 0x8721a7da, BRF_GRA | CPS1_TILES },
	{ "ffj_05.9a",     0x020000, 0xd0fcd4b5, BRF_GRA | CPS1_TILES },
	{ "ffj_24.5e",     0x020000, 0xa1ab607a, BRF_GRA | CPS1_TILES },
	{ "ffj_17.5c",     0x020000, 0x2dc18cf4, BRF_GRA | CPS1_TILES },
	{ "ffj_38.8h",     0x020000, 0x6535a57f, BRF_GRA | CPS1_TILES },
	{ "ffj_32.8f",     0x020000, 0xc8bc4a57, BRF_GRA | CPS1_TILES },
	{ "ffj_10.5b",     0x020000, 0x624a924a, BRF_GRA | CPS1_TILES },
	{ "ffj_02.5a",     0x020000, 0x5d91f694, BRF_GRA | CPS1_TILES },
	{ "ffj_14.10b",    0x020000, 0x0a2e9101, BRF_GRA | CPS1_TILES },
	{ "ffj_06.10a",    0x020000, 0x1c18f042, BRF_GRA | CPS1_TILES },
	{ "ffj_25.7e",     0x020000, 0x6e8181ea, BRF_GRA | CPS1_TILES },
	{ "ffj_18.7c",     0x020000, 0xb19ede59, BRF_GRA | CPS1_TILES },
	{ "ffj_39.9h",     0x020000, 0x9416b477, BRF_GRA | CPS1_TILES },
	{ "ffj_33.9f",     0x020000, 0x7369fa07, BRF_GRA | CPS1_TILES },

	{ "ff_23.13c",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ffj_30.12e",    0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ffj_31.13e",    0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Ffightj2)
STD_ROM_FN(Ffightj2)

static struct BurnRomInfo ForgottnRomDesc[] = {
	{ "lwu_11a.14f",   0x020000, 0xddf78831, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lwu_15a.14g",   0x020000, 0xf7ce2097, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lwu_10a.13f",   0x020000, 0x8cb38c81, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lwu_14a.13g",   0x020000, 0xd70ef9fd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw-07.13e",     0x080000, 0xfd252a26, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "lw-01.9d",      0x080000, 0x0318f298, BRF_GRA | CPS1_TILES },
	{ "lw-08.9f",      0x080000, 0x25a8e43c, BRF_GRA | CPS1_TILES },
	{ "lw-05.9e",      0x080000, 0xe4552fd7, BRF_GRA | CPS1_TILES },
	{ "lw-12.9g",      0x080000, 0x8e6a832b, BRF_GRA | CPS1_TILES },
	{ "lw-02.12d",     0x080000, 0x43e6c5c8, BRF_GRA | CPS1_TILES },
	{ "lw-09.12f",     0x080000, 0x899cb4ad, BRF_GRA | CPS1_TILES },
	{ "lw-06.12e",     0x080000, 0x5b9edffc, BRF_GRA | CPS1_TILES },
	{ "lw-13.12g",     0x080000, 0x8e058ef5, BRF_GRA | CPS1_TILES },

	{ "lwu_00.14a",    0x010000, 0x59df2a63, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "lw-03u.14c",    0x020000, 0x807d051f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "lw-04u.13c",    0x020000, 0xe6cd098e, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Forgottn)
STD_ROM_FN(Forgottn)

static struct BurnRomInfo Forgott1RomDesc[] = {
	{ "lw_11.12f",     0x020000, 0x73e920b7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw_15",         0x020000, 0x00000000, BRF_ESS | BRF_PRG | BRF_NODUMP | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw_10",         0x020000, 0x00000000, BRF_ESS | BRF_PRG | BRF_NODUMP | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw_14",         0x020000, 0x00000000, BRF_ESS | BRF_PRG | BRF_NODUMP | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw-07.10g",     0x080000, 0xfd252a26, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "lw_2.2b",       0x020000, 0x4bd75fee, BRF_GRA | CPS1_TILES },
	{ "lw_1.2a",       0x020000, 0x65f41485, BRF_GRA | CPS1_TILES },
	{ "lw-08.9b",      0x080000, 0x25a8e43c, BRF_GRA | CPS1_TILES },
	{ "lw-05.6d",      0x080000, 0xe4552fd7, BRF_GRA | CPS1_TILES },
	{ "lw_30.8h",      0x020000, 0xb385954e, BRF_GRA | CPS1_TILES },
	{ "lw_29.8f",      0x020000, 0x7bda1ac6, BRF_GRA | CPS1_TILES },
	{ "lw_4.3b",       0x020000, 0x50cf757f, BRF_GRA | CPS1_TILES },
	{ "lw_3.3a",       0x020000, 0xc03ef278, BRF_GRA | CPS1_TILES },
	{ "lw_32.9h",      0x020000, 0x30967a15, BRF_GRA | CPS1_TILES },
	{ "lw_31.9f",      0x020000, 0xc49d37fb, BRF_GRA | CPS1_TILES },
	{ "lw-02.6b",      0x080000, 0x43e6c5c8, BRF_GRA | CPS1_TILES },
	{ "lw_14.10b",     0x020000, 0x82862cce, BRF_GRA | CPS1_TILES },
	{ "lw_13.10a",     0x020000, 0xb81c0e96, BRF_GRA | CPS1_TILES },
	{ "lw-06.9d",      0x080000, 0x5b9edffc, BRF_GRA | CPS1_TILES },
	{ "lw_26.10e",     0x020000, 0x57bcd032, BRF_GRA | CPS1_TILES },
	{ "lw_25.10c",     0x020000, 0xbac91554, BRF_GRA | CPS1_TILES },
	{ "lw_16.11b",     0x020000, 0x40b26554, BRF_GRA | CPS1_TILES },
	{ "lw_15.11a",     0x020000, 0x1b7d2e07, BRF_GRA | CPS1_TILES },
	{ "lw_28.11e",     0x020000, 0xa805ad30, BRF_GRA | CPS1_TILES },
	{ "lw_27.11c",     0x020000, 0x103c1bd2, BRF_GRA | CPS1_TILES },

	{ "lw_00.13c",     0x010000, 0x59df2a63, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "lw-03u.12e",    0x020000, 0x807d051f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "lw-04u.13e",    0x020000, 0xe6cd098e, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Forgott1)
STD_ROM_FN(Forgott1)

static struct BurnRomInfo LostwrldRomDesc[] = {
	{ "lw_11c.14f",    0x020000, 0x67e42546, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw_15c.14g",    0x020000, 0x402e2a46, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw_10c.13f",    0x020000, 0xc46479d7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw_14c.13g",    0x020000, 0x97670f4a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "lw-07.13e",     0x080000, 0xfd252a26, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "lw-01.9d",      0x080000, 0x0318f298, BRF_GRA | CPS1_TILES },
	{ "lw-08.9f",      0x080000, 0x25a8e43c, BRF_GRA | CPS1_TILES },
	{ "lw-05.9e",      0x080000, 0xe4552fd7, BRF_GRA | CPS1_TILES },
	{ "lw-12.9g",      0x080000, 0x8e6a832b, BRF_GRA | CPS1_TILES },
	{ "lw-02.12d",     0x080000, 0x43e6c5c8, BRF_GRA | CPS1_TILES },
	{ "lw-09.12f",     0x080000, 0x899cb4ad, BRF_GRA | CPS1_TILES },
	{ "lw-06.12e",     0x080000, 0x5b9edffc, BRF_GRA | CPS1_TILES },
	{ "lw-13.12g",     0x080000, 0x8e058ef5, BRF_GRA | CPS1_TILES },

	{ "lw_00b.14a",    0x010000, 0x59df2a63, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "lw-03.14c",     0x020000, 0xce2159e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "lw-04.13c",     0x020000, 0x39305536, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Lostwrld)
STD_ROM_FN(Lostwrld)

static struct BurnRomInfo GhoulsRomDesc[] = {
	{ "dme_29.10h",    0x020000, 0x166a58a2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dme_30.10j",    0x020000, 0x7ac8407a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dme_27.9h",     0x020000, 0xf734b2be, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dme_28.9j",     0x020000, 0x03d3e714, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dm-17.7j",      0x080000, 0x3ea1b0f2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "dm-05.3a",      0x080000, 0x0ba9c0b0, BRF_GRA | CPS1_TILES },
	{ "dm-07.3f",      0x080000, 0x5d760ab9, BRF_GRA | CPS1_TILES },
	{ "dm-06.3c",      0x080000, 0x4ba90b59, BRF_GRA | CPS1_TILES },
	{ "dm-08.3g",      0x080000, 0x4bdee9de, BRF_GRA | CPS1_TILES },
	{ "dm_09.4a",      0x010000, 0xae24bb19, BRF_GRA | CPS1_TILES },
	{ "dm_18.7a",      0x010000, 0xd34e271a, BRF_GRA | CPS1_TILES },
	{ "dm_13.4e",      0x010000, 0x3f70dd37, BRF_GRA | CPS1_TILES },
	{ "dm_22.7e",      0x010000, 0x7e69e2e6, BRF_GRA | CPS1_TILES },
	{ "dm_11.4c",      0x010000, 0x37c9b6c6, BRF_GRA | CPS1_TILES },
	{ "dm_20.7c",      0x010000, 0x2f1345b4, BRF_GRA | CPS1_TILES },
	{ "dm_15.4g",      0x010000, 0x3c2a212a, BRF_GRA | CPS1_TILES },
	{ "dm_24.7g",      0x010000, 0x889aac05, BRF_GRA | CPS1_TILES },
	{ "dm_10.4b",      0x010000, 0xbcc0f28c, BRF_GRA | CPS1_TILES },
	{ "dm_19.7b",      0x010000, 0x2a40166a, BRF_GRA | CPS1_TILES },
	{ "dm_14.4f",      0x010000, 0x20f85c03, BRF_GRA | CPS1_TILES },
	{ "dm_23.7f",      0x010000, 0x8426144b, BRF_GRA | CPS1_TILES },
	{ "dm_12.4d",      0x010000, 0xda088d61, BRF_GRA | CPS1_TILES },
	{ "dm_21.7d",      0x010000, 0x17e11df0, BRF_GRA | CPS1_TILES },
	{ "dm_16.4h",      0x010000, 0xf187ba1c, BRF_GRA | CPS1_TILES },
	{ "dm_25.7h",      0x010000, 0x29f79c78, BRF_GRA | CPS1_TILES },

	{ "dm_26.10a",     0x010000, 0x3692f6e5, BRF_PRG | CPS1_Z80_PROGRAM },
};

STD_ROM_PICK(Ghouls)
STD_ROM_FN(Ghouls)

static struct BurnRomInfo GhoulsuRomDesc[] = {
	{ "dmu_29.10h",    0x020000, 0x334d85b2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmu_30.10j",    0x020000, 0xcee8ceb5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmu_27.9h",     0x020000, 0x4a524140, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmu_28.9j",     0x020000, 0x94aae205, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dm-17.7j",      0x080000, 0x3ea1b0f2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "dm-05.3a",      0x080000, 0x0ba9c0b0, BRF_GRA | CPS1_TILES },
	{ "dm-07.3f",      0x080000, 0x5d760ab9, BRF_GRA | CPS1_TILES },
	{ "dm-06.3c",      0x080000, 0x4ba90b59, BRF_GRA | CPS1_TILES },
	{ "dm-08.3g",      0x080000, 0x4bdee9de, BRF_GRA | CPS1_TILES },
	{ "dm_09.4a",      0x010000, 0xae24bb19, BRF_GRA | CPS1_TILES },
	{ "dm_18.7a",      0x010000, 0xd34e271a, BRF_GRA | CPS1_TILES },
	{ "dm_13.4e",      0x010000, 0x3f70dd37, BRF_GRA | CPS1_TILES },
	{ "dm_22.7e",      0x010000, 0x7e69e2e6, BRF_GRA | CPS1_TILES },
	{ "dm_11.4c",      0x010000, 0x37c9b6c6, BRF_GRA | CPS1_TILES },
	{ "dm_20.7c",      0x010000, 0x2f1345b4, BRF_GRA | CPS1_TILES },
	{ "dm_15.4g",      0x010000, 0x3c2a212a, BRF_GRA | CPS1_TILES },
	{ "dm_24.7g",      0x010000, 0x889aac05, BRF_GRA | CPS1_TILES },
	{ "dm_10.4b",      0x010000, 0xbcc0f28c, BRF_GRA | CPS1_TILES },
	{ "dm_19.7b",      0x010000, 0x2a40166a, BRF_GRA | CPS1_TILES },
	{ "dm_14.4f",      0x010000, 0x20f85c03, BRF_GRA | CPS1_TILES },
	{ "dm_23.7f",      0x010000, 0x8426144b, BRF_GRA | CPS1_TILES },
	{ "dm_12.4d",      0x010000, 0xda088d61, BRF_GRA | CPS1_TILES },
	{ "dm_21.7d",      0x010000, 0x17e11df0, BRF_GRA | CPS1_TILES },
	{ "dm_16.4h",      0x010000, 0xf187ba1c, BRF_GRA | CPS1_TILES },
	{ "dm_25.7h",      0x010000, 0x29f79c78, BRF_GRA | CPS1_TILES },

	{ "dm_26.10a",     0x010000, 0x3692f6e5, BRF_PRG | CPS1_Z80_PROGRAM },
};

STD_ROM_PICK(Ghoulsu)
STD_ROM_FN(Ghoulsu)

static struct BurnRomInfo DaimakaiRomDesc[] = {
	{ "dmj_38.12f",    0x020000, 0x82fd1798, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmj_39.12h",    0x020000, 0x35366ccc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmj_40.13f",    0x020000, 0xa17c170a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmj_41.13h",    0x020000, 0x6af0b391, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dm_33.10f",     0x020000, 0x384d60c4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dm_34.10h",     0x020000, 0x19abe30f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dm_35.11f",     0x020000, 0xc04b85c8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dm_36.11h",     0x020000, 0x89be83de, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "dm_02.4b",      0x020000, 0x8b98dc48, BRF_GRA | CPS1_TILES },
	{ "dm_01.4a",      0x020000, 0x80896c33, BRF_GRA | CPS1_TILES },
	{ "dm_10.9b",      0x020000, 0xc2e7d9ef, BRF_GRA | CPS1_TILES },
	{ "dm_09.9a",      0x020000, 0xc9c4afa5, BRF_GRA | CPS1_TILES },
	{ "dm_18.5e",      0x020000, 0x1aa0db99, BRF_GRA | CPS1_TILES },
	{ "dm_17.5c",      0x020000, 0xdc6ed8ad, BRF_GRA | CPS1_TILES },
	{ "dm_30.8h",      0x020000, 0xd9d3f8bd, BRF_GRA | CPS1_TILES },
	{ "dm_29.8f",      0x020000, 0x49a48796, BRF_GRA | CPS1_TILES },
	{ "dm_04.5b",      0x020000, 0xa4f4f8f0, BRF_GRA | CPS1_TILES },
	{ "dm_03.5a",      0x020000, 0xb1033e62, BRF_GRA | CPS1_TILES },
	{ "dm_12.10b",     0x020000, 0x10fdd76a, BRF_GRA | CPS1_TILES },
	{ "dm_11.10a",     0x020000, 0x9040cb04, BRF_GRA | CPS1_TILES },
	{ "dm_20.7e",      0x020000, 0x281d0b3e, BRF_GRA | CPS1_TILES },
	{ "dm_19.7c",      0x020000, 0x2623b52f, BRF_GRA | CPS1_TILES },
	{ "dm_32.9h",      0x020000, 0x99692344, BRF_GRA | CPS1_TILES },
	{ "dm_31.9f",      0x020000, 0x54acb729, BRF_GRA | CPS1_TILES },
	{ "dm_06.7b",      0x010000, 0xae24bb19, BRF_GRA | CPS1_TILES },
	{ "dm_05.7a",      0x010000, 0xd34e271a, BRF_GRA | CPS1_TILES },
	{ "dm_14.11b",     0x010000, 0x3f70dd37, BRF_GRA | CPS1_TILES },
	{ "dm_13.11a",     0x010000, 0x7e69e2e6, BRF_GRA | CPS1_TILES },
	{ "dm_22.8e",      0x010000, 0x37c9b6c6, BRF_GRA | CPS1_TILES },
	{ "dm_21.8c",      0x010000, 0x2f1345b4, BRF_GRA | CPS1_TILES },
	{ "dm_26.10e",     0x010000, 0x3c2a212a, BRF_GRA | CPS1_TILES },
	{ "dm_25.10c",     0x010000, 0x889aac05, BRF_GRA | CPS1_TILES },
	{ "dm_08.8b",      0x010000, 0xbcc0f28c, BRF_GRA | CPS1_TILES },
	{ "dm_07.8a",      0x010000, 0x2a40166a, BRF_GRA | CPS1_TILES },
	{ "dm_16.12b",     0x010000, 0x20f85c03, BRF_GRA | CPS1_TILES },
	{ "dm_15.12a",     0x010000, 0x8426144b, BRF_GRA | CPS1_TILES },
	{ "dm_24.9e",      0x010000, 0xda088d61, BRF_GRA | CPS1_TILES },
	{ "dm_23.9c",      0x010000, 0x17e11df0, BRF_GRA | CPS1_TILES },
	{ "dm_28.11e",     0x010000, 0xf187ba1c, BRF_GRA | CPS1_TILES },
	{ "dm_27.11c",     0x010000, 0x29f79c78, BRF_GRA | CPS1_TILES },

	{ "dm_37.13c",     0x010000, 0x3692f6e5, BRF_PRG | CPS1_Z80_PROGRAM },
};

STD_ROM_PICK(Daimakai)
STD_ROM_FN(Daimakai)

static struct BurnRomInfo DaimakbRomDesc[] = {
	{ "dmjb2.bin",     0x080000, 0x7d5f9f84, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "dmjb1.bin",     0x080000, 0x9b945cc4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },	

	{ "dm_02.4b",      0x020000, 0x8b98dc48, BRF_GRA | CPS1_TILES },
	{ "dm_01.4a",      0x020000, 0x80896c33, BRF_GRA | CPS1_TILES },
	{ "dm_10.9b",      0x020000, 0xc2e7d9ef, BRF_GRA | CPS1_TILES },
	{ "dm_09.9a",      0x020000, 0xc9c4afa5, BRF_GRA | CPS1_TILES },
	{ "dm_18.5e",      0x020000, 0x1aa0db99, BRF_GRA | CPS1_TILES },
	{ "dm_17.5c",      0x020000, 0xdc6ed8ad, BRF_GRA | CPS1_TILES },
	{ "dm_30.8h",      0x020000, 0xd9d3f8bd, BRF_GRA | CPS1_TILES },
	{ "dm_29.8f",      0x020000, 0x49a48796, BRF_GRA | CPS1_TILES },
	{ "dm_04.5b",      0x020000, 0xa4f4f8f0, BRF_GRA | CPS1_TILES },
	{ "dm_03.5a",      0x020000, 0xb1033e62, BRF_GRA | CPS1_TILES },
	{ "dm_12.10b",     0x020000, 0x10fdd76a, BRF_GRA | CPS1_TILES },
	{ "dm_11.10a",     0x020000, 0x9040cb04, BRF_GRA | CPS1_TILES },
	{ "dm_20.7e",      0x020000, 0x281d0b3e, BRF_GRA | CPS1_TILES },
	{ "dm_19.7c",      0x020000, 0x2623b52f, BRF_GRA | CPS1_TILES },
	{ "dm_32.9h",      0x020000, 0x99692344, BRF_GRA | CPS1_TILES },
	{ "dm_31.9f",      0x020000, 0x54acb729, BRF_GRA | CPS1_TILES },
	{ "dm_06.7b",      0x010000, 0xae24bb19, BRF_GRA | CPS1_TILES },
	{ "dm_05.7a",      0x010000, 0xd34e271a, BRF_GRA | CPS1_TILES },
	{ "dm_14.11b",     0x010000, 0x3f70dd37, BRF_GRA | CPS1_TILES },
	{ "dm_13.11a",     0x010000, 0x7e69e2e6, BRF_GRA | CPS1_TILES },
	{ "dm_22.8e",      0x010000, 0x37c9b6c6, BRF_GRA | CPS1_TILES },
	{ "dm_21.8c",      0x010000, 0x2f1345b4, BRF_GRA | CPS1_TILES },
	{ "dm_26.10e",     0x010000, 0x3c2a212a, BRF_GRA | CPS1_TILES },
	{ "dm_25.10c",     0x010000, 0x889aac05, BRF_GRA | CPS1_TILES },
	{ "dm_08.8b",      0x010000, 0xbcc0f28c, BRF_GRA | CPS1_TILES },
	{ "dm_07.8a",      0x010000, 0x2a40166a, BRF_GRA | CPS1_TILES },
	{ "dm_16.12b",     0x010000, 0x20f85c03, BRF_GRA | CPS1_TILES },
	{ "dm_15.12a",     0x010000, 0x8426144b, BRF_GRA | CPS1_TILES },
	{ "dm_24.9e",      0x010000, 0xda088d61, BRF_GRA | CPS1_TILES },
	{ "dm_23.9c",      0x010000, 0x17e11df0, BRF_GRA | CPS1_TILES },
	{ "dm_28.11e",     0x010000, 0xf187ba1c, BRF_GRA | CPS1_TILES },
	{ "dm_27.11c",     0x010000, 0x29f79c78, BRF_GRA | CPS1_TILES },

	{ "dm_37.13c",     0x010000, 0x3692f6e5, BRF_PRG | CPS1_Z80_PROGRAM },
};

STD_ROM_PICK(Daimakb)
STD_ROM_FN(Daimakb)

static struct BurnRomInfo KnightsRomDesc[] = {
	{ "kr_23e.rom",    0x080000, 0x1b3997eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "kr_22.rom",     0x080000, 0xd0b671a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "kr_gfx1.rom",   0x080000, 0x9e36c1a4, BRF_GRA | CPS1_TILES },
	{ "kr_gfx3.rom",   0x080000, 0xc5832cae, BRF_GRA | CPS1_TILES },
	{ "kr_gfx2.rom",   0x080000, 0xf095be2d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx4.rom",   0x080000, 0x179dfd96, BRF_GRA | CPS1_TILES },
	{ "kr_gfx5.rom",   0x080000, 0x1f4298d2, BRF_GRA | CPS1_TILES },
	{ "kr_gfx7.rom",   0x080000, 0x37fa8751, BRF_GRA | CPS1_TILES },
	{ "kr_gfx6.rom",   0x080000, 0x0200bc3d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx8.rom",   0x080000, 0x0bb2b4e7, BRF_GRA | CPS1_TILES },

	{ "kr_09.rom",     0x010000, 0x5e44d9ee, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kr_18.rom",     0x020000, 0xda69d15f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kr_19.rom",     0x020000, 0xbfc654e9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Knights)
STD_ROM_FN(Knights)

static struct BurnRomInfo KnightsuRomDesc[] = {
	{ "kru23.rom",     0x080000, 0x252bc2ba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "kr_22.rom",     0x080000, 0xd0b671a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "kr_gfx1.rom",   0x080000, 0x9e36c1a4, BRF_GRA | CPS1_TILES },
	{ "kr_gfx3.rom",   0x080000, 0xc5832cae, BRF_GRA | CPS1_TILES },
	{ "kr_gfx2.rom",   0x080000, 0xf095be2d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx4.rom",   0x080000, 0x179dfd96, BRF_GRA | CPS1_TILES },
	{ "kr_gfx5.rom",   0x080000, 0x1f4298d2, BRF_GRA | CPS1_TILES },
	{ "kr_gfx7.rom",   0x080000, 0x37fa8751, BRF_GRA | CPS1_TILES },
	{ "kr_gfx6.rom",   0x080000, 0x0200bc3d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx8.rom",   0x080000, 0x0bb2b4e7, BRF_GRA | CPS1_TILES },

	{ "kr_09.rom",     0x010000, 0x5e44d9ee, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kr_18.rom",     0x020000, 0xda69d15f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kr_19.rom",     0x020000, 0xbfc654e9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Knightsu)
STD_ROM_FN(Knightsu)

static struct BurnRomInfo KnightsjRomDesc[] = {
	{ "krj30.bin",     0x020000, 0xad3d1a8e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "krj37.bin",     0x020000, 0xe694a491, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "krj31.bin",     0x020000, 0x85596094, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "krj38.bin",     0x020000, 0x9198bf8f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kr_22.rom",     0x080000, 0xd0b671a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "kr_gfx1.rom",   0x080000, 0x9e36c1a4, BRF_GRA | CPS1_TILES },
	{ "kr_gfx3.rom",   0x080000, 0xc5832cae, BRF_GRA | CPS1_TILES },
	{ "kr_gfx2.rom",   0x080000, 0xf095be2d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx4.rom",   0x080000, 0x179dfd96, BRF_GRA | CPS1_TILES },
	{ "kr_gfx5.rom",   0x080000, 0x1f4298d2, BRF_GRA | CPS1_TILES },
	{ "kr_gfx7.rom",   0x080000, 0x37fa8751, BRF_GRA | CPS1_TILES },
	{ "kr_gfx6.rom",   0x080000, 0x0200bc3d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx8.rom",   0x080000, 0x0bb2b4e7, BRF_GRA | CPS1_TILES },

	{ "kr_09.rom",     0x010000, 0x5e44d9ee, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kr_18.rom",     0x020000, 0xda69d15f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kr_19.rom",     0x020000, 0xbfc654e9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Knightsj)
STD_ROM_FN(Knightsj)

static struct BurnRomInfo KnightsbRomDesc[] = {
	{ "040-z.02",      0x080000, 0x95d00a7e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "040-r.02",      0x080000, 0x5a9d0b64, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "kr_gfx1.rom",   0x080000, 0x9e36c1a4, BRF_GRA | CPS1_TILES },
	{ "kr_gfx3.rom",   0x080000, 0xc5832cae, BRF_GRA | CPS1_TILES },
	{ "kr_gfx2.rom",   0x080000, 0xf095be2d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx4.rom",   0x080000, 0x179dfd96, BRF_GRA | CPS1_TILES },
	{ "kr_gfx5.rom",   0x080000, 0x1f4298d2, BRF_GRA | CPS1_TILES },
	{ "kr_gfx7.rom",   0x080000, 0x37fa8751, BRF_GRA | CPS1_TILES },
	{ "kr_gfx6.rom",   0x080000, 0x0200bc3d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx8.rom",   0x080000, 0x0bb2b4e7, BRF_GRA | CPS1_TILES },

	{ "kr_09.rom",     0x010000, 0x5e44d9ee, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kr_18.rom",     0x020000, 0xda69d15f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kr_19.rom",     0x020000, 0xbfc654e9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Knightsb)
STD_ROM_FN(Knightsb)

static struct BurnRomInfo KnightshRomDesc[] = {
	{ "krh_23.rom",    0x080000, 0xfa2ff63d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "krh_22.rom",    0x080000, 0x1438d070, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "kr_gfx1.rom",   0x080000, 0x9e36c1a4, BRF_GRA | CPS1_TILES },
	{ "kr_gfx3.rom",   0x080000, 0xc5832cae, BRF_GRA | CPS1_TILES },
	{ "kr_gfx2.rom",   0x080000, 0xf095be2d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx4.rom",   0x080000, 0x179dfd96, BRF_GRA | CPS1_TILES },
	{ "kr_gfx5.rom",   0x080000, 0x1f4298d2, BRF_GRA | CPS1_TILES },
	{ "kr_gfx7.rom",   0x080000, 0x37fa8751, BRF_GRA | CPS1_TILES },
	{ "kr_gfx6.rom",   0x080000, 0x0200bc3d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx8.rom",   0x080000, 0x0bb2b4e7, BRF_GRA | CPS1_TILES },

	{ "kr_09.rom",     0x010000, 0x5e44d9ee, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kr_18.rom",     0x020000, 0xda69d15f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kr_19.rom",     0x020000, 0xbfc654e9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Knightsh)
STD_ROM_FN(Knightsh)

static struct BurnRomInfo KnghtshaRomDesc[] = {
	{ "krha_23.rom",   0x080000, 0xa7fd309a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "krh_22.rom",    0x080000, 0x1438d070, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "kr_gfx1.rom",   0x080000, 0x9e36c1a4, BRF_GRA | CPS1_TILES },
	{ "kr_gfx3.rom",   0x080000, 0xc5832cae, BRF_GRA | CPS1_TILES },
	{ "kr_gfx2.rom",   0x080000, 0xf095be2d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx4.rom",   0x080000, 0x179dfd96, BRF_GRA | CPS1_TILES },
	{ "kr_gfx5.rom",   0x080000, 0x1f4298d2, BRF_GRA | CPS1_TILES },
	{ "kr_gfx7.rom",   0x080000, 0x37fa8751, BRF_GRA | CPS1_TILES },
	{ "kr_gfx6.rom",   0x080000, 0x0200bc3d, BRF_GRA | CPS1_TILES },
	{ "kr_gfx8.rom",   0x080000, 0x0bb2b4e7, BRF_GRA | CPS1_TILES },

	{ "kr_09.rom",     0x010000, 0x5e44d9ee, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kr_18.rom",     0x020000, 0xda69d15f, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kr_19.rom",     0x020000, 0xbfc654e9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Knghtsha)
STD_ROM_FN(Knghtsha)

static struct BurnRomInfo KodRomDesc[] = {
	{ "kod30.rom",     0x020000, 0xc7414fd4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod37.rom",     0x020000, 0xa5bf40d2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod31.rom",     0x020000, 0x1fffc7bd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod38.rom",     0x020000, 0x89e57a82, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod28.rom",     0x020000, 0x9367bcd9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod35.rom",     0x020000, 0x4ca6a48a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod29.rom",     0x020000, 0x6a0ba878, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod36.rom",     0x020000, 0xb509b39d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "kod02.rom",     0x080000, 0xe45b8701, BRF_GRA | CPS1_TILES },
	{ "kod04.rom",     0x080000, 0xa7750322, BRF_GRA | CPS1_TILES },
	{ "kod01.rom",     0x080000, 0x5f74bf78, BRF_GRA | CPS1_TILES },
	{ "kod03.rom",     0x080000, 0x5e5303bf, BRF_GRA | CPS1_TILES },
	{ "kod11.rom",     0x080000, 0x113358f3, BRF_GRA | CPS1_TILES },
	{ "kod13.rom",     0x080000, 0x38853c44, BRF_GRA | CPS1_TILES },
	{ "kod10.rom",     0x080000, 0x9ef36604, BRF_GRA | CPS1_TILES },
	{ "kod12.rom",     0x080000, 0x402b9b4f, BRF_GRA | CPS1_TILES },

	{ "kod09.rom",     0x010000, 0xf5514510, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kod18.rom",     0x020000, 0x69ecb2c8, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kod19.rom",     0x020000, 0x02d851c1, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Kod)
STD_ROM_FN(Kod)

static struct BurnRomInfo KoduRomDesc[] = {
	{ "kdu-30b.bin",   0x020000, 0x825817f9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kdu-37b.bin",   0x020000, 0xd2422dfb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kdu-31b.bin",   0x020000, 0x9af36039, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kdu-38b.bin",   0x020000, 0xbe8405a1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod28.rom",     0x020000, 0x9367bcd9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kod35.rom",     0x020000, 0x4ca6a48a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kd-29.bin",     0x020000, 0x0360fa72, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kd-36a.bin",    0x020000, 0x95a3cef8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "kod02.rom",     0x080000, 0xe45b8701, BRF_GRA | CPS1_TILES },
	{ "kod04.rom",     0x080000, 0xa7750322, BRF_GRA | CPS1_TILES },
	{ "kod01.rom",     0x080000, 0x5f74bf78, BRF_GRA | CPS1_TILES },
	{ "kod03.rom",     0x080000, 0x5e5303bf, BRF_GRA | CPS1_TILES },
	{ "kod11.rom",     0x080000, 0x113358f3, BRF_GRA | CPS1_TILES },
	{ "kod13.rom",     0x080000, 0x38853c44, BRF_GRA | CPS1_TILES },
	{ "kod10.rom",     0x080000, 0x9ef36604, BRF_GRA | CPS1_TILES },
	{ "kod12.rom",     0x080000, 0x402b9b4f, BRF_GRA | CPS1_TILES },

	{ "kd09.bin",      0x010000, 0xbac6ec26, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kd18.bin",      0x020000, 0x4c63181d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kd19.bin",      0x020000, 0x92941b80, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Kodu)
STD_ROM_FN(Kodu)

static struct BurnRomInfo KodjRomDesc[] = {
	{ "kd30.bin",      0x020000, 0xebc788ad, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kd37.bin",      0x020000, 0xe55c3529, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kd31.bin",      0x020000, 0xc710d722, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kd38.bin",      0x020000, 0x57d6ed3a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "kd33.bin",      0x080000, 0x9bd7ad4b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "kod02.rom",     0x080000, 0xe45b8701, BRF_GRA | CPS1_TILES },
	{ "kod04.rom",     0x080000, 0xa7750322, BRF_GRA | CPS1_TILES },
	{ "kod01.rom",     0x080000, 0x5f74bf78, BRF_GRA | CPS1_TILES },
	{ "kod03.rom",     0x080000, 0x5e5303bf, BRF_GRA | CPS1_TILES },
	{ "kod11.rom",     0x080000, 0x113358f3, BRF_GRA | CPS1_TILES },
	{ "kod13.rom",     0x080000, 0x38853c44, BRF_GRA | CPS1_TILES },
	{ "kod10.rom",     0x080000, 0x9ef36604, BRF_GRA | CPS1_TILES },
	{ "kod12.rom",     0x080000, 0x402b9b4f, BRF_GRA | CPS1_TILES },

	{ "kd09.bin",      0x010000, 0xbac6ec26, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kd18.bin",      0x020000, 0x4c63181d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kd19.bin",      0x020000, 0x92941b80, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Kodj)
STD_ROM_FN(Kodj)

static struct BurnRomInfo KodbRomDesc[] = {
	{ "3.ic172",       0x080000, 0x036dd74c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "4.ic171",       0x080000, 0x3e4b7295, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "cp.ic90",       0x080000, 0xe3b8589e, BRF_GRA | CPS1_TILES },
	{ "dp.ic89",       0x080000, 0x3eec9580, BRF_GRA | CPS1_TILES },
	{ "ap.ic88",       0x080000, 0xfdf5f163, BRF_GRA | CPS1_TILES },
	{ "bp.ic87",       0x080000, 0x4e1c52b7, BRF_GRA | CPS1_TILES },
	{ "ci.ic91",       0x080000, 0x22228bc5, BRF_GRA | CPS1_TILES },
	{ "di.ic92",       0x080000, 0xab031763, BRF_GRA | CPS1_TILES },
	{ "ai.ic93",       0x080000, 0xcffbf4be, BRF_GRA | CPS1_TILES },
	{ "bi.ic94",       0x080000, 0x4a1b43fe, BRF_GRA | CPS1_TILES },

	{ "1.ic28",        0x010000, 0x01cae60c, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "2.ic19",        0x040000, 0xa2db1575, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Kodb)
STD_ROM_FN(Kodb)

static struct BurnRomInfo KodhRomDesc[] = {
	{ "23.096",        0x080000, 0xdaf89cfb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "22.096",        0x080000, 0xc83e19d8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "1.096",         0x080000, 0x09261881, BRF_GRA | CPS1_TILES },
	{ "2.096",         0x080000, 0xbc121ff2, BRF_GRA | CPS1_TILES },
	{ "3.096",         0x080000, 0xf463ae22, BRF_GRA | CPS1_TILES },
	{ "4.096",         0x080000, 0x01308733, BRF_GRA | CPS1_TILES },
	{ "kod11.rom",     0x080000, 0x113358f3, BRF_GRA | CPS1_TILES },
	{ "kod13.rom",     0x080000, 0x38853c44, BRF_GRA | CPS1_TILES },
	{ "7.096",         0x080000, 0xc7ab4704, BRF_GRA | CPS1_TILES },
	{ "kod12.rom",     0x080000, 0x402b9b4f, BRF_GRA | CPS1_TILES },

	{ "kd09.bin",      0x010000, 0xbac6ec26, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "kd18.bin",      0x020000, 0x4c63181d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "kd19.bin",      0x020000, 0x92941b80, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Kodh)
STD_ROM_FN(Kodh)

static struct BurnRomInfo MegamanRomDesc[] = {
	{ "rcma_23b.rom",  0x80000, 0x61e4a397, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "rcma_22b.rom",  0x80000, 0x708268c4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "rcma_21a.rom",  0x80000, 0x4376ea95, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "rcm_01.rom",    0x080000, 0x6ecdf13f, BRF_GRA | CPS1_TILES },
	{ "rcm_02.rom",    0x080000, 0x944d4f0f, BRF_GRA | CPS1_TILES },
	{ "rcm_03.rom",    0x080000, 0x36f3073c, BRF_GRA | CPS1_TILES },
	{ "rcm_04.rom",    0x080000, 0x54e622ff, BRF_GRA | CPS1_TILES },
	{ "rcm_05.rom",    0x080000, 0x5dd131fd, BRF_GRA | CPS1_TILES },
	{ "rcm_06.rom",    0x080000, 0xf0faf813, BRF_GRA | CPS1_TILES },
	{ "rcm_07.rom",    0x080000, 0x826de013, BRF_GRA | CPS1_TILES },
	{ "rcm_08.rom",    0x080000, 0xfbff64cf, BRF_GRA | CPS1_TILES },
	{ "rcm_10.rom",    0x080000, 0x4dc8ada9, BRF_GRA | CPS1_TILES },
	{ "rcm_11.rom",    0x080000, 0xf2b9ee06, BRF_GRA | CPS1_TILES },
	{ "rcm_12.rom",    0x080000, 0xfed5f203, BRF_GRA | CPS1_TILES },
	{ "rcm_13.rom",    0x080000, 0x5069d4a9, BRF_GRA | CPS1_TILES },
	{ "rcm_14.rom",    0x080000, 0x303be3bd, BRF_GRA | CPS1_TILES },
	{ "rcm_15.rom",    0x080000, 0x4f2d372f, BRF_GRA | CPS1_TILES },
	{ "rcm_16.rom",    0x080000, 0x93d97fde, BRF_GRA | CPS1_TILES },
	{ "rcm_17.rom",    0x080000, 0x92371042, BRF_GRA | CPS1_TILES },

	{ "rcm_09.rom",    0x020000, 0x9632d6ef, BRF_PRG | CPS1_Z80_PROGRAM },
	
	{ "rcm_18.rom",    0x020000, 0x80f1f8aa, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "rcm_19.rom",    0x020000, 0xf257dbe1, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Megaman)
STD_ROM_FN(Megaman)

static struct BurnRomInfo RockmanjRomDesc[] = {
	{ "rcm23a.bin",    0x080000, 0xefd96cb2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "rcm22a.bin",    0x080000, 0x8729a689, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "rcm21a.bin",    0x080000, 0x517ccde2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "rcm_01.rom",    0x080000, 0x6ecdf13f, BRF_GRA | CPS1_TILES },
	{ "rcm_02.rom",    0x080000, 0x944d4f0f, BRF_GRA | CPS1_TILES },
	{ "rcm_03.rom",    0x080000, 0x36f3073c, BRF_GRA | CPS1_TILES },
	{ "rcm_04.rom",    0x080000, 0x54e622ff, BRF_GRA | CPS1_TILES },
	{ "rcm_05.rom",    0x080000, 0x5dd131fd, BRF_GRA | CPS1_TILES },
	{ "rcm_06.rom",    0x080000, 0xf0faf813, BRF_GRA | CPS1_TILES },
	{ "rcm_07.rom",    0x080000, 0x826de013, BRF_GRA | CPS1_TILES },
	{ "rcm_08.rom",    0x080000, 0xfbff64cf, BRF_GRA | CPS1_TILES },
	{ "rcm_10.rom",    0x080000, 0x4dc8ada9, BRF_GRA | CPS1_TILES },
	{ "rcm_11.rom",    0x080000, 0xf2b9ee06, BRF_GRA | CPS1_TILES },
	{ "rcm_12.rom",    0x080000, 0xfed5f203, BRF_GRA | CPS1_TILES },
	{ "rcm_13.rom",    0x080000, 0x5069d4a9, BRF_GRA | CPS1_TILES },
	{ "rcm_14.rom",    0x080000, 0x303be3bd, BRF_GRA | CPS1_TILES },
	{ "rcm_15.rom",    0x080000, 0x4f2d372f, BRF_GRA | CPS1_TILES },
	{ "rcm_16.rom",    0x080000, 0x93d97fde, BRF_GRA | CPS1_TILES },
	{ "rcm_17.rom",    0x080000, 0x92371042, BRF_GRA | CPS1_TILES },

	{ "rcm_09.rom",    0x020000, 0x9632d6ef, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "rcm_18.rom",    0x020000, 0x80f1f8aa, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "rcm_19.rom",    0x020000, 0xf257dbe1, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Rockmanj)
STD_ROM_FN(Rockmanj)

static struct BurnRomInfo MercsRomDesc[] = {
	{ "so2_30e.11f",   0x020000, 0xe17f9bf7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_35e.11h",   0x020000, 0x78e63575, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_31e.12f",   0x020000, 0x51204d36, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_36e.12h",   0x020000, 0x9cfba8b4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2-32m.8h",    0x080000, 0x2eb5cf0c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "so2-6m.8a",     0x080000, 0xaa6102af, BRF_GRA | CPS1_TILES },
	{ "so2-8m.10a",    0x080000, 0x839e6869, BRF_GRA | CPS1_TILES },
	{ "so2-2m.4a",     0x080000, 0x597c2875, BRF_GRA | CPS1_TILES },
	{ "so2-4m.6a",     0x080000, 0x912a9ca0, BRF_GRA | CPS1_TILES },
	{ "so2_24.7d",     0x020000, 0x3f254efe, BRF_GRA | CPS1_TILES },
	{ "so2_14.7c",     0x020000, 0xf5a8905e, BRF_GRA | CPS1_TILES },
	{ "so2_26.9d",     0x020000, 0xf3aa5a4a, BRF_GRA | CPS1_TILES },
	{ "so2_16.9c",     0x020000, 0xb43cd1a8, BRF_GRA | CPS1_TILES },
	{ "so2_20.3d",     0x020000, 0x8ca751a3, BRF_GRA | CPS1_TILES },
	{ "so2_10.3c",     0x020000, 0xe9f569fd, BRF_GRA | CPS1_TILES },
	{ "so2_22.5d",     0x020000, 0xfce9a377, BRF_GRA | CPS1_TILES },
	{ "so2_12.5c",     0x020000, 0xb7df8a06, BRF_GRA | CPS1_TILES },

	{ "so2_09.12b",    0x010000, 0xd09d7c7a, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "so2_18.11c",    0x020000, 0xbbea1643, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "so2_19.12c",    0x020000, 0xac58aa71, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mercs)
STD_ROM_FN(Mercs)

static struct BurnRomInfo MercsuRomDesc[] = {
	{ "so2_30.11f",    0x020000, 0xe17f9bf7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "s02_35.11h",    0x020000, 0x4477df61, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_31.12f",    0x020000, 0x51204d36, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_36.12h",    0x020000, 0x9cfba8b4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2-32m.8h",    0x080000, 0x2eb5cf0c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "so2-6m.8a",     0x080000, 0xaa6102af, BRF_GRA | CPS1_TILES },
	{ "so2-8m.10a",    0x080000, 0x839e6869, BRF_GRA | CPS1_TILES },
	{ "so2-2m.4a",     0x080000, 0x597c2875, BRF_GRA | CPS1_TILES },
	{ "so2-4m.6a",     0x080000, 0x912a9ca0, BRF_GRA | CPS1_TILES },
	{ "so2_24.7d",     0x020000, 0x3f254efe, BRF_GRA | CPS1_TILES },
	{ "so2_14.7c",     0x020000, 0xf5a8905e, BRF_GRA | CPS1_TILES },
	{ "so2_26.9d",     0x020000, 0xf3aa5a4a, BRF_GRA | CPS1_TILES },
	{ "so2_16.9c",     0x020000, 0xb43cd1a8, BRF_GRA | CPS1_TILES },
	{ "so2_20.3d",     0x020000, 0x8ca751a3, BRF_GRA | CPS1_TILES },
	{ "so2_10.3c",     0x020000, 0xe9f569fd, BRF_GRA | CPS1_TILES },
	{ "so2_22.5d",     0x020000, 0xfce9a377, BRF_GRA | CPS1_TILES },
	{ "so2_12.5c",     0x020000, 0xb7df8a06, BRF_GRA | CPS1_TILES },

	{ "so2_09.12b",    0x010000, 0xd09d7c7a, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "so2_18.11c",    0x020000, 0xbbea1643, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "so2_19.12c",    0x020000, 0xac58aa71, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mercsu)
STD_ROM_FN(Mercsu)

static struct BurnRomInfo MercsuaRomDesc[] = {
	{ "so2_30a.11f",   0x020000, 0xe4e725d7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_35a.11h",   0x020000, 0xe7843445, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_31a.12f",   0x020000, 0xc0b91dea, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_36a.12h",   0x020000, 0x591edf6c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2-32m.8h",    0x080000, 0x2eb5cf0c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "so2-6m.8a",     0x080000, 0xaa6102af, BRF_GRA | CPS1_TILES },
	{ "so2-8m.10a",    0x080000, 0x839e6869, BRF_GRA | CPS1_TILES },
	{ "so2-2m.4a",     0x080000, 0x597c2875, BRF_GRA | CPS1_TILES },
	{ "so2-4m.6a",     0x080000, 0x912a9ca0, BRF_GRA | CPS1_TILES },
	{ "so2_24.7d",     0x020000, 0x3f254efe, BRF_GRA | CPS1_TILES },
	{ "so2_14.7c",     0x020000, 0xf5a8905e, BRF_GRA | CPS1_TILES },
	{ "so2_26.9d",     0x020000, 0xf3aa5a4a, BRF_GRA | CPS1_TILES },
	{ "so2_16.9c",     0x020000, 0xb43cd1a8, BRF_GRA | CPS1_TILES },
	{ "so2_20.3d",     0x020000, 0x8ca751a3, BRF_GRA | CPS1_TILES },
	{ "so2_10.3c",     0x020000, 0xe9f569fd, BRF_GRA | CPS1_TILES },
	{ "so2_22.5d",     0x020000, 0xfce9a377, BRF_GRA | CPS1_TILES },
	{ "so2_12.5c",     0x020000, 0xb7df8a06, BRF_GRA | CPS1_TILES },

	{ "so2_09.12b",    0x010000, 0xd09d7c7a, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "so2_18.11c",    0x020000, 0xbbea1643, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "so2_19.12c",    0x020000, 0xac58aa71, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mercsua)
STD_ROM_FN(Mercsua)

static struct BurnRomInfo MercsjRomDesc[] = {
	{ "so2_36.bin",    0x020000, 0xe17f9bf7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_42.bin",    0x020000, 0x2c3884c6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_37.bin",    0x020000, 0x51204d36, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_43.bin",    0x020000, 0x9cfba8b4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_34.bin",    0x020000, 0xb8dae95f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_40.bin",    0x020000, 0xde37771c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_35.bin",    0x020000, 0x7d24394d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "so2_41.bin",    0x020000, 0x914f85e0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "so2_09.bin",    0x020000, 0x690c261d, BRF_GRA | CPS1_TILES },
	{ "so2_01.bin",    0x020000, 0x31fd2715, BRF_GRA | CPS1_TILES },
	{ "so2_13.bin",    0x020000, 0xb5e48282, BRF_GRA | CPS1_TILES },
	{ "so2_05.bin",    0x020000, 0x54bed82c, BRF_GRA | CPS1_TILES },
	{ "so2_24.bin",    0x020000, 0x78b6f0cb, BRF_GRA | CPS1_TILES },
	{ "so2_17.bin",    0x020000, 0xe78bb308, BRF_GRA | CPS1_TILES },
	{ "so2_38.bin",    0x020000, 0x0010a9a2, BRF_GRA | CPS1_TILES },
	{ "so2_32.bin",    0x020000, 0x75dffc9a, BRF_GRA | CPS1_TILES },
	{ "so2_10.bin",    0x020000, 0x2f871714, BRF_GRA | CPS1_TILES },
	{ "so2_02.bin",    0x020000, 0xb4b2a0b7, BRF_GRA | CPS1_TILES },
	{ "so2_14.bin",    0x020000, 0x737a744b, BRF_GRA | CPS1_TILES },
	{ "so2_06.bin",    0x020000, 0x9d756f51, BRF_GRA | CPS1_TILES },
	{ "so2_25.bin",    0x020000, 0x6d0e05d6, BRF_GRA | CPS1_TILES },
	{ "so2_18.bin",    0x020000, 0x96f61f4e, BRF_GRA | CPS1_TILES },
	{ "so2_39.bin",    0x020000, 0xd52ba336, BRF_GRA | CPS1_TILES },
	{ "so2_33.bin",    0x020000, 0x39b90d25, BRF_GRA | CPS1_TILES },
	{ "so2_11.bin",    0x020000, 0x3f254efe, BRF_GRA | CPS1_TILES },
	{ "so2_03.bin",    0x020000, 0xf5a8905e, BRF_GRA | CPS1_TILES },
	{ "so2_15.bin",    0x020000, 0xf3aa5a4a, BRF_GRA | CPS1_TILES },
	{ "so2_07.bin",    0x020000, 0xb43cd1a8, BRF_GRA | CPS1_TILES },
	{ "so2_26.bin",    0x020000, 0x8ca751a3, BRF_GRA | CPS1_TILES },
	{ "so2_19.bin",    0x020000, 0xe9f569fd, BRF_GRA | CPS1_TILES },
	{ "so2_28.bin",    0x020000, 0xfce9a377, BRF_GRA | CPS1_TILES },
	{ "so2_21.bin",    0x020000, 0xb7df8a06, BRF_GRA | CPS1_TILES },

	{ "so2_23.bin",    0x010000, 0xd09d7c7a, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "so2_30.bin",    0x020000, 0xbbea1643, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "so2_31.bin",    0x020000, 0xac58aa71, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mercsj)
STD_ROM_FN(Mercsj)

static struct BurnRomInfo MswordRomDesc[] = {
	{ "mse_30.11f",    0x020000, 0x03fc8dbc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mse_35.11h",    0x020000, 0xd5bf66cd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mse_31.12f",    0x020000, 0x30332bcf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mse_36.12h",    0x020000, 0x8f7d6ce9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms-32m.8h",     0x080000, 0x2475ddfc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ms-5m.7a",      0x080000, 0xc00fe7e2, BRF_GRA | CPS1_TILES },
	{ "ms-7m.9a",      0x080000, 0x4ccacac5, BRF_GRA | CPS1_TILES },
	{ "ms-1m.3a",      0x080000, 0x0d2bbe00, BRF_GRA | CPS1_TILES },
	{ "ms-3m.5a",      0x080000, 0x3a1a5bf4, BRF_GRA | CPS1_TILES },

	{ "ms_9.12b",      0x010000, 0x57b29519, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ms_18.11c",     0x020000, 0xfb64e90d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ms_19.12c",     0x020000, 0x74f892b9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Msword)
STD_ROM_FN(Msword)

static struct BurnRomInfo Mswordr1RomDesc[] = {
	{ "ms_30.11f",     0x020000, 0x21c1f078, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_35.11h",     0x020000, 0xa540a73a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_31.12f",     0x020000, 0xd7e762b5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_36.12h",     0x020000, 0x66f2dcdb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms-32m.8h",     0x080000, 0x2475ddfc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ms-5m.7a",      0x080000, 0xc00fe7e2, BRF_GRA | CPS1_TILES },
	{ "ms-7m.9a",      0x080000, 0x4ccacac5, BRF_GRA | CPS1_TILES },
	{ "ms-1m.3a",      0x080000, 0x0d2bbe00, BRF_GRA | CPS1_TILES },
	{ "ms-3m.5a",      0x080000, 0x3a1a5bf4, BRF_GRA | CPS1_TILES },

	{ "ms_9.12b",      0x010000, 0x57b29519, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ms_18.11c",     0x020000, 0xfb64e90d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ms_19.12c",     0x020000, 0x74f892b9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mswordr1)
STD_ROM_FN(Mswordr1)

static struct BurnRomInfo MswordjRomDesc[] = {
	{ "msj_36.12f",    0x020000, 0x04f0ef50, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "msj_42.12h",    0x020000, 0x9fcbb9cd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "msj_37.13f",    0x020000, 0x6c060d70, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "msj_43.13h",    0x020000, 0xaec77787, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_34.10f",     0x020000, 0x0e59a62d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_40.10h",     0x020000, 0xbabade3a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_35.11f",     0x020000, 0x03da99d1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms_41.11h",     0x020000, 0xfadf99ea, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ms_09.4b",      0x020000, 0x4adee6f6, BRF_GRA | CPS1_TILES },
	{ "ms_01.4a",      0x020000, 0xf7ab1b88, BRF_GRA | CPS1_TILES },
	{ "ms_13.9b",      0x020000, 0xe01adc4b, BRF_GRA | CPS1_TILES },
	{ "ms_05.9a",      0x020000, 0xf62c2369, BRF_GRA | CPS1_TILES },
	{ "ms_24.5e",      0x020000, 0xbe64a3a1, BRF_GRA | CPS1_TILES },
	{ "ms_17.5c",      0x020000, 0x0bc1665f, BRF_GRA | CPS1_TILES },
	{ "ms_38.8h",      0x020000, 0x904a2ed5, BRF_GRA | CPS1_TILES },
	{ "ms_32.8f",      0x020000, 0x3d89c530, BRF_GRA | CPS1_TILES },
	{ "ms_10.5b",      0x020000, 0xf02c0718, BRF_GRA | CPS1_TILES },
	{ "ms_02.5a",      0x020000, 0xd071a405, BRF_GRA | CPS1_TILES },
	{ "ms_14.10b",     0x020000, 0xdfb2e4df, BRF_GRA | CPS1_TILES },
	{ "ms_06.10a",     0x020000, 0xd3ce2a91, BRF_GRA | CPS1_TILES },
	{ "ms_25.7e",      0x020000, 0x0f199d56, BRF_GRA | CPS1_TILES },
	{ "ms_18.7c",      0x020000, 0x1ba76df2, BRF_GRA | CPS1_TILES },
	{ "ms_39.9h",      0x020000, 0x01efce86, BRF_GRA | CPS1_TILES },
	{ "ms_33.9f",      0x020000, 0xce25defc, BRF_GRA | CPS1_TILES },

	{ "ms_23.13b",     0x010000, 0x57b29519, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ms_30.12c",     0x020000, 0xfb64e90d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ms_31.13c",     0x020000, 0x74f892b9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mswordj)
STD_ROM_FN(Mswordj)

static struct BurnRomInfo MsworduRomDesc[] = {
	{ "msu_30.11f",    0x020000, 0xd963c816, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "msu_35.11h",    0x020000, 0x72f179b3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "msu_31.12f",    0x020000, 0x20cd7904, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "msu_36.12h",    0x020000, 0xbf88c080, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ms-32m.8h",     0x080000, 0x2475ddfc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ms-5m.7a",      0x080000, 0xc00fe7e2, BRF_GRA | CPS1_TILES },
	{ "ms-7m.9a",      0x080000, 0x4ccacac5, BRF_GRA | CPS1_TILES },
	{ "ms-1m.3a",      0x080000, 0x0d2bbe00, BRF_GRA | CPS1_TILES },
	{ "ms-3m.5a",      0x080000, 0x3a1a5bf4, BRF_GRA | CPS1_TILES },

	{ "ms_9.12b",      0x010000, 0x57b29519, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ms_18.11c",     0x020000, 0xfb64e90d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ms_19.12c",     0x020000, 0x74f892b9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mswordu)
STD_ROM_FN(Mswordu)

static struct BurnRomInfo MtwinsRomDesc[] = {
	{ "che_30.11f",    0x020000, 0x9a2a2db1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "che_35.11h",    0x020000, 0xa7f96b02, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "che_31.12f",    0x020000, 0xbbff8a99, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "che_36.12h",    0x020000, 0x0fa00c39, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ck-32m.8h",     0x080000, 0x9b70bd41, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ck-5m.7a",      0x080000, 0x4ec75f15, BRF_GRA | CPS1_TILES },
	{ "ck-7m.9a",      0x080000, 0xd85d00d6, BRF_GRA | CPS1_TILES },
	{ "ck-1m.3a",      0x080000, 0xf33ca9d4, BRF_GRA | CPS1_TILES },
	{ "ck-3m.5a",      0x080000, 0x0ba2047f, BRF_GRA | CPS1_TILES },

	{ "ch_09.12b",     0x010000, 0x4d4255b7, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ch_18.12b",     0x020000, 0xf909e8de, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ch_19.12c",     0x020000, 0xfc158cf7, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Mtwins)
STD_ROM_FN(Mtwins)

static struct BurnRomInfo ChikijRomDesc[] = {
	{ "chj_36a.bin",   0x020000, 0xec1328d8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "chj_42a.bin",   0x020000, 0x4ae13503, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "chj_37a.bin",   0x020000, 0x46d2cf7b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "chj_43a.bin",   0x020000, 0x8d387fe8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ch_34.bin",     0x020000, 0x609ed2f9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ch_40.bin",     0x020000, 0xbe0d8301, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ch_35.bin",     0x020000, 0xb810867f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ch_41.bin",     0x020000, 0x8ad96155, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ch_09.bin",     0x020000, 0x567ab3ca, BRF_GRA | CPS1_TILES },
	{ "ch_01.bin",     0x020000, 0x7f3b7b56, BRF_GRA | CPS1_TILES },
	{ "ch_13.bin",     0x020000, 0x12a7a8ba, BRF_GRA | CPS1_TILES },
	{ "ch_05.bin",     0x020000, 0x6c1afb9a, BRF_GRA | CPS1_TILES },
	{ "ch_24.bin",     0x020000, 0x9cb6e6bc, BRF_GRA | CPS1_TILES },
	{ "ch_17.bin",     0x020000, 0xfe490846, BRF_GRA | CPS1_TILES },
	{ "ch_38.bin",     0x020000, 0x6e5c8cb6, BRF_GRA | CPS1_TILES },
	{ "ch_32.bin",     0x020000, 0x317d27b0, BRF_GRA | CPS1_TILES },
	{ "ch_10.bin",     0x020000, 0xe8251a9b, BRF_GRA | CPS1_TILES },
	{ "ch_02.bin",     0x020000, 0x7c8c88fb, BRF_GRA | CPS1_TILES },
	{ "ch_14.bin",     0x020000, 0x4012ec4b, BRF_GRA | CPS1_TILES },
	{ "ch_06.bin",     0x020000, 0x81884b2b, BRF_GRA | CPS1_TILES },
	{ "ch_25.bin",     0x020000, 0x1dfcbac5, BRF_GRA | CPS1_TILES },
	{ "ch_18.bin",     0x020000, 0x516a34d1, BRF_GRA | CPS1_TILES },
	{ "ch_39.bin",     0x020000, 0x872fb2a4, BRF_GRA | CPS1_TILES },
	{ "ch_33.bin",     0x020000, 0x30dc5ded, BRF_GRA | CPS1_TILES },

	{ "ch_23.bin",     0x010000, 0x4d4255b7, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ch_30.bin",     0x020000, 0xf909e8de, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ch_31.bin",     0x020000, 0xfc158cf7, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Chikij)
STD_ROM_FN(Chikij)

static struct BurnRomInfo NemoRomDesc[] = {
	{ "nme_30a.11f",   0x020000, 0xd2c03e56, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nme_35a.11h",   0x020000, 0x5fd31661, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nme_31a.12f",   0x020000, 0xb2bd4f6f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nme_36a.12h",   0x020000, 0xee9450e3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nm-32m.8h",     0x080000, 0xd6d1add3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "nm-5m.7a",      0x080000, 0x487b8747, BRF_GRA | CPS1_TILES },
	{ "nm-7m.9a",      0x080000, 0x203dc8c6, BRF_GRA | CPS1_TILES },
	{ "nm-1m.3a",      0x080000, 0x9e878024, BRF_GRA | CPS1_TILES },
	{ "nm-3m.5a",      0x080000, 0xbb01e6b6, BRF_GRA | CPS1_TILES },

	{ "nm_09.12b",     0x010000, 0x0f4b0581, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "nm_18.11c",     0x020000, 0xbab333d4, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "nm_19.12c",     0x020000, 0x2650a0a8, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Nemo)
STD_ROM_FN(Nemo)

static struct BurnRomInfo NemojRomDesc[] = {
	{ "nmj_36a.12f",   0x020000, 0xdaeceabb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nmj_42a.12h",   0x020000, 0x55024740, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nmj_37a.13f",   0x020000, 0x619068b6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nmj_43a.13h",   0x020000, 0xa948a53b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nm_34.10f",     0x020000, 0x5737feed, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nm_40.10h",     0x020000, 0x8a4099f3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nm_35.11f",     0x020000, 0xbd11a7f8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "nm_41.11h",     0x020000, 0x6309603d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "nm_09.4b",      0x020000, 0x9d60d286, BRF_GRA | CPS1_TILES },
	{ "nm_01.4a",      0x020000, 0x8a83f7c4, BRF_GRA | CPS1_TILES },
	{ "nm_13.9b",      0x020000, 0xa4909fe0, BRF_GRA | CPS1_TILES },
	{ "nm_05.9a",      0x020000, 0x16db1e61, BRF_GRA | CPS1_TILES },
	{ "nm_24.5e",      0x020000, 0x3312c648, BRF_GRA | CPS1_TILES },
	{ "nm_17.5c",      0x020000, 0xccfc50e2, BRF_GRA | CPS1_TILES },
	{ "nm_38.8h",      0x020000, 0xae98a997, BRF_GRA | CPS1_TILES },
	{ "nm_32.8f",      0x020000, 0xb3704dde, BRF_GRA | CPS1_TILES },
	{ "nm_10.5b",      0x020000, 0x33c1388c, BRF_GRA | CPS1_TILES },
	{ "nm_02.5a",      0x020000, 0x84c69469, BRF_GRA | CPS1_TILES },
	{ "nm_14.10b",     0x020000, 0x66612270, BRF_GRA | CPS1_TILES },
	{ "nm_06.10a",     0x020000, 0x8b9bcf95, BRF_GRA | CPS1_TILES },
	{ "nm_25.7e",      0x020000, 0xacfc84d2, BRF_GRA | CPS1_TILES },
	{ "nm_18.7c",      0x020000, 0x4347deed, BRF_GRA | CPS1_TILES },
	{ "nm_39.9h",      0x020000, 0x6a274ecd, BRF_GRA | CPS1_TILES },
	{ "nm_33.9f",      0x020000, 0xc469dc74, BRF_GRA | CPS1_TILES },
	
	{ "nm_23.13b",     0x010000, 0x8d3c5a42, BRF_PRG | CPS1_Z80_PROGRAM },
	
	{ "nm_30.12c",     0x020000, 0xbab333d4, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "nm_31.13c",     0x020000, 0x2650a0a8, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Nemoj)
STD_ROM_FN(Nemoj)

static struct BurnRomInfo Pang3RomDesc[] = {
	{ "pa3w-17.11l",   0x080000, 0x12138234, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "pa3w-16.10l",   0x080000, 0xd1ba585c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "pa3-01m.2c",    0x200000, 0x068a152c, BRF_GRA | CPS1_TILES },
	{ "pa3-07m.2f",    0x200000, 0x3a4a619d, BRF_GRA | CPS1_TILES },

	{ "pa3-11.11f",    0x008000, 0x90a08c46, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "pa3-05.10d",    0x020000, 0x73a10d5d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "pa3-06.11d",    0x020000, 0xaffa4f82, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Pang3)
STD_ROM_FN(Pang3)

static struct BurnRomInfo Pang3jRomDesc[] = {
	{ "pa3j-17.11l",   0x080000, 0x21f6e51f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "pa3j-16.10l",   0x080000, 0xca1d7897, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "pa3-01m.2c",    0x200000, 0x068a152c, BRF_GRA | CPS1_TILES },
	{ "pa3-07m.2f",    0x200000, 0x3a4a619d, BRF_GRA | CPS1_TILES },

	{ "pa3-11.11f",    0x008000, 0x90a08c46, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "pa3-05.10d",    0x020000, 0x73a10d5d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "pa3-06.11d",    0x020000, 0xaffa4f82, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Pang3j)
STD_ROM_FN(Pang3j)

static struct BurnRomInfo PnickjRomDesc[] = {
	{ "pnij36.bin",    0x020000, 0x2d4ffb2b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pnij42.bin",    0x020000, 0xc085dfaf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "pnij09.bin",    0x020000, 0x48177b0a, BRF_GRA | CPS1_TILES },
	{ "pnij01.bin",    0x020000, 0x01a0f311, BRF_GRA | CPS1_TILES },
	{ "pnij13.bin",    0x020000, 0x406451b0, BRF_GRA | CPS1_TILES },
	{ "pnij05.bin",    0x020000, 0x8c515dc0, BRF_GRA | CPS1_TILES },
	{ "pnij26.bin",    0x020000, 0xe2af981e, BRF_GRA | CPS1_TILES },
	{ "pnij18.bin",    0x020000, 0xf17a0e56, BRF_GRA | CPS1_TILES },
	{ "pnij38.bin",    0x020000, 0xeb75bd8c, BRF_GRA | CPS1_TILES },
	{ "pnij32.bin",    0x020000, 0x84560bef, BRF_GRA | CPS1_TILES },
	{ "pnij10.bin",    0x020000, 0xc2acc171, BRF_GRA | CPS1_TILES },
	{ "pnij02.bin",    0x020000, 0x0e21fc33, BRF_GRA | CPS1_TILES },
	{ "pnij14.bin",    0x020000, 0x7fe59b19, BRF_GRA | CPS1_TILES },
	{ "pnij06.bin",    0x020000, 0x79f4bfe3, BRF_GRA | CPS1_TILES },
	{ "pnij27.bin",    0x020000, 0x83d5cb0e, BRF_GRA | CPS1_TILES },
	{ "pnij19.bin",    0x020000, 0xaf08b230, BRF_GRA | CPS1_TILES },
	{ "pnij39.bin",    0x020000, 0x70fbe579, BRF_GRA | CPS1_TILES },
	{ "pnij33.bin",    0x020000, 0x3ed2c680, BRF_GRA | CPS1_TILES },

	{ "pnij17.bin",    0x010000, 0xe86f787a, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "pnij24.bin",    0x020000, 0x5092257d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "pnij25.bin",    0x020000, 0x22109aaa, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Pnickj)
STD_ROM_FN(Pnickj)

static struct BurnRomInfo PunisherRomDesc[] = {
	{ "pse_26.rom",    0x020000, 0x389a99d2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_30.rom",    0x020000, 0x68fb06ac, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_27.rom",    0x020000, 0x3eb181c3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_31.rom",    0x020000, 0x37108e7b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_24.rom",    0x020000, 0x0f434414, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_28.rom",    0x020000, 0xb732345d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_25.rom",    0x020000, 0xb77102e2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pse_29.rom",    0x020000, 0xec037bce, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ps_21.rom",     0x080000, 0x8affa5a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ps_gfx1.rom",   0x080000, 0x77b7ccab, BRF_GRA | CPS1_TILES },
	{ "ps_gfx3.rom",   0x080000, 0x0122720b, BRF_GRA | CPS1_TILES },
	{ "ps_gfx2.rom",   0x080000, 0x64fa58d4, BRF_GRA | CPS1_TILES },
	{ "ps_gfx4.rom",   0x080000, 0x60da42c8, BRF_GRA | CPS1_TILES },
	{ "ps_gfx5.rom",   0x080000, 0xc54ea839, BRF_GRA | CPS1_TILES },
	{ "ps_gfx7.rom",   0x080000, 0x04c5acbd, BRF_GRA | CPS1_TILES },
	{ "ps_gfx6.rom",   0x080000, 0xa544f4cc, BRF_GRA | CPS1_TILES },
	{ "ps_gfx8.rom",   0x080000, 0x8f02f436, BRF_GRA | CPS1_TILES },

	{ "ps_q.rom",      0x020000, 0x49ff4446, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ps_q1.rom",     0x080000, 0x31fd8726, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q2.rom",     0x080000, 0x980a9eef, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q3.rom",     0x080000, 0x0dd44491, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q4.rom",     0x080000, 0xbed42f03, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Punisher)
STD_ROM_FN(Punisher)

static struct BurnRomInfo PunishruRomDesc[] = {
	{ "psu26.rom",     0x020000, 0x9236d121, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu30.rom",     0x020000, 0x8320e501, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu27.rom",     0x020000, 0x61c960a1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu31.rom",     0x020000, 0x78d4c298, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu24.rom",     0x020000, 0x1cfecad7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu28.rom",     0x020000, 0xbdf921c1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu25.rom",     0x020000, 0xc51acc94, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psu29.rom",     0x020000, 0x52dce1ca, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ps_21.rom",     0x080000, 0x8affa5a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ps_gfx1.rom",   0x080000, 0x77b7ccab, BRF_GRA | CPS1_TILES },
	{ "ps_gfx3.rom",   0x080000, 0x0122720b, BRF_GRA | CPS1_TILES },
	{ "ps_gfx2.rom",   0x080000, 0x64fa58d4, BRF_GRA | CPS1_TILES },
	{ "ps_gfx4.rom",   0x080000, 0x60da42c8, BRF_GRA | CPS1_TILES },
	{ "ps_gfx5.rom",   0x080000, 0xc54ea839, BRF_GRA | CPS1_TILES },
	{ "ps_gfx7.rom",   0x080000, 0x04c5acbd, BRF_GRA | CPS1_TILES },
	{ "ps_gfx6.rom",   0x080000, 0xa544f4cc, BRF_GRA | CPS1_TILES },
	{ "ps_gfx8.rom",   0x080000, 0x8f02f436, BRF_GRA | CPS1_TILES },

	{ "ps_q.rom",      0x020000, 0x49ff4446, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ps_q1.rom",     0x080000, 0x31fd8726, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q2.rom",     0x080000, 0x980a9eef, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q3.rom",     0x080000, 0x0dd44491, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q4.rom",     0x080000, 0xbed42f03, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Punishru)
STD_ROM_FN(Punishru)

static struct BurnRomInfo PunishrjRomDesc[] = {
	{ "psj23.bin",     0x080000, 0x6b2fda52, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "psj22.bin",     0x080000, 0xe01036bc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "ps_21.rom",     0x080000, 0x8affa5a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ps_gfx1.rom",   0x080000, 0x77b7ccab, BRF_GRA | CPS1_TILES },
	{ "ps_gfx3.rom",   0x080000, 0x0122720b, BRF_GRA | CPS1_TILES },
	{ "ps_gfx2.rom",   0x080000, 0x64fa58d4, BRF_GRA | CPS1_TILES },
	{ "ps_gfx4.rom",   0x080000, 0x60da42c8, BRF_GRA | CPS1_TILES },
	{ "ps_gfx5.rom",   0x080000, 0xc54ea839, BRF_GRA | CPS1_TILES },
	{ "ps_gfx7.rom",   0x080000, 0x04c5acbd, BRF_GRA | CPS1_TILES },
	{ "ps_gfx6.rom",   0x080000, 0xa544f4cc, BRF_GRA | CPS1_TILES },
	{ "ps_gfx8.rom",   0x080000, 0x8f02f436, BRF_GRA | CPS1_TILES },

	{ "ps_q.rom",      0x020000, 0x49ff4446, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ps_q1.rom",     0x080000, 0x31fd8726, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q2.rom",     0x080000, 0x980a9eef, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q3.rom",     0x080000, 0x0dd44491, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "ps_q4.rom",     0x080000, 0xbed42f03, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Punishrj)
STD_ROM_FN(Punishrj)

static struct BurnRomInfo PunipicRomDesc[] = {
	{ "cpu5.bin",      0x080000, 0xc3151563, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cpu3.bin",      0x080000, 0x8c2593ac, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cpu4.bin",      0x080000, 0x665a5485, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cpu2.bin",      0x080000, 0xd7b13f39, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "gfx9.bin",      0x080000, 0x9b9a887a, BRF_GRA | CPS1_TILES },
	{ "gfx8.bin",      0x080000, 0x2b94287a, BRF_GRA | CPS1_TILES },
	{ "gfx7.bin",      0x080000, 0xe9bd74f5, BRF_GRA | CPS1_TILES },
	{ "gfx6.bin",      0x080000, 0xa5e1c8a4, BRF_GRA | CPS1_TILES },
	{ "gfx13.bin",     0x080000, 0x6d75a193, BRF_GRA | CPS1_TILES },
	{ "gfx12.bin",     0x080000, 0xa3c205c1, BRF_GRA | CPS1_TILES },
	{ "gfx11.bin",     0x080000, 0x22f2ec92, BRF_GRA | CPS1_TILES },
	{ "gfx10.bin",     0x080000, 0x763974c9, BRF_GRA | CPS1_TILES },
	
	{ "pic16c57",      0x004000, 0x00000000, BRF_PRG | BRF_NODUMP | CPS1_PIC },

	{ "sound.bin",     0x080000, 0xaeec9dc6, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Punipic)
STD_ROM_FN(Punipic)

static struct BurnRomInfo Punipic2RomDesc[] = {
	{ "prg4.bin",      0x080000, 0xc3151563, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "prg3.bin",      0x080000, 0x8c2593ac, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "prg2.bin",      0x080000, 0x665a5485, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "prg1.bin",      0x080000, 0xd7b13f39, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "pu11256.bin",   0x200000, 0x6581faea, BRF_GRA | CPS1_TILES },
	{ "pu13478.bin",   0x200000, 0x61613de4, BRF_GRA | CPS1_TILES },
	
	{ "pic16c57",      0x004000, 0x00000000, BRF_PRG | BRF_NODUMP | CPS1_PIC },

	{ "sound.bin",     0x080000, 0xaeec9dc6, BRF_SND | CPS1_OKIM6295_SAMPLES },
	
	{ "93c46.bin",     0x000080, 0x36ab4e7d, BRF_OPT },
};

STD_ROM_PICK(Punipic2)
STD_ROM_FN(Punipic2)

static struct BurnRomInfo Punipic3RomDesc[] = {
	{ "psb5b.rom",     0x080000, 0x58f42c05, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psb3b.rom",     0x080000, 0x90113db4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psb4a.rom",     0x080000, 0x665a5485, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "psb2a.rom",     0x080000, 0xd7b13f39, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "psb-a.rom",     0x200000, 0x57f0f5e3, BRF_GRA | CPS1_TILES },
	{ "psb-b.rom",     0x200000, 0xd9eb867e, BRF_GRA | CPS1_TILES },
};

STD_ROM_PICK(Punipic3)
STD_ROM_FN(Punipic3)

static struct BurnRomInfo QadRomDesc[] = {
	{ "qdu_36a.12f",   0x020000, 0xde9c24a0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "qdu_42a.12h",   0x020000, 0xcfe36f0c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "qdu_37a.13f",   0x020000, 0x10d22320, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "qdu_43a.13h",   0x020000, 0x15e6beb9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "qdu_09.4b",     0x020000, 0x8c3f9f44, BRF_GRA | CPS1_TILES },
	{ "qdu_01.4a",     0x020000, 0xf688cf8f, BRF_GRA | CPS1_TILES },
	{ "qdu_13.9b",     0x020000, 0xafbd551b, BRF_GRA | CPS1_TILES },
	{ "qdu_05.9a",     0x020000, 0xc3db0910, BRF_GRA | CPS1_TILES },
	{ "qdu_24.5e",     0x020000, 0x2f1bd0ec, BRF_GRA | CPS1_TILES },
	{ "qdu_17.5c",     0x020000, 0xa812f9e2, BRF_GRA | CPS1_TILES },
	{ "qdu_38.8h",     0x020000, 0xccdddd1f, BRF_GRA | CPS1_TILES },
	{ "qdu_32.8f",     0x020000, 0xa8d295d3, BRF_GRA | CPS1_TILES },

	{ "qdu_23.13b",    0x010000, 0xcfb5264b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "qdu_30.12c",    0x020000, 0xf190da84, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "qdu_31.13c",    0x020000, 0xb7583f73, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Qad)
STD_ROM_FN(Qad)

static struct BurnRomInfo QadjRomDesc[] = {
	{ "qad23a.bin",    0x080000, 0x4d3553de, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "qad22a.bin",    0x080000, 0x3191ddd0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "qad01.bin",     0x080000, 0x9d853b57, BRF_GRA | CPS1_TILES },
	{ "qad02.bin",     0x080000, 0xb35976c4, BRF_GRA | CPS1_TILES },
	{ "qad03.bin",     0x080000, 0xcea4ca8c, BRF_GRA | CPS1_TILES },
	{ "qad04.bin",     0x080000, 0x41b74d1b, BRF_GRA | CPS1_TILES },

	{ "qad09.bin",     0x010000, 0x733161cc, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "qad18.bin",     0x020000, 0x2bfe6f6a, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "qad19.bin",     0x020000, 0x13d3236b, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Qadj)
STD_ROM_FN(Qadj)

static struct BurnRomInfo Qtono2RomDesc[] = {
	{ "tn2j-30.11e",   0x020000, 0x9226eb5e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-37.11f",   0x020000, 0xd1d30da1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-31.12e",   0x020000, 0x015e6a8a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-38.12f",   0x020000, 0x1f139bcc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-28.9e",    0x020000, 0x86d27f71, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-35.9f",    0x020000, 0x7a1ab87d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-29.10e",   0x020000, 0x9c384e99, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "tn2j-36.10f",   0x020000, 0x4c4b2a0a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "tn2-02m.4a",    0x080000, 0xf2016a34, BRF_GRA | CPS1_TILES },
	{ "tn2-04m.6a",    0x080000, 0x094e0fb1, BRF_GRA | CPS1_TILES },
	{ "tn2-01m.3a",    0x080000, 0xcb950cf9, BRF_GRA | CPS1_TILES },
	{ "tn2-03m.5a",    0x080000, 0x18a5bf59, BRF_GRA | CPS1_TILES },
	{ "tn2-11m.4c",    0x080000, 0xd0edd30b, BRF_GRA | CPS1_TILES },
	{ "tn2-13m.6c",    0x080000, 0x426621c3, BRF_GRA | CPS1_TILES },
	{ "tn2-10m.3c",    0x080000, 0xa34ece70, BRF_GRA | CPS1_TILES },
	{ "tn2-12m.5c",    0x080000, 0xe04ff2f4, BRF_GRA | CPS1_TILES },

	{ "tn2j-09.12a",   0x008000, 0x6d8edcef, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tn2j-18.11c",   0x020000, 0xa40bf9a7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "tn2j-19.12c",   0x020000, 0x5b3b931e, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Qtono2)
STD_ROM_FN(Qtono2)

static struct BurnRomInfo Sf2RomDesc[] = {
	{ "sf2e.30g",      0x020000, 0xfe39ee33, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.37g",      0x020000, 0xfb92cd74, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.31g",      0x020000, 0x69a0a301, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.38g",      0x020000, 0x5e22db70, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.28g",      0x020000, 0x8bf9f1e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.35g",      0x020000, 0x626ef934, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2)
STD_ROM_FN(Sf2)

static struct BurnRomInfo Sf2ebRomDesc[] = {
	{ "sf2_30a.bin",   0x020000, 0x57bd7051, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e_37b.rom",  0x020000, 0x62691cdd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_31a.bin",   0x020000, 0xa673143d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_38a.bin",   0x020000, 0x4c2ccef7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_28a.bin",   0x020000, 0x4009955e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_35a.bin",   0x020000, 0x8c1f3994, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2eb)
STD_ROM_FN(Sf2eb)

static struct BurnRomInfo Sf2uaRomDesc[] = {
	{ "sf2u.30a",      0x020000, 0x08beb861, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37a",      0x020000, 0xb7638d69, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.31a",      0x020000, 0x0d5394e0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.38a",      0x020000, 0x42d6a79e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.28a",      0x020000, 0x387a175c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.35a",      0x020000, 0xa1a5adcc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ua)
STD_ROM_FN(Sf2ua)

static struct BurnRomInfo Sf2ubRomDesc[] = {
	{ "sf2_30a.bin",   0x020000, 0x57bd7051, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37b",      0x020000, 0x4a54d479, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_31a.bin",   0x020000, 0xa673143d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_38a.bin",   0x020000, 0x4c2ccef7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_28a.bin",   0x020000, 0x4009955e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_35a.bin",   0x020000, 0x8c1f3994, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ub)
STD_ROM_FN(Sf2ub)

static struct BurnRomInfo Sf2udRomDesc[] = {
	{ "sf2u.30d",      0x020000, 0x4bb2657c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37d",      0x020000, 0xb33b42f2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.31d",      0x020000, 0xd57b67d7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.38d",      0x020000, 0x9c8916ef, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.28d",      0x020000, 0x175819d1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.35d",      0x020000, 0x82060da4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ud)
STD_ROM_FN(Sf2ud)

static struct BurnRomInfo Sf2ueRomDesc[] = {
	{ "sf2u.30e",      0x020000, 0xf37cd088, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37e",      0x020000, 0x6c61a513, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.31e",      0x020000, 0x7c4771b4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.38e",      0x020000, 0xa4bd0cd9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.28e",      0x020000, 0xe3b95625, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.35e",      0x020000, 0x3648769a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ue)
STD_ROM_FN(Sf2ue)

static struct BurnRomInfo Sf2ufRomDesc[] = {
	{ "sf2e.30g",      0x020000, 0xfe39ee33, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37f",      0x020000, 0x169e7388, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.31g",      0x020000, 0x69a0a301, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.38f",      0x020000, 0x1510e4e2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.28f",      0x020000, 0xacd8175b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.35f",      0x020000, 0xc0a80bd1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2uf)
STD_ROM_FN(Sf2uf)

static struct BurnRomInfo Sf2uiRomDesc[] = {
	{ "sf2e.30g",      0x020000, 0xfe39ee33, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37i",      0x020000, 0x9df707dd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2e.31g",      0x020000, 0x69a0a301, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.38i",      0x020000, 0x4cb46daf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.28i",      0x020000, 0x1580be4c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.35i",      0x020000, 0x1468d185, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ui)
STD_ROM_FN(Sf2ui)

static struct BurnRomInfo Sf2ukRomDesc[] = {
	{ "sf2u.30k",      0x020000, 0x8f66076c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.37k",      0x020000, 0x4e1f6a83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.31k",      0x020000, 0xf9f89f60, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.38k",      0x020000, 0x6ce0a85a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.28k",      0x020000, 0x8e958f31, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2u.35k",      0x020000, 0xfce76fad, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2uk)
STD_ROM_FN(Sf2uk)

static struct BurnRomInfo Sf2jRomDesc[] = {
	{ "sf2j30.bin",    0x020000, 0x79022b31, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j37.bin",    0x020000, 0x516776ec, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j31.bin",    0x020000, 0xfe15cb39, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j38.bin",    0x020000, 0x38614d70, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j28.bin",    0x020000, 0xd283187a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j35.bin",    0x020000, 0xd28158e4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2j)
STD_ROM_FN(Sf2j)

static struct BurnRomInfo Sf2jaRomDesc[] = {
	{ "sf2_30a.bin",   0x020000, 0x57bd7051, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j_37a.bin",  0x020000, 0x1e1f6844, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_31a.bin",   0x020000, 0xa673143d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_38a.bin",   0x020000, 0x4c2ccef7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_28a.bin",   0x020000, 0x4009955e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_35a.bin",   0x020000, 0x8c1f3994, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ja)
STD_ROM_FN(Sf2ja)

static struct BurnRomInfo Sf2jcRomDesc[] = {
	{ "sf2_30c.bin",   0x020000, 0x8add35ec, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2j_37c.bin",  0x020000, 0x0d74a256, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_31c.bin",   0x020000, 0xc4fff4a9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_38c.bin",   0x020000, 0x8210fc0e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_28c.bin",   0x020000, 0x6eddd5e8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_35c.bin",   0x020000, 0x6bcb404c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2jc)
STD_ROM_FN(Sf2jc)

static struct BurnRomInfo Sf2bRomDesc[] = {
	{ "u3.bin",        0x020000, 0x2c9ece7c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u4.bin",        0x020000, 0x4efb4c7a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "sf2_31a.bin",   0x020000, 0xa673143d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_38a.bin",   0x020000, 0x4c2ccef7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

//	{ "sf2-4.bin",     0x020000, 0x76f9f91f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },	
//	{ "sf2-3.bin",     0x020000, 0xe8f14362, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "u6.bin",        0x020000, 0x5cfc3f39, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u5.bin",        0x020000, 0x47dd24b6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
		
	{ "sf2_29a.bin",   0x020000, 0xbb4af315, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sf2_36a.bin",   0x020000, 0xc02a13eb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
//	{ "sf2-1.bin",     0x020000, 0x6de44671, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
//	{ "sf2-2.bin",     0x020000, 0xbf0cd819, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	
	
	{ "sf2_06.bin",    0x080000, 0x22c9cc8e, BRF_GRA | CPS1_TILES },
	{ "sf2_08.bin",    0x080000, 0x57213be8, BRF_GRA | CPS1_TILES },
	{ "sf2_05.bin",    0x080000, 0xba529b4f, BRF_GRA | CPS1_TILES },
	{ "sf2_07.bin",    0x080000, 0x4b1b33a8, BRF_GRA | CPS1_TILES },
	{ "sf2_15.bin",    0x080000, 0x2c7e2229, BRF_GRA | CPS1_TILES },
	{ "sf2_17.bin",    0x080000, 0xb5548f17, BRF_GRA | CPS1_TILES },
	{ "sf2_14.bin",    0x080000, 0x14b84312, BRF_GRA | CPS1_TILES },
	{ "sf2_16.bin",    0x080000, 0x5e9cd89a, BRF_GRA | CPS1_TILES },
	{ "sf2_25.bin",    0x080000, 0x994bfa58, BRF_GRA | CPS1_TILES },
	{ "sf2_27.bin",    0x080000, 0x3e66ad9d, BRF_GRA | CPS1_TILES },
	{ "sf2_24.bin",    0x080000, 0xc1befaa8, BRF_GRA | CPS1_TILES },
	{ "sf2_26.bin",    0x080000, 0x0627c831, BRF_GRA | CPS1_TILES },

	{ "sf2_09.bin",    0x010000, 0xa4823a1b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sf2_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sf2_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2b)
STD_ROM_FN(Sf2b)

static struct BurnRomInfo Sf2ceRomDesc[] = {
	{ "sf2ce.23",      0x080000, 0x3f846b74, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2ce.22",      0x080000, 0x99f1cca4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ce)
STD_ROM_FN(Sf2ce)

static struct BurnRomInfo Sf2cejRomDesc[] = {
	{ "s92j_23b.bin",  0x080000, 0x140876c5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92j_22b.bin",  0x080000, 0x2fbb3bfe, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2cej)
STD_ROM_FN(Sf2cej)

static struct BurnRomInfo Sf2ceuaRomDesc[] = {
	{ "s92u-23a",      0x080000, 0xac44415b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2ce.22",      0x080000, 0x99f1cca4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ceua)
STD_ROM_FN(Sf2ceua)

static struct BurnRomInfo Sf2ceubRomDesc[] = {
	{ "s92-23b",       0x080000, 0x996a3015, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92-22b",       0x080000, 0x2bbe15ed, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92-21b",       0x080000, 0xb383cb1c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ceub)
STD_ROM_FN(Sf2ceub)

static struct BurnRomInfo Sf2ceucRomDesc[] = {
	{ "s92-23c",       0x080000, 0x0a8b6aa2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92-22c",       0x080000, 0x5fd8630b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92-21b",       0x080000, 0xb383cb1c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ceuc)
STD_ROM_FN(Sf2ceuc)

static struct BurnRomInfo Sf2accp2RomDesc[] = {
	{ "sf2ca-23.bin",  0x080000, 0x36c3ba2f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2ca-22.bin",  0x080000, 0x0550453d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2ca-21.bin",  0x040000, 0x4c1c43ba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2accp2)
STD_ROM_FN(Sf2accp2)

static struct BurnRomInfo Sf2rbRomDesc[] = {
	{ "sf2d__23.rom",  0x080000, 0x450532b0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2d__22.rom",  0x080000, 0xfe9d9cf5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2rb)
STD_ROM_FN(Sf2rb)

static struct BurnRomInfo Sf2rb2RomDesc[] = {
	{ "31.bin",        0x020000, 0x87954a41, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "27.bin",        0x020000, 0x40296ecd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "30.bin",        0x020000, 0x8141fe32, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },	
	{ "26.bin",        0x020000, 0xa6974195, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "29.bin",        0x020000, 0x7d9c479c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "25.bin",        0x020000, 0x9ef8f772, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "28.bin",        0x020000, 0xff728865, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "24.bin",        0x020000, 0x93579684, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },	
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2rb2)
STD_ROM_FN(Sf2rb2)

static struct BurnRomInfo Sf2redRomDesc[] = {
	{ "sf2red.23",     0x080000, 0x40276abb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2red.22",     0x080000, 0x18daf387, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2red.21",     0x080000, 0x52c486bb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2red)
STD_ROM_FN(Sf2red)

static struct BurnRomInfo Sf2v004RomDesc[] = {
	{ "sf2v004.23",    0x080000, 0x52d19f2c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2v004.22",    0x080000, 0x4b26fde7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2red.21",     0x080000, 0x52c486bb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2v004)
STD_ROM_FN(Sf2v004)

static struct BurnRomInfo Sf2hfRomDesc[] = {
	{ "s92e_23a.bin",  0x080000, 0x2DD72514, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2_22.bin",    0x080000, 0xaea6e035, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2_21.bin",    0x080000, 0xfd200288, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s2t_10.bin",    0x080000, 0x3c042686, BRF_GRA | CPS1_TILES },
	{ "s2t_11.bin",    0x080000, 0x8b7e7183, BRF_GRA | CPS1_TILES },
	{ "s2t_12.bin",    0x080000, 0x293c888c, BRF_GRA | CPS1_TILES },
	{ "s2t_13.bin",    0x080000, 0x842b35a4, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2hf)
STD_ROM_FN(Sf2hf)

static struct BurnRomInfo Sf2tRomDesc[] = {
	{ "sf2.23",        0x080000, 0x89a1fc38, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2_22.bin",    0x080000, 0xaea6e035, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2_21.bin",    0x080000, 0xfd200288, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s2t_10.bin",    0x080000, 0x3c042686, BRF_GRA | CPS1_TILES },
	{ "s2t_11.bin",    0x080000, 0x8b7e7183, BRF_GRA | CPS1_TILES },
	{ "s2t_12.bin",    0x080000, 0x293c888c, BRF_GRA | CPS1_TILES },
	{ "s2t_13.bin",    0x080000, 0x842b35a4, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2t)
STD_ROM_FN(Sf2t)

static struct BurnRomInfo Sf2tjRomDesc[] = {
	{ "s2tj_23.bin",   0x080000, 0xea73b4dc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s2t_22.bin",    0x080000, 0xaea6e035, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s2t_21.bin",    0x080000, 0xfd200288, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s2t_10.bin",    0x080000, 0x3c042686, BRF_GRA | CPS1_TILES },
	{ "s2t_11.bin",    0x080000, 0x8b7e7183, BRF_GRA | CPS1_TILES },
	{ "s2t_12.bin",    0x080000, 0x293c888c, BRF_GRA | CPS1_TILES },
	{ "s2t_13.bin",    0x080000, 0x842b35a4, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2tj)
STD_ROM_FN(Sf2tj)

static struct BurnRomInfo Sf2yycRomDesc[] = {
	{ "b12.rom",      0x0080000, 0x8f742fd5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "b14.rom",      0x0080000, 0x8831ec7f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "b11.rom",      0x0020000, 0x94a46525, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "b13.rom",      0x0020000, 0x8fb3dd47, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2yyc)
STD_ROM_FN(Sf2yyc)

static struct BurnRomInfo KouryuRomDesc[] = {
	{ "u196.rom",     0x0080000, 0xb23a869d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u222.rom",     0x0080000, 0x9236a79a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u195.rom",     0x0020000, 0xc95e4443, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u221.rom",     0x0020000, 0x64e6e091, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },	

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Kouryu)
STD_ROM_FN(Kouryu)

static struct BurnRomInfo Sf2m1RomDesc[] = {
	{ "222e",         0x0080000, 0x1e20d0a3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "196e",         0x0080000, 0x88cc38a3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m1)
STD_ROM_FN(Sf2m1)

static struct BurnRomInfo Sf2m2RomDesc[] = {
	{ "ch222esp",     0x0080000, 0x9e6d058a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "ch196esp",     0x0080000, 0xed2ff437, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m2)
STD_ROM_FN(Sf2m2)

static struct BurnRomInfo Sf2m3RomDesc[] = {
	{ "u222chp",      0x0080000, 0xdb567b66, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196chp",      0x0080000, 0x95ea597e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m3)
STD_ROM_FN(Sf2m3)

static struct BurnRomInfo Sf2m4RomDesc[] = {
	{ "u222ne",       0x0080000, 0x7133489e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196ne",       0x0080000, 0xb07a4f90, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m4)
STD_ROM_FN(Sf2m4)

static struct BurnRomInfo Sf2m5RomDesc[] = {
	{ "u222",         0x0080000, 0x03991fba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196",         0x0080000, 0x39f15a1e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m5)
STD_ROM_FN(Sf2m5)

static struct BurnRomInfo Sf2m6RomDesc[] = {
	{ "u222-6b",      0x0080000, 0x0a3692be, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196-6b",      0x0080000, 0x80454da7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m6)
STD_ROM_FN(Sf2m6)

static struct BurnRomInfo Sf2m7RomDesc[] = {
	{ "u222-2i",      0x0040000, 0x1ca7adbd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196-2i",      0x0040000, 0xf758408c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u222-2s",      0x0040000, 0x720cea3e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196-2s",      0x0040000, 0x9932832c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "s92_21a.bin",  0x0080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m7)
STD_ROM_FN(Sf2m7)

static struct BurnRomInfo Sf2m8RomDesc[] = {
	{ "u222chp",      0x0080000, 0xdb567b66, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u196chp",      0x0080000, 0x95ea597e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u221.epr",     0x0020000, 0xd8276822, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "u195.epr",     0x0020000, 0x924c6ce2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m8)
STD_ROM_FN(Sf2m8)

static struct BurnRomInfo Sf2m13RomDesc[] = {
	{ "222-040.13",   0x0080000, 0xec6f5cb3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "196-040.13",   0x0080000, 0x0e9ac52b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "221-010.13",   0x0020000, 0x8226c11c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "195-010.13",   0x0020000, 0x924c6ce2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s2t_10.bin",    0x080000, 0x3c042686, BRF_GRA | CPS1_TILES },
	{ "s2t_11.bin",    0x080000, 0x8b7e7183, BRF_GRA | CPS1_TILES },
	{ "s2t_12.bin",    0x080000, 0x293c888c, BRF_GRA | CPS1_TILES },
	{ "s2t_13.bin",    0x080000, 0x842b35a4, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2m13)
STD_ROM_FN(Sf2m13)

static struct BurnRomInfo Sf2tlonaRomDesc[] = {
	{ "t14mt.1",       0x080000, 0x158635ca, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "t14m.2",        0x080000, 0x882cd1c4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u221t.1m",      0x020000, 0x1073b7b6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u195t.1m",      0x020000, 0xdb7e1f72, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2tlona)
STD_ROM_FN(Sf2tlona)

static struct BurnRomInfo Sf2tlonbRomDesc[] = {
	{ "t14m.1",        0x080000, 0xafc7bd18, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "t14m.2",        0x080000, 0x882cd1c4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u221.1m",       0x020000, 0xd1707134, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u195.1m",       0x020000, 0xcd1d5666, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2tlonb)
STD_ROM_FN(Sf2tlonb)

static struct BurnRomInfo Sf2cebrRomDesc[] = {
	{ "sf2cebr.23",    0x080000, 0x74e848ee, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2cebr.22",    0x080000, 0xc3c49626, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92br_10.bin",  0x080000, 0xb3e1dd5f, BRF_GRA | CPS1_TILES },
	{ "s92br_11.bin",  0x080000, 0xf13af812, BRF_GRA | CPS1_TILES },
	{ "s92br_12.bin",  0x080000, 0x10ce42af, BRF_GRA | CPS1_TILES },
	{ "s92br_13.bin",  0x080000, 0x32cf5af3, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2cebr)
STD_ROM_FN(Sf2cebr)

static struct BurnRomInfo Sf2cehRomDesc[] = {
	{ "sf2ceh.23",     0x080000, 0x25dc14c8, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "sf2ceh.22",     0x080000, 0x1c9dd91c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },
	{ "s92_21a.bin",   0x080000, 0x925a7877, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ceh)
STD_ROM_FN(Sf2ceh)

static struct BurnRomInfo Sf2thRomDesc[] = {
	{ "stf2th-7.bin", 0x0080000, 0x03991fba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "stf2th-5.bin", 0x0080000, 0x3127302c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "sf2th-4.bin",  0x0020000, 0xc95e4443, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "stf2th-6.bin", 0x0020000, 0x64e6e091, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	
	// Some extra gfx too

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2th)
STD_ROM_FN(Sf2th)

static struct BurnRomInfo SfzchRomDesc[] = {
	{ "sfzch23",       0x080000, 0x1140743f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfza22",        0x080000, 0x8d9b2480, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfzch21",       0x080000, 0x5435225d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfza20",        0x080000, 0x806e8f38, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "sfz01",         0x080000, 0x0dd53e62, BRF_GRA | CPS1_TILES },
	{ "sfz02",         0x080000, 0x94c31e3f, BRF_GRA | CPS1_TILES },
	{ "sfz03",         0x080000, 0x9584ac85, BRF_GRA | CPS1_TILES },
	{ "sfz04",         0x080000, 0xb983624c, BRF_GRA | CPS1_TILES },
	{ "sfz05",         0x080000, 0x2b47b645, BRF_GRA | CPS1_TILES },
	{ "sfz06",         0x080000, 0x74fd9fb1, BRF_GRA | CPS1_TILES },
	{ "sfz07",         0x080000, 0xbb2c734d, BRF_GRA | CPS1_TILES },
	{ "sfz08",         0x080000, 0x454f7868, BRF_GRA | CPS1_TILES },
	{ "sfz10",         0x080000, 0x2a7d675e, BRF_GRA | CPS1_TILES },
	{ "sfz11",         0x080000, 0xe35546c8, BRF_GRA | CPS1_TILES },
	{ "sfz12",         0x080000, 0xf122693a, BRF_GRA | CPS1_TILES },
	{ "sfz13",         0x080000, 0x7cf942c8, BRF_GRA | CPS1_TILES },
	{ "sfz14",         0x080000, 0x09038c81, BRF_GRA | CPS1_TILES },
	{ "sfz15",         0x080000, 0x1aa17391, BRF_GRA | CPS1_TILES },
	{ "sfz16",         0x080000, 0x19a5abd6, BRF_GRA | CPS1_TILES },
	{ "sfz17",         0x080000, 0x248b3b73, BRF_GRA | CPS1_TILES },

	{ "sfz09",         0x010000, 0xc772628b, BRF_PRG | CPS1_Z80_PROGRAM },
	
	{ "sfz18",         0x020000, 0x61022b2d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sfz19",         0x020000, 0x3b5886d5, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sfzch)
STD_ROM_FN(Sfzch)

static struct BurnRomInfo SlammastRomDesc[] = {
	{ "mbe_23e.rom",   0x080000, 0x5394057a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_24b.rom",   0x020000, 0x95d5e729, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_28b.rom",   0x020000, 0xb1c7cbcb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_25b.rom",   0x020000, 0xa50d3fd4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },	
	{ "mbe_29b.rom",   0x020000, 0x08e32e56, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_21a.rom",   0x080000, 0xd5007b05, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_20a.rom",   0x080000, 0xaeb557b0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_qa.rom",     0x020000, 0xe21a03c4, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Slammast)
STD_ROM_FN(Slammast)

static struct BurnRomInfo SlammasaRomDesc[] = {
	{ "mba_23e.rom",   0x080000, 0x027e49db, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_24b.rom",   0x020000, 0x95d5e729, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_28b.rom",   0x020000, 0xb1c7cbcb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_25b.rom",   0x020000, 0xa50d3fd4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },	
	{ "mbe_29b.rom",   0x020000, 0x08e32e56, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_21a.rom",   0x080000, 0xd5007b05, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_20a.rom",   0x080000, 0xaeb557b0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_qa.rom",     0x020000, 0xe21a03c4, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Slammasa)
STD_ROM_FN(Slammasa)

static struct BurnRomInfo SlammasuRomDesc[] = {
	{ "mbu-23e.rom",   0x080000, 0x224f0062, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_24b.rom",   0x020000, 0x95d5e729, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_28b.rom",   0x020000, 0xb1c7cbcb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_25b.rom",   0x020000, 0xa50d3fd4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_29b.rom",   0x020000, 0x08e32e56, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_21a.rom",   0x080000, 0xd5007b05, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbu-20a.rom",   0x080000, 0xfc848af5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_qa.rom",     0x020000, 0xe21a03c4, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Slammasu)
STD_ROM_FN(Slammasu)

static struct BurnRomInfo MbomberjRomDesc[] = {
	{ "mbj23e",        0x080000, 0x0d06036a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_24b.rom",   0x020000, 0x95d5e729, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_28b.rom",   0x020000, 0xb1c7cbcb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_25b.rom",   0x020000, 0xa50d3fd4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_29b.rom",   0x020000, 0x08e32e56, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_21a.rom",   0x080000, 0xd5007b05, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_20a.rom",   0x080000, 0xaeb557b0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mbj_01.bin",    0x080000, 0xa53b1c81, BRF_GRA | CPS1_TILES },
	{ "mbj_03.bin",    0x080000, 0x23fe10f6, BRF_GRA | CPS1_TILES },
	{ "mbj_02.bin",    0x080000, 0xcb866c2f, BRF_GRA | CPS1_TILES },
	{ "mbj_04.bin",    0x080000, 0xc9143e75, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_qa.rom",     0x020000, 0xe21a03c4, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbomberj)
STD_ROM_FN(Mbomberj)

static struct BurnRomInfo MbombrdRomDesc[] = {
	{ "mbd_26.bin",    0x020000, 0x72b7451c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbde_30.rom",   0x020000, 0xa036dc16, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_27.bin",    0x020000, 0x4086f534, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_31.bin",    0x020000, 0x085f47f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_24.bin",    0x020000, 0xc20895a5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_28.bin",    0x020000, 0x2618d5e1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_25.bin",    0x020000, 0x9bdb6b11, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_29.bin",    0x020000, 0x3f52d5e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_21.bin",    0x080000, 0x690c026a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbd_20.bin",    0x080000, 0xb8b2139b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_q.bin",      0x020000, 0xd6fa76d1, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbombrd)
STD_ROM_FN(Mbombrd)

static struct BurnRomInfo MbombrdjRomDesc[] = {
	{ "mbd_26.bin",    0x020000, 0x72b7451c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbdj_30.bin",   0x020000, 0xbeff31cf, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_27.bin",    0x020000, 0x4086f534, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_31.bin",    0x020000, 0x085f47f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_24.bin",    0x020000, 0xc20895a5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_28.bin",    0x020000, 0x2618d5e1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_25.bin",    0x020000, 0x9bdb6b11, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_29.bin",    0x020000, 0x3f52d5e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_21.bin",    0x080000, 0x690c026a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbd_20.bin",    0x080000, 0xb8b2139b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mbj_01.bin",    0x080000, 0xa53b1c81, BRF_GRA | CPS1_TILES },
	{ "mbj_03.bin",    0x080000, 0x23fe10f6, BRF_GRA | CPS1_TILES },
	{ "mbj_02.bin",    0x080000, 0xcb866c2f, BRF_GRA | CPS1_TILES },
	{ "mbj_04.bin",    0x080000, 0xc9143e75, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_q.bin",      0x020000, 0xd6fa76d1, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbombrdj)
STD_ROM_FN(Mbombrdj)

static struct BurnRomInfo StriderRomDesc[] = {
	{ "strider.30",    0x020000, 0xda997474, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "strider.35",    0x020000, 0x5463aaa3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "strider.31",    0x020000, 0xd20786db, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "strider.36",    0x020000, 0x21aa2863, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "st-14.8h",      0x080000, 0x9b3cfc08, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "st-2.8a",       0x080000, 0x4eee9aea, BRF_GRA | CPS1_TILES },
	{ "st-11.10a",     0x080000, 0x2d7f21e4, BRF_GRA | CPS1_TILES },
	{ "st-5.4a",       0x080000, 0x7705aa46, BRF_GRA | CPS1_TILES },
	{ "st-9.6a",       0x080000, 0x5b18b722, BRF_GRA | CPS1_TILES },
	{ "st-1.7a",       0x080000, 0x005f000b, BRF_GRA | CPS1_TILES },
	{ "st-10.9a",      0x080000, 0xb9441519, BRF_GRA | CPS1_TILES },
	{ "st-4.3a",       0x080000, 0xb7d04e8b, BRF_GRA | CPS1_TILES },
	{ "st-8.5a",       0x080000, 0x6b4713b4, BRF_GRA | CPS1_TILES },

	{ "strider.09",    0x010000, 0x2ed403bc, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "strider.18",    0x020000, 0x4386bc80, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "strider.19",    0x020000, 0x444536d7, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Strider)
STD_ROM_FN(Strider)

static struct BurnRomInfo StridruaRomDesc[] = {
	{ "strid.30",      0x020000, 0x66aec273, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "strid.35",      0x020000, 0x50e0e865, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "strid.31",      0x020000, 0xeae93bd1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "strid.36",      0x020000, 0xb904a31d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "st-14.8h",      0x080000, 0x9b3cfc08, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "st-2.8a",       0x080000, 0x4eee9aea, BRF_GRA | CPS1_TILES },
	{ "st-11.10a",     0x080000, 0x2d7f21e4, BRF_GRA | CPS1_TILES },
	{ "st-5.4a",       0x080000, 0x7705aa46, BRF_GRA | CPS1_TILES },
	{ "st-9.6a",       0x080000, 0x5b18b722, BRF_GRA | CPS1_TILES },
	{ "st-1.7a",       0x080000, 0x005f000b, BRF_GRA | CPS1_TILES },
	{ "st-10.9a",      0x080000, 0xb9441519, BRF_GRA | CPS1_TILES },
	{ "st-4.3a",       0x080000, 0xb7d04e8b, BRF_GRA | CPS1_TILES },
	{ "st-8.5a",       0x080000, 0x6b4713b4, BRF_GRA | CPS1_TILES },

	{ "strid.09",      0x010000, 0x08d63519, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "strider.18",    0x020000, 0x4386bc80, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "strider.19",    0x020000, 0x444536d7, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Stridrua)
STD_ROM_FN(Stridrua)

static struct BurnRomInfo StriderjRomDesc[] = {
	{ "sthj23.bin",    0x080000, 0x046e7b12, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "st-14.8h",      0x080000, 0x9b3cfc08, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "st-2.8a",       0x080000, 0x4eee9aea, BRF_GRA | CPS1_TILES },
	{ "st-11.10a",     0x080000, 0x2d7f21e4, BRF_GRA | CPS1_TILES },
	{ "st-5.4a",       0x080000, 0x7705aa46, BRF_GRA | CPS1_TILES },
	{ "st-9.6a",       0x080000, 0x5b18b722, BRF_GRA | CPS1_TILES },
	{ "st-1.7a",       0x080000, 0x005f000b, BRF_GRA | CPS1_TILES },
	{ "st-10.9a",      0x080000, 0xb9441519, BRF_GRA | CPS1_TILES },
	{ "st-4.3a",       0x080000, 0xb7d04e8b, BRF_GRA | CPS1_TILES },
	{ "st-8.5a",       0x080000, 0x6b4713b4, BRF_GRA | CPS1_TILES },

	{ "strider.09",    0x010000, 0x2ed403bc, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "strider.18",    0x020000, 0x4386bc80, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "strider.19",    0x020000, 0x444536d7, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Striderj)
STD_ROM_FN(Striderj)

static struct BurnRomInfo StridrjaRomDesc[] = {
	{ "sth36.bin",     0x020000, 0x53c7b006, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth42.bin",     0x020000, 0x4037f65f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth37.bin",     0x020000, 0x80e8877d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth43.bin",     0x020000, 0x6b3fa466, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth34.bin",     0x020000, 0xbea770b5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth40.bin",     0x020000, 0x43b922dc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth35.bin",     0x020000, 0x5cc429da, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "sth41.bin",     0x020000, 0x50af457f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "sth09.bin",     0x020000, 0x1ef6bfbd, BRF_GRA | CPS1_TILES },
	{ "sth01.bin",     0x020000, 0x1e21b0c1, BRF_GRA | CPS1_TILES },
	{ "sth13.bin",     0x020000, 0x063263ae, BRF_GRA | CPS1_TILES },
	{ "sth05.bin",     0x020000, 0xec9f8714, BRF_GRA | CPS1_TILES },	
	{ "sth24.bin",     0x020000, 0x6356f4d2, BRF_GRA | CPS1_TILES },	
	{ "sth17.bin",     0x020000, 0xb4f73d86, BRF_GRA | CPS1_TILES },
	{ "sth38.bin",     0x020000, 0xee5abfc2, BRF_GRA | CPS1_TILES },
	{ "sth32.bin",     0x020000, 0x44a206a3, BRF_GRA | CPS1_TILES },	
	{ "sth10.bin",     0x020000, 0xdf3dd3bc, BRF_GRA | CPS1_TILES },	
	{ "sth02.bin",     0x020000, 0xc75f9ea0, BRF_GRA | CPS1_TILES },
	{ "sth14.bin",     0x020000, 0x6c03e19d, BRF_GRA | CPS1_TILES },
	{ "sth06.bin",     0x020000, 0xd84f5478, BRF_GRA | CPS1_TILES },	
	{ "sth25.bin",     0x020000, 0x921e506a, BRF_GRA | CPS1_TILES },
	{ "sth18.bin",     0x020000, 0x5b318956, BRF_GRA | CPS1_TILES },
	{ "sth39.bin",     0x020000, 0x9321d6aa, BRF_GRA | CPS1_TILES },
	{ "sth33.bin",     0x020000, 0xb47ddfc7, BRF_GRA | CPS1_TILES },	
	{ "sth11.bin",     0x020000, 0x2484f241, BRF_GRA | CPS1_TILES },
	{ "sth03.bin",     0x020000, 0xaaa07245, BRF_GRA | CPS1_TILES },
	{ "sth15.bin",     0x020000, 0xe415d943, BRF_GRA | CPS1_TILES },
	{ "sth07.bin",     0x020000, 0x97d072d2, BRF_GRA | CPS1_TILES },
	{ "sth26.bin",     0x020000, 0x0ebfcb02, BRF_GRA | CPS1_TILES },
	{ "sth19.bin",     0x020000, 0x257ce683, BRF_GRA | CPS1_TILES },
	{ "sth28.bin",     0x020000, 0x98ac8cd1, BRF_GRA | CPS1_TILES },
	{ "sth21.bin",     0x020000, 0x538d9423, BRF_GRA | CPS1_TILES },
	{ "sth12.bin",     0x020000, 0xf670a477, BRF_GRA | CPS1_TILES },
	{ "sth04.bin",     0x020000, 0x853d3e01, BRF_GRA | CPS1_TILES },
	{ "sth16.bin",     0x020000, 0x4092019f, BRF_GRA | CPS1_TILES },
	{ "sth08.bin",     0x020000, 0x2ce9b4c7, BRF_GRA | CPS1_TILES },
	{ "sth27.bin",     0x020000, 0xf82c88d9, BRF_GRA | CPS1_TILES },
	{ "sth20.bin",     0x020000, 0xeb584dd4, BRF_GRA | CPS1_TILES },
	{ "sth29.bin",     0x020000, 0x34ae2997, BRF_GRA | CPS1_TILES },
	{ "sth22.bin",     0x020000, 0x78dd9c48, BRF_GRA | CPS1_TILES },

	{ "sth23.bin",     0x010000, 0x2ed403bc, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "sth30.bin",     0x020000, 0x4386bc80, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sth31.bin",     0x020000, 0x444536d7, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Stridrja)
STD_ROM_FN(Stridrja)

static struct BurnRomInfo UnsquadRomDesc[] = {
	{ "aru_30.11f",    0x020000, 0x24d8f88d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "aru_35.11h",    0x020000, 0x8b954b59, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "aru_31.12f",    0x020000, 0x33e9694b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "aru_36.12h",    0x020000, 0x7cc8fb9e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar-32m.8h",     0x080000, 0xae1d7fb0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "ar-5m.7a",      0x080000, 0xbf4575d8, BRF_GRA | CPS1_TILES },
	{ "ar-7m.9a",      0x080000, 0xa02945f4, BRF_GRA | CPS1_TILES },
	{ "ar-1m.3a",      0x080000, 0x5965ca8d, BRF_GRA | CPS1_TILES },
	{ "ar-3m.5a",      0x080000, 0xac6db17d, BRF_GRA | CPS1_TILES },

	{ "ar_09.12b",     0x010000, 0xf3dd1367, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ar_18.11c",     0x020000, 0x584b43a9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Unsquad)
STD_ROM_FN(Unsquad)

static struct BurnRomInfo Area88RomDesc[] = {
	{ "ar_36.12f",     0x020000, 0x65030392, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_42.12h",     0x020000, 0xc48170de, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_37.13f",     0x020000, 0x33e9694b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_43.13h",     0x020000, 0x7cc8fb9e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_34.10f",     0x020000, 0xf6e80386, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_40.10h",     0x020000, 0xbe36c145, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_35.11f",     0x020000, 0x86d98ff3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "ar_41.11h",     0x020000, 0x758893d3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "ar_09.4b",      0x020000, 0xdb9376f8, BRF_GRA | CPS1_TILES },
	{ "ar_01.4a",      0x020000, 0x392151b4, BRF_GRA | CPS1_TILES },
	{ "ar_13.9b",      0x020000, 0x81436481, BRF_GRA | CPS1_TILES },
	{ "ar_05.9a",      0x020000, 0xe246ed9f, BRF_GRA | CPS1_TILES },
	{ "ar_24.5e",      0x020000, 0x9cd6e2a3, BRF_GRA | CPS1_TILES },
	{ "ar_17.5c",      0x020000, 0x0b8e0df4, BRF_GRA | CPS1_TILES },
	{ "ar_38.8h",      0x020000, 0x8b9e75b9, BRF_GRA | CPS1_TILES },
	{ "ar_32.8f",      0x020000, 0xdb6acdcf, BRF_GRA | CPS1_TILES },
	{ "ar_10.5b",      0x020000, 0x4219b622, BRF_GRA | CPS1_TILES },
	{ "ar_02.5a",      0x020000, 0xbac5dec5, BRF_GRA | CPS1_TILES },
	{ "ar_14.10b",     0x020000, 0xe6bae179, BRF_GRA | CPS1_TILES },
	{ "ar_06.10a",     0x020000, 0xc8f04223, BRF_GRA | CPS1_TILES },
	{ "ar_25.7e",      0x020000, 0x15ccf981, BRF_GRA | CPS1_TILES },
	{ "ar_18.7c",      0x020000, 0x9336db6a, BRF_GRA | CPS1_TILES },
	{ "ar_39.9h",      0x020000, 0x9b8e1363, BRF_GRA | CPS1_TILES },
	{ "ar_33.9f",      0x020000, 0x3968f4b5, BRF_GRA | CPS1_TILES },

	{ "ar_23.13c",     0x010000, 0xf3dd1367, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ar_30.12e",     0x020000, 0x584b43a9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Area88)
STD_ROM_FN(Area88)

static struct BurnRomInfo VarthRomDesc[] = {
	{ "vae_30b.11f",   0x020000, 0xadb8d391, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_35b.11h",   0x020000, 0x44e5548f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_31b.12f",   0x020000, 0x1749a71c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_36b.12h",   0x020000, 0x5f2e2450, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_28b.9f",    0x020000, 0xe524ca50, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_33b.9h",    0x020000, 0xc0bbf8c9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_29b.10f",   0x020000, 0x6640996a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_34b.10h",   0x020000, 0xfa59be8a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "va-5m.7a",      0x080000, 0xb1fb726e, BRF_GRA | CPS1_TILES },
	{ "va-7m.9a",      0x080000, 0x4c6588cd, BRF_GRA | CPS1_TILES },
	{ "va-1m.3a",      0x080000, 0x0b1ace37, BRF_GRA | CPS1_TILES },
	{ "va-3m.5a",      0x080000, 0x44dfe706, BRF_GRA | CPS1_TILES },

	{ "va_09.12b",     0x010000, 0x7a99446e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "va_18.11c",     0x020000, 0xde30510e, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "va_19.12c",     0x020000, 0x0610a4ac, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Varth)
STD_ROM_FN(Varth)

static struct BurnRomInfo Varthr1RomDesc[] = {
	{ "vae_30a.11f",   0x020000, 0x7fcd0091, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_35a.11h",   0x020000, 0x35cf9509, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_31a.12f",   0x020000, 0x15e5ee81, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_36a.12h",   0x020000, 0x153a201e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_28a.9f",    0x020000, 0x7a0e0d25, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_33a.9h",    0x020000, 0xf2365922, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_29a.10f",   0x020000, 0x5e2cd2c3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vae_34a.10h",   0x020000, 0x3d9bdf83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "va-5m.7a",      0x080000, 0xb1fb726e, BRF_GRA | CPS1_TILES },
	{ "va-7m.9a",      0x080000, 0x4c6588cd, BRF_GRA | CPS1_TILES },
	{ "va-1m.3a",      0x080000, 0x0b1ace37, BRF_GRA | CPS1_TILES },
	{ "va-3m.5a",      0x080000, 0x44dfe706, BRF_GRA | CPS1_TILES },

	{ "va_09.12b",     0x010000, 0x7a99446e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "va_18.11c",     0x020000, 0xde30510e, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "va_19.12c",     0x020000, 0x0610a4ac, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Varthr1)
STD_ROM_FN(Varthr1)

static struct BurnRomInfo VarthjRomDesc[] = {
	{ "vaj36b.bin",    0x020000, 0x1d798d6a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj42b.bin",    0x020000, 0x0f720233, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj37b.bin",    0x020000, 0x24414b17, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj43b.bin",    0x020000, 0x34b4b06c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj34b.bin",    0x020000, 0x87c79aed, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj40b.bin",    0x020000, 0x210b4bd0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj35b.bin",    0x020000, 0x6b0da69f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "vaj41b.bin",    0x020000, 0x6542c8a4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "va-5m.bin",     0x080000, 0xb1fb726e, BRF_GRA | CPS1_TILES },
	{ "va-7m.bin",     0x080000, 0x4c6588cd, BRF_GRA | CPS1_TILES },
	{ "va-1m.bin",     0x080000, 0x0b1ace37, BRF_GRA | CPS1_TILES },
	{ "va-3m.bin",     0x080000, 0x44dfe706, BRF_GRA | CPS1_TILES },

	{ "va_09.bin",     0x010000, 0x7a99446e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "va_18.bin",     0x020000, 0xde30510e, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "va_19.bin",     0x020000, 0x0610a4ac, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Varthj)
STD_ROM_FN(Varthj)

static struct BurnRomInfo VarthuRomDesc[] = {
	{ "vau23a.bin",    0x080000, 0xfbe68726, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "vau22a.bin",    0x080000, 0x0ed71bbd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "va-5m.bin",     0x080000, 0xb1fb726e, BRF_GRA | CPS1_TILES },
	{ "va-7m.bin",     0x080000, 0x4c6588cd, BRF_GRA | CPS1_TILES },
	{ "va-1m.bin",     0x080000, 0x0b1ace37, BRF_GRA | CPS1_TILES },
	{ "va-3m.bin",     0x080000, 0x44dfe706, BRF_GRA | CPS1_TILES },

	{ "va_09.bin",     0x010000, 0x7a99446e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "va_18.bin",     0x020000, 0xde30510e, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "va_19.bin",     0x020000, 0x0610a4ac, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Varthu)
STD_ROM_FN(Varthu)

static struct BurnRomInfo WillowRomDesc[] = {
	{ "wlu_30.rom",    0x020000, 0xd604dbb1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "willow-u.35",   0x020000, 0x7a791e77, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wlu_31.rom",    0x020000, 0x0eb48a83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wlu_36.rom",    0x020000, 0x36100209, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wl_32.rom",     0x080000, 0xdfd9f643, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "wl_gfx5.rom",   0x080000, 0xafa74b73, BRF_GRA | CPS1_TILES },
	{ "wl_gfx7.rom",   0x080000, 0x12a0dc0b, BRF_GRA | CPS1_TILES },
	{ "wl_gfx1.rom",   0x080000, 0xc6f2abce, BRF_GRA | CPS1_TILES },
	{ "wl_gfx3.rom",   0x080000, 0x4aa4c6d3, BRF_GRA | CPS1_TILES },
	{ "wl_24.rom",     0x020000, 0x6f0adee5, BRF_GRA | CPS1_TILES },
	{ "wl_14.rom",     0x020000, 0x9cf3027d, BRF_GRA | CPS1_TILES },
	{ "wl_26.rom",     0x020000, 0xf09c8ecf, BRF_GRA | CPS1_TILES },
	{ "wl_16.rom",     0x020000, 0xe35407aa, BRF_GRA | CPS1_TILES },
	{ "wl_20.rom",     0x020000, 0x84992350, BRF_GRA | CPS1_TILES },
	{ "wl_10.rom",     0x020000, 0xb87b5a36, BRF_GRA | CPS1_TILES },
	{ "wl_22.rom",     0x020000, 0xfd3f89f0, BRF_GRA | CPS1_TILES },
	{ "wl_12.rom",     0x020000, 0x7da49d69, BRF_GRA | CPS1_TILES },

	{ "wl_09.rom",     0x010000, 0xf6b3d060, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "wl_18.rom",     0x020000, 0xbde23d4d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "wl_19.rom",     0x020000, 0x683898f5, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Willow)
STD_ROM_FN(Willow)

static struct BurnRomInfo WillowjeRomDesc[] = {
	{ "wlu_30.rom",    0x020000, 0xd604dbb1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wlu_35.rom",    0x020000, 0xdaee72fe, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wlu_31.rom",    0x020000, 0x0eb48a83, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wlu_36.rom",    0x020000, 0x36100209, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wl_32.rom",     0x080000, 0xdfd9f643, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "wl_gfx5.rom",   0x080000, 0xafa74b73, BRF_GRA | CPS1_TILES },
	{ "wl_gfx7.rom",   0x080000, 0x12a0dc0b, BRF_GRA | CPS1_TILES },
	{ "wl_gfx1.rom",   0x080000, 0xc6f2abce, BRF_GRA | CPS1_TILES },
	{ "wl_gfx3.rom",   0x080000, 0x4aa4c6d3, BRF_GRA | CPS1_TILES },
	{ "wl_24.rom",     0x020000, 0x6f0adee5, BRF_GRA | CPS1_TILES },
	{ "wl_14.rom",     0x020000, 0x9cf3027d, BRF_GRA | CPS1_TILES },
	{ "wl_26.rom",     0x020000, 0xf09c8ecf, BRF_GRA | CPS1_TILES },
	{ "wl_16.rom",     0x020000, 0xe35407aa, BRF_GRA | CPS1_TILES },
	{ "wl_20.rom",     0x020000, 0x84992350, BRF_GRA | CPS1_TILES },
	{ "wl_10.rom",     0x020000, 0xb87b5a36, BRF_GRA | CPS1_TILES },
	{ "wl_22.rom",     0x020000, 0xfd3f89f0, BRF_GRA | CPS1_TILES },
	{ "wl_12.rom",     0x020000, 0x7da49d69, BRF_GRA | CPS1_TILES },

	{ "wl_09.rom",     0x010000, 0xf6b3d060, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "wl_18.rom",     0x020000, 0xbde23d4d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "wl_19.rom",     0x020000, 0x683898f5, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Willowje)
STD_ROM_FN(Willowje)

static struct BurnRomInfo WillowjRomDesc[] = {
	{ "wl36.bin",      0x020000, 0x2b0d7cbc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wl42.bin",      0x020000, 0x1ac39615, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wl37.bin",      0x020000, 0x30a717fa, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wl43.bin",      0x020000, 0xd0dddc9e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "wl_32.rom",     0x080000, 0xdfd9f643, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "wl_gfx5.rom",   0x080000, 0xafa74b73, BRF_GRA | CPS1_TILES },
	{ "wl_gfx7.rom",   0x080000, 0x12a0dc0b, BRF_GRA | CPS1_TILES },
	{ "wl_gfx1.rom",   0x080000, 0xc6f2abce, BRF_GRA | CPS1_TILES },
	{ "wl_gfx3.rom",   0x080000, 0x4aa4c6d3, BRF_GRA | CPS1_TILES },
	{ "wl_24.rom",     0x020000, 0x6f0adee5, BRF_GRA | CPS1_TILES },
	{ "wl_14.rom",     0x020000, 0x9cf3027d, BRF_GRA | CPS1_TILES },
	{ "wl_26.rom",     0x020000, 0xf09c8ecf, BRF_GRA | CPS1_TILES },
	{ "wl_16.rom",     0x020000, 0xe35407aa, BRF_GRA | CPS1_TILES },
	{ "wl_20.rom",     0x020000, 0x84992350, BRF_GRA | CPS1_TILES },
	{ "wl_10.rom",     0x020000, 0xb87b5a36, BRF_GRA | CPS1_TILES },
	{ "wl_22.rom",     0x020000, 0xfd3f89f0, BRF_GRA | CPS1_TILES },
	{ "wl_12.rom",     0x020000, 0x7da49d69, BRF_GRA | CPS1_TILES },

	{ "wl23.bin",      0x010000, 0xf6b3d060, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "wl30.bin",      0x020000, 0xbde23d4d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "wl32.bin",      0x020000, 0x683898f5, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Willowj)
STD_ROM_FN(Willowj)

static struct BurnRomInfo WofRomDesc[] = {
	{ "tk2e_23b.rom",  0x080000, 0x11fb2ed1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2e_22b.rom",  0x080000, 0x479b3f24, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wof)
STD_ROM_FN(Wof)

static struct BurnRomInfo WofaRomDesc[] = {
	{ "tk2a_23b.rom",  0x080000, 0x2e024628, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2a_22b.rom",  0x080000, 0x900ad4cd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofa)
STD_ROM_FN(Wofa)

static struct BurnRomInfo WofuRomDesc[] = {
	{ "tk2u.23c",      0x080000, 0x29b89c12, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2u.22c",      0x080000, 0xf5af4774, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofu)
STD_ROM_FN(Wofu)

static struct BurnRomInfo WofjRomDesc[] = {
	{ "tk2j23c.bin",   0x080000, 0x9b215a68, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2j22c.bin",   0x080000, 0xb74b09ac, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk205.bin",     0x080000, 0xe4a44d53, BRF_GRA | CPS1_TILES },
	{ "tk206.bin",     0x080000, 0x58066ba8, BRF_GRA | CPS1_TILES },
	{ "tk207.bin",     0x080000, 0xd706568e, BRF_GRA | CPS1_TILES },
	{ "tk208.bin",     0x080000, 0xd4a19a02, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofj)
STD_ROM_FN(Wofj)

static struct BurnRomInfo WofhfhRomDesc[] = {
	{ "23",            0x080000, 0x6ae4b312, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "22",            0x080000, 0x94e8d01a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "1",             0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "2",             0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "3",             0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "4",             0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "5",             0x080000, 0x34949d7b, BRF_GRA | CPS1_TILES },
	{ "6",             0x080000, 0xdfa70971, BRF_GRA | CPS1_TILES },
	{ "7",             0x080000, 0x073686a6, BRF_GRA | CPS1_TILES },
	{ "8",             0x080000, 0x5300f8db, BRF_GRA | CPS1_TILES },

	{ "9",             0x020000, 0x86fe8a97, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "18",            0x020000, 0xc04be720, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "19",            0x020000, 0xfbb8d8c1, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Wofhfh)
STD_ROM_FN(Wofhfh)

static struct BurnRomInfo WofbRomDesc[] = {
	{ "3-f2ab.040",    0x080000, 0x61fd0a01, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "1-9207.040",    0x080000, 0x7f59e24c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "4-d4d2.010",    0x020000, 0xfe5eee87, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "2-6c41.010",    0x020000, 0x739379be, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "5-caf3.040",    0x080000, 0xc8dcaa95, BRF_GRA | CPS1_TILES },
	{ "6-034f.040",    0x080000, 0x1ab0000c, BRF_GRA | CPS1_TILES },
	{ "7-b0fa.040",    0x080000, 0x8425ff6b, BRF_GRA | CPS1_TILES },
	{ "8-a6b7.040",    0x080000, 0x24ce197b, BRF_GRA | CPS1_TILES },
	{ "9-8a2c.040",    0x080000, 0x9d20ef9b, BRF_GRA | CPS1_TILES },
	{ "10-7d24.040",   0x080000, 0x90c93dd2, BRF_GRA | CPS1_TILES },
	{ "11-4171.040",   0x080000, 0x219fd7e2, BRF_GRA | CPS1_TILES },
	{ "12-f56b.040",   0x080000, 0xefc17c9a, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofb)
STD_ROM_FN(Wofb)

static struct BurnRomInfo Wof3jsRomDesc[] = {
	{ "3js_23.rom",    0x080000, 0x1ebb76da, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "3js_22.rom",    0x080000, 0xf41d6153, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP }, // First and second halves identical
	{ "3js_24.rom",    0x020000, 0x06ead409, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3js_28.rom",    0x020000, 0x8ba934e6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "3js_gfx5.rom",  0x080000, 0x94b58f82, BRF_GRA | CPS1_TILES },
	{ "3js_gfx7.rom",  0x080000, 0xdf4fb386, BRF_GRA | CPS1_TILES },
	{ "3js_gfx6.rom",  0x080000, 0xc22c5bd8, BRF_GRA | CPS1_TILES },
	{ "3js_gfx8.rom",  0x080000, 0xf9cfd08b, BRF_GRA | CPS1_TILES },

	{ "3js_09.rom",    0x010000, 0x21ce044c, BRF_PRG | CPS1_Z80_PROGRAM },
	
	{ "18",            0x020000, 0xc04be720, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "19",            0x020000, 0xfbb8d8c1, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Wof3js)
STD_ROM_FN(Wof3js)

static struct BurnRomInfo Wof3sjRomDesc[] = {
	{ "k6b.040",       0x080000, 0x7b365108, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "k6a.040",       0x080000, 0x10488a51, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "k6a.160",       0x200000, 0xa121180d, BRF_GRA | CPS1_TILES },
	{ "k6b.160",       0x200000, 0xa4db96c4, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wof3sj)
STD_ROM_FN(Wof3sj)

static struct BurnRomInfo Wof3sjaRomDesc[] = {
	{ "3js.800",       0x100000, 0x812f9200, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "k6a.160",       0x200000, 0xa121180d, BRF_GRA | CPS1_TILES },
	{ "k6b.160",       0x200000, 0xa4db96c4, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wof3sja)
STD_ROM_FN(Wof3sja)

static struct BurnRomInfo WofahaRomDesc[] = {
	{ "htk2ah_23b.rom",0x080000, 0x6e3ff382, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2ah_22b.rom", 0x080000, 0x48fd83c6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofaha)
STD_ROM_FN(Wofaha)

static struct BurnRomInfo WofhRomDesc[] = {
	{ "sgyx.800",      0x100000, 0x3703a650, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "sgyx-1.160",    0x200000, 0xa60be9f6, BRF_GRA | CPS1_TILES },
	{ "sgyx-2.160",    0x200000, 0x6ad9d048, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofh)
STD_ROM_FN(Wofh)

static struct BurnRomInfo WofhaRomDesc[] = {
	{ "fg-c.040",      0x080000, 0xd046fc86, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "fg-a.040",      0x080000, 0xf176ee8f, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "sgyx-1.160",    0x200000, 0xa60be9f6, BRF_GRA | CPS1_TILES },
	{ "sgyx-2.160",    0x200000, 0x6ad9d048, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofha)
STD_ROM_FN(Wofha)

static struct BurnRomInfo WofsjRomDesc[] = {
	{ "c-c47b.040",    0x080000, 0xb1809761, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "a-2402.040",    0x080000, 0x4fab4232, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofsj)
STD_ROM_FN(Wofsj)

static struct BurnRomInfo WofsjaRomDesc[] = {
	{ "one.800",       0x100000, 0x0507584d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofsja)
STD_ROM_FN(Wofsja)

static struct BurnRomInfo WofsjbRomDesc[] = {
	{ "c-d140.040",    0x080000, 0xe6d933a6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "a-0050.040",    0x080000, 0x403eaead, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofsjb)
STD_ROM_FN(Wofsjb)

static struct BurnRomInfo Cps1frogRomDesc[] = {
	{ "frog30-36.bin", 0x020000, 0x8eb8ddbe, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "frog35-42.bin", 0x020000, 0xacb5a988, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "frog31-37.bin", 0x020000, 0x154803cc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "frog36-43.bin", 0x020000, 0x154803cc, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "frog32-32m.bin",0x080000, 0x75660aac, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "frog05-05m.bin",0x080000, 0x7bbc4b54, BRF_GRA | CPS1_TILES },
	{ "frog07-07m.bin",0x080000, 0x0beadd80, BRF_GRA | CPS1_TILES },
	{ "frog01-01m.bin",0x080000, 0xf84d02bb, BRF_GRA | CPS1_TILES },
	{ "frog03-03m.bin",0x080000, 0xd784a5b0, BRF_GRA | CPS1_TILES },

	{ "frog09-09.bin", 0x010000, 0x3ac7fb5c, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "frog18-18.bin", 0x020000, 0x65d1ef07, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "frog19-19.bin", 0x020000, 0x7ee8cdcd, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cps1frog)
STD_ROM_FN(Cps1frog)

// Game config

typedef void (*KabukiDecode)();
static KabukiDecode KabukiDecodeFunction;

static int GameHasStars = 0;

struct GameConfig {
	const char *DriverName;
	int CpsBId;
	int CpsMapperId;
	int CpsStars;
	KabukiDecode DecodeFunction;
};

static const struct GameConfig ConfigTable[] =
{
	{ "1941"    , CPS_B_05    , mapper_YI24B , 0, NULL                },
	{ "1941j"   , CPS_B_05    , mapper_YI24B , 0, NULL                },
	{ "3wonders", CPS_B_21_BT1, mapper_RT24B , 0, NULL                },
	{ "3wonderu", CPS_B_21_BT1, mapper_RT24B , 0, NULL                },
	{ "wonder3" , CPS_B_21_BT1, mapper_RT22B , 0, NULL                },
	{ "captcomm", CPS_B_21_BT3, mapper_CC63B , 0, NULL                },
	{ "captcomu", CPS_B_21_BT3, mapper_CC63B , 0, NULL                },
	{ "captcomj", CPS_B_21_BT3, mapper_CC63B , 0, NULL                },
	{ "captcomb", CPS_B_21_BT3, mapper_CC63B , 0, NULL                },
	{ "cawing"  , CPS_B_16    , mapper_CA24B , 0, NULL                },
	{ "cawingr1", CPS_B_16    , mapper_CA24B , 0, NULL                },
	{ "cawingu" , CPS_B_16    , mapper_CA24B , 0, NULL                },
	{ "cawingj" , CPS_B_16    , mapper_CA22B , 0, NULL                },
	{ "cawingb" , CPS_B_16    , mapper_CA24B , 0, NULL                },
	{ "cworld2j", CPS_B_21_BT6, mapper_Q522B , 0, NULL                },
	{ "dino"    , CPS_B_21_QS2, mapper_CD63B , 0, dino_decode         },
	{ "dinou"   , CPS_B_21_QS2, mapper_CD63B , 0, dino_decode         },
	{ "dinoj"   , CPS_B_21_QS2, mapper_CD63B , 0, dino_decode         },
	{ "dinopic" , CPS_B_21_QS2, mapper_CD63B , 0, NULL                },
	{ "dinopic2", CPS_B_21_QS2, mapper_CD63B , 0, NULL                },
	{ "dinoh"   , CPS_B_21_DEF, mapper_CD63B , 0, dino_decode         },
	{ "dinoha"  , CPS_B_21_DEF, mapper_CD63B , 0, dino_decode         },
	{ "dinohb"  , CPS_B_21_QS2, mapper_CD63B , 0, dino_decode         },
	{ "dynwar"  , CPS_B_02    , mapper_TK22B , 0, NULL                },
	{ "dynwaru" , CPS_B_02    , mapper_TK22B , 0, NULL                },
	{ "dynwarj" , CPS_B_02    , mapper_TK22B , 0, NULL                },
	{ "ffight"  , CPS_B_04    , mapper_S224B , 0, NULL                },
	{ "ffightu" , CPS_B_04    , mapper_S224B , 0, NULL                },
	{ "ffightua", CPS_B_01    , mapper_S224B , 0, NULL                },
	{ "ffightub", CPS_B_05    , mapper_S224B , 0, NULL                },
	{ "ffightj" , CPS_B_04    , mapper_S224B , 0, NULL                },
	{ "ffightj1", CPS_B_02    , mapper_S224B , 0, NULL                },
	{ "ffightj2", CPS_B_01    , mapper_S224B , 0, NULL                },
	{ "forgottn", CPS_B_01    , mapper_LWCHR , 1, NULL                },
	{ "forgott1", CPS_B_01    , mapper_LWCHR , 1, NULL                },
	{ "lostwrld", CPS_B_01    , mapper_LWCHR , 1, NULL                },
	{ "ghouls"  , CPS_B_01    , mapper_DM620 , 0, NULL                },
	{ "ghoulsu" , CPS_B_01    , mapper_DM620 , 0, NULL                },
	{ "daimakai", CPS_B_01    , mapper_DM22A , 0, NULL                },
	{ "daimakb",  CPS_B_01    , mapper_DM620 , 0, NULL                },
	{ "knights" , CPS_B_21_BT4, mapper_KR63B , 0, NULL                },
	{ "knightsu", CPS_B_21_BT4, mapper_KR63B , 0, NULL                },
	{ "knightsj", CPS_B_21_BT4, mapper_KR63B , 0, NULL                },
	{ "knightsb", CPS_B_21_BT4, mapper_KR63B , 0, NULL                },
	{ "knightsh", CPS_B_21_DEF, mapper_KR63B , 0, NULL                },
	{ "knghtsha", CPS_B_21_DEF, mapper_KR63B , 0, NULL                },
	{ "kod"     , CPS_B_21_BT2, mapper_KD29B , 0, NULL                },
	{ "kodu"    , CPS_B_21_BT2, mapper_KD29B , 0, NULL                },
	{ "kodj"    , CPS_B_21_BT2, mapper_KD29B , 0, NULL                },
	{ "kodb"    , CPS_B_21_BT2, mapper_KD29B , 0, NULL                },
	{ "kodh"    , CPS_B_21_DEF, mapper_KD29B , 0, NULL                },
	{ "megaman" , CPS_B_21_DEF, mapper_RCM63B, 0, NULL                },
	{ "rockmanj", CPS_B_21_DEF, mapper_RCM63B, 0, NULL                },
	{ "mercs"   , CPS_B_12    , mapper_O224B , 0, NULL                },
	{ "mercsu"  , CPS_B_12    , mapper_O224B , 0, NULL                },
	{ "mercsua" , CPS_B_12    , mapper_O224B , 0, NULL                },
	{ "mercsj"  , CPS_B_12    , mapper_O224B , 0, NULL                },
	{ "msword"  , CPS_B_13    , mapper_MS24B , 0, NULL                },
	{ "mswordr1", CPS_B_13    , mapper_MS24B , 0, NULL                },
	{ "mswordu" , CPS_B_13    , mapper_MS24B , 0, NULL                },
	{ "mswordj" , CPS_B_13    , mapper_MS24B , 0, NULL                },
	{ "mtwins"  , CPS_B_14    , mapper_CK24B , 0, NULL                },
	{ "chikij"  , CPS_B_14    , mapper_CK24B , 0, NULL                },
	{ "nemo"    , CPS_B_15    , mapper_NM24B , 0, NULL                },
	{ "nemoj"   , CPS_B_15    , mapper_NM24B , 0, NULL                },
	{ "pang3"   , CPS_B_21_DEF, mapper_pang3 , 0, NULL                },
	{ "pang3j"  , CPS_B_21_DEF, mapper_pang3 , 0, NULL                },
	{ "pnickj"  , CPS_B_21_DEF, mapper_pnickj, 0, NULL                },
	{ "punisher", CPS_B_21_QS3, mapper_PS63B , 0, punisher_decode     },
	{ "punishru", CPS_B_21_QS3, mapper_PS63B , 0, punisher_decode     },
	{ "punishrj", CPS_B_21_QS3, mapper_PS63B , 0, punisher_decode     },
	{ "punipic" , CPS_B_21_QS3, mapper_PS63B , 0, NULL                },
	{ "punipic2", CPS_B_21_QS3, mapper_PS63B , 0, NULL                },
	{ "punipic3", CPS_B_21_QS3, mapper_PS63B , 0, NULL                },
	{ "qad"     , CPS_B_21_BT7, mapper_QD22B , 0, NULL                },
	{ "qadj"    , CPS_B_21_DEF, mapper_qadj  , 0, NULL                },
	{ "qtono2"  , CPS_B_21_DEF, mapper_qtono2, 0, NULL                },
	{ "sf2"     , CPS_B_11    , mapper_STF29 , 0, NULL                },
	{ "sf2eb"   , CPS_B_17    , mapper_STF29 , 0, NULL                },
	{ "sf2ua"   , CPS_B_17    , mapper_STF29 , 0, NULL                },
	{ "sf2ub"   , CPS_B_17    , mapper_STF29 , 0, NULL                },
	{ "sf2ud"   , CPS_B_05    , mapper_STF29 , 0, NULL                },
	{ "sf2ue"   , CPS_B_18    , mapper_STF29 , 0, NULL                },
	{ "sf2uf"   , CPS_B_15    , mapper_STF29 , 0, NULL                },
	{ "sf2ui"   , CPS_B_14    , mapper_STF29 , 0, NULL                },
	{ "sf2uk"   , CPS_B_17    , mapper_STF29 , 0, NULL                },
	{ "sf2j"    , CPS_B_13    , mapper_STF29 , 0, NULL                },
	{ "sf2ja"   , CPS_B_17    , mapper_STF29 , 0, NULL                },
	{ "sf2jc"   , CPS_B_12    , mapper_STF29 , 0, NULL                },
	{ "sf2b"    , CPS_B_17    , mapper_STF29 , 0, NULL                },
	{ "sf2ce"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2ceua" , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2ceub" , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2ceuc" , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2cej"  , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2rb"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2rb2"  , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2red"  , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2v004" , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2accp2", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2m1"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2m2"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2m3"   , HACK_B_2    , mapper_S9263B, 0, NULL                },
	{ "sf2m4"   , HACK_B_1    , mapper_S9263B, 0, NULL                },
	{ "sf2m5"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2m6"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2m7"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2m8"   , HACK_B_2    , mapper_S9263B, 0, NULL                },
	{ "sf2m13"  , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2tlona", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2tlonb", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2cebr" , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2ceh"  , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2th"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2yyc"  , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2koryu", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2hf"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2t"    , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2tj"   , CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "slammast", CPS_B_21_QS4, mapper_MB63B , 0, slammast_decode     },
	{ "slammasa", CPS_B_21_QS4, mapper_MB63B , 0, slammast_decode     },
	{ "slammasu", CPS_B_21_QS4, mapper_MB63B , 0, slammast_decode     },
	{ "mbomberj", CPS_B_21_QS4, mapper_MB63B , 0, slammast_decode     },
	{ "mbombrd",  CPS_B_21_QS5, mapper_MB63B , 0, slammast_decode     },
	{ "mbombrdj", CPS_B_21_QS5, mapper_MB63B , 0, slammast_decode     },
	{ "strider" , CPS_B_01    , mapper_ST24M1, 1, NULL                },
	{ "stridrua", CPS_B_01    , mapper_ST24M1, 1, NULL                },
	{ "striderj", CPS_B_01    , mapper_ST24M1, 1, NULL                },
	{ "stridrja", CPS_B_01    , mapper_ST22B , 1, NULL                },
	{ "unsquad" , CPS_B_11    , mapper_AR24B , 0, NULL                },
	{ "area88"  , CPS_B_11    , mapper_AR22B , 0, NULL                },
	{ "varth"   , CPS_B_04    , mapper_VA63B , 0, NULL                },
	{ "varthr1" , CPS_B_04    , mapper_VA63B , 0, NULL                },
	{ "varthu"  , CPS_B_04    , mapper_VA63B , 0, NULL                },
	{ "varthj"  , CPS_B_21_BT5, mapper_VA63B , 0, NULL                },
	{ "willow"  , CPS_B_03    , mapper_WL24B , 0, NULL                },
	{ "willowj" , CPS_B_03    , mapper_WL24B , 0, NULL                },
	{ "willowje", CPS_B_03    , mapper_WL24B , 0, NULL                },
	{ "wof"     , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "wofa"    , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "wofu"    , CPS_B_21_QS1, mapper_TK263B, 0, wof_decode          },
	{ "wofj"    , CPS_B_21_QS1, mapper_TK263B, 0, wof_decode          },
	{ "wofhfh"  , CPS_B_21_DEF, mapper_TK263B, 0, NULL                },
	{ "wofb"    , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "wof3js"  , CPS_B_21_DEF, mapper_TK263B, 0, NULL                },
	{ "wof3sj"  , HACK_B_3    , mapper_TK263B, 0, wof_decode          },
	{ "wof3sja" , HACK_B_3    , mapper_TK263B, 0, wof_decode          },
	{ "wofaha"  , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "wofh"    , HACK_B_3    , mapper_TK263B, 0, wof_decode          },
	{ "wofha"   , HACK_B_3    , mapper_TK263B, 0, wof_decode          },
	{ "wofjh"   , CPS_B_21_QS1, mapper_TK263B, 0, wof_decode          },
	{ "wofsj"   , HACK_B_3    , mapper_TK263B, 0, wof_decode          },
	{ "wofsja"  , HACK_B_3    , mapper_TK263B, 0, wof_decode          },
	{ "wofsjb"  , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "sfzch"   , CPS_B_21_DEF, mapper_sfzch , 0, NULL                },
	{ "cps1frog", CPS_B_04    , mapper_frog  , 0, NULL                },
	
#if defined INCLUDE_HACKS
	// Not included games
	{ "cps1demo", CPS_B_04    , mapper_sfzch , 0, NULL                },	
	{ "dinoeh"  , CPS_B_21_QS2, mapper_CD63B , 0, dino_decode         },
	{ "pnicku"  , CPS_B_21_DEF, mapper_pnickj, 0, NULL                },
	{ "slammash", CPS_B_21_QS4, mapper_MB63B , 0, slammast_decode     },
	{ "mbombdje", CPS_B_21_QS5, mapper_MB63B , 0, slammast_decode     },
	{ "mbombrda", CPS_B_21_QS5, mapper_MB63B , 0, slammast_decode     },
	{ "mbombrdh", CPS_B_21_QS5, mapper_MB63B , 0, slammast_decode     },
	{ "mbombrdu", CPS_B_21_QS5, mapper_MB63B , 0, slammast_decode     },
	{ "wofah"   , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "woffr"   , CPS_B_21_DEF, mapper_TK263B, 0, wof_decode          },
	{ "sfach"   , CPS_B_21_DEF, mapper_sfzch , 0, NULL                },
	{ "sfzbch"  , CPS_B_21_DEF, mapper_sfzch , 0, NULL                },
#endif

#if defined INCLUDE_DUPLICATES
	{ "dinob"   , CPS_B_21_QS2, mapper_CD63B , 0, NULL                },
	{ "kl2d"    , CPS_B_21_DEF, mapper_CD63B , 0, dino_decode         },
	{ "sf2tlonc", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2turyu", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
	{ "sf2tury2", CPS_B_21_DEF, mapper_S9263B, 0, NULL                },
#endif

	{ 0         , 0           , 0            , 0, 0                   }
};

static void SetGameConfig()
{
	const char *GameName = BurnDrvGetTextA(DRV_NAME);
	const struct GameConfig *k = &ConfigTable[0];

	while (k->DriverName) {
		if (strcmp(k->DriverName, GameName) == 0) {
			break;
		}
		++k;
	}

	if (k->DriverName) {
		GameHasStars = k->CpsStars;
		SetCpsBId(k->CpsBId, GameHasStars);
		SetGfxMapper(k->CpsMapperId);
		KabukiDecodeFunction = k->DecodeFunction;
	} else {
#if 1 && defined FBA_DEBUG
		bprintf(PRINT_IMPORTANT, _T("Missing Config Data\n"));
#endif
	}
}

// Driver functions

static unsigned int nCps68KByteswapRomNum = 0;
static unsigned int nCps68KNoByteswapRomNum = 0;
static unsigned int nCpsZ80RomNum = 0;
static unsigned int nCpsTilesRomNum = 0;
static unsigned int nCpsOkim6295RomNum = 0;
static unsigned int nCpsQsoundRomNum = 0;
static unsigned int nCpsPicRomNum = 0;

static int Cps1LoadRoms(int bLoad)
{
	struct BurnRomInfo ri;
	ri.nType = 0;
	ri.nLen = 0;
	int nOffset = -1;
	unsigned int i = 0;
	int nRet = 0;
	
	if (!bLoad) {
		do {
			ri.nLen = 0;
			ri.nType = 0;
			BurnDrvGetRomInfo(&ri, ++nOffset);
			if ((ri.nType & 0xff) == CPS1_68K_PROGRAM_BYTESWAP) {
				nCpsRomLen += ri.nLen;
				nCps68KByteswapRomNum++;
			}
			if ((ri.nType & 0xff) == CPS1_68K_PROGRAM_NO_BYTESWAP) {
				nCpsRomLen += ri.nLen;
				nCps68KNoByteswapRomNum++;
			}
			if ((ri.nType & 0xff) == CPS1_Z80_PROGRAM) {
				nCpsZRomLen += ri.nLen;
				nCpsZ80RomNum++;
			}
			if ((ri.nType & 0xff) == CPS1_TILES) {
				nCpsGfxLen += ri.nLen;
				nCpsTilesRomNum++;
			}
			if ((ri.nType & 0xff) == CPS1_PIC) {
				nCpsPicRomNum++;
			}
			if ((ri.nType & 0xff) == CPS1_OKIM6295_SAMPLES) {
				nCpsAdLen += ri.nLen;
				nCpsOkim6295RomNum++;
			}
			if ((ri.nType & 0xff) == CPS1_QSOUND_SAMPLES) {
				Cps1Qs = 1;
				nCpsQSamLen += ri.nLen;
				nCpsQsoundRomNum++;
			}
			i++;
			
		} while (ri.nLen);
		
		if (Cps1Qs) nCpsZRomLen *= 2;
		if (GameHasStars) nCpsGfxLen += 0x2000;
		if (PangEEP) nCpsGfxLen *= 2;
		if (nCpsPicRomNum) Cps1Pic = 1;
		
#if 1 && defined FBA_DEBUG
		if (nCpsRomLen) bprintf(PRINT_IMPORTANT, _T("68K Rom Length %06X, (%i roms byteswapped, %i roms not byteswapped)\n"), nCpsRomLen, nCps68KByteswapRomNum, nCps68KNoByteswapRomNum);
		if (nCpsZRomLen) bprintf(PRINT_IMPORTANT, _T("Z80 Rom Length %06X, (%i roms)\n"), nCpsZRomLen, nCpsZ80RomNum);
		if (nCpsGfxLen) bprintf(PRINT_IMPORTANT, _T("Tile Rom Length %08X, (%i roms)\n"), nCpsGfxLen, nCpsTilesRomNum);
		if (nCpsAdLen) bprintf(PRINT_IMPORTANT, _T("OKIM6295 Rom Length %08X, (%i roms)\n"), nCpsAdLen, nCpsOkim6295RomNum);
		if (nCpsQSamLen) bprintf(PRINT_IMPORTANT, _T("QSound Rom Length %08X, (%i roms)\n"), nCpsQSamLen, nCpsQsoundRomNum);
#endif
	}

	if (bLoad) {
		int Offset = 0;
		
		i = 0;
		while (i < nCps68KByteswapRomNum + nCps68KNoByteswapRomNum) {
			BurnDrvGetRomInfo(&ri, i + 0);
			if ((ri.nType & 0xff) == CPS1_68K_PROGRAM_BYTESWAP) {
				nRet = BurnLoadRom(CpsRom + Offset + 1, i + 0, 2); if (nRet) return 1;
				nRet = BurnLoadRom(CpsRom + Offset + 0, i + 1, 2); if (nRet) return 1;
			
				BurnDrvGetRomInfo(&ri, i + 0);
				Offset += ri.nLen;
				BurnDrvGetRomInfo(&ri, i + 1);
				Offset += ri.nLen;
				
				i += 2;
			}
			
			if ((ri.nType & 0xff) == CPS1_68K_PROGRAM_NO_BYTESWAP) {
				nRet = BurnLoadRom(CpsRom + Offset, i, 1); if (nRet) return 1;
			
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
				
				i++;
			}
		}
		
		// Graphics
		if (nCpsGfxLen) {
			Offset = 0;
			i = nCps68KByteswapRomNum + nCps68KNoByteswapRomNum;
			while (i < nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				if (ri.nLen < 0x80000) {
					CpsLoadTilesByte(CpsGfx + Offset, i);
			
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 2);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 3);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 4);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 5);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 6);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 7);
					Offset += ri.nLen;
					
					i += 8;
				} else {
					if (PangEEP) {
						CpsLoadTilesPang(CpsGfx + Offset, i);
			
						BurnDrvGetRomInfo(&ri, i + 0);
						Offset += ri.nLen;
						BurnDrvGetRomInfo(&ri, i + 1);
						Offset += ri.nLen;
						BurnDrvGetRomInfo(&ri, i + 2);
						Offset += ri.nLen;
						BurnDrvGetRomInfo(&ri, i + 3);
						Offset += ri.nLen;
					
						i += 4;
					} else {
						if (nCpsTilesRomNum < 4) {
							// Handle this seperately
							i += nCpsTilesRomNum;
						} else {
							CpsLoadTiles(CpsGfx + Offset, i);
						
							BurnDrvGetRomInfo(&ri, i + 0);
							Offset += ri.nLen;
							BurnDrvGetRomInfo(&ri, i + 1);
							Offset += ri.nLen;
							BurnDrvGetRomInfo(&ri, i + 2);
							Offset += ri.nLen;
							BurnDrvGetRomInfo(&ri, i + 3);
							Offset += ri.nLen;
					
							i += 4;
						}
					}
				}
			}			
		}
		
		// Z80 Program
		if (nCpsZRomLen) {
			Offset = 0;
			for (i = nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum; i < nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum + nCpsZ80RomNum; i++) {
				BurnLoadRom(CpsZRom + Offset, i, 1);
				
				if (Cps1Qs) BurnLoadRom(CpsEncZRom + Offset, i, 1);
			
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		// Pic (skip for now)
		if (nCpsPicRomNum) {
			BurnDrvGetRomInfo(&ri, i);
		}
		
		// OKIM6295 Samples
		if (nCpsAdLen) {
			Offset = 0;
			for (i = nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum + nCpsZ80RomNum; i < nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum + nCpsZ80RomNum + nCpsOkim6295RomNum; i++) {
				BurnLoadRom(CpsAd + Offset, i, 1);
			
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		// QSound Samples
		if (nCpsQSamLen) {
			Offset = 0;
			for (i = nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum + nCpsZ80RomNum; i < nCps68KByteswapRomNum + nCps68KNoByteswapRomNum + nCpsTilesRomNum + nCpsZ80RomNum + nCpsQsoundRomNum; i++) {
				BurnLoadRom((unsigned char*)CpsQSam + Offset, i, 1);
			
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
	}

	return nRet;
}

typedef void (*AmendProgRom)();
static AmendProgRom AmendProgRomCallback;

static int DrvInit()
{
	int nRet = 0;
	
	SetGameConfig();
    
	Cps1LoadRoms(0);

	Cps = 1;
	nRet = CpsInit(); if (nRet != 0) return 1;
	
	Cps1LoadRoms(1);
	
	if (AmendProgRomCallback) AmendProgRomCallback();
	
	SetGameConfig();
	
	if (Cps1Qs) {
		KabukiDecodeFunction();
	}
		
	nRet = CpsRunInit(); if (nRet != 0) return 1;
	
	return 0;
}

static int TwelveMhzInit()
{
	nCPS68KClockspeed = 12000000;
	
	return DrvInit();
}

static int CaptcombInit()
{
	int nRet = 0;
	
	CpsLayer1XOffs = -8;
	CpsLayer2XOffs = -11;
	CpsLayer3XOffs = -12;
	
	nRet = DrvInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesCaptcomb(CpsGfx, 4);
	
	return nRet;
}

unsigned char __fastcall CawingbInputRead(unsigned int a)
{
	unsigned char d = 0xff;
	
	switch (a) {
		case 0x882000: {
			d = (unsigned char)~Inp000;
			return d;
		}
		
		case 0x882001: {
			d = (unsigned char)~Inp001;
			return d;
		}
		
		case 0x882008: {
			d = (unsigned char)~Inp008;
			return d;
		}
		
		case 0x88200a: {
			d = (unsigned char)~Cpi01A;
			return d;
		}
		
		case 0x88200c: {
			d = (unsigned char)~Cpi01C;
			return d;
		}
		
		case 0x88200e: {
			d = (unsigned char)~Cpi01E;
			return d;
		}
	}

	return 0;
}

void __fastcall CawingbInputWrite(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x882006: {
			PsndSyncZ80((long long)SekTotalCycles() * nCpsZ80Cycles / nCpsCycles);
			
			PsndCode = d;
			return;
		}
	}

}

static int CawingbInit()
{
	int nRet = 0;
	
	CpsLayer1XOffs = 0xffc0;
//	CpsLayer2XOffs = 0xffc0;
//	CpsLayer3XOffs = 0xffc0;
	
	nRet = DrvInit();
	
	SekOpen(0);
	SekMapHandler(1, 0x882000, 0x88200f, SM_RAM);
	SekSetReadByteHandler(1, CawingbInputRead);
	SekSetWriteByteHandler(1, CawingbInputWrite);
	SekClose();
	
	return 0;
}

void __fastcall DinopicScrollWrite(unsigned int a, unsigned short d)
{
	if (a == 0x980000) {
		CpsLayer1YOffs = d;
		return;
	}
	
	if (a == 0x980002) {
		CpsLayer1XOffs = d - 0x40;
		return;
	}
	
	if (a == 0x980004) {
		CpsLayer2YOffs = d;
		return;
	}
	
	if (a == 0x980006) {
		CpsLayer2XOffs = d - 0x40;
		return;
	}
	
	if (a == 0x980008) {
		CpsLayer3YOffs = d;
		return;
	}
	
	if (a == 0x98000a) {
		CpsLayer3XOffs = d - 0x40;
		return;
	}
}

static int DinopicInit()
{
	int nRet = 0;
	
	Dinopic = 1;
	CpsDrawSpritesInReverse = 1;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesBootleg(CpsGfx + 0x000000, 4);
	CpsLoadTilesBootleg(CpsGfx + 0x200000, 8);
	
	BootlegSpriteRam = (unsigned char*)malloc(0x2000);
	
	SekOpen(0);
	SekMapMemory(BootlegSpriteRam, 0x990000, 0x991FFF, SM_RAM);
	SekMapHandler(1, 0x980000, 0x98000b, SM_WRITE);
	SekSetWriteWordHandler(1, DinopicScrollWrite);
	SekClose();
	
	return nRet;
}

static void DinohCallback()
{
	// Patch Q-Sound Test ???
	*((unsigned short*)(CpsRom + 0xaacf4)) = 0x4e71;
}

static int DinohInit()
{
	Cps1QsHack = 1;
	AmendProgRomCallback = DinohCallback;
	
	return TwelveMhzInit();
}

static void DinohaCallback()
{
	unsigned char *TempRom = (unsigned char*)malloc(0x200000);
	if (TempRom) {
		memcpy(TempRom, CpsRom, 0x200000);
		memset(CpsRom, 0, 0x200000);
		memcpy(CpsRom + 0x080000, TempRom + 0x000000, 0x80000);
		memcpy(CpsRom + 0x000000, TempRom + 0x080000, 0x80000);
		memcpy(CpsRom + 0x180000, TempRom + 0x100000, 0x80000);
		memcpy(CpsRom + 0x100000, TempRom + 0x180000, 0x80000);
		free(TempRom);
	}
	
	// Patch Q-Sound Test ???
	*((unsigned short*)(CpsRom + 0xaacf4)) = 0x4e71;
}

static int DinohaInit()
{
	int nRet = 0;
	
	Cps1QsHack = 1;
	AmendProgRomCallback = DinohaCallback;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 2);
	
	return nRet;
}

static int DinohbInit()
{
	int nRet = 0;
	
	Dinopic = 1;
	Cps1Pic = 1;
	CpsDrawSpritesInReverse = 1;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 2);
	
	BootlegSpriteRam = (unsigned char*)malloc(0x2000);
	
	SekOpen(0);
	SekMapMemory(BootlegSpriteRam, 0x990000, 0x991FFF, SM_RAM);
	SekMapHandler(1, 0x980000, 0x98000b, SM_WRITE);
	SekSetWriteWordHandler(1, DinopicScrollWrite);
	SekClose();
	
	return nRet;
}

static int ForgottnInit()
{
	int nRet = 0;
	
	Forgottn = 1;

	nRet = DrvInit();
	
	CpsStar = CpsGfx + nCpsGfxLen - 0x2000;
	
	CpsLoadStars(CpsStar, 9);
	
	return nRet;
}

static void GhoulsCallback()
{
	BurnByteswap(CpsRom + 0x080000, 0x080000);
}

static int GhoulsInit()
{
	Ghouls = 1;
	AmendProgRomCallback = GhoulsCallback;

	return DrvInit();
}

static int DaimakaiInit()
{
	Ghouls = 1;
	
	return DrvInit();
}

static int DaimakbInit()
{
	Ghouls = 1;
	Port6SoundWrite = 1;	
	
	return DrvInit();
}

static int KodbInit()
{
	int nRet = 0;

	Kodb = 1;
	
	nRet = DrvInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesByte(CpsGfx, 2);

	return nRet;
}

static int KodhInit()
{
	Kodh = 1;
	
	return DrvInit();
}

static int MercsInit()
{
	Mercs = 1;

	return DrvInit();
}

static int Pang3Init()
{
	PangEEP = 1;

	return TwelveMhzInit();
}

static void Pang3jCallback()
{
	for (int i = 0x80000; i < 0x100000; i += 2) {
		int src = CpsRom[i];
		int dst = src & 0xff00;
		if ( src & 0x01) dst ^= 0x04;
		if ( src & 0x02) dst ^= 0x21;
		if ( src & 0x04) dst ^= 0x01;
		if (~src & 0x08) dst ^= 0x50;
		if ( src & 0x10) dst ^= 0x40;
		if ( src & 0x20) dst ^= 0x06;
		if ( src & 0x40) dst ^= 0x08;
		if (~src & 0x80) dst ^= 0x88;
		CpsRom[i] = (unsigned char)dst;
  	}
}

static int Pang3jInit()
{
	AmendProgRomCallback = Pang3jCallback;
	
	return Pang3Init();
}

static int PunipicInit()
{
	int nRet = 0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesBootleg(CpsGfx + 0x000000, 4);
	CpsLoadTilesBootleg(CpsGfx + 0x200000, 8);
	
	return nRet;
}

static int Punipic2Init()
{
	int nRet = 0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesPunipic2(CpsGfx, 4);
	
	return nRet;
}

static int Punipic3Init()
{
	int nRet = 0;
	
	Cps1Pic = 1;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 4);
	
	return nRet;
}

static int QadInit()
{
	Qad = 1;

	return TwelveMhzInit();
}

static int Sf2jcInit()
{
	Sf2jc = 1;

	return DrvInit();
}

static int Sf2ceInit()
{
	nCPS68KClockspeed = 7000000;
	
	return DrvInit();
}

static void Sf2accp2Callback()
{
	CpsRom[0x11755] = 0x4e;
	CpsRom[0x11756] = 0x71;
}

static int Sf2accp2Init()
{
	AmendProgRomCallback = Sf2accp2Callback;
	
	return Sf2ceInit();
}

static void Sf2rbCallback()
{
	BurnByteswap(CpsRom, 0x100000);
	CpsRom[0xe5465] = 0x60;
	CpsRom[0xe5464] = 0x12;
}

static int Sf2rbInit()
{
	AmendProgRomCallback = Sf2rbCallback;
	
	return Sf2ceInit();
}

static void Sf2rb2Callback()
{
	BurnByteswap(CpsRom, 0x100000);
	CpsRom[0xe5333] = 0x60;
	CpsRom[0xe5332] = 0x14;
}

static int Sf2rb2Init()
{
	AmendProgRomCallback = Sf2rb2Callback;
	
	return Sf2ceInit();
}

static int Sf2thInit()
{
	int nRet = 0;
	
	Sf2Hack = 1;
	
	CpsLayer1XOffs = -12;
	CpsLayer2XOffs = -14;
	CpsLayer3XOffs = -16;
	CpsDrawSpritesInReverse = 1;
	
	nRet = Sf2ceInit();
	
	return nRet;
}

static void Sf2yycCallback()
{
	memcpy(CpsRom + 0x140000, CpsRom + 0x100000, 0x40000);
        
        *((unsigned short*)(CpsRom + 0xe55be)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55ca)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55cc)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55ce)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55d0)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55d2)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55d4)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55d6)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55d8)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55da)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55de)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55e2)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55e4)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55e6)) = 0x4e71;
	*((unsigned short*)(CpsRom + 0xe55e8)) = 0x4e71;
}

static int Sf2yycInit()
{
	nCpsRomLen = 0x40000;
	
	Sf2Hack = 1;
	AmendProgRomCallback = Sf2yycCallback;
	
	CpsLayer1XOffs = -12;
	CpsLayer2XOffs = -14;
	CpsLayer3XOffs = -16;
	CpsDrawSpritesInReverse = 1;
	
	return Sf2ceInit();
}

static void Sf2koryuCallback()
{
	BurnByteswap(CpsRom, 0x140000);
}

static int Sf2koryuInit()
{
	Sf2Hack = 1;
	AmendProgRomCallback = Sf2koryuCallback;
	
	CpsLayer1XOffs = -12;
	CpsLayer2XOffs = -14;
	CpsLayer3XOffs = -16;
	CpsDrawSpritesInReverse = 1;
	
	return Sf2ceInit();
}

static int Sf2m4Init()
{
	Sf2Hack = 1;
	
	CpsLayer1XOffs = -12;
	CpsLayer2XOffs = -14;
	CpsLayer3XOffs = -16;
	CpsDrawSpritesInReverse = 1;
	
	return Sf2ceInit();
}

void __fastcall Sf2m1ScrollWrite(unsigned int a, unsigned short d)
{
	if (a == 0x980000) {
		CpsReg[0x0e] = d & 0xff;
		CpsReg[0x0f] = d >> 8;
		return;
	}
	
	if (a == 0x980002) {
		d -= 0x40;
		CpsReg[0x0c] = d & 0xff;
		CpsReg[0x0d] = d >> 8;
		return;
	}
	
	if (a == 0x980004) {
		CpsReg[0x12] = d & 0xff;
		CpsReg[0x13] = d >> 8;
		return;
	}
	
	if (a == 0x980006) {
		d -= 0x40;
		CpsReg[0x10] = d & 0xff;
		CpsReg[0x11] = d >> 8;
		return;
	}
	
	if (a == 0x980008) {
		CpsReg[0x16] = d & 0xff;
		CpsReg[0x17] = d >> 8;
		return;
	}
	
	if (a == 0x98000a) {
		d -= 0x40;
		CpsReg[0x14] = d & 0xff;
		CpsReg[0x15] = d >> 8;
		return;
	}
	
	if (a == 0x980016) {
		CpsReg[0x06] = d & 0xff;
		CpsReg[0x07] = d >> 8;
		return;
	}
	
	if (a == 0x980166) {
		CpsReg[0x66] = d & 0xff;
		CpsReg[0x67] = d >> 8;
		return;
	}
}

static int Sf2m1Init()
{
	int nRet = 0;
	
	CpsLayer2XOffs = 4;
	Port6SoundWrite = 1;
	
	nRet = Sf2ceInit();
	
	SekOpen(0);
	SekMapHandler(1, 0x980000, 0x980167, SM_WRITE);
	SekSetWriteWordHandler(1, Sf2m1ScrollWrite);
	SekClose();
	
	return nRet;
}

static void Sf2m2Callback()
{
	*((unsigned short*)(CpsRom + 0xC0680)) = 0x4E71;
	*((unsigned short*)(CpsRom + 0xC0682)) = 0x4E71;
	*((unsigned short*)(CpsRom + 0xC0684)) = 0x4E71;
}

static int Sf2m2Init()
{
	Sf2Hack = 1;
	AmendProgRomCallback = Sf2m2Callback;
	
	CpsLayer1XOffs = -12;
	CpsLayer2XOffs = -14;
	CpsLayer3XOffs = -16;
	CpsDrawSpritesInReverse = 1;
	
	return Sf2ceInit();
}

unsigned char __fastcall Sf2m3ReadByte(unsigned int a)
{
	unsigned char d = 0xff;
	
	switch (a) {
		case 0x800010: {
			d = (unsigned char)~Inp010;
			return d;
		}
		
		case 0x800011: {
			d = (unsigned char)~Inp011;
			return d;
		}
		
		case 0x800029: {
			d = (unsigned char)~Inp029;
			return d;
		}
		
		case 0x800186: {
			d = (unsigned char)~Inp186;
			return d;
		}
		
		case 0x80002a: {
			d = (unsigned char)~Cpi01A;
			return d;
		}
		
		case 0x80002c: {
			d = (unsigned char)~Cpi01C;
			return d;
		}
		
		case 0x80002e: {
			d = (unsigned char)~Cpi01E;
			return d;
		}
	}
	
//	bprintf(PRINT_NORMAL, _T("Read byte %x\n"), a);
	
	return 0;
}

void __fastcall Sf2m3WriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x800191: {
			PsndSyncZ80((long long)SekTotalCycles() * nCpsZ80Cycles / nCpsCycles);

			PsndCode = d;
			return;
		}
	}
	
//	bprintf(PRINT_NORMAL, _T("Write byte %x, %x\n"), a, d);
}

void __fastcall Sf2m3WriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0x800100: {
			CpsReg[0x00] = d & 0xff;
			CpsReg[0x01] = d >> 8;
			return;
		}
		
		case 0x800102: {
			CpsReg[0x02] = d & 0xff;
			CpsReg[0x03] = d >> 8;
			return;
		}
		
		case 0x800104: {
			CpsReg[0x04] = d & 0xff;
			CpsReg[0x05] = d >> 8;
			return;
		}
		
		case 0x800106: {
			CpsReg[0x06] = d & 0xff;
			CpsReg[0x07] = d >> 8;
			return;
		}
		
		case 0x80010a: {
			CpsReg[0x0a] = d & 0xff;
			CpsReg[0x0b] = d >> 8;
			return;
		}
		
		case 0x800122: {
			CpsReg[0x22] = d & 0xff;
			CpsReg[0x23] = d >> 8;
			return;
		}
		
		case 0x80014a: {
			CpsReg[0x4a] = d & 0xff;
			CpsReg[0x4b] = d >> 8;
			return;
		}
		
		case 0x80014c: {
			CpsReg[0x4c] = d & 0xff;
			CpsReg[0x4d] = d >> 8;
			return;
		}
		
		case 0x80014e: {
			CpsReg[0x4e] = d & 0xff;
			CpsReg[0x4f] = d >> 8;
			return;
		}
		
		case 0x800150: {
			CpsReg[0x50] = d & 0xff;
			CpsReg[0x51] = d >> 8;
			return;
		}
		
		case 0x800152: {
			CpsReg[0x52] = d & 0xff;
			CpsReg[0x53] = d >> 8;
			return;
		}
		
		case 0x8001a8: {
			CpsReg[0x08] = d & 0xff;
			CpsReg[0x09] = d >> 8;
			return;
		}
		
		case 0x8001ac: {
			CpsReg[0x0c] = d & 0xff;
			CpsReg[0x0d] = d >> 8;
			return;
		}
		
		case 0x8001ae: {
			CpsReg[0x0e] = d & 0xff;
			CpsReg[0x0f] = d >> 8;
			return;
		}
		
		case 0x8001b0: {
			CpsReg[0x10] = d & 0xff;
			CpsReg[0x11] = d >> 8;
			return;
		}
		
		case 0x8001b2: {
			CpsReg[0x12] = d & 0xff;
			CpsReg[0x13] = d >> 8;
			return;
		}
		
		case 0x8001b4: {
			CpsReg[0x14] = d & 0xff;
			CpsReg[0x15] = d >> 8;
			return;
		}
		
		case 0x8001b6: {
			CpsReg[0x16] = d & 0xff;
			CpsReg[0x17] = d >> 8;
			return;
		}
		
		case 0x8001c0: {
			CpsReg[0x20] = d & 0xff;
			CpsReg[0x21] = d >> 8;
			return;
		}	
		
		case 0x8001c4: {
			CpsReg[0xc4] = d & 0xff;
			CpsReg[0xc5] = d >> 8;
			return;
		}
	}
	
//	bprintf(PRINT_NORMAL, _T("Write word %x, %x\n"), a, d);
}

static int Sf2m3Init()
{
	int nRet = 0;
	
	CpsLayer1XOffs = -16;
	CpsLayer2XOffs = -16;
	CpsLayer3XOffs = -16;
	CpsDrawSpritesInReverse = 1;
	
	nRet = Sf2ceInit();
	
	SekOpen(0);
	SekMapHandler(1, 0x800000, 0x800200, SM_RAM);
	SekSetReadByteHandler(1, Sf2m3ReadByte);
	SekSetWriteByteHandler(1, Sf2m3WriteByte);
	SekSetWriteWordHandler(1, Sf2m3WriteWord);
	
	SekClose();
	
	return nRet;
}

static int SfzchInit()
{
	int nRet = 0;
	
	nRet = DrvInit();
	
	Cps = 3;
	
	return nRet;
}

static int StriderInit()
{
	int nRet = 0;
	
	nRet = DrvInit();
	
	CpsStar = CpsGfx + nCpsGfxLen - 0x2000;
	
	CpsLoadStars(CpsStar, 5);
	
	return nRet;
}

static int StriderjInit()
{
	int nRet = 0;
	
	nRet = DrvInit();
	
	CpsStar = CpsGfx + nCpsGfxLen - 0x2000;
	
	CpsLoadStars(CpsStar, 2);
	
	return nRet;
}

static int StriderjaInit()
{
	int nRet = 0;
	
	nRet = DrvInit();
	
	CpsStar = CpsGfx + nCpsGfxLen - 0x2000;
	
	CpsLoadStarsByte(CpsStar, 8);
	
	return nRet;
}

static int WofbInit()
{
	int nRet = 0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesBootleg(CpsGfx + 0x000000, 4);
	CpsLoadTilesBootleg(CpsGfx + 0x200000, 8);
	
	return nRet;	
}

static void Wof3jsCallback()
{
	memcpy(CpsRom + 0x0c0000, CpsRom + 0x100000, 0x40000);
	memset(CpsRom + 0x100000, 0, 0x40000);
}

static int Wof3jsInit()
{
	AmendProgRomCallback = Wof3jsCallback;
	
	return TwelveMhzInit();
}

static int Wof3sjInit()
{
	int nRet = 0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 2);
	
	return nRet;
}

static int Wof3sjaInit()
{
	int nRet = 0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 1);
	
	return nRet;
}

static void WofhCallback()
{
	// Patch protection? check
	CpsRom[0xf11ed] = 0x4e;
	CpsRom[0xf11ec] = 0x71;
	CpsRom[0xf11ef] = 0x4e;
	CpsRom[0xf11ee] = 0x71;
}

static int WofhInit()
{
	int nRet = 0;
	
	AmendProgRomCallback = WofhCallback;
	
	CpsLayer1XOffs = 0xffc0;
	CpsLayer2XOffs = 0xffc0;
	CpsLayer3XOffs = 0xffc0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 1);
	
	return nRet;
}

static int WofhaInit()
{
	int nRet = 0;
	
	AmendProgRomCallback = WofhCallback;
	
	CpsLayer1XOffs = 0xffc0;
	CpsLayer2XOffs = 0xffc0;
	CpsLayer3XOffs = 0xffc0;
	
	nRet = TwelveMhzInit();
	
	memset(CpsGfx, 0, nCpsGfxLen);
	CpsLoadTilesHack160(CpsGfx, 2);
	
	return nRet;
}

static int DrvExit()
{
	CpsRunExit();
	CpsExit();
	
	nCpsRomLen = 0;
	nCpsZRomLen = 0;
	nCpsGfxLen = 0;
	nCpsAdLen = 0;
	nCpsQSamLen = 0;
	nCpsPicRomNum = 0;
	
	nCps68KByteswapRomNum = 0;
	nCps68KNoByteswapRomNum = 0;
	nCpsZ80RomNum = 0;
	nCpsTilesRomNum = 0;
	nCpsOkim6295RomNum = 0;
	nCpsQsoundRomNum = 0;
	KabukiDecodeFunction = NULL;
	GameHasStars = 0;
	
	AmendProgRomCallback = NULL;
	
	CpsLayer1XOffs = 0;
	CpsLayer2XOffs = 0;
	CpsLayer3XOffs = 0;
	CpsLayer1YOffs = 0;
	CpsLayer2YOffs = 0;
	CpsLayer3YOffs = 0;
	CpsDrawSpritesInReverse = 0;

	Cps = 0;
	Cps1Qs = 0;
	Cps1Pic = 0;
	Forgottn = 0;
	Ghouls = 0;
	Kodb = 0;
	Mercs = 0;
	PangEEP = 0;
	Qad = 0;
	Sf2jc = 0;
	Sf2Hack = 0;
	Dinopic = 0;
	Port6SoundWrite = 0;
	
	Cps1QsHack = 0;
	
	free(BootlegSpriteRam);
	BootlegSpriteRam = NULL;
	
	return 0;
}

// Driver Definitions

struct BurnDriver BurnDrvCps1941 = {
	"1941", NULL, NULL, "1990",
	"1941 - Counter Attack (World)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_CAPCOM_CPS1,
	NULL, NTFODrvRomInfo, NTFODrvRomName, NTFOInputInfo, NTFODIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCps1941j = {
	"1941j", "1941", NULL, "1990",
	"1941 - Counter Attack (Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_CAPCOM_CPS1,
	NULL, NTFOJDrvRomInfo, NTFOJDrvRomName, NTFOInputInfo, NTFODIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCps3wonders = {
	"3wonders", NULL, NULL, "1991",
	"Three Wonders (wonder 3 910520 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, ThreeWondersRomInfo, ThreeWondersRomName, ThreeWondersInputInfo, ThreeWondersDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCps3wonderu = {
	"3wonderu", "3wonders", NULL, "1991",
	"Three Wonders (wonder 3 910520 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Wonder3uRomInfo, Wonder3uRomName, ThreeWondersInputInfo, ThreeWondersDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWonder3 = {
	"wonder3", "3wonders", NULL, "1991",
	"Wonder 3 (910520 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Wonder3RomInfo, Wonder3RomName, ThreeWondersInputInfo, ThreeWondersDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCaptcomm = {
	"captcomm", NULL, NULL, "1991",
	"Captain Commando (911014 other country)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 4, HARDWARE_CAPCOM_CPS1,
	NULL, CaptcommRomInfo, CaptcommRomName, CaptcommInputInfo, CaptcommDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCaptcomu = {
	"captcomu", "captcomm", NULL, "1991",
	"Captain Commando (910928 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, CaptcomuRomInfo, CaptcomuRomName, CaptcommInputInfo, CaptcommDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCaptcomj = {
	"captcomj", "captcomm", NULL, "1991",
	"Captain Commando (911202 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, CaptcomjRomInfo, CaptcomjRomName, CaptcommInputInfo, CaptcommDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsCaptcomb = {
	"captcomb", "captcomm", NULL, "1991",
	"Captain Commando (bootleg)\0", "Sprite issues", "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 4, HARDWARE_CAPCOM_CPS1,
	NULL, CaptcombRomInfo, CaptcombRomName, CaptcommInputInfo, CaptcommDIPInfo,
	CaptcombInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCawing = {
	"cawing", NULL, NULL, "1990",
	"Carrier Air Wing (U.S. navy 901012 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, CawingRomInfo, CawingRomName, CawingInputInfo, CawingDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCawingr1 = {
	"cawingr1", "cawing", NULL, "1990",
	"Carrier Air Wing (U.S. navy 901009 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Cawingr1RomInfo, Cawingr1RomName, CawingInputInfo, CawingDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCawingu = {
	"cawingu", "cawing", NULL, "1990",
	"Carrier Air Wing (U.S. navy 901012 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, CawinguRomInfo, CawinguRomName, CawingInputInfo, CawingDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCawingj = {
	"cawingj", "cawing", NULL, "1990",
	"U.S. Navy (901012 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, CawingjRomInfo, CawingjRomName, CawingInputInfo, CawingDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsCawingb = {
	"cawingb", "cawing", NULL, "1990",
	"Carrier Air Wing (bootleg)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, CawingbRomInfo, CawingbRomName, CawingbInputInfo, CawingDIPInfo,
	CawingbInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCworld2j = {
	"cworld2j", NULL, NULL, "1992",
	"Capcom World 2 (920611 Japan)\0", NULL, "Capcom", "CPS1",
	L"Capcom World 2 (\u30AF\u30A4\u30BA\uFF15 \u3042\u3069\u3079\u3093\u3061\u3083\u30FC\u304F\u3044\u305A \u304B\u3077\u3053\u3093\u308F\u30FC\u308B\u3069\uFF12 920611 Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Cworld2jRomInfo, Cworld2jRomName, Cworld2jInputInfo, Cworld2jDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDino = {
	"dino", NULL, NULL, "1993",
	"Cadillacs & Dinosaurs (930201 etc)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinoRomInfo, DinoRomName, DinoInputInfo, DinoDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDinoj = {
	"dinoj", "dino", NULL, "1993",
	"Cadillacs Kyouryuu-Shinseiki (Cadillacs 930201 Japan)\0", NULL, "Capcom", "CPS1 / QSound",
	L"Cadillacs \u6050\u7ADC\u65B0\u4E16\u7D00 (Cadillacs 930201 Japan)\0Cadillacs Kyouryuu-Shinseiki (Cadillacs 930201 Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinojRomInfo, DinojRomName, DinoInputInfo, DinoDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDinou = {
	"dinou", "dino", NULL, "1993",
	"Cadillacs & Dinosaurs (930201 USA)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinouRomInfo, DinouRomName, DinoInputInfo, DinoDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsDinopic = {
	"dinopic", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs (bootleg with PIC16c57, set 1)\0", "Missing sprites, no sound", "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, DinopicRomInfo, DinopicRomName, DinoInputInfo, DinoDIPInfo,
	DinopicInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsDinopic2 = {
	"dinopic2", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs (bootleg with PIC16c57, set 2)\0", "Missing sprites, no sound", "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, Dinopic2RomInfo, Dinopic2RomName, DinoInputInfo, DinoDIPInfo,
	DinopicInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDinoh = {
	"dinoh", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs (930223 Asia TW bootleg)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinohRomInfo, DinohRomName, DinohInputInfo, DinohDIPInfo,
	DinohInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDinoha = {
	"dinoha", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs Turbo (930223 Asia TW bootleg)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinohaRomInfo, DinohaRomName, DinohInputInfo, DinohDIPInfo,
	DinohaInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsDinohb = {
	"dinohb", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs Turbo (bootleg with PIC16c57)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, DinohbRomInfo, DinohbRomName, DinoInputInfo, DinoDIPInfo,
	DinohbInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDw = {
	"dynwar", NULL, NULL, "1989",
	"Dynasty Wars (US set 1)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, DynwarRomInfo, DynwarRomName, DynwarInputInfo, DynwarDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDwu = {
	"dynwaru", "dynwar", NULL, "1989",
	"Dynasty Wars (US set 2)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, DynwaruRomInfo, DynwaruRomName, DynwarInputInfo, DynwarDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDwj = {
	"dynwarj", "dynwar", NULL, "1989",
	"Tenchi wo Kurau (Japan)\0", NULL, "Capcom", "CPS1",
	L"\u8EE2\u5730\u3092\u55B0\u3089\u3046 (Japan)\0Tenchi wo Kurau (Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, DynwarjRomInfo, DynwarjRomName, DynwarInputInfo, DynwarDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfight = {
	"ffight", NULL, NULL, "1989",
	"Final Fight (World)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, FfightRomInfo, FfightRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfightu = {
	"ffightu", "ffight", NULL, "1989",
	"Final Fight (USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, FfightuRomInfo, FfightuRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfightua = {
	"ffightua", "ffight", NULL, "1989",
	"Final Fight (900112 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, FfightuaRomInfo, FfightuaRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfightub = {
	"ffightub", "ffight", NULL, "1989",
	"Final Fight (900613 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, FfightubRomInfo, FfightubRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfightj = {
	"ffightj", "ffight", NULL, "1989",
	"Final Fight (Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, FfightjRomInfo, FfightjRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfightj1 = {
	"ffightj1", "ffight", NULL, "1989",
	"Final Fight (900305 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Ffightj1RomInfo, Ffightj1RomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsFfightj2 = {
	"ffightj2", "ffight", NULL, "1989",
	"Final Fight (900112 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Ffightj2RomInfo, Ffightj2RomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsForgottn = {
	"forgottn", NULL, NULL, "1988",
	"Forgotten Worlds (US)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1_GENERIC,
	NULL, ForgottnRomInfo, ForgottnRomName, ForgottnInputInfo, ForgottnDIPInfo,
	ForgottnInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsForgott1 = {
	"forgott1", "forgottn", NULL, "1988",
	"Forgotten Worlds (World?)\0", "Missing some 68k roms", "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1_GENERIC,
	NULL, Forgott1RomInfo, Forgott1RomName, ForgottnInputInfo, ForgottnDIPInfo,
	ForgottnInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsLostwrld = {
	"lostwrld", "forgottn", NULL, "1988",
	"Lost Worlds (Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1_GENERIC,
 	NULL, LostwrldRomInfo, LostwrldRomName, ForgottnInputInfo, ForgottnDIPInfo,
	ForgottnInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsGhouls = {
	"ghouls", NULL, NULL, "1988",
	"Ghouls'n Ghosts (World)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, GhoulsRomInfo, GhoulsRomName, GhoulsInputInfo, GhoulsDIPInfo,
	GhoulsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsGhoulsu = {
	"ghoulsu", "ghouls", NULL, "1988",
	"Ghouls'n Ghosts (US)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, GhoulsuRomInfo, GhoulsuRomName, GhoulsInputInfo, GhoulsuDIPInfo,
	GhoulsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsDaimakai = {
	"daimakai", "ghouls", NULL, "1988",
	"Dai Makai-Mura (Japan)\0", NULL, "Capcom", "CPS1",
	L"\u5927\u9B54\u754C\u6751 (Japan)\0Dai Makai-Mura (Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, DaimakaiRomInfo, DaimakaiRomName, GhoulsInputInfo, DaimakaiDIPInfo,
	DaimakaiInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsDaimakb = {
	"daimakb", "ghouls", NULL, "1988",
	"Dai Makai-Mura (Japan, bootleg)\0", "No scroll layers", "Capcom", "CPS1",
	L"\u5927\u9B54\u754C\u6751 (Japan)\0Dai Makai-Mura (Japan, bootleg)\0", NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, DaimakbRomInfo, DaimakbRomName, GhoulsInputInfo, DaimakaiDIPInfo,
	DaimakbInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKnights = {
	"knights", NULL, NULL, "1991",
	"Knights of the Round (911127 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KnightsRomInfo, KnightsRomName, KnightsInputInfo, KnightsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKnightsu = {
	"knightsu", "knights", NULL, "1991",
	"Knights of the Round (911127 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KnightsuRomInfo, KnightsuRomName, KnightsInputInfo, KnightsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKnightsj = {
	"knightsj", "knights", NULL, "1991",
	"Knights of the Round (911127 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KnightsjRomInfo, KnightsjRomName, KnightsInputInfo, KnightsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsKnightsb = {
	"knightsb", "knights", NULL, "1991",
	"Knights of the Round (911127 etc bootleg set 1)\0", "Some sprite problems", "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KnightsbRomInfo, KnightsbRomName, KnightsInputInfo, KnightsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKnightsh = {
	"knightsh", "knights", NULL, "1991",
	"Knights of the Round (911127 etc bootleg set 2)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KnightshRomInfo, KnightshRomName, KnightsInputInfo, KnightsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKnghtsha = {
	"knghtsha", "knights", NULL, "1991",
	"Knights of the Round (911127 etc bootleg set 3)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KnghtshaRomInfo, KnghtshaRomName, KnightsInputInfo, KnightsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKod = {
	"kod", NULL, NULL, "1991",
	"The King of Dragons (910711 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KodRomInfo, KodRomName, KodInputInfo, KodDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKodu = {
	"kodu", "kod", NULL, "1991",
	"The King of Dragons (US 910910)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KoduRomInfo, KoduRomName, KodInputInfo, KodjDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKodj = {
	"kodj", "kod", NULL, "1991",
	"The King of Dragons (Japan 910805)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KodjRomInfo, KodjRomName, KodInputInfo, KodjDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsKodb = {
	"kodb", "kod", NULL, "1991",
	"The King of Dragons (bootleg set 1)\0", "No Sprites", "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KodbRomInfo, KodbRomName, KodInputInfo, KodDIPInfo,
	KodbInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKodh = {
	"kodh", "kod", NULL, "1991",
	"The King of Dragons (bootleg set 2)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, KodhRomInfo, KodhRomName, KodhInputInfo, KodjDIPInfo,
	KodhInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMegaman = {
	"megaman", NULL, NULL, "1995",
	"Mega Man - the power battle (951006 Asia)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, MegamanRomInfo, MegamanRomName, MegamanInputInfo, MegamanDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsRockmanj = {
	"rockmanj", "megaman", NULL, "1995",
	"Rockman - the power battle (950922 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, RockmanjRomInfo, RockmanjRomName, MegamanInputInfo, RockmanjDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMercs = {
	"mercs", NULL, NULL, "1990",
	"Mercs (900302 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 3, HARDWARE_CAPCOM_CPS1,
	NULL, MercsRomInfo, MercsRomName, MercsInputInfo, MercsDIPInfo,
	MercsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsMercsu = {
	"mercsu", "mercs", NULL, "1990",
	"Mercs (900302 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 3, HARDWARE_CAPCOM_CPS1,
	NULL, MercsuRomInfo, MercsuRomName, MercsInputInfo, MercsDIPInfo,
	MercsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsMercsua = {
	"mercsua", "mercs", NULL, "1990",
	"Mercs (900608 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 3, HARDWARE_CAPCOM_CPS1,
	NULL, MercsuaRomInfo, MercsuaRomName, MercsInputInfo, MercsDIPInfo,
	MercsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsMercsj = {
	"mercsj", "mercs", NULL, "1990",
	"Senjo no Ookami II (Ookami 2 900302 Japan)\0", NULL, "Capcom", "CPS1",
	L"\u6226\u5834\u306E\u72FC II (Ookami 2 900302 Japan)\0Senjo no Ookami II (Ookami 2 900302 Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 3, HARDWARE_CAPCOM_CPS1,
	NULL, MercsjRomInfo, MercsjRomName, MercsInputInfo, MercsDIPInfo,
	MercsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsMsword = {
	"msword", NULL, NULL, "1990",
	"Magic Sword - heroic fantasy (25.07.1990 other country)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, MswordRomInfo, MswordRomName, MswordInputInfo, MswordDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMswordr1 = {
	"mswordr1", "msword", NULL, "1990",
	"Magic Sword - heroic fantasy (23.06.1990 other country)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Mswordr1RomInfo, Mswordr1RomName, MswordInputInfo, MswordDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMswordj = {
	"mswordj", "msword", NULL, "1990",
	"Magic Sword (23.06.1990 Japan)\0", NULL, "Capcom", "CPS1",
	L"Magic Sword (23.06.1990 Japan)\0Magic Sword\u30DE\u30B8\u30C3\u30AF\uFF65\u30BD\u30FC\u30C9\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, MswordjRomInfo, MswordjRomName, MswordInputInfo, MswordDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMswordu = {
	"mswordu", "msword", NULL, "1990",
	"Magic Sword - heroic fantasy (25.07.1990 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, MsworduRomInfo, MsworduRomName, MswordInputInfo, MswordDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMtwins = {
	"mtwins", NULL, NULL, "1990",
	"Mega Twins (chiki chiki boys 900619 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING,2,HARDWARE_CAPCOM_CPS1,
	NULL, MtwinsRomInfo, MtwinsRomName, MtwinsInputInfo, MtwinsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsChikij = {
	"chikij", "mtwins", NULL, "1990",
	"Chiki Chiki Boys (900619 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, ChikijRomInfo, ChikijRomName, MtwinsInputInfo, MtwinsDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsNemo = {
	"nemo", NULL, NULL, "1990",
	"Nemo (90 11 30 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, NemoRomInfo, NemoRomName, NemoInputInfo, NemoDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsNemoj = {
	"nemoj", "nemo", NULL, "1990",
	"Nemo (90 11 20 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2 ,HARDWARE_CAPCOM_CPS1,
	NULL, NemojRomInfo, NemojRomName, NemoInputInfo, NemoDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsPang3 = {
	"pang3", NULL, NULL, "1995",
	"Pang! 3 (950511 Euro)\0", NULL, "Mitchell", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1_GENERIC,
	NULL, Pang3RomInfo, Pang3RomName, Pang3InputInfo, Pang3DIPInfo,
	Pang3Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsPang3j = {
	"pang3j", "pang3", NULL, "1995",
	"Pang! 3 (950511 Japan)\0", NULL, "Mitchell", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1_GENERIC,
	NULL, Pang3jRomInfo, Pang3jRomName, Pang3InputInfo, Pang3DIPInfo,
	Pang3jInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsPnickj = {
	"pnickj", NULL, NULL, "1994",
	"Pnickies (940608 Japan)\0", NULL, "Compile (Capcom license)", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, PnickjRomInfo, PnickjRomName, PnickjInputInfo, PnickjDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsPunisher = {
	"punisher", NULL, NULL, "1993",
	"The Punisher (930422 etc)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, PunisherRomInfo, PunisherRomName, PunisherInputInfo, PunisherDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsPunishru = {
	"punishru", "punisher", NULL, "1993",
	"The Punisher (930422 USA)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, PunishruRomInfo, PunishruRomName, PunisherInputInfo, PunisherDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsPunishrj = {
	"punishrj", "punisher", NULL, "1993",
	"The Punisher (930422 Japan)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, PunishrjRomInfo, PunishrjRomName, PunisherInputInfo, PunisherDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsPunipic = {
	"punipic", "punisher", NULL, "1993",
	"The Punisher (bootleg with PIC16c57, set 1)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, PunipicRomInfo, PunipicRomName, PunisherInputInfo, PunisherDIPInfo,
	PunipicInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsPunipic2 = {
	"punipic2", "punisher", NULL, "1993",
	"The Punisher (bootleg with PIC16c57, set 2)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Punipic2RomInfo, Punipic2RomName, PunisherInputInfo, PunisherDIPInfo,
	Punipic2Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsPunipic3 = {
	"punipic3", "punisher", NULL, "1993",
	"The Punisher (bootleg with PIC16c57, set 3)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Punipic3RomInfo, Punipic3RomName, PunisherInputInfo, PunisherDIPInfo,
	Punipic3Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsQad = {
	"qad", NULL, NULL, "1992",
	"Quiz & Dragons (capcom quiz game 920701 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, QadRomInfo, QadRomName, QadInputInfo, QadDIPInfo,
	QadInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsQadj = {
	"qadj", "qad", NULL, "1992",
	"Quiz & Dragons (940921 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, QadjRomInfo, QadjRomName, QadInputInfo, QadjDIPInfo,
	QadInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsQtono2 = {
	"qtono2", NULL, NULL, "1995",
	"Quiz Tonosama no Yabou 2 Zenkoku-ban (tonosama 2 950123 Japan)\0", NULL, "Capcom", "CPS1",
	L"\u30AF\u30A4\u30BA\u6BBF\u69D8\u306E\u91CE\u671B\uFF12 (\u3068\u306E\u3055\u307E\u306E\u3084\u307C\u3046 2 950123 Japan)\0Quiz Tonosama no Yabou 2 Zenkoku-ban (tonosama 2 950123 Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Qtono2RomInfo, Qtono2RomName, Qtono2InputInfo, Qtono2DIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2 = {
	"sf2", NULL, NULL, "1991",
	"Street Fighter II - the world warrior (910522 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2RomInfo, Sf2RomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2eb = {
	"sf2eb", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910214 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ebRomInfo, Sf2ebRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ua = {
	"sf2ua", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910206 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2uaRomInfo, Sf2uaRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ub = {
	"sf2ub", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910214 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ubRomInfo, Sf2ubRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ud = {
	"sf2ud", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910318 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE ,2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2udRomInfo, Sf2udRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ue = {
	"sf2ue", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910228 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ueRomInfo, Sf2ueRomName, Sf2ueInputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2uf = {
	"sf2uf", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910411 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ufRomInfo, Sf2ufRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ui = {
	"sf2ui", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910522 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2uiRomInfo, Sf2uiRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2uk = {
	"sf2uk", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (911101 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ukRomInfo, Sf2ukRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2j = {
	"sf2j", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (911210 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2jRomInfo, Sf2jRomName, Sf2InputInfo, Sf2jDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ja = {
	"sf2ja", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910214 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2jaRomInfo, Sf2jaRomName, Sf2InputInfo, Sf2jDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2jc = {
	"sf2jc", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (910306 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2jcRomInfo, Sf2jcRomName, Sf2InputInfo, Sf2jDIPInfo,
	Sf2jcInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsSf2b = {
	"sf2b", "sf2", NULL, "1991",
	"Street Fighter II - the world warrior (bootleg)\0", "Resets itself", "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2bRomInfo, Sf2bRomName, Sf2InputInfo, Sf2DIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ce = {
	"sf2ce", NULL, NULL, "1992",
	"Street Fighter II' - champion edition (street fighter 2' 920313 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ceRomInfo, Sf2ceRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2cej = {
	"sf2cej", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (street fighter 2' 920513 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2cejRomInfo, Sf2cejRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ceua = {
	"sf2ceua", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (street fighter 2' 920313 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ceuaRomInfo, Sf2ceuaRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ceub = {
	"sf2ceub", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (street fighter 2' 920513 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ceubRomInfo, Sf2ceubRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ceuc = {
	"sf2ceuc", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (street fighter 2' 920803 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ceucRomInfo, Sf2ceucRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2accp2 = {
	"sf2accp2", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Accelerator Pt.II)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG,2,HARDWARE_CAPCOM_CPS1,
	NULL, Sf2accp2RomInfo, Sf2accp2RomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2accp2Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2rb = {
	"sf2rb", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Rainbow set 1)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG,2,HARDWARE_CAPCOM_CPS1,
	NULL, Sf2rbRomInfo, Sf2rbRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2rbInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2rb2 = {
	"sf2rb2", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Rainbow set 2)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG,2,HARDWARE_CAPCOM_CPS1,
	NULL, Sf2rb2RomInfo, Sf2rb2RomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2rb2Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2red = {
	"sf2red", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Red Wave)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2redRomInfo, Sf2redRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2v004 = {
	"sf2v004", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, V004)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2v004RomInfo, Sf2v004RomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2hf = {
	"sf2hf", "sf2ce", NULL, "1992",
	"Street Fighter II' - hyper fighting (street fighter 2' T 921209 ETC)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2hfRomInfo, Sf2hfRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2t = {
	"sf2t", "sf2hf", NULL, "1992",
	"Street Fighter II' - hyper fighting (street fighter 2' T 921209 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2tRomInfo, Sf2tRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2tj = {
	"sf2tj", "sf2hf", NULL, "1992",
	"Street Fighter II' Turbo - hyper fighting (street fighter 2' T 921209 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2tjRomInfo, Sf2tjRomName, Sf2InputInfo, Sf2jDIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2yyc = {
	"sf2yyc", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, YYC)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2yycRomInfo, Sf2yycRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2yycInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsKouryu = {
	"sf2koryu", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (Chinese bootleg, Xiang Long)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, KouryuRomInfo, KouryuRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2koryuInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsSf2m1 = {
	"sf2m1", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M1)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m1RomInfo, Sf2m1RomName, Sf2m1InputInfo, Sf2DIPInfo,
	Sf2m1Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2m2 = {
	"sf2m2", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M2)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m2RomInfo, Sf2m2RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m2Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2m3 = {
	"sf2m3", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M3)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m3RomInfo, Sf2m3RomName, Sf2m3InputInfo, Sf2DIPInfo,
	Sf2m3Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2m4 = {
	"sf2m4", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M4)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m4RomInfo, Sf2m4RomName, Sf2yycInputInfo, Sf2jDIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2m5 = {
	"sf2m5", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M5)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m5RomInfo, Sf2m5RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2m6 = {
	"sf2m6", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M6)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG,2,HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m6RomInfo, Sf2m6RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2m7 = {
	"sf2m7", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M7)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m7RomInfo, Sf2m7RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsSf2m8 = {
	"sf2m8", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M8)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m8RomInfo, Sf2m8RomName, Sf2m3InputInfo, Sf2DIPInfo,
	Sf2m3Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsSf2m13 = {
	"sf2m13", "sf2hf", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, M13)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2m13RomInfo, Sf2m13RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2tlona = {
	"sf2tlona", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Slay the Dragon set 1)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2tlonaRomInfo, Sf2tlonaRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2tlonb = {
	"sf2tlonb", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Slay the Dragon set 2)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2tlonbRomInfo, Sf2tlonbRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2cebr = {
	"sf2cebr", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Brazil)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2cebrRomInfo, Sf2cebrRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSf2ceh = {
	"sf2ceh", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (bootleg, Hispanic)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2cehRomInfo, Sf2cehRomName, Sf2InputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsSf2th = {
	"sf2th", "sf2hf", NULL, "1992",
	"Street Fighter II' - (Turbo Hack)\0", "Resets itself", "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2thRomInfo, Sf2thRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2thInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSfzch = {
	"sfzch", NULL, NULL, "1995",
	"Street Fighter Zero (951020 CPS Changer)\0", NULL, "Capcom", "CPS Changer",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPSCHANGER,
	NULL, SfzchRomInfo, SfzchRomName, SfzchInputInfo, NULL,
	SfzchInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSlammast = {
	"slammast", NULL, NULL, "1993",
	"Saturday Night Slam Masters (Slam Masters 930713 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 4, HARDWARE_CAPCOM_CPS1,
	NULL, SlammastRomInfo, SlammastRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSlammasa = {
	"slammasa", "slammast", NULL, "1993",
	"Saturday Night Slam Masters (Slam Masters 930301 Asia)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, SlammasaRomInfo, SlammasaRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsSlammasu = {
	"slammasu", "slammast", NULL, "1993",
	"Saturday Night Slam Masters (slam masters 930713 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, SlammasuRomInfo, SlammasuRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMbomberj = {
	"mbomberj", "slammast", NULL, "1993",
	"Muscle Bomber - the body explosion (930713 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbomberjRomInfo, MbomberjRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMbombrd = {
	"mbombrd", NULL, NULL, "1993",
	"Muscle Bomber Duo - ultimate team battle (931206 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbombrdRomInfo, MbombrdRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsMbombrdj = {
	"mbombrdj", "mbombrd", NULL, "1993",
	"Muscle Bomber Duo - heat up warriors (931206 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbombrdjRomInfo, MbombrdjRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsStrider = {
	"strider", NULL, NULL, "1989",
	"Strider (US set 1)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, StriderRomInfo, StriderRomName, StriderInputInfo, StriderDIPInfo,
	StriderInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsStridrua = {
	"stridrua", "strider", NULL, "1989",
	"Strider (US set 2)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_CAPCOM_CPS1,
	NULL, StridruaRomInfo, StridruaRomName, StriderInputInfo, StridruaDIPInfo,
	StriderInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsStriderj = {
	"striderj", "strider", NULL, "1989",
	"Strider Hiryu (Japan set 1)\0", NULL, "Capcom", "CPS1",
	L"\u30B9\u30C8\u30E9\u30A4\u30C0\u30FC\u98DB\u7ADC (Japan set 1)\0Strider Hiryu (Japan set 1)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_CAPCOM_CPS1,
	NULL, StriderjRomInfo, StriderjRomName, StriderInputInfo, StriderDIPInfo,
	StriderjInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsStridrja = {
	"stridrja", "strider", NULL, "1989",
	"Strider Hiryu (Japan set 2)\0", NULL, "Capcom", "CPS1",
	L"\u30B9\u30C8\u30E9\u30A4\u30C0\u30FC\u98DB\u7ADC (Japan set 2)\0Strider Hiryu (Japan set 2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_CAPCOM_CPS1,
	NULL, StridrjaRomInfo, StridrjaRomName, StriderInputInfo, StriderDIPInfo,
	StriderjaInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsUnsquad = {
	"unsquad", NULL, NULL, "1989",
	"U.N. Squadron (US)\0", NULL, "Daipro / Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, UnsquadRomInfo, UnsquadRomName, UnsquadInputInfo, UnsquadDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsArea88 = {
	"area88", "unsquad", NULL, "1989",
	"Area 88 (Japan)\0", NULL, "Daipro / Capcom", "CPS1",
	L"\u30A8\u30EA\u30A2\uFF18\uFF18 (Japan)\0Area 88 (Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Area88RomInfo, Area88RomName, UnsquadInputInfo, UnsquadDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsVarth = {
	"varth", NULL, NULL, "1992",
	"Varth - operation thunderstorm (920714 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_CAPCOM_CPS1,
	NULL, VarthRomInfo, VarthRomName, VarthInputInfo, VarthDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsVarthr1 = {
	"varthr1", "varth", NULL, "1992",
	"Varth - operation thunderstorm (920612 etc)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Varthr1RomInfo, Varthr1RomName, VarthInputInfo, VarthDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsVarthj = {
	"varthj", "varth", NULL, "1992",
	"Varth - operation thunderstorm (920714 Japan)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2 ,HARDWARE_CAPCOM_CPS1,
	NULL, VarthjRomInfo, VarthjRomName, VarthInputInfo, VarthDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsVarthu = {
	"varthu", "varth", NULL, "1992",
	"Varth - operation thunderstorm (920612 USA)\0", NULL, "Capcom (Romstar license)", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_CAPCOM_CPS1,
	NULL, VarthuRomInfo, VarthuRomName, VarthInputInfo, VarthDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 224, 384, 3, 4
};

struct BurnDriver BurnDrvCpsWillow = {
	"willow", NULL, NULL, "1989",
	"Willow (US)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, WillowRomInfo, WillowRomName, WillowInputInfo, WillowDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWillowje = {
	"willowje", "willow", NULL, "1989",
	"Willow (Japan, English)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, WillowjeRomInfo, WillowjeRomName, WillowInputInfo, WillowDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWillowj = {
	"willowj", "willow", NULL, "1989",
	"Willow (Japan, Japanese)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, WillowjRomInfo, WillowjRomName, WillowInputInfo, WillowDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWof = {
	"wof", NULL, NULL, "1992",
	"Warriors of Fate (921002 etc)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofRomInfo, WofRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWofa = {
	"wofa", "wof", NULL, "1992",
	"Sangokushi II (921005 Asia)\0", NULL, "Capcom", "CPS1 / QSound",
	L"\u4E09\u56FD\u5FD7II (Sangokushi 2 921005 Asia)\0Sangokushi II (921005 Asia)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofaRomInfo, WofaRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWofu = {
	"wofu", "wof", NULL, "1992",
	"Warriors of Fate (921031 USA)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofuRomInfo, WofuRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWofj = {
	"wofj", "wof", NULL, "1992",
	"Tenchi wo Kurau II - Sekiheki no Tatakai (921031 Japan)\0", NULL, "Capcom", "CPS1 / QSound",
	L"\u8EE2\u5730\u3092\u55B0\u3089\u3046II - \u8D64\u58C1\u306E\u6226\u3044 (Tenchi wo Kurau 2 921031 Japan)\0Tenchi wo Kurau II - Sekiheki no Tatakai (921031 Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofjRomInfo, WofjRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWofhfh = {
	"wofhfh", "wof", NULL, "1992",
	"Sangokushi II: Huo Fenghuang (Chinese bootleg)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, WofhfhRomInfo, WofhfhRomName, WofhfhInputInfo, WofhfhDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofb = {
	"wofb", "wof", NULL, "1992",
	"Warriors of Fate (bootleg)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofbRomInfo, WofbRomName, WofInputInfo, WofDIPInfo,
	WofbInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsWof3js = {
	"wof3js", "wof", NULL, "1992",
	"Sangokushi II: San Jian Sheng (Chinese bootleg)\0", NULL, "bootleg", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1,
	NULL, Wof3jsRomInfo, Wof3jsRomName, WofhfhInputInfo, WofhfhDIPInfo,
	Wof3jsInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWof3sj = {
	"wof3sj", "wof", NULL, "1992",
	"Sangokushi II: San Sheng Jian (Chinese bootleg set 1)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, Wof3sjRomInfo, Wof3sjRomName, WofInputInfo, WofDIPInfo,
	Wof3sjInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWof3sja = {
	"wof3sja", "wof", NULL, "1992",
	"Sangokushi II: San Sheng Jian (Chinese bootleg set 2)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, Wof3sjaRomInfo, Wof3sjaRomName, WofInputInfo, WofDIPInfo,
	Wof3sjaInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofaha = {
	"wofaha", "wof", NULL, "1992",
	"Sangokushi II (921005 Asia, bootleg)\0", NULL, "bootleg", "CPS1 / QSound",
	L"\u4E09\u56FD\u5FD7II (Sangokushi 2 921005 Asia bootleg)\0Sangokushi II (921005 Asia bootleg)\0", NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofahaRomInfo, WofahaRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofh = {
	"wofh", "wof", NULL, "1992",
	"Sangokushi II: Sanguo Yingxiong Zhuan (Chinese bootleg set 1)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofhRomInfo, WofhRomName, WofInputInfo, WofDIPInfo,
	WofhInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofha = {
	"wofha", "wof", NULL, "1992",
	"Sangokushi II: Sanguo Yingxiong Zhuan (Chinese bootleg set 2)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofhaRomInfo, WofhaRomName, WofInputInfo, WofDIPInfo,
	WofhaInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofsj = {
	"wofsj", "wof", NULL, "1992",
	"Sangokushi II: Sheng Jian Sanguo (Chinese bootleg set 1)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofsjRomInfo, WofsjRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofsja = {
	"wofsja", "wof", NULL, "1992",
	"Sangokushi II: Sheng Jian Sanguo (Chinese bootleg set 2)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofsjaRomInfo, WofsjaRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriverD BurnDrvCpsWofsjb = {
	"wofsjb", "wof", NULL, "1992",
	"Sangokushi II: Sheng Jian Sanguo (Chinese bootleg set 3)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofsjbRomInfo, WofsjbRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

struct BurnDriver BurnDrvCpsCps1frog = {
	"cps1frog", NULL, NULL, "2006",
	"Frog Feast (CPS-1)\0", NULL, "Rastersoft (Homebrew)", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Cps1frogRomInfo, Cps1frogRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

#if defined INCLUDE_HACKS

// Not included games
/*
// CPS-1 Demo
// Homebrew demo
static struct BurnRomInfo Cps1demoRomDesc[] = {
	{ "cd30-36.bin",   0x020000, 0x5eb617d6, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cd35-42.bin",   0x020000, 0x1f1c0a62, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "cd31-37.bin",   0x020000, 0x7ee8cdcd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // sound rom from cps1frog ?
	{ "cd36-43.bin",   0x020000, 0x7ee8cdcd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // sound rom from cps1frog ?
	{ "ff-32m.8h",     0x080000, 0xc747696e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd05-05m.bin",  0x080000, 0x35dc6153, BRF_GRA | CPS1_TILES },
	{ "cd07-07m.bin",  0x080000, 0xbaf08a38, BRF_GRA | CPS1_TILES },
	{ "cd01-01m.bin",  0x080000, 0x3ad8e790, BRF_GRA | CPS1_TILES },
	{ "cd03-03m.bin",  0x080000, 0xfb5186f3, BRF_GRA | CPS1_TILES },

	{ "ff_09.12b",     0x010000, 0xb8367eb5, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "ff_18.11c",     0x020000, 0x375c66e7, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "ff_19.12c",     0x020000, 0x1ef137f9, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Cps1demo)
STD_ROM_FN(Cps1demo)

struct BurnDriver BurnDrvCpsCps1demo = {
	"cps1demo", NULL, NULL, "2000",
	"CPS-1 Demo\0", NULL, "Charles Doty", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Cps1demoRomInfo, Cps1demoRomName, FfightInputInfo, FfightDIPInfo,
	DrvInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Cadillacs and Dinosaurs (hack???)
// very little difference between this and the etc version
static struct BurnRomInfo DinoehRomDesc[] = {
	{ "dinoeh.23",     0x080000, 0xa6b88364, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "cde_22a.rom",   0x080000, 0x9278aa12, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "dinoeh.21",     0x080000, 0xb89a0548, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd_gfx01.rom",  0x080000, 0x8da4f917, BRF_GRA | CPS1_TILES },
	{ "cd_gfx03.rom",  0x080000, 0x6c40f603, BRF_GRA | CPS1_TILES },
	{ "cd_gfx02.rom",  0x080000, 0x09c8fc2d, BRF_GRA | CPS1_TILES },
	{ "cd_gfx04.rom",  0x080000, 0x637ff38f, BRF_GRA | CPS1_TILES },
	{ "cd_gfx05.rom",  0x080000, 0x470befee, BRF_GRA | CPS1_TILES },
	{ "cd_gfx07.rom",  0x080000, 0x22bfb7a3, BRF_GRA | CPS1_TILES },
	{ "cd_gfx06.rom",  0x080000, 0xe7599ac4, BRF_GRA | CPS1_TILES },
	{ "cd_gfx08.rom",  0x080000, 0x211b4b15, BRF_GRA | CPS1_TILES },

	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Dinoeh)
STD_ROM_FN(Dinoeh)

struct BurnDriver BurnDrvCpsDinoeh = {
	"dinoeh", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs (hack?)\0", NULL, "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinoehRomInfo, DinoehRomName, DinoInputInfo, DinoDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Pnickies (USA)
// Seems like a region hack
static struct BurnRomInfo PnickuRomDesc[] = {
	{ "pnij36.bin",    0x020000, 0x2d4ffb2b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "pniu42.bin",    0x020000, 0x22d20227, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "pnij09.bin",    0x020000, 0x48177b0a, BRF_GRA | CPS1_TILES },
	{ "pnij01.bin",    0x020000, 0x01a0f311, BRF_GRA | CPS1_TILES },
	{ "pnij13.bin",    0x020000, 0x406451b0, BRF_GRA | CPS1_TILES },
	{ "pnij05.bin",    0x020000, 0x8c515dc0, BRF_GRA | CPS1_TILES },
	{ "pnij26.bin",    0x020000, 0xe2af981e, BRF_GRA | CPS1_TILES },
	{ "pnij18.bin",    0x020000, 0xf17a0e56, BRF_GRA | CPS1_TILES },
	{ "pnij38.bin",    0x020000, 0xeb75bd8c, BRF_GRA | CPS1_TILES },
	{ "pnij32.bin",    0x020000, 0x84560bef, BRF_GRA | CPS1_TILES },
	{ "pnij10.bin",    0x020000, 0xc2acc171, BRF_GRA | CPS1_TILES },
	{ "pnij02.bin",    0x020000, 0x0e21fc33, BRF_GRA | CPS1_TILES },
	{ "pnij14.bin",    0x020000, 0x7fe59b19, BRF_GRA | CPS1_TILES },
	{ "pnij06.bin",    0x020000, 0x79f4bfe3, BRF_GRA | CPS1_TILES },
	{ "pnij27.bin",    0x020000, 0x83d5cb0e, BRF_GRA | CPS1_TILES },
	{ "pnij19.bin",    0x020000, 0xaf08b230, BRF_GRA | CPS1_TILES },
	{ "pnij39.bin",    0x020000, 0x70fbe579, BRF_GRA | CPS1_TILES },
	{ "pnij33.bin",    0x020000, 0x3ed2c680, BRF_GRA | CPS1_TILES },

	{ "pnij17.bin",    0x010000, 0xe86f787a, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "pnij24.bin",    0x020000, 0x5092257d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "pnij25.bin",    0x020000, 0x22109aaa, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Pnicku)
STD_ROM_FN(Pnicku)

struct BurnDriver BurnDrvCpsPnicku = {
	"pnicku", "pnickj", NULL, "1994",
	"Pnickies (940608 USA)\0", NULL, "Compile (Capcom license)", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPS1,
	NULL, PnickuRomInfo, PnickuRomName, PnickjInputInfo, PnickjDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Saturday Night Slam Masters (Hispanic)
// Seems to be just a region hack
static struct BurnRomInfo SlammashRomDesc[] = {
	{ "mbh_23e.rom",   0x080000, 0xb0220f4a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_24b.rom",   0x020000, 0x95d5e729, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_28b.rom",   0x020000, 0xb1c7cbcb, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_25b.rom",   0x020000, 0xa50d3fd4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },	
	{ "mbe_29b.rom",   0x020000, 0x08e32e56, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbe_21a.rom",   0x080000, 0xd5007b05, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbe_20a.rom",   0x080000, 0xaeb557b0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_qa.rom",     0x020000, 0xe21a03c4, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Slammash)
STD_ROM_FN(Slammash)

struct BurnDriver BurnDrvCpsSlammash = {
	"slammash", "slammast", NULL, "1993",
	"Saturday Night Slam Masters (Slam Masters 930713 Hispanic)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, SlammashRomInfo, SlammashRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Muscle Bomber Duo (Japan E)
// Seems to be just a region hack
static struct BurnRomInfo MbombdjeRomDesc[] = {
	{ "mbd_26.bin",    0x020000, 0x72b7451c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbdje_30.rom",  0x020000, 0x61b09fb1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_27.bin",    0x020000, 0x4086f534, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_31.bin",    0x020000, 0x085f47f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_24.bin",    0x020000, 0xc20895a5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_28.bin",    0x020000, 0x2618d5e1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_25.bin",    0x020000, 0x9bdb6b11, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_29.bin",    0x020000, 0x3f52d5e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_21.bin",    0x080000, 0x690c026a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbd_20.bin",    0x080000, 0xb8b2139b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_q.bin",      0x020000, 0xd6fa76d1, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbombdje)
STD_ROM_FN(Mbombdje)

struct BurnDriver BurnDrvCpsMbombdje = {
	"mbombdje", "mbombrd", NULL, "1993",
	"Muscle Bomber Duo - heat up warriors (931206 Japan E)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbombdjeRomInfo, MbombdjeRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Muscle Bomber Duo (Asia)
// Seems to be just a region hack
static struct BurnRomInfo MbombrdaRomDesc[] = {
	{ "mbd_26.bin",    0x020000, 0x72b7451c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbda_30.rom",   0x020000, 0x42eaa9da, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_27.bin",    0x020000, 0x4086f534, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_31.bin",    0x020000, 0x085f47f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_24.bin",    0x020000, 0xc20895a5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_28.bin",    0x020000, 0x2618d5e1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_25.bin",    0x020000, 0x9bdb6b11, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_29.bin",    0x020000, 0x3f52d5e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_21.bin",    0x080000, 0x690c026a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbd_20.bin",    0x080000, 0xb8b2139b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_q.bin",      0x020000, 0xd6fa76d1, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbombrda)
STD_ROM_FN(Mbombrda)

struct BurnDriver BurnDrvCpsMbombrda = {
	"mbombrda", "mbombrd", NULL, "1993",
	"Muscle Bomber Duo - ultimate team battle (931206 Asia)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbombrdaRomInfo, MbombrdaRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Muscle Bomber Duo (Hispanic)
// Seems to be just a region hack
static struct BurnRomInfo MbombrdhRomDesc[] = {
	{ "mbd_26.bin",    0x020000, 0x72b7451c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbdh_30.rom",   0x020000, 0x836cea7d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_27.bin",    0x020000, 0x4086f534, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_31.bin",    0x020000, 0x085f47f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_24.bin",    0x020000, 0xc20895a5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_28.bin",    0x020000, 0x2618d5e1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_25.bin",    0x020000, 0x9bdb6b11, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_29.bin",    0x020000, 0x3f52d5e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_21.bin",    0x080000, 0x690c026a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbd_20.bin",    0x080000, 0xb8b2139b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_q.bin",      0x020000, 0xd6fa76d1, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbombrdh)
STD_ROM_FN(Mbombrdh)

struct BurnDriver BurnDrvCpsMbombrdh = {
	"mbombrdh", "mbombrd", NULL, "1993",
	"Muscle Bomber Duo - ultimate team battle (931206 Hispanic)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbombrdhRomInfo, MbombrdhRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Muscle Bomber Duo (USA)
// Seems to be just a region hack
static struct BurnRomInfo MbombrduRomDesc[] = {
	{ "mbd_26.bin",    0x020000, 0x72b7451c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbdu_30.rom",   0x020000, 0x5c234403, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_27.bin",    0x020000, 0x4086f534, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_31.bin",    0x020000, 0x085f47f0, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_24.bin",    0x020000, 0xc20895a5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_28.bin",    0x020000, 0x2618d5e1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_25.bin",    0x020000, 0x9bdb6b11, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_29.bin",    0x020000, 0x3f52d5e5, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "mbd_21.bin",    0x080000, 0x690c026a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "mbd_20.bin",    0x080000, 0xb8b2139b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "mb_gfx01.rom",  0x080000, 0x41468e06, BRF_GRA | CPS1_TILES },
	{ "mb_gfx03.rom",  0x080000, 0xf453aa9e, BRF_GRA | CPS1_TILES },
	{ "mb_gfx02.rom",  0x080000, 0x2ffbfea8, BRF_GRA | CPS1_TILES },
	{ "mb_gfx04.rom",  0x080000, 0x1eb9841d, BRF_GRA | CPS1_TILES },
	{ "mb_05.bin",     0x080000, 0x506b9dc9, BRF_GRA | CPS1_TILES },
	{ "mb_07.bin",     0x080000, 0xaff8c2fb, BRF_GRA | CPS1_TILES },
	{ "mb_06.bin",     0x080000, 0xb76c70e9, BRF_GRA | CPS1_TILES },
	{ "mb_08.bin",     0x080000, 0xe60c9556, BRF_GRA | CPS1_TILES },
	{ "mb_10.bin",     0x080000, 0x97976ff5, BRF_GRA | CPS1_TILES },
	{ "mb_12.bin",     0x080000, 0xb350a840, BRF_GRA | CPS1_TILES },
	{ "mb_11.bin",     0x080000, 0x8fb94743, BRF_GRA | CPS1_TILES },
	{ "mb_13.bin",     0x080000, 0xda810d5f, BRF_GRA | CPS1_TILES },

	{ "mb_q.bin",      0x020000, 0xd6fa76d1, BRF_ESS | BRF_PRG | CPS1_Z80_PROGRAM },

	{ "mb_q1.bin",     0x080000, 0x0630c3ce, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q2.bin",     0x080000, 0x354f9c21, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q3.bin",     0x080000, 0x7838487c, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q4.bin",     0x080000, 0xab66e087, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q5.bin",     0x080000, 0xc789fef2, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q6.bin",     0x080000, 0xecb81b61, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q7.bin",     0x080000, 0x041e49ba, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "mb_q8.bin",     0x080000, 0x59fe702a, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Mbombrdu)
STD_ROM_FN(Mbombrdu)

struct BurnDriver BurnDrvCpsMbombrdu = {
	"mbombrdu", "mbombrd", NULL, "1993",
	"Muscle Bomber Duo - ultimate team battle (931206 USA)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_CAPCOM_CPS1,
	NULL, MbombrduRomInfo, MbombrduRomName, SlammastInputInfo, SlammastDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Warriors of Fate (Asia bootleg)
// Very similar to wofa
static struct BurnRomInfo WofahRomDesc[] = {
	{ "htk2a_23b.rom", 0x080000, 0x1b17fc85, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2a_22b.rom",  0x080000, 0x900ad4cd, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5.rom",  0x080000, 0x291f0f0b, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7.rom",  0x080000, 0x3edeb949, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6.rom",  0x080000, 0x1abd14d6, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8.rom",  0x080000, 0xb27948e3, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofah)
STD_ROM_FN(Wofah)

struct BurnDriver BurnDrvCpsWofah = {
	"wofah", "wof", NULL, "1992",
	"Sangokushi II (921005 Asia, bootleg set 2)\0", NULL, "bootleg", "CPS1 / QSound",
	L"\u4E09\u56FD\u5FD7II (Sangokushi 2 921005 Asia bootleg set 2)\0Sangokushi II (921005 Asia bootleg set 2)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofahRomInfo, WofahRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Warriors of Fate (Japan bootleg)
// Very similar to wofj
static struct BurnRomInfo WofjhRomDesc[] = {
	{ "tk2j23h.bin",   0x080000, 0x84d38575, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2j22c.bin",   0x080000, 0xb74b09ac, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk205.bin",     0x080000, 0xe4a44d53, BRF_GRA | CPS1_TILES },
	{ "tk206.bin",     0x080000, 0x58066ba8, BRF_GRA | CPS1_TILES },
	{ "tk207.bin",     0x080000, 0xd706568e, BRF_GRA | CPS1_TILES },
	{ "tk208.bin",     0x080000, 0xd4a19a02, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Wofjh)
STD_ROM_FN(Wofjh)

struct BurnDriver BurnDrvCpsWofjh = {
	"wofjh", "wof", NULL, "1992",
	"Tenchi wo Kurau II - Sekiheki no Tatakai (921031 Japan bootleg)\0", NULL, "bootleg", "CPS1 / QSound",
	L"\u8EE2\u5730\u3092\u55B0\u3089\u3046II - \u8D64\u58C1\u306E\u6226\u3044 (Tenchi wo Kurau 2 921031 Japan bootleg)\0Tenchi wo Kurau II - Sekiheki no Tatakai (921031 Japan bootleg)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WofjhRomInfo, WofjhRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Warriors of Fate (French Translation)
// Very similar to wof
static struct BurnRomInfo WoffrRomDesc[] = {
	{ "tk2e_23bfr.rom",0x080000, 0xe006e81e, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "tk2e_22b.rom",  0x080000, 0x479b3f24, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "tk2_gfx1.rom",  0x080000, 0x0d9cb9bf, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx3.rom",  0x080000, 0x45227027, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx2.rom",  0x080000, 0xc5ca2460, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx4.rom",  0x080000, 0xe349551c, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx5fr.rom",0x080000, 0x1b6bc2b2, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx7fr.rom",0x080000, 0x3f1e5334, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx6fr.rom",0x080000, 0x28c9d96f, BRF_GRA | CPS1_TILES },
	{ "tk2_gfx8fr.rom",0x080000, 0xb3b9a29e, BRF_GRA | CPS1_TILES },

	{ "tk2_qa.rom",    0x020000, 0xc9183a0d, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "tk2_q1.rom",    0x080000, 0x611268cf, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q2.rom",    0x080000, 0x20f55ca9, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q3.rom",    0x080000, 0xbfcf6f52, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "tk2_q4.rom",    0x080000, 0x36642e88, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Woffr)
STD_ROM_FN(Woffr)

struct BurnDriver BurnDrvCpsWoffr = {
	"woffr", "wof", NULL, "1992",
	"Warriors of Fate (French Translation)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, WoffrRomInfo, WoffrRomName, WofInputInfo, WofDIPInfo,
	TwelveMhzInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Street Fighter Alpha (CPS-Changer)
// Seems to be just a region hack
static struct BurnRomInfo SfachRomDesc[] = {
	{ "sfach23",       0x080000, 0x02a1a853, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfza22",        0x080000, 0x8d9b2480, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfzch21",       0x080000, 0x5435225d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfza20",        0x080000, 0x806e8f38, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "sfz01",         0x080000, 0x0dd53e62, BRF_GRA | CPS1_TILES },
	{ "sfz02",         0x080000, 0x94c31e3f, BRF_GRA | CPS1_TILES },
	{ "sfz03",         0x080000, 0x9584ac85, BRF_GRA | CPS1_TILES },
	{ "sfz04",         0x080000, 0xb983624c, BRF_GRA | CPS1_TILES },
	{ "sfz05",         0x080000, 0x2b47b645, BRF_GRA | CPS1_TILES },
	{ "sfz06",         0x080000, 0x74fd9fb1, BRF_GRA | CPS1_TILES },
	{ "sfz07",         0x080000, 0xbb2c734d, BRF_GRA | CPS1_TILES },
	{ "sfz08",         0x080000, 0x454f7868, BRF_GRA | CPS1_TILES },
	{ "sfz10",         0x080000, 0x2a7d675e, BRF_GRA | CPS1_TILES },
	{ "sfz11",         0x080000, 0xe35546c8, BRF_GRA | CPS1_TILES },
	{ "sfz12",         0x080000, 0xf122693a, BRF_GRA | CPS1_TILES },
	{ "sfz13",         0x080000, 0x7cf942c8, BRF_GRA | CPS1_TILES },
	{ "sfz14",         0x080000, 0x09038c81, BRF_GRA | CPS1_TILES },
	{ "sfz15",         0x080000, 0x1aa17391, BRF_GRA | CPS1_TILES },
	{ "sfz16",         0x080000, 0x19a5abd6, BRF_GRA | CPS1_TILES },
	{ "sfz17",         0x080000, 0x248b3b73, BRF_GRA | CPS1_TILES },

	{ "sfz09",         0x010000, 0xc772628b, BRF_PRG | CPS1_Z80_PROGRAM },
	
	{ "sfz18",         0x020000, 0x61022b2d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sfz19",         0x020000, 0x3b5886d5, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sfach)
STD_ROM_FN(Sfach)

struct BurnDriver BurnDrvCpsSfach = {
	"sfach", "sfzch", NULL, "1995",
	"Street Fighter Alpha (950727 CPS Changer)\0", NULL, "Capcom", "CPS Changer",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPSCHANGER,
	NULL, SfachRomInfo, SfachRomName, SfzchInputInfo, NULL,
	SfzchInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Street Fighter Zero Brazil (CPS-Changer)
// Seems to be just a region hack
static struct BurnRomInfo SfzbchRomDesc[] = {
	{ "sfbch23",       0x080000, 0x53699f68, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfza22",        0x080000, 0x8d9b2480, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfzch21",       0x080000, 0x5435225d, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "sfza20",        0x080000, 0x806e8f38, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "sfz01",         0x080000, 0x0dd53e62, BRF_GRA | CPS1_TILES },
	{ "sfz02",         0x080000, 0x94c31e3f, BRF_GRA | CPS1_TILES },
	{ "sfz03",         0x080000, 0x9584ac85, BRF_GRA | CPS1_TILES },
	{ "sfz04",         0x080000, 0xb983624c, BRF_GRA | CPS1_TILES },
	{ "sfz05",         0x080000, 0x2b47b645, BRF_GRA | CPS1_TILES },
	{ "sfz06",         0x080000, 0x74fd9fb1, BRF_GRA | CPS1_TILES },
	{ "sfz07",         0x080000, 0xbb2c734d, BRF_GRA | CPS1_TILES },
	{ "sfz08",         0x080000, 0x454f7868, BRF_GRA | CPS1_TILES },
	{ "sfz10",         0x080000, 0x2a7d675e, BRF_GRA | CPS1_TILES },
	{ "sfz11",         0x080000, 0xe35546c8, BRF_GRA | CPS1_TILES },
	{ "sfz12",         0x080000, 0xf122693a, BRF_GRA | CPS1_TILES },
	{ "sfz13",         0x080000, 0x7cf942c8, BRF_GRA | CPS1_TILES },
	{ "sfz14",         0x080000, 0x09038c81, BRF_GRA | CPS1_TILES },
	{ "sfz15",         0x080000, 0x1aa17391, BRF_GRA | CPS1_TILES },
	{ "sfz16",         0x080000, 0x19a5abd6, BRF_GRA | CPS1_TILES },
	{ "sfz17",         0x080000, 0x248b3b73, BRF_GRA | CPS1_TILES },

	{ "sfz09",         0x010000, 0xc772628b, BRF_PRG | CPS1_Z80_PROGRAM },
	
	{ "sfz18",         0x020000, 0x61022b2d, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "sfz19",         0x020000, 0x3b5886d5, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sfzbch)
STD_ROM_FN(Sfzbch)

struct BurnDriver BurnDrvCpsSfzbch = {
	"sfzbch", "sfzch", NULL, "1995",
	"Street Fighter Zero (950727 Brazil CPS Changer)\0", NULL, "Capcom", "CPS Changer",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_CAPCOM_CPSCHANGER,
	NULL, SfzbchRomInfo, SfzbchRomName, SfzchInputInfo, NULL,
	SfzchInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};
*/
#endif

#if defined INCLUDE_DUPLICATES
/*
// Cadillacs and Dinosaurs (bootleg)
// this is the same as dinopic (once the pic program is added)
static struct BurnRomInfo DinobRomDesc[] = {
	{ "5.bin",         0x080000, 0x96dfcbf1, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "3.bin",         0x080000, 0x13dfeb08, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "7.bin",         0x080000, 0x6133f349, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "2.bin",         0x080000, 0x0e4058ba, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	
	{ "4.bin",         0x080000, 0xf3c2c98d, BRF_GRA | CPS1_TILES },
	{ "8.bin",         0x080000, 0xd574befc, BRF_GRA | CPS1_TILES },
	{ "9.bin",         0x080000, 0x55ef0adc, BRF_GRA | CPS1_TILES },
	{ "6.bin",         0x080000, 0xcc0805fc, BRF_GRA | CPS1_TILES },
	{ "13.bin",        0x080000, 0x1371f714, BRF_GRA | CPS1_TILES },
	{ "12.bin",        0x080000, 0xb284c4a7, BRF_GRA | CPS1_TILES },
	{ "11.bin",        0x080000, 0xb7ad3394, BRF_GRA | CPS1_TILES },
	{ "10.bin",        0x080000, 0x88847705, BRF_GRA | CPS1_TILES },
	
	{ "pic16c57-rp",   0x002d4c, 0x5a6d393c, BRF_PRG | CPS1_PIC },

	{ "1.bin",         0x080000, 0x7d921309, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Dinob)
STD_ROM_FN(Dinob)

struct BurnDriverD BurnDrvCpsDinob = {
	"dinob", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs (identical to dinopic)\0", "Missing sprites, no sound", "Capcom", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, DinobRomInfo, DinobRomName, DinoInputInfo, DinoDIPInfo,
	DinopicInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Cadillacs and Dinosaurs Turbo (Asia Bootleg)
// this is the same as dinoha
static struct BurnRomInfo Kl2dRomDesc[] = {
	{ "kl2-r1.800",    0x100000, 0x4c70dca7, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },
	{ "kl2-l2.800",    0x100000, 0xc6ae7338, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_NO_BYTESWAP },

	{ "cd-a.160",      0x200000, 0x7e4f9fb3, BRF_GRA | CPS1_TILES },
	{ "cd-b.160",      0x200000, 0x89532d85, BRF_GRA | CPS1_TILES },
	
	{ "cd_q.rom",      0x020000, 0x605fdb0b, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "cd_q1.rom",     0x080000, 0x60927775, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q2.rom",     0x080000, 0x770f4c47, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q3.rom",     0x080000, 0x2f273ffc, BRF_SND | CPS1_QSOUND_SAMPLES },
	{ "cd_q4.rom",     0x080000, 0x2c67821d, BRF_SND | CPS1_QSOUND_SAMPLES },
};

STD_ROM_PICK(Kl2d)
STD_ROM_FN(Kl2d)

struct BurnDriver BurnDrvCpsKl2d = {
	"kl2d", "dino", NULL, "1993",
	"Cadillacs and Dinosaurs Turbo (identical to dinoha)\0", NULL, "bootleg", "CPS1 / QSound",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_CAPCOM_CPS1_QSOUND,
	NULL, Kl2dRomInfo, Kl2dRomName, DinoInputInfo, DinoDIPInfo,
	DinohaInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Street Fighter 2 Champion Edition (bootleg, Slay the Dragon)
// this is the same as sf2tlona
static struct BurnRomInfo Sf2tloncRomDesc[] = {
	{ "u222-f83.040",  0x100000, 0x47468198, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical
	{ "u196-99d.040",  0x100000, 0x47d34191, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical
	{ "u221-59e.010",  0x040000, 0x5507bac9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical
	{ "u195-472.010",  0x040000, 0x7755a20a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2tlonc)
STD_ROM_FN(Sf2tlonc)

static int Sf2tloncInit()
{
	int nRet = 0;
	
	nRet = Sf2m4Init();
	
	memcpy(CpsRom + 0x100000, CpsRom + 0x200000, 0x80000);
        
	return nRet;
}

struct BurnDriver BurnDrvCpsSf2tlonc = {
	"sf2tlonc", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (identical to sf2tlona)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2tloncRomInfo, Sf2tloncRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2tloncInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Street Fighter 2 Champion Edition (bootleg, turyu)
// this is the same as sf2tlona
static struct BurnRomInfo Sf2turyuRomDesc[] = {
	{"u222-f83.040",  0x0100000, 0x47468198, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical
	{"u196-99d.040",  0x0100000, 0x47d34191, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical
	{"u221-59e.010",  0x0040000, 0x5507bac9, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical
	{"u195-472.010",  0x0040000, 0x7755a20a, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP }, // first and second halves are identical

	{ "s92_01.bin",   0x0080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",   0x0080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",   0x0080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",   0x0080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",   0x0080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",   0x0080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",   0x0080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",   0x0080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",   0x0080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",   0x0080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",   0x0080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",   0x0080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2turyu)
STD_ROM_FN(Sf2turyu)

struct BurnDriver BurnDrvCpsSf2turyu = {
	"sf2turyu", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (identical to sf2tlona)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2turyuRomInfo, Sf2turyuRomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2tloncInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Street Fighter 2 Champion Edition (bootleg, turyu set 2)
// this is the same as sf2tlonb
static struct BurnRomInfo Sf2tury2RomDesc[] = {
	{ "t14m.1",        0x080000, 0xafc7bd18, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "t14m.2",        0x080000, 0x882cd1c4, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u221.1m",       0x020000, 0xd1707134, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },
	{ "u195.1m",       0x020000, 0xcd1d5666, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s92_10.bin",    0x080000, 0x960687d5, BRF_GRA | CPS1_TILES },
	{ "s92_11.bin",    0x080000, 0x978ecd18, BRF_GRA | CPS1_TILES },
	{ "s92_12.bin",    0x080000, 0xd6ec9a0a, BRF_GRA | CPS1_TILES },
	{ "s92_13.bin",    0x080000, 0xed2c67f6, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",    0x010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",    0x020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",    0x020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2tury2)
STD_ROM_FN(Sf2tury2)

struct BurnDriver BurnDrvCpsSf2tury2 = {
	"sf2tury2", "sf2ce", NULL, "1992",
	"Street Fighter II' - champion edition (identical to sf2tlonb)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2tury2RomInfo, Sf2tury2RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2m4Init, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};

// Street Fighter 2 Champion Edition (bootleg, ce13)
// same as sf2m13
static struct BurnRomInfo Sf2ce13RomDesc[] = {
	{ "222-040.13",   0x0080000, 0xec6f5cb3, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "196-040.13",   0x0080000, 0x0e9ac52b, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "221-010.13",   0x0020000, 0x8226c11c, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },
	{ "195-010.13",   0x0020000, 0x924c6ce2, BRF_ESS | BRF_PRG | CPS1_68K_PROGRAM_BYTESWAP  },

	{ "s92_01.bin",    0x080000, 0x03b0d852, BRF_GRA | CPS1_TILES },
	{ "s92_02.bin",    0x080000, 0x840289ec, BRF_GRA | CPS1_TILES },
	{ "s92_03.bin",    0x080000, 0xcdb5f027, BRF_GRA | CPS1_TILES },
	{ "s92_04.bin",    0x080000, 0xe2799472, BRF_GRA | CPS1_TILES },
	{ "s92_05.bin",    0x080000, 0xba8a2761, BRF_GRA | CPS1_TILES },
	{ "s92_06.bin",    0x080000, 0xe584bfb5, BRF_GRA | CPS1_TILES },
	{ "s92_07.bin",    0x080000, 0x21e3f87d, BRF_GRA | CPS1_TILES },
	{ "s92_08.bin",    0x080000, 0xbefc47df, BRF_GRA | CPS1_TILES },
	{ "s2t_10.bin",    0x080000, 0x3c042686, BRF_GRA | CPS1_TILES },
	{ "s2t_11.bin",    0x080000, 0x8b7e7183, BRF_GRA | CPS1_TILES },
	{ "s2t_12.bin",    0x080000, 0x293c888c, BRF_GRA | CPS1_TILES },
	{ "s2t_13.bin",    0x080000, 0x842b35a4, BRF_GRA | CPS1_TILES },

	{ "s92_09.bin",   0x0010000, 0x08f6b60e, BRF_PRG | CPS1_Z80_PROGRAM },

	{ "s92_18.bin",   0x0020000, 0x7f162009, BRF_SND | CPS1_OKIM6295_SAMPLES },
	{ "s92_19.bin",   0x0020000, 0xbeade53f, BRF_SND | CPS1_OKIM6295_SAMPLES },
};

STD_ROM_PICK(Sf2ce13)
STD_ROM_FN(Sf2ce13)

struct BurnDriverD BurnDrvCpsSf2ce13 = {
	"sf2ce13", "sf2hf", NULL, "1992",
	"Street Fighter II' - champion edition (identical to sf2m13)\0", NULL, "Capcom", "CPS1",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_CAPCOM_CPS1,
	NULL, Sf2ce13RomInfo, Sf2ce13RomName, Sf2yycInputInfo, Sf2DIPInfo,
	Sf2ceInit, DrvExit, Cps1Frame, CpsRedraw, CpsAreaScan,
	0, NULL, NULL, NULL, &CpsRecalcPal, 384, 224, 4, 3
};
*/
#endif

#undef INCLUDE_HACKS
#undef INCLUDE_DUPLICATES

#undef CPS1_68K_PROGRAM_BYTESWAP
#undef CPS1_68K_PROGRAM_NO_BYTESWAP
#undef CPS1_Z80_PROGRAM
#undef CPS1_TILES
#undef CPS1_OKIM6295_SAMPLES
#undef CPS1_QSOUND_SAMPLES
#undef CPS1_PIC
