// Uo Poko
#include "cave.h"
#include "ymz280b.h"

#define CAVE_VBLANK_LINES 12

static unsigned char DrvJoy1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned short DrvInput[2] = {0x0000, 0x0000};

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *Rom01;
static unsigned char *Ram01;

static unsigned char DrvReset = 0;
static unsigned char bDrawScreen;
static bool bVBlank;

static char nVideoIRQ;
static char nSoundIRQ;
static char nUnknownIRQ;

static char nIRQPending;

static int nCurrentCPU;
static int nCyclesDone[2];
static int nCyclesTotal[2];
static int nCyclesSegment;

static struct BurnInputInfo uopokoInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 8,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},

	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0, 	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1, 	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2, 	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 3, 	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},
	{"P1 Button 3",	BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 3"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 8,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0, 	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 1, 	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2, 	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 3, 	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},
	{"P2 Button 3",	BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 3"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Diagnostics",	BIT_DIGITAL,	DrvJoy1 + 9,	"diag"},
	{"Service",		BIT_DIGITAL,	DrvJoy2 + 9,	"service"},
};

STDINPUTINFO(uopoko);

static void UpdateIRQStatus()
{
	nIRQPending = (nVideoIRQ == 0 || nSoundIRQ == 0 || nUnknownIRQ == 0);
	SekSetIRQLine(1, nIRQPending ? SEK_IRQSTATUS_ACK : SEK_IRQSTATUS_NONE);
}

unsigned char __fastcall uopokoReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x300003: {
			return YMZ280BReadStatus();
		}

		case 0x600000:
		case 0x600001:
		case 0x600002:
		case 0x600003: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0x600004:
		case 0x600005: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0x600006:
		case 0x600007: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0x900000:
			return (DrvInput[0] >> 8) ^ 0xFF;
		case 0x900001:
			return (DrvInput[0] & 0xFF) ^ 0xFF;
		case 0x900002:
			return (DrvInput[1] >> 8) ^ 0xF7 | (EEPROMRead() << 3);
		case 0x900003:
			return (DrvInput[1] & 0xFF) ^ 0xFF;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to read byte value of location %x\n", sekAddress);
		}
	}
	return 0;
}

unsigned short __fastcall uopokoReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x300002: {
			return YMZ280BReadStatus();
		}

		case 0x600000:
		case 0x600002: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0x600004: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0x600006: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0x900000:
			return DrvInput[0] ^ 0xFFFF;
		case 0x900002:
			return (DrvInput[1] ^ 0xF7FF) | (EEPROMRead() << 11);

		default: {
// 			bprintf(PRINT_NORMAL, "Attempt to read word value of location %x\n", sekAddress);
		}
	}
	return 0;
}

void __fastcall uopokoWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {
		case 0x300001:
			YMZ280BSelectRegister(byteValue);
			break;
		case 0x300003:
			YMZ280BWriteRegister(byteValue);
			break;

		case 0xA00000:
			EEPROMWrite(byteValue & 0x04, byteValue & 0x02, byteValue & 0x08);
			break;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write byte value %x to location %x\n", byteValue, sekAddress);
		}
	}
}

void __fastcall uopokoWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x300000:
			YMZ280BSelectRegister(wordValue);
			break;
		case 0x300002:
			YMZ280BWriteRegister(wordValue);
			break;

		case 0x600000:
			nCaveXOffset = wordValue;
			return;
		case 0x600002:
			nCaveYOffset = wordValue;
			return;
		case 0x600008:
			CaveSpriteBuffer();
			nCaveSpriteBank = wordValue;
			return;

		case 0x700000:
			CaveTileReg[0][0] = wordValue;
			break;
		case 0x700002:
			CaveTileReg[0][1] = wordValue;
			break;
		case 0x700004:
			CaveTileReg[0][2] = wordValue;
			break;

		case 0xA00000: {
			wordValue >>= 8;
			EEPROMWrite(wordValue & 0x04, wordValue & 0x02, wordValue & 0x08);
			break;
		}

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write word value %x to location %x\n", wordValue, sekAddress);

		}
	}
}

