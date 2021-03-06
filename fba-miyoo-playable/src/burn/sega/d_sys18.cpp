#include "sys16.h"

/*====================================================
Input defs
====================================================*/

#define A(a, b, c, d) {a, b, (unsigned char*)(c), d}

static struct BurnInputInfo System18InputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },
	{"P1 Fire 3"         , BIT_DIGITAL  , System16InputPort1 + 2, "p1 fire 3" },

	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	{"P2 Fire 3"         , BIT_DIGITAL  , System16InputPort2 + 2, "p2 fire 3" },

	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Service 2"         , BIT_DIGITAL  , System16InputPort0 + 6 , "service2" },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(System18);

static struct BurnInputInfo AstormInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 7, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },
	{"Coin 3"            , BIT_DIGITAL  , System16InputPort0 + 0, "p3 coin"   },
	{"Start 3"           , BIT_DIGITAL  , System16InputPort0 + 6, "p3 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },
	{"P1 Fire 3"         , BIT_DIGITAL  , System16InputPort1 + 2, "p1 fire 3" },
	{"P1 Service"        , BIT_DIGITAL  , System16InputPort1 + 3, "p1 fire 4" },

	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	{"P2 Fire 3"         , BIT_DIGITAL  , System16InputPort2 + 2, "p2 fire 3" },
	{"P2 Service"        , BIT_DIGITAL  , System16InputPort2 + 3, "p2 fire 4" },
	
	{"P3 Up"             , BIT_DIGITAL  , System16InputPort3 + 5, "p3 up"     },
	{"P3 Down"           , BIT_DIGITAL  , System16InputPort3 + 4, "p3 down"   },
	{"P3 Left"           , BIT_DIGITAL  , System16InputPort3 + 7, "p3 left"   },
	{"P3 Right"          , BIT_DIGITAL  , System16InputPort3 + 6, "p3 right"  },
	{"P3 Fire 1"         , BIT_DIGITAL  , System16InputPort3 + 0, "p3 fire 1" },
	{"P3 Fire 2"         , BIT_DIGITAL  , System16InputPort3 + 1, "p3 fire 2" },
	{"P3 Fire 3"         , BIT_DIGITAL  , System16InputPort3 + 2, "p3 fire 3" },
	{"P3 Service"        , BIT_DIGITAL  , System16InputPort3 + 3, "p3 fire 4" },

	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Astorm);

static struct BurnInputInfo DdcrewInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },
	{"Coin 3"            , BIT_DIGITAL  , System16InputPort0 + 7, "p3 coin"   },
	{"Start 3"           , BIT_DIGITAL  , System16InputPort3 + 3, "p3 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },

	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	
	{"P3 Up"             , BIT_DIGITAL  , System16InputPort3 + 5, "p3 up"     },
	{"P3 Down"           , BIT_DIGITAL  , System16InputPort3 + 4, "p3 down"   },
	{"P3 Left"           , BIT_DIGITAL  , System16InputPort3 + 7, "p3 left"   },
	{"P3 Right"          , BIT_DIGITAL  , System16InputPort3 + 6, "p3 right"  },
	{"P3 Fire 1"         , BIT_DIGITAL  , System16InputPort3 + 0, "p3 fire 1" },
	{"P3 Fire 2"         , BIT_DIGITAL  , System16InputPort3 + 1, "p3 fire 2" },

	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Ddcrew);

static struct BurnInputInfo Ddcrew2InputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },

	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	
	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Ddcrew2);

static struct BurnInputInfo DdcrewuInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },
	{"Coin 3"            , BIT_DIGITAL  , System16InputPort5 + 2, "p3 coin"   },
	{"Start 3"           , BIT_DIGITAL  , System16InputPort5 + 0, "p3 start"  },
	{"Coin 4"            , BIT_DIGITAL  , System16InputPort5 + 3, "p4 coin"   },
	{"Start 4"           , BIT_DIGITAL  , System16InputPort5 + 1, "p4 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },

	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	
	{"P3 Up"             , BIT_DIGITAL  , System16InputPort3 + 5, "p3 up"     },
	{"P3 Down"           , BIT_DIGITAL  , System16InputPort3 + 4, "p3 down"   },
	{"P3 Left"           , BIT_DIGITAL  , System16InputPort3 + 7, "p3 left"   },
	{"P3 Right"          , BIT_DIGITAL  , System16InputPort3 + 6, "p3 right"  },
	{"P3 Fire 1"         , BIT_DIGITAL  , System16InputPort3 + 0, "p3 fire 1" },
	{"P3 Fire 2"         , BIT_DIGITAL  , System16InputPort3 + 1, "p3 fire 2" },
	
	{"P4 Up"             , BIT_DIGITAL  , System16InputPort4 + 5, "p4 up"     },
	{"P4 Down"           , BIT_DIGITAL  , System16InputPort4 + 4, "p4 down"   },
	{"P4 Left"           , BIT_DIGITAL  , System16InputPort4 + 7, "p4 left"   },
	{"P4 Right"          , BIT_DIGITAL  , System16InputPort4 + 6, "p4 right"  },
	{"P4 Fire 1"         , BIT_DIGITAL  , System16InputPort4 + 0, "p4 fire 1" },
	{"P4 Fire 2"         , BIT_DIGITAL  , System16InputPort4 + 1, "p4 fire 2" },

	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Ddcrewu);

static struct BurnInputInfo DesertbrInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },
	{"Coin 3"            , BIT_DIGITAL  , System16InputPort3 + 3, "p3 coin"   },
	{"Start 3"           , BIT_DIGITAL  , System16InputPort0 + 7, "p3 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },
	{"P1 Fire 3"         , BIT_DIGITAL  , System16InputPort1 + 2, "p1 fire 3" },

	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	{"P2 Fire 3"         , BIT_DIGITAL  , System16InputPort2 + 2, "p2 fire 3" },
	
	{"P3 Up"             , BIT_DIGITAL  , System16InputPort3 + 5, "p3 up"     },
	{"P3 Down"           , BIT_DIGITAL  , System16InputPort3 + 4, "p3 down"   },
	{"P3 Left"           , BIT_DIGITAL  , System16InputPort3 + 7, "p3 left"   },
	{"P3 Right"          , BIT_DIGITAL  , System16InputPort3 + 6, "p3 right"  },
	{"P3 Fire 1"         , BIT_DIGITAL  , System16InputPort3 + 0, "p3 fire 1" },
	{"P3 Fire 2"         , BIT_DIGITAL  , System16InputPort3 + 1, "p3 fire 2" },
	{"P3 Fire 3"         , BIT_DIGITAL  , System16InputPort3 + 2, "p3 fire 3" },

	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Desertbr);

static struct BurnInputInfo LghostInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 0, "p1 coin"    },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 1, "p2 coin"    },
	{"Coin 3"            , BIT_DIGITAL   , System16InputPort0 + 7, "p3 coin"    },

	A("P1 X-Axis"        , BIT_ANALOG_REL, &System16AnalogPort0,   "mouse x-axis"   ),
	A("P1 Y-Axis"        , BIT_ANALOG_REL, &System16AnalogPort1,   "mouse y-axis"   ),
	{"P1 Fire 1"         , BIT_DIGITAL   , System16InputPort1 + 0, "mouse button 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , System16InputPort1 + 1, "mouse button 2" },
	
	A("P2 X-Axis"        , BIT_ANALOG_REL, &System16AnalogPort2,   "p2 x-axis"  ),
	A("P2 Y-Axis"        , BIT_ANALOG_REL, &System16AnalogPort3,   "p2 y-axis"  ),
	{"P2 Fire 1"         , BIT_DIGITAL   , System16InputPort1 + 2, "p2 fire 1"  },
	{"P2 Fire 2"         , BIT_DIGITAL   , System16InputPort1 + 3, "p2 fire 2"  },
	
	A("P3 X-Axis"        , BIT_ANALOG_REL, &System16AnalogPort4,   "p3 x-axis"  ),
	A("P3 Y-Axis"        , BIT_ANALOG_REL, &System16AnalogPort5,   "p3 y-axis"  ),
	{"P3 Fire 1"         , BIT_DIGITAL   , System16InputPort1 + 7, "p3 fire 1"  },
	{"P3 Fire 2"         , BIT_DIGITAL   , System16InputPort1 + 6, "p3 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 4 , "service"   },
	{"Service 2"         , BIT_DIGITAL   , System16InputPort0 + 5 , "service2"  },
	{"Service 3"         , BIT_DIGITAL   , System16InputPort0 + 6 , "service3"  },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 2 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Lghost);

static struct BurnInputInfo MwalkInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System16InputPort0 + 5, "p2 start"  },
	{"Coin 3"            , BIT_DIGITAL  , System16InputPort0 + 7, "p3 coin"   },
	{"Start 3"           , BIT_DIGITAL  , System16InputPort3 + 3, "p3 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System16InputPort1 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System16InputPort1 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System16InputPort1 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System16InputPort1 + 0, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System16InputPort1 + 1, "p1 fire 2" },
	{"P1 Service"        , BIT_DIGITAL  , System16InputPort1 + 3, "service"   },
	
	{"P2 Up"             , BIT_DIGITAL  , System16InputPort2 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System16InputPort2 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System16InputPort2 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System16InputPort2 + 0, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System16InputPort2 + 1, "p2 fire 2" },
	{"P2 Service"        , BIT_DIGITAL  , System16InputPort2 + 3, "service2"  },
	
	{"P3 Up"             , BIT_DIGITAL  , System16InputPort3 + 5, "p3 up"     },
	{"P3 Down"           , BIT_DIGITAL  , System16InputPort3 + 4, "p3 down"   },
	{"P3 Left"           , BIT_DIGITAL  , System16InputPort3 + 7, "p3 left"   },
	{"P3 Right"          , BIT_DIGITAL  , System16InputPort3 + 6, "p3 right"  },
	{"P3 Fire 1"         , BIT_DIGITAL  , System16InputPort3 + 0, "p3 fire 1" },
	{"P3 Fire 2"         , BIT_DIGITAL  , System16InputPort3 + 1, "p3 fire 2" },
	{"P3 Service"        , BIT_DIGITAL  , System16InputPort0 + 6, "service3"  },
	
	{"Service"           , BIT_DIGITAL  , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL  , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL  , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH, System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH, System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Mwalk);

static struct BurnInputInfo WwallyInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL   , System16InputPort0 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL   , System16InputPort1 + 3, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL   , System16InputPort1 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL   , System16InputPort1 + 1, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL   , System16InputPort1 + 2, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL   , System16InputPort1 + 0, "p1 fire 1" },

	{"P2 Up"             , BIT_DIGITAL   , System16InputPort2 + 3, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL   , System16InputPort2 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL   , System16InputPort2 + 1, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL   , System16InputPort2 + 2, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL   , System16InputPort2 + 0, "p2 fire 1" },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 3 , "service"  },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 2 , "diag"     },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"    },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"      },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"      },
};

STDINPUTINFO(Wwally);

#undef A

/*====================================================
Dip defs
====================================================*/

