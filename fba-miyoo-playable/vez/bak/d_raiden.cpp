// Raiden                          (c) 1990 Seibu Kaihatsu
// driver by Oliver Bergmann, Bryan McPhail, Randy Mongenel

// port to FB Alpha by OopsWare


#include "burnint.h"
#include "burn_ym3812.h"
#include "msm6295.h"
#include "vez.h"

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *RomV30A;
static unsigned char *RomV30B;
static unsigned char *RomZ80;

static unsigned char *RomGfx1;
static unsigned char *RomGfx2;
static unsigned char *RomGfx3;
static unsigned char *RomGfx4;

static unsigned char *RamV30A;
static unsigned char *RamV30B;
static unsigned char *RamV30S;
static unsigned char *RamZ80;

static unsigned char *RamSpr;
static unsigned short *RamFg;
static unsigned short *RamBg;
static unsigned short *RamTxt;
static unsigned char *RamPal;
static unsigned char *RamScroll;

static unsigned short *RamCurPal;

static unsigned char bRecalcPalette = 0;
static unsigned char DrvReset = 0;

static unsigned char DrvButton[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInput[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static unsigned char main2sub[2] = {0, 0};
static unsigned char sub2main[2] = {0, 0};
static int main2sub_pending = 0;
static int sub2main_pending = 0;
static unsigned char z80_irq1, z80_irq2;
static int seibu_bank = 0;

#define	GAME_RAIDEN		0
#define	GAME_RAIDENA	1
#define	GAME_RAIDENK	2
#define	GAME_RAIDENT	3
static int game_drv;

inline static unsigned int CalcCol(int offs)
{
	// xxxxBBBBGGGGRRRR
	int nColour = RamPal[offs + 0] | (RamPal[offs + 1] << 8);
	int r, g, b;

	r = (nColour & 0x000F) << 4;	// Red
	r |= r >> 4;
	g = (nColour & 0x00F0) >> 0;	// Green
	g |= g >> 4;
	b = (nColour & 0x0F00) >> 4;	// Blue
	b |= b >> 4;

	return BurnHighCol(r, g, b, 0);
}

static struct BurnInputInfo raidenInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvButton + 0,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},

	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvButton + 1,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 3,	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvInput + 3,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvInput + 4,	"dip"},
};

STDINPUTINFO(raiden);


static struct BurnDIPInfo raidenDIPList[] = {

	// Defaults
	{0x11,	0xFF, 0xFF,	0x00, NULL},
	{0x12,	0xFF, 0xFF,	0x00, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Coin Mode"},
	{0x11,	0x01, 0x01,	0x00, "A"},
	{0x11,	0x01, 0x01,	0x01, "B"},
	{0,		0xFE, 0,	16,	  "Coinage"},
	{0x11,	0x01, 0x1E, 0x00, "1 coin 1 credit"},
	{0x11,	0x01, 0x1E, 0x02, "2 coins 1 credit"},
	{0x11,	0x01, 0x1E, 0x04, "3 coins 1 credit"},
	{0x11,	0x01, 0x1E, 0x06, "4 coins 1 credits"},
	{0x11,	0x01, 0x1E, 0x08, "5 coins 1 credit"},
	{0x11,	0x01, 0x1E, 0x0A, "6 coins 1 credit"},
	{0x11,	0x01, 0x1E, 0x0C, "1 coin 2 credits"},
	{0x11,	0x01, 0x1E, 0x0E, "1 coin 3 credits"},
	{0x11,	0x01, 0x1E, 0x10, "1 coin 4 credits"},
	{0x11,	0x01, 0x1E, 0x12, "1 coin 5 credits"},
	{0x11,	0x01, 0x1E, 0x14, "1 coin 6 credits"},
	{0x11,	0x01, 0x1E, 0x16, "2 coins 3 credits"},
	{0x11,	0x01, 0x1E, 0x18, "3 coins 2 credits"},
	{0x11,	0x01, 0x1E, 0x1A, "5 coins 3 credits"},
	{0x11,	0x01, 0x1E, 0x1C, "8 coins 3 credits"},
	{0x11,	0x01, 0x1E, 0x1E, "Free Play"},
	{0,		0xFE, 0,	2,	  "Credits to Start"},
	{0x11,	0x01, 0x20,	0x00, "1"},
	{0x11,	0x01, 0x20,	0x20, "2"},
	{0,		0xFE, 0,	2,	  "Flip Screen"},
	{0x11,	0x01, 0x80,	0x00, "Off"},
	{0x11,	0x01, 0x80,	0x80, "On"},

	// DIP 2
	{0,		0xFE, 0,	4,	  "Lives"},
	{0x12,	0x01, 0x03, 0x00, "3"},
	{0x12,	0x01, 0x03, 0x01, "1"},
	{0x12,	0x01, 0x03, 0x02, "2"},
	{0x12,	0x01, 0x03, 0x03, "5"},
	{0,		0xFE, 0,	4,	  "Bonus Life"},
	{0x12,	0x01, 0x0c, 0x00, "150000 400000"},
	{0x12,	0x01, 0x0c, 0x04, "80000 300000"},
	{0x12,	0x01, 0x0c, 0x08, "300000 1000000"},
	{0x12,	0x01, 0x0c, 0x0c, "1000000 5000000"},
	{0,		0xFE, 0,	4,	  "Difficulty"},
	{0x12,	0x01, 0x30, 0x00, "Normal"},
	{0x12,	0x01, 0x30, 0x10, "Easy"},
	{0x12,	0x01, 0x30, 0x20, "Hard"},
	{0x12,	0x01, 0x30, 0x30, "Very Hard"},
	{0,		0xFE, 0,	2,	  "Allow Continue"},
	{0x12,	0x01, 0x40, 0x00, "Yes"},
	{0x12,	0x01, 0x40, 0x40, "No"},
	{0,		0xFE, 0,	2,	  "Demo sound"},
	{0x12,	0x01, 0x80, 0x00, "On"},
	{0x12,	0x01, 0x80, 0x80, "Off"},
};

STDDIPINFO(raiden);

static struct BurnRomInfo raidenRomDesc[] = {
	{ "rai1.bin",		0x010000, 0xa4b12785, BRF_ESS | BRF_PRG },	// CPU 0, V30
	{ "rai2.bin",		0x010000, 0x17640bd5, BRF_ESS | BRF_PRG }, 
	{ "rai3.bin",		0x020000, 0x9d735bf5, BRF_ESS | BRF_PRG },
	{ "rai4.bin",		0x020000, 0x8d184b99, BRF_ESS | BRF_PRG }, 

	{ "rai5.bin",		0x020000, 0x7aca6d61, BRF_ESS | BRF_PRG },	// CPU 1, V30
	{ "rai6a.bin",		0x020000, 0xe3d35cc2, BRF_ESS | BRF_PRG }, 

	{ "rai6.bin",		0x010000, 0x723a483b, BRF_ESS | BRF_PRG },	// CPU 2, Z80
	
	{ "rai9.bin",		0x008000, 0x1922b25e, BRF_GRA }, 			// Tiles
	{ "rai10.bin",		0x008000, 0x5f90786a, BRF_GRA },
	{ "raiu0919.bin",	0x080000, 0xda151f0b, BRF_GRA },
	{ "raiu0920.bin",	0x080000, 0xac1f57ac, BRF_GRA },
	{ "raiu165.bin",	0x080000, 0x946d7bde, BRF_GRA },

	{ "rai7.bin",		0x010000, 0x8f927822, BRF_SND }, 			// Sound
	
	{ "ep910pc-1.bin",	0x000884, 0x00000000, BRF_NODUMP }, 
	{ "ep910pc-2.bin",	0x000884, 0x00000000, BRF_NODUMP }, 
};

STD_ROM_PICK(raiden);
STD_ROM_FN(raiden);

static struct BurnRomInfo raidenaRomDesc[] = {
	{ "rai1.bin",		0x010000, 0xa4b12785, BRF_ESS | BRF_PRG },	// CPU 0, V30
	{ "rai2.bin",		0x010000, 0x17640bd5, BRF_ESS | BRF_PRG }, 
	{ "raiden03.rom",	0x020000, 0xf6af09d0, BRF_ESS | BRF_PRG },
	{ "raiden04.rom",	0x020000, 0x6bdfd416, BRF_ESS | BRF_PRG }, 

	{ "raiden05.rom",	0x020000, 0xed03562e, BRF_ESS | BRF_PRG },	// CPU 1, V30
	{ "raiden06.rom",	0x020000, 0xa19d5b5d, BRF_ESS | BRF_PRG }, 

	{ "raiden08.rom",	0x010000, 0x731adb43, BRF_ESS | BRF_PRG },	// CPU 2, Z80
	
	{ "rai9.bin",		0x008000, 0x1922b25e, BRF_GRA }, 			// Tiles
	{ "rai10.bin",		0x008000, 0x5f90786a, BRF_GRA },
	{ "raiu0919.bin",	0x080000, 0xda151f0b, BRF_GRA },
	{ "raiu0920.bin",	0x080000, 0xac1f57ac, BRF_GRA },
	{ "raiu165.bin",	0x080000, 0x946d7bde, BRF_GRA },

	{ "rai7.bin",		0x010000, 0x8f927822, BRF_SND }, 			// Sound
};

STD_ROM_PICK(raidena);
STD_ROM_FN(raidena);

static struct BurnRomInfo raidenkRomDesc[] = {
	{ "rai1.bin",		0x010000, 0xa4b12785, BRF_ESS | BRF_PRG },	// CPU 0, V30
	{ "rai2.bin",		0x010000, 0x17640bd5, BRF_ESS | BRF_PRG }, 
	{ "raiden03.rom",	0x020000, 0xf6af09d0, BRF_ESS | BRF_PRG },
	{ "1i",				0x020000, 0xfddf24da, BRF_ESS | BRF_PRG }, 

