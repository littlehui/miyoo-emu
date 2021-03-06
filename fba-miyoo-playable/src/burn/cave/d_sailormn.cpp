// Pretty Soldier Sailor Moon & Air Gallet
#include "cave.h"
#include "msm6295.h"
#include "burn_ym2151.h"

#include "bitswap.h"

#define CAVE_VBLANK_LINES (271.5 - 240)
// #define CAVE_VBLANK_LINES (12)

static unsigned char DrvJoy1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned short DrvInput[2] = {0x0000, 0x0000};

static unsigned char drvRegion = 0;

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *Rom01, *Rom02, *RomZ80;
static unsigned char *Ram01, *Ram02, *Ram03, *RamZ80;

static unsigned char DrvReset = 0;
static unsigned char bDrawScreen;
static char nVBlank;

static int nCurrentBank;

static int SoundLatch;
static int SoundLatchReply[48];
static int SoundLatchStatus;

static int SoundLatchReplyIndex;
static int SoundLatchReplyMax;

static char nVideoIRQ;
static char nSoundIRQ;
static char nUnknownIRQ;

static int nCaveCyclesDone[2];

int nWhichGame;				// 0 - sailormn/sailormo
							// 1 - agallet

static struct BurnInputInfo sailormnInputList[] = {
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
	{"Region",		BIT_DIPSWITCH,	&drvRegion,		"dip"},
};

STDINPUTINFO(sailormn);

static struct BurnDIPInfo sailormnDIPList[] = {
	// Region
	{0x15,	0xFF, 0xFF,	0x02, NULL},
	{0,		0xFD, 0,	6,	  "Region"},
	{0x15,	0x01, 0x07, 0x00, "Japan"},
	{0x15,	0x01, 0x07, 0x01, "U.S.A."},
	{0x15,	0x01, 0x07, 0x02, "Europe"},
	{0x15,	0x01, 0x07, 0x03, "Hong Kong"},
	{0x15,	0x01, 0x07, 0x04, "Taiwan"},
	{0x15,	0x01, 0x07, 0x05, "Korea"},
	{0x15,	0x01, 0x07, 0x06, "Japan"},
	{0x15,	0x01, 0x07, 0x07, "Japan"},
};

STDDIPINFO(sailormn);

static void UpdateIRQStatus()
{
	int nIRQPending = (nVideoIRQ == 0 || nSoundIRQ == 0 || nUnknownIRQ == 0);
	SekSetIRQLine(1, nIRQPending ? SEK_IRQSTATUS_ACK : SEK_IRQSTATUS_NONE);
}

static void drvZ80Bankswitch(int nBank)
{
	nBank &= 0x1F;
	if (nBank != nCurrentBank) {
		unsigned char* nStartAddress = RomZ80 + (nBank << 14);
		ZetMapArea(0x4000, 0x7FFF, 0, nStartAddress);
		ZetMapArea(0x4000, 0x7FFF, 2, nStartAddress);
		nCurrentBank = nBank;
	}
}

static void drvYM2151IRQHandler(int nStatus)
{
	if (nStatus) {
		ZetRaiseIrq(255);
		nCaveCyclesDone[1] += ZetRun(0x0400);
	}
}

unsigned char __fastcall sailormnZIn(unsigned short nAddress)
{
	nAddress &= 0xFF;

	switch (nAddress) {
		case 0x20: {
//			bprintf(PRINT_NORMAL, "Z80 read thingie.\n");
//			int nStatus = SoundLatchStatus;
//			SoundLatchStatus &= ~2;
//			return nStatus;

			return 0;
		}

		case 0x30:
//			bprintf(PRINT_NORMAL, "Z80 read soundlatch(lo)\n");
			SoundLatchStatus |= 0x04;
			return SoundLatch & 0xFF;

		case 0x40:
//			bprintf(PRINT_NORMAL, "Z80 read soundlatch(hi)\n");
			SoundLatchStatus |= 0x08;
			return SoundLatch >> 8;

		case 0x51:
//			bprintf(PRINT_NORMAL, "YM2151 status read.\n");
			return BurnYM2151ReadStatus();
		case 0x60:
//			bprintf(PRINT_NORMAL, "MSM6295 #0 status read.\n");
			return MSM6295ReadStatus(0);
		case 0x80:
//			bprintf(PRINT_NORMAL, "MSM6295 #1 status read.\n");
			return MSM6295ReadStatus(1);
	}

	return 0;
}

