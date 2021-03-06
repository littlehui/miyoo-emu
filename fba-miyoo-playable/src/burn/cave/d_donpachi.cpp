// Donpachi
#include "cave.h"
#include "msm6295.h"

#define CAVE_VBLANK_LINES 12

static unsigned char DrvJoy1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned short DrvInput[2] = {0x0000, 0x0000};

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *Rom01;
static unsigned char *Ram01;

static unsigned char DrvReset = 0;
static unsigned char bDrawScreen;
static bool bVBlank;

static int nBankSize[2] = {0x200000, 0x300000};

static char nVideoIRQ;
static char nSoundIRQ;
static char nUnknownIRQ;

static char nIRQPending;

static struct BurnInputInfo donpachiInputList[] = {
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

STDINPUTINFO(donpachi);

static void UpdateIRQStatus()
{
	nIRQPending = (nVideoIRQ == 0 || nSoundIRQ == 0 || nUnknownIRQ == 0);
	SekSetIRQLine(1, nIRQPending ? SEK_IRQSTATUS_ACK : SEK_IRQSTATUS_NONE);
}

unsigned char __fastcall donpachiReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		case 0x900000:
		case 0x900001:
		case 0x900002:
		case 0x900003: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0x900004:
		case 0x900005: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0x900006:
		case 0x900007: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0xB00001:
			return MSM6295ReadStatus(0);
		case 0xB00011:
			return MSM6295ReadStatus(1);

		case 0xC00000:
			return (DrvInput[0] >> 8) ^ 0xFF;
		case 0xC00001:
			return (DrvInput[0] & 0xFF) ^ 0xFF;
		case 0xC00002:
			return (DrvInput[1] >> 8) ^ 0xF7 | (EEPROMRead() << 3);
		case 0xC00003:
			return (DrvInput[1] & 0xFF) ^ 0xFF;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to read byte value of location %x\n", sekAddress);
		}
	}
	return 0;
}

unsigned short __fastcall donpachiReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x900000:
		case 0x900002: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}

		case 0x900004: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0x900006: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0xB00000:
			return MSM6295ReadStatus(0);
		case 0xB00010:
			return MSM6295ReadStatus(1);

		case 0xC00000:
			return DrvInput[0] ^ 0xFFFF;
		case 0xC00002:
			return DrvInput[1] ^ 0xF7FF | (EEPROMRead() << 11);

		default: {
// 			bprintf(PRINT_NORMAL, "Attempt to read word value of location %x\n", sekAddress);
		}
	}
	return 0;
}

void __fastcall donpachiWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		case 0xB00000:
		case 0xB00001:
		case 0xB00002:
		case 0xB00003:
			MSM6295Command(0, byteValue);
			break;
		case 0xB00010:
		case 0xB00011:
		case 0xB00012:
		case 0xB00013:
			MSM6295Command(1, byteValue);
			break;

		case 0xB00020:
		case 0xB00021:
		case 0xB00022:
		case 0xB00023:
		case 0xB00024:
		case 0xB00025:
		case 0xB00026:
		case 0xB00027:
		case 0xB00028:
		case 0xB00029:
		case 0xB0002A:
		case 0xB0002B:
		case 0xB0002C:
		case 0xB0002D:
		case 0xB0002E:
		case 0xB0002F: {
			int nBank = (sekAddress >> 1) & 3;
			int nChip = (sekAddress >> 3) & 1;
			int nAddress = byteValue << 16;
			while (nAddress > nBankSize[nChip]) {
				nAddress -= nBankSize[nChip];
			}

			if (nChip == 1) {
				MSM6295SampleData[1][nBank] = MSM6295ROM + nAddress;
				MSM6295SampleInfo[1][nBank] = MSM6295ROM + nAddress + (nBank << 8);
			} else {
				MSM6295SampleData[0][nBank] = MSM6295ROM + 0x100000 + nAddress;
				if (nBank == 0) {
					MSM6295SampleInfo[0][0] = MSM6295ROM + 0x100000 + nAddress + 0x0000;
					MSM6295SampleInfo[0][1] = MSM6295ROM + 0x100000 + nAddress + 0x0100;
					MSM6295SampleInfo[0][2] = MSM6295ROM + 0x100000 + nAddress + 0x0200;
					MSM6295SampleInfo[0][3] = MSM6295ROM + 0x100000 + nAddress + 0x0300;
				}
			}
			break;
		}

		case 0xD00000:
			EEPROMWrite(byteValue & 0x04, byteValue & 0x02, byteValue & 0x08);
			break;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write byte value %x to location %x\n", byteValue, sekAddress);
		}
	}
}

