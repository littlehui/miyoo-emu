#include "sys16.h"

/*====================================================
Input Defs
====================================================*/

#define A(a, b, c, d) {a, b, (unsigned char*)(c), d}

static struct BurnInputInfo Gforce2InputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Up/Down"          , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 y-axis"  ),
	A("Throttle"         , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 z-axis"  ),
	{"Shoot"             , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 1"  },
	{"Missile"           , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Gforce2);

static struct BurnInputInfo GlocInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Up/Down"          , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 y-axis"  ),
	A("Throttle"         , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 z-axis"  ),
	{"Fire 1"            , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 1"  },
	{"Fire 2"            , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 2"  },
	{"Fire 3"            , BIT_DIGITAL   , System16InputPort0 + 0, "p1 fire 3"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Gloc);

static struct BurnInputInfo Glocr360InputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Up/Down"          , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 y-axis"  ),
	A("Moving Roll"      , BIT_ANALOG_REL, &System16AnalogPort2,   "p2 x-axis"  ),
	A("Moving Pitch"     , BIT_ANALOG_REL, &System16AnalogPort3,   "p3 x-axis"  ),
	{"Fire 1"            , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 1"  },
	{"Fire 2"            , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Glocr360);

static struct BurnInputInfo PdriftInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Steering"         , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Accelerate"       , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 fire 1"  ),
	A("Brake"            , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 fire 2"  ),
	{"Gear"              , BIT_DIGITAL   , &System16Gear       ,   "p1 fire 3"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Pdrift);

static struct BurnInputInfo RchaseInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 4, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 7, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 5, "p2 coin"    },
	{"Start 2"           , BIT_DIGITAL   , System16InputPort0 + 6, "p2 start"   },

	A("P1 Left/Right"    , BIT_ANALOG_REL, &System16AnalogPort0,   "mouse x-axis"  ),
	A("P1 Up/Down"       , BIT_ANALOG_REL, &System16AnalogPort1,   "mouse y-axis"  ),
	{"P1 Fire 1"         , BIT_DIGITAL   , System16InputPort0 + 1, "mouse button 1"  },
	
	A("P2 Left/Right"    , BIT_ANALOG_REL, &System16AnalogPort2,   "p2 x-axis"  ),
	A("P2 Up/Down"       , BIT_ANALOG_REL, &System16AnalogPort3,   "p2 y-axis"  ),
	{"P2 Fire 1"         , BIT_DIGITAL   , System16InputPort0 + 0, "p2 fire 1"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 3 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 2 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Rchase);

#undef A

/*====================================================
Dip Defs
====================================================*/

#define YBOARD_COINAGE(dipval)								\
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

static struct BurnDIPInfo Gforce2DIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0x7e, NULL                                 },
	{0x0c, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0b, 0x01, 0x01, 0x01, "Off"                                },
	{0x0b, 0x01, 0x01, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Energy Timer"                       },
	{0x0b, 0x01, 0x06, 0x04, "Easy"                               },
	{0x0b, 0x01, 0x06, 0x06, "Normal"                             },
	{0x0b, 0x01, 0x06, 0x02, "Hard"                               },
	{0x0b, 0x01, 0x06, 0x00, "Hardest"                            },
	
	{0   , 0xfe, 0   , 2   , "Shield Strength"                    },
	{0x0b, 0x01, 0x08, 0x08, "Weak"                               },
	{0x0b, 0x01, 0x08, 0x00, "Strong"                             },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0b, 0x01, 0x30, 0x20, "Easy"                               },
	{0x0b, 0x01, 0x30, 0x30, "Normal"                             },
	{0x0b, 0x01, 0x30, 0x10, "Hard"                               },
	{0x0b, 0x01, 0x30, 0x00, "Hardest"                            },
	
	{0   , 0xfe, 0   , 4   , "Cabinet"                            },
	{0x0b, 0x01, 0xc0, 0xc0, "Super Deluxe"                       },
	{0x0b, 0x01, 0xc0, 0x80, "Deluxe"                             },
	{0x0b, 0x01, 0xc0, 0x40, "Upright"                            },
	{0x0b, 0x01, 0xc0, 0x00, "City"                               },
	
	// Dip 2
	YBOARD_COINAGE(0x0c)
};

STDDIPINFO(Gforce2);

static struct BurnDIPInfo GlocDIPList[]=
{
	// Default Values
	{0x0c, 0xff, 0xff, 0x6b, NULL                                 },
	{0x0d, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0x03, 0x02, "Easy"                               },
	{0x0c, 0x01, 0x03, 0x03, "Normal"                             },
	{0x0c, 0x01, 0x03, 0x01, "Hard"                               },
	{0x0c, 0x01, 0x03, 0x00, "Hardest"                            },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x04, 0x04, "Off"                                },
	{0x0c, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 3   , "Cabinet"                            },
	{0x0c, 0x01, 0x18, 0x18, "Moving"                             },
	{0x0c, 0x01, 0x18, 0x10, "Cockpit"                            },
	{0x0c, 0x01, 0x18, 0x08, "Upright"                            },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x20, 0x00, "No"                                 },
	{0x0c, 0x01, 0x20, 0x20, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Credits"                            },
	{0x0c, 0x01, 0xc0, 0x40, "1 to start, 1 to continue"          },
	{0x0c, 0x01, 0xc0, 0xc0, "2 to start, 1 to continue"          },
	{0x0c, 0x01, 0xc0, 0x80, "3 to start, 2 to continue"          },
	{0x0c, 0x01, 0xc0, 0x00, "4 to start, 3 to continue"          },
	
	// Dip 2
	YBOARD_COINAGE(0x0d)
};

STDDIPINFO(Gloc);

static struct BurnDIPInfo Glocr360DIPList[]=
{
	// Default Values
	{0x0c, 0xff, 0xff, 0xf3, NULL                                 },
	{0x0d, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 3   , "Game Type"                          },
	{0x0c, 0x01, 0x03, 0x02, "Fighting Only"                      },
	{0x0c, 0x01, 0x03, 0x03, "Fight/Experience"                   },
	{0x0c, 0x01, 0x03, 0x01, "Experience Only"                    },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x04, 0x04, "Off"                                },
	{0x0c, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 9   , "Initial Credit"                     },
	{0x0c, 0x01, 0xf0, 0xf0, "1"                                  },
	{0x0c, 0x01, 0xf0, 0xe0, "2"                                  },
	{0x0c, 0x01, 0xf0, 0xd0, "3"                                  },
	{0x0c, 0x01, 0xf0, 0xc0, "4"                                  },
	{0x0c, 0x01, 0xf0, 0xb0, "5"                                  },
	{0x0c, 0x01, 0xf0, 0xa0, "6"                                  },
	{0x0c, 0x01, 0xf0, 0x90, "8"                                  },
	{0x0c, 0x01, 0xf0, 0x80, "10"                                 },
	{0x0c, 0x01, 0xf0, 0x70, "12"                                 },
	
	// Dip 2
	{0   , 0xfe, 0   , 16  , "Coin A"                             },
	{0x0d, 0x01, 0x0f, 0x0f, "1 Coin  1 Credit"                   },
	{0x0d, 0x01, 0x0f, 0x0e, "1 Coin  2 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x0d, "1 Coin  3 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x0c, "1 Coin  4 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x0b, "1 Coin  5 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x0a, "1 Coin  6 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x09, "1 Coin  7 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x08, "1 Coin  8 Credits"                  },
	{0x0d, 0x01, 0x0f, 0x00, "Free Play (if coin B too) or 1C/1C" },

	{0   , 0xfe, 0   , 16  , "Coin B"                             },
	{0x0d, 0x01, 0xf0, 0xf0, "1 Coin  1 Credit"                   },
	{0x0d, 0x01, 0xf0, 0xe0, "1 Coin  2 Credits"                  },
	{0x0d, 0x01, 0xf0, 0xd0, "1 Coin  3 Credits"                  },
	{0x0d, 0x01, 0xf0, 0xc0, "1 Coin  4 Credits"                  },
	{0x0d, 0x01, 0xf0, 0xb0, "1 Coin  5 Credits"                  },
	{0x0d, 0x01, 0xf0, 0xa0, "1 Coin  6 Credits"                  },
	{0x0d, 0x01, 0xf0, 0x90, "1 Coin  7 Credits"                  },
	{0x0d, 0x01, 0xf0, 0x80, "1 Coin  8 Credits"                  },
	{0x0d, 0x01, 0xf0, 0x00, "Free Play (if coin A too) or 1C/1C" },
};

