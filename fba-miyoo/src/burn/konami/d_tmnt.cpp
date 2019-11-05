#include "tmnt.h"

// TMNT ------------------------------------------------
// Based on DTMNT and MAME
//----------------------------------------------------------
// Input list

//----------------------------------------------------------
// Input list
static struct BurnInputInfo TmntInputList[] = {
	{"P1 Coin"   , BIT_DIGITAL  , TmntCoin+0,"p1 coin"},
	{"P1 Start"  , BIT_DIGITAL  , TmntStart+0,"p1 start"},
	{"P1 Attack" , BIT_DIGITAL  , TmntAttack+0,"p1 fire 1"},
	{"P1 Jump"   , BIT_DIGITAL  , TmntJump+0,"p1 fire 2"},
	{"P1 Up"     , BIT_DIGITAL  , TmntUp+0,"p1 up"},
	{"P1 Down"   , BIT_DIGITAL  , TmntDown+0,"p1 down"},
	{"P1 Left"   , BIT_DIGITAL  , TmntLeft+0,"p1 left"},
	{"P1 Right"  , BIT_DIGITAL  , TmntRight+0,"p1 right"},
	{"Service 1" , BIT_DIGITAL  , TmntService+0,"service"},

	{"P2 Coin"   , BIT_DIGITAL  , TmntCoin+1,"p2 coin"},
	{"P2 Start"  , BIT_DIGITAL  , TmntStart+1,"p2 start"},
	{"P2 Attack" , BIT_DIGITAL  , TmntAttack+1,"p2 fire 1"},
	{"P2 Jump"   , BIT_DIGITAL  , TmntJump+1,"p2 fire 2"},
	{"P2 Up"     , BIT_DIGITAL  , TmntUp+1,"p2 up"},
	{"P2 Down"   , BIT_DIGITAL  , TmntDown+1,"p2 down"},
	{"P2 Left"   , BIT_DIGITAL  , TmntLeft+1,"p2 left"},
	{"P2 Right"  , BIT_DIGITAL  , TmntRight+1,"p2 right"},
	{"Service 2" , BIT_DIGITAL  , TmntService+1,""},

	{"P3 Coin"   , BIT_DIGITAL  , TmntCoin+2,"p3 coin"},
	{"P3 Start"  , BIT_DIGITAL  , TmntStart+2,"p3 start"},
	{"P3 Attack" , BIT_DIGITAL  , TmntAttack+2,"p3 fire 1"},
	{"P3 Jump"   , BIT_DIGITAL  , TmntJump+2,"p3 fire 2"},
	{"P3 Up"     , BIT_DIGITAL  , TmntUp+2,"p3 up"},
	{"P3 Down"   , BIT_DIGITAL  , TmntDown+2,"p3 down"},
	{"P3 Left"   , BIT_DIGITAL  , TmntLeft+2,"p3 left"},
	{"P3 Right"  , BIT_DIGITAL  , TmntRight+2,"p3 right"},
	{"Service 3" , BIT_DIGITAL  , TmntService+2,""},

	{"P4 Coin"   , BIT_DIGITAL  , TmntCoin+3,"p4 coin"},
	{"P4 Start"  , BIT_DIGITAL  , TmntStart+3,"p4 start"},
	{"P4 Attack" , BIT_DIGITAL  , TmntAttack+3,"p4 fire 1"},
	{"P4 Jump"   , BIT_DIGITAL  , TmntJump+3,"p4 fire 2"},
	{"P4 Up"     , BIT_DIGITAL  , TmntUp+3,"p4 up"},
	{"P4 Down"   , BIT_DIGITAL  , TmntDown+3,"p4 down"},
	{"P4 Left"   , BIT_DIGITAL  , TmntLeft+3,"p4 left"},
	{"P4 Right"  , BIT_DIGITAL  , TmntRight+3,"p4 right"},
	{"Service 4" , BIT_DIGITAL  , TmntService+3,""},

	{"Reset"     , BIT_DIGITAL  , &DrvReset	 ,"reset"},

	{"Dip 1"     , BIT_DIPSWITCH, TmntDip+0  ,"dip"},
	{"Dip 2"     , BIT_DIPSWITCH, TmntDip+1  ,"dip"},
	{"Dip 3"     , BIT_DIPSWITCH, TmntDip+2  ,"dip"},
};