void __fastcall sailormnZOut(unsigned short nAddress, unsigned char nValue)
{
	nAddress &= 0xFF;

	switch (nAddress) {

		case 0x00:
			drvZ80Bankswitch(nValue);
			break;

		case 0x10:
//			SoundLatchStatus |= 0x02;
//			SoundLatchReply = nValue;
			if (SoundLatchReplyIndex > SoundLatchReplyMax) {
				SoundLatchReplyMax = -1;
				SoundLatchReplyIndex = 0;
			}
			SoundLatchReplyMax++;
			SoundLatchReply[SoundLatchReplyMax] = nValue;
			break;

		case 0x50:
			BurnYM2151SelectRegister(nValue);
			break;
		case 0x51:
//			bprintf(PRINT_NORMAL, "YM2151 reg %02X -> %02X\n", CaveCurrentYM2151Register, nValue);
			BurnYM2151WriteRegister(nValue);
			break;

		case 0x60:
//			bprintf(PRINT_NORMAL, "MSM6295 #0 command sent.\n");
			MSM6295Command(0, nValue);
			break;
		case 0x70:
			MSM6295SampleInfo[0][0] = MSM6295ROM + ((nValue & 0x0F) << 17);
			MSM6295SampleInfo[0][1] = MSM6295ROM + ((nValue & 0x0F) << 17) + 0x0100;
			MSM6295SampleInfo[0][2] = MSM6295ROM + ((nValue & 0x0F) << 17) + 0x0200;
			MSM6295SampleInfo[0][3] = MSM6295ROM + ((nValue & 0x0F) << 17) + 0x0300;
			MSM6295SampleData[0][0] = MSM6295ROM + ((nValue & 0x0F) << 17);
			MSM6295SampleData[0][1] = MSM6295ROM + ((nValue & 0x0F) << 17) + 0x010000;
			MSM6295SampleData[0][2] = MSM6295ROM + ((nValue & 0xF0) << 13);
			MSM6295SampleData[0][3] = MSM6295ROM + ((nValue & 0xF0) << 13) + 0x010000;
			break;

		case 0x80:
//			bprintf(PRINT_NORMAL, "MSM6295 #1 command sent.\n");
			MSM6295Command(1, nValue);
			break;
		case 0xC0:
			MSM6295SampleInfo[1][0] = MSM6295ROM + 0x0200000 + ((nValue & 0x0F) << 17);
			MSM6295SampleInfo[1][1] = MSM6295ROM + 0x0200000 + ((nValue & 0x0F) << 17) + 0x0100;
			MSM6295SampleInfo[1][2] = MSM6295ROM + 0x0200000 + ((nValue & 0x0F) << 17) + 0x0200;
			MSM6295SampleInfo[1][3] = MSM6295ROM + 0x0200000 + ((nValue & 0x0F) << 17) + 0x0300;
			MSM6295SampleData[1][0] = MSM6295ROM + 0x0200000 + ((nValue & 0x0F) << 17);
			MSM6295SampleData[1][1] = MSM6295ROM + 0x0200000 + ((nValue & 0x0F) << 17) + 0x010000;
			MSM6295SampleData[1][2] = MSM6295ROM + 0x0200000 + ((nValue & 0xF0) << 13);
			MSM6295SampleData[1][3] = MSM6295ROM + 0x0200000 + ((nValue & 0xF0) << 13) + 0x010000;
			break;
	}
}

static int drvZInit()
{
	ZetInit(1);

	ZetSetInHandler(sailormnZIn);
	ZetSetOutHandler(sailormnZOut);

	// ROM bank 1
	ZetMapArea    (0x0000, 0x3FFF, 0, RomZ80 + 0x0000); // Direct Read from ROM
	ZetMapArea    (0x0000, 0x3FFF, 2, RomZ80 + 0x0000); // Direct Fetch from ROM
	// ROM bank 2
	ZetMapArea    (0x4000, 0x7FFF, 0, RomZ80 + 0x0000); // Direct Read from ROM
	ZetMapArea    (0x4000, 0x7FFF, 2, RomZ80 + 0x0000); //
	// RAM
	ZetMapArea    (0xC000, 0xDFFF, 0, RamZ80);			// Direct Read from RAM
	ZetMapArea    (0xC000, 0xDFFF, 1, RamZ80);			// Direct Write to RAM
	ZetMapArea    (0xC000, 0xDFFF, 2, RamZ80);			//
	// RAM mirror
	ZetMapArea    (0xE000, 0xFFFF, 0, RamZ80);			// Direct Read from RAM
	ZetMapArea    (0xE000, 0xFFFF, 1, RamZ80);			// Direct Write to RAM
	ZetMapArea    (0xE000, 0xFFFF, 2, RamZ80);			//

	ZetMemEnd();

	return 0;
}