#define SYSTEM18_COINAGE(dipval)								\
	{0   , 0xfe, 0   , 16  , "Coin A"                               },			\
	{dipval, 0x01, 0x0f, 0x07, "4 Coins 1 Credit"                   },			\
	{dipval, 0x01, 0x0f, 0x08, "3 Coins 1 Credit"                   },			\
	{dipval, 0x01, 0x0f, 0x09, "2 Coins 1 Credit"                   },			\
	{dipval, 0x01, 0x0f, 0x05, "2 Coins 1 Credit 5/3 6/4"           },			\
	{dipval, 0x01, 0x0f, 0x04, "2 Coins 1 Credit 4/3"               },			\
	{dipval, 0x01, 0x0f, 0x0f, "1 Coin  1 Credit"                   },			\
	{dipval, 0x01, 0x0f, 0x01, "1 Coin  1 Credit 2/3"               },			\
	{dipval, 0x01, 0x0f, 0x02, "1 Coin  1 Credit 4/5"               },			\
	{dipval, 0x01, 0x0f, 0x03, "1 Coin  1 Credit 5/6"               },			\
	{dipval, 0x01, 0x0f, 0x06, "2 Coins 3 Credits"                  },			\
	{dipval, 0x01, 0x0f, 0x0e, "1 Coin  2 Credits"                  },			\
	{dipval, 0x01, 0x0f, 0x0d, "1 Coin  3 Credits"                  },			\
	{dipval, 0x01, 0x0f, 0x0c, "1 Coin  4 Credits"                  },			\
	{dipval, 0x01, 0x0f, 0x0b, "1 Coin  5 Credits"                  },			\
	{dipval, 0x01, 0x0f, 0x0a, "1 Coin  6 Credits"                  },			\
	{dipval, 0x01, 0x0f, 0x00, "Free Play (if coin B too) or 1C/1C" },			\
												\
	{0   , 0xfe, 0   , 16  , "Coin B"                               },			\
	{dipval, 0x01, 0xf0, 0x70, "4 Coins 1 Credit"                   },			\
	{dipval, 0x01, 0xf0, 0x80, "3 Coins 1 Credit"                   },			\
	{dipval, 0x01, 0xf0, 0x90, "2 Coins 1 Credit"                   },			\
	{dipval, 0x01, 0xf0, 0x50, "2 Coins 1 Credit 5/3 6/4"           },			\
	{dipval, 0x01, 0xf0, 0x40, "2 Coins 1 Credit 4/3"               },			\
	{dipval, 0x01, 0xf0, 0xf0, "1 Coin  1 Credit"                   },			\
	{dipval, 0x01, 0xf0, 0x10, "1 Coin  1 Credit 2/3"               },			\
	{dipval, 0x01, 0xf0, 0x20, "1 Coin  1 Credit 4/5"               },			\
	{dipval, 0x01, 0xf0, 0x30, "1 Coin  1 Credit 5/6"               },			\
	{dipval, 0x01, 0xf0, 0x60, "2 Coins 3 Credits"                  },			\
	{dipval, 0x01, 0xf0, 0xe0, "1 Coin  2 Credits"                  },			\
	{dipval, 0x01, 0xf0, 0xd0, "1 Coin  3 Credits"                  },			\
	{dipval, 0x01, 0xf0, 0xc0, "1 Coin  4 Credits"                  },			\
	{dipval, 0x01, 0xf0, 0xb0, "1 Coin  5 Credits"                  },			\
	{dipval, 0x01, 0xf0, 0xa0, "1 Coin  6 Credits"                  },			\
	{dipval, 0x01, 0xf0, 0x00, "Free Play (if coin A too) or 1C/1C" },

static struct BurnDIPInfo AstormDIPList[]=
{
	// Default Values
	{0x21, 0xff, 0xff, 0xff, NULL                                 },
	{0x22, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x21)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x22, 0x01, 0x01, 0x01, "Off"                                },
	{0x22, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x22, 0x01, 0x02, 0x02, "Off"                                },
	{0x22, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 8   , "Difficulty"                         },
	{0x22, 0x01, 0x1c, 0x04, "Easiest"                            },
	{0x22, 0x01, 0x1c, 0x08, "Easier"                             },
	{0x22, 0x01, 0x1c, 0x0c, "Easy"                               },
	{0x22, 0x01, 0x1c, 0x1c, "Normal"                             },
	{0x22, 0x01, 0x1c, 0x10, "Hard"                               },
	{0x22, 0x01, 0x1c, 0x14, "Harder"                             },
	{0x22, 0x01, 0x1c, 0x18, "Hardest"                            },
	{0x22, 0x01, 0x1c, 0x00, "Special"                            },
	
	{0   , 0xfe, 0   , 2   , "Coin Chutes"                        },
	{0x22, 0x01, 0x20, 0x00, "1"                                  },
	{0x22, 0x01, 0x20, 0x20, "3"                                  },	
};

STDDIPINFO(Astorm);

static struct BurnDIPInfo Astorm2pDIPList[]=
{
	// Default Values
	{0x16, 0xff, 0xff, 0xff, NULL                                 },
	{0x17, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x16)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x17, 0x01, 0x01, 0x01, "Off"                                },
	{0x17, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x17, 0x01, 0x02, 0x02, "Off"                                },
	{0x17, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 8   , "Difficulty"                         },
	{0x17, 0x01, 0x1c, 0x04, "Easiest"                            },
	{0x17, 0x01, 0x1c, 0x08, "Easier"                             },
	{0x17, 0x01, 0x1c, 0x0c, "Easy"                               },
	{0x17, 0x01, 0x1c, 0x1c, "Normal"                             },
	{0x17, 0x01, 0x1c, 0x10, "Hard"                               },
	{0x17, 0x01, 0x1c, 0x14, "Harder"                             },
	{0x17, 0x01, 0x1c, 0x18, "Hardest"                            },
	{0x17, 0x01, 0x1c, 0x00, "Special"                            },
	
	{0   , 0xfe, 0   , 2   , "Coin Chutes"                        },
	{0x17, 0x01, 0x20, 0x00, "1"                                  },
	{0x17, 0x01, 0x20, 0x20, "2"                                  },	
};

STDDIPINFO(Astorm2p);

static struct BurnDIPInfo CltchitrDIPList[]=
{
	// Default Values
	{0x16, 0xff, 0xff, 0xff, NULL                                 },
	{0x17, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x16)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x17, 0x01, 0x01, 0x01, "Off"                                },
	{0x17, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x17, 0x01, 0x02, 0x02, "Off"                                },
	{0x17, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Game Time P1"                       },
	{0x17, 0x01, 0x0c, 0x04, "2 Credits 18 Outcounts 14 Min"      },
	{0x17, 0x01, 0x0c, 0x00, "1 Credit   6 Outcounts 7 Min"       },
	{0x17, 0x01, 0x0c, 0x08, "1 Credit  12 Outcounts 12 Min"      },
	{0x17, 0x01, 0x0c, 0x0c, "1C 6OC 8M / 2C 18OC 14M"            },
	
	{0   , 0xfe, 0   , 4   , "Game Time P2"                       },
	{0x17, 0x01, 0x30, 0x10, "4 Credits 18 Outcounts 16 Min"      },
	{0x17, 0x01, 0x30, 0x00, "2 Credits  6 Outcounts 8 Min"       },
	{0x17, 0x01, 0x30, 0x20, "2 Credits 12 Outcounts 14 Min"      },
	{0x17, 0x01, 0x30, 0x30, "2C 6OC 8M / 4C 18OC 16M"            },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x17, 0x01, 0xc0, 0x40, "Easiest"                            },
	{0x17, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x17, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x17, 0x01, 0xc0, 0x00, "Hard"                               },
};

STDDIPINFO(Cltchitr);

static struct BurnDIPInfo DdcrewDIPList[]=
{
	// Default Values
	{0x1b, 0xff, 0xff, 0xff, NULL                                 },
	{0x1c, 0xff, 0xff, 0xf7, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x1b)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Credits Needed"                     },
	{0x1c, 0x01, 0x01, 0x01, "1 to start, 1 to continue"          },
	{0x1c, 0x01, 0x01, 0x00, "2 to start, 1 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Switch to Start"                    },
	{0x1c, 0x01, 0x02, 0x02, "Start"                              },
	{0x1c, 0x01, 0x02, 0x00, "Attack"                             },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x1c, 0x01, 0x04, 0x04, "Common"                             },
	{0x1c, 0x01, 0x04, 0x00, "Individual"                         },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x1c, 0x01, 0x08, 0x08, "Off"                                },
	{0x1c, 0x01, 0x08, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Player Start/Continue"              },
	{0x1c, 0x01, 0x30, 0x30, "3/3"                                },
	{0x1c, 0x01, 0x30, 0x20, "2/3"                                },
	{0x1c, 0x01, 0x30, 0x10, "2/2"                                },
	{0x1c, 0x01, 0x30, 0x00, "3/4"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x1c, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x1c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x1c, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x1c, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Ddcrew);

static struct BurnDIPInfo Ddcrew2DIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                                 },
	{0x14, 0xff, 0xff, 0xf7, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x13)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Credits Needed"                     },
	{0x14, 0x01, 0x01, 0x01, "1 to start, 1 to continue"          },
	{0x14, 0x01, 0x01, 0x00, "2 to start, 1 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Switch to Start"                    },
	{0x14, 0x01, 0x02, 0x02, "Start"                              },
	{0x14, 0x01, 0x02, 0x00, "Attack"                             },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x14, 0x01, 0x04, 0x04, "Common"                             },
	{0x14, 0x01, 0x04, 0x00, "Individual"                         },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x14, 0x01, 0x08, 0x08, "Off"                                },
	{0x14, 0x01, 0x08, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Player Start/Continue"              },
	{0x14, 0x01, 0x30, 0x30, "3/3"                                },
	{0x14, 0x01, 0x30, 0x20, "2/3"                                },
	{0x14, 0x01, 0x30, 0x10, "2/2"                                },
	{0x14, 0x01, 0x30, 0x00, "3/4"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x14, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x14, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x14, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x14, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Ddcrew2);

static struct BurnDIPInfo DdcrewuDIPList[]=
{
	// Default Values
	{0x23, 0xff, 0xff, 0xff, NULL                                 },
	{0x24, 0xff, 0xff, 0xf7, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x23)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Credits Needed"                     },
	{0x24, 0x01, 0x01, 0x01, "1 to start, 1 to continue"          },
	{0x24, 0x01, 0x01, 0x00, "2 to start, 1 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Switch to Start"                    },
	{0x24, 0x01, 0x02, 0x02, "Start"                              },
	{0x24, 0x01, 0x02, 0x00, "Attack"                             },

	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x24, 0x01, 0x04, 0x04, "Common"                             },
	{0x24, 0x01, 0x04, 0x00, "Individual"                         },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x24, 0x01, 0x08, 0x08, "Off"                                },
	{0x24, 0x01, 0x08, 0x00, "On"                                 },

	{0   , 0xfe, 0   , 4   , "Player Start/Continue"              },
	{0x24, 0x01, 0x30, 0x30, "3/3"                                },
	{0x24, 0x01, 0x30, 0x20, "2/3"                                },
	{0x24, 0x01, 0x30, 0x10, "2/2"                                },
	{0x24, 0x01, 0x30, 0x00, "3/4"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x24, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x24, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x24, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x24, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Ddcrewu);

