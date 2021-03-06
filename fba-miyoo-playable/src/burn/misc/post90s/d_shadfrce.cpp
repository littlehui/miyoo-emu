/********************************************************************************
 Shadow Force (c)1993 Technos
 Preliminary Driver by David Haywood
 Based on the Various Other Technos Games
 ********************************************************************************
 port to Finalburn Alpha by OopsWare. 2007
 ********************************************************************************
 
 2007.07.05
   bug fix: no reset while init driver, 68k boot from pc addr 0x000000 ...
   add pri-buf (RamPri) for render
   
 ********************************************************************************/

#include "burnint.h"
#include "burn_ym2151.h"
#include "msm6295.h"

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;

static unsigned char *Rom68K;
static unsigned char *RomZ80;
static unsigned char *RomGfx01;
static unsigned char *RomGfx02;
static unsigned char *RomGfx03;

static unsigned char *Ram68K;
static unsigned short *RamBg00;
static unsigned short *RamBg01;
static unsigned short *RamFg;
static unsigned short *RamSpr;
static unsigned short *RamPal;
static unsigned char *RamZ80;

static unsigned short *RamCurPal;
static unsigned char *RamPri;

static unsigned char DrvButton[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvDipBtn[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static unsigned char DrvInput[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvReset = 0;
static unsigned char nBrightness = 0xFF;

static unsigned char bVBlink = 0;
static unsigned short bg0scrollx, bg0scrolly, bg1scrollx, bg1scrolly;
static unsigned char nSoundlatch = 0;
static unsigned char bRecalcPalette = 0;

static int nZ80Cycles;

inline static void CalcCol(int idx)
{
	/* xBBBBBGGGGGRRRRR */
	unsigned short nColour = RamPal[idx];
	int r = (nColour & 0x001F) << 3;	r |= r >> 5;	// Red
	int g = (nColour & 0x03E0) >> 2;	g |= g >> 5;	// Green
	int b = (nColour & 0x7C00) >> 7;	b |= b >> 5;	// Blue
	r = (r * nBrightness) >> 8;
	g = (g * nBrightness) >> 8;
	b = (b * nBrightness) >> 8;
	RamCurPal[idx] = BurnHighCol(r, g, b, 0);
}

static struct BurnInputInfo shadfrceInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvDipBtn + 0,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},

	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 0,	"p1 right"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 left"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 down"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},
	{"P1 Button 3",	BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 3"},
	{"P1 Button 4",	BIT_DIGITAL,	DrvButton + 0,	"p1 fire 4"},
	{"P1 Button 5",	BIT_DIGITAL,	DrvButton + 1,	"p1 fire 5"},
	{"P1 Button 6",	BIT_DIGITAL,	DrvButton + 2,	"p1 fire 6"},

	{"P2 Coin",		BIT_DIGITAL,	DrvDipBtn + 1,	"p2 coin"},		// only available when you are in "test mode"
	{"P2 Start",	BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},

	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 0,	"p2 right"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 left"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},
	{"P2 Button 3",	BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 3"},
	{"P2 Button 4",	BIT_DIGITAL,	DrvButton + 3,	"p2 fire 4"},
	{"P2 Button 5",	BIT_DIGITAL,	DrvButton + 4,	"p2 fire 5"},
	{"P2 Button 6",	BIT_DIGITAL,	DrvButton + 5,	"p2 fire 6"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvInput + 1,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvInput + 3,	"dip"},
	{"Dip C",		BIT_DIPSWITCH,	DrvInput + 5,	"dip"},
	{"Dip D",		BIT_DIPSWITCH,	DrvInput + 7,	"dip"},
};

STDINPUTINFO(shadfrce);

static struct BurnDIPInfo shadfrceDIPList[] = {

	// Defaults
	{0x19,	0xFF, 0xFF,	0x00, NULL},
	{0x1A,	0xFF, 0xFF,	0x00, NULL},
	{0x1B,	0xFF, 0xFF,	0x00, NULL},
	{0x1C,	0xFF, 0xFF,	0x00, NULL},

	// DIP 1
	// coin 1
	// coin 2
	{0,		0xFE, 0,	2,	  "Service1"},	// only available when you are in "test mode"
	{0x19,	0x01, 0x04,	0x00, "Off"},
	{0x19,	0x01, 0x04,	0x04, "On"},
	
	// DIP 2
	{0,		0xFE, 0,	4,	  "Difficulty"},
	{0x1A,	0x01, 0x03, 0x00, "Normal"},
	{0x1A,	0x01, 0x03, 0x01, "Hard"},
	{0x1A,	0x01, 0x03, 0x02, "Easy"},
	{0x1A,	0x01, 0x03, 0x03, "Hardest"},
	{0,		0xFE, 0,	4,	  "Stage Clear Energy Regain"},
	{0x1A,	0x01, 0x0C, 0x00, "25%"}, 
	{0x1A,	0x01, 0x0C, 0x04, "10%"},	
	{0x1A,	0x01, 0x0C, 0x08, "50%"}, 
	{0x1A,	0x01, 0x0C, 0x0C, "0%"},	
	
	// DIP 3
	{0,		0xFE, 0,	4,	  "Coinage"},	
	{0x1B,	0x01, 0x06, 0x00, "1C_1C"},
	{0x1B,	0x01, 0x06, 0x02, "1C_2C"},
	{0x1B,	0x01, 0x06, 0x04, "2C_1C"},
	{0x1B,	0x01, 0x06, 0x06, "3C_1C"},
	
	{0,		0xFE, 0,	2,	  "Continue Price"},
	{0x1B,	0x01, 0x08, 0x00, "Off"},
	{0x1B,	0x01, 0x08, 0x80, "On"},
	{0,		0xFE, 0,	2,	  "Free Play"},
	{0x1B,	0x01, 0x10, 0x00, "Off"}, 
	{0x1B,	0x01, 0x10, 0x10, "On"},
	{0,		0xFE, 0,	2,	  "Flip Screen"},
	{0x1B,	0x01, 0x20, 0x00, "Off"}, 
	{0x1B,	0x01, 0x20, 0x20, "On"},
	
	// DIP 4
	{0,		0xFE, 0,	2,	  "Demo Sounds"},
	{0x1C,	0x01, 0x01,	0x00, "Off"},
	{0x1C,	0x01, 0x01,	0x01, "On"},
	{0,		0xFE, 0,	2,	  "Test Mode"},
	{0x1C,	0x01, 0x02,	0x00, "Off"},
	{0x1C,	0x01, 0x02,	0x02, "On"},
//	{0,		0xFE, 0,	2,	  "VBlink"},	// ???
//	{0x1C,	0x01, 0x04,	0x00, "Off"},
//	{0x1C,	0x01, 0x04,	0x04, "On"},

};

STDDIPINFO(shadfrce);

// Rom information
static struct BurnRomInfo shadfrceRomDesc[] = {
	{ "32a12-01.34",  0x040000, 0x04501198, BRF_ESS | BRF_PRG },	// 68000 code 
	{ "32a13-01.26",  0x040000, 0xb8f8a05c, BRF_ESS | BRF_PRG }, 
	{ "32a14-0.33",   0x040000, 0x08279be9, BRF_ESS | BRF_PRG }, 
	{ "32a15-0.14",	  0x040000, 0xbfcadfea, BRF_ESS | BRF_PRG }, 
	
	{ "32j10-0.42",   0x010000, 0x65daf475, BRF_ESS | BRF_PRG },	// Z80 code
	
	{ "32a11-0.55",	  0x020000, 0xcfaf5e77, BRF_GRA }, 				// gfx 1 chars
	
	{ "32j4-0.12", 	  0x200000, 0x1ebea5b6, BRF_GRA }, 				// gfx 2 sprite
	{ "32j5-0.13",	  0x200000, 0x600026b5, BRF_GRA }, 
	{ "32j6-0.24", 	  0x200000, 0x6cde8ebe, BRF_GRA }, 
	{ "32j7-0.25",	  0x200000, 0xbcb37922, BRF_GRA }, 
	{ "32j8-0.32",	  0x200000, 0x201bebf6, BRF_GRA }, 

	{ "32j1-0.4",     0x100000, 0xf1cca740, BRF_GRA },				// gfx 3 bg
	{ "32j2-0.5",     0x100000, 0x5fac3e01, BRF_GRA },			
	{ "32j3-0.6",     0x100000, 0xd297925e, BRF_GRA },
	
	{ "32j9-0.76",    0x080000, 0x16001e81, BRF_SND },				// PCM
};

STD_ROM_PICK(shadfrce);
STD_ROM_FN(shadfrce);

static struct BurnRomInfo shadfrcjRomDesc[] = {
	{ "32j12-01.ic34",0x040000, 0x38fdbe1d, BRF_ESS | BRF_PRG },	// 68000 code 
	{ "32j13-01.ic26",0x040000, 0x6e1df6f1, BRF_ESS | BRF_PRG }, 
	{ "32j14-01.ic33",0x040000, 0x89e3fb60, BRF_ESS | BRF_PRG }, 
	{ "32j15-01.ic14",0x040000, 0x3dc3a84a, BRF_ESS | BRF_PRG }, 
	
	{ "32j10-0.42",   0x010000, 0x65daf475, BRF_ESS | BRF_PRG },	// Z80 code
	
	{ "32j11-0.ic55", 0x020000, 0x7252d993, BRF_GRA }, 				// gfx 1 chars
	
	{ "32j4-0.12", 	  0x200000, 0x1ebea5b6, BRF_GRA }, 				// gfx 2 sprite
	{ "32j5-0.13",	  0x200000, 0x600026b5, BRF_GRA }, 
	{ "32j6-0.24", 	  0x200000, 0x6cde8ebe, BRF_GRA }, 
	{ "32j7-0.25",	  0x200000, 0xbcb37922, BRF_GRA }, 
	{ "32j8-0.32",	  0x200000, 0x201bebf6, BRF_GRA }, 

	{ "32j1-0.4",     0x100000, 0xf1cca740, BRF_GRA },				// gfx 3 bg
	{ "32j2-0.5",     0x100000, 0x5fac3e01, BRF_GRA },			
	{ "32j3-0.6",     0x100000, 0xd297925e, BRF_GRA },
	
	{ "32j9-0.76",    0x080000, 0x16001e81, BRF_SND },				// PCM
};

STD_ROM_PICK(shadfrcj);
STD_ROM_FN(shadfrcj);

static int MemIndex()
{
	unsigned char *Next; Next = Mem;
	Rom68K 		= Next; Next += 0x100000;			// 68000 ROM
	RomZ80		= Next; Next += 0x010000;			// Z80 ROM
	RomGfx01	= Next; Next += 0x020000 / 4 * 8;	// fg 8x8x4
	RomGfx02	= Next; Next += 0xA00000 / 5 * 8;	// spr 16x16x5 sprite
	RomGfx03	= Next; Next += 0x300000 / 6 * 8;	// bg 16x16x6 tile
	MSM6295ROM	= Next; Next += 0x080000;
	
	RamStart	= Next;
	RamBg00		= (unsigned short *) Next; Next += 0x002000;
	RamBg01		= (unsigned short *) Next; Next += 0x002000;
	RamFg		= (unsigned short *) Next; Next += 0x002000;
	RamSpr		= (unsigned short *) Next; Next += 0x002000;
	RamPal		= (unsigned short *) Next; Next += 0x008000;
	Ram68K		= Next; Next += 0x010000;
	RamZ80		= Next; Next += 0x001800;
	RamEnd		= Next;
	
	RamCurPal	= (unsigned short *) Next; Next += 0x008000;
	RamPri		= Next; Next += 0x014000;			// 320x256 Priority Buffer
	
	MemEnd		= Next;
	return 0;
}

unsigned char __fastcall shadfrceReadByte(unsigned int sekAddress)
{
/*
	INP0 : --AABBBB CCCCCCCC :  DIP2-8 DIP2-7,  coin 1 coin 2 service 1 , player 1
	INP1 : --DDDDDD EEEEEEEE :  Difficulty but DIP2-8 DIP2-7,  player 2
	INP2 : --FFFFFF GGGGGGGG :  Coinage ~ Flip_Screen but Demo_Sounds service, button 4~6
	INP3 : ----HHII ???????? :  v-blink? ,  service Demo_Sounds
*/	
	switch (sekAddress) {
		case 0x1D0020:
			return (~DrvInput[1] & 0x3F);
		case 0x1D0021:
			return ~DrvInput[0];
		case 0x1D0022:
			return (~DrvInput[3] & 0x3F);
		case 0x1D0023:
			return ~DrvInput[2];
		case 0x1D0024:
			return (~DrvInput[5] & 0x3F);
		case 0x1D0025:
			return ~DrvInput[4];
		case 0x1D0026:
			return ~(DrvInput[7] | (bVBlink << 2)) /*& 0x0F*/;
		case 0x1D0027:
			return ~DrvInput[6];
			
		case 0x1C000B:
			return 0;
		case 0x1D000D:
			return nBrightness;
		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall shadfrceReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x1D0020:
			return ~(DrvInput[0] | (DrvInput[1] << 8)) & 0x3FFF;
		case 0x1D0022:
			return ~(DrvInput[2] | (DrvInput[3] << 8)) & 0x3FFF;
		case 0x1D0024:
			return ~(DrvInput[4] | (DrvInput[5] << 8)) & 0x3FFF;
		case 0x1D0026:
			return ~(DrvInput[6] | ( (DrvInput[7] | (bVBlink << 2))<< 8)) /*& 0x0FFF*/;
		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall shadfrceWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {
		case 0x1C000B:
			// shadfrce_flip_screen
			break;
		case 0x1D000C:
//			bprintf(PRINT_NORMAL, _T("sound write %02x\n"), byteValue);
			nSoundlatch = byteValue;
			ZetNmi();
			break;
		case 0x1D000D:
			//bprintf(PRINT_NORMAL, _T("Brightness set to %02x\n"), byteValue);
			nBrightness = byteValue;
			for(int i=0;i<0x4000;i++) CalcCol(i);
			break;

		case 0x1C0009:
		case 0x1C000D:
		case 0x1D0007:
		case 0x1D0009:
		case 0x1D0011:
		case 0x1D0013:
		case 0x1D0015:
		case 0x1D0017:	// NOP 
			break;
		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
		
	}
}

void __fastcall shadfrceWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x1C0000: bg0scrollx = wordValue & 0x1FF; break;
		case 0x1C0002: bg0scrolly = wordValue & 0x1FF; break;
		case 0x1C0004: bg1scrollx = wordValue & 0x1FF; break;
		case 0x1C0006: bg1scrolly = wordValue & 0x1FF; break;
		
		case 0x1D000D:
			bprintf(PRINT_NORMAL, _T("Brightness set to %04x\n"), wordValue);
			//nBrightness = byteValue;
			break;
		case 0x1D0016:
			// wait v-blink dip change call back ???
			//bprintf(PRINT_NORMAL, _T("v-blink clear at PC 0x%06x\n"), SekGetPC(0) );
			break;
			
		case 0x1D0000:	// NOP
		case 0x1D0002:
		case 0x1D0006:
		case 0x1D0008:
			break;
		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);

	}
}