unsigned char __fastcall sailormnReadByte(unsigned int sekAddress)
{
//	bprintf(PRINT_NORMAL, "Attempt to read byte value of location %x\n", sekAddress);

	switch (sekAddress) {
		case 0xB80000:
		case 0xB80001: {
			unsigned char nRet = ((nVBlank ^ 1) << 2) | (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0xB80002:
		case 0xB80003: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0xB80004:
		case 0xB80005: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0xB80006:
		case 0xB80007: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0x600000:
			return (DrvInput[0] >> 8) ^ 0xFF;
		case 0x600001:
			return (DrvInput[0] & 0xFF) ^ 0xFF;
		case 0x600002:
			return (DrvInput[1] >> 8) ^ 0xF7 | (EEPROMRead() << 3);
		case 0x600003:
			return (DrvInput[1] & 0xFF) ^ 0xFF;

#if 0
		case 0xB8006C:
		case 0xB8006D:
			bprintf(PRINT_NORMAL, "Soundlatch status read (byte).\n");
//			return SoundLatchStatus & 3;
			return 0;
		case 0xB8006E:
		case 0xB8006F:
			bprintf(PRINT_NORMAL, "Sound latch read (byte).\n");
//			SoundLatchStatus = 2;
//			return SoundLatchReply;
			return 0;
#endif

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to read byte value of location %x\n", sekAddress);
		}
	}
	return 0;
}

unsigned short __fastcall sailormnReadWord(unsigned int sekAddress)
{
//	bprintf(PRINT_NORMAL, "Attempt to read word value of location %x\n", sekAddress);

	switch (sekAddress) {
		case 0xB80000: {
			unsigned short nRet = ((nVBlank ^ 1) << 2) | (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0xB80002: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0xB80004: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0xB80006: {
			unsigned short nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}

		case 0xB8006C:
//			SoundLatchStatus &= ~4;
//			return SoundLatchStatus & 3;

			if (SoundLatchReplyIndex > SoundLatchReplyMax) {
				return 2;
			}
			return 0;

		case 0xB8006E:
//			bprintf(PRINT_NORMAL, "Sound latch read.\n");
//			SoundLatchStatus = 2;
//			return SoundLatchReply;

			if (SoundLatchReplyIndex > SoundLatchReplyMax) {
				SoundLatchReplyIndex = 0;
				SoundLatchReplyMax = -1;
				return 0;
			}
//			bprintf(PRINT_NORMAL, "Sound latch reply read (%02X).\n", SoundLatchReply[SoundLatchReplyIndex]);
			return SoundLatchReply[SoundLatchReplyIndex++];

		case 0x600000:
			return DrvInput[0] ^ 0xFFFF;
		case 0x600002:
			return DrvInput[1] ^ 0xF7FF | (EEPROMRead() << 11);

		default: {
// 			bprintf(PRINT_NORMAL, "Attempt to read word value of location %x\n", sekAddress);
		}
	}
	return 0;
}

void __fastcall sailormnWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
//	bprintf(PRINT_NORMAL, "Attempt to write byte value %x to location %x\n", byteValue, sekAddress);

	switch (sekAddress) {
//		case 0xB8006E:
//			SoundLatch &= 0xFF00;
//			SoundLatch |= byteValue << 8;
//			break;
//		case 0xB8006F:
//			SoundLatch &= 0x00FF;
//			SoundLatch |= byteValue;
//			break;

		case 0x700000:
			nCaveTileBank = byteValue & 1;
			EEPROMWrite(byteValue & 0x04, byteValue & 0x02, byteValue & 0x08);
			return;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write byte value %x to location %x\n", byteValue, sekAddress);
		}
	}
}

void __fastcall sailormnWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
//	bprintf(PRINT_NORMAL, "Attempt to write word value %x to location %x\n", wordValue, sekAddress);

	switch (sekAddress) {
		case 0xA00000:
			CaveTileReg[0][0] = wordValue;
			return;
		case 0xA00002:
			CaveTileReg[0][1] = wordValue;
			return;
		case 0xA00004:
			CaveTileReg[0][2] = wordValue;
			return;

		case 0xA80000:
			CaveTileReg[1][0] = wordValue;
			return;
		case 0xA80002:
			CaveTileReg[1][1] = wordValue;
			return;
		case 0xA80004:
			CaveTileReg[1][2] = wordValue;
			return;

		case 0xB00000:
			CaveTileReg[2][0] = wordValue;
			return;
		case 0xB00002:
			CaveTileReg[2][1] = wordValue;
			return;
		case 0xB00004:
			CaveTileReg[2][2] = wordValue;
			return;

		case 0xB80000:
			nCaveXOffset = wordValue;
			return;
		case 0xB80002:
			nCaveYOffset = wordValue;
			return;
		case 0xB80008:
			CaveSpriteBuffer();
			nCaveSpriteBank = wordValue;
			return;

		case 0xB8006E:
//			bprintf(PRINT_NORMAL, "Sound command sent: %04X\n", wordValue);

			SoundLatch = wordValue;
			SoundLatchStatus |= 0x0C;

			ZetNmi();
			nCaveCyclesDone[1] += ZetRun(0x0400);
			return;

		case 0x700000:
			wordValue >>= 8;
			nCaveTileBank = wordValue & 1;
			EEPROMWrite(wordValue & 0x04, wordValue & 0x02, wordValue & 0x08);
			return;

		default: {
//			bprintf(PRINT_NORMAL, "Attempt to write word value %x to location %x\n", wordValue, sekAddress);

		}
	}
}

