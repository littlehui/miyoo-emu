// TC0150ROD

#include "tiles_generic.h"
#include "taito_ic.h"

unsigned char *TC0150RODRom = NULL;
unsigned char *TC0150RODRam = NULL;
static int TC0150RODFlipScreenX;

static void DrawScanLine(int y, const UINT16 *src, int Transparent, int /*Pri*/)
{
	unsigned short* pPixel;
	int Length;
	
	if (!TC0150RODFlipScreenX) {
		pPixel = pTransDraw + (y * nScreenWidth);
	
		Length = nScreenWidth;
	
		if (Transparent) {
			while (Length--) {
				UINT16 sPixel = *src++;
				if (sPixel < 0x7fff) {
					*pPixel = sPixel;
				}
				pPixel++;
			}
		} else {
			while (Length--) {
				*pPixel++ = *src++;
			}
		}
	} else {
		pPixel = pTransDraw + (y * nScreenWidth) + (nScreenWidth - 1);
	
		Length = nScreenWidth;
	
		if (Transparent) {
			while (Length--) {
				UINT16 sPixel = *src++;
				if (sPixel < 0x7fff) {
					*pPixel = sPixel;
				}
				pPixel--;
			}
		} else {
			while (Length--) {
				*pPixel-- = *src++;
			}
		}
	}
}