void __fastcall donpachiWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x600000:
			CaveTileReg[1][0] = wordValue;
			break;
		case 0x600002:
			CaveTileReg[1][1] = wordValue;
			break;
		case 0x600004:
			CaveTileReg[1][2] = wordValue;
			break;

		case 0x700000:
			CaveTileReg[0][0] = wordValue;
			break;
		case 0x700002:
			CaveTileReg[0][1] = wordValue;
			break;
		case 0x700004:
			CaveTileReg[0][2] = wordValue;
			break;

		case 0x800000:
			CaveTileReg[2][0] = wordValue;
			break;
		case 0x800002:
			CaveTileReg[2][1] = wordValue;
			break;
		case 0x800004:
			CaveTileReg[2][2] = wordValue;
			break;

		case 0x900000:
			nCaveXOffset = wordValue;
			return;
		case 0x900002:
			nCaveYOffset = wordValue;
			return;
		case 0x900008:
			CaveSpriteBuffer();
			nCaveSpriteBank = wordValue;
			return;

		case 0xB00000:
		case 0xB00002:
			MSM6295Command(0, wordValue);
			break;
		case 0xB00010:
		case 0xB00012:
			MSM6295Command(1, wordValue);
			break;

		case 0xB00020:
		case 0xB00021:
		case 0xB00022:
		case 0xB00023:
		case 0xB00024:
		case 0xB00025:
		case 0xB00026:
		case 0xB00027:
		case 0xB00028:
		case 0xB00029:
		case 0xB0002A:
		case 0xB0002B:
		case 0xB0002C:
		case 0xB0002D:
		case 0xB0002E:
		case 0xB0002F: {
			int nBank = (sekAddress >> 1) & 3;
			int nChip = (sekAddress >> 3) & 1;
			int nAddress = wordValue << 16;
			while (nAddress > nBankSize[nChip]) {
				nAddress -= nBankSize[nChip];
			}

			if (nChip == 1) {
				MSM6295SampleData[1][nBank] = MSM6295ROM + nAddress;
				MSM6295SampleInfo[1][nBank] = MSM6295ROM + nAddress + (nBank << 8);
			} else {
				MSM6295SampleData[0][nBank] = MSM6295ROM + 0x100000 + nAddress;
				if (nBank == 0) {
					MSM6295SampleInfo[0][0] = MSM6295ROM + 0x100000 + nAddress + 0x0000;
					MSM6295SampleInfo[0][1] = MSM6295ROM + 0x100000 + nAddress + 0x0100;
					MSM6295SampleInfo[0][2] = MSM6295ROM + 0x100000 + nAddress + 0x0200;
					MSM6295SampleInfo[0][3] = MSM6295ROM + 0x100000 + nAddress + 0x0300;
				}
			}
			break;
		}

		case 0xD00000:
			wordValue >>= 8;
			EEPROMWrite(wordValue & 0x04, wordValue & 0x02, wordValue & 0x08);
			break;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write word value %x to location %x\n", wordValue, sekAddress);

		}
	}
}

static int DrvExit()
{
	EEPROMExit();

	MSM6295Exit(1);
	MSM6295Exit(0);

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

	nVideoIRQ = 1;
	nSoundIRQ = 1;
	nUnknownIRQ = 1;

	nIRQPending = 0;

	MSM6295Reset(0);

	return 0;
}

static int DrvDraw()
{
	CavePalUpdate4Bit(0, 128);				// Update the palette
	CaveClearScreen(CavePalette[0x7F00]);

	if (bDrawScreen) {
//		CaveGetBitmap();

		CaveTileRender(1);					// Render tiles
	}

	return 0;
}

inline static int CheckSleep(int)
{
#if 1 && defined USE_SPEEDHACKS
	int nCurrentPC = SekGetPC(-1);

	// All versions are the same
	if (!nIRQPending && nCurrentPC >= 0x002ED6 && nCurrentPC <= 0x002EE2) {
		return 1;
	}
#endif

	return 0;
}

