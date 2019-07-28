// DEC0 baddudes
#include "burnint.h"

#include "dec0.h"

static unsigned char baddudesReset         = 0;

static unsigned char *Mem                  = NULL;
static unsigned char *MemEnd               = NULL;
static unsigned char *RamStart             = NULL;
static unsigned char *RamEnd               = NULL;

static unsigned char *baddudesRom          = NULL;
static unsigned char *baddudesSubRom       = NULL;

static unsigned char *baddudesRam          = NULL;

static unsigned char *baddudesSubRam       = NULL;

static int nCyclesDone[2], nCyclesTotal[2];
static int nCyclesSegment;


static struct BurnInputInfo baddudesInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , dec_inputbits2 + 4, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , dec_inputbits2 + 2, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , dec_inputbits2 + 5, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , dec_inputbits2 + 3, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , dec_inputbits0 + 0, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , dec_inputbits0 + 1, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , dec_inputbits0 + 2, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , dec_inputbits0 + 3, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , dec_inputbits0 + 4, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , dec_inputbits0 + 5, "p1 fire 2" },

	{"P2 Up"             , BIT_DIGITAL  , dec_inputbits1 + 0, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , dec_inputbits1 + 1, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , dec_inputbits1 + 2, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , dec_inputbits1 + 3, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , dec_inputbits1 + 4, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , dec_inputbits1 + 5, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &baddudesReset        , "reset"     },
};

STDINPUTINFO(baddudes);

static struct BurnRomInfo baddudesRomDesc[] =
{
	{"baddudes.4", 0x10000, 0x4bf158a7, 0x10}, //  0	68000 Program Code
	{"baddudes.1", 0x10000, 0x74f5110c, 0x10}, //  1	68000 Program Code
	{"baddudes.6", 0x10000, 0x3ff8da57, 0x10}, //  2	68000 Program Code
	{"baddudes.3", 0x10000, 0xf8f2bd94, 0x10}, //  3	68000 Program Code
	{"baddudes.7", 0x08000, 0x9fb1ef4b, 0x10}, // 6502 code
	{"baddudes.25",  0x08000, 0xbcf59a69, 0x10}, // text roms
	{"baddudes.26",  0x08000, 0x9aff67b8, 0x10},  
	{ "baddudes.18", 0x10000, 0x05cfc3e5, 0x10}, // tiles
	{ "baddudes.20", 0x10000, 0xe11e988f, 0x10},  
	{ "baddudes.22", 0x10000, 0xb893d880, 0x10}, 
	{ "baddudes.24", 0x10000, 0x6f226dda, 0x10}, 
	{ "baddudes.30", 0x10000, 0x982da0d1, 0x10}, 
	{ "baddudes.30", 0x10000, 0x982da0d1, 0x10}, 
	{ "baddudes.28", 0x10000, 0xf01ebb3b, 0x10}, 
	{ "baddudes.28", 0x10000, 0xf01ebb3b, 0x10}, 
	{ "baddudes.15", 0x10000, 0xa38a7d30, 0x10},  // spr
	{ "baddudes.16", 0x08000, 0x17e42633, 0x10},  
	{ "baddudes.11", 0x10000, 0x3a77326c, 0x10}, 
	{ "baddudes.12", 0x08000, 0xfea2a134, 0x10}, 
	{ "baddudes.13", 0x10000, 0xe5ae2751, 0x10}, 
	{ "baddudes.14", 0x08000, 0xe83c760a, 0x10}, 
	{ "baddudes.9", 0x10000, 0x6901e628, 0x10}, 
	{ "baddudes.10", 0x08000, 0xeeee8a1a, 0x10}, 
	{ "baddudes.8", 0x10000, 0x3c87463e, 0x10},  //adpcm

};

STD_ROM_PICK(baddudes);
STD_ROM_FN(baddudes);

int baddudesDoReset()
{
	SekOpen(0);
	SekReset();
	SekClose();
	return 0;
}

unsigned char __fastcall baddudesReadByte(unsigned int a)
{
	if (a>=0x180000 && a<=0x180fff)
	{
		printf("byte read from shared ram\n");
		return 0x00;	
	}


	if (a>=0x30c000 && a<=0x30c00b)
	{
		//	printf("byte read from dec0_controls_r %x\n",a-1);
		int offset = a - 0x30c000;
		return dec_controls_read(offset-1)&0xFF;//0x13;;	
	}


	printf("baddudesReadByte %x\n",a);
	return 0;
}