/*
void __fastcall shadfrceWriteBytePalette(unsigned int sekAddress, unsigned char byteValue)
{
	//CavePalWriteByte(sekAddress & 0xFFFF, byteValue);
	bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to palette %x\n"), byteValue, sekAddress);
}
*/

void __fastcall shadfrceWriteWordPalette(unsigned int sekAddress, unsigned short wordValue)
{
	sekAddress &= 0x7FFF;
	sekAddress >>= 1;
	RamPal[sekAddress] = wordValue;
	CalcCol(sekAddress);
}

unsigned char __fastcall shadfrceZRead(unsigned short a)
{
	switch (a) {
	case 0xC801:	// YM2151_status_port_0_r
		//bprintf(PRINT_NORMAL, _T("YM2151_status_port_0_r\n"));
		return BurnYM2151ReadStatus();
	case 0xD800:	// OKIM6295_status_0_r
		//bprintf(PRINT_NORMAL, _T("OKIM6295_status_0_r\n"));
		return MSM6295ReadStatus(0);
	case 0xE000:	// soundlatch_r
		//bprintf(PRINT_NORMAL, _T("Soundlatch_r\n"));
		return nSoundlatch;
//	default:
//		bprintf(PRINT_NORMAL, _T("Z80 address %04X read.\n"), a);
	}
	return 0;
}