static struct BurnDIPInfo DesertbrDIPList[]=
{
	// Default Values
	{0x1e, 0xff, 0xff, 0xff, NULL                                 },
	{0x1f, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x1e)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x17, 0x01, 0x01, 0x01, "Off"                                },
	{0x17, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x1f, 0x01, 0x02, 0x02, "Off"                                },
	{0x1f, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Play Mode"                          },
	{0x1f, 0x01, 0x08, 0x00, "2 Players"                          },
	{0x1f, 0x01, 0x08, 0x08, "3 Players"                          },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x15, 0x01, 0x10, 0x10, "Common"                             },
	{0x15, 0x01, 0x10, 0x00, "Individual"                         },
	
	{0   , 0xfe, 0   , 2   , "Start Button"                       },
	{0x15, 0x01, 0x20, 0x20, "Use"                                },
	{0x15, 0x01, 0x20, 0x00, "Unused"                             },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x1f, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x1f, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x1f, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x1f, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Desertbr);

static struct BurnDIPInfo LghostDIPList[]=
{
	// Default Values
	{0x14, 0xff, 0xff, 0xff, NULL                                 },
	{0x15, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x14)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x15, 0x01, 0x01, 0x01, "Off"                                },
	{0x15, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x15, 0x01, 0x02, 0x02, "Off"                                },
	{0x15, 0x01, 0x02, 0x00, "On"                                 },

	{0   , 0xfe, 0   , 8   , "Difficulty"                         },
	{0x15, 0x01, 0x1c, 0x18, "Easy"                               },
	{0x15, 0x01, 0x1c, 0x14, "Easier"                             },
	{0x15, 0x01, 0x1c, 0x0c, "Easiest"                            },
	{0x15, 0x01, 0x1c, 0x1c, "Normal"                             },
	{0x15, 0x01, 0x1c, 0x10, "Hard"                               },
	{0x15, 0x01, 0x1c, 0x04, "Harder"                             },
	{0x15, 0x01, 0x1c, 0x08, "Hardest"                            },
	{0x15, 0x01, 0x1c, 0x00, "Extra Hardest"                      },
	
	{0   , 0xfe, 0   , 2   , "Continue Mode"                      },
	{0x15, 0x01, 0x20, 0x00, "Off"                                },
	{0x15, 0x01, 0x20, 0x20, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x15, 0x01, 0x40, 0x00, "Common"                             },
	{0x15, 0x01, 0x40, 0x40, "Individual"                         },	
};

STDDIPINFO(Lghost);

static struct BurnDIPInfo MwalkDIPList[]=
{
	// Default Values
	{0x1e, 0xff, 0xff, 0xff, NULL                                 },
	{0x1f, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x1e)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x1f, 0x01, 0x01, 0x01, "Off"                                },
	{0x1f, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x1f, 0x01, 0x02, 0x02, "Off"                                },
	{0x1f, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Lives"                              },
	{0x1f, 0x01, 0x04, 0x04, "2"                                  },
	{0x1f, 0x01, 0x04, 0x00, "3"                                  },
	
	{0   , 0xfe, 0   , 2   , "Player Vitality"                    },
	{0x1f, 0x01, 0x08, 0x08, "Low"                                },
	{0x1f, 0x01, 0x08, 0x00, "High"                               },
	
	{0   , 0xfe, 0   , 2   , "Play Mode"                          },
	{0x1f, 0x01, 0x10, 0x10, "2 Players"                          },
	{0x1f, 0x01, 0x10, 0x00, "3 Players"                          },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x1f, 0x01, 0x20, 0x20, "Common"                             },
	{0x1f, 0x01, 0x20, 0x00, "Individual"                         },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x1f, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x1f, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x1f, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x1f, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Mwalk);

static struct BurnDIPInfo ShdancerDIPList[]=
{
	// Default Values
	{0x16, 0xff, 0xff, 0xff, NULL                                 },
	{0x17, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x16)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x17, 0x01, 0x01, 0x01, "Off"                                },
	{0x17, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x17, 0x01, 0x02, 0x02, "Off"                                },
	{0x17, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Lives"                              },
	{0x17, 0x01, 0x0c, 0x00, "2"                                  },
	{0x17, 0x01, 0x0c, 0x0c, "3"                                  },
	{0x17, 0x01, 0x0c, 0x08, "4"                                  },
	{0x17, 0x01, 0x0C, 0x04, "5"                                  },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x17, 0x01, 0x30, 0x20, "Easy"                               },
	{0x17, 0x01, 0x30, 0x30, "Normal"                             },
	{0x17, 0x01, 0x30, 0x10, "Hard"                               },
	{0x17, 0x01, 0x30, 0x00, "Hardest"                            },
	
	{0   , 0xfe, 0   , 4   , "Time Adjust"                        },
	{0x17, 0x01, 0xc0, 0x00, "2:20"                               },
	{0x17, 0x01, 0xc0, 0x40, "2:40"                               },
	{0x17, 0x01, 0xc0, 0xc0, "3:00"                               },
	{0x17, 0x01, 0xc0, 0x80, "3:30"                               },
};

STDDIPINFO(Shdancer);

static struct BurnDIPInfo WwallyDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0xff, NULL                                 },
	{0x12, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	SYSTEM18_COINAGE(0x11)
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "2 Credits to Start"                 },
	{0x12, 0x01, 0x01, 0x01, "Off"                                },
	{0x12, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x12, 0x01, 0x02, 0x02, "Off"                                },
	{0x12, 0x01, 0x02, 0x00, "On"                                 },

	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x12, 0x01, 0x04, 0x04, "Common"                             },
	{0x12, 0x01, 0x04, 0x00, "Individual"                         },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x12, 0x01, 0x60, 0x40, "Easy"                               },
	{0x12, 0x01, 0x60, 0x60, "Normal"                             },
	{0x12, 0x01, 0x60, 0x20, "Hard"                               },
	{0x12, 0x01, 0x60, 0x00, "Hardest"                            },
};

STDDIPINFO(Wwally);

#undef SYSTEM18_COINAGE

/*====================================================
Rom defs
====================================================*/

static struct BurnRomInfo AstormRomDesc[] = {
	{ "epr13182.a6",   0x40000, 0xe31f2a1c, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13181.a5",   0x40000, 0x78cd3b26, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr13073.bin",  0x40000, 0xdf5d0a61, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13074.bin",  0x40000, 0x787afab8, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13075.bin",  0x40000, 0x4e01b477, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13082.bin",  0x40000, 0xa782b704, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13089.bin",  0x40000, 0x2a4227f0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13081.bin",  0x40000, 0xeb510228, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13088.bin",  0x40000, 0x3b6b4c55, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13080.bin",  0x40000, 0xe668eefb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13087.bin",  0x40000, 0x2293427d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13079.bin",  0x40000, 0xde9221ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13086.bin",  0x40000, 0x8c9a71c4, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13083a.bin", 0x20000, 0xe7528e06, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13076.bin",  0x40000, 0x94e6c76e, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13077.bin",  0x40000, 0xe2ec0d8d, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13078.bin",  0x40000, 0x15684dc5, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0154.key",  0x02000, 0xb86b6b8f, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Astorm);
STD_ROM_FN(Astorm);

static struct BurnRomInfo Astorm3RomDesc[] = {
	{ "epr13165.a6",   0x40000, 0x6efcd381, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13164.a5",   0x40000, 0x97d693c6, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr13073.bin",  0x40000, 0xdf5d0a61, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13074.bin",  0x40000, 0x787afab8, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13075.bin",  0x40000, 0x4e01b477, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13082.bin",  0x40000, 0xa782b704, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13089.bin",  0x40000, 0x2a4227f0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13081.bin",  0x40000, 0xeb510228, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13088.bin",  0x40000, 0x3b6b4c55, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13080.bin",  0x40000, 0xe668eefb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13087.bin",  0x40000, 0x2293427d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13079.bin",  0x40000, 0xde9221ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13086.bin",  0x40000, 0x8c9a71c4, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13083.bin",  0x20000, 0x5df3af20, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13076.bin",  0x40000, 0x94e6c76e, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13077.bin",  0x40000, 0xe2ec0d8d, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13078.bin",  0x40000, 0x15684dc5, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0148.key",  0x02000, 0x72e4b64a, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Astorm3);
STD_ROM_FN(Astorm3);

static struct BurnRomInfo AstormjRomDesc[] = {
	{ "epr13085.a6",   0x40000, 0x15f74e2d, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13084.a5",   0x40000, 0x9687b38f, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr13073.bin",  0x40000, 0xdf5d0a61, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13074.bin",  0x40000, 0x787afab8, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13075.bin",  0x40000, 0x4e01b477, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13082.bin",  0x40000, 0xa782b704, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13089.bin",  0x40000, 0x2a4227f0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13081.bin",  0x40000, 0xeb510228, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13088.bin",  0x40000, 0x3b6b4c55, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13080.bin",  0x40000, 0xe668eefb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13087.bin",  0x40000, 0x2293427d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13079.bin",  0x40000, 0xde9221ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13086.bin",  0x40000, 0x8c9a71c4, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13083.bin",  0x20000, 0x5df3af20, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13076.bin",  0x40000, 0x94e6c76e, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13077.bin",  0x40000, 0xe2ec0d8d, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13078.bin",  0x40000, 0x15684dc5, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0146.key",  0x02000, 0xe94991c5, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Astormj);
STD_ROM_FN(Astormj);

static struct BurnRomInfo AstormuRomDesc[] = {
	{ "epr13095.a6",   0x40000, 0x55d40742, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13094.a5",   0x40000, 0x92b305f9, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr13073.bin",  0x40000, 0xdf5d0a61, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13074.bin",  0x40000, 0x787afab8, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13075.bin",  0x40000, 0x4e01b477, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13082.bin",  0x40000, 0xa782b704, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13089.bin",  0x40000, 0x2a4227f0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13081.bin",  0x40000, 0xeb510228, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13088.bin",  0x40000, 0x3b6b4c55, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13080.bin",  0x40000, 0xe668eefb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13087.bin",  0x40000, 0x2293427d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13079.bin",  0x40000, 0xde9221ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13086.bin",  0x40000, 0x8c9a71c4, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13083.bin",  0x20000, 0x5df3af20, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13076.bin",  0x40000, 0x94e6c76e, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13077.bin",  0x40000, 0xe2ec0d8d, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13078.bin",  0x40000, 0x15684dc5, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0147.key",  0x02000, 0x3fd54ba7, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Astormu);
STD_ROM_FN(Astormu);

static struct BurnRomInfo BloxeedRomDesc[] = {
	{ "epr12911.a6",   0x20000, 0xa481581a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12910.a5",   0x20000, 0xdd1bc3bf, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "opr12884.b1",   0x10000, 0xe024aa33, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12885.b2",   0x10000, 0x8041b814, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12886.b3",   0x10000, 0xde32285e, SYS16_ROM_TILES | BRF_GRA },
	