	{ "raiden05.rom",	0x020000, 0xed03562e, BRF_ESS | BRF_PRG },	// CPU 1, V30
	{ "raiden06.rom",	0x020000, 0xa19d5b5d, BRF_ESS | BRF_PRG }, 

	{ "8b",				0x010000, 0x99ee7505, BRF_ESS | BRF_PRG },	// CPU 2, Z80
	
	{ "rai9.bin",		0x008000, 0x1922b25e, BRF_GRA }, 			// Tiles
	{ "rai10.bin",		0x008000, 0x5f90786a, BRF_GRA },
	{ "raiu0919.bin",	0x080000, 0xda151f0b, BRF_GRA },
	{ "raiu0920.bin",	0x080000, 0xac1f57ac, BRF_GRA },
	{ "raiu165.bin",	0x080000, 0x946d7bde, BRF_GRA },

	{ "rai7.bin",		0x010000, 0x8f927822, BRF_SND }, 			// Sound
};

STD_ROM_PICK(raidenk);
STD_ROM_FN(raidenk);

static struct BurnRomInfo raidentRomDesc[] = {
	{ "rai1.bin",		0x010000, 0xa4b12785, BRF_ESS | BRF_PRG },	// CPU 0, V30
	{ "rai2.bin",		0x010000, 0x17640bd5, BRF_ESS | BRF_PRG }, 
	{ "raiden03.rom",	0x020000, 0xf6af09d0, BRF_ESS | BRF_PRG },
	{ "raid04t.023",	0x020000, 0x61eefab1, BRF_ESS | BRF_PRG }, 

	{ "raiden05.rom",	0x020000, 0xed03562e, BRF_ESS | BRF_PRG },	// CPU 1, V30
	{ "raiden06.rom",	0x020000, 0xa19d5b5d, BRF_ESS | BRF_PRG }, 

	{ "raid08.212",		0x010000, 0xcbe055c7, BRF_ESS | BRF_PRG },	// CPU 2, Z80
	
	{ "rai9.bin",		0x008000, 0x1922b25e, BRF_GRA }, 			// Tiles
	{ "rai10.bin",		0x008000, 0x5f90786a, BRF_GRA },
	{ "raiu0919.bin",	0x080000, 0xda151f0b, BRF_GRA },
	{ "raiu0920.bin",	0x080000, 0xac1f57ac, BRF_GRA },
	{ "raiu165.bin",	0x080000, 0x946d7bde, BRF_GRA },