void __fastcall shadfrceZWrite(unsigned short a, unsigned char d)
{
	switch (a) {
	case 0xC800:	// YM2151_register_port_0_w
		//bprintf(PRINT_NORMAL, _T("BurnYM2151SelectRegister(%02x)\n"), d);
		BurnYM2151SelectRegister(d);
		break;
	case 0xC801:	// YM2151_data_port_0_w
		//bprintf(PRINT_NORMAL, _T("BurnYM2151WriteRegister(%02x)\n"), d);
		BurnYM2151WriteRegister(d);
		break;
	case 0xD800:	// OKIM6295_data_0_w
		//bprintf(PRINT_NORMAL, _T("MSM6295Command(%02x)\n"), d);
		MSM6295Command(0, d);
		break;
	case 0xE800:	// oki_bankswitch_w
		bprintf(PRINT_NORMAL, _T("oki_bankswitch_w(%02X)\n"), d);
		break;
//	default:
//		bprintf(PRINT_NORMAL, _T("Z80 address %04X -> %02X.\n"), a, d);
	}
}

static void shadfrceYM2151IRQHandler(int nStatus)
{
	if (nStatus) {
		//ZetRaiseIrq(255);
		ZetSetIRQLine(0xFF, ZET_IRQSTATUS_ACK);
		ZetRun(0x0400);
	} else {
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	}
}

static int DrvDoReset()
{
	SekOpen(0);
    SekSetIRQLine(0, SEK_IRQSTATUS_NONE);
	SekReset();
	SekClose();
	
	ZetOpen(0);
	ZetReset();
	ZetClose();

	MSM6295Reset(0);
	BurnYM2151Reset();

	return 0;
}

static int loadDecodeGfx01()
{
/*
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 2, 4, 6 },
	{ 1, 0, 8*8+1, 8*8+0, 16*8+1, 16*8+0, 24*8+1, 24*8+0 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	32*8
*/
	unsigned char * tmp = (unsigned char *) malloc (0x20000);
	unsigned char * pgfx = RomGfx01;
	if (tmp == NULL) return 1;
	BurnLoadRom(tmp,  5, 1);
	
	for (int i=0; i<(0x20000/32); i++) {
		for( int y=0;y<8;y++) {
			
			pgfx[0] = (((tmp[ 0]>>0)&1)<<0) | (((tmp[ 0]>>2)&1)<<1) | (((tmp[ 0]>>4)&1)<<2) | (((tmp[ 0]>>6)&1)<<3);
			pgfx[1] = (((tmp[ 0]>>1)&1)<<0) | (((tmp[ 0]>>3)&1)<<1) | (((tmp[ 0]>>5)&1)<<2) | (((tmp[ 0]>>7)&1)<<3);
			pgfx[2] = (((tmp[ 8]>>0)&1)<<0) | (((tmp[ 8]>>2)&1)<<1) | (((tmp[ 8]>>4)&1)<<2) | (((tmp[ 8]>>6)&1)<<3);
			pgfx[3] = (((tmp[ 8]>>1)&1)<<0) | (((tmp[ 8]>>3)&1)<<1) | (((tmp[ 8]>>5)&1)<<2) | (((tmp[ 8]>>7)&1)<<3);
			pgfx[4] = (((tmp[16]>>0)&1)<<0) | (((tmp[16]>>2)&1)<<1) | (((tmp[16]>>4)&1)<<2) | (((tmp[16]>>6)&1)<<3);
			pgfx[5] = (((tmp[16]>>1)&1)<<0) | (((tmp[16]>>3)&1)<<1) | (((tmp[16]>>5)&1)<<2) | (((tmp[16]>>7)&1)<<3);
			pgfx[6] = (((tmp[24]>>0)&1)<<0) | (((tmp[24]>>2)&1)<<1) | (((tmp[24]>>4)&1)<<2) | (((tmp[24]>>6)&1)<<3);
			pgfx[7] = (((tmp[24]>>1)&1)<<0) | (((tmp[24]>>3)&1)<<1) | (((tmp[24]>>5)&1)<<2) | (((tmp[24]>>7)&1)<<3);
					
			tmp += 1;
			pgfx += 8;
		}
		tmp += 24;
	}
	
	free(tmp);
	return 0;
}

static int loadDecodeGfx02()
{
/*
	16,16,
	RGN_FRAC(1,5),
	5,
	{ 0x800000*8, 0x600000*8, 0x400000*8, 0x200000*8, 0x000000*8 },
	{ 0,1,2,3,4,5,6,7,16*8+0,16*8+1,16*8+2,16*8+3,16*8+4,16*8+5,16*8+6,16*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },
	16*16
*/
	unsigned char * tmp1 = (unsigned char *) malloc (0xA00000);
	unsigned char * tmp2 = tmp1 + 0x200000;
	unsigned char * tmp3 = tmp2 + 0x200000;
	unsigned char * tmp4 = tmp3 + 0x200000;
	unsigned char * tmp5 = tmp4 + 0x200000;
	unsigned char * pgfx = RomGfx02;
	
	if (tmp1 == NULL) return 1;
	
	BurnLoadRom(tmp1,  6, 1);
	BurnLoadRom(tmp2,  7, 1);
	BurnLoadRom(tmp3,  8, 1);
	BurnLoadRom(tmp4,  9, 1);
	BurnLoadRom(tmp5, 10, 1);

//	TODO: be lazy to research how BurnProgresser work, so ...

//	BurnSetProgressRange(1.0);
	
	for (int i=0; i<(0x200000/32); i++) {
		for( int y=0;y<16;y++) {
			
			for(int x=0;x<8;x++) {
				pgfx[(7-x)+0] =	(((tmp1[ 0] >> x) & 0x01) << 0) | 
								(((tmp2[ 0] >> x) & 0x01) << 1) | 
								(((tmp3[ 0] >> x) & 0x01) << 2) | 
								(((tmp4[ 0] >> x) & 0x01) << 3) | 
								(((tmp5[ 0] >> x) & 0x01) << 4);
				pgfx[(7-x)+8] =	(((tmp1[16] >> x) & 0x01) << 0) | 
								(((tmp2[16] >> x) & 0x01) << 1) | 
								(((tmp3[16] >> x) & 0x01) << 2) | 
								(((tmp4[16] >> x) & 0x01) << 3) | 
								(((tmp5[16] >> x) & 0x01) << 4);
			}
			tmp1 += 1;
			tmp2 += 1;
			tmp3 += 1;
			tmp4 += 1;
			tmp5 += 1;
			pgfx += 16;
		}
		
//		if ((i & 0xFFF) == 0)
//			BurnUpdateProgress( 1.0 / 16, i ? NULL : _T("Decodeing graphics..."), 1);
		
		tmp1 += 16;
		tmp2 += 16;
		tmp3 += 16;
		tmp4 += 16;
		tmp5 += 16;
	}

	free(tmp1);
	return 0;
}