void __fastcall sailormnWriteBytePalette(unsigned int sekAddress, unsigned char byteValue)
{
	CavePalWriteByte(sekAddress & 0xFFFF, byteValue);
}

void __fastcall sailormnWriteWordPalette(unsigned int sekAddress, unsigned short wordValue)
{
	CavePalWriteWord(sekAddress & 0xFFFF, wordValue);
}

static int DrvExit()
{
	EEPROMExit();

	MSM6295Exit(1);
	MSM6295Exit(0);
	BurnYM2151Exit();

	CaveTileExit();
	CaveSpriteExit();
    CavePalExit();

	ZetExit();

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

	nCurrentBank = -1;
	drvZ80Bankswitch(0);

	ZetReset();

	EEPROMReset();

	nVideoIRQ = 1;
	nSoundIRQ = 1;
	nUnknownIRQ = 1;

	SoundLatch = 0;
	SoundLatchStatus = 0x0C;

	memset(SoundLatchReply, 0, sizeof(SoundLatchReply));
	SoundLatchReplyIndex = 0;
	SoundLatchReplyMax = -1;

	nCaveCyclesDone[0] = nCaveCyclesDone[1] = 0;

	unsigned char nRegion = drvRegion & 0x07;
	if (nRegion > 5) {
		nRegion = 0;
	}
	unsigned int nIndex;
	if (nWhichGame) {
		nIndex = 0x01E;
	} else {
		nIndex = 0x010;
	}
	EEPROMFill(&nRegion, nIndex, 1);

	MSM6295Reset(0);
	MSM6295Reset(1);
	BurnYM2151Reset();

	return 0;
}

static int DrvDraw()
{
	if (CaveRecalcPalette) {
		CavePalUpdate8Bit(0x4400, 12);
		CaveRecalcPalette = 1;
	}
	CavePalUpdate4Bit(0, 64);

	CaveClearScreen(CavePalette[nWhichGame ? 0x3F00 : 0x5FF0]);

	if (bDrawScreen) {
//		CaveGetBitmap();

#if 0
		CaveTileRender(1);
#else
		if (nWhichGame) {
			// Air Gallet always enables row-scroll and row-select for 16x16 layers, but always without effect
			// So, force tile drawing routines to ignore row-scroll and row-select
			CaveTileRender(0);
		} else {
			CaveTileRender(1);
		}
#endif
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
	int nInterleave = 4;

	int nCyclesTotal[2];

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
	nCaveCyclesDone[0] = 0;
#if 0
	nCyclesTotal[1] = (int)((long long)8000000 * nBurnCPUSpeedAdjust / (0x0100 * CAVE_REFRESHRATE));
#else
	nCyclesTotal[1] = (int)(8000000 / CAVE_REFRESHRATE);
#endif
	nCaveCyclesDone[1] -= nCyclesTotal[1];
	if (nCaveCyclesDone[1] < 0) {
		nCaveCyclesDone[1] = 0;
	}

	nCyclesVBlank = nCyclesTotal[0] - (int)((nCyclesTotal[0] * CAVE_VBLANK_LINES) / 271.5);
	nVBlank = 0;

	int nSoundBufferPos = 0;

	SekOpen(0);

	for (int i = 0; i < nInterleave; i++) {
    	int nCurrentCPU;
		int nNext;

		// Run 68000

		nCurrentCPU = 0;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;

		// Trigger VBlank interrupt
		if (!nVBlank && nNext > nCyclesVBlank) {
			if (nCaveCyclesDone[nCurrentCPU] < nCyclesVBlank) {
				nCyclesSegment = nCyclesVBlank - nCaveCyclesDone[nCurrentCPU];
				nCaveCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
			}

			if (pBurnDraw != NULL) {
				DrvDraw();												// Draw screen if needed
			}

//			CaveSpriteBuffer();

			nVBlank = 1;
			nVideoIRQ = 0;
			UpdateIRQStatus();
		}

		nCyclesSegment = nNext - nCaveCyclesDone[nCurrentCPU];
		if (nVBlank || (!CheckSleep(nCurrentCPU))) {					// See if this CPU is busywaiting
			nCaveCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		} else {
			nCaveCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
		}

		// Run Z80
		nCurrentCPU = 1;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCaveCyclesDone[nCurrentCPU];
		nCaveCyclesDone[nCurrentCPU] += ZetRun(nCyclesSegment);

		{
			// Render sound segment
			if (pBurnSoundOut) {
				int nSegmentLength = nBurnSoundLen / nInterleave;
				short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				MSM6295Render(0, pSoundBuf, nSegmentLength);
				MSM6295Render(1, pSoundBuf, nSegmentLength);
				nSoundBufferPos += nSegmentLength;
			}
		}

	}

	SekClose();

	{
		// Make sure the buffer is entirely filled.
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (nSegmentLength) {
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				MSM6295Render(0, pSoundBuf, nSegmentLength);
				MSM6295Render(1, pSoundBuf, nSegmentLength);
			}
		}
	}

	return 0;
}