void __fastcall uopokoWriteBytePalette(unsigned int sekAddress, unsigned char byteValue)
{
	CavePalWriteByte(sekAddress & 0xFFFF, byteValue);
}

void __fastcall uopokoWriteWordPalette(unsigned int sekAddress, unsigned short wordValue)
{
	CavePalWriteWord(sekAddress & 0xFFFF, wordValue);
}

static void TriggerSoundIRQ(int nStatus)
{
	nSoundIRQ = nStatus ^ 1;
	UpdateIRQStatus();

	if (nIRQPending && nCurrentCPU != 0) {
		nCyclesDone[0] += SekRun(0x0400);
	}
}

static int DrvExit()
{
	YMZ280BExit();

	EEPROMExit();

	CaveTileExit();
	CaveSpriteExit();
    CavePalExit();

	SekExit();				// Deallocate 68000s

	// Deallocate all used memory
	free(Mem);
	Mem = NULL;

	return 0;
}

static int DrvDoReset()
{
	SekOpen(0);
	SekReset();
	SekClose();

	EEPROMReset();

	YMZ280BReset();

	nVideoIRQ = 1;
	nSoundIRQ = 1;
	nUnknownIRQ = 1;

	nIRQPending = 0;

	YMZ280BReset();

	return 0;
}

static int DrvDraw()
{
	CavePalUpdate8Bit(0, 128);				// Update the palette
	CaveClearScreen(CavePalette[0x7F00]);

	if (bDrawScreen) {
//		CaveGetBitmap();

		CaveTileRender(1);					// Render tiles
	}

	return 0;
}

inline static int CheckSleep(int)
{
	return 0;
}

static int DrvFrame()
{
	int nCyclesVBlank;
	int nInterleave = 8;

	if (DrvReset) {														// Reset machine
		DrvDoReset();
	}

	// Compile digital inputs
	DrvInput[0] = 0x0000;  												// Player 1
	DrvInput[1] = 0x0000;  												// Player 2
	for (int i = 0; i < 16; i++) {
		DrvInput[0] |= (DrvJoy1[i] & 1) << i;
		DrvInput[1] |= (DrvJoy2[i] & 1) << i;
	}
	CaveClearOpposites(&DrvInput[0]);
	CaveClearOpposites(&DrvInput[1]);

	SekNewFrame();

	nCyclesTotal[0] = (int)((long long)16000000 * nBurnCPUSpeedAdjust / (0x0100 * CAVE_REFRESHRATE));
	nCyclesDone[0] = 0;

	nCyclesVBlank = nCyclesTotal[0] - (int)((nCyclesTotal[0] * CAVE_VBLANK_LINES) / 271.5);
	bVBlank = false;

	int nSoundBufferPos = 0;

	SekOpen(0);

	for (int i = 1; i <= nInterleave; i++) {
		int nNext;

		// Render sound segment
		if ((i & 1) == 0) {
			if (pBurnSoundOut) {
				int nSegmentEnd = nBurnSoundLen * i / nInterleave;
				short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
				YMZ280BRender(pSoundBuf, nSegmentEnd - nSoundBufferPos);
				nSoundBufferPos = nSegmentEnd;
			}
		}

		// Run 68000
    	nCurrentCPU = 0;
		nNext = i * nCyclesTotal[nCurrentCPU] / nInterleave;

		// See if we need to trigger the VBlank interrupt
		if (!bVBlank && nNext > nCyclesVBlank) {
			if (nCyclesDone[nCurrentCPU] < nCyclesVBlank) {
				nCyclesSegment = nCyclesVBlank - nCyclesDone[nCurrentCPU];
				if (!CheckSleep(nCurrentCPU)) {							// See if this CPU is busywaiting
					nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
				} else {
					nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
				}
			}

			if (pBurnDraw != NULL) {
				DrvDraw();												// Draw screen if needed
			}

			bVBlank = true;
			nVideoIRQ = 0;
			UpdateIRQStatus();
		}

		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (!CheckSleep(nCurrentCPU)) {									// See if this CPU is busywaiting
			nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		} else {
			nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
		}

		nCurrentCPU = -1;
	}

	{
		// Make sure the buffer is entirely filled.
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (nSegmentLength) {
				YMZ280BRender(pSoundBuf, nSegmentLength);
			}
		}
	}

	SekClose();

	return 0;
}

