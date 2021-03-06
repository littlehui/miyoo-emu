#include "tiles_generic.h"
#include "taito.h"
#include "taito_ic.h"
#include "burn_ym2151.h"
#include "burn_ym2610.h"
#include "burn_ym2203.h"
#include "msm5205.h"
#include "msm6295.h"
#include "eeprom_93cxx.h"
#include "burn_gun.h"

unsigned char TaitoInputPort0[8]       = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char TaitoInputPort1[8]       = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char TaitoInputPort2[8]       = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char TaitoInputPort3[8]       = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char TaitoInputPort4[8]       = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char TaitoInputPort5[8]       = { 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char TaitoDip[2]              = { 0, 0 };
unsigned char TaitoInput[6]            = { 0, 0, 0, 0, 0, 0 };
unsigned char TaitoReset               = 0;

int           TaitoAnalogPort0         = 0;
int           TaitoAnalogPort1         = 0;
int           TaitoAnalogPort2         = 0;
int           TaitoAnalogPort3         = 0;

unsigned char *TaitoMem                = NULL;
unsigned char *TaitoMemEnd             = NULL;
unsigned char *TaitoRamStart           = NULL;
unsigned char *TaitoRamEnd             = NULL;
unsigned char *Taito68KRam1            = NULL;
unsigned char *Taito68KRam2            = NULL;
unsigned char *TaitoSharedRam          = NULL;
unsigned char *TaitoZ80Ram1            = NULL;
unsigned char *TaitoZ80Ram2            = NULL;
unsigned char *TaitoPaletteRam         = NULL;
unsigned char *TaitoSpriteRam          = NULL;
unsigned char *TaitoSpriteRam2         = NULL;
unsigned char *TaitoSpriteRamBuffered  = NULL;
unsigned char *TaitoSpriteRamDelayed   = NULL;
unsigned char *TaitoSpriteExtension    = NULL;
unsigned char *TaitoVideoRam           = NULL;
unsigned int  *TaitoPalette            = NULL;
unsigned char *TaitoPriorityMap        = NULL;

unsigned char TaitoZ80Bank = 0;
unsigned char TaitoSoundLatch = 0;
unsigned char TaitoCpuACtrl = 0;
int TaitoRoadPalBank = 0;

int TaitoXOffset = 0;
int TaitoYOffset = 0;
int TaitoIrqLine = 0;
int TaitoFrameInterleave = 0;
int TaitoFlipScreenX = 0;

int TaitoNum68Ks = 0;
int TaitoNumZ80s = 0;
int TaitoNumYM2610 = 0;
int TaitoNumYM2151 = 0;
int TaitoNumYM2203 = 0;
int TaitoNumMSM5205 = 0;
int TaitoNumMSM6295 = 0;
int TaitoNumEEPROM = 0;

int nTaitoCyclesDone[4], nTaitoCyclesTotal[4];
int nTaitoCyclesSegment;

TaitoRender TaitoDrawFunction;
TaitoMakeInputs TaitoMakeInputsFunction;
TaitoResetFunc TaitoResetFunction;

int TaitoDoReset()
{
	int i;
	
	for (i = 0; i < TaitoNum68Ks; i++) {
		SekOpen(i);
		SekReset();
		SekClose();
	}
	
	for (i = 0; i < TaitoNumZ80s; i++) {
		ZetOpen(i);
		ZetReset();
		ZetClose();
	}
	
	if (TaitoNumYM2610) BurnYM2610Reset();
	if (TaitoNumYM2151) BurnYM2151Reset();
	if (TaitoNumYM2203) BurnYM2203Reset();
	
	for (i = 0; i < TaitoNumMSM5205; i++) {
		MSM5205Reset(i);
	}
	
	for (i = 0; i < TaitoNumMSM6295; i++) {
		MSM6295Reset(i);
	}
	
	if (TaitoNumEEPROM) EEPROMReset();
	
	TaitoICReset();
	
	TaitoZ80Bank = 0;
	TaitoSoundLatch = 0;
	TaitoRoadPalBank = 0;
	TaitoCpuACtrl = 0xff;
	
	return 0;
}

unsigned char *Taito68KRom1    = NULL;
unsigned char *Taito68KRom2    = NULL;
unsigned char *TaitoZ80Rom1    = NULL;
unsigned char *TaitoZ80Rom2    = NULL;
unsigned char *TaitoChars      = NULL;
unsigned char *TaitoCharsB     = NULL;
unsigned char *TaitoCharsPivot = NULL;
unsigned char *TaitoSpritesA   = NULL;
unsigned char *TaitoSpritesB   = NULL;
unsigned char *TaitoSpriteMapRom = NULL;
unsigned char *TaitoYM2610ARom = NULL;
unsigned char *TaitoYM2610BRom = NULL;
unsigned char *TaitoMSM6295Rom = NULL;

unsigned int Taito68KRom1Num = 0;
unsigned int Taito68KRom2Num = 0;
unsigned int TaitoZ80Rom1Num = 0;
unsigned int TaitoZ80Rom2Num = 0;
unsigned int TaitoCharRomNum = 0;
unsigned int TaitoCharBRomNum = 0;
unsigned int TaitoCharPivotRomNum = 0;
unsigned int TaitoSpriteARomNum = 0;
unsigned int TaitoSpriteBRomNum = 0;
unsigned int TaitoRoadRomNum       = 0;
unsigned int TaitoSpriteMapRomNum  = 0;
unsigned int TaitoYM2610ARomNum = 0;
unsigned int TaitoYM2610BRomNum = 0;
unsigned int TaitoMSM5205RomNum = 0;
unsigned int TaitoMSM6295RomNum = 0;

unsigned int Taito68KRom1Size = 0;
unsigned int Taito68KRom2Size = 0;
unsigned int TaitoZ80Rom1Size = 0;
unsigned int TaitoZ80Rom2Size = 0;
unsigned int TaitoCharRomSize = 0;
unsigned int TaitoCharBRomSize = 0;
unsigned int TaitoCharPivotRomSize = 0;
unsigned int TaitoSpriteARomSize = 0;
unsigned int TaitoSpriteBRomSize = 0;
unsigned int TaitoRoadRomSize = 0;
unsigned int TaitoSpriteMapRomSize = 0;
unsigned int TaitoYM2610ARomSize = 0;
unsigned int TaitoYM2610BRomSize = 0;
unsigned int TaitoMSM5205RomSize = 0;
unsigned int TaitoMSM6295RomSize = 0;

unsigned int TaitoCharModulo = 0;
unsigned int TaitoCharNumPlanes = 0;
unsigned int TaitoCharWidth = 0;
unsigned int TaitoCharHeight = 0;
unsigned int TaitoNumChar = 0;
int *TaitoCharPlaneOffsets = NULL;
int *TaitoCharXOffsets = NULL;
int *TaitoCharYOffsets = NULL;

unsigned int TaitoCharBModulo = 0;
unsigned int TaitoCharBNumPlanes = 0;
unsigned int TaitoCharBWidth = 0;
unsigned int TaitoCharBHeight = 0;
unsigned int TaitoNumCharB = 0;
int *TaitoCharBPlaneOffsets = NULL;
int *TaitoCharBXOffsets = NULL;
int *TaitoCharBYOffsets = NULL;

unsigned int TaitoCharPivotModulo = 0;
unsigned int TaitoCharPivotNumPlanes = 0;
unsigned int TaitoCharPivotWidth = 0;
unsigned int TaitoCharPivotHeight = 0;
unsigned int TaitoNumCharPivot = 0;
int *TaitoCharPivotPlaneOffsets = NULL;
int *TaitoCharPivotXOffsets = NULL;
int *TaitoCharPivotYOffsets = NULL;

unsigned int TaitoSpriteAModulo = 0;
unsigned int TaitoSpriteANumPlanes = 0;
unsigned int TaitoSpriteAWidth = 0;
unsigned int TaitoSpriteAHeight = 0;
unsigned int TaitoNumSpriteA = 0;
int *TaitoSpriteAPlaneOffsets = NULL;
int *TaitoSpriteAXOffsets = NULL;
int *TaitoSpriteAYOffsets = NULL;
int TaitoSpriteAInvertRom = 0;

unsigned int TaitoSpriteBModulo = 0;
unsigned int TaitoSpriteBNumPlanes = 0;
unsigned int TaitoSpriteBWidth = 0;
unsigned int TaitoSpriteBHeight = 0;
unsigned int TaitoNumSpriteB = 0;
int *TaitoSpriteBPlaneOffsets = NULL;
int *TaitoSpriteBXOffsets = NULL;
int *TaitoSpriteBYOffsets = NULL;

int TaitoLoadRoms(int bLoad)
{
	struct BurnRomInfo ri;
	ri.nType = 0;
	ri.nLen = 0;
	int nOffset = -1;
	unsigned int i = 0;
	int nRet = 0;
	
	if (!bLoad) {
		do {
			ri.nLen = 0;
			ri.nType = 0;
			BurnDrvGetRomInfo(&ri, ++nOffset);
			if ((ri.nType & 0xff) == TAITO_68KROM1 || (ri.nType & 0xff) == TAITO_68KROM1_BYTESWAP || (ri.nType & 0xff) == TAITO_68KROM1_BYTESWAP_JUMPING) {
				Taito68KRom1Size += ri.nLen;
				Taito68KRom1Num++;
			}
			if ((ri.nType & 0xff) == TAITO_68KROM2 || (ri.nType & 0xff) == TAITO_68KROM2_BYTESWAP) {
				Taito68KRom2Size += ri.nLen;
				Taito68KRom2Num++;
			}
			if ((ri.nType & 0xff) == TAITO_Z80ROM1) {
				TaitoZ80Rom1Size += ri.nLen;
				TaitoZ80Rom1Num++;
			}
			if ((ri.nType & 0xff) == TAITO_Z80ROM2) {
				TaitoZ80Rom2Size += ri.nLen;
				TaitoZ80Rom2Num++;
			}
			if ((ri.nType & 0xff) == TAITO_CHARS || (ri.nType & 0xff) == TAITO_CHARS_BYTESWAP) {
				TaitoCharRomSize += ri.nLen;
				TaitoCharRomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_CHARSB || (ri.nType & 0xff) == TAITO_CHARSB_BYTESWAP) {
				TaitoCharBRomSize += ri.nLen;
				TaitoCharBRomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_SPRITESA || (ri.nType & 0xff) == TAITO_SPRITESA_BYTESWAP || (ri.nType & 0xff) == TAITO_SPRITESA_BYTESWAP32) {
				TaitoSpriteARomSize += ri.nLen;
				TaitoSpriteARomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_SPRITESB || (ri.nType & 0xff) == TAITO_SPRITESB_BYTESWAP || (ri.nType & 0xff) == TAITO_SPRITESB_BYTESWAP32) {
				TaitoSpriteBRomSize += ri.nLen;
				TaitoSpriteBRomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_ROAD) {
				TaitoRoadRomSize += ri.nLen;
				TaitoRoadRomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_SPRITEMAP) {
				TaitoSpriteMapRomSize += ri.nLen;
				TaitoSpriteMapRomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_YM2610A) {
				TaitoYM2610ARomSize += ri.nLen;
				TaitoYM2610ARomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_YM2610B) {
				TaitoYM2610BRomSize += ri.nLen;
				TaitoYM2610BRomNum++;
			}			
			if ((ri.nType & 0xff) == TAITO_MSM5205 || (ri.nType & 0xff) == TAITO_MSM5205_BYTESWAP) {
				TaitoMSM5205RomSize += ri.nLen;
				TaitoMSM5205RomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_CHARS_PIVOT) {
				TaitoCharPivotRomSize += ri.nLen;
				TaitoCharPivotRomNum++;
			}
			if ((ri.nType & 0xff) == TAITO_MSM6295) {
				TaitoMSM6295RomSize += ri.nLen;
				TaitoMSM6295RomNum++;
			}
		} while (ri.nLen);
		
#if 1 && defined FBA_DEBUG
		if (Taito68KRom1Size) bprintf(PRINT_IMPORTANT, _T("68K #1 Rom Length %06X, (%i roms)\n"), Taito68KRom1Size, Taito68KRom1Num);
		if (Taito68KRom2Size) bprintf(PRINT_IMPORTANT, _T("68K #2 Rom Length %06X, (%i roms)\n"), Taito68KRom2Size, Taito68KRom2Num);
		if (TaitoZ80Rom1Size) bprintf(PRINT_IMPORTANT, _T("Z80 #1 Rom Length %06X, (%i roms)\n"), TaitoZ80Rom1Size, TaitoZ80Rom1Num);
		if (TaitoZ80Rom2Size) bprintf(PRINT_IMPORTANT, _T("Z80 #2 Rom Length %06X, (%i roms)\n"), TaitoZ80Rom2Size, TaitoZ80Rom2Num);
		if (TaitoCharRomSize) bprintf(PRINT_IMPORTANT, _T("Char Rom Length %08X, (%i roms, 0x%06X tiles)\n"), TaitoCharRomSize, TaitoCharRomNum, TaitoNumChar);
		if (TaitoCharBRomSize) bprintf(PRINT_IMPORTANT, _T("Char B Rom Length %08X, (%i roms, 0x%06X tiles)\n"), TaitoCharBRomSize, TaitoCharBRomNum, TaitoNumCharB);
		if (TaitoSpriteARomSize) bprintf(PRINT_IMPORTANT, _T("Sprite A Rom Length %08X, (%i roms, 0x%06X tiles)\n"), TaitoSpriteARomSize, TaitoSpriteARomNum, TaitoNumSpriteA);
		if (TaitoSpriteBRomSize) bprintf(PRINT_IMPORTANT, _T("Sprite B Rom Length %08X, (%i roms, 0x%06X tiles)\n"), TaitoSpriteBRomSize, TaitoSpriteBRomNum, TaitoNumSpriteB);
		if (TaitoRoadRomSize) bprintf(PRINT_IMPORTANT, _T("Road Rom Length %08X, (%i roms)\n"), TaitoRoadRomSize, TaitoRoadRomNum);
		if (TaitoSpriteMapRomSize) bprintf(PRINT_IMPORTANT, _T("Sprite Map Rom Length %08X, (%i roms)\n"), TaitoSpriteMapRomSize, TaitoSpriteMapRomNum);
		if (TaitoYM2610ARomSize) bprintf(PRINT_IMPORTANT, _T("YM2610 Samples Rom Length %08X, (%i roms)\n"), TaitoYM2610ARomSize, TaitoYM2610ARomNum);
		if (TaitoYM2610BRomSize) bprintf(PRINT_IMPORTANT, _T("YM2610 Delta-T Rom Length %08X, (%i roms)\n"), TaitoYM2610BRomSize, TaitoYM2610BRomNum);
		if (TaitoMSM5205RomSize) bprintf(PRINT_IMPORTANT, _T("MSM5205 Rom Length %08X, (%i roms)\n"), TaitoMSM5205RomSize, TaitoMSM5205RomNum);
		if (TaitoCharPivotRomSize) bprintf(PRINT_IMPORTANT, _T("Pivot Char Rom Length %08X, (%i roms, 0x%06X tiles)\n"), TaitoCharPivotRomSize, TaitoCharPivotRomNum, TaitoNumCharPivot);
		if (TaitoMSM6295RomSize) bprintf(PRINT_IMPORTANT, _T("MSM6295 Rom Length %08X, (%i roms)\n"), TaitoMSM6295RomSize, TaitoMSM6295RomNum);
#endif
	}
	
	if (bLoad) {
		int Offset = 0;
		
		i = 0;
		while (i < Taito68KRom1Num) {
			BurnDrvGetRomInfo(&ri, i + 0);
			
			if ((ri.nType & 0xff) == TAITO_68KROM1_BYTESWAP) {
				nRet = BurnLoadRom(Taito68KRom1 + Offset + 1, i + 0, 2); if (nRet) return 1;
				nRet = BurnLoadRom(Taito68KRom1 + Offset + 0, i + 1, 2); if (nRet) return 1;
			
				BurnDrvGetRomInfo(&ri, i + 0);
				Offset += ri.nLen;
				BurnDrvGetRomInfo(&ri, i + 1);
				Offset += ri.nLen;
				
				i += 2;
			}
			
			if ((ri.nType & 0xff) == TAITO_68KROM1_BYTESWAP_JUMPING) {
				nRet = BurnLoadRom(Taito68KRom1 + Offset + 0, i + 0, 2); if (nRet) return 1;
			
				BurnDrvGetRomInfo(&ri, i + 0);
				Offset += ri.nLen;
			
				i++;
			}
			
			if ((ri.nType & 0xff) == TAITO_68KROM1) {
				nRet = BurnLoadRom(Taito68KRom1 + Offset, i, 1); if (nRet) return 1;
			
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
				
				i++;
			}
		}
		
		if (Taito68KRom2Size) {
			Offset = 0;
			i = Taito68KRom1Num;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_68KROM2_BYTESWAP) {
					nRet = BurnLoadRom(Taito68KRom2 + Offset + 1, i + 0, 2); if (nRet) return 1;
					nRet = BurnLoadRom(Taito68KRom2 + Offset + 0, i + 1, 2); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
				
					i += 2;
				}
				
				if ((ri.nType & 0xff) == TAITO_68KROM2) {
					nRet = BurnLoadRom(Taito68KRom2 + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
			}
		}
		
		if (TaitoZ80Rom1Size) {
			Offset = 0;
			for (i = Taito68KRom1Num + Taito68KRom2Num; i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num; i++) {
				BurnLoadRom(TaitoZ80Rom1 + Offset, i, 1);
				
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		if (TaitoZ80Rom2Size) {
			Offset = 0;
			for (i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num; i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num; i++) {
				BurnLoadRom(TaitoZ80Rom2 + Offset, i, 1);
				
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		if (TaitoCharRomSize) {
			unsigned char *TempRom = (unsigned char*)malloc(TaitoCharRomSize);
			memset(TempRom, 0, TaitoCharRomSize);
			
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_CHARS) {
					nRet = BurnLoadRom(TempRom + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
				
				if ((ri.nType & 0xff) == TAITO_CHARS_BYTESWAP) {
					nRet = BurnLoadRom(TempRom + Offset + 0, i + 0, 2); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 1, i + 1, 2); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
				
					i += 2;
				}
			}
			
			GfxDecode(TaitoNumChar, TaitoCharNumPlanes, TaitoCharWidth, TaitoCharHeight, TaitoCharPlaneOffsets, TaitoCharXOffsets, TaitoCharYOffsets, TaitoCharModulo, TempRom, TaitoChars);
			
			free(TempRom);
		}
		
		if (TaitoCharBRomSize) {
			unsigned char *TempRom = (unsigned char*)malloc(TaitoCharBRomSize);
			memset(TempRom, 0, TaitoCharBRomSize);
			
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_CHARSB) {
					nRet = BurnLoadRom(TempRom + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
				
				if ((ri.nType & 0xff) == TAITO_CHARSB_BYTESWAP) {
					nRet = BurnLoadRom(TempRom + Offset + 0, i + 0, 2); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 1, i + 1, 2); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
				
					i += 2;
				}
			}
			
			GfxDecode(TaitoNumCharB, TaitoCharBNumPlanes, TaitoCharBWidth, TaitoCharBHeight, TaitoCharBPlaneOffsets, TaitoCharBXOffsets, TaitoCharBYOffsets, TaitoCharBModulo, TempRom, TaitoCharsB);
			
			free(TempRom);
		}
		
		if (TaitoSpriteARomSize) {
			unsigned char *TempRom = (unsigned char*)malloc(TaitoSpriteARomSize);
			memset(TempRom, 0, TaitoSpriteARomSize);
			
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_SPRITESA) {
					nRet = BurnLoadRom(TempRom + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
				
				if ((ri.nType & 0xff) == TAITO_SPRITESA_BYTESWAP) {
					nRet = BurnLoadRom(TempRom + Offset + 0, i + 0, 2); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 1, i + 1, 2); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
				
					i += 2;
				}
				
				if ((ri.nType & 0xff) == TAITO_SPRITESA_BYTESWAP32) {
					nRet = BurnLoadRom(TempRom + Offset + 0, i + 0, 4); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 1, i + 1, 4); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 2, i + 2, 4); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 3, i + 3, 4); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 2);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 3);
					Offset += ri.nLen;
				
					i += 4;
				}
			}
			
			if (TaitoSpriteAInvertRom) {
				for (unsigned int j = 0; j < TaitoSpriteARomSize; j++) {
					TempRom[j] ^= 0xff;
				}
			}
			
			GfxDecode(TaitoNumSpriteA, TaitoSpriteANumPlanes, TaitoSpriteAWidth, TaitoSpriteAHeight, TaitoSpriteAPlaneOffsets, TaitoSpriteAXOffsets, TaitoSpriteAYOffsets, TaitoSpriteAModulo, TempRom, TaitoSpritesA);
			
			free(TempRom);
		}
		
		if (TaitoSpriteBRomSize) {
			unsigned char *TempRom = (unsigned char*)malloc(TaitoSpriteBRomSize);
			memset(TempRom, 0, TaitoSpriteBRomSize);
			
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_SPRITESB) {
					nRet = BurnLoadRom(TempRom + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
				
				if ((ri.nType & 0xff) == TAITO_SPRITESB_BYTESWAP) {
					nRet = BurnLoadRom(TempRom + Offset + 0, i + 0, 2); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 1, i + 1, 2); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
				
					i += 2;
				}
				
				if ((ri.nType & 0xff) == TAITO_SPRITESB_BYTESWAP32) {
					nRet = BurnLoadRom(TempRom + Offset + 0, i + 0, 4); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 1, i + 1, 4); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 2, i + 2, 4); if (nRet) return 1;
					nRet = BurnLoadRom(TempRom + Offset + 3, i + 3, 4); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 2);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 3);
					Offset += ri.nLen;
				
					i += 4;
				}
			}
			
			GfxDecode(TaitoNumSpriteB, TaitoSpriteBNumPlanes, TaitoSpriteBWidth, TaitoSpriteBHeight, TaitoSpriteBPlaneOffsets, TaitoSpriteBXOffsets, TaitoSpriteBYOffsets, TaitoSpriteBModulo, TempRom, TaitoSpritesB);
			
			free(TempRom);
		}
		
		if (TaitoRoadRomSize) {
			Offset = 0;
			for (i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum; i < Taito68KRom1Num + Taito68KRom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum; i++) {
				BurnLoadRom(TC0150RODRom + Offset, i, 1);
				
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		if (TaitoSpriteMapRomSize) {
			Offset = 0;
			for (i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum; i < Taito68KRom1Num + Taito68KRom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum; i++) {
				BurnLoadRom(TaitoSpriteMapRom + Offset, i, 1);
				
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		if (TaitoYM2610ARomSize) {
			Offset = 0;
			for (i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum; i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum; i++) {
				BurnLoadRom(TaitoYM2610ARom + Offset, i, 1);
				
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		if (TaitoYM2610BRomSize) {
			Offset = 0;
			for (i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum; i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum; i++) {
				BurnLoadRom(TaitoYM2610BRom + Offset, i, 1);
				
				BurnDrvGetRomInfo(&ri, i);
				Offset += ri.nLen;
			}
		}
		
		if (TaitoMSM5205RomSize) {
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum + TaitoMSM5205RomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_MSM5205) {
					nRet = BurnLoadRom(MSM5205ROM + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
				
				if ((ri.nType & 0xff) == TAITO_MSM5205_BYTESWAP) {
					nRet = BurnLoadRom(MSM5205ROM + Offset + 0, i + 0, 2); if (nRet) return 1;
					nRet = BurnLoadRom(MSM5205ROM + Offset + 1, i + 1, 2); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i + 0);
					Offset += ri.nLen;
					BurnDrvGetRomInfo(&ri, i + 1);
					Offset += ri.nLen;
				
					i += 2;
				}
			}
		}
		
		if (TaitoCharPivotRomSize) {
			unsigned char *TempRom = (unsigned char*)malloc(TaitoCharPivotRomSize);
			memset(TempRom, 0, TaitoCharPivotRomSize);
			
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum + TaitoMSM5205RomNum;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum + TaitoMSM5205RomNum + TaitoCharPivotRomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_CHARS_PIVOT) {
					nRet = BurnLoadRom(TempRom + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
			}
			
			GfxDecode(TaitoNumCharPivot, TaitoCharPivotNumPlanes, TaitoCharPivotWidth, TaitoCharPivotHeight, TaitoCharPivotPlaneOffsets, TaitoCharPivotXOffsets, TaitoCharPivotYOffsets, TaitoCharPivotModulo, TempRom, TaitoCharsPivot);
			
			free(TempRom);
		}
		
		if (TaitoMSM6295RomSize) {
			Offset = 0;
			i = Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum + TaitoMSM5205RomNum + TaitoCharPivotRomNum;
			
			while (i < Taito68KRom1Num + Taito68KRom2Num + TaitoZ80Rom1Num + TaitoZ80Rom2Num + TaitoCharRomNum + TaitoCharBRomNum + TaitoSpriteARomNum + TaitoSpriteBRomNum + TaitoRoadRomNum + TaitoSpriteMapRomNum + TaitoYM2610ARomNum + TaitoYM2610BRomNum + TaitoMSM5205RomNum + TaitoCharPivotRomNum + TaitoMSM6295RomNum) {
				BurnDrvGetRomInfo(&ri, i + 0);
				
				if ((ri.nType & 0xff) == TAITO_MSM6295) {
					nRet = BurnLoadRom(TaitoMSM6295Rom + Offset, i, 1); if (nRet) return 1;
				
					BurnDrvGetRomInfo(&ri, i);
					Offset += ri.nLen;
				
					i++;
				}
			}
		}
	}
	
	return 0;
}

int TaitoExit()
{
	int i;

	if (TaitoNum68Ks) SekExit();
	if (TaitoNumZ80s) ZetExit();
	if (TaitoNumYM2610) BurnYM2610Exit();
	if (TaitoNumYM2151) BurnYM2151Exit();
	if (TaitoNumYM2203) BurnYM2203Exit();
	for (i = 0; i < TaitoNumMSM5205; i++) {
		MSM5205Exit(i);
	}
	for (i = 0; i < TaitoNumMSM6295; i++) {
		MSM6295Exit(i);
	}
	if (TaitoNumEEPROM) EEPROMExit();
	
	TaitoICExit();
	
	GenericTilesExit();
	BurnGunExit();
	
	free(TaitoMem);
	TaitoMem = NULL;

	Taito68KRom1Num = 0;
	Taito68KRom2Num = 0;
	TaitoZ80Rom1Num = 0;
	TaitoZ80Rom2Num = 0;
	TaitoCharRomNum = 0;
	TaitoCharBRomNum = 0;
	TaitoCharPivotRomNum = 0;
	TaitoSpriteARomNum = 0;
	TaitoSpriteBRomNum = 0;
	TaitoRoadRomNum = 0;
	TaitoSpriteMapRomNum = 0;
	TaitoYM2610ARomNum = 0;
	TaitoYM2610BRomNum = 0;
	TaitoMSM5205RomNum = 0;
	TaitoMSM6295RomNum = 0;

	Taito68KRom1Size = 0;
	Taito68KRom2Size = 0;
	TaitoZ80Rom1Size = 0;
	TaitoZ80Rom2Size = 0;
	TaitoCharRomSize = 0;
	TaitoCharBRomSize = 0;
	TaitoCharPivotRomSize = 0;
	TaitoSpriteARomSize = 0;
	TaitoSpriteBRomSize = 0;
	TaitoRoadRomSize = 0;
	TaitoSpriteMapRomSize = 0;
	TaitoYM2610ARomSize = 0;
	TaitoYM2610BRomSize = 0;
	TaitoMSM5205RomSize = 0;
	TaitoMSM6295RomSize = 0;
	
	TaitoCharModulo = 0;
	TaitoCharNumPlanes = 0;
	TaitoCharWidth = 0;
	TaitoCharHeight = 0;
	TaitoNumChar = 0;
	TaitoCharPlaneOffsets = NULL;
	TaitoCharXOffsets = NULL;
	TaitoCharYOffsets = NULL;
	
	TaitoCharBModulo = 0;
	TaitoCharBNumPlanes = 0;
	TaitoCharBWidth = 0;
	TaitoCharBHeight = 0;
	TaitoNumCharB = 0;
	TaitoCharBPlaneOffsets = NULL;
	TaitoCharBXOffsets = NULL;
	TaitoCharBYOffsets = NULL;
	
	TaitoCharPivotModulo = 0;
	TaitoCharPivotNumPlanes = 0;
	TaitoCharPivotWidth = 0;
	TaitoCharPivotHeight = 0;
	TaitoNumCharPivot = 0;
	TaitoCharPivotPlaneOffsets = NULL;
	TaitoCharPivotXOffsets = NULL;
	TaitoCharPivotYOffsets = NULL;

	TaitoSpriteAModulo = 0;
	TaitoSpriteANumPlanes = 0;
	TaitoSpriteAWidth = 0;
	TaitoSpriteAHeight = 0;
	TaitoNumSpriteA = 0;
	TaitoSpriteAPlaneOffsets = NULL;
	TaitoSpriteAXOffsets = NULL;
	TaitoSpriteAYOffsets = NULL;
	TaitoSpriteAInvertRom = 0;
	
	TaitoSpriteBModulo = 0;
	TaitoSpriteBNumPlanes = 0;
	TaitoSpriteBWidth = 0;
	TaitoSpriteBHeight = 0;
	TaitoNumSpriteB = 0;
	TaitoSpriteBPlaneOffsets = NULL;
	TaitoSpriteBXOffsets = NULL;
	TaitoSpriteBYOffsets = NULL;
	
	TaitoZ80Bank = 0;
	TaitoSoundLatch = 0;
	TaitoRoadPalBank = 0;
	TaitoCpuACtrl = 0;
	
	TaitoXOffset = 0;
	TaitoYOffset = 0;
	TaitoIrqLine = 0;
	TaitoFrameInterleave = 0;
	TaitoFlipScreenX = 0;
	
	TaitoNum68Ks = 0;
	TaitoNumZ80s = 0;
	TaitoNumYM2610 = 0;
	TaitoNumYM2151 = 0;
	TaitoNumYM2203 = 0;
	TaitoNumMSM5205 = 0;
	TaitoNumMSM6295 = 0;
	TaitoNumEEPROM = 0;
	
	TaitoDrawFunction = NULL;
	TaitoMakeInputsFunction = NULL;
	TaitoResetFunction = NULL;
	
	return 0;
}