// This routine is called first to determine how much memory is needed (MemEnd-(unsigned char *)0),
// and then afterwards to set up all the pointers
static int MemIndex()
{
	unsigned char* Next; Next = Mem;
	Rom01			= Next; Next += 0x080000;		// 68K program
	Rom02			= Next; Next += 0x200000;
	RomZ80			= Next; Next += 0x080000;
	CaveSpriteROM	= Next; Next += 0x800000;
	CaveTileROM[0]	= Next; Next += 0x400000;		// Tile layer 0
	CaveTileROM[1]	= Next; Next += 0x400000;		// Tile layer 1
	if (nWhichGame) {
		CaveTileROM[2]	= Next; Next += 0x400000;		// Tile layer 2 (agallet)
	} else {
		CaveTileROM[2]	= Next; Next += 0x01400000;		// Tile layer 2 (Sailor Moon)
	}
	MSM6295ROM		= Next; Next += 0x400000;		// MSM6295 ADPCM data
	RamStart		= Next;
	Ram01			= Next; Next += 0x010002;		// CPU #0 work RAM
	Ram02			= Next; Next += 0x008000;		//
	Ram03			= Next; Next += 0x004002;		//
	RamZ80			= Next; Next += 0x002000;
	CaveTileRAM[0]	= Next; Next += 0x008000;
	CaveTileRAM[1]	= Next; Next += 0x008000;
	CaveTileRAM[2]	= Next; Next += 0x008002;
	CaveSpriteRAM	= Next; Next += 0x010002;
	CavePalSrc		= Next; Next += 0x010000;		// palette
	RamEnd			= Next;
	MemEnd			= Next;

	return 0;
}

static void sailormnDecodeSprites(unsigned char* pData, int nLen)
{
	unsigned char* pOrg = pData + nLen - 1;
	unsigned char* pDest = pData + ((nLen - 1) << 1);

	for (int i = 0; i < nLen; i++, pOrg--, pDest -= 2) {
		pDest[0] = *pOrg & 15;
		pDest[1] = *pOrg >> 4;
	}

	return;
}

static void sailormnDecodeTiles(unsigned char* pData, int nLen)
{
	unsigned char* pOrg = pData + nLen - 1;
	unsigned char* pDest = pData + ((nLen - 1) << 1);

	for (int i = 0; i < nLen; i++, pOrg--, pDest -= 2) {
		pDest[1] = *pOrg & 15;
		pDest[0] = *pOrg >> 4;
	}

	return;
}

static int sailormnLoadRoms()
{
	unsigned char* pTemp;

	// Load 68000 ROM
	BurnLoadRom(Rom01, 0, 1);
	BurnLoadRom(Rom02, 1, 1);

	// Load Z80 ROM
	BurnLoadRom(RomZ80, 2, 1);

	pTemp = (unsigned char*)malloc(0x400000);
	BurnLoadRom(pTemp + 0x000000, 3, 1);
	BurnLoadRom(pTemp + 0x200000, 4, 1);
	for (int i = 0; i < 0x400000; i++) {
		CaveSpriteROM[i ^ 0x950C4] = pTemp[BITSWAP24(i, 23, 22, 21, 20, 15, 10, 12, 6, 11, 1, 13, 3, 16, 17, 2, 5, 14, 7, 18, 8, 4, 19, 9, 0)];
	}
	free(pTemp);
	sailormnDecodeSprites(CaveSpriteROM, 0x400000);

	BurnLoadRom(CaveTileROM[0], 5, 1);
	sailormnDecodeTiles(CaveTileROM[0], 0x200000);
	BurnLoadRom(CaveTileROM[1], 6, 1);
	sailormnDecodeTiles(CaveTileROM[1], 0x200000);
	BurnLoadRom(CaveTileROM[2] + 0x000000, 7, 1);
	BurnLoadRom(CaveTileROM[2] + 0x200000, 8, 1);
	BurnLoadRom(CaveTileROM[2] + 0x400000, 9, 1);
	BurnLoadRom(CaveTileROM[2] + 0x600000, 10, 1);
	BurnLoadRom(CaveTileROM[2] + 0x800000, 11, 1);
	sailormnDecodeTiles(CaveTileROM[2], 0xA00000);

	pTemp = (unsigned char*)malloc(0x600000);
	BurnLoadRom(pTemp + 0x000000, 12, 1);
	BurnLoadRom(pTemp + 0x200000, 13, 1);
	BurnLoadRom(pTemp + 0x400000, 14, 1);
	for (int i = 0; i < 0x500000; i++) {
		CaveTileROM[2][(i << 2) + 0] |= (pTemp[i] & 0x03) << 4;
		CaveTileROM[2][(i << 2) + 1] |= (pTemp[i] & 0x0C) << 2;
		CaveTileROM[2][(i << 2) + 2] |= (pTemp[i] & 0x30);
		CaveTileROM[2][(i << 2) + 3] |= (pTemp[i] & 0xC0) >> 2;
	}
	free(pTemp);

	// Load OKIM6295 data
	BurnLoadRom(MSM6295ROM + 0x0000000, 15, 1);
	BurnLoadRom(MSM6295ROM + 0x0200000, 16, 1);
	BurnLoadRom(MSM6295ROM + 0x0280000, 16, 1);
	BurnLoadRom(MSM6295ROM + 0x0300000, 16, 1);
	BurnLoadRom(MSM6295ROM + 0x0380000, 16, 1);

	return 0;
}