STDDIPINFO(Glocr360);

static struct BurnDIPInfo PdriftDIPList[]=
{
	// Default Values
	{0x0a, 0xff, 0xff, 0xea, NULL                                 },
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 3   , "Cabinet"                            },
	{0x0a, 0x01, 0x03, 0x03, "Moving"                             },
	{0x0a, 0x01, 0x03, 0x02, "Upright/Sit Down"                   },
	{0x0a, 0x01, 0x03, 0x01, "Mini Upright"                       },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0a, 0x01, 0x04, 0x04, "Off"                                },
	{0x0a, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Credits"                            },
	{0x0a, 0x01, 0x08, 0x08, "1 to start, 1 to continue"          },
	{0x0a, 0x01, 0x08, 0x18, "2 to start, 1 to continue"          },
	{0x0a, 0x01, 0x08, 0x00, "2 to start, 2 to continue"          },
	{0x0a, 0x01, 0x08, 0x10, "3 to start, 2 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0a, 0x01, 0x20, 0x00, "No"                                 },
	{0x0a, 0x01, 0x20, 0x20, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0a, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x0a, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0a, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x0a, 0x01, 0xc0, 0x00, "Hardest"                            },

	// Dip 2
	YBOARD_COINAGE(0x0b)
};

STDDIPINFO(Pdrift);

static struct BurnDIPInfo PdrifteDIPList[]=
{
	// Default Values
	{0x0a, 0xff, 0xff, 0xea, NULL                                 },
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 3   , "Cabinet"                            },
	{0x0a, 0x01, 0x03, 0x03, "Moving"                             },
	{0x0a, 0x01, 0x03, 0x02, "Upright"                            },
	{0x0a, 0x01, 0x03, 0x01, "Mini Upright"                       },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0a, 0x01, 0x04, 0x04, "Off"                                },
	{0x0a, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Initial Credit"                     },
	{0x0a, 0x01, 0x10, 0x00, "1"                                  },
	{0x0a, 0x01, 0x10, 0x10, "2"                                  },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0a, 0x01, 0x20, 0x00, "No"                                 },
	{0x0a, 0x01, 0x20, 0x20, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0a, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x0a, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0a, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x0a, 0x01, 0xc0, 0x00, "Hardest"                            },

	// Dip 2
	YBOARD_COINAGE(0x0b)
};

STDDIPINFO(Pdrifte);

static struct BurnDIPInfo PdriftjDIPList[]=
{
	// Default Values
	{0x0a, 0xff, 0xff, 0xea, NULL                                 },
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 3   , "Cabinet"                            },
	{0x0a, 0x01, 0x03, 0x03, "Moving"                             },
	{0x0a, 0x01, 0x03, 0x02, "Upright/Sit Down"                   },
	{0x0a, 0x01, 0x03, 0x01, "Mini Upright"                       },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0a, 0x01, 0x04, 0x04, "Off"                                },
	{0x0a, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Initial Credit"                     },
	{0x0a, 0x01, 0x10, 0x00, "1"                                  },
	{0x0a, 0x01, 0x10, 0x10, "2"                                  },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0a, 0x01, 0x20, 0x00, "No"                                 },
	{0x0a, 0x01, 0x20, 0x20, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0a, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x0a, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0a, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x0a, 0x01, 0xc0, 0x00, "Hardest"                            },

	// Dip 2
	YBOARD_COINAGE(0x0b)
};

STDDIPINFO(Pdriftj);

static struct BurnDIPInfo RchaseDIPList[]=
{
	// Default Values
	{0x0d, 0xff, 0xff, 0x63, NULL                                 },
	{0x0e, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 4   , "Credits"                            },
	{0x0d, 0x01, 0x03, 0x03, "1 to start, 1 to continue"          },
	{0x0d, 0x01, 0x03, 0x02, "2 to start, 1 to continue"          },
	{0x0d, 0x01, 0x03, 0x01, "3 to start, 2 to continue"          },
	{0x0d, 0x01, 0x03, 0x00, "4 to start, 3 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x0d, 0x01, 0x04, 0x04, "Single"                             },
	{0x0d, 0x01, 0x04, 0x00, "Twin"                               },
	
	{0   , 0xfe, 0   , 2   , "Cabinet"                            },
	{0x0d, 0x01, 0x08, 0x08, "Moving"                             },
	{0x0d, 0x01, 0x08, 0x00, "Upright"                            },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0d, 0x01, 0x60, 0x40, "Easy"                               },
	{0x0d, 0x01, 0x60, 0x60, "Normal"                             },
	{0x0d, 0x01, 0x60, 0x20, "Hard"                               },
	{0x0d, 0x01, 0x60, 0x00, "Hardest"                            },
		
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0d, 0x01, 0x80, 0x80, "Off"                                },
	{0x0d, 0x01, 0x80, 0x00, "On"                                 },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Coin to Credit"                     },
	{0x0e, 0x01, 0x03, 0x00, "4 Coins 1 Credit"                   },
	{0x0e, 0x01, 0x03, 0x01, "3 Coins 1 Credit"                   },
	{0x0e, 0x01, 0x03, 0x02, "2 Coins 1 Credit"                   },
	{0x0e, 0x01, 0x03, 0x03, "1 Coins 1 Credit"                   },
	
	{0   , 0xfe, 0   , 2   , "Coin #1 mulitplier"                 },
	{0x0e, 0x01, 0x04, 0x04, "x1"                                 },
	{0x0e, 0x01, 0x04, 0x00, "x2"                                 },
	
	{0   , 0xfe, 0   , 4   , "Coin #2 mulitplier"                 },
	{0x0e, 0x01, 0x18, 0x18, "x1"                                 },
	{0x0e, 0x01, 0x18, 0x10, "x4"                                 },
	{0x0e, 0x01, 0x18, 0x08, "x5"                                 },
	{0x0e, 0x01, 0x18, 0x00, "x6"                                 },
	
	{0   , 0xfe, 0   , 8   , "Bonus Adder"                        },
	{0x0e, 0x01, 0xe0, 0xe0, "None"                               },
	{0x0e, 0x01, 0xe0, 0xc0, "2 gives 1 more"                     },
	{0x0e, 0x01, 0xe0, 0xa0, "3 gives 1 more"                     },
	{0x0e, 0x01, 0xe0, 0x80, "4 gives 1 more"                     },
	{0x0e, 0x01, 0xe0, 0x60, "5 gives 1 more"                     },
	{0x0e, 0x01, 0xe0, 0x40, "6 gives 1 more"                     },
	{0x0e, 0x01, 0xe0, 0x20, "7 gives 1 more"                     },
	{0x0e, 0x01, 0xe0, 0x00, "Error"                              },
};