void __fastcall baddudesWriteByte(unsigned int a, unsigned char d)
{
	if (a>=0x180000 && a<=0x180fff)
	{
		printf("byte write to shared ram %x,%x\n",a,d);
	}

	if (a>=0x30c010 && a<=0x30c01f)
	{
		int offset = a - 0x30c010;
		dec_ctrl_writeword(offset-1,d);
		return;
	}

	printf("baddudesWriteByte %x,%x\n",a,d);
}

unsigned short __fastcall baddudesReadWord(unsigned int a)
{
	/*	if (a>=0x180000 && a<=0x180fff)
	{
	printf("word read from shared ram %x\n",a);
	return 0xff;	
	}

	if (a>=0x300000 && a<=0x30001f)
	{
	printf("word read from dec0_rotary_r %x\n",a);
	return 0xff;	
	}
	*/
	if (a>=0x30c000 && a<=0x30c00b)
	{
		//	printf("word read from dec0_controls_r %x\n",a);
		int offset = a - 0x30c000;
		return dec_controls_read(offset);	
	}

	printf("baddudesReadWord %x\n",a);
	return 0;
}


void __fastcall baddudesWriteWord(unsigned int a, unsigned short d)
{
	if (a>=0x180000 && a<=0x180fff)
	{
		printf("word write to shared ram %x,%x\n",a,d);
	}

	if (a>=0x242000 && a<=0x24207f)
	{
		//	printf("word write to dec0_pf1_colscroll\n",a,d);
		return;
	}

	if (a>=0x242400 && a<=0x2427ff)
	{
		//		printf("word write to dec0_pf1_rowscroll\n",a,d);
		return;
	}



	if (a>=0x30c010 && a<=0x30c01f)
	{
		int offset = a - 0x30c010;
		dec_ctrl_writeword(offset,d);
		return;
	}

	printf("baddudesWriteWord %x,%x\n",a,d);

}

static int MemIndex()
{
	unsigned char *Next; Next = Mem;
	baddudesRom          = Next; Next += 0x60000;
	baddudesSubRom       = Next; Next += 0x10000;

	RamStart = Next;
	baddudesRam          = Next; Next += 0x1000000;
	baddudesSubRam       = Next; Next += 0x00800;
	RamEnd = Next;

	MemEnd = Next;
	return 0;
}

int baddudesInit()
{
	int nRet = 0, nLen;

	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	decvid_init();

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(baddudesRom + 0x00001, 0, 2); 
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesRom + 0x00000, 1, 2); 
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesRom + 0x40001, 2, 2); 
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesRom + 0x40000, 3, 2); 
	if (nRet != 0) return 1;

	// load and decode char roms	
	unsigned char* baddudesTemp = (unsigned char*) malloc(0xC00000);

	memset(baddudesTemp, 0 , sizeof(baddudesTemp));

	nRet = BurnLoadRom(baddudesTemp,5,1);
	if (nRet != 0) return 1;

	nRet = BurnLoadRom(baddudesTemp+ 0x08000,6,1);
	if (nRet != 0) 
		return 1;


	decodechars(robocopChars,baddudesTemp,0x20000,2048 );

	//load and decode BG1 roms
	for (int w =0 ; w<=4 ; w++)
	{
		BurnLoadRom(baddudesTemp+ (0x10000 * w),7+w,1);	
	}

	decodeBG1(robocopBG1,baddudesTemp,2048 );
	
	//load and decode BG2 roms
	for (int w =0 ; w<=4 ; w++)
	{
		BurnLoadRom(baddudesTemp+ (0x8000 * w),11+w,1);	
	}


	BurnLoadRom(baddudesTemp+ 0x8000,11,1);
	memcpy(baddudesTemp,baddudesTemp + (0x8000*2),0x8000);

	BurnLoadRom(baddudesTemp+ (0x8000*3),13,1);

	memcpy(baddudesTemp+(0x8000*2),baddudesTemp + (0x8000*4),0x8000);

	decodeBG2(robocopBG2,baddudesTemp,1024 );

	// load and decode sprite roms
	nRet = BurnLoadRom(baddudesTemp ,15,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x10000,16,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x18000,17,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x28000,18,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x30000,19,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x40000,20,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x48000,21,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(baddudesTemp+ 0x58000,22,1);
	if (nRet != 0) return 1;

	decodesprite(robocopSprites,baddudesTemp,3072 );


	free(baddudesTemp);

	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(baddudesRom       , 0x000000, 0x05ffff, SM_ROM);
	SekMapMemory(baddudesRam+0x242800, 0x242800, 0x243fff, SM_RAM);

	SekMapMemory(dec0_pf1_control_0	, 0x240000, 0x240007, SM_RAM);
	SekMapMemory(dec0_pf1_control_1	, 0x240010, 0x240017, SM_RAM);
	SekMapMemory(dec0_pf1_colscroll	, 0x242000, 0x24207f, SM_RAM);
	SekMapMemory(dec0_pf1_rowscroll	, 0x242400, 0x2427ff, SM_RAM);
	SekMapMemory(dec0_pf1_data, 0x244000, 0x245fff, SM_RAM); //dec0_pf1_data

	SekMapMemory(dec0_pf2_control_0	, 0x246000, 0x246007, SM_RAM);
	SekMapMemory(dec0_pf2_control_1	, 0x246010, 0x246017, SM_RAM);
	SekMapMemory(dec0_pf2_colscroll	, 0x248000, 0x24807f, SM_RAM);
	SekMapMemory(dec0_pf2_rowscroll	, 0x248400, 0x2487ff, SM_RAM);	
	SekMapMemory(dec0_pf2_data, 0x24a000, 0x24a7ff, SM_RAM); //dec0_pf2_data

	SekMapMemory(dec0_pf3_control_0	, 0x24c000, 0x24c007, SM_RAM);
	SekMapMemory(dec0_pf3_control_1	, 0x24c010, 0x24c017, SM_RAM);
	SekMapMemory(dec0_pf3_colscroll	, 0x24c800, 0x24c87f, SM_RAM);
	SekMapMemory(dec0_pf3_rowscroll	, 0x24cc00, 0x24cfff, SM_RAM);	
	SekMapMemory(dec0_pf3_data, 0x24d000, 0x24d7ff, SM_RAM); //dec0_pf3_data

	SekMapMemory(robocopPaletteRam1, 0x310000, 0x3107ff, SM_RAM);
	SekMapMemory(robocopPaletteRam2, 0x314000, 0x3147ff, SM_RAM);
	SekMapMemory(baddudesRam+0xff8000, 0xff8000, 0xffbfff, SM_RAM); // dec0_ram
	SekMapMemory(robocopSpriteRam, 0xffc000, 0xffc7ff, SM_RAM);
	SekSetReadWordHandler(0, baddudesReadWord);
	SekSetWriteWordHandler(0, baddudesWriteWord);
	SekSetReadByteHandler(0, baddudesReadByte);
	SekSetWriteByteHandler(0, baddudesWriteByte);
	SekClose();


	DecSharedInit();
	init_dec0_aud(4,23);
	// Reset the driver
	baddudesDoReset();

	return 0;
}