void TC0150RODDraw(int yOffs, int pOffs, int Type, int RoadTrans, int LowPriority, int HighPriority)
{
	int xOffs = 0xa7;
	yOffs += 16;
	
	UINT16* RoadRam = (UINT16*)TC0150RODRam;
	
	UINT16 ScanLine[512];
	UINT16 RoadALine[512];
	UINT16 RoadBLine[512];
	UINT16 *Dst16;
	UINT16 *RoadA;
	UINT16 *RoadB;
	UINT16 *RoadGfx = (UINT16*)TC0150RODRom;
	
	UINT16 Pixel, Colour, GfxWord;
	UINT16 RoadAClipL, RoadAClipR, RoadABodyCtrl;
	UINT16 RoadBClipL, RoadBClipR, RoadBBodyCtrl;;
	UINT16 Pri, PixPri;
	UINT8 Priorities[6];
	int xIndex, RoadRamIndex, RoadRam2Index, i;
	int xOffset, PalOffs, PalLOffs, PalROffs;
	int RoadGfxTileNum, ColBank, RoadCentre;
	int RoadCtrl = RoadRam[0xfff];
	int LeftEdge, RightEdge, Begin, End, RightOver, LeftOver;
	int LineNeedsDrawing, DrawTopRoadLine, BackgroundOnly;

	int y = 0;

	int RoadAAddress = yOffs * 4 + ((RoadCtrl & 0x0300) << 2);
	int RoadBAddress = yOffs * 4 + ((RoadCtrl & 0x0c00) << 0);
	
	int PrioritySwitchLine = (RoadCtrl & 0x00ff) - yOffs;

	do {
		LineNeedsDrawing = 0;
		RoadRamIndex = RoadAAddress + (y * 4);
		RoadRam2Index = RoadBAddress + (y * 4);
		
		RoadA = RoadALine;
		RoadB = RoadBLine;
		
		for (i = 0; i < nScreenWidth; i++) {
			*RoadA++ = 0x8000;
			*RoadB++ = 0x8000;
		}
		
		Priorities[0] = 1;
		Priorities[1] = 1;
		Priorities[2] = 2;
		Priorities[3] = 3;
		Priorities[4] = 3;
		Priorities[5] = 1;
		
		RoadAClipR = RoadRam[RoadRamIndex + 0];
		RoadAClipL = RoadRam[RoadRamIndex + 1];
		RoadABodyCtrl = RoadRam[RoadRamIndex + 2];
		RoadBClipR = RoadRam[RoadRam2Index + 0];
		RoadBClipL = RoadRam[RoadRam2Index + 1];
		RoadBBodyCtrl = RoadRam[RoadRam2Index + 2];
		
		if (RoadABodyCtrl & 0x2000) Priorities[2] += 2;
		if (RoadBBodyCtrl & 0x2000) Priorities[2] += 1;
		if (RoadAClipL    & 0x2000) Priorities[3] -= 1;
		if (RoadBClipL    & 0x2000) Priorities[3] -= 2;
		if (RoadAClipR    & 0x2000) Priorities[4] -= 1;
		if (RoadBClipR    & 0x2000) Priorities[4] -= 2;

		if (Priorities[4] == 0)	Priorities[4]++;
		
		// Road A
		PalROffs = (RoadAClipR & 0x1000) >> 11;
		PalLOffs = (RoadAClipL & 0x1000) >> 11;
		
		xOffset  = RoadABodyCtrl & 0x7ff;
		PalOffs  = (RoadABodyCtrl & 0x1800) >> 11;
		ColBank  = (RoadRam[RoadRamIndex + 3] & 0xf000) >> 10;
		RoadGfxTileNum = RoadRam[RoadRamIndex + 3] & 0x3ff;
		RightOver = 0;
		LeftOver = 0;

		RoadCentre = 0x5ff - ((-xOffset + xOffs) & 0x7ff);
		LeftEdge = RoadCentre - (RoadAClipL & 0x3ff);
		RightEdge = RoadCentre + 1 + (RoadAClipR & 0x3ff);

		if (RoadAClipL || RoadAClipR) LineNeedsDrawing = 1;
		
		Begin = LeftEdge + 1;
		if (Begin < 0) Begin = 0;
		
		End = RightEdge;
		if (End > nScreenWidth)	End = nScreenWidth;
		
		if (RightEdge < 0) {
			RightOver = -RightEdge;
			RightEdge = 0;
		}
		
		if (LeftEdge >= nScreenWidth)
		{
			LeftOver = LeftEdge - nScreenWidth + 1;
			LeftEdge = nScreenWidth - 1;
		}

		BackgroundOnly = (RoadCentre > (nScreenWidth - 2 + (1024 / 2))) ? 1 : 0;
		
		Colour = ((pOffs + ColBank + PalOffs) << 4) + ((Type) ? (1) : (4));
		Pri = Priorities[2] << 12;

		xIndex = (-xOffset + xOffs + Begin) & 0x7ff;
		RoadA = RoadALine + nScreenWidth - 1 - Begin;

		if ((LineNeedsDrawing) && (Begin < End)) {
			for (i = Begin; i < End; i++) {
				if (RoadGfxTileNum) {
					GfxWord = RoadGfx[(RoadGfxTileNum << 8) + (xIndex >> 3)];
					Pixel = ((GfxWord >> (7 - (xIndex % 8) + 8)) & 1) * 2 + ((GfxWord >> (7 - (xIndex % 8))) & 1);

					if ((Pixel) || !(RoadTrans)) {
						if (Type) Pixel = (Pixel - 1) & 3;
						*RoadA-- = (Colour + Pixel) | Pri;
					}
					else	*RoadA-- = 0xf000;
				}
				else RoadA--;

				xIndex++;
				xIndex &= 0x7ff;
			}
		}
		
		// Left Edge of Road A
		Colour = ((pOffs + ColBank + PalLOffs) << 4) + ((Type) ? (1) : (4));
		Pri = Priorities[0] << 12;
		
		if (BackgroundOnly) {
			if (RoadAClipL & 0x8000) {
				RoadA = RoadALine;
				for (i = 0; i < nScreenWidth; i++) {
					*RoadA++ = (Colour + (Type ? (3) : (0)));
				}
			}
		} else {
			if ((LeftEdge >= 0) && (LeftEdge < nScreenWidth)) {
				xIndex = ((1024 / 2) - 1 - LeftOver) & 0x7ff;

				RoadA = RoadALine + nScreenWidth - 1 - LeftEdge;

				if (LineNeedsDrawing) {
					for (i = LeftEdge; i >= 0; i--)	{
						GfxWord = RoadGfx[(RoadGfxTileNum << 8) + (xIndex >> 3)];
						Pixel = ((GfxWord >> (7 - (xIndex % 8) + 8)) & 1) * 2 + ((GfxWord >> (7 - (xIndex % 8))) & 1);

						PixPri = (Pixel == 0) ? (0) : (Pri);

						if ((Pixel == 0) && !(RoadAClipL & 0x8000)) {
							RoadA++;
						} else {
							if (Type) Pixel = (Pixel - 1) & 3;
							*RoadA++ = (Colour + Pixel) | PixPri;
						}

						xIndex--;
						xIndex &= 0x7ff;
					}
				}
			}
		}
		
		// Right Edge of Road A
		Colour = ((pOffs + ColBank + PalROffs) << 4) + ((Type) ? (1) : (4));
		Pri = Priorities[1] << 12;
		
		if ((RightEdge < nScreenWidth) && (RightEdge >= 0)) {
			xIndex = ((1024 / 2) + RightOver) & 0x7ff;

			RoadA = RoadALine + nScreenWidth - 1 - RightEdge;

			if (LineNeedsDrawing) {
				for (i = RightEdge; i < nScreenWidth; i++) {
					GfxWord = RoadGfx[(RoadGfxTileNum << 8) + (xIndex >> 3)];
					Pixel = ((GfxWord >> (7 - (xIndex % 8) + 8)) & 1) * 2 + ((GfxWord >> (7 - (xIndex % 8))) & 1);

					PixPri = (Pixel == 0) ? (0) : (Pri);

					if ((Pixel == 0) && !(RoadAClipR & 0x8000)) {
						RoadA--;
					} else {
						if (Type) Pixel = (Pixel - 1) & 3;
						*RoadA-- = (Colour + Pixel) | PixPri;
					}

					xIndex++;
					xIndex &= 0x7ff;
				}
			}
		}
		
		// Road B
		PalROffs = (RoadBClipR & 0x1000) >> 11;
		PalLOffs = (RoadBClipL & 0x1000) >> 11;
		xOffset  =  RoadBBodyCtrl & 0x7ff;
		PalOffs  = (RoadBBodyCtrl & 0x1800) >> 11;
		ColBank  = (RoadRam[RoadRam2Index + 3] & 0xf000) >> 10;
		RoadGfxTileNum = RoadRam[RoadRam2Index + 3] & 0x3ff;
		RightOver = 0;
		LeftOver = 0;

		RoadCentre = 0x5ff - ((-xOffset + xOffs) & 0x7ff);
		
		LeftEdge = RoadCentre - (RoadBClipL & 0x3ff);
		RightEdge = RoadCentre + 1 + (RoadBClipR & 0x3ff);

		if (((RoadBClipL) || (RoadBClipR)) && ((RoadCtrl & 0x800) || (Type == 2))) {
			DrawTopRoadLine = 1;
			LineNeedsDrawing = 1;
		}
		else {
			DrawTopRoadLine = 0;
		}

		Begin = LeftEdge + 1;
		if (Begin < 0) Begin = 0;
		
		End = RightEdge;
		if (End > nScreenWidth) {
			End = nScreenWidth;
		}
		
		if (RightEdge < 0) {
			RightOver = -RightEdge;
			RightEdge = 0;
		}
		
		if (LeftEdge >= nScreenWidth)
		{
			LeftOver = LeftEdge - nScreenWidth + 1;
			LeftEdge = nScreenWidth - 1;
		}

		BackgroundOnly = (RoadCentre > (nScreenWidth - 2 + (1024 / 2))) ? 1 : 0;
		
		Colour = ((pOffs + ColBank + PalOffs) << 4) + ((Type) ? (1) : (4));
		Pri = Priorities[5] << 12;

		xIndex = (-xOffset + xOffs + Begin) & 0x7ff;
		
		if (xIndex > 0x3ff) {
			RoadB = RoadBLine + nScreenWidth - 1 - Begin;

			if (DrawTopRoadLine && RoadGfxTileNum && (Begin < End)) {
				for (i = Begin; i < End; i++) {
					GfxWord = RoadGfx[(RoadGfxTileNum << 8) + (xIndex >> 3)];
					Pixel = ((GfxWord >> (7 - (xIndex % 8) + 8)) & 1) * 2 + ((GfxWord >> (7 - (xIndex % 8))) & 1);

					if ((Pixel) || !(RoadTrans)) {
						if (Type) Pixel = (Pixel - 1) & 3;
						*RoadB-- = (Colour + Pixel) | Pri;
					}
					else	*RoadB-- = 0xf000;

					xIndex++;
					xIndex &= 0x7ff;
				}
			}
		}
		
		// Left Edge of Road B
		Colour = ((pOffs + ColBank + PalLOffs) << 4) + ((Type) ? (1) : (4));
		Pri = Priorities[3] << 12;

		if (BackgroundOnly) {
			if ((RoadBClipL & 0x8000) && DrawTopRoadLine) {
				RoadB = RoadBLine;
				for (i = 0; i < nScreenWidth; i++) {
					*RoadB++ = (Colour + (Type ? (3) : (0)));
				}
			}
		} else {
			if ((LeftEdge >= 0) && (LeftEdge < nScreenWidth)) {
				xIndex = ((1024 / 2) - 1 - LeftOver) & 0x7ff;

				RoadB = RoadBLine + nScreenWidth - 1 - LeftEdge;

				if (LineNeedsDrawing) {
					for (i = LeftEdge; i >= 0; i--)	{
						GfxWord = RoadGfx[(RoadGfxTileNum << 8) + (xIndex >> 3)];
						Pixel = ((GfxWord >> (7 - (xIndex % 8) + 8)) & 1) * 2 + ((GfxWord >> (7 - (xIndex % 8))) & 1);

						PixPri = (Pixel == 0) ? (0) : (Pri);

						if ((Pixel == 0) && !(RoadBClipL & 0x8000)) {
							RoadB++;
						} else {
							if (Type) Pixel = (Pixel - 1) & 3;
							*RoadB++ = (Colour + Pixel) | PixPri;
						}

						xIndex--;
						if (xIndex < 0) break;
					}
				}
			}
		}
		
		// Right Edge of Road B
		Colour = ((pOffs + ColBank + PalROffs) << 4) + ((Type) ? (1) : (4));
		Pri = Priorities[4] << 12;
		
		if ((RightEdge < nScreenWidth) && (RightEdge >= 0)) {
			xIndex = ((1024 / 2) + RightOver) & 0x7ff;

			RoadB = RoadBLine + nScreenWidth - 1 - RightEdge;

			if (DrawTopRoadLine) {
				for (i = RightEdge; i < nScreenWidth; i++) {
					GfxWord = RoadGfx[(RoadGfxTileNum << 8) + (xIndex >> 3)];
					Pixel = ((GfxWord >> (7 - (xIndex % 8) + 8)) & 1) * 2 + ((GfxWord >> (7 - (xIndex % 8))) & 1);

					PixPri = (Pixel == 0) ? (0) : (Pri);

					if ((Pixel == 0) && !(RoadBClipR & 0x8000)) {
						RoadB--;
					} else {
						if (Type) Pixel = (Pixel - 1) & 3;
						*RoadB-- = (Colour + Pixel) | PixPri;
					}

					xIndex++;
					if (xIndex > 0x3ff) break;
				}
			}
		}
		
		if (LineNeedsDrawing) {
			Dst16 = ScanLine;

			for (i = 0; i < nScreenWidth; i++) {
				if (RoadALine[i] == 0x8000) {
					*Dst16++ = RoadBLine[i] & 0x8fff;
				}
				else if (RoadBLine[i] == 0x8000) {
					*Dst16++ = RoadALine[i] & 0x8fff;
				}
				else {
					if ((RoadBLine[i] & 0x7000) > (RoadALine[i] & 0x7000)) {
						*Dst16++ = RoadBLine[i] & 0x8fff;
					} else	{
						*Dst16++ = RoadALine[i] & 0x8fff;
					}
				}
			}
			
			if (y > PrioritySwitchLine) {
				DrawScanLine(y, ScanLine, 1, HighPriority);
			} else {
				DrawScanLine(y, ScanLine, 1, LowPriority);
			}
		}
		
		y++;
	} while (y < nScreenHeight);
}

void TC0150RODReset()
{

}

void TC0150RODInit(int nRomSize, int xFlip)
{
	TC0150RODRom = (unsigned char*)malloc(nRomSize);
	memset(TC0150RODRom, 0, nRomSize);
	TC0150RODRam = (unsigned char*)malloc(0x2000);
	memset(TC0150RODRam, 0, 0x2000);
	
	TC0150RODFlipScreenX = xFlip;
	
	TaitoIC_TC0150RODInUse = 1;
}

void TC0150RODExit()
{
	free(TC0150RODRom);
	TC0150RODRom = NULL;
	
	free(TC0150RODRam);
	TC0150RODRam = NULL;
	
	TC0150RODFlipScreenX = 0;
}

void TC0150RODScan(int nAction)
{
	struct BurnArea ba;
	
	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = TC0150RODRam;
		ba.nLen	  = 0x2000;
		ba.szName = "TC0150ROD Ram";
		BurnAcb(&ba);
	}
}
