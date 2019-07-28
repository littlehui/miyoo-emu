// DEC0 robocop
#include "burnint.h"

#include "dec0.h"

static unsigned char robocopReset         = 0;

static unsigned char *Mem                  = NULL;
static unsigned char *MemEnd               = NULL;
static unsigned char *RamStart             = NULL;
static unsigned char *RamEnd               = NULL;

static unsigned char *robocopRom          = NULL;
static unsigned char *robocopSubRom       = NULL;

static unsigned char *robocopRam          = NULL;

static unsigned char *robocopSubRam       = NULL;

static int nCyclesDone[2], nCyclesTotal[2];
static int nCyclesSegment;


static struct BurnInputInfo robocopInputList[] = {
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

	{"Reset"             , BIT_DIGITAL  , &robocopReset        , "reset"     },
};

STDINPUTINFO(robocop);

/*
static struct BurnDIPInfo robocopDIPList[]=
{

};

STDDIPINFO(robocop);
*/
static struct BurnRomInfo robocopRomDesc[] =
{
	{"ep05-4.11c", 0x10000, 0x29c35379, 0x10}, //  0	68000 Program Code
	{"ep01-4.11b", 0x10000, 0x77507c69, 0x10}, //  1	68000 Program Code
	{"ep04-3", 0x10000, 0x39181778, 0x10}, //  2	68000 Program Code
	{"ep00-3", 0x10000, 0xe128541f, 0x10}, //  3	68000 Program Code

	{"ep03-3", 0x08000, 0x5b164b24, 0x10}, // 6502 code

	{"ep23",  0x10000, 0xa77e4ab1, 0x10}, // text roms
	{"ep22",  0x10000, 0x9fbd6903, 0x10},  

	{ "ep20", 0x10000, 0x1d8d38b8, 0x10}, // tiles
	{ "ep21", 0x10000, 0x187929b2, 0x10},  
	{ "ep18", 0x10000, 0xb6580b5e, 0x10}, 
	{ "ep19", 0x10000, 0x9bad01c7, 0x10}, 

	{ "ep14", 0x08000, 0xca56ceda, 0x10}, 
	{ "ep15", 0x08000, 0xa945269c, 0x10}, 
	{ "ep16", 0x08000, 0xe7fa4d58, 0x10}, 
	{ "ep17", 0x08000, 0x84aae89d, 0x10}, 

	{ "ep07", 0x10000, 0x495d75cf, 0x10},  // spr
	{ "ep06", 0x08000, 0xa2ae32e2, 0x10},  
	{ "ep11", 0x10000, 0x62fa425a, 0x10}, 
	{ "ep10", 0x08000, 0xcce3bd95, 0x10}, 
	{ "ep09", 0x10000, 0x11bed656, 0x10}, 
	{ "ep08", 0x08000, 0xc45c7b4c, 0x10}, 
	{ "ep13", 0x10000, 0x8fca9f28, 0x10}, 
	{ "ep12", 0x08000, 0x3cd1d0c3, 0x10}, 

	{ "ep02", 0x10000, 0x711ce46f, 0x10},  //adpcm

	{"en_24.a2", 0x200, 0xb8e2ca98, 0x10}, //Hu6280 code

};

STD_ROM_PICK(robocop);
STD_ROM_FN(robocop);

static struct BurnRomInfo robocopbRomDesc[] =
{
	{"robop_05.rom", 0x10000, 0xbcef3e9b, 0x10}, //  0	68000 Program Code
	{"robop_01.rom", 0x10000, 0xc9803685, 0x10}, //  1	68000 Program Code
	{"robop_04.rom", 0x10000, 0x9d7b79e0, 0x10}, //  2	68000 Program Code
	{"robop_00.rom", 0x10000, 0x80ba64ab, 0x10}, //  3	68000 Program Code

	{"ep03-3", 0x08000, 0x5b164b24, 0x10},//4  6502 code

	{"ep23",  0x10000, 0xa77e4ab1, 0x10}, //5 text roms
	{"ep22",  0x10000, 0x9fbd6903, 0x10},  