static int loadDecodeGfx03()
{
/*
	16,16,
	RGN_FRAC(1,3),
	6,
	{ 0x000000*8+8, 0x000000*8+0, 0x100000*8+8, 0x100000*8+0, 0x200000*8+8, 0x200000*8+0 },
	{ 0, 1, 2, 3, 4, 5, 6, 7,16*16+0,16*16+1,16*16+2,16*16+3,16*16+4,16*16+5,16*16+6,16*16+7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16, 8*16,9*16,10*16,11*16,12*16,13*16,14*16,15*16 },
	64*8
*/	
	unsigned char * tmp1 = (unsigned char *) malloc (0x300000);
	unsigned char * tmp2 = tmp1 + 0x100000;
	unsigned char * tmp3 = tmp2 + 0x100000;
	unsigned char * pgfx = RomGfx03;
	
	if (tmp1 == NULL) return 1;

	BurnLoadRom(tmp1, 11, 1);
	BurnLoadRom(tmp2, 12, 1);
	BurnLoadRom(tmp3, 13, 1);
	
	for (int i=0; i<(0x100000/64); i++) {
		for( int y=0;y<16;y++) {
			
			for(int x=0;x<8;x++) {
				pgfx[(7-x)+0] =	(((tmp3[ 0] >> x) & 0x01) << 0) | (((tmp3[ 1] >> x) & 0x01) << 1) | 
								(((tmp2[ 0] >> x) & 0x01) << 2) | (((tmp2[ 1] >> x) & 0x01) << 3) | 
								(((tmp1[ 0] >> x) & 0x01) << 4) | (((tmp1[ 1] >> x) & 0x01) << 5);
				pgfx[(7-x)+8] =	(((tmp3[32] >> x) & 0x01) << 0) | (((tmp3[33] >> x) & 0x01) << 1) | 
								(((tmp2[32] >> x) & 0x01) << 2) | (((tmp2[33] >> x) & 0x01) << 3) | 
								(((tmp1[32] >> x) & 0x01) << 4) | (((tmp1[33] >> x) & 0x01) << 5);
			}
			tmp1 += 2;
			tmp2 += 2;
			tmp3 += 2;
			pgfx += 16;
		}
		tmp1 += 32;
		tmp2 += 32;
		tmp3 += 32;
	}
	
	free(tmp1);
	
	return 0;
}

static int shadfrceInit()
{
	int nRet;
	
	Mem = NULL;
	MemIndex();
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex();	
	
	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000000, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000001, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x080000, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x080001, 3, 2); if (nRet != 0) return 1;
	
	nRet = BurnLoadRom(RomZ80 + 0x000000, 4, 1); if (nRet != 0) return 1;
	
	loadDecodeGfx01();
	loadDecodeGfx02();
	loadDecodeGfx03();

	BurnLoadRom(MSM6295ROM, 14, 1);	

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x0FFFFF, SM_ROM);	// CPU 0 ROM

		SekMapMemory((unsigned char *)RamBg00,
									0x100000, 0x101FFF, SM_RAM);	// b ground 0
		SekMapMemory((unsigned char *)RamBg01,		
									0x102000, 0x103FFF, SM_RAM);	// b ground 1
		SekMapMemory((unsigned char *)RamFg,			
									0x140000, 0x141FFF, SM_RAM);	// f ground
		SekMapMemory((unsigned char *)RamSpr,
									0x142000, 0x143FFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0x180000, 0x187FFF, SM_ROM);	// palette
		SekMapMemory(Ram68K,		0x1F0000, 0x1FFFFF, SM_RAM);	// 68K RAM

		SekMapHandler(1,			0x180000, 0x187FFF, SM_WRITE);
		
		
		SekSetReadWordHandler(0, shadfrceReadWord);
		SekSetReadByteHandler(0, shadfrceReadByte);
		SekSetWriteWordHandler(0, shadfrceWriteWord);
		SekSetWriteByteHandler(0, shadfrceWriteByte);
		
		//SekSetWriteByteHandler(1, shadfrceWriteBytePalette);
		SekSetWriteWordHandler(1, shadfrceWriteWordPalette);

		SekClose();
	}

	{
		ZetInit(1);
		ZetOpen(0);
	
		ZetSetReadHandler(shadfrceZRead);
		ZetSetWriteHandler(shadfrceZWrite);
		//ZetSetInHandler(shadfrceZIn);
		//ZetSetOutHandler(shadfrceZOut);
	
		// ROM bank 1
		ZetMapArea(0x0000, 0xBFFF, 0, RomZ80);
		ZetMapArea(0x0000, 0xBFFF, 2, RomZ80);

		// RAM
		ZetMapArea(0xC000, 0xC7FF, 0, RamZ80);
		ZetMapArea(0xC000, 0xC7FF, 1, RamZ80);
		ZetMapArea(0xC000, 0xC7FF, 2, RamZ80);

		ZetMapArea(0xF000, 0xFFFF, 0, RamZ80+0x0800);
		ZetMapArea(0xF000, 0xFFFF, 1, RamZ80+0x0800);
		ZetMapArea(0xF000, 0xFFFF, 2, RamZ80+0x0800);
		
		ZetMemEnd();
		ZetClose();
	}
	
	BurnYM2151Init(3579545, 50.0);		// 3.5795 MHz
	YM2151SetIrqHandler(0, &shadfrceYM2151IRQHandler);
	MSM6295Init(0, 12000, 50.0, 1);		// 12.000 KHz
	nZ80Cycles = 3579545 * 100 / nBurnFPS;

	DrvDoReset();
	return 0;
}

static int shadfrceExit()
{
	MSM6295Exit(0);
	BurnYM2151Exit();
	
	SekExit();
	ZetExit();
	
	free(Mem);
	Mem = NULL;
	
	return 0;
}

#define	TILE_BG0(x)				\
	if (d[x]) { p[x]=pal[d[x]|c]; pp[x]=2; }

#define	TILE_BG0_FLIPX(x)		\
	if (d[x]) { p[15-x]=pal[d[x]|c]; pp[15-x]=2; }

#define	TILE_BG0_E(z)			\
	if (d[z]&&(x+z)>=0 && (x+z)<320) { p[z]=pal[d[z]|c]; pp[z]=2; }

#define	TILE_BG0_FLIPX_E(z)		\
	if (d[z]&&(x+15-z)>=0&&(x+15-z)<320) { p[15-z]=pal[d[z]|c]; pp[15-z]=2; }