// This routine is called first to determine how much memory is needed (MemEnd-(unsigned char *)0),
// and then afterwards to set up all the pointers
static int MemIndex()
{
	unsigned char* Next; Next = Mem;
	Rom01			= Next; Next += 0x100000;		// 68K program
	CaveSpriteROM	= Next; Next += 0x800000;
	CaveTileROM[0]	= Next; Next += 0x400000;		// Tile layer 0
	YMZ280BROM		= Next; Next += 0x200000;
	RamStart		= Next;
	Ram01			= Next; Next += 0x010000;		// CPU #0 work RAM
	CaveTileRAM[0]	= Next; Next += 0x008000;
	CaveSpriteRAM	= Next; Next += 0x010000;
	CavePalSrc		= Next; Next += 0x010000;		// palette
	RamEnd			= Next;
	MemEnd			= Next;

	return 0;
}

static void NibbleSwap1(unsigned char* pData, int nLen)
{
	unsigned char* pOrg = pData + nLen - 1;
	unsigned char* pDest = pData + ((nLen - 1) << 1);

	for (int i = 0; i < nLen; i++, pOrg--, pDest -= 2) {
		pDest[0] = *pOrg & 15;
		pDest[1] = *pOrg >> 4;
	}

	return;
}

static void NibbleSwap4(unsigned char* pData, int nLen)
{
	for (int i = 0; i < nLen; i++, pData += 2) {
		unsigned char n1 = pData[0];
		unsigned char n2 = pData[1];

		pData[1] = (n2 << 4) | (n1 & 0x0F);
		pData[0] = (n2 & 0xF0) | (n1 >> 4);
	}

	return;
}

static int LoadRoms()
{
	// Load 68000 ROM
	BurnLoadRom(Rom01 + 0, 1, 2);
	BurnLoadRom(Rom01 + 1, 0, 2);

	BurnLoadRom(CaveSpriteROM + 0x0000000, 2, 1);
	NibbleSwap1(CaveSpriteROM, 0x400000);

	BurnLoadRom(CaveTileROM[0] + 0x000000, 3, 1);
	NibbleSwap4(CaveTileROM[0], 0x200000);

	// Load YMZ280B data
	BurnLoadRom(YMZ280BROM, 4, 1);

	return 0;
}

// Scan ram
static int DrvScan(int nAction, int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {						// Return minimum compatible version
		*pnMin = 0x020902;
	}

	EEPROMScan(nAction, pnMin);			// Scan EEPROM

	if (nAction & ACB_VOLATILE) {		// Scan volatile ram

		memset(&ba, 0, sizeof(ba));
    	ba.Data		= RamStart;
		ba.nLen		= RamEnd - RamStart;
		ba.szName	= "RAM";
		BurnAcb(&ba);

		SekScan(nAction);				// scan 68000 states

		YMZ280BScan();

		SCAN_VAR(nVideoIRQ);
		SCAN_VAR(nSoundIRQ);
		SCAN_VAR(nUnknownIRQ);
		SCAN_VAR(bVBlank);

		CaveScanGraphics();

		SCAN_VAR(DrvInput);
	}

	return 0;
}