static struct BurnInputInfo Tmnt2pInputList[] = {
	{"P1 Coin"   , BIT_DIGITAL  , TmntCoin+0,"p1 coin"},
	{"P1 Start"  , BIT_DIGITAL  , Tmnt2pStart+0,"p1 start"},
	{"P1 Attack" , BIT_DIGITAL  , TmntAttack+0,"p1 fire 1"},
	{"P1 Jump"   , BIT_DIGITAL  , TmntJump+0,"p1 fire 2"},
	{"P1 Up"     , BIT_DIGITAL  , TmntUp+0,"p1 up"},
	{"P1 Down"   , BIT_DIGITAL  , TmntDown+0,"p1 down"},
	{"P1 Left"   , BIT_DIGITAL  , TmntLeft+0,"p1 left"},
	{"P1 Right"  , BIT_DIGITAL  , TmntRight+0,"p1 right"},
	{"Service 1" , BIT_DIGITAL  , TmntService+0,"service"},

	{"P2 Coin"   , BIT_DIGITAL  , TmntCoin+1,"p2 coin"},
	{"P2 Start"  , BIT_DIGITAL  , Tmnt2pStart+1,"p2 start"},
	{"P2 Attack" , BIT_DIGITAL  , TmntAttack+1,"p2 fire 1"},
	{"P2 Jump"   , BIT_DIGITAL  , TmntJump+1,"p2 fire 2"},
	{"P2 Up"     , BIT_DIGITAL  , TmntUp+1,"p2 up"},
	{"P2 Down"   , BIT_DIGITAL  , TmntDown+1,"p2 down"},
	{"P2 Left"   , BIT_DIGITAL  , TmntLeft+1,"p2 left"},
	{"P2 Right"  , BIT_DIGITAL  , TmntRight+1	,"p2 right"},
	{"Service 2" , BIT_DIGITAL  , TmntService+1,""},

	{"Reset"     , BIT_DIGITAL  , &DrvReset	 ,"reset"},

	{"Dip 1"     , BIT_DIPSWITCH, TmntDip+0  ,"dip"},
	{"Dip 2"     , BIT_DIPSWITCH, TmntDip+1  ,"dip"},
	{"Dip 3"     , BIT_DIPSWITCH, TmntDip+2  ,"dip"},
};

STDINPUTINFO(Tmnt);
STDINPUTINFO(Tmnt2p);

static struct BurnDIPInfo TmntDIPList[]= {
	// Default Values
	{0x25, 0xff, 0xff, 0x00, NULL},
	{0x26, 0xff, 0xff, 0x20, NULL},
	{0x27, 0xff, 0xff, 0x00, NULL},

	// Dip 1
	{0   , 0xfe, 0   , 16  , "Coin 1"},
	{0x25, 0x01, 0x0f, 0x00, "1 coin 1 play"},
	{0x25, 0x01, 0x0f, 0x01, "1 coin 2 plays"},
	{0x25, 0x01, 0x0f, 0x02, "1 coin 3 plays"},
	{0x25, 0x01, 0x0f, 0x03, "1 coin 4 plays"},
	{0x25, 0x01, 0x0f, 0x04, "1 coin 5 plays"},
	{0x25, 0x01, 0x0f, 0x05, "1 coin 6 plays"},
	{0x25, 0x01, 0x0f, 0x06, "1 coin 7 plays"},
	{0x25, 0x01, 0x0f, 0x07, "2 coins 1 play"},
	{0x25, 0x01, 0x0f, 0x08, "2 coins 3 plays"},
	{0x25, 0x01, 0x0f, 0x09, "2 coins 5 plays"},
	{0x25, 0x01, 0x0f, 0x0a, "3 coins 1 play"},
	{0x25, 0x01, 0x0f, 0x0b, "3 coins 2 plays"},
	{0x25, 0x01, 0x0f, 0x0c, "3 coins 4 plays"},
	{0x25, 0x01, 0x0f, 0x0d, "4 coins 1 play"},
  {0x25, 0x01, 0x0f, 0x0e, "4 coins 3 plays"},
	{0x25, 0x01, 0x0f, 0x0f, "5 coins 1 play"},

	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"},
	{0x26, 0x01, 0x60, 0x00, "Easy"},
	{0x26, 0x01, 0x60, 0x20, "Normal"},
	{0x26, 0x01, 0x60, 0x40, "Difficult"},
	{0x26, 0x01, 0x60, 0x60, "Very difficult"},

	{0   , 0xfe, 0   , 4   , "Players"},
	{0x26, 0x01, 0x03, 0x00, "1"},
	{0x26, 0x01, 0x03, 0x01, "2"},
	{0x26, 0x01, 0x03, 0x02, "3"},
	{0x26, 0x01, 0x03, 0x03, "5"},