static int DrvFrame()
{
	int nCyclesVBlank;
	int nInterleave = 8;

	int nCyclesTotal[2];
	int nCyclesDone[2];

	int nCyclesSegment;

	if (DrvReset) {														// Reset machine
		DrvDoReset();
	}

	// Compile digital inputs
	DrvInput[0] = 0x0000;  												// Player 1
	DrvInput[1] = 0x0000;  												// Player 2
	for (int i = 0; i < 10; i++) {
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
    	int nCurrentCPU = 0;
		int nNext = i * nCyclesTotal[nCurrentCPU] / nInterleave;

		// Run 68000

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
	}

	// Make sure the buffer is entirely filled.
	{
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (nSegmentLength) {
				MSM6295Render(0, pSoundBuf, nSegmentLength);
				MSM6295Render(1, pSoundBuf, nSegmentLength);
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
	Rom01			= Next; Next += 0x080000;		// 68K program
	CaveSpriteROM	= Next; Next += 0x800000;
	CaveTileROM[0]	= Next; Next += 0x200000;		// Tile layer 0
	CaveTileROM[1]	= Next; Next += 0x200000;		// Tile layer 1
	CaveTileROM[2]	= Next; Next += 0x080000;		// Tile layer 2
	MSM6295ROM		= Next; Next += 0x300000;
	RamStart		= Next;
	Ram01			= Next; Next += 0x010000;		// CPU #0 work RAM
	CaveTileRAM[0]	= Next; Next += 0x008000;
	CaveTileRAM[1]	= Next; Next += 0x008000;
	CaveTileRAM[2]	= Next; Next += 0x008000;
	CaveSpriteRAM	= Next; Next += 0x010000;
	CavePalSrc		= Next; Next += 0x001000;		// palette
	RamEnd			= Next;
	MemEnd			= Next;

	return 0;
}

static void NibbleSwap2(unsigned char* pData, int nLen)
{
	unsigned char* pOrg = pData + nLen - 1;
	unsigned char* pDest = pData + ((nLen - 1) << 1);

	for (int i = 0; i < nLen; i++, pOrg--, pDest -= 2) {
		pDest[1] = *pOrg & 15;
		pDest[0] = *pOrg >> 4;
	}

	return;
}

static int LoadRoms()
{
	// Load 68000 ROM
	BurnLoadRom(Rom01, 0, 1);

	BurnLoadRom(CaveSpriteROM + 0x000000, 1, 1);
	BurnLoadRom(CaveSpriteROM + 0x200000, 2, 1);
	BurnByteswap(CaveSpriteROM, 0x400000);
	NibbleSwap2(CaveSpriteROM, 0x400000);

	BurnLoadRom(CaveTileROM[0], 3, 1);
	NibbleSwap2(CaveTileROM[0], 0x100000);
	BurnLoadRom(CaveTileROM[1], 4, 1);
	NibbleSwap2(CaveTileROM[1], 0x100000);
	BurnLoadRom(CaveTileROM[2], 5, 1);
	NibbleSwap2(CaveTileROM[2], 0x040000);

	// Load MSM6295 ADPCM data
	BurnLoadRom(MSM6295ROM, 6, 1);
	BurnLoadRom(MSM6295ROM + 0x100000, 7, 1);

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

		MSM6295Scan(0, nAction);
		MSM6295Scan(1, nAction);

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

	// Load the roms into memory
	if (LoadRoms()) {
		return 1;
	}

	{
		SekInit(0, 0x68000);													// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom01,						0x000000, 0x07FFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram01,						0x100000, 0x10FFFF, SM_RAM);
		SekMapMemory(CaveTileRAM[1],			0x200000, 0x207FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[0],			0x300000, 0x307FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2] + 0x4000,	0x400000, 0x403FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2] + 0x4000,	0x404000, 0x407FFF, SM_RAM);
		SekMapMemory(CaveSpriteRAM,				0x500000, 0x50FFFF, SM_RAM);
		SekMapMemory(CavePalSrc,				0xA08000, 0xA08FFF, SM_RAM);	// Palette RAM

		SekSetReadWordHandler(0, donpachiReadWord);
		SekSetReadByteHandler(0, donpachiReadByte);
		SekSetWriteWordHandler(0, donpachiWriteWord);
		SekSetWriteByteHandler(0, donpachiWriteByte);

		SekClose();
	}

	CavePalInit(0x8000);
	CaveTileInit();
	CaveSpriteInit(0, 0x0800000);
	CaveTileInitLayer(0, 0x200000, 8, 0x4000);
	CaveTileInitLayer(1, 0x200000, 8, 0x4000);
	CaveTileInitLayer(2, 0x080000, 8, 0x4000);

	MSM6295Init(0, 8000, 50.0, 0);
	MSM6295Init(1, 16000, 50.0, 0);

	MSM6295SampleData[0][0] = MSM6295ROM + 0x100000;
	MSM6295SampleInfo[0][0] = MSM6295ROM + 0x100000 + 0x0000;
	MSM6295SampleData[0][1] = MSM6295ROM + 0x100000;
	MSM6295SampleInfo[0][1] = MSM6295ROM + 0x100000 + 0x0100;
	MSM6295SampleData[0][2] = MSM6295ROM + 0x100000;
	MSM6295SampleInfo[0][2] = MSM6295ROM + 0x100000 + 0x0200;
	MSM6295SampleData[0][3] = MSM6295ROM + 0x100000;
	MSM6295SampleInfo[0][3] = MSM6295ROM + 0x100000 + 0x0300;

	bDrawScreen = true;

#if defined FBA_DEBUG && defined USE_SPEEDHACKS
	bprintf(PRINT_IMPORTANT, _T("  * Using speed-hacks (detecting idle loops).\n"));
#endif

	DrvDoReset(); // Reset machine

	return 0;
}