	{ "ep20", 0x10000, 0x1d8d38b8, 0x10}, //7 tiles
	{ "ep21", 0x10000, 0x187929b2, 0x10},  
	{ "ep18", 0x10000, 0xb6580b5e, 0x10}, 
	{ "ep19", 0x10000, 0x9bad01c7, 0x10}, 

	{ "ep14", 0x08000, 0xca56ceda, 0x10}, 
	{ "ep15", 0x08000, 0xa945269c, 0x10}, 
	{ "ep16", 0x08000, 0xe7fa4d58, 0x10}, 
	{ "ep17", 0x08000, 0x84aae89d, 0x10}, 

	{ "ep07", 0x10000, 0x495d75cf, 0x10},  //15 spr
	{ "ep06", 0x08000, 0xa2ae32e2, 0x10},  
	{ "ep11", 0x10000, 0x62fa425a, 0x10}, 
	{ "ep10", 0x08000, 0xcce3bd95, 0x10}, 
	{ "ep09", 0x10000, 0x11bed656, 0x10}, 
	{ "ep08", 0x08000, 0xc45c7b4c, 0x10}, 
	{ "ep13", 0x10000, 0x8fca9f28, 0x10}, 
	{ "ep12", 0x08000, 0x3cd1d0c3, 0x10}, 

	{ "ep02", 0x10000, 0x711ce46f, 0x10},  //adpcm

};

STD_ROM_PICK(robocopb);
STD_ROM_FN(robocopb);

int robocopDoReset()
{
	reset_dec0_aud();
	SekOpen(0);
	SekReset();
	SekClose();

	return 0;
}

unsigned char __fastcall robocopReadByte(unsigned int a)
{
	if (a>=0x180000 && a<=0x180fff)
	{
		printf("byte read from shared ram\n");
		return 0xff;	
	}


	if (a>=0x30c000 && a<=0x30c00b)
	{
		//	printf("byte read from dec0_controls_r %x\n",a-1);
		int offset = a - 0x30c000;
		return dec_controls_read(offset-1)&0xFF;	
	}


	printf("robocopReadByte %x\n",a);
	return 0;
}

void __fastcall robocopWriteByte(unsigned int a, unsigned char d)
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

	printf("robocopWriteByte %x,%x\n",a,d);
}

