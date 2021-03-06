#include "sys16.h"

/*====================================================
Input defs
====================================================*/

#define A(a, b, c, d) {a, b, (unsigned char*)(c), d}

static struct BurnInputInfo AbcopInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Accelerate"       , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 fire 1"  ),
	{"Jump"              , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Abcop);

static struct BurnInputInfo Aburner2InputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Up/Down"          , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 y-axis"  ),
	A("Throttle"         , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 z-axis"  ),
	{"Vulcan"            , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 1"  },
	{"Missile"           , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Aburner2);

static struct BurnInputInfo GpriderInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Accelerate"       , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 fire 1"  ),
	A("Brake"            , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 fire 2"  ),
	{"Shift Down"        , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 3"  },
	{"Shift Up"          , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 4"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Gprider);

static struct BurnInputInfo LoffireInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("P1 X-Axis"        , BIT_ANALOG_REL, &System16AnalogPort0,   "mouse x-axis"   ),
	A("P1 Y-Axis"        , BIT_ANALOG_REL, &System16AnalogPort1,   "mouse y-axis"   ),
	{"P1 Fire 1"         , BIT_DIGITAL   , System16InputPort1 + 7, "mouse button 1" },
	{"P1 Fire 2"         , BIT_DIGITAL   , System16InputPort1 + 6, "mouse button 2" },
	
	A("P2 X-Axis"        , BIT_ANALOG_REL, &System16AnalogPort2,   "p2 x-axis"  ),
	A("P2 Y-Axis"        , BIT_ANALOG_REL, &System16AnalogPort3,   "p2 y-axis"  ),
	{"P2 Fire 1"         , BIT_DIGITAL   , System16InputPort1 + 5, "p2 fire 1"  },
	{"P2 Fire 2"         , BIT_DIGITAL   , System16InputPort1 + 4, "p2 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Service 2"         , BIT_DIGITAL   , System16InputPort0 + 3 , "service2"  },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Loffire);

static struct BurnInputInfo RacheroInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Accelerate"       , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 fire 1"  ),
	A("Brake"            , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 fire 2"  ),
	{"Move to centre"    , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 3"  },
	{"Suicide"           , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 4"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Rachero);

static struct BurnInputInfo SmgpInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Accelerate"       , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 fire 1"  ),
	A("Brake"            , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 fire 2"  ),
	{"Shift Down"        , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 3"  },
	{"Shift Up"          , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 4"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Smgp);

static struct BurnInputInfo ThndrbldInputList[] = {
	{"Coin 1"            , BIT_DIGITAL   , System16InputPort0 + 6, "p1 coin"    },
	{"Start 1"           , BIT_DIGITAL   , System16InputPort0 + 3, "p1 start"   },
	{"Coin 2"            , BIT_DIGITAL   , System16InputPort0 + 7, "p2 coin"    },

	A("Left/Right"       , BIT_ANALOG_REL, &System16AnalogPort0,   "p1 x-axis"  ),
	A("Up/Down"          , BIT_ANALOG_REL, &System16AnalogPort1,   "p1 y-axis"  ),
	A("Throttle"         , BIT_ANALOG_REL, &System16AnalogPort2,   "p1 z-axis"  ),
	{"Cannon"            , BIT_DIGITAL   , System16InputPort0 + 4, "p1 fire 1"  },
	{"Missile"           , BIT_DIGITAL   , System16InputPort0 + 5, "p1 fire 2"  },
	
	{"Service"           , BIT_DIGITAL   , System16InputPort0 + 2 , "service"   },
	{"Diagnostics"       , BIT_DIGITAL   , System16InputPort0 + 1 , "diag"      },
	{"Reset"             , BIT_DIGITAL   , &System16Reset         , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH , System16Dip + 0        , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH , System16Dip + 1        , "dip"       },
};

STDINPUTINFO(Thndrbld);

#undef A

/*====================================================
Dip Defs
====================================================*/

#define BOARDX_COINAGE(dipval)								\
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

static struct BurnDIPInfo AbcopDIPList[]=
{
	// Default Values
	{0x09, 0xff, 0xff, 0xff, NULL                                 },
	{0x0a, 0xff, 0xff, 0xf9, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x09)

	// Dip 2
	{0   , 0xfe, 0   , 2   , "Credits"                            },
	{0x0a, 0x01, 0x01, 0x01, "1 to start, 1 to continue"          },
	{0x0a, 0x01, 0x01, 0x00, "2 to start, 1 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0a, 0x01, 0x02, 0x02, "Off"                                },
	{0x0a, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0a, 0x01, 0x04, 0x04, "No"                                 },
	{0x0a, 0x01, 0x04, 0x00, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Time"                               },
	{0x0a, 0x01, 0x30, 0x10, "Easy"                               },
	{0x0a, 0x01, 0x30, 0x30, "Normal"                             },
	{0x0a, 0x01, 0x30, 0x20, "Hard"                               },
	{0x0a, 0x01, 0x30, 0x00, "Hardest"                            },
		
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0a, 0x01, 0xc0, 0x40, "Easy"                               },
	{0x0a, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0a, 0x01, 0xc0, 0x80, "Hard"                               },
	{0x0a, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Abcop);
	
static struct BurnDIPInfo Aburner2DIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0xcd, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 4   , "Cabinet Type"                       },
	{0x0c, 0x01, 0x03, 0x03, "Moving Deluxe"                      },
	{0x0c, 0x01, 0x03, 0x02, "Moving Standard"                    },
	{0x0c, 0x01, 0x03, 0x01, "Upright 1"                          },
	{0x0c, 0x01, 0x03, 0x00, "Upright 2"                          },
	
	{0   , 0xfe, 0   , 2   , "Throttle Lever"                     },
	{0x0c, 0x01, 0x04, 0x00, "No"                                 },
	{0x0c, 0x01, 0x04, 0x04, "Yes"                                },
	
	{0   , 0xfe, 0   , 2   , "Lives"                              },
	{0x0c, 0x01, 0x08, 0x08, "3"                                  },
	{0x0c, 0x01, 0x08, 0x00, "4"                                  },
	
	{0   , 0xfe, 0   , 2   , "Ship Increase"                      },
	{0x0c, 0x01, 0x10, 0x10, "No"                                 },
	{0x0c, 0x01, 0x10, 0x00, "Yes"                                },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x20, 0x20, "No"                                 },
	{0x0c, 0x01, 0x20, 0x00, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x0c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0c, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x0c, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Aburner2);

static struct BurnDIPInfo AburnerDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0xc1, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 4   , "Cabinet Type"                       },
	{0x0c, 0x01, 0x03, 0x03, "Moving Deluxe"                      },
	{0x0c, 0x01, 0x03, 0x02, "Moving Standard"                    },
	{0x0c, 0x01, 0x03, 0x01, "Upright 1"                          },
	{0x0c, 0x01, 0x03, 0x00, "Upright 2"                          },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x04, 0x04, "Off"                                },
	{0x0c, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Lives"                              },
	{0x0c, 0x01, 0x10, 0x10, "3"                                  },
	{0x0c, 0x01, 0x10, 0x00, "3 x credits"                        },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x20, 0x20, "No"                                 },
	{0x0c, 0x01, 0x20, 0x00, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x0c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0c, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x0c, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Aburner);

static struct BurnDIPInfo GpriderDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0xfe, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet Type"                       },
	{0x0c, 0x01, 0x03, 0x03, "Ride On"                            },
	{0x0c, 0x01, 0x03, 0x02, "Upright"                            },
	
	{0   , 0xfe, 0   , 2   , "ID Number"                          },
	{0x0c, 0x01, 0x08, 0x08, "Main"                               },
	{0x0c, 0x01, 0x08, 0x00, "Slave"                              },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x10, 0x00, "Off"                                },
	{0x0c, 0x01, 0x10, 0x10, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0xc0, 0x80, "Easy"                               },
	{0x0c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0c, 0x01, 0xc0, 0x40, "Hard"                               },
	{0x0c, 0x01, 0xc0, 0x00, "Hardest"                            },	
};

STDDIPINFO(Gprider);

static struct BurnDIPInfo LoffireDIPList[]=
{
	// Default Values
	{0x0e, 0xff, 0xff, 0xff, NULL                                 },
	{0x0f, 0xff, 0xff, 0x9c, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0e)

	// Dip 2
	{0   , 0xfe, 0   , 2   , "Language"                           },
	{0x0f, 0x01, 0x01, 0x01, "Japanese"                           },
	{0x0f, 0x01, 0x01, 0x00, "English"                            },
	
	{0   , 0xfe, 0   , 2   , "Cabinet"                            },
	{0x0f, 0x01, 0x02, 0x02, "Cockpit"                            },
	{0x0f, 0x01, 0x02, 0x00, "Upright"                            },
	
	{0   , 0xfe, 0   , 2   , "2 Credits to start"                 },
	{0x0f, 0x01, 0x04, 0x04, "Off"                                },
	{0x0f, 0x01, 0x04, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0f, 0x01, 0x18, 0x10, "Easy"                               },
	{0x0f, 0x01, 0x18, 0x18, "Normal"                             },
	{0x0f, 0x01, 0x18, 0x08, "Hard"                               },
	{0x0f, 0x01, 0x18, 0x00, "Hardest"                            },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0f, 0x01, 0x20, 0x20, "No"                                 },
	{0x0f, 0x01, 0x20, 0x00, "Yes"                                },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0f, 0x01, 0x40, 0x40, "Off"                                },
	{0x0f, 0x01, 0x40, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Coin Chute"                         },
	{0x0f, 0x01, 0x80, 0x80, "Single"                             },
	{0x0f, 0x01, 0x80, 0x00, "Twin"                               },	
};

STDDIPINFO(Loffire);

static struct BurnDIPInfo RacheroDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0xf9, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 2   , "Credits"                            },
	{0x0c, 0x01, 0x01, 0x01, "1 to start, 1 to continue"          },
	{0x0c, 0x01, 0x01, 0x00, "2 to start, 1 to continue"          },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x02, 0x02, "Off"                                },
	{0x0c, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x04, 0x04, "No"                                 },
	{0x0c, 0x01, 0x04, 0x00, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Time"                               },
	{0x0c, 0x01, 0x30, 0x10, "Easy"                               },
	{0x0c, 0x01, 0x30, 0x30, "Normal"                             },
	{0x0c, 0x01, 0x30, 0x20, "Hard"                               },
	{0x0c, 0x01, 0x30, 0x00, "Very Hard"                          },
		
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0xc0, 0x40, "Easy"                               },
	{0x0c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0c, 0x01, 0xc0, 0x80, "Hard"                               },
	{0x0c, 0x01, 0xc0, 0x00, "Very Hard"                          },
};