STDDIPINFO(Rchase);

static struct BurnDIPInfo StrkfgtrDIPList[]=
{
	// Default Values
	{0x0c, 0xff, 0xff, 0x5b, NULL                                 },
	{0x0d, 0xff, 0xff, 0xff, NULL                                 },

	// Dip 1
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0x03, 0x02, "Easy"                               },
	{0x0c, 0x01, 0x03, 0x03, "Normal"                             },
	{0x0c, 0x01, 0x03, 0x01, "Hard"                               },
	{0x0c, 0x01, 0x03, 0x00, "Hardest"                            },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x04, 0x04, "Off"                                },
	{0x0c, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x08, 0x00, "No"                                 },
	{0x0c, 0x01, 0x08, 0x08, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Credits"                            },
	{0x0c, 0x01, 0x30, 0x10, "1 to start, 1 to continue"          },
	{0x0c, 0x01, 0x30, 0x30, "2 to start, 1 to continue"          },
	{0x0c, 0x01, 0x30, 0x20, "3 to start, 2 to continue"          },
	{0x0c, 0x01, 0x30, 0x00, "4 to start, 3 to continue"          },
	
	{0   , 0xfe, 0   , 3   , "Cabinet"                            },
	{0x0c, 0x01, 0xc0, 0xc0, "Moving"                             },
	{0x0c, 0x01, 0xc0, 0x80, "Cockpit"                            },
	{0x0c, 0x01, 0xc0, 0x40, "Upright"                            },
		
	// Dip 2
	YBOARD_COINAGE(0x0d)
};

STDDIPINFO(Strkfgtr);

#undef YBOARD_COINAGE

/*====================================================
Rom Defs
====================================================*/

static struct BurnRomInfo Gforce2RomDesc[] = {
	{ "epr-11688.bin",    0x20000, 0xc845f2df, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11687.bin",    0x20000, 0x1cbefbbf, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11875.bin",    0x20000, 0xc81701c6, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11874.bin",    0x20000, 0x5301fd79, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-11816.bin",    0x20000, 0x317dd0c2, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-11815.bin",    0x20000, 0xf1fb22f1, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-11468.bin",    0x20000, 0x74ca9ca5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11467.bin",    0x20000, 0x6e60e736, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11695.bin",    0x20000, 0x38a864be, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11694.bin",    0x20000, 0x7e297b84, SYS16_ROM_SPRITES | BRF_GRA },
		