unsigned short __fastcall robocopReadWord(unsigned int a)
{
/*	if (a>=0x180000 && a<=0x180fff)
	{
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

	printf("robocopReadWord %x\n",a);
	return 0;
}


void __fastcall robocopWriteWord(unsigned int a, unsigned short d)
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

	printf("robocopWriteWord %x,%x\n",a,d);

}





static int MemIndex()
{
	unsigned char *Next; Next = Mem;
	robocopRom          = Next; Next += 0x60000;
	robocopSubRom       = Next; Next += 0x10000;

	RamStart = Next;
	robocopRam          = Next; Next += 0x1000000;
	robocopSubRam       = Next; Next += 0x00800;
	RamEnd = Next;

	MemEnd = Next;
	return 0;
}

int robocopInit()
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
	nRet = BurnLoadRom(robocopRom + 0x00001, 0, 2); 
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopRom + 0x00000, 1, 2); 
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopRom + 0x20001, 2, 2); 
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopRom + 0x20000, 3, 2); 
	if (nRet != 0) return 1;

	// load and decode char roms	
	unsigned char* robocopTemp = (unsigned char*) malloc(0xC00000);
	memset(robocopTemp, 0 , sizeof(robocopTemp));

	nRet = BurnLoadRom(robocopTemp,5,1);
	if (nRet != 0) 
		return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x10000,6,1);
	if (nRet != 0) 
		return 1;

	decodechars(robocopChars,robocopTemp,262144,4096 );

	//load and decode BG1 roms
	for (int w =0 ; w<=4 ; w++)
	{
		BurnLoadRom(robocopTemp+ (0x10000 * w),7+w,1);	
	}

	decodeBG1(robocopBG1,robocopTemp,2048 );

	//load and decode BG2 roms
	for (int w =0 ; w<=4 ; w++)
	{
		BurnLoadRom(robocopTemp+ (0x8000 * w),11+w,1);	
	}
	
	decodeBG2(robocopBG2,robocopTemp,1024 );
	
	// load and decode sprite roms
	nRet = BurnLoadRom(robocopTemp ,15,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x10000,16,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x18000,17,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x28000,18,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x30000,19,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x40000,20,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x48000,21,1);
	if (nRet != 0) return 1;
	nRet = BurnLoadRom(robocopTemp+ 0x58000,22,1);
	if (nRet != 0) return 1;

	decodesprite(robocopSprites,robocopTemp,3072 );

	free(robocopTemp);

	// Setup the 68000 emulation
	SekInit(0, 0x68000);
	SekOpen(0);
	SekMapMemory(robocopRom       , 0x000000, 0x05ffff, SM_ROM);
	SekMapMemory(robocopRam+0x242800, 0x242800, 0x243fff, SM_RAM);

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
	SekMapMemory(robocopRam+0xff8000, 0xff8000, 0xffbfff, SM_RAM); // dec0_ram
	SekMapMemory(robocopSpriteRam, 0xffc000, 0xffc7ff, SM_RAM);
	SekSetReadWordHandler(0, robocopReadWord);
	SekSetWriteWordHandler(0, robocopWriteWord);
	SekSetReadByteHandler(0, robocopReadByte);
	SekSetWriteByteHandler(0, robocopWriteByte);
	SekClose();

	DecSharedInit();
	init_dec0_aud(4,23);

	// Reset the driver
	robocopDoReset();

	return 0;
}

int robocopExit()
{
	exit_dec0_aud();
	DecSharedInit();
	SekExit();
	free(Mem);
	Mem = NULL;
	decvid_exit();
	return 0;
}

void robocopRender()
{
	int trans = 0;
	BurnTransferClear();


	decvid_calcpal();
	BurnClearScreen();

	if (dec_pri_reg & 0x04)
		trans = 0x08;
	else
		trans = 0x00;

	if (dec_pri_reg & 0x01)
	{

		decvid_drawbg1();

		if (dec_pri_reg & 0x02)
			decvid_drawsprites(0x08,trans);

		decvid_drawbg2();
	}
	else
	{
		decvid_drawbg2();
		if (dec_pri_reg & 0x02)
			decvid_drawsprites(0x08,trans);
		decvid_drawbg1();
	}

	if (dec_pri_reg & 0x02)
		decvid_drawsprites(0x08,trans ^ 0x08);
	else
		decvid_drawsprites(0x00,0x00);

	decvid_drawchars();

	BurnTransferCopy(robocopPalette);
}

int robocopFrame()
{
	int nInterleave = 20;

	if (robocopReset)
		robocopDoReset();

	DecMakeInputs();

	nCyclesTotal[0] = 10000000 / 60;
	nCyclesTotal[1] = 1500000 / 60;
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
		nCyclesDone[nCurrentCPU] += m6502Run(nCyclesSegment);

		render_dec_aud(nInterleave, nCyclesTotal[0]);
	}
	m6502SetIRQ(M6502_IRQ);
	SekSetIRQLine(6, SEK_IRQSTATUS_AUTO);

	endframe_dec_aud(nCyclesTotal[0]);

	VBL_ACK = false;
	SekClose();
	m6502Close();
	if (pBurnDraw)
		robocopRender();

	return 0;
}

static int robocopScan(int /*nAction*/,int */*pnMin*/)
{

	return 0;
}

struct BurnDriverD BurnDrvrobocop = {
	"robocop", NULL, NULL, "1988",
		"Robocop (World revision 4)\0", NULL, "Data East Corporation", "DEC0",
		NULL, NULL, NULL, NULL,
		BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
		NULL, robocopRomInfo, robocopRomName, robocopInputInfo, NULL,
		robocopInit, robocopExit, robocopFrame, NULL, robocopScan,
		0, NULL, NULL, NULL, NULL, 256, 256, 4, 3
};

struct BurnDriverD BurnDrvrobocopb = {
	"robocopb", "robocop", NULL, "1988",
		"Robocop (World bootleg)\0", NULL, "bootleg", "DEC0",
		NULL, NULL, NULL, NULL,
		BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S,
		NULL, robocopbRomInfo, robocopbRomName, robocopInputInfo, NULL,
		robocopInit, robocopExit, robocopFrame, NULL, robocopScan,
		0, NULL, NULL, NULL, NULL, 256, 256, 4, 3
};