int baddudesExit()
{
	exit_dec0_aud();
	DecSharedExit();
	SekExit();
	free(Mem);
	Mem = NULL;
	decvid_exit();
	return 0;
}


void baddudesRender()
{
//	int trans = 0;
	BurnTransferClear();

	decvid_calcpal();
	BurnClearScreen();


	if ((dec_pri_reg & 0x01) == 0)
	{
		decvid_drawbg1();

		decvid_drawsprites(0,0);

		decvid_drawbg2();
	}
	else
	{
		decvid_drawbg2();
		//if (dec_pri_reg & 0x02)
			decvid_drawsprites(0,0);
		decvid_drawbg1();
	}

	//decvid_drawsprites(0x00,0x00);
	decvid_drawchars();

	BurnTransferCopy(robocopPalette);
}

int baddudesFrame()
{
	int nInterleave = 4;

	if (baddudesReset)
		baddudesDoReset();

	DecMakeInputs();

	nCyclesTotal[0] = 10000000 / 60;
	nCyclesTotal[1] = 4000000 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

//	int nSoundBufferPos = 0;

	SekNewFrame();

	SekOpen(0);
	m6502Open(0);
	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run 68000
		nCurrentCPU = 0;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);

		
		// m6502
		nCurrentCPU = 1;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += 	m6502Run(nCyclesSegment);

		render_dec_aud(nInterleave, nCyclesTotal[0]);
	}
	endframe_dec_aud(nCyclesTotal[0]);
	m6502SetIRQ(M6502_IRQ);
	SekSetIRQLine(6, SEK_IRQSTATUS_AUTO);

	VBL_ACK = false;
	SekClose();
	m6502Close();
	if (pBurnDraw)
		baddudesRender();

	return 0;
}

static int baddudesScan(int /*nAction*/,int */*pnMin*/)
{

	return 0;
}

struct BurnDriverD BurnDrvbaddudes = {
	"baddudes", NULL, NULL, "1988",
		"Bad Dudes vs. Dragonninja (US)\0", NULL, "Data East USA", "DEC0",
		NULL, NULL, NULL, NULL,
		BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
		NULL, baddudesRomInfo, baddudesRomName, baddudesInputInfo, NULL,
		baddudesInit, baddudesExit, baddudesFrame, NULL, baddudesScan,
		0, NULL, NULL, NULL, NULL, 256, 256, 4, 3
};