// Rom information
static struct BurnRomInfo donpachiRomDesc[] = {
	{ "prgu.u29",     0x080000, 0x89C36802, BRF_ESS | BRF_PRG }, //  0 CPU #0 code

	{ "atdp.u44",     0x200000, 0x7189E953, BRF_GRA },			 //  1 Sprite data
	{ "atdp.u45",     0x200000, 0x6984173F, BRF_GRA },			 //  2

	{ "atdp.u54",     0x100000, 0x6BDA6B66, BRF_GRA },			 //  3 Layer 0 Tile data
	{ "atdp.u57",     0x100000, 0x0A0E72B9, BRF_GRA },			 //  4 Layer 1 Tile data
	{ "text.u58",     0x040000, 0x5DBA06E7, BRF_GRA },			 //  5 Layer 2 Tile data

	{ "atdp.u32",     0x100000, 0x0D89FCCA, BRF_SND },			 //  6 MSM6295 #1 ADPCM data
	{ "atdp.u33",     0x200000, 0xD749DE00, BRF_SND },			 //  7 MSM6295 #0/1 ADPCM data
};


STD_ROM_PICK(donpachi);
STD_ROM_FN(donpachi);

static struct BurnRomInfo donpachjRomDesc[] = {
	{ "prg.u29",      0x080000, 0x6BE14AF6, BRF_ESS | BRF_PRG }, //  0 CPU #0 code

	{ "atdp.u44",     0x200000, 0x7189E953, BRF_GRA },			 //  1 Sprite data
	{ "atdp.u45",     0x200000, 0x6984173F, BRF_GRA },			 //  2

	{ "atdp.u54",     0x100000, 0x6BDA6B66, BRF_GRA },			 //  3 Layer 0 Tile data
	{ "atdp.u57",     0x100000, 0x0A0E72B9, BRF_GRA },			 //  4 Layer 1 Tile data
	{ "u58.bin",      0x040000, 0x285379FF, BRF_GRA },			 //  5 Layer 2 Tile data

	{ "atdp.u32",     0x100000, 0x0D89FCCA, BRF_SND },			 //  6 MSM6295 #1 ADPCM data
	{ "atdp.u33",     0x200000, 0xD749DE00, BRF_SND },			 //  7 MSM6295 #0/1 ADPCM data
};


STD_ROM_PICK(donpachj);
STD_ROM_FN(donpachj);

static struct BurnRomInfo donpackrRomDesc[] = {
	{ "prgk.u26",     0x080000, 0xBBAF4C8B, BRF_ESS | BRF_PRG }, //  0 CPU #0 code