STDDIPINFO(Rachero);

static struct BurnDIPInfo SmgpDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0x7f, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 8   , "Machine ID"                         },
	{0x0c, 0x01, 0x07, 0x07, "1"                                  },
	{0x0c, 0x01, 0x07, 0x06, "2"                                  },
	{0x0c, 0x01, 0x07, 0x05, "3"                                  },
	{0x0c, 0x01, 0x07, 0x04, "4"                                  },
	{0x0c, 0x01, 0x07, 0x03, "5"                                  },
	{0x0c, 0x01, 0x07, 0x02, "6"                                  },
	{0x0c, 0x01, 0x07, 0x01, "7"                                  },
	{0x0c, 0x01, 0x07, 0x00, "8"                                  },
	
	{0   , 0xfe, 0   , 8   , "Number of Machines"                 },
	{0x0c, 0x01, 0x38, 0x38, "1"                                  },
	{0x0c, 0x01, 0x38, 0x30, "2"                                  },
	{0x0c, 0x01, 0x38, 0x28, "3"                                  },
	{0x0c, 0x01, 0x38, 0x20, "4"                                  },
	{0x0c, 0x01, 0x38, 0x18, "5"                                  },
	{0x0c, 0x01, 0x38, 0x10, "6"                                  },
	{0x0c, 0x01, 0x38, 0x08, "7"                                  },
	{0x0c, 0x01, 0x38, 0x00, "8"                                  },
	
	{0   , 0xfe, 0   , 3   , "Cabinet"                            },
	{0x0c, 0x01, 0xc0, 0xc0, "Deluxe"                             },
	{0x0c, 0x01, 0xc0, 0x80, "Cockpit"                            },
	{0x0c, 0x01, 0xc0, 0x40, "Upright"                            },
};

STDDIPINFO(Smgp);

static struct BurnDIPInfo ThndrbldDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet Type"                       },
	{0x0c, 0x01, 0x01, 0x01, "Econ Upright"                       },
	{0x0c, 0x01, 0x01, 0x00, "Mini Upright"                       },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x02, 0x02, "Off"                                },
	{0x0c, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Time"                               },
	{0x0c, 0x01, 0x04, 0x04, "30 sec"                             },
	{0x0c, 0x01, 0x04, 0x00, "0 sec"                              },
	
	{0   , 0xfe, 0   , 4   , "Lives"                              },
	{0x0c, 0x01, 0x18, 0x08, "2"                                  },
	{0x0c, 0x01, 0x18, 0x18, "3"                                  },
	{0x0c, 0x01, 0x18, 0x10, "4"                                  },
	{0x0c, 0x01, 0x18, 0x00, "5"                                  },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x20, 0x00, "No"                                 },
	{0x0c, 0x01, 0x20, 0x20, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0xc0, 0x40, "Easy"                               },
	{0x0c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0c, 0x01, 0xc0, 0x80, "Hard"                               },
	{0x0c, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Thndrbld);

static struct BurnDIPInfo Thndrbd1DIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xff, NULL                                 },
	{0x0c, 0xff, 0xff, 0xfd, NULL                                 },

	// Dip 1
	BOARDX_COINAGE(0x0b)

	// Dip 2
	{0   , 0xfe, 0   , 2   , "Cabinet Type"                       },
	{0x0c, 0x01, 0x01, 0x01, "Deluxe"                             },
	{0x0c, 0x01, 0x01, 0x00, "Standing"                           },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"                        },
	{0x0c, 0x01, 0x02, 0x02, "Off"                                },
	{0x0c, 0x01, 0x02, 0x00, "On"                                 },
	
	{0   , 0xfe, 0   , 2   , "Time"                               },
	{0x0c, 0x01, 0x04, 0x04, "30 sec"                             },
	{0x0c, 0x01, 0x04, 0x00, "0 sec"                              },
	
	{0   , 0xfe, 0   , 4   , "Lives"                              },
	{0x0c, 0x01, 0x18, 0x08, "2"                                  },
	{0x0c, 0x01, 0x18, 0x18, "3"                                  },
	{0x0c, 0x01, 0x18, 0x10, "4"                                  },
	{0x0c, 0x01, 0x18, 0x00, "5"                                  },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"                     },
	{0x0c, 0x01, 0x20, 0x00, "No"                                 },
	{0x0c, 0x01, 0x20, 0x20, "Yes"                                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"                         },
	{0x0c, 0x01, 0xc0, 0x40, "Easy"                               },
	{0x0c, 0x01, 0xc0, 0xc0, "Normal"                             },
	{0x0c, 0x01, 0xc0, 0x80, "Hard"                               },
	{0x0c, 0x01, 0xc0, 0x00, "Hardest"                            },
};

STDDIPINFO(Thndrbd1);

#undef BOARDX_COINAGE

/*====================================================
Rom Defs
====================================================*/

static struct BurnRomInfo AbcopRomDesc[] = {
	{ "epr-13568b.ic58",  0x20000, 0xf88db35b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13556b.ic63",  0x20000, 0x337bf32e, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13559.ic57",   0x20000, 0x4588bf19, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13558.ic62",   0x20000, 0x11259ed4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13566.ic20",   0x20000, 0x22e52f32, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13565.ic29",   0x20000, 0xa21784bd, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "opr-13553.ic154",  0x10000, 0x8c418837, SYS16_ROM_TILES | BRF_GRA },
	{ "opr-13554.ic153",  0x10000, 0x4e3df9f0, SYS16_ROM_TILES | BRF_GRA },
	{ "opr-13555.ic152",  0x10000, 0x6c4a1d42, SYS16_ROM_TILES | BRF_GRA },
	
	{ "opr-13552.ic90",   0x20000, 0xcc2cf706, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13551.ic94",   0x20000, 0xd6f276c1, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13550.ic98",   0x20000, 0xf16518dd, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13549.ic102",  0x20000, 0xcba407a7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13548.ic91",   0x20000, 0x080fd805, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13547.ic95",   0x20000, 0x42d4dd68, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13546.ic99",   0x20000, 0xca6fbf3d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13545.ic103",  0x20000, 0xc9e58dd2, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13544.ic92",   0x20000, 0x9c1436d9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13543.ic96",   0x20000, 0x2c1c8f0e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13542.ic100",  0x20000, 0x01fd52b8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13541.ic104",  0x20000, 0xa45c547b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13540.ic93",   0x20000, 0x84b42ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13539.ic97",   0x20000, 0xcd6e524f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13538.ic101",  0x20000, 0xbf9a4586, SYS16_ROM_SPRITES | BRF_GRA },
	{ "opr-13537.ic105",  0x20000, 0xfa14ed3e, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "opr-13564.ic40",   0x10000, 0xe70ba138, SYS16_ROM_ROAD | BRF_GRA },
	