#define	TILE_BG0_LINE			\
	TILE_BG0( 0)				\
	TILE_BG0( 1)				\
	TILE_BG0( 2)				\
	TILE_BG0( 3)				\
	TILE_BG0( 4)				\
	TILE_BG0( 5)				\
	TILE_BG0( 6)				\
	TILE_BG0( 7)				\
	TILE_BG0( 8)				\
	TILE_BG0( 9)				\
	TILE_BG0(10)				\
	TILE_BG0(11)				\
	TILE_BG0(12)				\
	TILE_BG0(13)				\
	TILE_BG0(14)				\
	TILE_BG0(15)

#define	TILE_BG0_FLIPX_LINE		\
	TILE_BG0_FLIPX( 0)			\
	TILE_BG0_FLIPX( 1)			\
	TILE_BG0_FLIPX( 2)			\
	TILE_BG0_FLIPX( 3)			\
	TILE_BG0_FLIPX( 4)			\
	TILE_BG0_FLIPX( 5)			\
	TILE_BG0_FLIPX( 6)			\
	TILE_BG0_FLIPX( 7)			\
	TILE_BG0_FLIPX( 8)			\
	TILE_BG0_FLIPX( 9)			\
	TILE_BG0_FLIPX(10)			\
	TILE_BG0_FLIPX(11)			\
	TILE_BG0_FLIPX(12)			\
	TILE_BG0_FLIPX(13)			\
	TILE_BG0_FLIPX(14)			\
	TILE_BG0_FLIPX(15)

#define	TILE_BG0_LINE_E						\
	if (((y+k)>=0) && ((y+k)<256)) {		\
		TILE_BG0_E( 0)						\
		TILE_BG0_E( 1)						\
		TILE_BG0_E( 2)						\
		TILE_BG0_E( 3)						\
		TILE_BG0_E( 4)						\
		TILE_BG0_E( 5)						\
		TILE_BG0_E( 6)						\
		TILE_BG0_E( 7)						\
		TILE_BG0_E( 8)						\
		TILE_BG0_E( 9)						\
		TILE_BG0_E(10)						\
		TILE_BG0_E(11)						\
		TILE_BG0_E(12)						\
		TILE_BG0_E(13)						\
		TILE_BG0_E(14)						\
		TILE_BG0_E(15)						\
	}

#define	TILE_BG0_FLIPX_LINE_E				\
	if (((y+k)>=0) && ((y+k)<256)) {		\
		TILE_BG0_FLIPX_E( 0)				\
		TILE_BG0_FLIPX_E( 1)				\
		TILE_BG0_FLIPX_E( 2)				\
		TILE_BG0_FLIPX_E( 3)				\
		TILE_BG0_FLIPX_E( 4)				\
		TILE_BG0_FLIPX_E( 5)				\
		TILE_BG0_FLIPX_E( 6)				\
		TILE_BG0_FLIPX_E( 7)				\
		TILE_BG0_FLIPX_E( 8)				\
		TILE_BG0_FLIPX_E( 9)				\
		TILE_BG0_FLIPX_E(10)				\
		TILE_BG0_FLIPX_E(11)				\
		TILE_BG0_FLIPX_E(12)				\
		TILE_BG0_FLIPX_E(13)				\
		TILE_BG0_FLIPX_E(14)				\
		TILE_BG0_FLIPX_E(15)				\
	}

static void tileBackground_0()
{
	/* ---- ----  YXcc CCCC  --TT TTTT TTTT TTTT */
	
	unsigned short * pal = RamCurPal + 0x2000;
	int offs, mx, my, x, y;
	
	mx = -1;
	my = 0;
	for (offs = 0; offs < 32*32*2; offs+=2) {
		mx++;
		if (mx == 32) {
			mx = 0;
			my++;
		}

		x = mx * 16 - bg0scrollx;
		if (x <= (320-512)) x += 512;
		
		y = my * 16 - bg0scrolly;
		if (y <= (256-512)) y += 512;
		
		if ( x<=-16 || x>=320 || y<=-16 || y>= 256 ) 
			continue;
		else 
		if ( x >=0 && x <= (320-16) && y >= 0 && y <= (256-16)) {

			unsigned int tileno = RamBg00[offs+1] & 0x3FFF;
			if (tileno == 0) continue;
			
 			unsigned int c = (RamBg00[offs] & 0x1F);
 			if (c & 0x10) c ^= 0x30;	// skip hole 
 			c <<= 6;
 			
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 320 + x;
			unsigned char *d = RomGfx03 + (tileno << 8);
			
			unsigned char * pp = RamPri + y * 320 + x;
			
			if (RamBg00[offs] & 0x0080) {
				p += 320 * 15;
				pp += 320 * 15;
				if (RamBg00[offs] & 0x0040) {
	 				for (int k=0;k<16;k++) {
	 					
		 				TILE_BG0_FLIPX_LINE
		
		 				d += 16;
		 				p -= 320;
		 				pp -= 320;
		 			}
	 			} else {
					for (int k=0;k<16;k++) {
		 				
		 				TILE_BG0_LINE
		
		 				d += 16;
		 				p -= 320;
		 				pp -= 320;
		 			}
	 			}
 			} else {
 				if (RamBg00[offs] & 0x0040) {
	 				for (int k=0;k<16;k++) {
	 					
		 				TILE_BG0_FLIPX_LINE
		
		 				d += 16;
		 				p += 320;
		 				pp += 320;
		 			}
	 			} else {
					for (int k=0;k<16;k++) {
						
		 				TILE_BG0_LINE

		 				d += 16;
		 				p += 320;
		 				pp += 320;
		 			}
	 			}			
 			}
 			
		} else {

			unsigned int tileno = RamBg00[offs+1] & 0x3FFF;
			if (tileno == 0) continue;
			
 			unsigned int c = (RamBg00[offs] & 0x1F);
 			if (c & 0x10) c ^= 0x30;	// skip hole
 			c <<= 6;
 			
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 320 + x;
			unsigned char *d = RomGfx03 + (tileno << 8);
			unsigned char * pp = RamPri + y * 320 + x;
			
			if (RamBg00[offs] & 0x0080) {
				p += 320 * 15;
				pp += 320 * 15;
				if (RamBg00[offs] & 0x0040) {
	 				for (int k=0;k<16;k++) {
	 					
		 				TILE_BG0_FLIPX_LINE_E
		 						
		 				d += 16;
		 				p -= 320;
		 				pp -= 320;
		 			}
	 			} else {
					for (int k=0;k<16;k++) {
		 				
		 				TILE_BG0_LINE_E
		
		 				d += 16;
		 				p -= 320;
		 				pp -= 320;
		 			}
	 			}
 			} else {
 				if (RamBg00[offs] & 0x0040) {
	 				for (int k=0;k<16;k++) {
	 					
		 				TILE_BG0_FLIPX_LINE_E
		
		 				d += 16;
		 				p += 320;
		 				pp += 320;
		 			}
	 			} else {
					for (int k=0;k<16;k++) {
						
		 				TILE_BG0_LINE_E

		 				d += 16;
		 				p += 320;
		 				pp += 320;
		 			}
	 			}			
 			}
			
		}
	}
	
}

