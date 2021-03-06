// mazinger
#include "cave.h"
#include "msm6295.h"
#include "burn_ym2203.h"
#include "bitswap.h"

#define CAVE_VBLANK_LINES 12

static unsigned char DrvJoy1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static unsigned short DrvInput[2] = {0x0000, 0x0000};

static unsigned char drvRegion = 0;

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;
static unsigned char *Rom01, *RomZ80;
static unsigned char *Ram01, *RamZ80;
static unsigned char *MSM6295ROMSrc;

static unsigned char DrvReset = 0;
static unsigned char bDrawScreen;
static bool bVBlank;

static char nVideoIRQ;
static char nSoundIRQ;
static char nUnknownIRQ;

static char nIRQPending;

static int nCyclesTotal[2];
static int nCyclesDone[2];

static int SoundLatch;
static int SoundLatchReply[48];
static int SoundLatchStatus;

static int SoundLatchReplyIndex;
static int SoundLatchReplyMax;

static unsigned char DrvZ80Bank;
static unsigned char DrvOkiBank1;
static unsigned char DrvOkiBank2;

static struct BurnInputInfo mazingerInputList[] = {
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

STDINPUTINFO(mazinger);

static struct BurnDIPInfo mazingerDIPList[] = {
	// Region
	{0x15,	0xFF, 0xFF,	0x31, NULL},
	
	{0,		0xFD, 0,	2,	  "Region"},
	{0x15,	0x01, 0xff, 0x30, "Japan"},
	{0x15,	0x01, 0xff, 0x31, "World"},
};

STDDIPINFO(mazinger);

static void UpdateIRQStatus()
{
	nIRQPending = (nVideoIRQ == 0 || nSoundIRQ == 0 || nUnknownIRQ == 0);
	SekSetIRQLine(1, nIRQPending ? SEK_IRQSTATUS_ACK : SEK_IRQSTATUS_NONE);
}

unsigned char __fastcall mazingerReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {
		default: {
 			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
		}
	}
	return 0;
}

void __fastcall mazingerWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {
		default: {
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);

		}
	}
}

unsigned short __fastcall mazingerReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x300000:
		case 0x300002: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			return nRet;
		}
		case 0x300004: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nVideoIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		case 0x300006: {
			unsigned char nRet = (nUnknownIRQ << 1) | nVideoIRQ;
			nUnknownIRQ = 1;
			UpdateIRQStatus();
			return nRet;
		}
		
		case 0x30006E:
			if (SoundLatchReplyIndex > SoundLatchReplyMax) {
				SoundLatchReplyIndex = 0;
				SoundLatchReplyMax = -1;
				return 0;
			}
			return SoundLatchReply[SoundLatchReplyIndex++];
			
		case 0x800000:
			return DrvInput[0] ^ 0xFFFF;
		case 0x800002:
			return DrvInput[1] ^ 0xF7FF | (EEPROMRead() << 11);		
		
		default: {
 			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
		}
	}
	return 0;
}

void __fastcall mazingerWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	if (sekAddress >= 0x30000a && sekAddress <= 0x300066) return;
	if (sekAddress >= 0x30006a && sekAddress <= 0x30006c) return;
	if (sekAddress >= 0x300004 && sekAddress <= 0x300006) return;
	
	switch (sekAddress) {
		case 0x300000:
			nCaveXOffset = wordValue;
			return;
		case 0x300002:
			nCaveYOffset = wordValue;
			return;
			
		case 0x300008:
//			CaveSpriteBuffer();
			nCaveSpriteBank = wordValue;
			return;
			
		case 0x30006e:
			SoundLatch = wordValue;
			SoundLatchStatus |= 0x0C;

			ZetOpen(0);
			ZetNmi();
			nCyclesDone[1] += ZetRun(0x0400);
			ZetClose();
			return;
		
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
			
		case 0x900000:
			wordValue >>= 8;
			EEPROMWrite(wordValue & 0x04, wordValue & 0x02, wordValue & 0x08);
			break;
			
		default: {
			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);

		}
	}
}