	{ "opr12891.a11",  0x10000, 0x90d31a8c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr12887.b11",  0x10000, 0xf0c0f49d, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr12888.a4",   0x20000, 0x6f2fc63c, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "317-0139.key",  0x02000, 0x9aae84cb, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Bloxeed);
STD_ROM_FN(Bloxeed);

static struct BurnRomInfo CltchitrRomDesc[] = {
	{ "epr13794.4a",   0x40000, 0xc8d80233, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13795.6a",   0x40000, 0xb0b60b67, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13784.5a",   0x40000, 0x80c8180d, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13786.7a",   0x40000, 0x3095dac0, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr13773.1c",   0x80000, 0x3fc600e5, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13774.2c",   0x80000, 0x2411a824, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13775.3c",   0x80000, 0xcf527bf6, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13779.10c",  0x80000, 0xc707f416, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13787.10a",  0x80000, 0xf05c68c6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13780.11c",  0x80000, 0xa4c341e0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13788.11a",  0x80000, 0x0106fea6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13781.12c",  0x80000, 0xf33b13af, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13789.12a",  0x80000, 0x09ba8835, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13793.7c",   0x80000, 0xa3d31944, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13792.6c",   0x80000, 0x808f9695, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13791.5c",   0x80000, 0x35c16d80, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0176.key",  0x02000, 0x9b072430, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Cltchitr);
STD_ROM_FN(Cltchitr);

static struct BurnRomInfo CltchtrjRomDesc[] = {
	{ "epr13783.a4",   0x40000, 0xe2a1d5af, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13796.a6",   0x40000, 0x06001c67, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13785.a5",   0x40000, 0x09714762, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13797.a7",   0x40000, 0x361ade9f, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr13773.1c",   0x80000, 0x3fc600e5, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13774.2c",   0x80000, 0x2411a824, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13775.3c",   0x80000, 0xcf527bf6, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13779.10c",  0x80000, 0xc707f416, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13787.10a",  0x80000, 0xf05c68c6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13780.11c",  0x80000, 0xa4c341e0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13788.11a",  0x80000, 0x0106fea6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13781.12c",  0x80000, 0xf33b13af, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13789.12a",  0x80000, 0x09ba8835, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13782.c13",  0x40000, 0x73790852, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13790.a13",  0x40000, 0x23849101, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13778.c7",   0x20000, 0x35e86146, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13777.c6",   0x80000, 0xd1524782, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr13776.c5",   0x80000, 0x282ac9fe, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0175.key",  0x02000, 0x70d9d283, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Cltchtrj);
STD_ROM_FN(Cltchtrj);

static struct BurnRomInfo DdcrewRomDesc[] = {
	{ "epr-14160.a4",  0x40000, 0xb9f897b7, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-14161.a6",  0x40000, 0xbb03c1f0, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14139.5a",  0x40000, 0x06c31531, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14141.7a",  0x40000, 0x080a494b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr-14127.1c",  0x40000, 0x2228cd88, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14128.2c",  0x40000, 0xedba8e10, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14129.3c",  0x40000, 0xe8ecc305, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr-14134.10c", 0x80000, 0x4fda6a4b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14142.10a", 0x80000, 0x3cbf1f2a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14135.11c", 0x80000, 0xe9c74876, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14143.11a", 0x80000, 0x59022c31, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14136.12c", 0x80000, 0x720d9858, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14144.12a", 0x80000, 0x7775fdd4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14137.13c", 0x80000, 0x846c4265, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14145.13a", 0x80000, 0x0e76c797, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr-14133.7c",  0x20000, 0xcff96665, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-14132.6c",  0x80000, 0x1fae0220, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr-14131.5c",  0x80000, 0xbe5a7d0b, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr-14130.4c",  0x80000, 0x948f34a1, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0190.key",  0x02000, 0x2d502b11, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Ddcrew);
STD_ROM_FN(Ddcrew);

static struct BurnRomInfo Ddcrew1RomDesc[] = {
	{ "epr-14154.a4",  0x40000, 0x9a0dadf0, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-14155.a6",  0x40000, 0xe74362f4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14139.5a",  0x40000, 0x06c31531, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14141.7a",  0x40000, 0x080a494b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr-14127.1c",  0x40000, 0x2228cd88, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14128.2c",  0x40000, 0xedba8e10, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14129.3c",  0x40000, 0xe8ecc305, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr-14134.10c", 0x80000, 0x4fda6a4b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14142.10a", 0x80000, 0x3cbf1f2a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14135.11c", 0x80000, 0xe9c74876, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14143.11a", 0x80000, 0x59022c31, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14136.12c", 0x80000, 0x720d9858, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14144.12a", 0x80000, 0x7775fdd4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14137.13c", 0x80000, 0x846c4265, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14145.13a", 0x80000, 0x0e76c797, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr-14133.7c",  0x20000, 0xcff96665, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-14132.6c",  0x80000, 0x1fae0220, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr-14131.5c",  0x80000, 0xbe5a7d0b, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr-14130.4c",  0x80000, 0x948f34a1, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0187.key",  0x02000, 0x1dfb60be, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Ddcrew1);
STD_ROM_FN(Ddcrew1);

static struct BurnRomInfo Ddcrew2RomDesc[] = {
	{ "epr-14148.a4",  0x40000, 0xdf4cb0cf, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-14149.a6",  0x40000, 0x380ff818, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14139.5a",  0x40000, 0x06c31531, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14141.7a",  0x40000, 0x080a494b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr-14127.1c",  0x40000, 0x2228cd88, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14128.2c",  0x40000, 0xedba8e10, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14129.3c",  0x40000, 0xe8ecc305, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr-14134.10c", 0x80000, 0x4fda6a4b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14142.10a", 0x80000, 0x3cbf1f2a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14135.11c", 0x80000, 0xe9c74876, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14143.11a", 0x80000, 0x59022c31, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14136.12c", 0x80000, 0x720d9858, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14144.12a", 0x80000, 0x7775fdd4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14137.13c", 0x80000, 0x846c4265, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14145.13a", 0x80000, 0x0e76c797, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr-14133.7c",  0x20000, 0xcff96665, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-14132.6c",  0x80000, 0x1fae0220, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr-14131.5c",  0x80000, 0xbe5a7d0b, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr-14130.4c",  0x80000, 0x948f34a1, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0184.key",  0x02000, 0xcee06254, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Ddcrew2);
STD_ROM_FN(Ddcrew2);

static struct BurnRomInfo DdcrewjRomDesc[] = {
	{ "epr-14138.a4",  0x40000, 0xdf280b1b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-14140.a6",  0x40000, 0x48f223ee, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14139.5a",  0x40000, 0x06c31531, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14141.7a",  0x40000, 0x080a494b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr-14127.1c",  0x40000, 0x2228cd88, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14128.2c",  0x40000, 0xedba8e10, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14129.3c",  0x40000, 0xe8ecc305, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr-14134.10c", 0x80000, 0x4fda6a4b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14142.10a", 0x80000, 0x3cbf1f2a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14135.11c", 0x80000, 0xe9c74876, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14143.11a", 0x80000, 0x59022c31, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14136.12c", 0x80000, 0x720d9858, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14144.12a", 0x80000, 0x7775fdd4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14137.13c", 0x80000, 0x846c4265, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14145.13a", 0x80000, 0x0e76c797, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr-14133.7c",  0x20000, 0xcff96665, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-14132.6c",  0x80000, 0x1fae0220, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr-14131.5c",  0x80000, 0xbe5a7d0b, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr-14130.4c",  0x80000, 0x948f34a1, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0182.key",  0x02000, 0x2e8a3601, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Ddcrewj);
STD_ROM_FN(Ddcrewj);

static struct BurnRomInfo DdcrewuRomDesc[] = {
	{ "epr-14152.4a",  0x40000, 0x69c7b571, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-14153.6a",  0x40000, 0xe01fae0c, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14139.5a",  0x40000, 0x06c31531, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mpr-14141.7a",  0x40000, 0x080a494b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr-14127.1c",  0x40000, 0x2228cd88, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14128.2c",  0x40000, 0xedba8e10, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-14129.3c",  0x40000, 0xe8ecc305, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr-14134.10c", 0x80000, 0x4fda6a4b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14142.10a", 0x80000, 0x3cbf1f2a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14135.11c", 0x80000, 0xe9c74876, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14143.11a", 0x80000, 0x59022c31, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14136.12c", 0x80000, 0x720d9858, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr-14144.12a", 0x80000, 0x7775fdd4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14137.13c", 0x80000, 0x846c4265, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-14145.13a", 0x80000, 0x0e76c797, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr-14133.7c",  0x20000, 0xcff96665, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-14132.6c",  0x80000, 0x1fae0220, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr-14131.5c",  0x80000, 0xbe5a7d0b, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr-14130.4c",  0x80000, 0x948f34a1, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0186.key",  0x02000, 0x7acaf1fd, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Ddcrewu);
STD_ROM_FN(Ddcrewu);

static struct BurnRomInfo DesertbrRomDesc[] = {
	{ "epr14802.a4",   0x080000, 0x9ab93cbc, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr14902.a6",   0x080000, 0x6724e7b1, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr14793.a5",   0x080000, 0xdc9d7af3, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr14795.a7",   0x080000, 0x7e5bf7d9, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr14781.b1",   0x100000, 0xc4f7d7aa, SYS16_ROM_TILES | BRF_GRA },
	{ "epr14782.b2",   0x100000, 0xccc98d05, SYS16_ROM_TILES | BRF_GRA },
	{ "epr14783.b3",   0x100000, 0xef202bec, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr14788.b10",  0x100000, 0xb5b05536, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14796.a10",  0x100000, 0xc033220a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14789.b11",  0x100000, 0x0f9bcb97, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14797.a11",  0x100000, 0x4c301cc9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14790.b12",  0x100000, 0x6a07ac27, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14798.a12",  0x100000, 0x50634625, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14791.b13",  0x100000, 0xa4ae352b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr14799.a13",  0x100000, 0xaeb7b025, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr14787.b7",   0x040000, 0xcc6feec7, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr14786.c6",   0x080000, 0xcc8349f2, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr14785.c5",   0x080000, 0x7babba13, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "epr14784.c4",   0x080000, 0x073878e4, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0196.key",  0x002000, 0xcb942262, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Desertbr);
STD_ROM_FN(Desertbr);

static struct BurnRomInfo LghostRomDesc[] = {
	{ "epr13429.4a",   0x40000, 0x09bd65c0, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13430.6a",   0x40000, 0x51009fe0, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13411.5a",   0x40000, 0x5160167b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13413.7a",   0x40000, 0x656b3bd8, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr13414.1c",   0x40000, 0xdada2419, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13415.2c",   0x40000, 0xbbb62c48, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13416.3c",   0x40000, 0x1d11dbae, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr13604.10c",  0x80000, 0x4009c8e5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13603.10a",  0x80000, 0x5350a94e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13424.11c",  0x80000, 0xfb98d920, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13421.11a",  0x80000, 0x2fc75890, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13425.12c",  0x80000, 0xf8252589, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13422.12a",  0x80000, 0x48a0754d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13426.13c",  0x80000, 0x5cfb1e25, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13423.13a",  0x80000, 0x335bbc9d, SYS16_ROM_SPRITES | BRF_GRA },	