static void tileBackground_1()
{
	/* cccc TTTT TTTT TTTT */
	
	unsigned short * pal = RamCurPal + 0x2000;
	int offs, mx, my, x, y;
	
	mx = -1;
	my = 0;
	for (offs = 0; offs < 32*32; offs++) {
		mx++;
		if (mx == 32) {
			mx = 0;
			my++;
		}

		x = mx * 16 - bg1scrollx;
		if (x <= (320-512)) x += 512;
		
		y = my * 16 - bg1scrolly;
		if (y <= (256-512)) y += 512;
		
		if ( x<=-16 || x>=320 || y<=-16 || y>= 256 ) 
			continue;
		else
		if ( x >=0 && x <= (320-16) && y >= 0 && y <= (256-16)) {

			unsigned int tileno = RamBg01[offs] & 0x0FFF;
 			unsigned int c = ((RamBg01[offs] & 0xF000) >> 6) + (64 << 6);
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 320 + x;
			unsigned char *d = RomGfx03 + (tileno << 8);
			
			for (int k=0;k<16;k++) {
 				p[ 0] = pal[ d[ 0] | c ];
 				p[ 1] = pal[ d[ 1] | c ];
 				p[ 2] = pal[ d[ 2] | c ];
 				p[ 3] = pal[ d[ 3] | c ];
 				p[ 4] = pal[ d[ 4] | c ];
 				p[ 5] = pal[ d[ 5] | c ];
 				p[ 6] = pal[ d[ 6] | c ];
 				p[ 7] = pal[ d[ 7] | c ];
 				p[ 8] = pal[ d[ 8] | c ];
 				p[ 9] = pal[ d[ 9] | c ];
 				p[10] = pal[ d[10] | c ];
 				p[11] = pal[ d[11] | c ];
 				p[12] = pal[ d[12] | c ];
 				p[13] = pal[ d[13] | c ];
 				p[14] = pal[ d[14] | c ];
 				p[15] = pal[ d[15] | c ];

 				d += 16;
 				p += 320;
 			}
		} else {
			unsigned int tileno = RamBg01[offs] & 0x0FFF;
 			unsigned int c = ((RamBg01[offs] & 0xF000) >> 6) + (64 << 6);
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 320 + x;
			unsigned char *d = RomGfx03 + (tileno << 8);
			
			for (int k=0;k<16;k++) {
				if ( (y+k)>=0 && (y+k)<256 ) {
	 				if ((x +  0) >= 0 && (x +  0)<320) p[ 0] = pal[ d[ 0] | c ];
	 				if ((x +  1) >= 0 && (x +  1)<320) p[ 1] = pal[ d[ 1] | c ];
	 				if ((x +  2) >= 0 && (x +  2)<320) p[ 2] = pal[ d[ 2] | c ];
	 				if ((x +  3) >= 0 && (x +  3)<320) p[ 3] = pal[ d[ 3] | c ];
	 				if ((x +  4) >= 0 && (x +  4)<320) p[ 4] = pal[ d[ 4] | c ];
	 				if ((x +  5) >= 0 && (x +  5)<320) p[ 5] = pal[ d[ 5] | c ];
	 				if ((x +  6) >= 0 && (x +  6)<320) p[ 6] = pal[ d[ 6] | c ];
	 				if ((x +  7) >= 0 && (x +  7)<320) p[ 7] = pal[ d[ 7] | c ];
	 				if ((x +  8) >= 0 && (x +  8)<320) p[ 8] = pal[ d[ 8] | c ];
	 				if ((x +  9) >= 0 && (x +  9)<320) p[ 9] = pal[ d[ 9] | c ];
	 				if ((x + 10) >= 0 && (x + 10)<320) p[10] = pal[ d[10] | c ];
	 				if ((x + 11) >= 0 && (x + 11)<320) p[11] = pal[ d[11] | c ];
	 				if ((x + 12) >= 0 && (x + 12)<320) p[12] = pal[ d[12] | c ];
	 				if ((x + 13) >= 0 && (x + 13)<320) p[13] = pal[ d[13] | c ];
	 				if ((x + 14) >= 0 && (x + 14)<320) p[14] = pal[ d[14] | c ];
	 				if ((x + 15) >= 0 && (x + 15)<320) p[15] = pal[ d[15] | c ];
	 			}
 				d += 16;
 				p += 320;
 			}
		}
	}
}

static void tileForeground()
{
	/* ---- ----  tttt tttt  ---- ----  pppp TTTT */
	
	unsigned short * pal = RamCurPal;
	int offs, mx, my, x, y;
	
	mx = -1;
	my = 0;
	for (offs = 0; offs < 64*32*2; offs+=2) {
		mx++;
		if (mx == 64) {
			mx = 0;
			my++;
		}

		x = mx * 8;
		y = my * 8;
		
		if ( x<=-8 || x>=320 || y<=-8 || y>= 256 ) 
			continue;
		else
		if ( x >=0 && x < (320-8) && y >= 0 && y < (256-8)) {

			unsigned int tileno = (RamFg[offs] & 0x00FF) | ((RamFg[offs+1] & 0x000F) << 8);
			if (tileno == 0) continue;
			
 			unsigned int c = (RamFg[offs+1] & 0x00F0) << 2;
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 320 + x;
			unsigned char *d = RomGfx01 + (tileno << 6);
			
			for (int k=0;k<8;k++) {
 				if (d[0]) p[0] = pal[d[0]|c];
 				if (d[1]) p[1] = pal[d[1]|c];
 				if (d[2]) p[2] = pal[d[2]|c];
 				if (d[3]) p[3] = pal[d[3]|c];
 				if (d[4]) p[4] = pal[d[4]|c];
 				if (d[5]) p[5] = pal[d[5]|c];
 				if (d[6]) p[6] = pal[d[6]|c];
 				if (d[7]) p[7] = pal[d[7]|c];

 				d += 8;
 				p += 320;
 			}
		} else {

			unsigned int tileno = (RamFg[offs] & 0x00FF) | ((RamFg[offs+1] & 0x000F) << 8);
			if (tileno == 0) continue;
			
 			unsigned int c = (RamFg[offs+1] & 0x00F0) << 2;
 			unsigned short * p = (unsigned short *) pBurnDraw + y * 320 + x;
			unsigned char *d = RomGfx01 + (tileno << 6);
			
			for (int k=0;k<8;k++) {
				if ( (y+k)>=0 && (y+k)<256 ) {
	 				if ((x + 0) >= 0 && (x + 0)<320) p[0] = pal[ d[0] | c ];
	 				if ((x + 1) >= 0 && (x + 1)<320) p[1] = pal[ d[1] | c ];
	 				if ((x + 2) >= 0 && (x + 2)<320) p[2] = pal[ d[2] | c ];
	 				if ((x + 3) >= 0 && (x + 3)<320) p[3] = pal[ d[3] | c ];
	 				if ((x + 4) >= 0 && (x + 4)<320) p[4] = pal[ d[4] | c ];
	 				if ((x + 5) >= 0 && (x + 5)<320) p[5] = pal[ d[5] | c ];
	 				if ((x + 6) >= 0 && (x + 6)<320) p[6] = pal[ d[6] | c ];
	 				if ((x + 7) >= 0 && (x + 7)<320) p[7] = pal[ d[7] | c ];
	 			}
 				d += 8;
 				p += 320;
 			}
		}
	}
}

#define TILE_SPR_NORMAL(x)				\
	if (q[x]) { if((pp[x]&(0x05|pri))==0) p[x]=pal[q[x]|color]; pp[x]|=(1<<pri); }

#define TILE_SPR_FLIP_X(x)				\
	if (q[x]) { if((pp[15-x]&(0x05|pri))==0) p[15-x]=pal[q[x]|color]; pp[15-x]|=(1<<pri); }

#define TILE_SPR_NORMAL_E(x)			\
	if (q[x]&&((sx+x)>=0)&&((sx+x)<320)) { if((pp[x]&(0x05|pri))==0) p[x]=pal[q[x]|color]; pp[x]|=(1<<pri); }