void __fastcall mazingerWriteBytePalette(unsigned int sekAddress, unsigned char byteValue)
{
	CavePalWriteByte(sekAddress & 0xffff, byteValue);
}

void __fastcall mazingerWriteWordPalette(unsigned int sekAddress, unsigned short wordValue)
{
	CavePalWriteWord(sekAddress & 0xffff, wordValue);
}

unsigned char __fastcall mazingerZIn(unsigned short nAddress)
{
	nAddress &= 0xFF;

	switch (nAddress) {
		case 0x30: {
			SoundLatchStatus |= 0x04;
			return SoundLatch & 0xFF;
		}
		
		case 0x52: {
			return BurnYM2203Read(0, 0);
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Port Read %x\n"), nAddress);
		}
	}

	return 0;
}

void __fastcall mazingerZOut(unsigned short nAddress, unsigned char nValue)
{
	nAddress &= 0xFF;

	switch (nAddress) {
		case 0x00: {
			DrvZ80Bank = nValue & 0x07;
			
			ZetMapArea(0x4000, 0x7FFF, 0, RomZ80 + (DrvZ80Bank * 0x4000));
			ZetMapArea(0x4000, 0x7FFF, 2, RomZ80 + (DrvZ80Bank * 0x4000));
			return;
		}
		
		case 0x10:
			if (SoundLatchReplyIndex > SoundLatchReplyMax) {
				SoundLatchReplyMax = -1;
				SoundLatchReplyIndex = 0;
			}
			SoundLatchReplyMax++;
			SoundLatchReply[SoundLatchReplyMax] = nValue;
			break;
		
		case 0x50: {
			BurnYM2203Write(0, 0, nValue);
			return;
		}
		
		case 0x51: {
			BurnYM2203Write(0, 1, nValue);
			return;
		}
		
		case 0x70: {
			MSM6295Command(0, nValue);
			return;
		}
		
		case 0x74: {
			DrvOkiBank1 = (nValue >> 0) & 0x03;
			DrvOkiBank2 = (nValue >> 4) & 0x03;
			
			memcpy(MSM6295ROM + 0x00000, MSM6295ROMSrc + 0x20000 * DrvOkiBank1, 0x20000);
			memcpy(MSM6295ROM + 0x20000, MSM6295ROMSrc + 0x20000 * DrvOkiBank2, 0x20000);
			return;
		}
		
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Port Write %x, %x\n"), nAddress, nValue);
		}
	}
}

unsigned char __fastcall mazingerZRead(unsigned short a)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall mazingerZWrite(unsigned short a, unsigned char d)
{
	switch (a) {
		default: {
			bprintf(PRINT_NORMAL, _T("Z80 Write => %04X, %02X\n"), a, d);
		}
	}
}

static int DrvExit()
{
	EEPROMExit();

	MSM6295Exit(0);

	CaveTileExit();
	CaveSpriteExit();
    CavePalExit();

	SekExit();				// Deallocate 68000s
	ZetExit();
	
	BurnYM2203Exit();
	
	SoundLatch = 0;
	DrvZ80Bank = 0;
	DrvOkiBank1 = 0;
	DrvOkiBank2 = 0;

	// Deallocate all used memory
	free(Mem);
	Mem = NULL;

	return 0;
}

static int DrvDoReset()
{
	SekOpen(0);
	SekReset();
	SekRun(10000);	// Need to run for a bit and reset to make it start - Watchdog would force reset?
	SekReset();
	SekClose();
	
	ZetOpen(0);
	ZetReset();
	ZetClose();
	
	BurnYM2203Reset();
	MSM6295Reset(0);

	EEPROMReset();
	
	nVideoIRQ = 1;
	nSoundIRQ = 1;
	nUnknownIRQ = 1;

	nIRQPending = 0;
	
	SoundLatch = 0;
	DrvZ80Bank = 0;
	DrvOkiBank1 = 0;
	DrvOkiBank2 = 0;
	
	SoundLatch = 0;
	SoundLatchStatus = 0x0C;

	memset(SoundLatchReply, 0, sizeof(SoundLatchReply));
	SoundLatchReplyIndex = 0;
	SoundLatchReplyMax = -1;
	
	unsigned char nRegion = drvRegion & 0xff;
	EEPROMFill(&nRegion, 4, 1);

	return 0;
}