static int agalletLoadRoms()
{
	// Load 68000 ROM
	BurnLoadRom(Rom01, 0, 1);

	// Load Z80 ROM
	BurnLoadRom(RomZ80, 1, 1);

	BurnLoadRom(CaveSpriteROM + 0x000000, 2, 1);
	BurnLoadRom(CaveSpriteROM + 0x200000, 3, 1);
	sailormnDecodeSprites(CaveSpriteROM, 0x400000);

	BurnLoadRom(CaveTileROM[0], 4, 1);
	sailormnDecodeTiles(CaveTileROM[0], 0x200000);
	BurnLoadRom(CaveTileROM[1], 5, 1);
	sailormnDecodeTiles(CaveTileROM[1], 0x200000);
	BurnLoadRom(CaveTileROM[2], 6, 1);
	sailormnDecodeTiles(CaveTileROM[2], 0x200000);

	unsigned char* pTemp = (unsigned char*)malloc(0x200000);
	BurnLoadRom(pTemp, 7, 1);
	for (int i = 0; i < 0x0100000; i++) {
		CaveTileROM[2][(i << 2) + 0] |= (pTemp[i] & 0x03) << 4;
		CaveTileROM[2][(i << 2) + 1] |= (pTemp[i] & 0x0C) << 2;
		CaveTileROM[2][(i << 2) + 2] |= (pTemp[i] & 0x30);
		CaveTileROM[2][(i << 2) + 3] |= (pTemp[i] & 0xC0) >> 2;
	}
	free(pTemp);

	// Load OKIM6295 data
	BurnLoadRom(MSM6295ROM + 0x0000000, 8, 1);
	BurnLoadRom(MSM6295ROM + 0x0200000, 9, 1);

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

		SekScan(nAction);				// scan 68000
		ZetScan(nAction);				// scan Z80

		SCAN_VAR(SoundLatch);
		SCAN_VAR(SoundLatchStatus);
		SCAN_VAR(SoundLatchReply);
		SCAN_VAR(SoundLatchReplyIndex);
		SCAN_VAR(SoundLatchReplyMax);

		SCAN_VAR(nCurrentBank);

		MSM6295Scan(0, nAction);
		MSM6295Scan(1, nAction);
		BurnYM2151Scan(nAction);

		SCAN_VAR(nVideoIRQ);
		SCAN_VAR(nSoundIRQ);
		SCAN_VAR(nUnknownIRQ);
		SCAN_VAR(nVBlank);

		CaveScanGraphics();

		SCAN_VAR(DrvInput);

		if (nAction & ACB_WRITE) {
			int nBank = nCurrentBank;
			nCurrentBank = -1;
			drvZ80Bankswitch(nBank);

		  CaveRecalcPalette = 1;
		}
	}

	return 0;
}