	{0   , 0xfe, 0   , 2   , "Demo sound"},
	{0x26, 0x01, 0x80, 0x00, "Off"},
	{0x26, 0x01, 0x80, 0x80, "On"},

	// Dip 3
	{0   , 0xfe, 0   , 2   , "Screen"},
	{0x27, 0x01, 0x01, 0x00, "Normal"},
	{0x27, 0x01, 0x01, 0x01, "Upside down"},

	{0   , 0xfe, 0   , 2   , NULL},
	{0x27, 0x01, 0x04, 0x00, "Game mode"},
	{0x27, 0x01, 0x04, 0x04, "Test mode"},
};

static struct BurnDIPInfo Tmnt2pDIPList[]= {
	// Default Values
	{0x13, 0xff, 0xff, 0x00, NULL},
	{0x14, 0xff, 0xff, 0x20, NULL},
	{0x15, 0xff, 0xff, 0x00, NULL},

	// Dip 1
	{0   , 0xfe, 0   , 16  , "Coin 1"},
	{0x13, 0x01, 0x0f, 0x00, "1 coin 1 play"},
	{0x13, 0x01, 0x0f, 0x01, "1 coin 2 plays"},
	{0x13, 0x01, 0x0f, 0x02, "1 coin 3 plays"},
	{0x13, 0x01, 0x0f, 0x03, "1 coin 4 plays"},
	{0x13, 0x01, 0x0f, 0x04, "1 coin 5 plays"},
	{0x13, 0x01, 0x0f, 0x05, "1 coin 6 plays"},
	{0x13, 0x01, 0x0f, 0x06, "1 coin 7 plays"},
	{0x13, 0x01, 0x0f, 0x07, "2 coins 1 play"},
	{0x13, 0x01, 0x0f, 0x08, "2 coins 3 plays"},
	{0x13, 0x01, 0x0f, 0x09, "2 coins 5 plays"},
	{0x13, 0x01, 0x0f, 0x0a, "3 coins 1 play"},
	{0x13, 0x01, 0x0f, 0x0b, "3 coins 2 plays"},
	{0x13, 0x01, 0x0f, 0x0c, "3 coins 4 plays"},
	{0x13, 0x01, 0x0f, 0x0d, "4 coins 1 play"},
  {0x13, 0x01, 0x0f, 0x0e, "4 coins 3 plays"},
	{0x13, 0x01, 0x0f, 0x0f, "Free play"},