static int DrvDraw()
{
	if (CaveRecalcPalette) {
		CavePalUpdate8Bit(0x4400, 12);
		CaveRecalcPalette = 1;
	}
	CavePalUpdate4Bit(0, 64);

	CaveClearScreen(CavePalette[0x3F00]);

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
	ZetNewFrame();
	
	SekOpen(0);
	
	nCyclesTotal[0] = (int)((long long)16000000 * nBurnCPUSpeedAdjust / (0x0100 * CAVE_REFRESHRATE));
	nCyclesTotal[1] = (int)(4000000 / CAVE_REFRESHRATE);
	nCyclesDone[0] = nCyclesDone[1] = 0;

	nCyclesVBlank = nCyclesTotal[0] - (int)((nCyclesTotal[0] * CAVE_VBLANK_LINES) / 271.5);
	bVBlank = false;

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
			
			CaveSpriteBuffer();
			unsigned char Temp = nCaveSpriteBank;
			nCaveSpriteBank = nCaveSpriteBankDelay;
			nCaveSpriteBankDelay = Temp;

			bVBlank = true;
			nVideoIRQ = 0;
			nUnknownIRQ = 0;
			UpdateIRQStatus();
		}
		
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (!CheckSleep(nCurrentCPU)) {									// See if this CPU is busywaiting
			nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		} else {
			nCyclesDone[nCurrentCPU] += SekIdle(nCyclesSegment);
		}
	}
	
	SekClose();
	
	ZetOpen(0);
	BurnTimerEndFrame(nCyclesTotal[1] - nCyclesDone[1]);
	BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
	if (pBurnSoundOut) MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	ZetClose();

	return 0;
}

// This routine is called first to determine how much memory is needed (MemEnd-(unsigned char *)0),
// and then afterwards to set up all the pointers
static int MemIndex()
{
	unsigned char* Next; Next = Mem;
	Rom01			= Next; Next += 0x100000;		// 68K program
	RomZ80			= Next; Next += 0x020000;
	CaveSpriteROM	= Next; Next += 0x800000;
	CaveTileROM[0]	= Next; Next += 0x400000;		// Tile layer 0
	CaveTileROM[1]	= Next; Next += 0x400000;		// Tile layer 1
	MSM6295ROM		= Next; Next += 0x040000;
	MSM6295ROMSrc		= Next; Next += 0x080000;
	RamStart		= Next;
	Ram01			= Next; Next += 0x010000;		// CPU #0 work RAM
	RamZ80			= Next; Next += 0x001000;
	CaveTileRAM[0]	= Next; Next += 0x008000;
	CaveTileRAM[1]	= Next; Next += 0x008000;
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
	BurnLoadRom(Rom01 + 0x00000, 0, 1);
	BurnLoadRom(Rom01 + 0x80000, 1, 1);
	
	BurnLoadRom(RomZ80, 2, 1);

	unsigned char *pTemp = (unsigned char*)malloc(0x400000);
	BurnLoadRom(pTemp + 0x000000, 3, 1);
	BurnLoadRom(pTemp + 0x200000, 4, 1);
	for (int i = 0; i < 0x400000; i++) {
		CaveSpriteROM[i ^ 0xdf88] = pTemp[BITSWAP24(i,23,22,21,20,19,9,7,3,15,4,17,14,18,2,16,5,11,8,6,13,1,10,12,0)];
	}
	free(pTemp);
	NibbleSwap1(CaveSpriteROM, 0x400000);

	BurnLoadRom(CaveTileROM[0], 5, 1);
	NibbleSwap2(CaveTileROM[0], 0x200000);
	
	pTemp = (unsigned char*)malloc(0x200000);
	BurnLoadRom(pTemp, 6, 1);
	for (int i = 0; i < 0x0100000; i++) {
		CaveTileROM[1][(i << 1) + 1] = (pTemp[(i << 1) + 0] & 15) | ((pTemp[(i << 1) + 1] & 15) << 4);
		CaveTileROM[1][(i << 1) + 0] = (pTemp[(i << 1) + 0] >> 4) | (pTemp[(i << 1) + 1] & 240);
	}
	free(pTemp);

	// Load MSM6295 ADPCM data
	BurnLoadRom(MSM6295ROMSrc, 7, 1);

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

		SekScan(nAction);
		ZetScan(nAction);

		BurnYM2203Scan(nAction, pnMin);
		MSM6295Scan(0, nAction);

		SCAN_VAR(nVideoIRQ);
		SCAN_VAR(nSoundIRQ);
		SCAN_VAR(nUnknownIRQ);
		SCAN_VAR(bVBlank);

		CaveScanGraphics();

		SCAN_VAR(DrvInput);
		SCAN_VAR(SoundLatch);
		SCAN_VAR(DrvZ80Bank);
		SCAN_VAR(DrvOkiBank1);
		SCAN_VAR(DrvOkiBank2);
		
		if (nAction & ACB_WRITE) {
			ZetOpen(0);
			ZetMapArea(0x4000, 0x7FFF, 0, RomZ80 + (DrvZ80Bank * 0x4000));
			ZetMapArea(0x4000, 0x7FFF, 2, RomZ80 + (DrvZ80Bank * 0x4000));
			ZetClose();
			
			memcpy(MSM6295ROM + 0x00000, MSM6295ROMSrc + 0x20000 * DrvOkiBank1, 0x20000);
			memcpy(MSM6295ROM + 0x20000, MSM6295ROMSrc + 0x20000 * DrvOkiBank2, 0x20000);

			CaveRecalcPalette = 1;
		}
	}

	return 0;
}