	{ "epr13417.7c",   0x20000, 0xcd7beb49, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr13420.6c",   0x40000, 0x3de0dee4, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13419.5c",   0x40000, 0xe7021b0a, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13418.4c",   0x40000, 0x0732594d, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0166.key",  0x02000, 0x8379961f, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Lghost);
STD_ROM_FN(Lghost);

static struct BurnRomInfo LghostuRomDesc[] = {
	{ "epr13427.4a",   0x40000, 0x5bf8fb6b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13428.6a",   0x40000, 0x276775f5, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13411.5a",   0x40000, 0x5160167b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13413.7a",   0x40000, 0x656b3bd8, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "epr13414.1c",   0x40000, 0xdada2419, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13415.2c",   0x40000, 0xbbb62c48, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13416.3c",   0x40000, 0x1d11dbae, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr13604.10c",  0x80000, 0x4009c8e5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13603.10a",  0x80000, 0x5350a94e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13424.11c",  0x80000, 0xfb98d920, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13421.11a",  0x80000, 0x2fc75890, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13425.12c",  0x80000, 0xf8252589, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13422.12a",  0x80000, 0x48a0754d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13426.13c",  0x80000, 0x5cfb1e25, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13423.13a",  0x80000, 0x335bbc9d, SYS16_ROM_SPRITES | BRF_GRA },	

	{ "epr13417.7c",   0x20000, 0xcd7beb49, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr13420.6c",   0x40000, 0x3de0dee4, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13419.5c",   0x40000, 0xe7021b0a, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13418.4c",   0x40000, 0x0732594d, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0165.key",  0x02000, 0xa04267ab, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Lghostu);
STD_ROM_FN(Lghostu);

static struct BurnRomInfo MwalkRomDesc[] = {
	{ "epr13235.a6",   0x40000, 0x6983e129, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr13234.a5",   0x40000, 0xc9fd20f2, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr13216.b1",   0x40000, 0x862d2c03, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13217.b2",   0x40000, 0x7d1ac3ec, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13218.b3",   0x40000, 0x56d3393c, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13224.b11",  0x40000, 0xc59f107b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13231.a11",  0x40000, 0xa5e96346, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13223.b10",  0x40000, 0x364f60ff, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13230.a10",  0x40000, 0x9550091f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13222.b9",   0x40000, 0x523df3ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13229.a9",   0x40000, 0xf40dc45d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13221.b8",   0x40000, 0x9ae7546a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13228.a8",   0x40000, 0xde3786be, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13225.a4",   0x20000, 0x56c2e82b, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr13219.b4",   0x40000, 0x19e2061f, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13220.b5",   0x40000, 0x58d4d9ce, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13249.b6",   0x40000, 0x623edc5d, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0159.key",  0x02000, 0x507838f0, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
	
	{ "315-5437.ic4",  0x01000, 0x4bf63bc1, SYS16_ROM_PROM | BRF_PRG},
};


STD_ROM_PICK(Mwalk);
STD_ROM_FN(Mwalk);

static struct BurnRomInfo MwalkblRomDesc[] = {
	{ "mwalkbl.01",    0x10000, 0xf49cdb16, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.05",    0x10000, 0xc483f29f, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.02",    0x10000, 0x0bde1896, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.06",    0x10000, 0x5b9fc688, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.03",    0x10000, 0x0c5fe15c, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.07",    0x10000, 0x9e600704, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.04",    0x10000, 0x64692f79, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "mwalkbl.08",    0x10000, 0x546ca530, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr13216.b1",   0x40000, 0x862d2c03, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13217.b2",   0x40000, 0x7d1ac3ec, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr13218.b3",   0x40000, 0x56d3393c, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr13224.b11",  0x40000, 0xc59f107b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13231.a11",  0x40000, 0xa5e96346, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13223.b10",  0x40000, 0x364f60ff, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13230.a10",  0x40000, 0x9550091f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13222.b9",   0x40000, 0x523df3ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr13229.a9",   0x40000, 0xf40dc45d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13221.b8",   0x40000, 0x9ae7546a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13228.a8",   0x40000, 0xde3786be, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr13225.a4",   0x20000, 0x56c2e82b, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr13219.b4",   0x40000, 0x19e2061f, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13220.b5",   0x40000, 0x58d4d9ce, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr13249.b6",   0x40000, 0x623edc5d, SYS16_ROM_RF5C68DATA | BRF_SND },
};


STD_ROM_PICK(Mwalkbl);
STD_ROM_FN(Mwalkbl);

static struct BurnRomInfo ShdancerRomDesc[] = {
	{ "shdancer.a6",   0x40000, 0x3d5b3fa9, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "shdancer.a5",   0x40000, 0x2596004e, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr12712.b1",   0x40000, 0x9bdabe3d, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr12713.b2",   0x40000, 0x852d2b1c, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr12714.b3",   0x40000, 0x448226ce, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12719.b11",  0x40000, 0xd6888534, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12726.a11",  0x40000, 0xff344945, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12718.b10",  0x40000, 0xba2efc0c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12725.a10",  0x40000, 0x268a0c17, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12717.b9",   0x40000, 0xc81cc4f8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12724.a9",   0x40000, 0x0f4903dc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12716.b8",   0x40000, 0xa870e629, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12723.a8",   0x40000, 0xc606cf90, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr12720.a4",   0x20000, 0x7a0d8de1, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr12715.b4",   0x40000, 0x07051a52, SYS16_ROM_RF5C68DATA | BRF_SND },	
};


STD_ROM_PICK(Shdancer);
STD_ROM_FN(Shdancer);

static struct BurnRomInfo Shdance1RomDesc[] = {
	{ "epr12772b.a6",  0x40000, 0x6868a4d4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12771b.a5",  0x40000, 0x04e30c84, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr12712.b1",   0x40000, 0x9bdabe3d, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr12713.b2",   0x40000, 0x852d2b1c, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr12714.b3",   0x40000, 0x448226ce, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12719.b11",  0x40000, 0xd6888534, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12726.a11",  0x40000, 0xff344945, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12718.b10",  0x40000, 0xba2efc0c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12725.a10",  0x40000, 0x268a0c17, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12717.b9",   0x40000, 0xc81cc4f8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12724.a9",   0x40000, 0x0f4903dc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12716.b8",   0x40000, 0xa870e629, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12723.a8",   0x40000, 0xc606cf90, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr12720.a4",   0x20000, 0x7a0d8de1, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr12715.b4",   0x40000, 0x07051a52, SYS16_ROM_RF5C68DATA | BRF_SND },	
};

	
STD_ROM_PICK(Shdance1);
STD_ROM_FN(Shdance1);

static struct BurnRomInfo ShdancejRomDesc[] = {
	{ "epr12722b.a6",  0x40000, 0xc00552a2, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12721b.a5",  0x40000, 0x653d351a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr12712.b1",   0x40000, 0x9bdabe3d, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr12713.b2",   0x40000, 0x852d2b1c, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr12714.b3",   0x40000, 0x448226ce, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12719.b11",  0x40000, 0xd6888534, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12726.a11",  0x40000, 0xff344945, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12718.b10",  0x40000, 0xba2efc0c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12725.a10",  0x40000, 0x268a0c17, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12717.b9",   0x40000, 0xc81cc4f8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12724.a9",   0x40000, 0x0f4903dc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12716.b8",   0x40000, 0xa870e629, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12723.a8",   0x40000, 0xc606cf90, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr12720.a4",   0x20000, 0x7a0d8de1, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr12715.b4",   0x40000, 0x07051a52, SYS16_ROM_RF5C68DATA | BRF_SND },	
};


STD_ROM_PICK(Shdancej);
STD_ROM_FN(Shdancej);

static struct BurnRomInfo ShdancblRomDesc[] = {
	{ "ic39",          0x10000, 0xadc1781c, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic53",          0x10000, 0x1c1ac463, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic38",          0x10000, 0xcd6e155b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic52",          0x10000, 0xbb3c49a4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic37",          0x10000, 0x1bd8d5c3, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic51",          0x10000, 0xce2e71b4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic36",          0x10000, 0xbb861290, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic50",          0x10000, 0x7f7b82b1, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "ic4",           0x20000, 0xf0a016fe, SYS16_ROM_TILES | BRF_GRA },
	{ "ic18",          0x20000, 0xf6bee053, SYS16_ROM_TILES | BRF_GRA },
	{ "ic3",           0x20000, 0xe07e6b5d, SYS16_ROM_TILES | BRF_GRA },
	{ "ic17",          0x20000, 0xf59deba1, SYS16_ROM_TILES | BRF_GRA },
	{ "ic2",           0x20000, 0x60095070, SYS16_ROM_TILES | BRF_GRA },
	{ "ic16",          0x20000, 0x0f0d5dd3, SYS16_ROM_TILES | BRF_GRA },
	