	{0   , 0xfe, 0   , 15  , "Coin 2"},
	{0x13, 0x82, 0xf0, 0x00, "1 coin 1 play"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x10, "1 coin 2 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x20, "1 coin 3 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x30, "1 coin 4 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x40, "1 coin 5 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x50, "1 coin 6 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x60, "1 coin 7 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x70, "2 coins 1 play"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x80, "2 coins 3 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0x90, "2 coins 5 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0xa0, "3 coins 1 play"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0xb0, "3 coins 2 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0xc0, "3 coins 4 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0xd0, "4 coins 1 play"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
	{0x13, 0x82, 0xf0, 0xe0, "4 coins 3 plays"},
	{0x13, 0x00, 0x0f, 0x0f, NULL},
//	{0x13, 0x82, 0xf0, 0xf0, "Invalid"},
//	{0x13, 0x00, 0x0f, 0x0f, NULL},

	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"},
	{0x14, 0x01, 0x60, 0x00, "Easy"},
	{0x14, 0x01, 0x60, 0x20, "Normal"},
	{0x14, 0x01, 0x60, 0x40, "Difficult"},
	{0x14, 0x01, 0x60, 0x60, "Very difficult"},

	{0   , 0xfe, 0   , 4   , "Players"},
	{0x14, 0x01, 0x03, 0x00, "1"},
	{0x14, 0x01, 0x03, 0x01, "2"},
	{0x14, 0x01, 0x03, 0x02, "3"},
	{0x14, 0x01, 0x03, 0x03, "5"},

	{0   , 0xfe, 0   , 2   , "Demo sound"},
	{0x14, 0x01, 0x80, 0x00, "Off"},
	{0x14, 0x01, 0x80, 0x80, "On"},

	// Dip 3
	{0   , 0xfe, 0   , 2   , "Screen"},
	{0x15, 0x01, 0x01, 0x00, "Normal"},
	{0x15, 0x01, 0x01, 0x01, "Upside down"},

	{0   , 0xfe, 0   , 2   , NULL},
	{0x15, 0x01, 0x04, 0x00, "Game mode"},
	{0x15, 0x01, 0x04, 0x04, "Test mode"},
};

STDDIPINFO(Tmnt);
STDDIPINFO(Tmnt2p);

// Rom information
// ==============================================
// Teenage Mutant Ninja Turtles (World 4 Players)
// ==============================================
static struct BurnRomInfo TmntRomDesc[] = {
	{ "963-x23.j17", 0x20000, 0xa9549004, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-x24.k17", 0x20000, 0xe5cc9067, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-x21.j15", 0x10000, 0x5789cf92, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-x22.k15", 0x10000, 0x0a74e277, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmnt);
STD_ROM_FN(Tmnt);

// ==================================================
// Teenage Mutant Ninja Turtles (US 4 Players, set 1)
// ==================================================
static struct BurnRomInfo TmntuRomDesc[] = {
	{ "963-r23.j17", 0x20000, 0xa7f61195, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-r24.k17", 0x20000, 0x661e056a, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-r21.j15", 0x10000, 0xde047bb6, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-r22.k15", 0x10000, 0xd86a0888, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmntu);
STD_ROM_FN(Tmntu);

// ==================================================
// Teenage Mutant Ninja Turtles (US 4 Players, set 2)
// ==================================================
static struct BurnRomInfo TmntuaRomDesc[] = {
	{ "963-j23.j17", 0x20000, 0xf77314e2, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-j24.k17", 0x20000, 0x47f662d3, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-j21.j15", 0x10000, 0x7bee9fe8, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-j22.k15", 0x10000, 0x2efed09f, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmntua);
STD_ROM_FN(Tmntua);

// ==========================================
// Teenage Mutant Hero Turtles (UK 4 Players)
// ==========================================
static struct BurnRomInfo TmhtRomDesc[] = {
	{ "963-f23.j17", 0x20000, 0x9cb5e461, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-f24.k17", 0x20000, 0x2d902fab, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-f21.j15", 0x10000, 0x9fa25378, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-f22.k15", 0x10000, 0x2127ee53, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmht);
STD_ROM_FN(Tmht);

// ==============================================
// Teenage Mutant Ninja Turtles (Japan 4 Players)
// ==============================================
static struct BurnRomInfo TmntjRomDesc[] = {
	{ "963_223.j17", 0x20000, 0x0d34a5ff, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963_224.k17", 0x20000, 0x2fd453f2, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963_221.j15", 0x10000, 0xfa8e25fd, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963_222.k15", 0x10000, 0xca437a4f, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmntj);
STD_ROM_FN(Tmntj);

// =================================================
// Teenage Mutant Hero Turtles (UK 2 Players, set 1)
// =================================================
static struct BurnRomInfo Tmht2pRomDesc[] = {
	{ "963-u23.j17", 0x20000, 0x58bec748, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-u24.k17", 0x20000, 0xdce87c8d, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-u21.j15", 0x10000, 0xabce5ead, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-u22.k15", 0x10000, 0x4ecc8d6b, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmht2p);
STD_ROM_FN(Tmht2p);

// =================================================
// Teenage Mutant Hero Turtles (UK 2 Players, set 2)
// =================================================
static struct BurnRomInfo Tmht2paRomDesc[] = {
	{ "963-_23.j17", 0x20000, 0x8698061a, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-_24.k17", 0x20000, 0x4036c075, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-_21.j15", 0x10000, 0xddcc979c, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-_22.k15", 0x10000, 0x71a38d27, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmht2pa);
STD_ROM_FN(Tmht2pa);

// ==============================================
// Teenage Mutant Ninja Turtles (Japan 2 Players)
// ==============================================
static struct BurnRomInfo Tmnt2pjRomDesc[] = {
	{ "963-123.j17", 0x20000, 0x6a3527c9, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "963-124.k17", 0x20000, 0x2c4bfa15, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "963-121.j15", 0x10000, 0x4181b733, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "963-122.k15", 0x10000, 0xc64eb5ff, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmnt2pj);
STD_ROM_FN(Tmnt2pj);

// ================================================
// Teenage Mutant Ninja Turtles (Oceania 2 Players)
// ================================================
static struct BurnRomInfo Tmnt2poRomDesc[] = {
	{ "tmnt123.j17", 0x20000, 0x2d905183, BRF_ESS | BRF_PRG }, //  0 68000 code (even)
	{ "tmnt124.k17", 0x20000, 0xe0125352, BRF_ESS | BRF_PRG }, //  1            (odd)
	{ "tmnt21.j15",  0x10000, 0x12deeafb, BRF_ESS | BRF_PRG }, //  2            (even)
	{ "tmnt22.k15",  0x10000, 0xaec4f1c3, BRF_ESS | BRF_PRG }, //  3            (odd)