static int DrvInit()
{
	int nLen;

	BurnSetRefreshRate(CAVE_REFRESHRATE);

	// Find out how much memory is needed
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) {
		return 1;
	}
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex();													// Index the allocated memory

	EEPROMInit(1024, 16);										// EEPROM has 1024 bits, uses 16-bit words

	{
		unsigned char data[] = { 0x03, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08 };
		EEPROMFill(data, 0, 0x0A);
	}

	// Load the roms into memory
	if (LoadRoms()) {
		return 1;
	}

	{
		SekInit(0, 0x68000);												// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom01,					0x000000, 0x0FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram01,					0x100000, 0x10FFFF, SM_RAM);
		SekMapMemory(CaveSpriteRAM,			0x400000, 0x40FFFF, SM_RAM);
		SekMapMemory(CaveTileRAM[0],		0x500000, 0x507FFF, SM_RAM);

		SekMapMemory(CavePalSrc,			0x800000, 0x80FFFF, SM_ROM);	// Palette RAM (write goes through handler)
		SekMapHandler(1,					0x800000, 0x80FFFF, SM_WRITE);	//

		SekSetReadWordHandler(0, uopokoReadWord);
		SekSetReadByteHandler(0, uopokoReadByte);
		SekSetWriteWordHandler(0, uopokoWriteWord);
		SekSetWriteByteHandler(0, uopokoWriteByte);

		SekSetWriteWordHandler(1,uopokoWriteWordPalette);
		SekSetWriteByteHandler(1, uopokoWriteBytePalette);

		SekClose();
	}

	nCaveRowModeOffset = 1;

	CavePalInit(0x8000);
	CaveTileInit();
	CaveSpriteInit(1, 0x800000);
	CaveTileInitLayer(0, 0x400000, 8, 0x4000);

	YMZ280BInit(16934400, &TriggerSoundIRQ, 3);

	bDrawScreen = true;

	DrvDoReset(); // Reset machine

	return 0;
}

// Rom information
static struct BurnRomInfo uopokoRomDesc[] = {
	{ "u26.int",      0x080000, 0xb445c9ac, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "u25.int",      0x080000, 0xa1258482, BRF_ESS | BRF_PRG }, //  1

	{ "u33.bin",      0x400000, 0x5D142Ad2, BRF_GRA },			 //  2 Sprite data

	{ "u49.bin",      0x400000, 0x12FB11BB, BRF_GRA },			 //  3 Layer 0 Tile data

	{ "u4.bin",       0x200000, 0xA2D0D755, BRF_SND },			 //  4 YMZ280B (AD)PCM data
};


STD_ROM_PICK(uopoko);
STD_ROM_FN(uopoko);

static struct BurnRomInfo uopokojRomDesc[] = {
	{ "u26.bin",      0x080000, 0xE7EEC050, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "u25.bin",      0x080000, 0x68CB6211, BRF_ESS | BRF_PRG }, //  1

	{ "u33.bin",      0x400000, 0x5D142Ad2, BRF_GRA },			 //  2 Sprite data

	{ "u49.bin",      0x400000, 0x12FB11BB, BRF_GRA },			 //  3 Layer 0 Tile data

	{ "u4.bin",       0x200000, 0xA2D0D755, BRF_SND },			 //  4 YMZ280B (AD)PCM data
};


STD_ROM_PICK(uopokoj);
STD_ROM_FN(uopokoj);

struct BurnDriver BurnDrvUoPoko = {
	"uopoko", NULL, NULL, "1999",
	"Puzzle Uo Poko (International)\0", NULL, "Cave / Jaleco", "Cave",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY,
	NULL, uopokoRomInfo, uopokoRomName, uopokoInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 320, 240, 4, 3
};

struct BurnDriver BurnDrvUoPokoj = {
	"uopokoj", "uopoko", NULL, "1999",
	"Uo Poko (Japan, ver. 1998 Feb 06)\0", NULL, "Cave / Jaleco", "Cave",
	L"\u30D1\u30BA\u30EB - \u9B5A\u30DD\u30B3 (Japan, ver. 1998 Feb 06)\0Uo Poko (Japan, ver. 1998 Feb 06)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY,
	NULL, uopokojRomInfo, uopokojRomName, uopokoInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 320, 240, 4, 3
};