	{ "epr-13560.ic17",   0x10000, 0x83050925, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "opr-13563.ic11",   0x20000, 0x4083e74f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "opr-13562.ic12",   0x20000, 0x3cc3968f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "opr-13561.ic13",   0x20000, 0x80a7c02a, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0169b.key",    0x02000, 0x058da36e, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Abcop);
STD_ROM_FN(Abcop);

static struct BurnRomInfo Aburner2RomDesc[] = {
	{ "11107.58",         0x20000, 0x6d87bab7, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "11108.104",        0x20000, 0x202a3e1d, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "11109.20",         0x20000, 0x85a0fe07, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "11110.29",         0x20000, 0xf3d6797c, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "11115.154",        0x10000, 0xe8e32921, SYS16_ROM_TILES | BRF_GRA },
	{ "11114.153",        0x10000, 0x2e97f633, SYS16_ROM_TILES | BRF_GRA },
	{ "11113.152",        0x10000, 0x36058c8c, SYS16_ROM_TILES | BRF_GRA },
	
	{ "10932.125",        0x20000, 0xcc0821d6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10934.129",        0x20000, 0x4a51b1fa, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10936.133",        0x20000, 0xada70d64, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10938.102",        0x20000, 0xe7675baf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10933.126",        0x20000, 0xc8efb2c3, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10935.130",        0x20000, 0xc1e23521, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10937.134",        0x20000, 0xf0199658, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10939.103",        0x20000, 0xa0d49480, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11103.127",        0x20000, 0xbdd60da2, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11104.131",        0x20000, 0x06a35fce, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11105.135",        0x20000, 0x027b0689, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11106.104",        0x20000, 0x9e1fec09, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11116.128",        0x20000, 0x49b4c1ba, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11117.132",        0x20000, 0x821fbb71, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11118.136",        0x20000, 0x8f38540b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "11119.105",        0x20000, 0xd0343a8e, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-10922.ic40",   0x10000, 0xb49183d4, SYS16_ROM_ROAD | BRF_GRA },
	
	{ "epr-11112.ic17",   0x10000, 0xd777fc6d, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "10931.11",         0x20000, 0x9209068f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "10930.12",         0x20000, 0x6493368b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "11102.13",         0x20000, 0x6c07c78d, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Aburner2);
STD_ROM_FN(Aburner2);

static struct BurnRomInfo AburnerRomDesc[] = {
	{ "epr10940.bin",     0x20000, 0x4d132c4e, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr10941.bin",     0x20000, 0x136ea264, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr10927.bin",     0x20000, 0x66d36757, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr10928.bin",     0x20000, 0x7c01d40b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr10926.bin",     0x10000, 0xed8bd632, SYS16_ROM_TILES | BRF_GRA },
	{ "epr10925.bin",     0x10000, 0x4ef048cc, SYS16_ROM_TILES | BRF_GRA },
	{ "epr10924.bin",     0x10000, 0x50c15a6d, SYS16_ROM_TILES | BRF_GRA },
	
	{ "10932.125",        0x20000, 0xcc0821d6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10934.129",        0x20000, 0x4a51b1fa, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10936.133",        0x20000, 0xada70d64, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10938.102",        0x20000, 0xe7675baf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10933.126",        0x20000, 0xc8efb2c3, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10935.130",        0x20000, 0xc1e23521, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10937.134",        0x20000, 0xf0199658, SYS16_ROM_SPRITES | BRF_GRA },
	{ "10939.103",        0x20000, 0xa0d49480, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10942.bin",     0x20000, 0x5ce10b8c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10943.bin",     0x20000, 0xb98294dc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10944.bin",     0x20000, 0x17be8f67, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10945.bin",     0x20000, 0xdf4d4c4f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10946.bin",     0x20000, 0xd7d485f4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10947.bin",     0x20000, 0x08838392, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10948.bin",     0x20000, 0x64284761, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr10949.bin",     0x20000, 0xd8437d92, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-10922.ic40",   0x10000, 0xb49183d4, SYS16_ROM_ROAD | BRF_GRA },
	
	{ "epr-10923.ic17",   0x10000, 0x6888eb8f, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "10931.11",         0x20000, 0x9209068f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "10930.12",         0x20000, 0x6493368b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "11102.13",         0x20000, 0x6c07c78d, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Aburner);
STD_ROM_FN(Aburner);

static struct BurnRomInfo GpriderRomDesc[] = {
	{ "epr-13409.ic58",   0x20000, 0x00000000, SYS16_ROM_PROG | BRF_ESS | BRF_NODUMP | BRF_PRG },
	{ "epr-13408.ic63",   0x20000, 0x00000000, SYS16_ROM_PROG | BRF_ESS | BRF_NODUMP | BRF_PRG },
			
	{ "epr-13395.ic20",   0x20000, 0xd6ccfac7, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13394.ic29",   0x20000, 0x914a55ec, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13393.ic21",   0x20000, 0x08d023cc, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13392.ic30",   0x20000, 0xf927cd42, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr-13383.ic154",  0x10000, 0x24f897a7, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-13384.ic153",  0x10000, 0xfe8238bd, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-13385.ic152",  0x10000, 0x6df1b995, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr-13382.ic90",   0x20000, 0x01dac209, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13381.ic94",   0x20000, 0x3a50d931, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13380.ic98",   0x20000, 0xad1024c8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13379.ic102",  0x20000, 0x1ac17625, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13378.ic91",   0x20000, 0x50c9b867, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13377.ic95",   0x20000, 0x9b12f5c0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13376.ic99",   0x20000, 0x449ac518, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13375.ic103",  0x20000, 0x5489a9ff, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13374.ic92",   0x20000, 0x6a319e4f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13373.ic96",   0x20000, 0xeca5588b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13372.ic100",  0x20000, 0x0b45a433, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13371.ic104",  0x20000, 0xb68f4cff, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13370.ic93",   0x20000, 0x78276620, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13369.ic97",   0x20000, 0x8625bf0f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13368.ic101",  0x20000, 0x0f50716c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13367.ic105",  0x20000, 0x4b1bb51f, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-13388.ic17",   0x10000, 0x706581e4, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13391.ic11",   0x20000, 0x8c30c867, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13390.ic12",   0x20000, 0x8c93cd05, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13389.ic13",   0x20000, 0x4e4c758e, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0163.key",     0x02000, 0xc1d4d207, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Gprider);
STD_ROM_FN(Gprider);

static struct BurnRomInfo Gprider1RomDesc[] = {
	{ "epr-13407.ic58",   0x20000, 0x03553ebd, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13406.ic63",   0x20000, 0x122c711f, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr-13395.ic20",   0x20000, 0xd6ccfac7, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13394.ic29",   0x20000, 0x914a55ec, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13393.ic21",   0x20000, 0x08d023cc, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-13392.ic30",   0x20000, 0xf927cd42, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr-13383.ic154",  0x10000, 0x24f897a7, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-13384.ic153",  0x10000, 0xfe8238bd, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-13385.ic152",  0x10000, 0x6df1b995, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr-13382.ic90",   0x20000, 0x01dac209, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13381.ic94",   0x20000, 0x3a50d931, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13380.ic98",   0x20000, 0xad1024c8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13379.ic102",  0x20000, 0x1ac17625, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13378.ic91",   0x20000, 0x50c9b867, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13377.ic95",   0x20000, 0x9b12f5c0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13376.ic99",   0x20000, 0x449ac518, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13375.ic103",  0x20000, 0x5489a9ff, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13374.ic92",   0x20000, 0x6a319e4f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13373.ic96",   0x20000, 0xeca5588b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13372.ic100",  0x20000, 0x0b45a433, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13371.ic104",  0x20000, 0xb68f4cff, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13370.ic93",   0x20000, 0x78276620, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13369.ic97",   0x20000, 0x8625bf0f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13368.ic101",  0x20000, 0x0f50716c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-13367.ic105",  0x20000, 0x4b1bb51f, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-13388.ic17",   0x10000, 0x706581e4, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-13391.ic11",   0x20000, 0x8c30c867, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13390.ic12",   0x20000, 0x8c93cd05, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-13389.ic13",   0x20000, 0x4e4c758e, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0162.key",     0x02000, 0x8067de53, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Gprider1);
STD_ROM_FN(Gprider1);

static struct BurnRomInfo LoffireRomDesc[] = {
	{ "epr12849.rom",     0x20000, 0x61cfd2fe, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12850.rom",     0x20000, 0x14598f2a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
			
	{ "epr12804.rom",     0x20000, 0xb853480e, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12805.rom",     0x20000, 0x4a7200c3, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12802.rom",     0x20000, 0xd746bb39, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12803.rom",     0x20000, 0xc1d9e751, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "opr12791.rom",     0x10000, 0xacfa69ba, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12792.rom",     0x10000, 0xe506723c, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12793.rom",     0x10000, 0x0ce8cce3, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr12787.rom",     0x20000, 0x6431a3a6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12788.rom",     0x20000, 0x1982a0ce, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12789.rom",     0x20000, 0x97d03274, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12790.rom",     0x20000, 0x816e76e6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12783.rom",     0x20000, 0xc13feea9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12784.rom",     0x20000, 0x39b94c65, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12785.rom",     0x20000, 0x05ed0059, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12786.rom",     0x20000, 0xa4123165, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12779.rom",     0x20000, 0xae58af7c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12780.rom",     0x20000, 0xee670c1e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12781.rom",     0x20000, 0x538f6bc5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12782.rom",     0x20000, 0x5acc34f7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12775.rom",     0x20000, 0x693056ec, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12776.rom",     0x20000, 0x61efbdfd, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12777.rom",     0x20000, 0x29d5b953, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12778.rom",     0x20000, 0x2fb68e07, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12798.rom",     0x10000, 0x0587738d, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12799.rom",     0x20000, 0xbc60181c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr12800.rom",     0x20000, 0x1158c1a3, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr12801.rom",     0x20000, 0x2d6567c4, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0136.key",     0x02000, 0x344bfe0c, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Loffire);
STD_ROM_FN(Loffire);

static struct BurnRomInfo LoffirejRomDesc[] = {
	{ "epr12794.bin",     0x20000, 0x1e588992, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12795.bin",     0x20000, 0xd43d7427, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
			
	{ "epr12804.rom",     0x20000, 0xb853480e, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12805.rom",     0x20000, 0x4a7200c3, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12802.rom",     0x20000, 0xd746bb39, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12803.rom",     0x20000, 0xc1d9e751, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "opr12791.rom",     0x10000, 0xacfa69ba, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12792.rom",     0x10000, 0xe506723c, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12793.rom",     0x10000, 0x0ce8cce3, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr12787.rom",     0x20000, 0x6431a3a6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12788.rom",     0x20000, 0x1982a0ce, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12789.rom",     0x20000, 0x97d03274, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12790.rom",     0x20000, 0x816e76e6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12783.rom",     0x20000, 0xc13feea9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12784.rom",     0x20000, 0x39b94c65, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12785.rom",     0x20000, 0x05ed0059, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12786.rom",     0x20000, 0xa4123165, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12779.rom",     0x20000, 0xae58af7c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12780.rom",     0x20000, 0xee670c1e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12781.rom",     0x20000, 0x538f6bc5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12782.rom",     0x20000, 0x5acc34f7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12775.rom",     0x20000, 0x693056ec, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12776.rom",     0x20000, 0x61efbdfd, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12777.rom",     0x20000, 0x29d5b953, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12778.rom",     0x20000, 0x2fb68e07, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12798.rom",     0x10000, 0x0587738d, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12799.rom",     0x20000, 0xbc60181c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr12800.rom",     0x20000, 0x1158c1a3, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr12801.rom",     0x20000, 0x2d6567c4, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0134.key",     0x02000, 0x732626d4, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Loffirej);
STD_ROM_FN(Loffirej);

static struct BurnRomInfo LoffireuRomDesc[] = {
	{ "epr12847a.bin",    0x20000, 0xc50eb4ed, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12848a.bin",    0x20000, 0xf8ff8640, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
			
	{ "epr12804.rom",     0x20000, 0xb853480e, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12805.rom",     0x20000, 0x4a7200c3, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12802.rom",     0x20000, 0xd746bb39, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12803.rom",     0x20000, 0xc1d9e751, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "opr12791.rom",     0x10000, 0xacfa69ba, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12792.rom",     0x10000, 0xe506723c, SYS16_ROM_TILES | BRF_GRA },
	{ "opr12793.rom",     0x10000, 0x0ce8cce3, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr12787.rom",     0x20000, 0x6431a3a6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12788.rom",     0x20000, 0x1982a0ce, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12789.rom",     0x20000, 0x97d03274, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12790.rom",     0x20000, 0x816e76e6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12783.rom",     0x20000, 0xc13feea9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12784.rom",     0x20000, 0x39b94c65, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12785.rom",     0x20000, 0x05ed0059, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12786.rom",     0x20000, 0xa4123165, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12779.rom",     0x20000, 0xae58af7c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12780.rom",     0x20000, 0xee670c1e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12781.rom",     0x20000, 0x538f6bc5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12782.rom",     0x20000, 0x5acc34f7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12775.rom",     0x20000, 0x693056ec, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12776.rom",     0x20000, 0x61efbdfd, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12777.rom",     0x20000, 0x29d5b953, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12778.rom",     0x20000, 0x2fb68e07, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12798.rom",     0x10000, 0x0587738d, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12799.rom",     0x20000, 0xbc60181c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr12800.rom",     0x20000, 0x1158c1a3, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr12801.rom",     0x20000, 0x2d6567c4, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0135.key",     0x02000, 0xc53ad019, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Loffireu);
STD_ROM_FN(Loffireu);

static struct BurnRomInfo Rascot2RomDesc[] = {
	{ "ep13965a",         0x20000, 0x7eacdfb3, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "ep13694a",         0x20000, 0x15b86498, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr13967",         0x20000, 0x3b92e2b8, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr13966",         0x20000, 0xeaa644e1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr13961" ,        0x10000, 0x68038629, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13962",         0x10000, 0x7d7605bc, SYS16_ROM_TILES | BRF_GRA },
	{ "epr13963",         0x10000, 0xf3376b65, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr13957",         0x20000, 0x6d50fb54, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13958",         0x20000, 0x7803a027, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13959",         0x20000, 0xdb245b22, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr13960",         0x20000, 0xb974128d, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "ep13221a",         0x10000, 0x0d429ac4, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Rascot2);
STD_ROM_FN(Rascot2);

static struct BurnRomInfo RacheroRomDesc[] = {
	{ "epr-13129.ic58",   0x20000, 0xad9f32e7, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-13130.ic63",   0x20000, 0x6022777b, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-12855.ic57",   0x20000, 0xcecf1e73, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-12856.ic62",   0x20000, 0xda900ebb, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-12857.ic20",   0x20000, 0x8a2328cc, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-12858.ic29",   0x20000, 0x38a248b7, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr-12879.ic154",  0x10000, 0xc1a9de7a, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-12880.ic153",  0x10000, 0x27ff04a5, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-12881.ic152",  0x10000, 0x72f14491, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr-12872.ic90",   0x20000, 0x68d56139, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12873.ic94",   0x20000, 0x3d3ec450, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12874.ic98",   0x20000, 0x7d6bde23, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12875.ic102",  0x20000, 0xe33092bf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12868.ic91",   0x20000, 0x96289583, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12869.ic95",   0x20000, 0x2ef0de02, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12870.ic99",   0x20000, 0xc76630e1, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12871.ic103",  0x20000, 0x23401b1a, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12864.ic92",   0x20000, 0x77d6cff4, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12865.ic96",   0x20000, 0x1e7e685b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12866.ic100",  0x20000, 0xfdf31329, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12867.ic104",  0x20000, 0xb25e37fd, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12860.ic93",   0x20000, 0x86b64119, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12861.ic97",   0x20000, 0xbccff19b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12862.ic101",  0x20000, 0x7d4c3b05, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-12863.ic105",  0x20000, 0x85095053, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-12859.ic17",   0x10000, 0xd57881da, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-12876.ic11",   0x20000, 0xf72a34a0, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-12877.ic12",   0x20000, 0x18c1b6d2, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-12878.ic13",   0x20000, 0x7c212c15, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0144.key",     0x02000, 0x8740bbff, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};
	

STD_ROM_PICK(Rachero);
STD_ROM_FN(Rachero);

static struct BurnRomInfo SmgpRomDesc[] = {
	{ "epr12563b.58",     0x20000, 0xbaf1f333, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12564b.63",     0x20000, 0xb5191af0, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12576a.20",     0x20000, 0x2c9599c1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12577a.29",     0x20000, 0xabf9a50b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12609.93",      0x20000, 0xa867812f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12610.97",      0x20000, 0x53b99417, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12611.101",     0x20000, 0xbd5c6ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12612.105",     0x20000, 0xac86e890, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0126a.key",    0x02000, 0x2abc1982, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgp);
STD_ROM_FN(Smgp);

static struct BurnRomInfo Smgp5RomDesc[] = {
	{ "epr12563.58",      0x20000, 0x6d7325ae, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12564.63",      0x20000, 0xadfbf921, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12576.20",      0x20000, 0x23266b26, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12577.29",      0x20000, 0xd5b53211, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12413.93",      0x20000, 0x2f1693df, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12414.97",      0x20000, 0xc78f3d45, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12415.101",     0x20000, 0x6080e9ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12416.105",     0x20000, 0x6f1f2769, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12535.8",       0x10000, 0x80453597, SYS16_ROM_Z80PROG3 | BRF_ESS | BRF_PRG },
	
	{ "epr12505.8",       0x08000, 0x5020788a, SYS16_ROM_Z80PROG4 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "mpr12437.20",      0x20000, 0xa1c7e712, SYS16_ROM_PCM2DATA | BRF_SND },
	{ "mpr12438.21",      0x20000, 0x6573d46b, SYS16_ROM_PCM2DATA | BRF_SND },
	{ "mpr12439.22",      0x20000, 0x13bf6de5, SYS16_ROM_PCM2DATA | BRF_SND },
	
	{ "317-0126.key",     0x02000, 0x4d917996, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgp5);
STD_ROM_FN(Smgp5);

static struct BurnRomInfo Smgp6RomDesc[] = {
	{ "epr12563a.58",     0x20000, 0x2e64b10e, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12564a.63",     0x20000, 0x5baba3e7, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12576a.20",     0x20000, 0x2c9599c1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12577a.29",     0x20000, 0xabf9a50b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12609.93",      0x20000, 0xa867812f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12610.97",      0x20000, 0x53b99417, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12611.101",     0x20000, 0xbd5c6ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12612.105",     0x20000, 0xac86e890, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0126a.key",    0x02000, 0x2abc1982, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};

STD_ROM_PICK(Smgp6);
STD_ROM_FN(Smgp6);

static struct BurnRomInfo SmgpjRomDesc[] = {
	{ "epr12432b.58",     0x20000, 0xc1a29db1, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12433b.63",     0x20000, 0x97199eb1, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12441a.20",     0x20000, 0x2c9599c1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12442a.29",     0x20000, 0x77a5ec16, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12413.93",      0x20000, 0x2f1693df, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12414.97",      0x20000, 0xc78f3d45, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12415.101",     0x20000, 0x6080e9ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12416.105",     0x20000, 0x6f1f2769, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0124a.key",    0x02000, 0x022a8a16, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgpj);
STD_ROM_FN(Smgpj);

static struct BurnRomInfo SmgpjaRomDesc[] = {
	{ "epr12432a.58",     0x20000, 0x22517672, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12433a.63",     0x20000, 0xa46b5d13, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12441a.20",     0x20000, 0x2c9599c1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12442a.29",     0x20000, 0x77a5ec16, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12413.93",      0x20000, 0x2f1693df, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12414.97",      0x20000, 0xc78f3d45, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12415.101",     0x20000, 0x6080e9ed, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12416.105",     0x20000, 0x6f1f2769, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0124a.key",    0x02000, 0x022a8a16, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgpja);
STD_ROM_FN(Smgpja);

static struct BurnRomInfo SmgpuRomDesc[] = {
	{ "epr12561c.58",     0x20000, 0xa5b0f3fe, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12562c.63",     0x20000, 0x799e55f4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12574a.20",     0x20000, 0xf8b5c38b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12575a.29",     0x20000, 0x248b1d17, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12609.93",      0x20000, 0xa867812f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12610.97",      0x20000, 0x53b99417, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12611.101",     0x20000, 0xbd5c6ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12612.105",     0x20000, 0xac86e890, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0125a.key",    0x02000, 0x3ecdb120, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgpu);
STD_ROM_FN(Smgpu);

static struct BurnRomInfo Smgpu1RomDesc[] = {
	{ "epr12561b.58",     0x20000, 0x80a32655, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12562b.63",     0x20000, 0xd525f2a8, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12574a.20",     0x20000, 0xf8b5c38b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12575a.29",     0x20000, 0x248b1d17, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12609.93",      0x20000, 0xa867812f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12610.97",      0x20000, 0x53b99417, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12611.101",     0x20000, 0xbd5c6ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12612.105",     0x20000, 0xac86e890, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0125a.key",    0x02000, 0x3ecdb120, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgpu1);
STD_ROM_FN(Smgpu1);

static struct BurnRomInfo Smgpu2RomDesc[] = {
	{ "epr12561a.58",     0x20000, 0xe505eb89, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12562a.63",     0x20000, 0xc3af4215, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12574a.20",     0x20000, 0xf8b5c38b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12575a.29",     0x20000, 0x248b1d17, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12609.93",      0x20000, 0xa867812f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12610.97",      0x20000, 0x53b99417, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12611.101",     0x20000, 0xbd5c6ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12612.105",     0x20000, 0xac86e890, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0125a.key",    0x02000, 0x3ecdb120, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgpu2);
STD_ROM_FN(Smgpu2);

static struct BurnRomInfo Smgpu3RomDesc[] = {
	{ "epr12561.58",      0x20000, 0x80a32655, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr12562.63",      0x20000, 0xd525f2a8, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
		
	{ "epr12574a.20",     0x20000, 0xf8b5c38b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr12575a.29",     0x20000, 0x248b1d17, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr12429.154",     0x10000, 0x5851e614, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12430.153",     0x10000, 0x05e00134, SYS16_ROM_TILES | BRF_GRA },
	{ "epr12431.152",     0x10000, 0x35572f4a, SYS16_ROM_TILES | BRF_GRA },
	
	{ "mpr12425.90",      0x20000, 0x14bf2a15, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12426.94",      0x20000, 0x28b60dc0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12427.98",      0x20000, 0x0a367928, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12428.102",     0x20000, 0xefa80ad5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12421.91",      0x20000, 0x25f46140, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12422.95",      0x20000, 0xcb51c8f6, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12423.99",      0x20000, 0x0be9818e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12424.103",     0x20000, 0x0ce00dfc, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12417.92",      0x20000, 0xa806eabf, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12418.96",      0x20000, 0xed1a0f2b, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12419.100",     0x20000, 0xce4568cb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "mpr12420.104",     0x20000, 0x679442eb, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12609.93",      0x20000, 0xa867812f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12610.97",      0x20000, 0x53b99417, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12611.101",     0x20000, 0xbd5c6ab0, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr12612.105",     0x20000, 0xac86e890, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr12436.17",      0x10000, 0x16ec5f0a, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr12587.14",      0x08000, 0x2afe648b, SYS16_ROM_Z80PROG2 | BRF_ESS | BRF_PRG },
	
	{ "mpr12437.11",      0x20000, 0xa1c7e712, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12438.12",      0x20000, 0x6573d46b, SYS16_ROM_PCMDATA | BRF_SND },
	{ "mpr12439.13",      0x20000, 0x13bf6de5, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0125a.key",    0x02000, 0x3ecdb120, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Smgpu3);
STD_ROM_FN(Smgpu3);

static struct BurnRomInfo ThndrbldRomDesc[] = {
	{ "epr-11405.ic58",   0x20000, 0xe057dd5a, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11406.ic63",   0x20000, 0xc6b994b8, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11306.ic57",   0x20000, 0x4b95f2b4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11307.ic62",   0x20000, 0x2d6833e4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11390.ic20",   0x20000, 0xed988fdb, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11391.ic29",   0x20000, 0x12523bc1, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11310.ic21",   0x20000, 0x5d9fa02c, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11311.ic30",   0x20000, 0x483de21b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr-11314.ic154",  0x10000, 0xd4f954a9, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-11315.ic153",  0x10000, 0x35813088, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-11316.ic152",  0x10000, 0x84290dff, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr-11323.ic90",   0x20000, 0x27e40735, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11322.ic94",   0x20000, 0x10364d74, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11321.ic98",   0x20000, 0x8e738f58, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11320.ic102",  0x20000, 0xa95c76b8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11327.ic91",   0x20000, 0xdeae90f1, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11326.ic95",   0x20000, 0x29198403, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11325.ic99",   0x20000, 0xb9e98ae9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11324.ic103",  0x20000, 0x9742b552, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11331.ic92",   0x20000, 0x3a2c042e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11330.ic96",   0x20000, 0xaa7c70c5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11329.ic100",  0x20000, 0x31b20257, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11328.ic104",  0x20000, 0xda39e89c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11395.ic93",   0x20000, 0x90775579, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11394.ic97",   0x20000, 0x5f2783be, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11393.ic101",  0x20000, 0x525e2e1d, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11392.ic105",  0x20000, 0xb4a382f7, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11313.ic29",   0x10000, 0x6a56c4c3, SYS16_ROM_ROAD | BRF_GRA },
	
	{ "epr-11396.ic17",   0x10000, 0xd37b54a4, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11317.ic11",   0x20000, 0xd4e7ac1f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11318.ic12",   0x20000, 0x70d3f02c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11319.ic13",   0x20000, 0x50d9242e, SYS16_ROM_PCMDATA | BRF_SND },
	
	{ "317-0056.key",     0x02000, 0xb40cd2c5, SYS16_ROM_KEY | BRF_ESS | BRF_PRG },
};


STD_ROM_PICK(Thndrbld);
STD_ROM_FN(Thndrbld);

static struct BurnRomInfo Thndrbd1RomDesc[] = {
	{ "epr-11304.ic58",   0x20000, 0xa90630ef, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11305.ic63",   0x20000, 0x9ba3ef61, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11306.ic57",   0x20000, 0x4b95f2b4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	{ "epr-11307.ic62",   0x20000, 0x2d6833e4, SYS16_ROM_PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11308.ic20",   0x20000, 0x7956c238, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11309.ic29",   0x20000, 0xc887f620, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11310.ic21",   0x20000, 0x5d9fa02c, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },
	{ "epr-11311.ic30",   0x20000, 0x483de21b, SYS16_ROM_PROG2 | BRF_ESS | BRF_PRG },

	{ "epr-11314.ic154",  0x10000, 0xd4f954a9, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-11315.ic153",  0x10000, 0x35813088, SYS16_ROM_TILES | BRF_GRA },
	{ "epr-11316.ic152",  0x10000, 0x84290dff, SYS16_ROM_TILES | BRF_GRA },
	
	{ "epr-11323.ic90",   0x20000, 0x27e40735, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11322.ic94",   0x20000, 0x10364d74, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11321.ic98",   0x20000, 0x8e738f58, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11320.ic102",  0x20000, 0xa95c76b8, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11327.ic91",   0x20000, 0xdeae90f1, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11326.ic95",   0x20000, 0x29198403, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11325.ic99",   0x20000, 0xb9e98ae9, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11324.ic103",  0x20000, 0x9742b552, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11331.ic92",   0x20000, 0x3a2c042e, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11330.ic96",   0x20000, 0xaa7c70c5, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11329.ic100",  0x20000, 0x31b20257, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11328.ic104",  0x20000, 0xda39e89c, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11335.ic93",   0x20000, 0xf19b3e86, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11334.ic97",   0x20000, 0x348f91c7, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11333.ic101",  0x20000, 0x05a2333f, SYS16_ROM_SPRITES | BRF_GRA },
	{ "epr-11332.ic105",  0x20000, 0xdc089ec6, SYS16_ROM_SPRITES | BRF_GRA },
	
	{ "epr-11313.ic29",   0x10000, 0x6a56c4c3, SYS16_ROM_ROAD | BRF_GRA },
	
	{ "epr-11312.ic17",   0x10000, 0x3b974ed2, SYS16_ROM_Z80PROG | BRF_ESS | BRF_PRG },
	
	{ "epr-11317.ic11",   0x20000, 0xd4e7ac1f, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11318.ic12",   0x20000, 0x70d3f02c, SYS16_ROM_PCMDATA | BRF_SND },
	{ "epr-11319.ic13",   0x20000, 0x50d9242e, SYS16_ROM_PCMDATA | BRF_SND },
};


STD_ROM_PICK(Thndrbd1);
STD_ROM_FN(Thndrbd1);

/*====================================================
Memory Handlers
====================================================*/

static UINT8 iochip_regs[2][8];
static UINT8 iochip_force_input;

inline UINT16 iochip_r(int which, int port, int inputval)
{
	UINT16 result = iochip_regs[which][port];

	/* for ports 0-3, the direction is controlled 4 bits at a time by register 6 */
	if (port <= 3)
	{
		if (iochip_force_input || ((iochip_regs[which][6] >> (2*port+0)) & 1))
			result = (result & ~0x0f) | (inputval & 0x0f);
		if (iochip_force_input || ((iochip_regs[which][6] >> (2*port+1)) & 1))
			result = (result & ~0xf0) | (inputval & 0xf0);
	}

	/* for port 4, the direction is controlled 1 bit at a time by register 7 */
	else
	{
		if ((iochip_regs[which][7] >> 0) & 1)
			result = (result & ~0x01) | (inputval & 0x01);
		if ((iochip_regs[which][7] >> 1) & 1)
			result = (result & ~0x02) | (inputval & 0x02);
		if ((iochip_regs[which][7] >> 2) & 1)
			result = (result & ~0x04) | (inputval & 0x04);
		if ((iochip_regs[which][7] >> 3) & 1)
			result = (result & ~0x08) | (inputval & 0x08);
		result &= 0x0f;
	}
	return result;
}

unsigned short __fastcall XBoardReadWord(unsigned int a)
{
	if (a >= 0x0e0000 && a <= 0x0e0007) {
		return System16MultiplyChipRead(0, (a - 0x0e0000) >> 1);
	}
	
	if (a >= 0x0e4000 && a <= 0x0e401f) {
		return System16DivideChipRead(0, (a - 0x0e4000) >> 1);
	}
	
	if (a >= 0x0e8000 && a <= 0x0e801f) {
		return System16CompareTimerChipRead(0, (a - 0x0e8000) >> 1);
	}
	
	if (a >= 0x2e0000 && a <= 0x2e0007) {
		return System16MultiplyChipRead(1, (a - 0x2e0000) >> 1);
	}
	
	if (a >= 0x2e4000 && a <= 0x2e401f) {
		return System16DivideChipRead(1, (a - 0x2e4000) >> 1);
	}
	
	if (a >= 0x2e8000 && a <= 0x2e800f) {
		return System16CompareTimerChipRead(1, (a - 0x2e8000) >> 1);
	}
	
	switch (a) {
		case 0x2ee000: {
			// swap the halves of the road RAM
			memcpy(System16RoadRamBuff, System16RoadRam, 0x1000);
			UINT32 *src = (UINT32 *)System16RoadRamBuff;
			UINT32 *dst = (UINT32 *)System16RoadRam;
			for (int i = 0; i < 0x1000/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
		
			return System16RoadControl;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Word -> 0x%06X\n"), a);
#endif

	return 0xffff;
}

unsigned char __fastcall XBoardReadByte(unsigned int a)
{
	if (a >= 0x0e4000 && a <= 0x0e401f) {
		return (unsigned char)System16DivideChipRead(0, (a - 0x0e4000) >> 1);
	}
	
	if (a >= 0x2e4000 && a <= 0x2e401f) {
		return (unsigned char)System16DivideChipRead(0, (a - 0x2e4000) >> 1);
	}
	
	if (a >= 0x140000 && a <= 0x14ffff) {
		int offset = ((a - 0x140000) >> 1) & 7;
		switch (offset) {
			case 0: {
				return iochip_r(0, 0, 0xff);
			}
	
			case 1: {
				return iochip_r(0, 1, 0xff);
			}
			
			case 2: {
				return iochip_r(0, 2, 0);
			}
			
			case 3: {
				return iochip_r(0, 3, 0);
			}
			
			case 4: {
				return iochip_r(0, 4, 0);
			}
		}
		return 0;
	}
	
	if (a == 0x150001) return 0xff - System16Input[0];
	if (a == 0x150003) return 0xff - System16Input[1];
	if (a == 0x150005) return System16Dip[0];
	if (a == 0x150007) return System16Dip[1];
	
	if (a >= 0x150008 && a <= 0x15ffff) {
		int offset = ((a - 0x150000) >> 1) & 7;
		switch (offset) {
			case 0: {
				return iochip_r(1, 0, ~System16Input[0]);
			}
			
			case 1: {
				return iochip_r(1, 1, 0xff);
			}
			
			case 2: {
				return iochip_r(1, 2, System16Dip[0]);
			}
			
			case 3: {
				return iochip_r(1, 3, System16Dip[1]);
			}
			
			case 4: {
				return iochip_r(1, 4, 0);
			}
		}
		return 0;
	}
	
	switch (a) {
		case 0x130001: {
			if (System16ProcessAnalogControlsDo) return System16ProcessAnalogControlsDo((iochip_regs[0][2] >> 2) & 7);
		}
		
		case 0x2ee000:
		case 0x2ee001:
		case 0x2ee002: {
			// swap the halves of the road RAM
			memcpy(System16RoadRamBuff, System16RoadRam, 0x1000);
			UINT32 *src = (UINT32 *)System16RoadRamBuff;
			UINT32 *dst = (UINT32 *)System16RoadRam;
			for (int i = 0; i < 0x1000/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
		
			return System16RoadControl;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Read Byte -> 0x%06X\n"), a);
#endif

	return 0xff;
}

void __fastcall XBoardWriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x0c0000 && a <= 0x0cffff) {
		System16BTileWordWrite(a - 0x0c0000, d);
		return;
	}

	if (a >= 0x0e0000 && a <= 0x0e0007) {
		System16MultiplyChipWrite(0, (a - 0x0e0000) >> 1, d);
		return;
	}
	
	if (a >= 0x0e4000 && a <= 0x0e401f) {
		System16DivideChipWrite(0, (a - 0x0e4000) >> 1, d);
		return;
	}
	
	if (a >= 0x0e8000 && a <= 0x0e801f) {
		if (a == 0x0e8016) {
			System16SoundLatch = d & 0xff;
			ZetOpen(0);
			ZetNmi();
			ZetClose();
		}
		
		System16CompareTimerChipWrite(0, (a - 0x0e8000) >> 1, d);
		return;
	}
	
	if (a >= 0x140000 && a <= 0x14ffff) {
		int offset = ((a - 0x140000) >> 1) & 7;
		
		iochip_regs[0][offset] = d;
		
		if (offset == 2) {
			System16VideoEnable = d & 0x20;
			if (!(d & 0x01)) {
				ZetOpen(0);
				ZetReset();
				ZetClose();
			}
		}
		return;
	}
	
	if (a >= 0x150000 && a <= 0x15ffff) {
		int offset = ((a - 0x150000) >> 1) & 7;
		
		iochip_regs[1][offset] = d;
		return;
	}
	
	if (a >= 0x2e0000 && a <= 0x2e0007) {
		System16MultiplyChipWrite(1, (a - 0x2e0000) >> 1, d);
		return;
	}
	
	if (a >= 0x2e4000 && a <= 0x2e401f) {
		System16DivideChipWrite(1, (a - 0x2e4000) >> 1, d);
		return;
	}
	
	if (a >= 0x2e8000 && a <= 0x2e800f) {
		System16CompareTimerChipWrite(1, (a - 0x2e8000) >> 1, d);
		return;
	}
	
	switch (a) {
		case 0x110000: {
			UINT32 *src = (UINT32 *)System16SpriteRam;
			UINT32 *dst = (UINT32 *)System16SpriteRamBuff;

			/* swap the halves of the sprite RAM */
			for (unsigned int i = 0; i < System16SpriteRamSize/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
		
			/* hack for thunderblade */
			memset(System16SpriteRam, 0xff, System16SpriteRamSize);
			
			return;
		}
		
		case 0x2ee000: {
			System16RoadControl = d & 7;
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

void __fastcall XBoardWriteByte(unsigned int a, unsigned char d)
{
	if (a >= 0x0c0000 && a <= 0x0cffff) {
		System16BTileByteWrite((a - 0x0c0000) ^ 1, d);
		return;
	}
	
	if (a >= 0x140000 && a <= 0x14ffff) {
		int offset = ((a - 0x140000) >> 1) & 7;
		
		iochip_regs[0][offset] = d;
		
		if (offset == 2) {
			System16VideoEnable = d & 0x20;
			if (!(d & 0x01)) {
				ZetOpen(0);
				ZetReset();
				ZetClose();
			}
		}
		return;
	}
	
	if (a >= 0x150000 && a <= 0x15ffff) {
		int offset = ((a - 0x150000) >> 1) & 7;
		
		iochip_regs[1][offset] = d;
		return;
	}

	switch (a) {
		case 0x110000:
		case 0x110001: {
			UINT32 *src = (UINT32 *)System16SpriteRam;
			UINT32 *dst = (UINT32 *)System16SpriteRamBuff;

			/* swap the halves of the sprite RAM */
			for (unsigned int i = 0; i < System16SpriteRamSize/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
		
			/* hack for thunderblade */
			memset(System16SpriteRam, 0xff, System16SpriteRamSize);
			return;
		}
		
		case 0x130001: {
			System16AnalogSelect = d;
			return;
		}
		
		case 0x0e8017: {
			System16CompareTimerChipWrite(0, (a - 0x0e8000) >> 1, d);
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

unsigned short __fastcall XBoard2ReadWord(unsigned int a)
{
	if (a >= 0x0e0000 && a <= 0x0e0007) {
		return System16MultiplyChipRead(1, (a - 0x0e0000) >> 1);
	}
	
	if (a >= 0x0e4000 && a <= 0x0e401f) {
		return System16DivideChipRead(1, (a - 0x0e4000) >> 1);
	}
	
	if (a >= 0x0e8000 && a <= 0x0e800f) {
		return System16CompareTimerChipRead(1, (a - 0x0e8000) >> 1);
	}
	
	if (a >= 0x2e0000 && a <= 0x2e0007) {
		return System16MultiplyChipRead(1, (a - 0x2e0000) >> 1);
	}
	
	if (a >= 0x2e4000 && a <= 0x2e401f) {
		return System16DivideChipRead(1, (a - 0x2e4000) >> 1);
	}
	
	switch (a) {
		case 0x0ee000:
		case 0x2ee000: {
			// swap the halves of the road RAM
			memcpy(System16RoadRamBuff, System16RoadRam, 0x1000);
			UINT32 *src = (UINT32 *)System16RoadRamBuff;
			UINT32 *dst = (UINT32 *)System16RoadRam;
			for (int i = 0; i < 0x1000/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
		
			return System16RoadControl;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 # 2 Read Word -> 0x%06X\n"), a);
#endif

	return 0;
}

unsigned char __fastcall XBoard2ReadByte(unsigned int a)
{
	switch (a) {
		case 0x0ee000:
		case 0x0ee001: {
			// swap the halves of the road RAM
			memcpy(System16RoadRamBuff, System16RoadRam, 0x1000);
			UINT32 *src = (UINT32 *)System16RoadRamBuff;
			UINT32 *dst = (UINT32 *)System16RoadRam;
			for (int i = 0; i < 0x1000/4; i++) {
				UINT32 temp = *src;
				*src++ = *dst;
				*dst++ = temp;
			}
		
			return System16RoadControl;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 # 2 Read Byte -> 0x%06X\n"), a);
#endif

	return 0;
}

void __fastcall XBoard2WriteWord(unsigned int a, unsigned short d)
{
	if (a >= 0x0e0000 && a <= 0x0e0007) {
		System16MultiplyChipWrite(1, (a - 0x0e0000) >> 1, d);
		return;
	}
	
	if (a >= 0x0e4000 && a <= 0x0e401f) {
		System16DivideChipWrite(1, (a - 0x0e4000) >> 1, d);
		return;
	}
	
	if (a >= 0x0e8000 && a <= 0x0e800f) {
		System16CompareTimerChipWrite(1, (a - 0x0e8000) >> 1, d);
		return;
	}
	
	if (a >= 0x2e0000 && a <= 0x2e0007) {
		System16MultiplyChipWrite(1, (a - 0x2e0000) >> 1, d);
		return;
	}
	
	if (a >= 0x2e4000 && a <= 0x2e401f) {
		System16DivideChipWrite(1, (a - 0x2e4000) >> 1, d);
		return;
	}

	switch (a) {
		case 0x0ee000:
		case 0x2ee000: {
			System16RoadControl = d & 7;
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 # 2 Write Word -> 0x%06X, 0x%04X\n"), a, d);
#endif
}

void __fastcall XBoard2WriteByte(unsigned int a, unsigned char d)
{
	switch (a) {
		case 0x0ee001: 
		case 0x2ee000: {
			System16RoadControl = d & 7;
			return;
		}
	}

#if 0 && defined FBA_DEBUG
	bprintf(PRINT_NORMAL, _T("68000 # 2 Write Byte -> 0x%06X, 0x%02X\n"), a, d);
#endif
}

/*====================================================
Driver Inits
====================================================*/

unsigned char AbcopProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 0: {
			temp = 0x80 - (System16AnalogPort0 >> 4);
			if (temp < 0x20) temp = 0x20;
			if (temp > 0xe0) temp = 0xe0;
			return temp;
		}
		
		case 1: {
			if (System16AnalogPort1 > 1) return 0xff;
			return 0;
		}
	}
	
	return 0;
}

unsigned char AburnerProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 0: {
			temp = 0x80 + (System16AnalogPort0 >> 4);
			if (temp < 0x20) temp = 0x20;
			if (temp > 0xe0) temp = 0xe0;
			return temp;
		}
		
		case 1: {
			temp = 0x80 - (System16AnalogPort1 >> 4);
			if (temp < 0x40) temp = 0x40;
			if (temp > 0xc0) temp = 0xc0;
			return temp;
		}
		
		case 2: {
			temp = 0x80 + (System16AnalogPort2 >> 4);
			if (temp > 0xc0) return 0xff;
			if (temp < 0x40) return 0;
			return 0x80;
		}
	}
	
	return 0;
}

unsigned char GpriderProcessAnalogControls(UINT16 value)
{
	switch (value) {
		case 0: {
			return 0x80 + (System16AnalogPort0 >> 4);
		}
		
		case 1: {
			if (System16AnalogPort1 > 1) return 0x10;
			return 0xef;
		}
		
		case 2: {
			if (System16AnalogPort2 > 1) return 0x10;
			return 0xef;
		}
	}
	
	return 0;
}

unsigned char LoffireProcessAnalogControls(UINT16 value)
{
	switch (value) {
		case 0: {
			return BurnGunReturnX(0);
		}
		
		case 1: {
			return ~BurnGunReturnY(0);
		}
		
		case 2: {
			return BurnGunReturnX(1);
		}
		
		case 3: {
			return ~BurnGunReturnY(1);
		}
	}
	
	return 0;
}

unsigned char RacheroProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 0: {
			temp = 0x80 - (System16AnalogPort0 >> 4);
			if (temp < 0x20) temp = 0x20;
			if (temp > 0xe0) temp = 0xe0;
			return temp;
		}
		
		case 1: {
			if (System16AnalogPort1 > 1) return 0xff;
			return 0;
		}
		
		case 2: {
			if (System16AnalogPort2 > 1) return 0xff;
			return 0;
		}
	}
	
	return 0;
}

unsigned char SmgpProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 0: {
			temp = 0x80 + (System16AnalogPort0 >> 4);
			if (temp < 0x39) temp = 0x38;
			if (temp > 0xc8) temp = 0xc8;
			return temp;
		}
		
		case 1: {
			if (System16AnalogPort1 > 1) return 0xb8;
			return 0x38;
		}
		
		case 2: {
			if (System16AnalogPort2 > 1) return 0xa8;
			return 0x28;
		}
	}
	
	return 0;
}

unsigned char ThndrbldProcessAnalogControls(UINT16 value)
{
	unsigned char temp = 0;
	
	switch (value) {
		case 0: {
			return 0x80 - (System16AnalogPort0 >> 4);
		}
		
		case 1: {
			temp = 0x80 + (System16AnalogPort2 >> 4);
			if (temp == 1) temp = 0;
			return temp;
		}
		
		case 2: {
			return 0x80 + (System16AnalogPort1 >> 4);
		}
	}
	
	return 0;
}

static int AbcopInit()
{
	System16ProcessAnalogControlsDo = AbcopProcessAnalogControls;
	
	int nRet = System16Init();

	return nRet;
}

static int Aburner2Init()
{
	System16ProcessAnalogControlsDo = AburnerProcessAnalogControls;
	
	int nRet = System16Init();
	
	if (!nRet) {
		System16RoadPriority = 0;
	}

	return nRet;
}

static int GpriderInit()
{
	System16ProcessAnalogControlsDo = GpriderProcessAnalogControls;
	
	int nRet = System16Init();

	return nRet;
}

static int LoffireInit()
{
	BurnGunInit(2, true);
	
	System16ProcessAnalogControlsDo = LoffireProcessAnalogControls;
	
	int nRet = System16Init();

	return nRet;
}

static int RacheroInit()
{
	System16ProcessAnalogControlsDo = RacheroProcessAnalogControls;
	
	int nRet = System16Init();

	return nRet;
}

static int SmgpInit()
{
	System16ProcessAnalogControlsDo = SmgpProcessAnalogControls;
	
	int nRet = System16Init();

	return nRet;
}

static int ThndrbldInit()
{
	System16ProcessAnalogControlsDo = ThndrbldProcessAnalogControls;
	
	int nRet = System16Init();

	return nRet;
}

static int XBoardExit()
{
	memset(iochip_regs, 0, sizeof(iochip_regs));
	iochip_force_input = 0;

	return System16Exit();
}

static int XBoardScan(int nAction,int *pnMin)
{
	if (pnMin != NULL) {					// Return minimum compatible version
		*pnMin =  0x029660;
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(iochip_regs);
		SCAN_VAR(iochip_force_input);
	}
	
	return System16Scan(nAction, pnMin);;
}

/*====================================================
Driver Defs
====================================================*/

struct BurnDriver BurnDrvAbcop = {
	"abcop", NULL, NULL, "1990",
	"A.B. Cop (FD1094 317-0169b)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, AbcopRomInfo, AbcopRomName, AbcopInputInfo, AbcopDIPInfo,
	AbcopInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvAburner2 = {
	"aburner2", NULL, NULL, "1987",
	"After Burner II\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32,
	NULL, Aburner2RomInfo, Aburner2RomName, Aburner2InputInfo, Aburner2DIPInfo,
	Aburner2Init, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvAburner = {
	"aburner", "aburner2", NULL, "1987",
	"After Burner (Japan)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32,
	NULL, AburnerRomInfo, AburnerRomName, Aburner2InputInfo, AburnerDIPInfo,
	Aburner2Init, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvGprider = {
	"gprider", NULL, NULL, "1990",
	"GP Rider (set 2, World, FD1094 317-0163)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	0, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, GpriderRomInfo, GpriderRomName, GpriderInputInfo, GpriderDIPInfo,
	GpriderInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvGprider1 = {
	"gprider1", "gprider", NULL, "1990",
	"GP Rider (set 1, US, FD1094 317-0162)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Gprider1RomInfo, Gprider1RomName, GpriderInputInfo, GpriderDIPInfo,
	GpriderInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvLoffire = {
	"loffire", NULL, NULL, "1989",
	"Line of Fire / Bakudan Yarou (World, FD1094 317-0136)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, LoffireRomInfo, LoffireRomName, LoffireInputInfo, LoffireDIPInfo,
	LoffireInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvLoffirej = {
	"loffirej", "loffire", NULL, "1989",
	"Line of Fire / Bakudan Yarou (Japan, FD1094 317-0134)\0", NULL, "Sega", "X-Board",
	L"Line of Fire / Bakudan Yarou (Japan, FD1094 317-0134)\0\u7206\u5F3E\u91CE\u90CE\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, LoffirejRomInfo, LoffirejRomName, LoffireInputInfo, LoffireDIPInfo,
	LoffireInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvLoffireu = {
	"loffireu", "loffire", NULL, "1989",
	"Line of Fire / Bakudan Yarou (US, FD1094 317-0135)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, LoffireuRomInfo, LoffireuRomName, LoffireInputInfo, LoffireDIPInfo,
	LoffireInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvRachero = {
	"rachero", NULL, NULL, "1989",
	"Racing Hero (FD1094 317-0144)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, RacheroRomInfo, RacheroRomName, RacheroInputInfo, RacheroDIPInfo,
	RacheroInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriverD BurnDrvRascot2 = {
	"rascot2", NULL, NULL, "19??",
	"Royal Ascot 2\0", NULL, "????", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32,
	NULL, Rascot2RomInfo, Rascot2RomName, Aburner2InputInfo, Aburner2DIPInfo,
	Aburner2Init, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgp = {
	"smgp", NULL, NULL, "1989",
	"Super Monaco GP (set 9, World, Rev B, 'Twin', FD1094 317-0126a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, SmgpRomInfo, SmgpRomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgp5 = {
	"smgp5", "smgp", NULL, "1989",
	"Super Monaco GP (set 7, World, 'Air Drive Cabinet', FD1094 317-0126)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Smgp5RomInfo, Smgp5RomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgp6 = {
	"smgp6", "smgp", NULL, "1989",
	"Super Monaco GP (set 8, World, Rev A, FD1094 317-0126a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Smgp6RomInfo, Smgp6RomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgpj = {
	"smgpj", "smgp", NULL, "1989",
	"Super Monaco GP (set 2, Japan, Rev B, FD1094 317-0124a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, SmgpjRomInfo, SmgpjRomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgpja = {
	"smgpja", "smgp", NULL, "1989",
	"Super Monaco GP (set 1, Japan, Rev A, FD1094 317-0124a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, SmgpjaRomInfo, SmgpjaRomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgpu = {
	"smgpu", "smgp", NULL, "1989",
	"Super Monaco GP (set 6, US, Rev C, FD1094 317-0125a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, SmgpuRomInfo, SmgpuRomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgpu1 = {
	"smgpu1", "smgp", NULL, "1989",
	"Super Monaco GP (set 5, US, Rev B, FD1094 317-0125a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Smgpu1RomInfo, Smgpu1RomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgpu2 = {
	"smgpu2", "smgp", NULL, "1989",
	"Super Monaco GP (set 4, US, Rev A, FD1094 317-0125a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Smgpu2RomInfo, Smgpu2RomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvSmgpu3 = {
	"smgpu3", "smgp", NULL, "1989",
	"Super Monaco GP (set 3, US, FD1094 317-0125a)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, Smgpu3RomInfo, Smgpu3RomName, SmgpInputInfo, SmgpDIPInfo,
	SmgpInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvThndrbld = {
	"thndrbld", NULL, NULL, "1987",
	"Thunder Blade (upright, FD1094 317-0056)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32 | HARDWARE_SEGA_FD1094_ENC,
	NULL, ThndrbldRomInfo, ThndrbldRomName, ThndrbldInputInfo, ThndrbldDIPInfo,
	ThndrbldInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};

struct BurnDriver BurnDrvThndrbd1 = {
	"thndrbd1", "thndrbld", NULL, "1987",
	"Thunder Blade (deluxe/standing, unprotected)\0", NULL, "Sega", "X-Board",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_SEGA_SYSTEMX | HARDWARE_SEGA_SPRITE_LOAD32,
	NULL, Thndrbd1RomInfo, Thndrbd1RomName, ThndrbldInputInfo, Thndrbd1DIPInfo,
	ThndrbldInit, XBoardExit, XBoardFrame, NULL, XBoardScan,
	0, NULL, NULL, NULL, NULL, 320, 224, 4, 3
};