static void DrvFMIRQHandler(int, int nStatus)
{
	if (nStatus & 1) {
		ZetSetIRQLine(0xff, ZET_IRQSTATUS_ACK);
	} else {
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	}
}

static int DrvSynchroniseStream(int nSoundRate)
{
	return (long long)ZetTotalCycles() * nSoundRate / 4000000;
}

static double DrvGetTime()
{
	return (double)ZetTotalCycles() / 4000000;
}

static int drvZInit()
{
	ZetInit(1);
	
	ZetOpen(0);

	ZetSetInHandler(mazingerZIn);
	ZetSetOutHandler(mazingerZOut);
	ZetSetReadHandler(mazingerZRead);
	ZetSetWriteHandler(mazingerZWrite);

	// ROM bank 1
	ZetMapArea    (0x0000, 0x3FFF, 0, RomZ80 + 0x0000); // Direct Read from ROM
	ZetMapArea    (0x0000, 0x3FFF, 2, RomZ80 + 0x0000); // Direct Fetch from ROM
	// ROM bank 2
	ZetMapArea    (0x4000, 0x7FFF, 0, RomZ80 + 0x4000); // Direct Read from ROM
	ZetMapArea    (0x4000, 0x7FFF, 2, RomZ80 + 0x4000); //
	// RAM
	ZetMapArea    (0xc000, 0xc7FF, 0, RamZ80 + 0x0000);			// Direct Read from RAM
	ZetMapArea    (0xc000, 0xc7FF, 1, RamZ80 + 0x0000);			// Direct Write to RAM
	ZetMapArea    (0xc000, 0xc7FF, 2, RamZ80 + 0x0000);			//
	
	ZetMapArea    (0xf800, 0xffFF, 0, RamZ80 + 0x0800);			// Direct Read from RAM
	ZetMapArea    (0xf800, 0xffFF, 1, RamZ80 + 0x0800);			// Direct Write to RAM
	ZetMapArea    (0xf800, 0xffFF, 2, RamZ80 + 0x0800);			//

	ZetMemEnd();
	
	ZetClose();

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
	
	unsigned char data[] = { 0xFF,0xED,0x00,0x00,0x31,0x12 };
	EEPROMFill(data, 0, 0x06);

	// Load the roms into memory
	if (LoadRoms()) {
		return 1;
	}

	{
		SekInit(0, 0x68000);													// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom01,				0x000000, 0x07FFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram01,				0x100000, 0x10FFFF, SM_RAM);
		SekMapMemory(CaveSpriteRAM,			0x200000, 0x20FFFF, SM_RAM);
		SekMapMemory(CaveTileRAM[1] + 0x4000,		0x400000, 0x403FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[1] + 0x4000,		0x404000, 0x407FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[0] + 0x4000,		0x500000, 0x503FFF, SM_RAM);
		SekMapMemory(CaveTileRAM[0] + 0x4000,		0x504000, 0x507FFF, SM_RAM);
		SekMapMemory(CavePalSrc,		   	0xC08000, 0xc087FF, SM_RAM);	// Palette RAM
		SekMapMemory(CavePalSrc + 0x8800,		0xC08800, 0xC0FFFF, SM_ROM);	// Palette RAM (write goes through handler)
		SekMapHandler(1,				0xC08800, 0xC0FFFF, SM_WRITE);	//

		SekMapMemory(Rom01 + 0x80000,			0xD00000, 0xD7FFFF, SM_ROM);	// CPU 0 ROM

		SekSetReadByteHandler(0, mazingerReadByte);
		SekSetWriteByteHandler(0, mazingerWriteByte);
		SekSetReadWordHandler(0, mazingerReadWord);
		SekSetWriteWordHandler(0, mazingerWriteWord);
		
		SekSetWriteWordHandler(1, mazingerWriteWordPalette);
		SekSetWriteByteHandler(1, mazingerWriteBytePalette);
		SekClose();
	}
	
	drvZInit();

	CavePalInit(0x8000);
	CaveTileInit();
	CaveSpriteInit(2, 0x0800000);
	CaveTileInitLayer(0, 0x400000, 8, 0x0000);
	CaveTileInitLayer(1, 0x400000, 6, 0x4400);
	
	BurnYM2203Init(1, 4000000, &DrvFMIRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
	BurnTimerAttachZet(4000000);
	
	memcpy(MSM6295ROM, MSM6295ROMSrc, 0x40000);
	MSM6295Init(0, 1056000 / 132, 100.0, 1);
	
	bDrawScreen = true;

#if defined FBA_DEBUG && defined USE_SPEEDHACKS
	bprintf(PRINT_IMPORTANT, _T("  * Using speed-hacks (detecting idle loops).\n"));
#endif

	DrvDoReset(); // Reset machine

	return 0;
}

// Rom information
static struct BurnRomInfo mazingerRomDesc[] = {
	{ "mzp-0.u24",    0x080000, 0x43a4279f, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "mzp-1.924",    0x080000, 0xdb40acba, BRF_ESS | BRF_PRG }, //  1
	
	{ "mzs.u21",      0x020000, 0xc5b4f7ed, BRF_ESS | BRF_PRG }, //  2 Z80 Code

	{ "bp943a-2.u56", 0x200000, 0x97e13959, BRF_GRA },	     //  3 Sprite data
	{ "bp943a-3.u55", 0x080000, 0x9c4957dd, BRF_GRA },	     //  4

	{ "bp943a-1.u60", 0x200000, 0x46327415, BRF_GRA },			 //  5 Layer 0 Tile data
	{ "bp943a-0.u63", 0x200000, 0xc1fed98a, BRF_GRA },			 //  6 Layer 1 Tile data

	{ "bp943a-4.u64", 0x080000, 0x3fc7f29a, BRF_SND },			 //  7 MSM6295 #1 ADPCM data
};


STD_ROM_PICK(mazinger);
STD_ROM_FN(mazinger);

struct BurnDriver BurnDrvmazinger = {
	"mazinger", NULL, NULL, "1994",
	"Mazinger Z (International/Japan)\0", NULL, "Banpresto/Dynamic Pl. Toei Animation", "Cave",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL |BDF_ORIENTATION_FLIPPED | BDF_16BIT_ONLY, 2, HARDWARE_CAVE_68K_Z80,
	NULL, mazingerRomInfo, mazingerRomName, mazingerInputInfo, mazingerDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan,
	0, NULL, NULL, NULL,
	&CaveRecalcPalette, 240, 384, 3, 4
};