#define TILE_SPR_FLIP_X_E(x)			\
	if (q[x]&&((sx+15-x)>=0)&&((sx+15-x)<320)) { if((pp[15-x]&(0x05|pri))==0) p[15-x]=pal[q[x]|color]; pp[15-x]|=(1<<pri); }

#define TILE_SPR_NORMAL_LINE			\
	TILE_SPR_NORMAL( 0)					\
	TILE_SPR_NORMAL( 1)					\
	TILE_SPR_NORMAL( 2)					\
	TILE_SPR_NORMAL( 3)					\
	TILE_SPR_NORMAL( 4)					\
	TILE_SPR_NORMAL( 5)					\
	TILE_SPR_NORMAL( 6)					\
	TILE_SPR_NORMAL( 7)					\
	TILE_SPR_NORMAL( 8)					\
	TILE_SPR_NORMAL( 9)					\
	TILE_SPR_NORMAL(10)					\
	TILE_SPR_NORMAL(11)					\
	TILE_SPR_NORMAL(12)					\
	TILE_SPR_NORMAL(13)					\
	TILE_SPR_NORMAL(14)					\
	TILE_SPR_NORMAL(15)

#define TILE_SPR_FLIP_X_LINE			\
	TILE_SPR_FLIP_X( 0)					\
	TILE_SPR_FLIP_X( 1)					\
	TILE_SPR_FLIP_X( 2)					\
	TILE_SPR_FLIP_X( 3)					\
	TILE_SPR_FLIP_X( 4)					\
	TILE_SPR_FLIP_X( 5)					\
	TILE_SPR_FLIP_X( 6)					\
	TILE_SPR_FLIP_X( 7)					\
	TILE_SPR_FLIP_X( 8)					\
	TILE_SPR_FLIP_X( 9)					\
	TILE_SPR_FLIP_X(10)					\
	TILE_SPR_FLIP_X(11)					\
	TILE_SPR_FLIP_X(12)					\
	TILE_SPR_FLIP_X(13)					\
	TILE_SPR_FLIP_X(14)					\
	TILE_SPR_FLIP_X(15)

#define TILE_SPR_NORMAL_LINE_E			\
	TILE_SPR_NORMAL_E( 0)				\
	TILE_SPR_NORMAL_E( 1)				\
	TILE_SPR_NORMAL_E( 2)				\
	TILE_SPR_NORMAL_E( 3)				\
	TILE_SPR_NORMAL_E( 4)				\
	TILE_SPR_NORMAL_E( 5)				\
	TILE_SPR_NORMAL_E( 6)				\
	TILE_SPR_NORMAL_E( 7)				\
	TILE_SPR_NORMAL_E( 8)				\
	TILE_SPR_NORMAL_E( 9)				\
	TILE_SPR_NORMAL_E(10)				\
	TILE_SPR_NORMAL_E(11)				\
	TILE_SPR_NORMAL_E(12)				\
	TILE_SPR_NORMAL_E(13)				\
	TILE_SPR_NORMAL_E(14)				\
	TILE_SPR_NORMAL_E(15)

#define TILE_SPR_FLIP_X_LINE_E			\
	TILE_SPR_FLIP_X_E( 0)				\
	TILE_SPR_FLIP_X_E( 1)				\
	TILE_SPR_FLIP_X_E( 2)				\
	TILE_SPR_FLIP_X_E( 3)				\
	TILE_SPR_FLIP_X_E( 4)				\
	TILE_SPR_FLIP_X_E( 5)				\
	TILE_SPR_FLIP_X_E( 6)				\
	TILE_SPR_FLIP_X_E( 7)				\
	TILE_SPR_FLIP_X_E( 8)				\
	TILE_SPR_FLIP_X_E( 9)				\
	TILE_SPR_FLIP_X_E(10)				\
	TILE_SPR_FLIP_X_E(11)				\
	TILE_SPR_FLIP_X_E(12)				\
	TILE_SPR_FLIP_X_E(13)				\
	TILE_SPR_FLIP_X_E(14)				\
	TILE_SPR_FLIP_X_E(15)

static void pdrawgfx(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy,int pri)
{
	unsigned short * p	= (unsigned short *) pBurnDraw;
	unsigned char * pp = RamPri;
	unsigned char * q	= RomGfx02 + (code << 8);
	unsigned short *pal	= RamCurPal + 0x1000;

	p += sy * 320 + sx;
	pp += sy * 320 + sx;
		
	if (sx < 0 || sx >= (320-16) || sy < 0 || sy >= (256-16) ) {
		
		if ((sx <= -16) || (sx >= 320) || (sy <= -16) || (sy >= 256))
			return;
			
		if (flipy) {
		
			p += 320 * 15;
			pp += 320 * 15;
			
			if (flipx) {
			
				for (int i=15;i>=0;i--) {
					if (((sy+i)>=0) && ((sy+i)<256)) {
						
						TILE_SPR_FLIP_X_LINE_E
						
					}
					p -= 320;
					pp -= 320;
					q += 16;
				}	
			
			} else {
	
				for (int i=15;i>=0;i--) {
					if (((sy+i)>=0) && ((sy+i)<256)) {
						
						TILE_SPR_NORMAL_LINE_E

					}
					p -= 320;
					pp -= 320;
					q += 16;
				}		
			}
			
		} else {
			
			if (flipx) {
			
				for (int i=0;i<16;i++) {
					if (((sy+i)>=0) && ((sy+i)<256)) {
						
						TILE_SPR_FLIP_X_LINE_E
						
					}
					p += 320;
					pp += 320;
					q += 16;
				}		
			
			} else {
	
				for (int i=0;i<16;i++) {
					if (((sy+i)>=0) && ((sy+i)<256)) {

						TILE_SPR_NORMAL_LINE_E

					}
					p += 320;
					pp += 320;
					q += 16;
				}	
	
			}
			
		}
		
		return;
	}

	if (flipy) {
		
		p += 320 * 15;
		pp += 320 * 15;
		
		if (flipx) {
		
			for (int i=0;i<16;i++) {
				
				TILE_SPR_FLIP_X_LINE
	
				p -= 320;
				pp -= 320;
				q += 16;
			}	
		
		} else {

			for (int i=0;i<16;i++) {
				
				TILE_SPR_NORMAL_LINE
	
				p -= 320;
				pp -= 320;
				q += 16;
			}		
		}
		
	} else {
		
		if (flipx) {
		
			for (int i=0;i<16;i++) {
				
				TILE_SPR_FLIP_X_LINE
	
				p += 320;
				pp += 320;
				q += 16;
			}		
		
		} else {

			for (int i=0;i<16;i++) {
				
				TILE_SPR_NORMAL_LINE
	
				p += 320;
				pp += 320;
				q += 16;
			}	

		}
		
	}

}