	{ "atdp.u44",     0x200000, 0x7189E953, BRF_GRA },			 //  1 Sprite data
	{ "atdp.u45",     0x200000, 0x6984173F, BRF_GRA },			 //  2

	{ "atdp.u54",     0x100000, 0x6BDA6B66, BRF_GRA },			 //  3 Layer 0 Tile data
	{ "atdp.u57",     0x100000, 0x0A0E72B9, BRF_GRA },			 //  4 Layer 1 Tile data
	{ "u58.bin",      0x040000, 0x285379FF, BRF_GRA },			 //  5 Layer 2 Tile data

	{ "atdp.u32",     0x100000, 0x0D89FCCA, BRF_SND },			 //  6 MSM6295 #1 ADPCM data
	{ "atdp.u33",     0x200000, 0xD749DE00, BRF_SND },			 //  7 MSM6295 #0/1 ADPCM data
};


STD_ROM_PICK(donpackr);
STD_ROM_FN(donpackr);

static struct BurnRomInfo donpachkRomDesc[] = {
	{ "37.u29",       0x080000, 0x71f39f30, BRF_ESS | BRF_PRG }, //  0 CPU #0 code

	{ "atdp.u44",     0x200000, 0x7189E953, BRF_GRA },			 //  1 Sprite data
	{ "atdp.u45",     0x200000, 0x6984173F, BRF_GRA },			 //  2

	{ "atdp.u54",     0x100000, 0x6BDA6B66, BRF_GRA },			 //  3 Layer 0 Tile data
	{ "atdp.u57",     0x100000, 0x0A0E72B9, BRF_GRA },			 //  4 Layer 1 Tile data
	{ "u58.bin",      0x040000, 0x285379ff, BRF_GRA },			 //  5 Layer 2 Tile data

	{ "atdp.u32",     0x100000, 0x0D89FCCA, BRF_SND },			 //  6 MSM6295 #1 ADPCM data
	{ "atdp.u33",     0x200000, 0xD749DE00, BRF_SND },			 //  7 MSM6295 #0/1 ADPCM data
};


STD_ROM_PICK(donpachk);
STD_ROM_FN(donpachk);


struct BurnDriver BurnDrvDonpachi = {
	"donpachi", NULL, NULL, "1995",
	"DonPachi (ver. 1.01 1995/05/11, U.S.A)\0", NULL, "Atlus / Cave", "Cave",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY | HARDWARE_CAVE_M6295,
	NULL, donpachiRomInfo, donpachiRomName, donpachiInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};

struct BurnDriver BurnDrvDonpachj = {
	"donpacjp", "donpachi", NULL, "1995",
	"DonPachi (ver. 1.01 1995/05/11, Japan)\0", NULL, "Atlus / Cave", "Cave",
	L"DonPachi (ver. 1.01 1995/05/11, Japan)\0\u9996\u9818\u8702 (ver. 1.01 1995/05/11, Japan)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY | HARDWARE_CAVE_M6295,
	NULL, donpachjRomInfo, donpachjRomName, donpachiInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};

struct BurnDriver BurnDrvDonpachkr = {
	"donpackr", "donpachi", NULL, "1995",
	"DonPachi (ver. 1.12 1995/05/2x, Korea)\0", NULL, "Atlus / Cave", "Cave",
	L"DonPachi (ver. 1.01 1995/05/2x, Korea)\0\u9996\u9818\u8702 (ver. 1.01 1995/05/2x, Korea)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY | HARDWARE_CAVE_M6295,
	NULL, donpackrRomInfo, donpackrRomName, donpachiInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};

struct BurnDriver BurnDrvDonpachk = {
	"donpachk", "donpachi", NULL, "1995",
	"DonPachi (ver. 1.12 1995/05/2x, Hong Kong)\0", NULL, "Atlus / Cave", "Cave",
	L"DonPachi (ver. 1.01 1995/05/2x, Hong Kong)\0\u9996\u9818\u8702 (ver. 1.01 1995/05/2x, Hong Kong)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_ONLY | HARDWARE_CAVE_M6295,
	NULL, donpachkRomInfo, donpachkRomName, donpachiInputInfo, NULL,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};