	{ "ic73",          0x10000, 0x59e77c96, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic58",          0x10000, 0x9cd5c8c7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic74",          0x10000, 0x90ea5407, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic59",          0x10000, 0xff40e872, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic75",          0x10000, 0x27d2fa61, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic60",          0x10000, 0x826d7245, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic76",          0x10000, 0xf36db688, SYS16_ROM_SPRITES | BRF_GRA },	
	{ "ic61",          0x10000, 0xdcf8068b, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "ic77",          0x10000, 0xf93470b7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic62",          0x10000, 0x50ca8065, SYS16_ROM_SPRITES | BRF_GRA },
	{ "sdbl.78",       0x10000, 0xe533be5d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic63",          0x10000, 0xd1866aa9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic95",          0x10000, 0x828b8294, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic90",          0x10000, 0x3602b758, SYS16_ROM_SPRITES | BRF_GRA },
	{ "sdbl.94",       0x10000, 0xe2fa2b41, SYS16_ROM_SPRITES | BRF_GRA },	
	{ "ic89",          0x10000, 0x1ba4be93, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "ic79",          0x10000, 0xf22548ee, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic64",          0x10000, 0x7a8b7bcc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic80",          0x10000, 0x6209f7f9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic65",          0x10000, 0x90ffca14, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic81",          0x10000, 0x34692f23, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic66",          0x10000, 0x5d655517, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic82",          0x10000, 0x7ae40237, SYS16_ROM_SPRITES | BRF_GRA },	
	{ "ic67",          0x10000, 0x0e5d0855, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "ic83",          0x10000, 0xa9040a32, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic68",          0x10000, 0x8d684e53, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic84",          0x10000, 0xd6810031, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic69",          0x10000, 0xc47d32e2, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic92",          0x10000, 0xb57d5cb5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic88",          0x10000, 0x9de140e1, SYS16_ROM_SPRITES | BRF_GRA },
	{ "ic91",          0x10000, 0x49def6c8, SYS16_ROM_SPRITES | BRF_GRA },	
	{ "ic87",          0x10000, 0x8172a991, SYS16_ROM_SPRITES | BRF_GRA },	

	{ "ic45",          0x10000, 0x576b3a81, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "ic46",          0x10000, 0xc84e8c84, SYS16_ROM_RF5C68DATA | BRF_SND },	
};


STD_ROM_PICK(Shdancbl);
STD_ROM_FN(Shdancbl);

static struct BurnRomInfo WwallyjRomDesc[] = {
	{ "epr14730b.a4",  0x040000, 0xe72bc17a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr14731b.a6",  0x040000, 0x6e3235b9, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr14719.c1",   0x040000, 0x8b58c743, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr14720.c2",   0x040000, 0xf96d19f4, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr14721.c3",   0x040000, 0xc4ced91d, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr14726.c10",  0x100000, 0x7213d1d3, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14732.a10",  0x100000, 0x04ced549, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14727.c11",  0x100000, 0x3b74e0f0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14733.a11",  0x100000, 0x6da0444f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14728.c12",  0x080000, 0x5b921587, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14734.a12",  0x080000, 0x6f3f5ed9, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr14725.c7",   0x020000, 0x2b29684f, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr14724.c6",   0x080000, 0x47cbea86, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr14723.c5",   0x080000, 0xbc5adc27, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr14722.c4",   0x080000, 0x1bd081f8, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0197b.key", 0x002000, 0xf5b7c5b4, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Wwallyj);
STD_ROM_FN(Wwallyj);

static struct BurnRomInfo WwallyjaRomDesc[] = {
	{ "epr14730a.a4",  0x040000, 0xdaa7880e, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr14731a.a6",  0x040000, 0x5e36353b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },

	{ "mpr14719.c1",   0x040000, 0x8b58c743, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr14720.c2",   0x040000, 0xf96d19f4, SYS16_ROM_TILES | BRF_GRA },
	{ "mpr14721.c3",   0x040000, 0xc4ced91d, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr14726.c10",  0x100000, 0x7213d1d3, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14732.a10",  0x100000, 0x04ced549, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14727.c11",  0x100000, 0x3b74e0f0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14733.a11",  0x100000, 0x6da0444f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14728.c12",  0x080000, 0x5b921587, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr14734.a12",  0x080000, 0x6f3f5ed9, SYS16_ROM_SPRITES | BRF_GRA },

	{ "epr14725.c7",   0x020000, 0x2b29684f, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr14724.c6",   0x080000, 0x47cbea86, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr14723.c5",   0x080000, 0xbc5adc27, SYS16_ROM_RF5C68DATA | BRF_SND },
	{ "mpr14722.c4",   0x080000, 0x1bd081f8, SYS16_ROM_RF5C68DATA | BRF_SND },
	
	{ "317-0197a.key", 0x002000, 0x2fb6a9a1, SYS16_ROM_KEY | BRF_ESS | BRF_PRG},
};


STD_ROM_PICK(Wwallyja);
STD_ROM_FN(Wwallyja);

/*====================================================
Memory Handlers
====================================================*/

static UINT8 misc_io_data[0x10];

static unsigned char io_chip_r(unsigned int offset)
{
	switch (offset) {
		case 0x03:
		case 0x07: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return 0xff;
		}
		
		case 0x00: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return 0xff - System16Input[1];
		}
		
		case 0x01: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return 0xff - System16Input[2];
		}
		
		case 0x02: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return 0xff - System16Input[3];
		}
		
		case 0x04: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return 0xff - System16Input[0];
		}
		
		case 0x05: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return System16Dip[0];
		}
		
		case 0x06: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return System16Dip[1];
		}
		
		case 0x08: {
			return 'S';
		}
		
		case 0x09: {
			return 'E';
		}
		
		case 0x0a: {
			return 'G';
		}
		
		case 0x0b: {
			return 'A';
		}
		
		case 0x0c: 
		case 0x0e: {
			return misc_io_data[0x0e];
		}
		
		case 0x0d: 
		case 0x0f: {
			return misc_io_data[0x0f];
		}
	}
	
	return 0xff;
}

static void io_chip_w(unsigned int offset, unsigned short d)
{
	UINT8 old;
	
	offset &= 0x1f/2;

	old = misc_io_data[offset];
	misc_io_data[offset] = d;
	
	switch (offset) {
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x04:
		case 0x05:
		case 0x06: {
			return;
		}
		
		case 0x07: {
			for (int i = 0; i < 4; i++) {
				if (System16TileBanks[0 + i] != ((d & 0xf) * 4 + i)) {
					System16TileBanks[0 + i] = (d & 0xf) * 4 + i;
					System16RecalcBgTileMap = 1;
					System16RecalcBgAltTileMap = 1;
					System16RecalcFgTileMap = 1;
					System16RecalcFgAltTileMap = 1;
				}
				if (System16TileBanks[4 + i] != (((d >> 4) & 0xf) * 4 + i)) {
					System16TileBanks[4 + i] = ((d >> 4) & 0xf) * 4 + i;
					System16RecalcBgTileMap = 1;
					System16RecalcBgAltTileMap = 1;
					System16RecalcFgTileMap = 1;
					System16RecalcFgAltTileMap = 1;
				}
			}
			return;
		}
		
		case 0x0e: {
			System16VideoEnable = d & 0x02;
			if ((old ^ d) & 4) System18VdpEnable = d & 0x04;
			return;
		}
		
	}
}

static void System18GfxBankWrite(unsigned int offset, unsigned short d)
{
	// Tile Banking
	if (offset < 8) {
		int MaxBanks = System16NumTiles / 1024;
		if (d >= MaxBanks) d %= MaxBanks;
		if (System16TileBanks[offset] != d) {
			System16TileBanks[offset] = d;
			System16RecalcBgTileMap = 1;
			System16RecalcBgAltTileMap = 1;
			System16RecalcFgTileMap = 1;
			System16RecalcFgAltTileMap = 1;
		}
	} else {
		// Sprite Banking
		int MaxBanks = System16SpriteRomSize / 0x40000;
		if (d >= MaxBanks) d = 255;
		System16SpriteBanks[(offset - 8) * 2 + 0] = d * 2 + 0;
		System16SpriteBanks[(offset - 8) * 2 + 1] = d * 2 + 1;
	}
}

unsigned short __fastcall System18ReadWord(unsigned int a)
{
	if (a >= 0xc00000 && a <= 0xc0000f) {
		return GenesisVDPRead((a - 0xc00000) >> 1);
	}
	
#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Word -> 0x%06X\n"), a);
#endif

	return 0xffff;
}

unsigned char __fastcall System18ReadByte(unsigned int a)
{
	if (a >= 0xa40000 && a <= 0xa4001f) {
		return io_chip_r((a - 0xa40000) >> 1);
	}
	
	if (a >= 0xe40000 && a <= 0xe4001f) {
		return io_chip_r((a - 0xe40000) >> 1);
	}
	
#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Byte -> 0x%06X\n"), a);
#endif

	return 0xff;
}

void __fastcall System18WriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x400000 && a <= 0x40ffff) {
		System16BTileWordWrite(a - 0x400000, d);
		return;
	}
	
	if (a >= 0xc00000 && a <= 0xc0000f) {
		GenesisVDPWrite((a - 0xc00000) >> 1, d);
		return;
	}
	
	switch (a) {
		case 0xe42000: {
			System18VdpMixing = d & 0xff;
			return;
		}
	}
	
#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

void __fastcall System18WriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x400000 && a <= 0x40ffff) {
		System16BTileByteWrite((a - 0x400000) ^ 1, d);
		return;
	}
	
	if (a >= 0x3e0000 && a <= 0x3e001f) {
		System18GfxBankWrite((a - 0x3e0000) >> 1, d);
		return;
	}
	
	if (a >= 0xa40000 && a <= 0xa41fff) {
		io_chip_w((a - 0xa40000) >> 1, d);
		return;
	}
	
	if (a >= 0xc00000 && a <= 0xc0000f) {
		GenesisVDPWrite((a - 0xc00000) >> 1, d);
		return;
	}
	
	if (a >= 0xe40000 && a <= 0xe41fff) {
		io_chip_w((a - 0xe40000) >> 1, d);
		return;
	}

	switch (a) {
		case 0xa42001:
		case 0xe42001: {
			System18VdpMixing = d & 0xff;
			return;
		}
	
		case 0xfe0007: {
			System16SoundLatch = d & 0xff;
			ZetOpen(0);
			ZetNmi();
			ZetClose();
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Byte -> 0x%06X, 0x%02X\n"), a, d);
#endif
}

unsigned char __fastcall Astorm3ReadByte(unsigned int a)
{
	if (a >= 0xa00000 && a <= 0xa0001f) {
		return io_chip_r((a - 0xa00000) >> 1);
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Byte -> 0x%06X\n"), a);
#endif

	return 0xff;
}

void __fastcall Astorm3WriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0xa00000 && a <= 0xa01fff) {
		io_chip_w((a - 0xa00000) >> 1, d);
		return;
	}

	switch (a) {
		case 0xa02101: {
			System18VdpMixing = d & 0xff;
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Byte -> 0x%06X, 0x%02X\n"), a, d);
#endif
}

unsigned char __fastcall DdcrewuReadByte(unsigned int a)
{
	switch (a) {
		case 0xe43021: {
			return 0xff - System16Input[3];
		}
		
		case 0xe43023: {
			return 0xff - System16Input[4];
		}
		
		case 0xe43025: {
			return 0xff - System16Input[5];
		}
	}
	
	return 0xff;
}

UINT8 LghostValue;

unsigned char __fastcall LghostReadByte(unsigned int a)
{
	switch (a) {
		case 0xe43011:
		case 0xe43013:
		case 0xe43015:
		case 0xe43017: {
			UINT8 result = LghostValue | 0x7f;
			LghostValue <<= 1;
			return result;
		}
	}

	return 0xff;
}

void __fastcall LghostWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0xe43011: {
			LghostValue = ~BurnGunReturnY(0);
			return;
		}
		
		case 0xe43013: {
			LghostValue = BurnGunReturnX(0);
			return;
		}
		
		case 0xe43015: {
			LghostValue = (System16AnalogSelect) ? ~BurnGunReturnY(2) : ~BurnGunReturnY(1);
			return;
		}
		
		case 0xe43017: {
			LghostValue = (System16AnalogSelect) ? BurnGunReturnX(2) : BurnGunReturnX(1);
			return;
		}
		
		case 0xe43021: {
			System16AnalogSelect = d & 1;
			return;
		}
	}
}

unsigned char __fastcall MwalkblReadByte(unsigned int a)
{
	switch (a) {
		case 0xc40001: {
			return System16Dip[0];
		}
		
		case 0xc40003: {
			return System16Dip[1];
		}
		
		case 0xc41001: {
			return 0xff - System16Input[0];
		}
		
		case 0xc41003: {
			return 0xff - System16Input[1];
		}
		
		case 0xc41005: {
			return 0xff - System16Input[2];
		}
		
		case 0xc41007: {
			return 0xff - System16Input[3];
		}
	}
	
	return 0xff;
}

void __fastcall MwalkblWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0xc40007: {
			System16SoundLatch = d & 0xff;
			ZetOpen(0);
			ZetNmi();
			ZetClose();
			return;
		}
		
		case 0xc46601: {
			System16VideoEnable = d & 0x02;
			return;
		}
		