static int gameInit()
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

	if (nWhichGame) {
		unsigned char* data = (unsigned char*)malloc(48);
		for (int i = 0; i < 16; i++) {
			data[i + 0x00] = 0xFF;
			data[i + 0x10] = 0x00;
			data[i + 0x20] = 0xFF;
		}
		data[0x14] = 3;
		data[0x1E] = drvRegion;
		data[0x28] = 0;
		data[0x29] = 0;

		EEPROMFill(data, 0, 0x30);

		// Load the roms into memory
		if (agalletLoadRoms()) {
			return 1;
		}

	} else {
		unsigned char data[] = { 0x00, 0xA5, 0x00, 0xA5, 0x00, 0xA5, 0x00, 0xA5, 0x01, 0xA5, 0x01, 0xA5, 0x04, 0xA5, 0x01, 0xA5, 0x02, 0xA5 };
		data[16] |= drvRegion;
		EEPROMFill(data, 0, 0x12);

		// Load the roms into memory
		if (sailormnLoadRoms()) {
			return 1;
		}
	}

	{
		SekInit(0, 0x68000);												// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom01,					0x000000, 0x07FFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram01,					0x100000, 0x110001, SM_RAM);	// ram (Air Gallet tests too far)
		SekMapMemory(Rom02,					0x200000, 0x3FFFFF, SM_ROM);
		SekMapMemory(Ram02,					0x400000, 0x407FFF, SM_RAM);
		SekMapMemory(Ram03,					0x40C000, 0x410001, SM_RAM);	// RAM (Air Gallet tests too far)
		SekMapMemory(CaveSpriteRAM,			0x500000, 0x510001, SM_RAM);	// Sprite RAM (Air Gallet tests too far)
		SekMapMemory(CaveTileRAM[0],	   	0x800000, 0x807FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[1],	   	0x880000, 0x887FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[2],		0x900000, 0x908001, SM_RAM);	// Air Gallet tests too far

		SekMapMemory(CavePalSrc,		   	0x408000, 0x4087FF, SM_RAM);	// Palette RAM
		SekMapMemory(CavePalSrc + 0x8800,	0x408800, 0x40BFFF, SM_ROM);	// Palette RAM (write goes through handler)
		SekMapHandler(1,					0x408800, 0x40BFFF, SM_WRITE);	//

		SekSetReadWordHandler(0, sailormnReadWord);
		SekSetReadByteHandler(0, sailormnReadByte);
		SekSetWriteWordHandler(0, sailormnWriteWord);
		SekSetWriteByteHandler(0, sailormnWriteByte);

		SekSetWriteWordHandler(1, sailormnWriteWordPalette);
		SekSetWriteByteHandler(1, sailormnWriteBytePalette);

		SekClose();
	}

	drvZInit();

	nCaveExtraXOffset = -1;
	nCaveRowModeOffset = 2;

	CavePalInit(0x8000);
	CaveTileInit();
	if (nWhichGame) {
		CaveSpriteInit(1, 0x0800000);
	} else {
		CaveSpriteInit(2, 0x0800000);
	}
	CaveTileInitLayer(0, 0x400000, 4, 0x4400);
	CaveTileInitLayer(1, 0x400000, 4, 0x4800);
	if (nWhichGame) {
		CaveTileInitLayer(2, 0x400000, 6, 0x4C00);
	} else {
		CaveTileInitLayer(2, 0x01400000, 6, 0x4C00);
	}

	BurnYM2151Init(32000000 / 8, 25.0);
	BurnYM2151SetIrqHandler(&drvYM2151IRQHandler);

	MSM6295Init(0, 16000, 80.0, 1);
	MSM6295Init(1, 16000, 80.0, 1);

	bDrawScreen = true;

#if 0
#if defined FBA_DEBUG && defined USE_SPEEDHACKS
	bprintf(PRINT_IMPORTANT, "  * Using speed-hacks (detecting idle loops).\n");
#endif
#endif

	DrvDoReset();

	return 0;
}

static int sailormnInit()
{
	nWhichGame = 0;
	return gameInit();
}

static int agalletInit()
{
	nWhichGame = 1;
	return gameInit();
}

// Rom information

static struct BurnRomInfo sailormnRomDesc[] = {
	{ "bpsm945a.u45", 0x080000, 0x898C9515, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "bpsm.u46",     0x200000, 0x32084E80, BRF_ESS | BRF_PRG }, //  1

	{ "bpsm945a.u9",  0x080000, 0x438DE548, BRF_ESS | BRF_PRG }, //  2 Z80 code

	{ "bpsm.u76",     0x200000, 0xA243A5BA, BRF_GRA },			 //  3 Sprite data
	{ "bpsm.u77",     0x200000, 0x5179A4AC, BRF_GRA },			 //  4

	{ "bpsm.u53",     0x200000, 0xB9B15F83, BRF_GRA },			 //  5 Layer 0 Tile data
	{ "bpsm.u54",     0x200000, 0x8F00679D, BRF_GRA },			 //  6 Layer 1 Tile data

	{ "bpsm.u57",     0x200000, 0x86BE7B63, BRF_GRA },			 //  7 Layer 2 Tile data
	{ "bpsm.u58",     0x200000, 0xE0BBA83B, BRF_GRA },			 //  8
	{ "bpsm.u62",     0x200000, 0xA1E3BFAC, BRF_GRA },			 //  9
	{ "bpsm.u61",     0x200000, 0x6A014B52, BRF_GRA },			 // 10
	{ "bpsm.u60",     0x200000, 0x992468C0, BRF_GRA },			 // 11

	{ "bpsm.u65",     0x200000, 0xF60FB7B5, BRF_GRA },			 // 12
	{ "bpsm.u64",     0x200000, 0x6559D31C, BRF_GRA },			 // 13
	{ "bpsm.u63",     0x200000, 0xD57A56B4, BRF_GRA },			 // 14

	{ "bpsm.u48",     0x200000, 0x498E4ED1, BRF_SND },			 // 15 MSM6295 #0 ADPCM data
	{ "bpsm.u47",     0x080000, 0x0F2901B9, BRF_SND },			 // 16 MSM6295 #1 ADPCM data
};


STD_ROM_PICK(sailormn);
STD_ROM_FN(sailormn);

static struct BurnRomInfo sailormoRomDesc[] = {
	{ "smprg.u45",    0x080000, 0x234F1152, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "bpsm.u46",     0x200000, 0x32084E80, BRF_ESS | BRF_PRG }, //  1

	{ "bpsm945a.u9",  0x080000, 0x438DE548, BRF_ESS | BRF_PRG }, //  2 Z80 code