	{ "epr-11469.bin",    0x20000, 0xed7a2299, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11470.bin",    0x20000, 0x34dea550, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11477.bin",    0x20000, 0xa2784653, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11478.bin",    0x20000, 0x8b778993, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11471.bin",    0x20000, 0xf1974069, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11472.bin",    0x20000, 0x0d24409a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11479.bin",    0x20000, 0xecd6138a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11480.bin",    0x20000, 0x64ad66c5, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11473.bin",    0x20000, 0x0538c6ec, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11474.bin",    0x20000, 0xeb923c50, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11481.bin",    0x20000, 0x78e652b6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11482.bin",    0x20000, 0x2f879766, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11475.bin",    0x20000, 0x69cfec89, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11476.bin",    0x20000, 0xa60b9b79, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11483.bin",    0x20000, 0xd5d3a505, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11484.bin",    0x20000, 0xb8a56a50, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11696.bin",    0x20000, 0x99e8e49e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11697.bin",    0x20000, 0x7545c52e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11700.bin",    0x20000, 0xe13839c1, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11701.bin",    0x20000, 0x9fb3d365, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11698.bin",    0x20000, 0xcfeba3e2, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11699.bin",    0x20000, 0x4a00534a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11702.bin",    0x20000, 0x2a09c627, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11703.bin",    0x20000, 0x43bb7d9f, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11524.bin",    0x20000, 0x5d35849f, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11525.bin",    0x20000, 0x9ae47552, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11532.bin",    0x20000, 0xb3565ddb, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11533.bin",    0x20000, 0xf5d16e8a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11526.bin",    0x20000, 0x094cb3f0, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11527.bin",    0x20000, 0xe821a144, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11534.bin",    0x20000, 0xb7f0ad7c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11535.bin",    0x20000, 0x95da7a46, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11693.bin",    0x10000, 0x0497785c, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11465.bin",    0x80000, 0xe1436dab, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11516.bin",    0x20000, 0x19d0e17f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11814.bin",    0x20000, 0x0b05d376, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Gforce2);
STD_ROM_FN(Gforce2);

static struct BurnRomInfo Gforce2jRomDesc[] = {
	{ "epr-11511.bin",    0x20000, 0xd80a86d6, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11510.bin",    0x20000, 0xd2b1bef4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11515.bin",    0x20000, 0xd85875cf, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11514.bin",    0x20000, 0x3dcc6919, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-11513.bin",    0x20000, 0xe18bc177, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-11512.bin",    0x20000, 0x6010e63e, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-11468.bin",    0x20000, 0x74ca9ca5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11467.bin",    0x20000, 0x6e60e736, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11695.bin",    0x20000, 0x38a864be, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11694.bin",    0x20000, 0x7e297b84, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11469.bin",    0x20000, 0xed7a2299, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11470.bin",    0x20000, 0x34dea550, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11477.bin",    0x20000, 0xa2784653, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11478.bin",    0x20000, 0x8b778993, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11471.bin",    0x20000, 0xf1974069, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11472.bin",    0x20000, 0x0d24409a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11479.bin",    0x20000, 0xecd6138a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11480.bin",    0x20000, 0x64ad66c5, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11473.bin",    0x20000, 0x0538c6ec, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11474.bin",    0x20000, 0xeb923c50, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11481.bin",    0x20000, 0x78e652b6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11482.bin",    0x20000, 0x2f879766, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11475.bin",    0x20000, 0x69cfec89, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11476.bin",    0x20000, 0xa60b9b79, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11483.bin",    0x20000, 0xd5d3a505, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11484.bin",    0x20000, 0xb8a56a50, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11696.bin",    0x20000, 0x99e8e49e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11697.bin",    0x20000, 0x7545c52e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11700.bin",    0x20000, 0xe13839c1, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11701.bin",    0x20000, 0x9fb3d365, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11698.bin",    0x20000, 0xcfeba3e2, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11699.bin",    0x20000, 0x4a00534a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11702.bin",    0x20000, 0x2a09c627, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11703.bin",    0x20000, 0x43bb7d9f, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11524.bin",    0x20000, 0x5d35849f, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11525.bin",    0x20000, 0x9ae47552, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11532.bin",    0x20000, 0xb3565ddb, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11533.bin",    0x20000, 0xf5d16e8a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11526.bin",    0x20000, 0x094cb3f0, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11527.bin",    0x20000, 0xe821a144, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11534.bin",    0x20000, 0xb7f0ad7c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11535.bin",    0x20000, 0x95da7a46, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11693.bin",    0x10000, 0x0497785c, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11465.bin",    0x80000, 0xe1436dab, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11516.bin",    0x20000, 0x19d0e17f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11814.bin",    0x20000, 0x0b05d376, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Gforce2j);
STD_ROM_FN(Gforce2j);

static struct BurnRomInfo GlocRomDesc[] = {
	{ "epr-13170.25",     0x20000, 0x45189229, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13169.24",     0x20000, 0x1b47cd6e, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13028.27",     0x20000, 0xb6aa2edf, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13027.26",     0x20000, 0x6463c87a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13032.81",     0x20000, 0x7da09c4e, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13031.80",     0x20000, 0xf3c7e3f4, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-13030.54",     0x20000, 0x81abcabf, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-13029.53",     0x20000, 0xf3638efb, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-13037.14",     0x80000, 0xb801a250, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13039.16",     0x80000, 0xd7e1266d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13038.15",     0x80000, 0x0b2edb6d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13040.17",     0x80000, 0x4aeb3a85, SYS16_ROM_SPRITES | BRF_GRA },
		
	{ "epr-13048.67",     0x80000, 0xfe1eb0dd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13056.75",     0x80000, 0x5904f8e6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13044.63",     0x80000, 0x4d931f89, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13052.71",     0x80000, 0x0291f040, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13064.86",     0x80000, 0x5f8e651b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13072.114",    0x80000, 0x6b85641a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13060.82",     0x80000, 0xee16ad97, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13068.110",    0x80000, 0x64d52bbb, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13047.66",     0x80000, 0x53340832, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13055.74",     0x80000, 0x39b6b665, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13043.62",     0x80000, 0x208f16fd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13051.70",     0x80000, 0xad62cbd4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13063.85",     0x80000, 0xc580bf6d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13071.113",    0x80000, 0xdf99ef99, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13059.81",     0x80000, 0x4c982558, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13067.109",    0x80000, 0xf97f6119, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13046.65",     0x80000, 0xc75a86e9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13054.73",     0x80000, 0x2934549a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13042.61",     0x80000, 0x53ed97af, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13050.69",     0x80000, 0x04429068, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13062.84",     0x80000, 0x4fdb4ee3, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13070.112",    0x80000, 0x52ea130e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13058.80",     0x80000, 0x19ff1626, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13066.108",    0x80000, 0xbc70a250, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13045.64",     0x80000, 0x54d5bc6d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13053.72",     0x80000, 0x9502af13, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13041.60",     0x80000, 0xd0a7402c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13049.68",     0x80000, 0x5b9c0b6c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13061.83",     0x80000, 0x7b95ec3b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13069.111",    0x80000, 0xe1f538f0, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13057.79",     0x80000, 0x73baefee, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13065.107",    0x80000, 0x8937a655, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-13033.102",    0x10000, 0x6df5e827, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13036.107",    0x80000, 0x7890c26c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13035.106",    0x80000, 0x009fa13e, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13034.105",    0x80000, 0xcd22d95d, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Gloc);
STD_ROM_FN(Gloc);

static struct BurnRomInfo Glocr360RomDesc[] = {
	{ "epr-13623.25",     0x20000, 0x58ad10e7, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13622.24",     0x20000, 0xc4e68dbf, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13323a.27",    0x20000, 0x02e24a33, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13322a.26",    0x20000, 0x94f67740, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13327.81",     0x20000, 0x627036f9, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13326.80",     0x20000, 0x162ac233, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-13325a.54",    0x20000, 0xaba307e5, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-13324a.53",    0x20000, 0xeb1b19e5, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-13037.14",     0x80000, 0xb801a250, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13039.16",     0x80000, 0xd7e1266d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13038.15",     0x80000, 0x0b2edb6d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13040.17",     0x80000, 0x4aeb3a85, SYS16_ROM_SPRITES | BRF_GRA },
		
	{ "epr-13048.67",     0x80000, 0xfe1eb0dd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13056.75",     0x80000, 0x5904f8e6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13044.63",     0x80000, 0x4d931f89, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13052.71",     0x80000, 0x0291f040, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13064.86",     0x80000, 0x5f8e651b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13072.114",    0x80000, 0x6b85641a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13060.82",     0x80000, 0xee16ad97, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13068.110",    0x80000, 0x64d52bbb, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13047.66",     0x80000, 0x53340832, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13055.74",     0x80000, 0x39b6b665, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13043.62",     0x80000, 0x208f16fd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13051.70",     0x80000, 0xad62cbd4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13063.85",     0x80000, 0xc580bf6d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13071.113",    0x80000, 0xdf99ef99, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13059.81",     0x80000, 0x4c982558, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13067.109",    0x80000, 0xf97f6119, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13331.65",     0x80000, 0x8ea8febe, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13333.73",     0x80000, 0x5bcd37d4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13330.61",     0x80000, 0x1e325d52, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13332.69",     0x80000, 0x8fd8067e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13335.84",     0x80000, 0x98ea420b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13337.112",    0x80000, 0xf55f00a4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13334.80",     0x80000, 0x72725060, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13336.108",    0x80000, 0xe2d4d477, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13045.64",     0x80000, 0x54d5bc6d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13053.72",     0x80000, 0x9502af13, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13041.60",     0x80000, 0xd0a7402c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13049.68",     0x80000, 0x5b9c0b6c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13061.83",     0x80000, 0x7b95ec3b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13069.111",    0x80000, 0xe1f538f0, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13057.79",     0x80000, 0x73baefee, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13065.107",    0x80000, 0x8937a655, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-13624.102",    0x10000, 0xeff33f2d, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13036.107",    0x80000, 0x7890c26c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13035.106",    0x80000, 0x009fa13e, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13625.105",    0x80000, 0xfae71fd2, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Glocr360);
STD_ROM_FN(Glocr360);

static struct BurnRomInfo PdriftRomDesc[] = {
	{ "epr-12017.25",     0x20000, 0x31190322, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-12016.24",     0x20000, 0x499f64a6, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11748.27",     0x20000, 0x82a76cab, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11747.26",     0x20000, 0x9796ece5, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11905.81",     0x20000, 0x1cf68109, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11904.80",     0x20000, 0xbb993681, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-12019a.54",    0x20000, 0x11188a30, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-12018a.53",    0x20000, 0x1c582e1f, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-11791.14",     0x20000, 0x36b2910a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11789.16",     0x20000, 0xb86f8d2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11792.15",     0x20000, 0xc85caf6e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11790.17",     0x20000, 0x2a564e66, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11757.67",     0x20000, 0xe46dc478, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11758.75",     0x20000, 0x5b435c87, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11773.63",     0x20000, 0x1b5d5758, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11774.71",     0x20000, 0x2ca0c170, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11759.86",     0x20000, 0xac8111f6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11760.114",    0x20000, 0x91282af9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11775.82",     0x20000, 0x48225793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11776.110",    0x20000, 0x78c46198, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11761.66",     0x20000, 0xbaa5d065, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11762.74",     0x20000, 0x1d1af7a5, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11777.62",     0x20000, 0x9662dd32, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11778.70",     0x20000, 0x2dfb7494, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11763.85",     0x20000, 0x1ee23407, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11764.113",    0x20000, 0xe859305e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11779.81",     0x20000, 0xa49cd793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11780.109",    0x20000, 0xd514ed81, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11765.65",     0x20000, 0x649e2dff, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11766.73",     0x20000, 0xd92fb7fc, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11781.61",     0x20000, 0x9692d4cd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11782.69",     0x20000, 0xc913bb43, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11767.84",     0x20000, 0x1f8ad054, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11768.112",    0x20000, 0xdb2c4053, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11783.80",     0x20000, 0x6d189007, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11784.108",    0x20000, 0x57f5fd64, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11769.64",     0x20000, 0x28f0ab51, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11770.72",     0x20000, 0xd7557ea9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11785.60",     0x20000, 0xe6ef32c4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11786.68",     0x20000, 0x2066b49d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11771.83",     0x20000, 0x67635618, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11772.111",    0x20000, 0x0f798d3a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11787.79",     0x20000, 0xe631dc12, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11788.107",    0x20000, 0x8464c66e, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-11899.102",    0x10000, 0xed9fa889, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-11754.107",    0x80000, 0xebeb8484, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11755.105",    0x20000, 0x12e43f8a, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11756.106",    0x20000, 0xc2db1244, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "epr-11895.ic1",    0x20000, 0xee99a6fd, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11896.ic2",    0x20000, 0x4bebc015, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11897.ic3",    0x20000, 0x4463cb95, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11898.ic4",    0x20000, 0x5d19d767, SYS16_ROM_PROM | BRF_OPT },
};


STD_ROM_PICK(Pdrift);
STD_ROM_FN(Pdrift);

static struct BurnRomInfo PdriftaRomDesc[] = {
	{ "epr-12017.25",     0x20000, 0x31190322, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-12016.24",     0x20000, 0x499f64a6, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11748.27",     0x20000, 0x82a76cab, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11747.26",     0x20000, 0x9796ece5, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11905.81",     0x20000, 0x1cf68109, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11904.80",     0x20000, 0xbb993681, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-12019.54",     0x20000, 0xe514d7b6, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-12018.53",     0x20000, 0x0a3f7faf, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-11791.14",     0x20000, 0x36b2910a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11789.16",     0x20000, 0xb86f8d2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11792.15",     0x20000, 0xc85caf6e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11790.17",     0x20000, 0x2a564e66, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11757.67",     0x20000, 0xe46dc478, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11758.75",     0x20000, 0x5b435c87, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11773.63",     0x20000, 0x1b5d5758, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11774.71",     0x20000, 0x2ca0c170, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11759.86",     0x20000, 0xac8111f6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11760.114",    0x20000, 0x91282af9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11775.82",     0x20000, 0x48225793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11776.110",    0x20000, 0x78c46198, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11761.66",     0x20000, 0xbaa5d065, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11762.74",     0x20000, 0x1d1af7a5, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11777.62",     0x20000, 0x9662dd32, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11778.70",     0x20000, 0x2dfb7494, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11763.85",     0x20000, 0x1ee23407, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11764.113",    0x20000, 0xe859305e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11779.81",     0x20000, 0xa49cd793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11780.109",    0x20000, 0xd514ed81, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11765.65",     0x20000, 0x649e2dff, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11766.73",     0x20000, 0xd92fb7fc, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11781.61",     0x20000, 0x9692d4cd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11782.69",     0x20000, 0xc913bb43, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11767.84",     0x20000, 0x1f8ad054, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11768.112",    0x20000, 0xdb2c4053, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11783.80",     0x20000, 0x6d189007, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11784.108",    0x20000, 0x57f5fd64, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11769.64",     0x20000, 0x28f0ab51, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11770.72",     0x20000, 0xd7557ea9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11785.60",     0x20000, 0xe6ef32c4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11786.68",     0x20000, 0x2066b49d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11771.83",     0x20000, 0x67635618, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11772.111",    0x20000, 0x0f798d3a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11787.79",     0x20000, 0xe631dc12, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11788.107",    0x20000, 0x8464c66e, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-11899.102",    0x10000, 0xed9fa889, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-11754.107",    0x80000, 0xebeb8484, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11755.105",    0x20000, 0x12e43f8a, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11756.106",    0x20000, 0xc2db1244, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "epr-11895.ic1",    0x20000, 0xee99a6fd, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11896.ic2",    0x20000, 0x4bebc015, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11897.ic3",    0x20000, 0x4463cb95, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11898.ic4",    0x20000, 0x5d19d767, SYS16_ROM_PROM | BRF_OPT },
};


STD_ROM_PICK(Pdrifta);
STD_ROM_FN(Pdrifta);

static struct BurnRomInfo PdrifteRomDesc[] = {
	{ "epr-11901.25",     0x20000, 0x16744be8, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11900.24",     0x20000, 0x0a170d06, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11748.27",     0x20000, 0x82a76cab, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11747.26",     0x20000, 0x9796ece5, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11905.81",     0x20000, 0x1cf68109, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11904.80",     0x20000, 0xbb993681, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-11903.54",     0x20000, 0xd004f411, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-11902.53",     0x20000, 0xe8028e08, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-11791.14",     0x20000, 0x36b2910a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11789.16",     0x20000, 0xb86f8d2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11792.15",     0x20000, 0xc85caf6e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11790.17",     0x20000, 0x2a564e66, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11757.67",     0x20000, 0xe46dc478, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11758.75",     0x20000, 0x5b435c87, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11773.63",     0x20000, 0x1b5d5758, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11774.71",     0x20000, 0x2ca0c170, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11759.86",     0x20000, 0xac8111f6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11760.114",    0x20000, 0x91282af9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11775.82",     0x20000, 0x48225793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11776.110",    0x20000, 0x78c46198, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11761.66",     0x20000, 0xbaa5d065, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11762.74",     0x20000, 0x1d1af7a5, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11777.62",     0x20000, 0x9662dd32, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11778.70",     0x20000, 0x2dfb7494, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11763.85",     0x20000, 0x1ee23407, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11764.113",    0x20000, 0xe859305e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11779.81",     0x20000, 0xa49cd793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11780.109",    0x20000, 0xd514ed81, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11765.65",     0x20000, 0x649e2dff, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11766.73",     0x20000, 0xd92fb7fc, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11781.61",     0x20000, 0x9692d4cd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11782.69",     0x20000, 0xc913bb43, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11767.84",     0x20000, 0x1f8ad054, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11768.112",    0x20000, 0xdb2c4053, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11783.80",     0x20000, 0x6d189007, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11784.108",    0x20000, 0x57f5fd64, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11769.64",     0x20000, 0x28f0ab51, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11770.72",     0x20000, 0xd7557ea9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11785.60",     0x20000, 0xe6ef32c4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11786.68",     0x20000, 0x2066b49d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11771.83",     0x20000, 0x67635618, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11772.111",    0x20000, 0x0f798d3a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11787.79",     0x20000, 0xe631dc12, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11788.107",    0x20000, 0x8464c66e, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-11899.102",    0x10000, 0xed9fa889, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-11754.107",    0x80000, 0xebeb8484, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11755.105",    0x20000, 0x12e43f8a, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11756.106",    0x20000, 0xc2db1244, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "epr-11895.ic1",    0x20000, 0xee99a6fd, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11896.ic2",    0x20000, 0x4bebc015, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11897.ic3",    0x20000, 0x4463cb95, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11898.ic4",    0x20000, 0x5d19d767, SYS16_ROM_PROM | BRF_OPT },
};


STD_ROM_PICK(Pdrifte);
STD_ROM_FN(Pdrifte);

static struct BurnRomInfo PdriftjRomDesc[] = {
	{ "epr-11746a.25",    0x20000, 0xb0f1caf4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11745a.24",    0x20000, 0xa89720cd, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11748.27",     0x20000, 0x82a76cab, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11747.26",     0x20000, 0x9796ece5, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11752.81",     0x20000, 0xb6bb8111, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11751.80",     0x20000, 0x7f0d0311, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-11750b.54",    0x20000, 0xbc14ce30, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-11749b.53",    0x20000, 0x9e385568, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-11791.14",     0x20000, 0x36b2910a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11789.16",     0x20000, 0xb86f8d2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11792.15",     0x20000, 0xc85caf6e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11790.17",     0x20000, 0x2a564e66, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11757.67",     0x20000, 0xe46dc478, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11758.75",     0x20000, 0x5b435c87, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11773.63",     0x20000, 0x1b5d5758, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11774.71",     0x20000, 0x2ca0c170, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11759.86",     0x20000, 0xac8111f6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11760.114",    0x20000, 0x91282af9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11775.82",     0x20000, 0x48225793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11776.110",    0x20000, 0x78c46198, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11761.66",     0x20000, 0xbaa5d065, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11762.74",     0x20000, 0x1d1af7a5, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11777.62",     0x20000, 0x9662dd32, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11778.70",     0x20000, 0x2dfb7494, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11763.85",     0x20000, 0x1ee23407, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11764.113",    0x20000, 0xe859305e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11779.81",     0x20000, 0xa49cd793, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11780.109",    0x20000, 0xd514ed81, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11765.65",     0x20000, 0x649e2dff, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11766.73",     0x20000, 0xd92fb7fc, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11781.61",     0x20000, 0x9692d4cd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11782.69",     0x20000, 0xc913bb43, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11767.84",     0x20000, 0x1f8ad054, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11768.112",    0x20000, 0xdb2c4053, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11783.80",     0x20000, 0x6d189007, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11784.108",    0x20000, 0x57f5fd64, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-11769.64",     0x20000, 0x28f0ab51, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11770.72",     0x20000, 0xd7557ea9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11785.60",     0x20000, 0xe6ef32c4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11786.68",     0x20000, 0x2066b49d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11771.83",     0x20000, 0x67635618, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11772.111",    0x20000, 0x0f798d3a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11787.79",     0x20000, 0xe631dc12, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-11788.107",    0x20000, 0x8464c66e, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-11899.102",    0x10000, 0xed9fa889, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-11754.107",    0x80000, 0xebeb8484, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11755.105",    0x20000, 0x12e43f8a, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11756.106",    0x20000, 0xc2db1244, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "epr-11895.ic1",    0x20000, 0xee99a6fd, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11896.ic2",    0x20000, 0x4bebc015, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11897.ic3",    0x20000, 0x4463cb95, SYS16_ROM_PROM | BRF_OPT },
	{ "epr-11898.ic4",    0x20000, 0x5d19d767, SYS16_ROM_PROM | BRF_OPT },
};


STD_ROM_PICK(Pdriftj);
STD_ROM_FN(Pdriftj);

static struct BurnRomInfo RchaseRomDesc[] = {
	{ "ic25.bin",         0x20000, 0x388b2365, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic24.bin",         0x20000, 0x14dba5d4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic27.bin",         0x20000, 0xdc1cd5a4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ic26.bin",         0x20000, 0x969fdb3a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "ic81.bin",         0x20000, 0xc5d525b6, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "ic80.bin",         0x20000, 0x299e3c7c, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "ic54.bin",         0x20000, 0x18eb23c5, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "ic53.bin",         0x20000, 0x8f4f824e, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "vic14.bin",        0x40000, 0x1fdf1b87, SYS16_ROM_SPRITES | BRF_GRA },
	{ "vic16.bin",        0x40000, 0x9a1dd53c, SYS16_ROM_SPRITES | BRF_GRA },
		
	{ "vic67.bin",        0x80000, 0x9fa88781, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic75.bin",        0x80000, 0x49e824bb, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic63.bin",        0x80000, 0x35b5187e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic71.bin",        0x80000, 0x9a538b9b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic86.bin",        0x80000, 0xe11c6c67, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic114.bin",       0x80000, 0x16344535, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic82.bin",        0x80000, 0x78e9983b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic110.bin",       0x80000, 0xe9daa1a4, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "vic66.bin",        0x80000, 0xb83df159, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic74.bin",        0x80000, 0x76dbe9ce, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic62.bin",        0x80000, 0x9e998209, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic70.bin",        0x80000, 0x2caddf1a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic85.bin",        0x80000, 0xb15e19ff, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic113.bin",       0x80000, 0x84c7008f, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic81.bin",        0x80000, 0xc3cf5faa, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic109.bin",       0x80000, 0x7e91beb2, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "vic65.bin",        0x80000, 0x31dbb2c3, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic73.bin",        0x80000, 0x7e68257d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic61.bin",        0x80000, 0x71031ad0, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic69.bin",        0x80000, 0x27e70a5e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic84.bin",        0x80000, 0x7540bf85, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic112.bin",       0x80000, 0x7d87b94d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic80.bin",        0x80000, 0x87725d74, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "vic108.bin",       0x80000, 0x73477291, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "ic102.bin",        0x10000, 0x7cc3b543, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "ic107.bin",        0x80000, 0x345f5a41, SYS16_ROM_PCMDATA | BRF_SND },
	{ "ic106.bin",        0x80000, 0xf604c270, SYS16_ROM_PCMDATA | BRF_SND },
	{ "ic105.bin",        0x80000, 0x76095538, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Rchase);
STD_ROM_FN(Rchase);

static struct BurnRomInfo StrkfgtrRomDesc[] = {
	{ "epr-13824.25",     0x20000, 0x2cf2610c, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13823.24",     0x20000, 0x2c98242f, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13826.27",     0x20000, 0x3d34ea55, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13825.26",     0x20000, 0xfe218d83, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13830.81",     0x20000, 0xf9adc9d1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13829.80",     0x20000, 0xc5cd85dd, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	
	{ "epr-13828.54",     0x20000, 0x2470cf5f, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	{ "epr-13827.53",     0x20000, 0xa9d0cf7d, SYS16_ROM_PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr-13832.14",     0x80000, 0x41679754, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13833.16",     0x80000, 0x6148e11a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13038.15",     0x80000, 0x0b2edb6d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13040.17",     0x80000, 0x4aeb3a85, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-13048.67",     0x80000, 0xfe1eb0dd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13056.75",     0x80000, 0x5904f8e6, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13044.63",     0x80000, 0x4d931f89, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13052.71",     0x80000, 0x0291f040, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13064.86",     0x80000, 0x5f8e651b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13072.114",    0x80000, 0x6b85641a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13060.82",     0x80000, 0xee16ad97, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13068.110",    0x80000, 0x64d52bbb, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13047.66",     0x80000, 0x53340832, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13055.74",     0x80000, 0x39b6b665, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13043.62",     0x80000, 0x208f16fd, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13051.70",     0x80000, 0xad62cbd4, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13063.85",     0x80000, 0xc580bf6d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13071.113",    0x80000, 0xdf99ef99, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13059.81",     0x80000, 0x4c982558, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13067.109",    0x80000, 0xf97f6119, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13046.65",     0x80000, 0xc75a86e9, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13054.73",     0x80000, 0x2934549a, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13042.61",     0x80000, 0x53ed97af, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13050.69",     0x80000, 0x04429068, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13062.84",     0x80000, 0x4fdb4ee3, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13070.112",    0x80000, 0x52ea130e, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13058.80",     0x80000, 0x19ff1626, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13066.108",    0x80000, 0xbc70a250, SYS16_ROM_SPRITES2 | BRF_GRA },
	
	{ "epr-13045.64",     0x80000, 0x54d5bc6d, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13053.72",     0x80000, 0x9502af13, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13041.60",     0x80000, 0xd0a7402c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13049.68",     0x80000, 0x5b9c0b6c, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13061.83",     0x80000, 0x7b95ec3b, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13069.111",    0x80000, 0xe1f538f0, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13057.79",     0x80000, 0x73baefee, SYS16_ROM_SPRITES2 | BRF_GRA },
	{ "epr-13065.107",    0x80000, 0x8937a655, SYS16_ROM_SPRITES2 | BRF_GRA },
		
	{ "epr-13831.102",    0x10000, 0xdabbcea1, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "mpr-13036.107",    0x80000, 0x7890c26c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr-13035.106",    0x80000, 0x009fa13e, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr-13034.105",    0x80000, 0xcd22d95d, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Strkfgtr);
STD_ROM_FN(Strkfgtr);

/*====================================================
Memory Handlers
====================================================*/

static UINT8 misc_io_data[0x10];
static UINT8 analog_data[4];

static unsigned char io_chip_r(unsigned int offset)
{
	switch (offset) {
		case 0x00:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x07: {
			if (misc_io_data[0x1e/2] & (1 << offset)) return misc_io_data[offset];
			return 0xff;
		}
		
		case 0x01: {
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
	misc_io_data[offset] = d;	
	
	switch (offset) {
		case 0x04: {
			System16VideoEnable = d & 0x80;
			
			if (d & 0x04) {
				int nLastCPU = nSekActive;
				SekClose();
				SekOpen(2);
				SekReset();
				SekClose();
				SekOpen(nLastCPU);
			}
			
			if (d & 0x08) {
				int nLastCPU = nSekActive;
				SekClose();
				SekOpen(1);
				SekReset();
				SekClose();
				SekOpen(nLastCPU);
			}
			
			if (!(d & 0x10)) {
				ZetOpen(0);
				ZetReset();
				ZetClose();
			}
			
			return;
		}
	}
}

static unsigned char analog_r(unsigned int offset)
{
	int result = analog_data[offset] & 0x80;
	analog_data[offset] <<= 1;
	return result;
}

static void analog_w(unsigned int offset, unsigned short d)
{
	if (offset == 3) {
		if (System16ProcessAnalogControlsDo) analog_data[offset] = System16ProcessAnalogControlsDo(3 + (misc_io_data[0x08/2] & 3));
	} else {
		if (System16ProcessAnalogControlsDo) analog_data[offset] = System16ProcessAnalogControlsDo(offset & 3);
	}
}

unsigned short __fastcall YBoardReadWord(unsigned int a)
{
	if (a >= 0x080000 && a <= 0x080007) {
		return System16MultiplyChipRead(0, (a - 0x080000) >> 1);
	}
	
	if (a >= 0x084000 && a <= 0x08401f) {
		return System16DivideChipRead(0, (a - 0x084000) >> 1);
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Word -> 0x%06X\n"), a);
#endif

	return 0xffff;
}

unsigned char __fastcall YBoardReadByte(unsigned int a)
{
	if (a >= 0x100000 && a <= 0x10001f) {
		return io_chip_r((a - 0x100000) >> 1);
	}
	
	if (a >= 0x100040 && a <= 0x100047) {
		return analog_r((a - 0x100040) >> 1);
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Byte -> 0x%06X\n"), a);
#endif

	return 0xff;
}

void __fastcall YBoardWriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x100000 && a <= 0x10001f) {
		io_chip_w((a - 0x100000) >> 1, d);
		return;
	}

	if (a >= 0x080000 && a <= 0x080007) {
		System16MultiplyChipWrite(0, (a - 0x080000) >> 1, d);
		return;
	}
	
	if (a >= 0x084000 && a <= 0x08401f) {
		System16DivideChipWrite(0, (a - 0x084000) >> 1, d);
		return;
	}
	
#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

void __fastcall YBoardWriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x100000 && a <= 0x10001f) {
		io_chip_w((a - 0x100000) >> 1, d);
		return;
	}
	
	if (a >= 0x100040 && a <= 0x100047) {
		analog_w((a - 0x100040) >> 1, d);
		return;
	}
	
	switch (a) {
		case 0x082001: {
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

unsigned short __fastcall YBoard2ReadWord(unsigned int a)
{	
	if (a >= 0x080000 && a <= 0x080007) {
		return System16MultiplyChipRead(1, (a - 0x080000) >> 1);
	}
	
	if (a >= 0x084000 && a <= 0x08401f) {
		return System16DivideChipRead(1, (a - 0x084000) >> 1);
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 #2 Read Word -> 0x%06X\n"), a);
#endif

	return 0xffff;
}

void __fastcall YBoard2WriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x080000 && a <= 0x080007) {
		System16MultiplyChipWrite(1, (a - 0x080000) >> 1, d);
		return;
	}
	
	if (a >= 0x084000 && a <= 0x08401f) {
		System16DivideChipWrite(1, (a - 0x084000) >> 1, d);
		return;
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 #2 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

unsigned short __fastcall YBoard3ReadWord(unsigned int a)
{
	if (a >= 0x080000 && a <= 0x080007) {
		return System16MultiplyChipRead(2, (a - 0x080000) >> 1);
	}
	
	if (a >= 0x084000 && a <= 0x08401f) {
		return System16DivideChipRead(2, (a - 0x084000) >> 1);
	}
	
	switch (a) {
		case 0x198000: {
			/* swap the halves of the rotation RAM */
			UINT32 *src = (UINT32 *)System16RotateRam;
			UINT32 *dst = (UINT32 *)System16RotateRamBuff;
			
			for (unsigned int i = 0; i < System16RotateRamSize/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
			return 0xffff;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 #3 Read Word -> 0x%06X\n"), a);
#endif

	return 0xffff;
}

unsigned char __fastcall YBoard3ReadByte(unsigned int a)
{
	if (a >= 0x084000 && a <= 0x08401f) {
		return System16DivideChipRead(2, (a - 0x084000) >> 1);
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 #3 Read Byte -> 0x%06X\n"), a);
#endif

	return 0xff;
}

void __fastcall YBoard3WriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x080000 && a <= 0x080007) {
		System16MultiplyChipWrite(2, (a - 0x080000) >> 1, d);
		return;
	}
	
	if (a >= 0x084000 && a <= 0x08401f) {
		System16DivideChipWrite(2, (a - 0x084000) >> 1, d);
		return;
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 #3 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

/*====================================================
Driver Inits
====================================================*/

unsigned char Gforce2ProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 0: {
			temp = 0x80 + (System16AnalogPort0 >> 4);
			return temp;
		}
		
		case 1: {
			temp = 0x80 - (System16AnalogPort1 >> 4);
			return temp;
		}
		
		case 2: {
			temp = 0x80 + (System16AnalogPort2 >> 4);
			if (temp == 0x80) return 0x80;
			if (temp > 0x80) return 0xff;
			return 0;
		}
	}
	
	return 0;
}

unsigned char GlocProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 3: {
			temp = 0x80 - (System16AnalogPort1 >> 4);
			if (temp < 0x40) temp = 0x40;
			if (temp > 0xc0) temp = 0xc0;
			return temp;
		}
		
		case 4: {
			temp = 0x80 + (System16AnalogPort2 >> 4);
			if (temp > 0xc0) return 0xff;
			if (temp < 0x40) return 0;
			return 0x80;
		}
		
		case 5: {
			temp = 0x80 + (System16AnalogPort0 >> 4);
			if (temp < 0x20) temp = 0x20;
			if (temp > 0xe0) temp = 0xe0;
			return temp;
		}
	}
	
	return 0;
}

unsigned char Glocr360ProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 1: {
			temp = 0x7f + (System16AnalogPort3 >> 4);
			if (temp == 0xfe) temp = 0xff;
			return temp;
		}
		
		case 2: {
			temp = 0x7f + (System16AnalogPort2 >> 4);
			if (temp == 0xfe) temp = 0xff;
			return temp;
		}
		
		case 3: {
			temp = 0x7f - (System16AnalogPort1 >> 4);
			if (temp == 0xfe) temp = 0xff;
			return temp;
		}
		
		case 5: {
			temp = 0x7f + (System16AnalogPort0 >> 4);
			if (temp == 0xfe) temp = 0xff;
			return temp;
		}
	}
	
	return 0;
}

unsigned char PdriftProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 3: {
			if (System16AnalogPort2 > 1) return 0xff;
			return 0;
		}
		
		case 4: {
			if (System16AnalogPort1 > 1) return 0xff;
			return 0;
		}
		
		case 5: {
			temp = 0x80 + (System16AnalogPort0 >> 4);
			if (temp < 0x20) temp = 0x20;
			if (temp > 0xe0) temp = 0xe0;
			return temp;
		}
	}
	
	return 0;
}

unsigned char RchaseProcessAnalogControls(UINT16 value)
{
	switch (value) {
		case 0: {
			return BurnGunReturnX(0);
		}
		
		case 1: {
			return BurnGunReturnY(0);
		}
		
		case 2: {
			return BurnGunReturnX(1);
		}
		
		case 3: {
			return BurnGunReturnY(1);
		}
	}
	
	return 0;
}

static int Gforce2Init()
{
	System16ProcessAnalogControlsDo = Gforce2ProcessAnalogControls;
	
	System16PCMDataSizePreAllocate = 0x180000;
	
	int nRet = System16Init();
	
	unsigned char *pTemp = (unsigned char*)malloc(0x0c0000);
	memcpy(pTemp, System16PCMData, 0x0c0000);
	memset(System16PCMData, 0, 0x180000);
	memcpy(System16PCMData + 0x000000, pTemp + 0x000000, 0x80000);
	memcpy(System16PCMData + 0x080000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x0a0000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x0c0000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x0e0000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x100000, pTemp + 0x0a0000, 0x20000);
	memcpy(System16PCMData + 0x120000, pTemp + 0x0a0000, 0x20000);
	memcpy(System16PCMData + 0x140000, pTemp + 0x0a0000, 0x20000);
	memcpy(System16PCMData + 0x160000, pTemp + 0x0a0000, 0x20000);
	free(pTemp);
	
	return nRet;
}

static int GlocInit()
{
	System16ProcessAnalogControlsDo = GlocProcessAnalogControls;
	
	int nRet = System16Init();
	
	return nRet;
}

static int Glocr360Init()
{
	System16ProcessAnalogControlsDo = Glocr360ProcessAnalogControls;
	
	int nRet = System16Init();
	
	return nRet;
}

static int PdriftInit()
{
	System16ProcessAnalogControlsDo = PdriftProcessAnalogControls;
	
	System16HasGears = true;
	
	System16PCMDataSizePreAllocate = 0x180000;
	
	int nRet = System16Init();
	
	if (!nRet) YBoardIrq2Scanline = 0;
	
	unsigned char *pTemp = (unsigned char*)malloc(0x0c0000);
	memcpy(pTemp, System16PCMData, 0x0c0000);
	memset(System16PCMData, 0, 0x180000);
	memcpy(System16PCMData + 0x000000, pTemp + 0x000000, 0x80000);
	memcpy(System16PCMData + 0x080000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x0a0000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x0c0000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x0e0000, pTemp + 0x080000, 0x20000);
	memcpy(System16PCMData + 0x100000, pTemp + 0x0a0000, 0x20000);
	memcpy(System16PCMData + 0x120000, pTemp + 0x0a0000, 0x20000);
	memcpy(System16PCMData + 0x140000, pTemp + 0x0a0000, 0x20000);
	memcpy(System16PCMData + 0x160000, pTemp + 0x0a0000, 0x20000);
	free(pTemp);

	return nRet;
}

static int RchaseInit()
{
	BurnGunInit(2, false);
	
	System16ProcessAnalogControlsDo = RchaseProcessAnalogControls;
	
	int nRet = System16Init();
	
	return nRet;
}

static int YBoardExit()
{
	memset(misc_io_data, 0, sizeof(misc_io_data));
	memset(analog_data, 0, sizeof(analog_data));

	return System16Exit();
}

static int YBoardScan(int nAction,int *pnMin)
{
	if (pnMin != NULL) {					// Return minimum compatible version
		*pnMin =  0x029660;
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(misc_io_data);
		SCAN_VAR(analog_data);
	}
	
	return System16Scan(nAction, pnMin);;
}

struct BurnDriver BurnDrvGforce2 = {
	"gforce2", NULL, NULL, "1988",
	"Galaxy Force 2\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, Gforce2RomInfo, Gforce2RomName, Gforce2InputInfo, Gforce2DIPInfo,
	Gforce2Init, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvGforce2j = {
	"gforce2j", "gforce2", NULL, "1988",
	"Galaxy Force 2 (Japan)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, Gforce2jRomInfo, Gforce2jRomName, Gforce2InputInfo, Gforce2DIPInfo,
	Gforce2Init, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvGloc = {
	"gloc", NULL, NULL, "1990",
	"G-LOC Air Battle (US)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, GlocRomInfo, GlocRomName, GlocInputInfo, GlocDIPInfo,
	GlocInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvGlocr360 = {
	"glocr360", "gloc", NULL, "1990",
	"G-LOC R360\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, Glocr360RomInfo, Glocr360RomName, Glocr360InputInfo, Glocr360DIPInfo,
	Glocr360Init, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvPdrift = {
	"pdrift", NULL, NULL, "1988",
	"Power Drift (World, Rev A)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, PdriftRomInfo, PdriftRomName, PdriftInputInfo, PdriftDIPInfo,
	PdriftInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvPdrifta = {
	"pdrifta", "pdrift", NULL, "1988",
	"Power Drift (World)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, PdriftaRomInfo, PdriftaRomName, PdriftInputInfo, PdriftDIPInfo,
	PdriftInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvPdrifte = {
	"pdrifte", "pdrift", NULL, "1988",
	"Power Drift (World, Earlier)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, PdrifteRomInfo, PdrifteRomName, PdriftInputInfo, PdrifteDIPInfo,
	PdriftInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvPdriftj = {
	"pdriftj", "pdrift", NULL, "1988",
	"Power Drift (Japan)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, PdriftjRomInfo, PdriftjRomName, PdriftInputInfo, PdriftjDIPInfo,
	PdriftInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvRchase = {
	"rchase", NULL, NULL, "1991",
	"Rail Chase (Japan)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, RchaseRomInfo, RchaseRomName, RchaseInputInfo, RchaseDIPInfo,
	RchaseInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvStrkfgtr = {
	"strkfgtr", NULL, NULL, "1991",
	"Strike Fighter (Japan)\0", NULL, "Sega", "Y-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMY,
	NULL, StrkfgtrRomInfo, StrkfgtrRomName, GlocInputInfo, StrkfgtrDIPInfo,
	GlocInit, YBoardExit, YBoardFrame, NULL, YBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};