		case 0xc46801: {
			for (int i = 0; i < 4; i++) {
				if (System16TileBanks[0 + i] != ((d & 0xf) * 4 + i)) {
					System16TileBanks[0 + i] = (d & 0xf) * 4 + i;
					System16RecalcBgTileMap = 1;
					System16RecalcBgAltTileMap = 1;
					System16RecalcFgTileMap = 1;
					System16RecalcFgAltTileMap = 1;
				}
				if (System16TileBanks[4 + i] != (((d >> 4) & 0xf) * 4 + i)) {
					System16TileBanks[4 + i] = ((d >> 4) & 0xf) * 4 + i;
					System16RecalcBgTileMap = 1;
					System16RecalcBgAltTileMap = 1;
					System16RecalcFgTileMap = 1;
					System16RecalcFgAltTileMap = 1;
				}
			}
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Byte -> 0x%06X, 0x%02X\n"), a, d);
#endif
}

void __fastcall MwalkblWriteWord(unsigned int a, unsigned short d)
{
	switch (a) {
		case 0xc46000: {
			System16ScrollY[1] = d & 0xff;
			return;
		}
		
		case 0xc46200: {
			System16ScrollY[0] = d & 0xff;
			return;
		}
		
		case 0xc46400: {
			BootlegBgPage[0] = (d >> 8) & 0xf;
			System16ScrollX[0] = d & 0xff;			
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

unsigned char __fastcall ShdancblReadByte(unsigned int a)
{
	switch (a) {
		case 0xc40001: {
			return System16Dip[0];
		}
		
		case 0xc40003: {
			return System16Dip[1];
		}
		
		case 0xc41001: {
			return 0xff - System16Input[0];
		}
		
		case 0xc41003: {
			return 0xff - System16Input[1];
		}
		
		case 0xc41005: {
			return 0xff - System16Input[2];
		}
		
		case 0xc41007: {
			return 0xff - System16Input[3];
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Byte -> 0x%06X\n"), a);
#endif
	
	return 0xff;
}

void __fastcall ShdancblWriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0xc40007: {
			System16SoundLatch = d & 0xff;
			ZetOpen(0);
			ZetRaiseIrq(0);
			ZetClose();
			return;
		}
		
		case 0xe4001d: {
			//System16VideoEnable = d & 0x02;
			return;
		}
		
		case 0xe4000f: {
			for (int i = 0; i < 4; i++) {
				if (System16TileBanks[0 + i] != ((d & 0xf) * 4 + i)) {
					System16TileBanks[0 + i] = (d & 0xf) * 4 + i;
					System16RecalcBgTileMap = 1;
					System16RecalcBgAltTileMap = 1;
					System16RecalcFgTileMap = 1;
					System16RecalcFgAltTileMap = 1;
				}
				if (System16TileBanks[4 + i] != (((d >> 4) & 0xf) * 4 + i)) {
					System16TileBanks[4 + i] = ((d >> 4) & 0xf) * 4 + i;
					System16RecalcBgTileMap = 1;
					System16RecalcBgAltTileMap = 1;
					System16RecalcFgTileMap = 1;
					System16RecalcFgAltTileMap = 1;
				}
			}
			return;
		}

	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Byte -> 0x%06X, 0x%02X\n"), a, d);
#endif
}

static short WwallyTrack1X = 0;
static short WwallyTrack1Y = 0;
static short WwallyTrack2X = 0;
static short WwallyTrack2Y = 0;

static UINT8 WwallyLastX[2];
static UINT8 WwallyLastY[2];

unsigned char __fastcall WwallyReadByte(unsigned int a)
{
	switch (a) {
		case 0xa43001: {
			return (WwallyTrack1X - WwallyLastX[0]) & 0xff;
		}
		
		case 0xa43005: {
			return (WwallyTrack1Y - WwallyLastY[0]) & 0xff;
		}
		
		case 0xa43009: {
			return (WwallyTrack2X - WwallyLastX[1]) & 0xff;
		}
		
		case 0xa4300d: {
			return (WwallyTrack2Y - WwallyLastY[1]) & 0xff;
		}

	}
	
	return 0xff;
}

void __fastcall WwallyWriteWord(unsigned int a, unsigned short /*d*/)
{
	switch (a) {
		case 0xa43000: {
			WwallyLastX[0] = WwallyTrack1X;
			WwallyLastY[0] = WwallyTrack1Y;
			return;
		}
		
		case 0xa43008: {
			WwallyLastX[1] = WwallyTrack2X;
			WwallyLastY[1] = WwallyTrack2Y;
			return;
		}
	}
}

/*====================================================
Driver inits
====================================================*/

void WwallyMakeAnalogInputs()
{
	if (System16InputPort1[1]) WwallyTrack1X += 0x4;
	if (System16InputPort1[2]) WwallyTrack1X -= 0x4;
	if (WwallyTrack1X >= 0x100) WwallyTrack1X = 0;
	if (WwallyTrack1X < 0) WwallyTrack1X = 0xfc;
	
	if (System16InputPort1[3]) WwallyTrack1Y -= 0x4;
	if (System16InputPort1[4]) WwallyTrack1Y += 0x4;
	if (WwallyTrack1Y >= 0x100) WwallyTrack1Y = 0;
	if (WwallyTrack1Y < 0) WwallyTrack1Y = 0xfc;
	
	if (System16InputPort2[1]) WwallyTrack2X += 0x4;
	if (System16InputPort2[2]) WwallyTrack2X -= 0x4;
	if (WwallyTrack2X >= 0x100) WwallyTrack2X = 0;
	if (WwallyTrack2X < 0) WwallyTrack2X = 0xfc;
	
	if (System16InputPort2[3]) WwallyTrack2Y -= 0x4;
	if (System16InputPort2[4]) WwallyTrack2Y += 0x4;
	if (WwallyTrack2Y >= 0x100) WwallyTrack2Y = 0;
	if (WwallyTrack2Y < 0) WwallyTrack2Y = 0xfc;
}

static int System18BankRom40000()
{
	int nRet = 1;
	unsigned char *pTemp = (unsigned char*)malloc(0x280000);
	
	if (pTemp) {
		memcpy(pTemp, System16Rom, 0x280000);
		memset(System16Rom, 0, 0x280000);
		memcpy(System16Rom + 0x000000, pTemp + 0x00000, 0x80000);
		memcpy(System16Rom + 0x200000, pTemp + 0x80000, 0x80000);
		free(pTemp);
		nRet = 0;
	}
		
	return nRet;
}

static int System18BankRom80000()
{
	int nRet = 1;
	unsigned char *pTemp = (unsigned char*)malloc(0x300000);
	
	if (pTemp) {
		memcpy(pTemp, System16Rom, 0x300000);
		memset(System16Rom, 0, 0x300000);
		memcpy(System16Rom + 0x000000, pTemp + 0x000000, 0x100000);
		memcpy(System16Rom + 0x200000, pTemp + 0x100000, 0x100000);
		free(pTemp);
		nRet = 0;
	}
		
	return nRet;
}

static int System18Bank40000Init()
{
	System16RomSize = 0x180000;
	
	System16CustomLoadRomDo = System18BankRom40000;
	
	System16SpriteRomSize = 0x800000 - 0x400000;

	int nRet = System16Init();
	
	if (!nRet) {
		SekOpen(0);
		SekMapMemory(System16Rom + 0x200000, 0x200000, 0x27ffff, SM_READ);
		SekClose();
		
		unsigned char *pTemp = (unsigned char*)malloc(0x400000);
		if (pTemp) {
			memcpy(pTemp, System16Sprites, 0x400000);
			memset(System16Sprites, 0, System16SpriteRomSize);
			memcpy(System16Sprites + 0x000000, pTemp + 0x000000, 0x100000);
			memcpy(System16Sprites + 0x200000, pTemp + 0x100000, 0x100000);
			memcpy(System16Sprites + 0x400000, pTemp + 0x200000, 0x100000);
			memcpy(System16Sprites + 0x600000, pTemp + 0x300000, 0x100000);
		} else {
			nRet = 1;
		}
		free(pTemp);
	}
	
	return nRet;
}

static int System18Bank80000Init()
{
	System16RomSize = 0x100000;
	
	System16CustomLoadRomDo = System18BankRom80000;

	int nRet = System16Init();
	
	if (!nRet) {
		SekOpen(0);
		SekMapMemory(System16Rom + 0x200000, 0x200000, 0x27ffff, SM_READ);
		SekClose();
	}
	
	return nRet;
}

void Astorm3Map68K()
{
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(System16Rom           , 0x000000, 0x0bffff, SM_READ);
	SekMapMemory(System16Code          , 0x000000, 0x0bffff, SM_FETCH);
	SekMapMemory(System16TileRam       , 0x100000, 0x10ffff, SM_RAM);
	SekMapMemory(System16TextRam       , 0x110000, 0x110fff, SM_RAM);
	SekMapMemory(System16SpriteRam     , 0x200000, 0x2007ff, SM_RAM);
	SekMapMemory(System16PaletteRam    , 0x140000, 0x140fff, SM_RAM);
	SekMapMemory(System16Ram           , 0xffc000, 0xffffff, SM_RAM);
	SekSetReadWordHandler(0, System18ReadWord);
	SekSetWriteWordHandler(0, System18WriteWord);
	SekSetReadByteHandler(0, System18ReadByte);
	SekSetWriteByteHandler(0, System18WriteByte);
	SekMapHandler(1, 0xa00000, 0xa03fff, SM_RAM);
	SekSetReadByteHandler(1, Astorm3ReadByte);
	SekSetWriteByteHandler(1, Astorm3WriteByte);
	SekClose();
}

static int Astorm3Init()
{
	System16Map68KDo = Astorm3Map68K;
	
	int nRet = System16Init();

	return nRet;
}

static int DdcrewuInit()
{
	int nRet = System18Bank40000Init();
	
	if (!nRet) {
		SekOpen(0);
		SekMapHandler(1, 0xe43020, 0xe43025, SM_READ);
		SekSetReadByteHandler(1, DdcrewuReadByte);
		SekClose();
	}

	return nRet;
}

static int LghostInit()
{
	BurnGunInit(3, true);
	
	int nRet = System18Bank40000Init();
	
	if (!nRet) {
		SekOpen(0);
		SekMapHandler(1, 0xe43010, 0xe43021, SM_RAM);
		SekSetReadByteHandler(1, LghostReadByte);
		SekSetWriteByteHandler(1, LghostWriteByte);
		SekClose();
	}

	return nRet;
}

static int MwalkblPatchRom()
{
	*((unsigned short*)(System16Rom + 0x070212)) = 0x4e71;
	*((unsigned short*)(System16Rom + 0x070116)) = 0x4e71;
	*((unsigned short*)(System16Rom + 0x00314a)) = 0x4642;
	System16Rom[0x00311a] = 0x3f;
	System16Rom[0x070103] = 0x00;
	System16Rom[0x070109] = 0x00;
	System16Rom[0x007727] = 0x00;
	System16Rom[0x007729] = 0x00;
	System16Rom[0x00780d] = 0x00;
	System16Rom[0x00780f] = 0x00;
	System16Rom[0x007861] = 0x00;
	System16Rom[0x007863] = 0x00;
	System16Rom[0x007d47] = 0x00;
	System16Rom[0x007863] = 0x00;
	System16Rom[0x008533] = 0x00;
	System16Rom[0x008535] = 0x00;
	System16Rom[0x0085bd] = 0x00;
	System16Rom[0x0085bf] = 0x00;
	System16Rom[0x009a4b] = 0x00;
	System16Rom[0x009a4d] = 0x00;
	System16Rom[0x009b2f] = 0x00;
	System16Rom[0x009b31] = 0x00;
	System16Rom[0x00a05b] = 0x00;
	System16Rom[0x00a05d] = 0x00;
	System16Rom[0x00a23f] = 0x00;
	System16Rom[0x00a241] = 0x00;
	System16Rom[0x010159] = 0x00;
	System16Rom[0x01015b] = 0x00;
	System16Rom[0x0109fb] = 0x00;
	System16Rom[0x0109fd] = 0x00;
	
	return 0;
}

static int MwalkblInit()
{
	System16CustomLoadRomDo = MwalkblPatchRom;
	
	System16SpriteXOffset = 128;

	int nRet = System16Init();
	
	if (!nRet) {
		SekOpen(0);
		SekMapHandler(1, 0xc40000, 0xc41009, SM_READ);
		SekSetReadByteHandler(1, MwalkblReadByte);
		SekMapHandler(2, 0xc40006, 0xc46801, SM_WRITE);
		SekSetWriteByteHandler(2, MwalkblWriteByte);
		SekSetWriteWordHandler(2, MwalkblWriteWord);
		SekClose();
	}

	return nRet;
}

static int ShdancblInit()
{
	System16SpriteXOffset = 112;

	int nRet = System16Init();
	
	if (!nRet) {
		SekOpen(0);
		SekSetReadByteHandler(0, ShdancblReadByte);
		SekSetWriteByteHandler(0, ShdancblWriteByte);
		SekClose();
		
		System16VideoEnable = 1;
	}

	return nRet;
}

static int WwallyInit()
{
	System16MakeAnalogInputsDo = WwallyMakeAnalogInputs;
	
	int nRet = System16Init();
	
	if (!nRet) {
		SekOpen(0);
		SekMapHandler(1, 0xa43000, 0xa4300e, SM_RAM);
		SekSetReadByteHandler(1, WwallyReadByte);
		SekSetWriteWordHandler(1, WwallyWriteWord);
		SekClose();
	}
	
	return nRet;
}

static int System18Exit()
{
	memset(misc_io_data, 0, sizeof(misc_io_data));
	
	GenesisVDPExit();
	
	return System16Exit();
}

static int System18Scan(int nAction,int *pnMin)
{
	if (pnMin != NULL) {					// Return minimum compatible version
		*pnMin =  0x029660;
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(misc_io_data);
		
		GenesisVDPScan();
	}
	
	return System16Scan(nAction, pnMin);;
}

static int LghostExit()
{
	LghostValue = 0;

	return System18Exit();
}

static int LghostScan(int nAction,int *pnMin)
{
	if (pnMin != NULL) {					// Return minimum compatible version
		*pnMin =  0x029660;
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(LghostValue);
	}
	
	return System18Scan(nAction, pnMin);;
}

static int WwallyExit()
{
	WwallyTrack1X = 0;
	WwallyTrack1Y = 0;
	WwallyTrack2X = 0;
	WwallyTrack2Y = 0;

	memset(WwallyLastX, 0, sizeof(WwallyLastX));
	memset(WwallyLastY, 0, sizeof(WwallyLastY));

	return System18Exit();
}

static int WwallyScan(int nAction,int *pnMin)
{
	if (pnMin != NULL) {					// Return minimum compatible version
		*pnMin =  0x029660;
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(WwallyTrack1X);
		SCAN_VAR(WwallyTrack1Y);
		SCAN_VAR(WwallyTrack2X);
		SCAN_VAR(WwallyTrack2Y);
		SCAN_VAR(WwallyLastX);
		SCAN_VAR(WwallyLastY);
	}
	
	return System18Scan(nAction, pnMin);;
}

/*====================================================
Driver defs
====================================================*/

struct BurnDriver BurnDrvAstorm = {
	"astorm", NULL, NULL, "1990",
	"Alien Storm (set 4, World, 2 Players, FD1094 317-0154)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, AstormRomInfo, AstormRomName, System18InputInfo, Astorm2pDIPInfo,
	Astorm3Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvAstorm3 = {
	"astorm3", "astorm", NULL, "1990",
	"Alien Storm (set 3, World, 3 Players, FD1094 317-0148)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Astorm3RomInfo, Astorm3RomName, AstormInputInfo, AstormDIPInfo,
	Astorm3Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvAstormj = {
	"astormj", "astorm", NULL, "1990",
	"Alien Storm (set 1, Japan, 2 Players, FD1094 317-0146)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, AstormjRomInfo, AstormjRomName, System18InputInfo, Astorm2pDIPInfo,
	Astorm3Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvAstormu = {
	"astormu", "astorm", NULL, "1990",
	"Alien Storm (set 2, US, 3 Players, FD1094 317-0147)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, AstormuRomInfo, AstormuRomName, AstormInputInfo, AstormDIPInfo,
	Astorm3Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriverD BurnDrvBloxeed = {
	"bloxeed", NULL, NULL, "1990",
	"Bloxeed (Japan, FD1094 317-0139)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, BloxeedRomInfo, BloxeedRomName, System18InputInfo, NULL,
	System16Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvCltchitr = {
	"cltchitr", NULL, NULL, "1991",
	"Clutch Hitter (set 2, US, FD1094 317-0176)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, CltchitrRomInfo, CltchitrRomName, System18InputInfo, CltchitrDIPInfo,
	System18Bank40000Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvCltchtrj = {
	"cltchtrj", "cltchitr", NULL, "1991",
	"Clutch Hitter (set 1, Japan, FD1094 317-0175)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, CltchtrjRomInfo, CltchtrjRomName, System18InputInfo, CltchitrDIPInfo,
	System18Bank40000Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvDdcrew = {
	"ddcrew", NULL, NULL, "1991",
	"D. D. Crew (set 4, World, 3 Player, FD1094 317-0190)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, DdcrewRomInfo, DdcrewRomName, DdcrewInputInfo, DdcrewDIPInfo,
	System18Bank40000Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvDdcrew1 = {
	"ddcrew1", "ddcrew", NULL, "1991",
	"D. D. Crew (set 1, World, 4 Player, FD1094 317-?)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Ddcrew1RomInfo, Ddcrew1RomName, DdcrewuInputInfo, DdcrewuDIPInfo,
	DdcrewuInit, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvDdcrew2 = {
	"ddcrew2", "ddcrew", NULL, "1991",
	"D. D. Crew (set 2, World, 2 Player, FD1094 317-0184)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Ddcrew2RomInfo, Ddcrew2RomName, Ddcrew2InputInfo, Ddcrew2DIPInfo,
	System18Bank40000Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvDdcrewj = {
	"ddcrewj", "ddcrew", NULL, "1991",
	"D. D. Crew (set 5, Japan, 2 Player, FD1094 317-0182)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, DdcrewjRomInfo, DdcrewjRomName, Ddcrew2InputInfo, Ddcrew2DIPInfo,
	System18Bank40000Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvDdcrewu = {
	"ddcrewu", "ddcrew", NULL, "1991",
	"D. D. Crew (set 3, US, 4 Player, FD1094 317-0186)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 4, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, DdcrewuRomInfo, DdcrewuRomName, DdcrewuInputInfo, DdcrewuDIPInfo,
	DdcrewuInit, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvDesertbr = {
	"desertbr", NULL, NULL, "1992",
	"Desert Breaker (FD1094 317-0196)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, DesertbrRomInfo, DesertbrRomName, DesertbrInputInfo, DesertbrDIPInfo,
	System18Bank80000Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 224, 320, 3, 4
};

struct BurnDriver BurnDrvLghost = {
	"lghost", NULL, NULL, "1990",
	"Laser Ghost (set 2, World, 317-0166)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, LghostRomInfo, LghostRomName, LghostInputInfo, LghostDIPInfo,
	LghostInit, LghostExit, System18Frame, NULL, LghostScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvLghostu = {
	"lghostu", "lghost", NULL, "1990",
	"Laser Ghost (set 1, US, 317-0165)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, LghostuRomInfo, LghostuRomName, LghostInputInfo, LghostDIPInfo,
	LghostInit, LghostExit, System18Frame, NULL, LghostScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvMWalk = {
	"mwalk", NULL, NULL, "1990",
	"Michael Jackson's Moonwalker (set 3, World, FD1094/8751 317-0159)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	0, 3, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, MwalkRomInfo, MwalkRomName, MwalkInputInfo, MwalkDIPInfo,
	System16Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvMWalkbl = {
	"mwalkbl", "mwalk", NULL, "1990",
	"Michael Jackson's Moonwalker (bootleg)\0", "Incomplete Sound", "bootleg", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_BOOTLEG, 3, HARDWARE_SEGA_SYSTEM18,
	NULL, MwalkblRomInfo, MwalkblRomName, MwalkInputInfo, MwalkDIPInfo,
	MwalkblInit, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvShdancer = {
	"shdancer", NULL, NULL, "1989",
	"Shadow Dancer (set 3, US)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM18,
	NULL, ShdancerRomInfo, ShdancerRomName, System18InputInfo, ShdancerDIPInfo,
	System16Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvShdance1 = {
	"shdance1", "shdancer", NULL, "1989",
	"Shadow Dancer (set 1)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM18,
	NULL, Shdance1RomInfo, Shdance1RomName, System18InputInfo, ShdancerDIPInfo,
	System16Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvShdancej = {
	"shdancej", "shdancer", NULL, "1989",
	"Shadow Dancer (set 2, Japan)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM18,
	NULL, ShdancejRomInfo, ShdancejRomName, System18InputInfo, ShdancerDIPInfo,
	System16Init, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriverD BurnDrvShdancbl = {
	"shdancbl", "shdancer", NULL, "1989",
	"Shadow Dancer (bootleg)\0", "Incomplete Sound", "bootleg", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_CLONE | BDF_BOOTLEG, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_INVERT_TILES,
	NULL, ShdancblRomInfo, ShdancblRomName, System18InputInfo, ShdancerDIPInfo,
	ShdancblInit, System18Exit, System18Frame, NULL, System18Scan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvWwallyj = {
	"wwallyj", NULL, NULL, "1992",
	"Wally wo Sagase! (rev B, Japan, FD1094 317-0197B)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, WwallyjRomInfo, WwallyjRomName, WwallyInputInfo, WwallyDIPInfo,
	WwallyInit, WwallyExit, System18Frame, NULL, WwallyScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvWwallyja = {
	"wwallyja", "wwallyj", NULL, "1992",
	"Wally wo Sagase! (rev A, Japan, FD1094 317-0197A)\0", "Incomplete Sound", "Sega", "System 18",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEM18 | HARDWARE_SEGA_FD1094_ENC,
	NULL, WwallyjaRomInfo, WwallyjaRomName, WwallyInputInfo, WwallyDIPInfo,
	WwallyInit, WwallyExit, System18Frame, NULL, WwallyScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};