	{ "963a28.h27",  0x80000, 0xdb4769a8, BRF_GRA }, //  4 tile bitmaps
	{ "963a29.k27",  0x80000, 0x8069cd2e, BRF_GRA }, //  5

	{ "963a16.k6",   0x80000, 0xd4bd9984, BRF_GRA }, //  6 sprites
	{ "963a18.h6",   0x80000, 0xdd51adef, BRF_GRA }, //  7
	{ "963a15.k4",   0x80000, 0x1f324eed, BRF_GRA }, //  8
	{ "963a17.h4",   0x80000, 0xb5239a44, BRF_GRA }, //  9

	{ "963e20.g13",  0x08000, 0x1692a6d6, BRF_SND | BRF_PRG }, //  10 Z80 rom

	{ "963a26.c13",  0x20000, 0xe2ac3063, BRF_SND }, //  11 samples (K007232)
	{ "963a27.d18",  0x20000, 0x2dfd674b, BRF_SND }, //  12 samples (UPD7759C)
	{ "963a25.d5",   0x80000, 0xfca078c7, BRF_SND }, //  13 title music sample

	{ "963a30.g7",   0x0100,  0xabd82680, BRF_GRA }, //  14 sprite address decoder
	{ "963a31.g19",  0x0100,  0xf8004a1c, BRF_GRA }, //  15 priority encoder (not used)	
};

STD_ROM_PICK(Tmnt2po);
STD_ROM_FN(Tmnt2po);


struct BurnDriverD BurnDrvTmnt={
	"tmnt",NULL,NULL,"1989",
	"Teenage Mutant Ninja Turtles (World 4 Players)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING,4,HARDWARE_MISC_PRE90S,
	NULL,TmntRomInfo,TmntRomName,TmntInputInfo, TmntDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmntu={
	"tmntu","tmnt",NULL,"1989",
	"Teenage Mutant Ninja Turtles (US 4 Players, set 1)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,4,HARDWARE_MISC_PRE90S,
	NULL,TmntuRomInfo,TmntuRomName,TmntInputInfo, TmntDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmntua={
	"tmntua","tmnt",NULL,"1989",
	"Teenage Mutant Ninja Turtles (US 4 Players, set 2)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,4,HARDWARE_MISC_PRE90S,
	NULL,TmntuaRomInfo,TmntuaRomName,TmntInputInfo, TmntDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmht={
	"tmht","tmnt",NULL,"1989",
	"Teenage Mutant Hero Turtles (UK 4 Players)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,4,HARDWARE_MISC_PRE90S,
	NULL,TmhtRomInfo,TmhtRomName,TmntInputInfo, TmntDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmntj={
	"tmntj","tmnt",NULL,"1990",
	"Teenage Mutant Ninja Turtles (Japan 4 Players) \0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,4,HARDWARE_MISC_PRE90S,
	NULL,TmntjRomInfo,TmntjRomName,TmntInputInfo, TmntDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmht2p={
	"tmht2p","tmnt",NULL,"1989",
	"Teenage Mutant Hero Turtles (UK 2 Players, set 1)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_MISC_PRE90S,
	NULL,Tmht2pRomInfo,Tmht2pRomName,Tmnt2pInputInfo, Tmnt2pDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmht2pa={
	"tmht2pa","tmnt",NULL,"1989",
	"Teenage Mutant Hero Turtles (UK 2 Players, set 2)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_MISC_PRE90S,
	NULL,Tmht2paRomInfo,Tmht2paRomName,Tmnt2pInputInfo, Tmnt2pDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmnt2pj={
	"tmnt2pj","tmnt",NULL,"1990",
	"Teenage Mutant Ninja Turtles (Japan 2 Players)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_MISC_PRE90S,
	NULL,Tmnt2pjRomInfo,Tmnt2pjRomName,Tmnt2pInputInfo, Tmnt2pDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};

struct BurnDriverD BurnDrvTmnt2po={
	"tmnt2po","tmnt",NULL,"1989",
	"Teenage Mutant Ninja Turtles (Oceania 2 Players)\0","Preliminary driver","Konami","Konami 68000 based",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_MISC_PRE90S,
	NULL,Tmnt2poRomInfo,Tmnt2poRomName,Tmnt2pInputInfo, Tmnt2pDIPInfo,
	TmntInit,TmntExit,TmntFrame,NULL,TmntScan,
	0, NULL, NULL, NULL, &bTmntRecalcPal,304,224,4,3
};