static void drawSprites()
{
	/* | ---- ---- hhhf Fe-Y | ---- ---- yyyy yyyy | ---- ---- TTTT TTTT | ---- ---- tttt tttt |
       | ---- ---- -pCc cccX | ---- ---- xxxx xxxx | ---- ---- ---- ---- | ---- ---- ---- ---- | */

	unsigned short * finish = RamSpr;
	unsigned short * source = finish + 0x2000/2 - 8;
	int hcount;
	
	while( source>=finish ) {
		int ypos = 0x100 - (((source[0] & 0x0003) << 8) | (source[1] & 0x00ff));
		int xpos = (((source[4] & 0x0001) << 8) | (source[5] & 0x00ff)) + 1;
		int tile = ((source[2] & 0x00ff) << 8) | (source[3] & 0x00ff);
		int height = (source[0] & 0x00e0) >> 5;
		int enable = ((source[0] & 0x0004));
		int flipx = ((source[0] & 0x0010) >> 4);
		int flipy = ((source[0] & 0x0008) >> 3);
		int pal = ((source[4] & 0x003e));
		int pri_mask = (source[4] & 0x0040) ? 0x02 : 0x00;
			
//		if ( (1 << pri_mask) & nSpriteEnable) {
			
			if (pal & 0x20) pal ^= 0x60;	// skip hole
			pal <<= 5;
			
			height++;
			if (enable)	{
				for (hcount=0;hcount<height;hcount++) {
					pdrawgfx(tile+hcount,pal,flipx,flipy,xpos,ypos-hcount*16-16,pri_mask);
					pdrawgfx(tile+hcount,pal,flipx,flipy,xpos-0x200,ypos-hcount*16-16,pri_mask);
					pdrawgfx(tile+hcount,pal,flipx,flipy,xpos,ypos-hcount*16-16+0x200,pri_mask);
					pdrawgfx(tile+hcount,pal,flipx,flipy,xpos-0x200,ypos-hcount*16-16+0x200,pri_mask);
				}
			}
			
//		}
		source-=8;
	}

}

static void DrvDraw()
{
	memset( RamPri, 0, 320*256);

//	if (nBurnLayer & 2)
	tileBackground_1();
		
//	if (nBurnLayer & 4)
	tileBackground_0();
	
	drawSprites();
	
//	if (nBurnLayer & 8)
	tileForeground();
}

static int shadfrceFrame()
{
	if (DrvReset)														// Reset machine
		DrvDoReset();
		
	if (bRecalcPalette) {
		for(int i=0;i<0x4000;i++) CalcCol(i);
		bRecalcPalette = 0;
	}

	DrvInput[0] = 0x00;													// Joy1
	DrvInput[2] = 0x00;													// Joy2
	DrvInput[4] = 0x00;													// Buttons
	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvJoy1[i] & 1) << i;
		DrvInput[2] |= (DrvJoy2[i] & 1) << i;
		DrvInput[4] |= (DrvButton[i] & 1) << i;
	}
	DrvInput[1] = (DrvInput[1] & 0xFC) | (DrvDipBtn[0] & 1) | ((DrvDipBtn[1] & 1) << 1); 

	SekNewFrame();
	ZetNewFrame();

	SekOpen(0);
	ZetOpen(0);
	
/*	
	005344	rts
	005346	btst    #$2, $1d0026.l
	00534e	bne     5346
	005350	clr.w   $1d0016.l
	005356	btst    #$2, $1d0026.l
	00535e	beq     5356
	005360	movea.l #$142000, A0
	
	0053b2	dbra    D0, 53b0
	0053b6	btst    #$2, $1d0026.l
	0053be	bne     53b6
	0053c0	clr.w   $1d0016.l
	0053c6	btst    #$2, $1d0026.l
	0053ce	beq     53c6
	0053d0	jmp     (A6)
	
	it will check v-blink dip (in 1d0016.l)
	
 */			
 	
 	int nSoundBufferPos = 0;
 	
	bVBlink = 1;
	SekRun(3500000 / 60);
	
	ZetRun(nZ80Cycles >> 2);
	if (pBurnSoundOut) {
		int nSegmentLength = (nBurnSoundLen * 1 / 4) - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		BurnYM2151Render(pSoundBuf, nSegmentLength);
		MSM6295Render(0, pSoundBuf, nSegmentLength);
		nSoundBufferPos += nSegmentLength;
	}
	
	bVBlink = 0;
	SekRun(3500000 / 60);
	SekSetIRQLine(2, SEK_IRQSTATUS_AUTO);

	ZetRun(nZ80Cycles >> 2);
	if (pBurnSoundOut) {
		int nSegmentLength = (nBurnSoundLen * 2 / 4) - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		BurnYM2151Render(pSoundBuf, nSegmentLength);
		MSM6295Render(0, pSoundBuf, nSegmentLength);
		nSoundBufferPos += nSegmentLength;
	}

	bVBlink = 1;
	SekRun(3500000 / 60);
	
	ZetRun(nZ80Cycles >> 2);
	if (pBurnSoundOut) {
		int nSegmentLength = (nBurnSoundLen * 3 / 4) - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		BurnYM2151Render(pSoundBuf, nSegmentLength);
		MSM6295Render(0, pSoundBuf, nSegmentLength);
		nSoundBufferPos += nSegmentLength;
	}
	
	
	bVBlink = 0;
	SekRun(3500000 / 60);
	SekSetIRQLine(3, SEK_IRQSTATUS_AUTO);
	
	ZetRun(nZ80Cycles >> 2);
	if (pBurnSoundOut) {
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		BurnYM2151Render(pSoundBuf, nSegmentLength);
		MSM6295Render(0, pSoundBuf, nSegmentLength);
		nSoundBufferPos += nSegmentLength;
	}

/*
	ZetRun(nZ80Cycles);
	if (pBurnSoundOut) {
		BurnYM2151Render(pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	}
*/

	ZetClose();
	SekClose();
	
	if (pBurnDraw)
		DrvDraw();												// Draw screen if needed
	
	

	return 0;
}

static int shadfrceScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin)						// Return minimum compatible version
		*pnMin =  0x029671;

	if (nAction & ACB_MEMORY_RAM) {								// Scan all memory, devices & variables
		memset(&ba, 0, sizeof(ba));
    	ba.Data	  = RamStart;
		ba.nLen	  = RamEnd-RamStart;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {
		SekScan(nAction);										// Scan 68000 state
		ZetScan(nAction);

		MSM6295Scan(0, nAction);
		BurnYM2151Scan(nAction);
		
		SCAN_VAR(DrvInput);
		SCAN_VAR(nBrightness);
		SCAN_VAR(bg0scrollx);
		SCAN_VAR(bg0scrolly);
		SCAN_VAR(bg1scrollx);
		SCAN_VAR(bg1scrolly);
		SCAN_VAR(nSoundlatch);
	}
	
	if (nAction & ACB_WRITE) {
		// recalc palette and brightness
		for(int i=0;i<0x4000;i++) CalcCol(i);
	}
	
	return 0;
}

struct BurnDriver BurnDrvShadfrce = {
	"shadfrce", NULL, NULL, "1993",
	"Shadow Force (US Version 2)\0", NULL, "Technos Japan", "misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_MISC_POST90S,
	NULL, shadfrceRomInfo, shadfrceRomName, shadfrceInputInfo, shadfrceDIPInfo,
	shadfrceInit, shadfrceExit, shadfrceFrame, NULL, shadfrceScan, 0, NULL, NULL, NULL, &bRecalcPalette, 
	320, 256, 4, 3
};

struct BurnDriver BurnDrvShadfrcj = {
	"shadfrcj", "shadfrce", NULL, "1993",
	"Shadow Force (Japan Version 2)\0", NULL, "Technos Japan", "misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY | BDF_CLONE, 2, HARDWARE_MISC_POST90S,
	NULL, shadfrcjRomInfo, shadfrcjRomName, shadfrceInputInfo, shadfrceDIPInfo,
	shadfrceInit, shadfrceExit, shadfrceFrame, NULL, shadfrceScan, 0, NULL, NULL, NULL, &bRecalcPalette, 
	320, 256, 4, 3
};