	{ "bpsm.u76",     0x200000, 0xA243A5BA, BRF_GRA },			 //  3 Sprite data
	{ "bpsm.u77",     0x200000, 0x5179A4AC, BRF_GRA },			 //  4

	{ "bpsm.u53",     0x200000, 0xB9B15F83, BRF_GRA },			 //  5 Layer 0 Tile data
	{ "bpsm.u54",     0x200000, 0x8F00679D, BRF_GRA },			 //  6 Layer 1 Tile data

	{ "bpsm.u57",     0x200000, 0x86BE7B63, BRF_GRA },			 //  7 Layer 2 Tile data
	{ "bpsm.u58",     0x200000, 0xE0BBA83B, BRF_GRA },			 //  8
	{ "bpsm.u62",     0x200000, 0xA1E3BFAC, BRF_GRA },			 //  9
	{ "bpsm.u61",     0x200000, 0x6A014B52, BRF_GRA },			 // 10
	{ "bpsm.u60",     0x200000, 0x992468C0, BRF_GRA },			 // 11

	{ "bpsm.u65",     0x200000, 0xF60FB7B5, BRF_GRA },			 // 12
	{ "bpsm.u64",     0x200000, 0x6559D31C, BRF_GRA },			 // 13
	{ "bpsm.u63",     0x200000, 0xD57A56B4, BRF_GRA },			 // 14

	{ "bpsm.u48",     0x200000, 0x498E4ED1, BRF_SND },			 // 15 MSM6295 #0 ADPCM data
	{ "bpsm.u47",     0x080000, 0x0F2901B9, BRF_SND },			 // 16 MSM6295 #1 ADPCM data
};


STD_ROM_PICK(sailormo);
STD_ROM_FN(sailormo);

static struct BurnRomInfo agalletRomDesc[] = {
	{ "bp962a.u45",   0x080000, 0x24815046, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "bp962a.u9",    0x080000, 0x06CADDBE, BRF_ESS | BRF_PRG }, //  1 Z80 code

	{ "bp962a.u76",   0x200000, 0x858DA439, BRF_GRA },			 //  2 Sprite data
	{ "bp962a.u77",   0x200000, 0xEA2BA35E, BRF_GRA },			 //  3

	{ "bp962a.u53",   0x200000, 0xFCD9A107, BRF_GRA },			 //  4 Layer 0 Tile data
	{ "bp962a.u54",   0x200000, 0x0CFA3409, BRF_GRA },			 //  5 Layer 1 Tile data
	{ "bp962a.u57",   0x200000, 0x6D608957, BRF_GRA },			 //  6 Layer 2 Tile data
	{ "bp962a.u65",   0x200000, 0x135FCF9A, BRF_GRA },			 //  7

	{ "bp962a.u48",   0x200000, 0xAE00A1CE, BRF_SND },			 //  8 MSM6295 #0 ADPCM data
	{ "bp962a.u47",   0x200000, 0x6D4E9737, BRF_SND },			 //  9 MSM6295 #1 ADPCM data
};


STD_ROM_PICK(agallet);
STD_ROM_FN(agallet);

struct BurnDriver BurnDrvSailorMoonB = {
	"sailormn", NULL, NULL, "1995",
	"Pretty Soldier Sailor Moon (version 95/03/22B)\0", NULL, "BanPresto / Gazelle", "Cave",
	L"Pretty Soldier Sailor Moon (version 95/03/22B)\0\u7F8E\u5C11\u5973\u6226\u58EB \u30BB\u30FC\u30E9\u30FC\u30E0\u30FC\u30F3 (version 95/03/22B)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_Z80,
	NULL, sailormnRomInfo, sailormnRomName, sailormnInputInfo, sailormnDIPInfo,
	sailormnInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 320, 240, 4, 3
};

struct BurnDriver BurnDrvSailorMoon = {
	"sailormo", "sailormn", NULL, "1995",
	"Pretty Soldier Sailor Moon (version 95/03/22)\0", NULL, "BanPresto / Gazelle", "Cave",
	L"Pretty Soldier Sailor Moon (version 95/03/22)\0\u7F8E\u5C11\u5973\u6226\u58EB \u30BB\u30FC\u30E9\u30FC\u30E0\u30FC\u30F3 (version 95/03/22)\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_Z80,
	NULL, sailormoRomInfo, sailormoRomName, sailormnInputInfo, sailormnDIPInfo,
	sailormnInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 320, 240, 4, 3
};

struct BurnDriver BurnDrvAirGallet = {
	"agallet", NULL, NULL, "1996",
	"Air Gallet\0", NULL, "BanPresto / Gazelle", "Cave",
	L"Air Gallet\0\u30A2\u30EF\u30A6\u30AE\u30E3\u30EC\u30C3\u30C8\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_Z80,
	NULL, agalletRomInfo, agalletRomName, sailormnInputInfo, sailormnDIPInfo,
	agalletInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 320, 3, 4
};