	{ "rai7.bin",		0x010000, 0x8f927822, BRF_SND }, 			// Sound
};

STD_ROM_PICK(raident);
STD_ROM_FN(raident);

static void update_irq_lines()
{
	if ((z80_irq1 & z80_irq2) == 0xff)
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	else
		ZetSetIRQLine(z80_irq1 & z80_irq2, ZET_IRQSTATUS_ACK);
}

static unsigned char seibu_main_v30_r(unsigned int offset)
{
	switch (offset) {
		case 0x4: return sub2main[0];
		case 0x5: return 0;
			
		case 0x6: return sub2main[1];
		case 0x7: return 0;
			
		case 0xa: return main2sub_pending ? 1 : 0;
		case 0xb: return 0;
		default : return 0xff;
	}
}

static void seibu_main_v30_w(unsigned int offset, unsigned char data)
{
	switch (offset) {
		case 0x0: main2sub[0] = data; break;
		case 0x2: main2sub[1] = data; break;
		case 0x8:  
			z80_irq2 = 0xdf;
			update_irq_lines();
			break;
		case 0xc:
			sub2main_pending = 0;
			main2sub_pending = 1;
			break;
	}
}

void raidenFMIRQHandler(int, int nStatus)
{
	z80_irq1 = (nStatus) ? 0xd7 : 0xff;
	update_irq_lines();
}

static void set_seibu_bank(int bank) 
{
	bank &= 1;
	if (seibu_bank != bank) {
		seibu_bank = bank;
		ZetMapArea(0x8000, 0xFFFF, 0, RomZ80 + 0x8000 * (1-seibu_bank) );
		ZetMapArea(0x8000, 0xFFFF, 2, RomZ80 + 0x8000 * (1-seibu_bank) );
	}
}

int raidenSynchroniseStream(int nSoundRate)
{
	return (long long)ZetTotalCycles() * nSoundRate / 3579545;
}

unsigned char __fastcall raidenReadByte(unsigned int vezAddress)
{
	switch (vezAddress) {
		case 0x0b000: return ~DrvInput[1];
		case 0x0b001: return ~DrvInput[2];
		case 0x0b002: return ~DrvInput[3];
		case 0x0b003: return ~DrvInput[4];
		
		case 0x0d000:
		case 0x0d001:
		case 0x0d002:
		case 0x0d003:
		case 0x0d004:
		case 0x0d005:
		case 0x0d006:
		case 0x0d007:
		case 0x0d008:
		case 0x0d009:
		case 0x0d00a:
		case 0x0d00b:
		case 0x0d00c:
		case 0x0d00d: return seibu_main_v30_r(vezAddress - 0x0d000);
	
		//default:
		//	bprintf(PRINT_NORMAL, _T("CPU #0 Attempt to read byte value of location %x\n"), vezAddress);
	}
	return 0;
}

void __fastcall raidenWriteByte(unsigned int vezAddress, unsigned char byteValue)
{
	
	switch (vezAddress) {
		
		case 0x0b000:
		case 0x0b001:
		case 0x0b002:
		case 0x0b003:
		case 0x0b004:
		case 0x0b005:
		case 0x0b006:
		case 0x0b007:
			// raiden_control_w
			// flip_screen not support
			break;

		case 0x0d060:
		case 0x0d061:
		case 0x0d062:
		case 0x0d063:
		case 0x0d064:
		case 0x0d065:
		case 0x0d066:
		case 0x0d067:
			RamScroll[ vezAddress - 0x0d060 ] = byteValue;
			break;
		
		case 0x0d000:
		case 0x0d001:
		case 0x0d002:
		case 0x0d003:
		case 0x0d004:
		case 0x0d005:
		case 0x0d006:
		case 0x0d007:
		case 0x0d008:
		case 0x0d009:
		case 0x0d00a:
		case 0x0d00b:
		case 0x0d00c:
		case 0x0d00d:
			seibu_main_v30_w(vezAddress - 0x0d000, byteValue);
			break;		
		//default:
		//	bprintf(PRINT_NORMAL, _T("CPU #0 Attempt to write byte value %x to location %x\n"), byteValue, vezAddress);
	}
}

unsigned char __fastcall raidenAltReadByte(unsigned int vezAddress)
{
	switch (vezAddress) {
		case 0x0a000:
		case 0x0a001:
		case 0x0a002:
		case 0x0a003:
		case 0x0a004:
		case 0x0a005:
		case 0x0a006:
		case 0x0a007:
		case 0x0a008:
		case 0x0a009:
		case 0x0a00a:
		case 0x0a00b:
		case 0x0a00c:
		case 0x0a00d: return seibu_main_v30_r(vezAddress - 0x0a000);
	
		case 0x0e000: return ~DrvInput[1];
		case 0x0e001: return ~DrvInput[2];
		case 0x0e002: return ~DrvInput[3];
		case 0x0e003: return ~DrvInput[4];
		
		//default:
		//	bprintf(PRINT_NORMAL, _T("CPU #0 Attempt to read byte value of location %x\n"), vezAddress);
	}
	return 0;
}

void __fastcall raidenAltWriteByte(unsigned int vezAddress, unsigned char byteValue)
{
	
	switch (vezAddress) {
		
		case 0x0b000:
		case 0x0b001:
		case 0x0b002:
		case 0x0b003:
		case 0x0b004:
		case 0x0b005:
		case 0x0b006:
		case 0x0b007:
			// raiden_control_w
			// flip_screen not support
			break;

		case 0x0f002: RamScroll[0] = byteValue; break;
		case 0x0f004: RamScroll[1] = byteValue;	break;
		case 0x0f012: RamScroll[2] = byteValue; break;
		case 0x0f014: RamScroll[3] = byteValue;	break;
		case 0x0f022: RamScroll[4] = byteValue; break;
		case 0x0f024: RamScroll[5] = byteValue;	break;
		case 0x0f032: RamScroll[6] = byteValue; break;
		case 0x0f034: RamScroll[7] = byteValue;	break;
		
		case 0x0a000:
		case 0x0a001:
		case 0x0a002:
		case 0x0a003:
		case 0x0a004:
		case 0x0a005:
		case 0x0a006:
		case 0x0a007:
		case 0x0a008:
		case 0x0a009:
		case 0x0a00a:
		case 0x0a00b:
		case 0x0a00c:
		case 0x0a00d:
			seibu_main_v30_w(vezAddress - 0x0a000, byteValue);
			break;		
		//default:
		//	bprintf(PRINT_NORMAL, _T("CPU #0 Attempt to write byte value %x to location %x\n"), byteValue, vezAddress);
	}
}

unsigned char __fastcall raidenSubReadByte(unsigned int vezAddress)
{
	switch (vezAddress) {
		case 0x04008: {
			unsigned char nRet = RamV30S[0x8];
			//int pc = VezPC();
			//if (pc==0xfcde6 && ret!=0x40) cpu_spin();
			bprintf(PRINT_NORMAL, _T("sub_cpu_spin_r %02x%02x\n"), RamV30S[0x8], RamV30S[0x9]);
			return nRet; }
		case 0x04009:
			bprintf(PRINT_NORMAL, _T("sub_cpu_spin_r %02x%02x\n"), RamV30S[0x8], RamV30S[0x9]);
			return RamV30S[0x9];
			
		//default:
		//	bprintf(PRINT_NORMAL, _T("CPU #1 Attempt to read byte value of location %x\n"), vezAddress);
	}
	return 0;
}

void __fastcall raidenSubWriteByte(unsigned int vezAddress, unsigned char byteValue)
{
	if ((vezAddress & 0xFF000) == 0x03000 ) {
		vezAddress -= 0x03000;
		RamPal[ vezAddress ] = byteValue;
		if (vezAddress & 1)
			RamCurPal[ vezAddress >> 1 ] = CalcCol( vezAddress - 1 );
		return;
	}

//	switch (vezAddress) {
//		default:
//			bprintf(PRINT_NORMAL, _T("CPU #1 Attempt to write byte value %x to location %x\n"), byteValue, vezAddress);
//	}

}

unsigned char __fastcall raidenZ80Read(unsigned short a)
{
	switch (a) {
		case 0x4008: return BurnYM3812Read(0);
		case 0x4010: return main2sub[0];
		case 0x4011: return main2sub[1];
		case 0x4012: return sub2main_pending ? 1 : 0;
		case 0x4013: return DrvInput[0];
		case 0x6000: return MSM6295ReadStatus(0);
//		default:
//			bprintf(PRINT_NORMAL, _T("Z80 Attempt to read word value of location %04x\n"), a);
	}
	return 0;
}


void __fastcall raidenZ80Write(unsigned short a,unsigned char v)
{
	switch (a) {
		case 0x4000:
			// just a guess
			main2sub_pending = 0;
			sub2main_pending = 1;
			break;
		case 0x4001:
			// seibu_irq_clear_w
			z80_irq1 = 0xff;
			z80_irq2 = 0xff;
			update_irq_lines();
			break;
		case 0x4002:
			// seibu_rst10_ack_w
			break;
		case 0x4003:
			// seibu_rst18_ack_w
			z80_irq2 = 0xff;
			update_irq_lines();
			break;
		case 0x4007:
			// seibu_bank_w
			set_seibu_bank(v);
			break;
		case 0x4008:
			// YM3812_control_port_0_w
			BurnYM3812Write(0, v);
			break;
		case 0x4009:
			// YM3812_write_port_0_w
			BurnYM3812Write(1, v);
			break;
		case 0x4018:
			sub2main[0] = v;
			break;
		case 0x4019:
			sub2main[1] = v;
			break;
		case 0x401b:
			// seibu_coin_w
			//bprintf(PRINT_NORMAL, _T("seibu_coin_w(%02x)\n"), v);
			break;
		case 0x6000:
			// OKIM6295_data_0_w
			MSM6295Command(0, v);
			break;
		//default:
		//	bprintf(PRINT_NORMAL, _T("Z80 Attempt to write word value %x to location %x\n"), v, a);
	}
}

static int DrvDoReset()
{
	
	VezOpen(0);
    //littlehuimodify
    VezReset(0x7fff);
    //VezReset();
	VezClose();
	
	VezOpen(1);
	//VezReset();
	//littlehuimodify
    VezReset(0x1fff);
    VezClose();
	
	seibu_bank = -1;
	set_seibu_bank(0);
	
	ZetReset();
	z80_irq1 = 0xff;
	z80_irq2 = 0xff;
	ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	
	MSM6295Reset(0);
	BurnYM3812Reset();
	
	return 0;
}

static int MemIndex()
{
	unsigned char *Next; Next = Mem;
	RomV30A		= Next; Next += 0x060000;			// V30 main cpu
	RomV30B		= Next; Next += 0x040000;			// V30 sub cpu
	RomZ80		= Next; Next += 0x020000;			// Z80 sound cpu
	
	RomGfx1		= Next; Next += 0x020000;			// char
	RomGfx2		= Next; Next += 0x100000;			// tile 1
	RomGfx3		= Next; Next += 0x100000;			// tile 2
	RomGfx4		= Next; Next += 0x100000;			// spr

	MSM6295ROM	= Next; Next += 0x010000;
	
	RamStart	= Next;
	
	RamV30A		= Next; Next += 0x007000;
	RamV30B		= Next; Next += 0x002000;
	RamV30S		= Next; Next += 0x001000;
	RamZ80		= Next; Next += 0x000800;
	
	RamSpr		= Next; Next += 0x001000;
	RamFg		= (unsigned short *)Next; Next += 0x000800;
	RamBg		= (unsigned short *)Next; Next += 0x000800;
	RamTxt		= (unsigned short *)Next; Next += 0x000800;

	RamPal		= Next; Next += 0x001000;
	RamScroll	= Next; Next += 0x000008;

	RamEnd		= Next;

	RamCurPal	= (unsigned short *) Next; Next += 0x001000;
	
	MemEnd		= Next;
	return 0;
}

void decode_gfx_1(unsigned char * dst, unsigned char * src)
{
	for(int i=0;i<0x10000/32;i++) {
		for (int j=0;j<8;j++) {
			
			dst[0] = (((src[0] >> 7) & 1) << 2) | (((src[0] >> 3) & 1) << 3) | (((src[0x8000] >> 7) & 1) << 0) | (((src[0x8000] >> 3) & 1) << 1);
			dst[1] = (((src[0] >> 6) & 1) << 2) | (((src[0] >> 2) & 1) << 3) | (((src[0x8000] >> 6) & 1) << 0) | (((src[0x8000] >> 2) & 1) << 1);
			dst[2] = (((src[0] >> 5) & 1) << 2) | (((src[0] >> 1) & 1) << 3) | (((src[0x8000] >> 5) & 1) << 0) | (((src[0x8000] >> 1) & 1) << 1);
			dst[3] = (((src[0] >> 4) & 1) << 2) | (((src[0] >> 0) & 1) << 3) | (((src[0x8000] >> 4) & 1) << 0) | (((src[0x8000] >> 0) & 1) << 1);
			dst[4] = (((src[1] >> 7) & 1) << 2) | (((src[1] >> 3) & 1) << 3) | (((src[0x8001] >> 7) & 1) << 0) | (((src[0x8001] >> 3) & 1) << 1);
			dst[5] = (((src[1] >> 6) & 1) << 2) | (((src[1] >> 2) & 1) << 3) | (((src[0x8001] >> 6) & 1) << 0) | (((src[0x8001] >> 2) & 1) << 1);
			dst[6] = (((src[1] >> 5) & 1) << 2) | (((src[1] >> 1) & 1) << 3) | (((src[0x8001] >> 5) & 1) << 0) | (((src[0x8001] >> 1) & 1) << 1);
			dst[7] = (((src[1] >> 4) & 1) << 2) | (((src[1] >> 0) & 1) << 3) | (((src[0x8001] >> 4) & 1) << 0) | (((src[0x8001] >> 0) & 1) << 1);
			
			src += 2;
			dst += 8;
		}
	}
}

void decode_gfx_2(unsigned char * dst, unsigned char * src)
{
	for(int i=0;i<0x80000/128;i++) {
		for (int j=0;j<16;j++) {
			
			dst[ 0] = (((src[ 1] >> 7) & 1) << 2) | (((src[ 1] >> 3) & 1) << 3) | (((src[ 0] >> 7) & 1) << 0) | (((src[ 0] >> 3) & 1) << 1);
			dst[ 1] = (((src[ 1] >> 6) & 1) << 2) | (((src[ 1] >> 2) & 1) << 3) | (((src[ 0] >> 6) & 1) << 0) | (((src[ 0] >> 2) & 1) << 1);
			dst[ 2] = (((src[ 1] >> 5) & 1) << 2) | (((src[ 1] >> 1) & 1) << 3) | (((src[ 0] >> 5) & 1) << 0) | (((src[ 0] >> 1) & 1) << 1);
			dst[ 3] = (((src[ 1] >> 4) & 1) << 2) | (((src[ 1] >> 0) & 1) << 3) | (((src[ 0] >> 4) & 1) << 0) | (((src[ 0] >> 0) & 1) << 1);
			dst[ 4] = (((src[ 3] >> 7) & 1) << 2) | (((src[ 3] >> 3) & 1) << 3) | (((src[ 2] >> 7) & 1) << 0) | (((src[ 2] >> 3) & 1) << 1);
			dst[ 5] = (((src[ 3] >> 6) & 1) << 2) | (((src[ 3] >> 2) & 1) << 3) | (((src[ 2] >> 6) & 1) << 0) | (((src[ 2] >> 2) & 1) << 1);
			dst[ 6] = (((src[ 3] >> 5) & 1) << 2) | (((src[ 3] >> 1) & 1) << 3) | (((src[ 2] >> 5) & 1) << 0) | (((src[ 2] >> 1) & 1) << 1);
			dst[ 7] = (((src[ 3] >> 4) & 1) << 2) | (((src[ 3] >> 0) & 1) << 3) | (((src[ 2] >> 4) & 1) << 0) | (((src[ 2] >> 0) & 1) << 1);
                                                                                                                                          
			dst[ 8] = (((src[65] >> 7) & 1) << 2) | (((src[65] >> 3) & 1) << 3) | (((src[64] >> 7) & 1) << 0) | (((src[64] >> 3) & 1) << 1);
			dst[ 9] = (((src[65] >> 6) & 1) << 2) | (((src[65] >> 2) & 1) << 3) | (((src[64] >> 6) & 1) << 0) | (((src[64] >> 2) & 1) << 1);
			dst[10] = (((src[65] >> 5) & 1) << 2) | (((src[65] >> 1) & 1) << 3) | (((src[64] >> 5) & 1) << 0) | (((src[64] >> 1) & 1) << 1);
			dst[11] = (((src[65] >> 4) & 1) << 2) | (((src[65] >> 0) & 1) << 3) | (((src[64] >> 4) & 1) << 0) | (((src[64] >> 0) & 1) << 1);
			dst[12] = (((src[67] >> 7) & 1) << 2) | (((src[67] >> 3) & 1) << 3) | (((src[66] >> 7) & 1) << 0) | (((src[66] >> 3) & 1) << 1);
			dst[13] = (((src[67] >> 6) & 1) << 2) | (((src[67] >> 2) & 1) << 3) | (((src[66] >> 6) & 1) << 0) | (((src[66] >> 2) & 1) << 1);
			dst[14] = (((src[67] >> 5) & 1) << 2) | (((src[67] >> 1) & 1) << 3) | (((src[66] >> 5) & 1) << 0) | (((src[66] >> 1) & 1) << 1);
			dst[15] = (((src[67] >> 4) & 1) << 2) | (((src[67] >> 0) & 1) << 3) | (((src[66] >> 4) & 1) << 0) | (((src[66] >> 0) & 1) << 1);

			
			src += 4;
			dst += 16;
		}
		src += 64;
	}
}

static void common_decrypt()
{
	unsigned char *RAM = RomV30A;
	int i;
	unsigned char a;

	static const unsigned char xor_table[4][16]={
	  {0xF1,0xF9,0xF5,0xFD,0xF1,0xF1,0x3D,0x3D,0x73,0xFB,0x77,0xFF,0x73,0xF3,0x3F,0x3F},   // rom 3
	  {0xDF,0xFF,0xFF,0xFF,0xDB,0xFF,0xFB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFB,0xFF,0xFB,0xFF},   // rom 4
	  {0x7F,0x7F,0xBB,0x77,0x77,0x77,0xBE,0xF6,0x7F,0x7F,0xBB,0x77,0x77,0x77,0xBE,0xF6},   // rom 5
	  {0xFF,0xFF,0xFD,0xFD,0xFD,0xFD,0xEF,0xEF,0xFF,0xFF,0xFD,0xFD,0xFD,0xFD,0xEF,0xEF}	   // rom 6
	};

	// Rom 3 - main cpu even bytes
	for (i=0x20000; i<0x60000; i+=2) {
		a=RAM[i];
		a^=xor_table[0][(i/2) & 0x0f];
    	a^=0xff;
   		a=(a & 0x31) | ((a<<1) & 0x04) | ((a>>5) & 0x02) | ((a<<4) & 0x40) | ((a<<4) & 0x80) | ((a>>4) & 0x08);
		RAM[i]=a;
	}

	// Rom 4 - main cpu odd bytes
	for (i=0x20001; i<0x60000; i+=2) {
		a=RAM[i];
		a^=xor_table[1][(i/2) & 0x0f];
    	a^=0xff;
   		a=(a & 0xdb) | ((a>>3) & 0x04) | ((a<<3) & 0x20);
		RAM[i]=a;
	}

	RAM = RomV30B;

	// Rom 5 - sub cpu even bytes
	for (i=0x00000; i<0x40000; i+=2) {
		a=RAM[i];
		a^=xor_table[2][(i/2) & 0x0f];
    	a^=0xff;
   		a=(a & 0x32) | ((a>>1) & 0x04) | ((a>>4) & 0x08) | ((a<<5) & 0x80) | ((a>>6) & 0x01) | ((a<<6) & 0x40);
		RAM[i]=a;
	}

	// Rom 6 - sub cpu odd bytes
	for (i=0x00001; i<0x40000; i+=2) {
		a=RAM[i];
		a^=xor_table[3][(i/2) & 0x0f];
    	a^=0xff;
   		a=(a & 0xed) | ((a>>3) & 0x02) | ((a<<3) & 0x10);
		RAM[i]=a;
	}
}

#define BIT(x,n) (((x)>>(n))&1)

#define BITSWAP8(val,B7,B6,B5,B4,B3,B2,B1,B0) \
		((BIT(val,B7) << 7) | \
		 (BIT(val,B6) << 6) | \
		 (BIT(val,B5) << 5) | \
		 (BIT(val,B4) << 4) | \
		 (BIT(val,B3) << 3) | \
		 (BIT(val,B2) << 2) | \
		 (BIT(val,B1) << 1) | \
		 (BIT(val,B0) << 0))

static unsigned char decrypt_data(int a,unsigned char src)
{
	if ( BIT(a,9)  &  BIT(a,8))             src ^= 0x80;
	if ( BIT(a,11) &  BIT(a,4) &  BIT(a,1)) src ^= 0x40;
	if ( BIT(a,11) & ~BIT(a,8) &  BIT(a,1)) src ^= 0x04;
	if ( BIT(a,13) & ~BIT(a,6) &  BIT(a,4)) src ^= 0x02;
	if (~BIT(a,11) &  BIT(a,9) &  BIT(a,2)) src ^= 0x01;

	if (BIT(a,13) &  BIT(a,4)) src = BITSWAP8(src,7,6,5,4,3,2,0,1);
	if (BIT(a, 8) &  BIT(a,4)) src = BITSWAP8(src,7,6,5,4,2,3,1,0);

	return src;
}

static unsigned char decrypt_opcode(int a,unsigned char src)
{
	if ( BIT(a,9)  &  BIT(a,8))             src ^= 0x80;
	if ( BIT(a,11) &  BIT(a,4) &  BIT(a,1)) src ^= 0x40;
	if (~BIT(a,13) & BIT(a,12))             src ^= 0x20;
	if (~BIT(a,6)  &  BIT(a,1))             src ^= 0x10;
	if (~BIT(a,12) &  BIT(a,2))             src ^= 0x08;
	if ( BIT(a,11) & ~BIT(a,8) &  BIT(a,1)) src ^= 0x04;
	if ( BIT(a,13) & ~BIT(a,6) &  BIT(a,4)) src ^= 0x02;
	if (~BIT(a,11) &  BIT(a,9) &  BIT(a,2)) src ^= 0x01;

	if (BIT(a,13) &  BIT(a,4)) src = BITSWAP8(src,7,6,5,4,3,2,0,1);
	if (BIT(a, 8) &  BIT(a,4)) src = BITSWAP8(src,7,6,5,4,2,3,1,0);
	if (BIT(a,12) &  BIT(a,9)) src = BITSWAP8(src,7,6,4,5,3,2,1,0);
	if (BIT(a,11) & ~BIT(a,6)) src = BITSWAP8(src,6,7,5,4,3,2,1,0);

	return src;
}

void seibu_sound_decrypt()
{
	for (int i=0; i<0x10000;i++) {
		unsigned char src = RomZ80[i];
		RomZ80[i+0x00000] = decrypt_data(i,src);
		RomZ80[i+0x10000] = decrypt_opcode(i,src);
	}
}

static int DrvInit()
{
	int nRet;

	if ( strcmp(BurnDrvGetTextA(DRV_NAME), "raiden") == 0 )
		game_drv = GAME_RAIDEN;
	else
	if ( strcmp(BurnDrvGetTextA(DRV_NAME), "raidena") == 0 )
		game_drv = GAME_RAIDENA;
	else
	if ( strcmp(BurnDrvGetTextA(DRV_NAME), "raidenk") == 0 )
		game_drv = GAME_RAIDENK;
	else
	if ( strcmp(BurnDrvGetTextA(DRV_NAME), "raident") == 0 )
		game_drv = GAME_RAIDENT;
	else
		return 1;
	
	Mem = NULL;
	MemIndex();
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex();	
	
	nRet = BurnLoadRom(RomV30A + 0x000000, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(RomV30A + 0x000001, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(RomV30A + 0x020000, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(RomV30A + 0x020001, 3, 2); if (nRet != 0) return 1;

	nRet = BurnLoadRom(RomV30B + 0x000000, 4, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(RomV30B + 0x000001, 5, 2); if (nRet != 0) return 1;

	if (game_drv != GAME_RAIDEN)
		common_decrypt();

	nRet = BurnLoadRom(RomZ80  + 0x000000, 6, 1); if (nRet != 0) return 1;

	if (game_drv == GAME_RAIDENA || game_drv == GAME_RAIDENT) 
		seibu_sound_decrypt();

	unsigned char * tmp = (unsigned char *) malloc (0x80000);
	if (tmp == 0) return 1;
	
	BurnLoadRom(tmp + 0x00000,  7, 1);
	BurnLoadRom(tmp + 0x08000,  8, 1);
	decode_gfx_1(RomGfx1, tmp);

	BurnLoadRom(tmp + 0x00000,  9, 1);
	decode_gfx_2(RomGfx2, tmp);
	BurnLoadRom(tmp + 0x00000, 10, 1);
	decode_gfx_2(RomGfx3, tmp);
	BurnLoadRom(tmp + 0x00000, 11, 1);
	decode_gfx_2(RomGfx4, tmp);
	
	free(tmp);
	
	BurnLoadRom(MSM6295ROM, 12, 1);
	
	{
		unsigned int cpu_types[] = { 8, 8 };
		VezInit(2, &cpu_types[0]);
	    
	    // main-cpu
	    VezOpen(0);
		
		VezMapArea(0x00000, 0x06fff, 0, RamV30A);			// RAM
		VezMapArea(0x00000, 0x06fff, 1, RamV30A);

		VezMapArea(0x07000, 0x07fff, 0, RamSpr);			// Sprites
		VezMapArea(0x07000, 0x07fff, 1, RamSpr);

		VezMapArea(0x0c000, 0x0c7ff, 1, (unsigned char *)RamTxt);
		
		VezMapArea(0xA0000, 0xFFFFF, 0, RomV30A);			// CPU 0 ROM
		VezMapArea(0xA0000, 0xFFFFF, 2, RomV30A);			// CPU 0 ROM
		
		if (game_drv == GAME_RAIDEN) {
			VezMapArea(0x0a000, 0x0afff, 0, RamV30S);			// Share RAM
			VezMapArea(0x0a000, 0x0afff, 1, RamV30S);
			VezSetReadHandler(raidenReadByte);
			VezSetWriteHandler(raidenWriteByte);
		} else {
			VezMapArea(0x08000, 0x08fff, 0, RamV30S);			// Share RAM
			VezMapArea(0x08000, 0x08fff, 1, RamV30S);
			VezSetReadHandler(raidenAltReadByte);
			VezSetWriteHandler(raidenAltWriteByte);
		}

		VezClose();

	    // sub-cpu
	    VezOpen(1);
		
		VezMapArea(0x00000, 0x01fff, 0, RamV30B);			// RAM
		VezMapArea(0x00000, 0x01fff, 1, RamV30B);

		VezMapArea(0x02000, 0x027ff, 0, (unsigned char *)RamBg);				// Background
		VezMapArea(0x02000, 0x027ff, 1, (unsigned char *)RamBg);

		VezMapArea(0x02800, 0x02fff, 0, (unsigned char *)RamFg);				// Foreground
		VezMapArea(0x02800, 0x02fff, 1, (unsigned char *)RamFg);

		VezMapArea(0x03000, 0x03fff, 0, RamPal);			// Palette
		//VezMapArea(0x03000, 0x03fff, 1, RamPal);
		
		VezMapArea(0x04000, 0x04fff, 0, RamV30S);			// Share RAM
		VezMapArea(0x04000, 0x04fff, 1, RamV30S);
		
		VezMapArea(0xC0000, 0xFFFFF, 0, RomV30B);			// CPU 1 ROM
		VezMapArea(0xC0000, 0xFFFFF, 2, RomV30B);			// CPU 1 ROM
		
		VezSetReadHandler(raidenSubReadByte);
		VezSetWriteHandler(raidenSubWriteByte);
		
		VezClose();
	}
	
	{
		ZetInit(1);
		ZetOpen(0);
		
		ZetMapArea(0x0000, 0x1FFF, 0, RomZ80);
		
		if (game_drv == GAME_RAIDENA || game_drv == GAME_RAIDENT)
			ZetMapArea(0x0000, 0x1FFF, 2, RomZ80 + 0x10000, RomZ80);
		else
			ZetMapArea(0x0000, 0x1FFF, 2, RomZ80);
		
		ZetMapArea(0x2000, 0x27FF, 0, RamZ80);
		ZetMapArea(0x2000, 0x27FF, 1, RamZ80);
		ZetMapArea(0x2000, 0x27FF, 2, RamZ80);

		//ZetMapArea(0x8000, 0xFFFF, 0, RomZ80 + 0x8000);
		//ZetMapArea(0x8000, 0xFFFF, 2, RomZ80 + 0x8000);
		//use set_seibu_bank(0); in DrvDoReset();
		
		ZetMemEnd();
		
		ZetSetReadHandler(raidenZ80Read);
		ZetSetWriteHandler(raidenZ80Write);
		
	}
	
	BurnYM3812Init(3579545, &raidenFMIRQHandler, &raidenSynchroniseStream, 0);
	BurnTimerAttachZet(3579545);
	
	MSM6295Init(0, 8000, 80.0, 1); //	1320000
	DrvDoReset();
	return 0;
}

static int DrvExit()
{
	MSM6295Exit(0);
	BurnYM3812Exit();
	
	VezExit();
	ZetExit();
	
	free(Mem);
	Mem = NULL;
	
	return 0;
}

static void drawBackground()
{
	/* cccc tttt tttt tttt */
	unsigned short * pal = RamCurPal + 0;
	int offs, mx, my, x, y, sx, sy;
	
	if (game_drv == GAME_RAIDEN) {
		sx = (RamScroll[0] | (RamScroll[1] << 8)) & 0x1ff;
		sy = (RamScroll[2] | (RamScroll[3] << 8)) & 0x1ff;
	} else {
		sx = ((RamScroll[2]&0x10)<<4) | ((RamScroll[3]&0x7f) << 1) | ((RamScroll[3]&0x80) >> 7);
		sy = ((RamScroll[0]&0x10)<<4) | ((RamScroll[1]&0x7f) << 1) | ((RamScroll[1]&0x80) >> 7);
	}
	
	mx = 0;
	my = -1;
	for (offs = 0; offs < 32*32; offs++) {
		my++;
		if (my == 32) {
			my = 0;
			mx++;
		}
		
		x = mx * 16 -  0 - sx;
		if (x < -16) x += 512;
		
		y = my * 16 - 16 - sy;
		if (y < -16) y += 512;
		
		if (x<=-16 || x>=256 || y<=-16 || y>=224) continue;
		else {
			unsigned int tileno = RamBg[offs] & 0x0FFF;
			//if (tileno == 0) continue;
 			unsigned int c = (RamBg[offs] & 0xF000) >> 8;
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 256 + x;
			unsigned char *d = RomGfx2 + (tileno << 8);
			
			if (x<0 || x>(256-16) || y<0 || y>(224-16))
				for (int k=0;k<16;k++) {
					if ((y+k)>=0 && (y+k)<224) {
		 				if ((x+ 0)>=0 && (x+ 0)<256) p[ 0] = pal[d[ 0]|c];
		 				if ((x+ 1)>=0 && (x+ 1)<256) p[ 1] = pal[d[ 1]|c];
		 				if ((x+ 2)>=0 && (x+ 2)<256) p[ 2] = pal[d[ 2]|c];
		 				if ((x+ 3)>=0 && (x+ 3)<256) p[ 3] = pal[d[ 3]|c];
		 				if ((x+ 4)>=0 && (x+ 4)<256) p[ 4] = pal[d[ 4]|c];
		 				if ((x+ 5)>=0 && (x+ 5)<256) p[ 5] = pal[d[ 5]|c];
		 				if ((x+ 6)>=0 && (x+ 6)<256) p[ 6] = pal[d[ 6]|c];
		 				if ((x+ 7)>=0 && (x+ 7)<256) p[ 7] = pal[d[ 7]|c];
		 				if ((x+ 8)>=0 && (x+ 8)<256) p[ 8] = pal[d[ 8]|c];
		 				if ((x+ 9)>=0 && (x+ 9)<256) p[ 9] = pal[d[ 9]|c];
		 				if ((x+10)>=0 && (x+10)<256) p[10] = pal[d[10]|c];
		 				if ((x+11)>=0 && (x+11)<256) p[11] = pal[d[11]|c];
		 				if ((x+12)>=0 && (x+12)<256) p[12] = pal[d[12]|c];
		 				if ((x+13)>=0 && (x+13)<256) p[13] = pal[d[13]|c];
		 				if ((x+14)>=0 && (x+14)<256) p[14] = pal[d[14]|c];
		 				if ((x+15)>=0 && (x+15)<256) p[15] = pal[d[15]|c];
	 				}
	 				d += 16;
	 				p += 256;
	 			}
			else 
				for (int k=0;k<16;k++) {
	 				p[ 0] = pal[d[ 0]|c];
	 				p[ 1] = pal[d[ 1]|c];
	 				p[ 2] = pal[d[ 2]|c];
	 				p[ 3] = pal[d[ 3]|c];
	 				p[ 4] = pal[d[ 4]|c];
	 				p[ 5] = pal[d[ 5]|c];
	 				p[ 6] = pal[d[ 6]|c];
	 				p[ 7] = pal[d[ 7]|c];
	 				p[ 8] = pal[d[ 8]|c];
	 				p[ 9] = pal[d[ 9]|c];
	 				p[10] = pal[d[10]|c];
	 				p[11] = pal[d[11]|c];
	 				p[12] = pal[d[12]|c];
	 				p[13] = pal[d[13]|c];
	 				p[14] = pal[d[14]|c];
	 				p[15] = pal[d[15]|c];
	 				
	 				d += 16;
	 				p += 256;
	 			}
		} 
	}
}

static void drawForeground()
{
	/* cccc tttt tttt tttt */
	unsigned short * pal = RamCurPal + 256;
	int offs, mx, my, x, y, sx, sy;

	if (game_drv == GAME_RAIDEN) {
		sx = (RamScroll[4] | (RamScroll[5] << 8)) & 0x1ff;
		sy = (RamScroll[6] | (RamScroll[7] << 8)) & 0x1ff;
	} else {
		sx = ((RamScroll[6]&0x10)<<4) | ((RamScroll[7]&0x7f) << 1) | ((RamScroll[7]&0x80) >> 7);
		sy = ((RamScroll[4]&0x10)<<4) | ((RamScroll[5]&0x7f) << 1) | ((RamScroll[5]&0x80) >> 7);
	}

	mx = 0;
	my = -1;
	for (offs = 0; offs < 32*32; offs++) {
		my++;
		if (my == 32) {
			my = 0;
			mx++;
		}
		
		x = mx * 16 -  0 - sx;
		if (x < -16) x += 512;
		
		y = my * 16 - 16 - sy;
		if (y < -16) y += 512;
		
		if (x<=-16 || x>=256 || y<=-16 || y>=224) continue;
		else {
			unsigned int tileno = RamFg[offs] & 0x0FFF;
			if (tileno == 0) continue;
 			unsigned int c = (RamFg[offs] & 0xF000) >> 8;
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 256 + x;
			unsigned char *d = RomGfx3 + (tileno << 8);
			
			if (x<0 || x>(256-16) || y<0 || y>(224-16))
				for (int k=0;k<16;k++) {
					if ((y+k)>=0 && (y+k)<224) {
		 				if (d[ 0]!=15 && (x+ 0)>=0 && (x+ 0)<256) p[ 0] = pal[d[ 0]|c];
		 				if (d[ 1]!=15 && (x+ 1)>=0 && (x+ 1)<256) p[ 1] = pal[d[ 1]|c];
		 				if (d[ 2]!=15 && (x+ 2)>=0 && (x+ 2)<256) p[ 2] = pal[d[ 2]|c];
		 				if (d[ 3]!=15 && (x+ 3)>=0 && (x+ 3)<256) p[ 3] = pal[d[ 3]|c];
		 				if (d[ 4]!=15 && (x+ 4)>=0 && (x+ 4)<256) p[ 4] = pal[d[ 4]|c];
		 				if (d[ 5]!=15 && (x+ 5)>=0 && (x+ 5)<256) p[ 5] = pal[d[ 5]|c];
		 				if (d[ 6]!=15 && (x+ 6)>=0 && (x+ 6)<256) p[ 6] = pal[d[ 6]|c];
		 				if (d[ 7]!=15 && (x+ 7)>=0 && (x+ 7)<256) p[ 7] = pal[d[ 7]|c];
		 				if (d[ 8]!=15 && (x+ 8)>=0 && (x+ 8)<256) p[ 8] = pal[d[ 8]|c];
		 				if (d[ 9]!=15 && (x+ 9)>=0 && (x+ 9)<256) p[ 9] = pal[d[ 9]|c];
		 				if (d[10]!=15 && (x+10)>=0 && (x+10)<256) p[10] = pal[d[10]|c];
		 				if (d[11]!=15 && (x+11)>=0 && (x+11)<256) p[11] = pal[d[11]|c];
		 				if (d[12]!=15 && (x+12)>=0 && (x+12)<256) p[12] = pal[d[12]|c];
		 				if (d[13]!=15 && (x+13)>=0 && (x+13)<256) p[13] = pal[d[13]|c];
		 				if (d[14]!=15 && (x+14)>=0 && (x+14)<256) p[14] = pal[d[14]|c];
		 				if (d[15]!=15 && (x+15)>=0 && (x+15)<256) p[15] = pal[d[15]|c];
	 				}
	 				d += 16;
	 				p += 256;
	 			}
			else 
				for (int k=0;k<16;k++) {
	 				if (d[ 0] != 15) p[ 0] = pal[d[ 0]|c];
	 				if (d[ 1] != 15) p[ 1] = pal[d[ 1]|c];
	 				if (d[ 2] != 15) p[ 2] = pal[d[ 2]|c];
	 				if (d[ 3] != 15) p[ 3] = pal[d[ 3]|c];
	 				if (d[ 4] != 15) p[ 4] = pal[d[ 4]|c];
	 				if (d[ 5] != 15) p[ 5] = pal[d[ 5]|c];
	 				if (d[ 6] != 15) p[ 6] = pal[d[ 6]|c];
	 				if (d[ 7] != 15) p[ 7] = pal[d[ 7]|c];
	 				if (d[ 8] != 15) p[ 8] = pal[d[ 8]|c];
	 				if (d[ 9] != 15) p[ 9] = pal[d[ 9]|c];
	 				if (d[10] != 15) p[10] = pal[d[10]|c];
	 				if (d[11] != 15) p[11] = pal[d[11]|c];
	 				if (d[12] != 15) p[12] = pal[d[12]|c];
	 				if (d[13] != 15) p[13] = pal[d[13]|c];
	 				if (d[14] != 15) p[14] = pal[d[14]|c];
	 				if (d[15] != 15) p[15] = pal[d[15]|c];
	 				
	 				d += 16;
	 				p += 256;
	 			}
		} 
	}
}

static void drawSprites(int pri)
{
	/* yyyyyyyy -ff-CCCC tttttttt ----TTTT xxxxxxxx PP-----X -------- EEEEEEEE */
	unsigned short * pal = RamCurPal + 512;
	
	for(unsigned char * pSpr = RamSpr + 0x1000 - 8; pSpr>=RamSpr; pSpr-=8 ) {
		
		if (pSpr[7]!=0xf) continue;
		if (!(pri & pSpr[5])) continue;

		int fx= pSpr[1] & 0x20;
		int fy= pSpr[1] & 0x40;
		int y = pSpr[0] - 16;
		int x = pSpr[4];

		if (pSpr[5]&1) x = 0 - (0x100-x);

		int c = (pSpr[1] & 0xf) << 4;
		int sprite = (pSpr[2] | (pSpr[3] << 8)) & 0x0fff;
		
		if (x<=-16 || x>=256 || y<=-16 || y>=224) continue;
		else {
			
			unsigned char *d = RomGfx4 + (sprite << 8);
 			unsigned short * p = (unsigned short *) pBurnDraw ;
			p += y * 256 + x;
			
			if (x<0 || x>(256-16) || y<0 || y>(224-16)) {
				
				
				if ( fy ) {
					p += 256 * 15;
					
					if ( fx )
						for (int k=15;k>=0;k--) {
							if ((y+k)>=0 && (y+k)<224) {
				 				if (d[15]!=15 && (x+ 0)>=0 && (x+ 0)<256) p[ 0] = pal[d[15]|c];
				 				if (d[14]!=15 && (x+ 1)>=0 && (x+ 1)<256) p[ 1] = pal[d[14]|c];
				 				if (d[13]!=15 && (x+ 2)>=0 && (x+ 2)<256) p[ 2] = pal[d[13]|c];
				 				if (d[12]!=15 && (x+ 3)>=0 && (x+ 3)<256) p[ 3] = pal[d[12]|c];
				 				if (d[11]!=15 && (x+ 4)>=0 && (x+ 4)<256) p[ 4] = pal[d[11]|c];
				 				if (d[10]!=15 && (x+ 5)>=0 && (x+ 5)<256) p[ 5] = pal[d[10]|c];
				 				if (d[ 9]!=15 && (x+ 6)>=0 && (x+ 6)<256) p[ 6] = pal[d[ 9]|c];
				 				if (d[ 8]!=15 && (x+ 7)>=0 && (x+ 7)<256) p[ 7] = pal[d[ 8]|c];
				 				if (d[ 7]!=15 && (x+ 8)>=0 && (x+ 8)<256) p[ 8] = pal[d[ 7]|c];
				 				if (d[ 6]!=15 && (x+ 9)>=0 && (x+ 9)<256) p[ 9] = pal[d[ 6]|c];
				 				if (d[ 5]!=15 && (x+10)>=0 && (x+10)<256) p[10] = pal[d[ 5]|c];
				 				if (d[ 4]!=15 && (x+11)>=0 && (x+11)<256) p[11] = pal[d[ 4]|c];
				 				if (d[ 3]!=15 && (x+12)>=0 && (x+12)<256) p[12] = pal[d[ 3]|c];
				 				if (d[ 2]!=15 && (x+13)>=0 && (x+13)<256) p[13] = pal[d[ 2]|c];
				 				if (d[ 1]!=15 && (x+14)>=0 && (x+14)<256) p[14] = pal[d[ 1]|c];
				 				if (d[ 0]!=15 && (x+15)>=0 && (x+15)<256) p[15] = pal[d[ 0]|c];
			 				}
			 				d += 16;
			 				p -= 256;
			 			}
					else
						for (int k=15;k>=0;k--) {
							if ((y+k)>=0 && (y+k)<224) {
				 				if (d[ 0]!=15 && (x+ 0)>=0 && (x+ 0)<256) p[ 0] = pal[d[ 0]|c];
				 				if (d[ 1]!=15 && (x+ 1)>=0 && (x+ 1)<256) p[ 1] = pal[d[ 1]|c];
				 				if (d[ 2]!=15 && (x+ 2)>=0 && (x+ 2)<256) p[ 2] = pal[d[ 2]|c];
				 				if (d[ 3]!=15 && (x+ 3)>=0 && (x+ 3)<256) p[ 3] = pal[d[ 3]|c];
				 				if (d[ 4]!=15 && (x+ 4)>=0 && (x+ 4)<256) p[ 4] = pal[d[ 4]|c];
				 				if (d[ 5]!=15 && (x+ 5)>=0 && (x+ 5)<256) p[ 5] = pal[d[ 5]|c];
				 				if (d[ 6]!=15 && (x+ 6)>=0 && (x+ 6)<256) p[ 6] = pal[d[ 6]|c];
				 				if (d[ 7]!=15 && (x+ 7)>=0 && (x+ 7)<256) p[ 7] = pal[d[ 7]|c];
				 				if (d[ 8]!=15 && (x+ 8)>=0 && (x+ 8)<256) p[ 8] = pal[d[ 8]|c];
				 				if (d[ 9]!=15 && (x+ 9)>=0 && (x+ 9)<256) p[ 9] = pal[d[ 9]|c];
				 				if (d[10]!=15 && (x+10)>=0 && (x+10)<256) p[10] = pal[d[10]|c];
				 				if (d[11]!=15 && (x+11)>=0 && (x+11)<256) p[11] = pal[d[11]|c];
				 				if (d[12]!=15 && (x+12)>=0 && (x+12)<256) p[12] = pal[d[12]|c];
				 				if (d[13]!=15 && (x+13)>=0 && (x+13)<256) p[13] = pal[d[13]|c];
				 				if (d[14]!=15 && (x+14)>=0 && (x+14)<256) p[14] = pal[d[14]|c];
				 				if (d[15]!=15 && (x+15)>=0 && (x+15)<256) p[15] = pal[d[15]|c];
			 				}
			 				d += 16;
			 				p -= 256;
			 			}
					
				} else {

					if ( fx )
						for (int k=0;k<16;k++) {
							if ((y+k)>=0 && (y+k)<224) {
				 				if (d[15]!=15 && (x+ 0)>=0 && (x+ 0)<256) p[ 0] = pal[d[15]|c];
				 				if (d[14]!=15 && (x+ 1)>=0 && (x+ 1)<256) p[ 1] = pal[d[14]|c];
				 				if (d[13]!=15 && (x+ 2)>=0 && (x+ 2)<256) p[ 2] = pal[d[13]|c];
				 				if (d[12]!=15 && (x+ 3)>=0 && (x+ 3)<256) p[ 3] = pal[d[12]|c];
				 				if (d[11]!=15 && (x+ 4)>=0 && (x+ 4)<256) p[ 4] = pal[d[11]|c];
				 				if (d[10]!=15 && (x+ 5)>=0 && (x+ 5)<256) p[ 5] = pal[d[10]|c];
				 				if (d[ 9]!=15 && (x+ 6)>=0 && (x+ 6)<256) p[ 6] = pal[d[ 9]|c];
				 				if (d[ 8]!=15 && (x+ 7)>=0 && (x+ 7)<256) p[ 7] = pal[d[ 8]|c];
				 				if (d[ 7]!=15 && (x+ 8)>=0 && (x+ 8)<256) p[ 8] = pal[d[ 7]|c];
				 				if (d[ 6]!=15 && (x+ 9)>=0 && (x+ 9)<256) p[ 9] = pal[d[ 6]|c];
				 				if (d[ 5]!=15 && (x+10)>=0 && (x+10)<256) p[10] = pal[d[ 5]|c];
				 				if (d[ 4]!=15 && (x+11)>=0 && (x+11)<256) p[11] = pal[d[ 4]|c];
				 				if (d[ 3]!=15 && (x+12)>=0 && (x+12)<256) p[12] = pal[d[ 3]|c];
				 				if (d[ 2]!=15 && (x+13)>=0 && (x+13)<256) p[13] = pal[d[ 2]|c];
				 				if (d[ 1]!=15 && (x+14)>=0 && (x+14)<256) p[14] = pal[d[ 1]|c];
				 				if (d[ 0]!=15 && (x+15)>=0 && (x+15)<256) p[15] = pal[d[ 0]|c];
			 				}
			 				d += 16;
			 				p += 256;
			 			}
					else
						for (int k=0;k<16;k++) {
							if ((y+k)>=0 && (y+k)<224) {
				 				if (d[ 0]!=15 && (x+ 0)>=0 && (x+ 0)<256) p[ 0] = pal[d[ 0]|c];
				 				if (d[ 1]!=15 && (x+ 1)>=0 && (x+ 1)<256) p[ 1] = pal[d[ 1]|c];
				 				if (d[ 2]!=15 && (x+ 2)>=0 && (x+ 2)<256) p[ 2] = pal[d[ 2]|c];
				 				if (d[ 3]!=15 && (x+ 3)>=0 && (x+ 3)<256) p[ 3] = pal[d[ 3]|c];
				 				if (d[ 4]!=15 && (x+ 4)>=0 && (x+ 4)<256) p[ 4] = pal[d[ 4]|c];
				 				if (d[ 5]!=15 && (x+ 5)>=0 && (x+ 5)<256) p[ 5] = pal[d[ 5]|c];
				 				if (d[ 6]!=15 && (x+ 6)>=0 && (x+ 6)<256) p[ 6] = pal[d[ 6]|c];
				 				if (d[ 7]!=15 && (x+ 7)>=0 && (x+ 7)<256) p[ 7] = pal[d[ 7]|c];
				 				if (d[ 8]!=15 && (x+ 8)>=0 && (x+ 8)<256) p[ 8] = pal[d[ 8]|c];
				 				if (d[ 9]!=15 && (x+ 9)>=0 && (x+ 9)<256) p[ 9] = pal[d[ 9]|c];
				 				if (d[10]!=15 && (x+10)>=0 && (x+10)<256) p[10] = pal[d[10]|c];
				 				if (d[11]!=15 && (x+11)>=0 && (x+11)<256) p[11] = pal[d[11]|c];
				 				if (d[12]!=15 && (x+12)>=0 && (x+12)<256) p[12] = pal[d[12]|c];
				 				if (d[13]!=15 && (x+13)>=0 && (x+13)<256) p[13] = pal[d[13]|c];
				 				if (d[14]!=15 && (x+14)>=0 && (x+14)<256) p[14] = pal[d[14]|c];
				 				if (d[15]!=15 && (x+15)>=0 && (x+15)<256) p[15] = pal[d[15]|c];
			 				}
			 				d += 16;
			 				p += 256;
			 			}
				}

			} else {
				
				if ( fy ) {
					
					p += 256 * 15;
					
					if ( fx ) {
					
						for (int k=15;k>=0;k--) {
			 				if (d[15] != 15) p[ 0] = pal[d[15]|c];
			 				if (d[14] != 15) p[ 1] = pal[d[14]|c];
			 				if (d[13] != 15) p[ 2] = pal[d[13]|c];
			 				if (d[12] != 15) p[ 3] = pal[d[12]|c];
			 				if (d[11] != 15) p[ 4] = pal[d[11]|c];
			 				if (d[10] != 15) p[ 5] = pal[d[10]|c];
			 				if (d[ 9] != 15) p[ 6] = pal[d[ 9]|c];
			 				if (d[ 8] != 15) p[ 7] = pal[d[ 8]|c];
			 				if (d[ 7] != 15) p[ 8] = pal[d[ 7]|c];
			 				if (d[ 6] != 15) p[ 9] = pal[d[ 6]|c];
			 				if (d[ 5] != 15) p[10] = pal[d[ 5]|c];
			 				if (d[ 4] != 15) p[11] = pal[d[ 4]|c];
			 				if (d[ 3] != 15) p[12] = pal[d[ 3]|c];
			 				if (d[ 2] != 15) p[13] = pal[d[ 2]|c];
			 				if (d[ 1] != 15) p[14] = pal[d[ 1]|c];
			 				if (d[ 0] != 15) p[15] = pal[d[ 0]|c];
			 				
			 				d += 16;
			 				p -= 256;
			 			}
			 			
		 			} else {
		 				
						for (int k=15;k>=0;k--) {
			 				if (d[ 0] != 15) p[ 0] = pal[d[ 0]|c];
			 				if (d[ 1] != 15) p[ 1] = pal[d[ 1]|c];
			 				if (d[ 2] != 15) p[ 2] = pal[d[ 2]|c];
			 				if (d[ 3] != 15) p[ 3] = pal[d[ 3]|c];
			 				if (d[ 4] != 15) p[ 4] = pal[d[ 4]|c];
			 				if (d[ 5] != 15) p[ 5] = pal[d[ 5]|c];
			 				if (d[ 6] != 15) p[ 6] = pal[d[ 6]|c];
			 				if (d[ 7] != 15) p[ 7] = pal[d[ 7]|c];
			 				if (d[ 8] != 15) p[ 8] = pal[d[ 8]|c];
			 				if (d[ 9] != 15) p[ 9] = pal[d[ 9]|c];
			 				if (d[10] != 15) p[10] = pal[d[10]|c];
			 				if (d[11] != 15) p[11] = pal[d[11]|c];
			 				if (d[12] != 15) p[12] = pal[d[12]|c];
			 				if (d[13] != 15) p[13] = pal[d[13]|c];
			 				if (d[14] != 15) p[14] = pal[d[14]|c];
			 				if (d[15] != 15) p[15] = pal[d[15]|c];
			 				
			 				d += 16;
			 				p -= 256;
			 			}
		 				 				
		 			}
		 								
				} else {
				
					if ( fx ) {
					
						for (int k=0;k<16;k++) {
			 				if (d[15] != 15) p[ 0] = pal[d[15]|c];
			 				if (d[14] != 15) p[ 1] = pal[d[14]|c];
			 				if (d[13] != 15) p[ 2] = pal[d[13]|c];
			 				if (d[12] != 15) p[ 3] = pal[d[12]|c];
			 				if (d[11] != 15) p[ 4] = pal[d[11]|c];
			 				if (d[10] != 15) p[ 5] = pal[d[10]|c];
			 				if (d[ 9] != 15) p[ 6] = pal[d[ 9]|c];
			 				if (d[ 8] != 15) p[ 7] = pal[d[ 8]|c];
			 				if (d[ 7] != 15) p[ 8] = pal[d[ 7]|c];
			 				if (d[ 6] != 15) p[ 9] = pal[d[ 6]|c];
			 				if (d[ 5] != 15) p[10] = pal[d[ 5]|c];
			 				if (d[ 4] != 15) p[11] = pal[d[ 4]|c];
			 				if (d[ 3] != 15) p[12] = pal[d[ 3]|c];
			 				if (d[ 2] != 15) p[13] = pal[d[ 2]|c];
			 				if (d[ 1] != 15) p[14] = pal[d[ 1]|c];
			 				if (d[ 0] != 15) p[15] = pal[d[ 0]|c];
			 				
			 				d += 16;
			 				p += 256;
			 			}
			 			
		 			} else {
		 				
						for (int k=0;k<16;k++) {
			 				if (d[ 0] != 15) p[ 0] = pal[d[ 0]|c];
			 				if (d[ 1] != 15) p[ 1] = pal[d[ 1]|c];
			 				if (d[ 2] != 15) p[ 2] = pal[d[ 2]|c];
			 				if (d[ 3] != 15) p[ 3] = pal[d[ 3]|c];
			 				if (d[ 4] != 15) p[ 4] = pal[d[ 4]|c];
			 				if (d[ 5] != 15) p[ 5] = pal[d[ 5]|c];
			 				if (d[ 6] != 15) p[ 6] = pal[d[ 6]|c];
			 				if (d[ 7] != 15) p[ 7] = pal[d[ 7]|c];
			 				if (d[ 8] != 15) p[ 8] = pal[d[ 8]|c];
			 				if (d[ 9] != 15) p[ 9] = pal[d[ 9]|c];
			 				if (d[10] != 15) p[10] = pal[d[10]|c];
			 				if (d[11] != 15) p[11] = pal[d[11]|c];
			 				if (d[12] != 15) p[12] = pal[d[12]|c];
			 				if (d[13] != 15) p[13] = pal[d[13]|c];
			 				if (d[14] != 15) p[14] = pal[d[14]|c];
			 				if (d[15] != 15) p[15] = pal[d[15]|c];
			 				
			 				d += 16;
			 				p += 256;
			 			}
		 				 				
		 			}
	 			
	 			}
	 		}
		} 
	
	}
	
}

static void drawText()
{
	/* TT-- cccc  tttt tttt */
	unsigned short * pal = RamCurPal + 768;
	int offs, mx, my, x, y;

	mx = 0;
	my = -1;
	for (offs = 0; offs < 32*32; offs++) {
		my++;
		if (my == 32) {
			my = 0;
			mx++;
		}
		x = mx * 8;
		y = my * 8 - 16;
		
		if ( y<0 || y>=224) continue;
		else {
			unsigned int tileno = (RamTxt[offs] & 0x00FF) | ((RamTxt[offs] & 0xC000) >> 6);
			if (tileno == 0) continue;
 			unsigned int c = (RamTxt[offs] & 0x0F00) >> 4;
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 256 + x;
			unsigned char *d = RomGfx1 + (tileno << 6);
			
			for (int k=0;k<8;k++) {
 				if (d[0] != 15) p[0] = pal[d[0]|c];
 				if (d[1] != 15) p[1] = pal[d[1]|c];
 				if (d[2] != 15) p[2] = pal[d[2]|c];
 				if (d[3] != 15) p[3] = pal[d[3]|c];
 				if (d[4] != 15) p[4] = pal[d[4]|c];
 				if (d[5] != 15) p[5] = pal[d[5]|c];
 				if (d[6] != 15) p[6] = pal[d[6]|c];
 				if (d[7] != 15) p[7] = pal[d[7]|c];
 				d += 8;
 				p += 256;
 			}
		} 
	}
}

static void drawTextAlt()
{
	/* TT-- cccc  tttt tttt */
	unsigned short * pal = RamCurPal + 768;
	int offs, mx, my, x, y;

	mx = -1;
	my = 0;
	for (offs = 0; offs < 32*32; offs++) {
		mx++;
		if (mx == 32) {
			mx = 0;
			my++;
		}
		x = mx * 8;
		y = my * 8 - 16;
		
		if ( y<0 || y>=224) continue;
		else {
			unsigned int tileno = (RamTxt[offs] & 0x00FF) | ((RamTxt[offs] & 0xC000) >> 6);
			if (tileno == 0) continue;
 			unsigned int c = (RamTxt[offs] & 0x0F00) >> 4;
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 256 + x;
			unsigned char *d = RomGfx1 + (tileno << 6);
			
			for (int k=0;k<8;k++) {
 				if (d[0] != 15) p[0] = pal[d[0]|c];
 				if (d[1] != 15) p[1] = pal[d[1]|c];
 				if (d[2] != 15) p[2] = pal[d[2]|c];
 				if (d[3] != 15) p[3] = pal[d[3]|c];
 				if (d[4] != 15) p[4] = pal[d[4]|c];
 				if (d[5] != 15) p[5] = pal[d[5]|c];
 				if (d[6] != 15) p[6] = pal[d[6]|c];
 				if (d[7] != 15) p[7] = pal[d[7]|c];
 				d += 8;
 				p += 256;
 			}
		} 
	}
}

static void DrvDraw()
{
	drawBackground();
	drawSprites(0x40);
	drawForeground();
	drawSprites(0x80);
	drawText();
}

static int DrvFrame()
{
	if (DrvReset) DrvDoReset();
	
	if (bRecalcPalette) {
		for (int i=0;i<(0x1000/2);i++)
			RamCurPal[i] = CalcCol(i << 1);
		bRecalcPalette = 0;
	}
	
	DrvInput[0] = 0x00;	// Buttons
	DrvInput[1] = 0x00;	// Joy1
	DrvInput[2] = 0x00;	// Joy2
	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvButton[i] & 1) << i;
		DrvInput[1] |= (DrvJoy1[i] & 1) << i;
		DrvInput[2] |= (DrvJoy2[i] & 1) << i;
	}
	
	VezNewFrame();
	ZetNewFrame();
	
	for (int i=0; i<200; i++) {
		VezOpen(0);
		VezRun(10000000 / 60 / 200);
		VezOpen(1);
		VezRun(10000000 / 60 / 200);
	}

	VezOpen(0);
	VezSetIRQLine(0xc8, VEZ_IRQSTATUS_ACK);

	VezOpen(1);
	VezSetIRQLine(0xc8, VEZ_IRQSTATUS_ACK);

	ZetRun( 3579545 / 60 );

	if (pBurnDraw) DrvDraw();
	
	BurnTimerEndFrame(3579545 / 60);
	BurnYM3812Update(pBurnSoundOut, nBurnSoundLen);
	if (pBurnSoundOut)
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	
	return 0;	
}

static void DrvDrawAlt()
{
	drawBackground();
	drawSprites(0x40);
	drawForeground();
	drawSprites(0x80);
	drawTextAlt();
}

static int DrvFrameAlt()
{
	if (DrvReset) DrvDoReset();
	
	if (bRecalcPalette) {
		for (int i=0;i<(0x1000/2);i++)
			RamCurPal[i] = CalcCol(i << 1);
		bRecalcPalette = 0;
	}
	
	DrvInput[0] = 0x00;	// Buttons
	DrvInput[1] = 0x00;	// Joy1
	DrvInput[2] = 0x00;	// Joy2
	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvButton[i] & 1) << i;
		DrvInput[1] |= (DrvJoy1[i] & 1) << i;
		DrvInput[2] |= (DrvJoy2[i] & 1) << i;
	}
	
	VezNewFrame();
	ZetNewFrame();
	
	for (int i=0; i<120; i++) {
		VezOpen(0);
		VezRun(10000000 / 60 / 120);
		VezOpen(1);
		VezRun(10000000 / 60 / 120);
	}

	VezOpen(0);
	VezSetIRQLine(0xc8, VEZ_IRQSTATUS_ACK);

	VezOpen(1);
	VezSetIRQLine(0xc8, VEZ_IRQSTATUS_ACK);

	ZetRun( 3579545 / 60 );

	if (pBurnDraw) DrvDrawAlt();
	
	BurnTimerEndFrame(3579545 / 60);
	BurnYM3812Update(pBurnSoundOut, nBurnSoundLen);
	if (pBurnSoundOut)
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	
	return 0;	
}

static int DrvScan(int nAction,int *pnMin)
{
	if ( pnMin ) *pnMin =  0x029671;
	
	struct BurnArea ba;
	
	if (nAction & ACB_MEMORY_RAM) {					// Scan all memory, devices & variables
		memset(&ba, 0, sizeof(ba));
    	ba.Data	  = RamStart;
		ba.nLen	  = RamEnd-RamStart;
		ba.szName = "All Ram";
		BurnAcb(&ba);
		
		if (nAction & ACB_WRITE)
			bRecalcPalette = 1;
	}
	
	if (nAction & ACB_DRIVER_DATA) {

		VezScan(nAction);
		ZetScan(nAction);
		
		BurnYM3812Scan(nAction, pnMin);
		MSM6295Scan(0, nAction);

		int cbank;
		if (nAction & ACB_WRITE) {
			SCAN_VAR(cbank);
			set_seibu_bank(cbank);
		} else {
			cbank = seibu_bank;
			SCAN_VAR(cbank);
		}

		SCAN_VAR(DrvInput);
		
		SCAN_VAR(main2sub);
		SCAN_VAR(sub2main);
		SCAN_VAR(main2sub_pending);
		SCAN_VAR(sub2main_pending);
		SCAN_VAR(z80_irq1);
		SCAN_VAR(z80_irq2);
	}
	
	return 0;
}

struct BurnDriver BurnDrvRaiden = {
	"raiden", NULL, NULL, "1990",
	"Raiden\0", NULL, "Seibu Kaihatsu", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_POST90S,
	NULL, raidenRomInfo, raidenRomName, raidenInputInfo, raidenDIPInfo,
	DrvInit, DrvExit, DrvFrame, NULL, DrvScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	224, 256, 3, 4
};

struct BurnDriver BurnDrvRaidena = {
	"raidena", "raiden", NULL, "1990",
	"Raiden (Alternate Hardware)\0", NULL, "Seibu Kaihatsu", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY | BDF_ORIENTATION_VERTICAL | BDF_CLONE, 2, HARDWARE_MISC_POST90S,
	NULL, raidenaRomInfo, raidenaRomName, raidenInputInfo, raidenDIPInfo,
	DrvInit, DrvExit, DrvFrameAlt, NULL, DrvScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	224, 256, 3, 4
};

struct BurnDriver BurnDrvRaidenk = {
	"raidenk", "raiden", NULL, "1990",
	"Raiden (Korea)\0", NULL, "Seibu Kaihatsu (IBL Corporation license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY | BDF_ORIENTATION_VERTICAL | BDF_CLONE, 2, HARDWARE_MISC_POST90S,
	NULL, raidenkRomInfo, raidenkRomName, raidenInputInfo, raidenDIPInfo,
	DrvInit, DrvExit, DrvFrameAlt, NULL, DrvScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	224, 256, 3, 4
};

struct BurnDriver BurnDrvRaident = {
	"raident", "raiden", NULL, "1990",
	"Raiden (Taiwan)\0", NULL, "Seibu Kaihatsu (Liang HWA Electronics license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY | BDF_ORIENTATION_VERTICAL | BDF_CLONE, 2, HARDWARE_MISC_POST90S,
	NULL, raidentRomInfo, raidentRomName, raidenInputInfo, raidenDIPInfo,
	DrvInit, DrvExit, DrvFrameAlt, NULL, DrvScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	224, 256, 3, 4
};
