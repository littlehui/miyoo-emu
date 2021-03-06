// TC0480SCP

#include "tiles_generic.h"
#include "taito_ic.h"

unsigned char *TC0480SCPRam = NULL;
static unsigned char *TC0480SCPChars;
UINT16 TC0480SCPCtrl[0x18];
static int BgScrollX[4];
static int BgScrollY[4];
static int CharScrollX;
static int CharScrollY;
static int TC0480SCPTilesNum;
static int TC0480SCPPriReg;
static int TC0480SCPXOffset;
static int TC0480SCPYOffset;
static int TC0480SCPTextXOffset;
static int TC0480SCPTextYOffset;
static unsigned short *pTC0480SCPTempDraw = NULL;
static int TC0480SCPColBase;

static int TC0480SCPYVisOffset;

static const UINT16 TC0480SCPBgPriLookup[8] =
{
	0x0123,
	0x1230,
	0x2301,
	0x3012,
	0x3210,
	0x2103,
	0x1032,
	0x0321
};

#define PLOTPIXEL(x, po) pPixel[x] = nPalette | pTileData[x] | po;
#define PLOTPIXEL_FLIPX(x, a, po) pPixel[x] = nPalette | pTileData[a] | po;
#define PLOTPIXEL_MASK(x, mc, po) if (pTileData[x] != mc) {pPixel[x] = nPalette | pTileData[x] | po;}
#define PLOTPIXEL_MASK_FLIPX(x, a, mc, po) if (pTileData[a] != mc) {pPixel[x] = nPalette | pTileData[a] | po;}
#define CLIPPIXEL(x, sx, mx, a) if ((sx + x) >= 0 && (sx + x) < mx) { a; };

static void RenderTile(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + (StartY * 512) + StartX;

	for (int y = 0; y < 16; y++, pPixel += 512, pTileData += 16) {
		PLOTPIXEL( 0, nPaletteOffset);
		PLOTPIXEL( 1, nPaletteOffset);
		PLOTPIXEL( 2, nPaletteOffset);
		PLOTPIXEL( 3, nPaletteOffset);
		PLOTPIXEL( 4, nPaletteOffset);
		PLOTPIXEL( 5, nPaletteOffset);
		PLOTPIXEL( 6, nPaletteOffset);
		PLOTPIXEL( 7, nPaletteOffset);
		PLOTPIXEL( 8, nPaletteOffset);
		PLOTPIXEL( 9, nPaletteOffset);
		PLOTPIXEL(10, nPaletteOffset);
		PLOTPIXEL(11, nPaletteOffset);
		PLOTPIXEL(12, nPaletteOffset);
		PLOTPIXEL(13, nPaletteOffset);
		PLOTPIXEL(14, nPaletteOffset);
		PLOTPIXEL(15, nPaletteOffset);
	}
}

static void RenderTile_FlipX(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + (StartY * 512) + StartX;

	for (int y = 0; y < 16; y++, pPixel += 512, pTileData += 16) {
		PLOTPIXEL_FLIPX(15,  0, nPaletteOffset);
		PLOTPIXEL_FLIPX(14,  1, nPaletteOffset);
		PLOTPIXEL_FLIPX(13,  2, nPaletteOffset);
		PLOTPIXEL_FLIPX(12,  3, nPaletteOffset);
		PLOTPIXEL_FLIPX(11,  4, nPaletteOffset);
		PLOTPIXEL_FLIPX(10,  5, nPaletteOffset);
		PLOTPIXEL_FLIPX( 9,  6, nPaletteOffset);
		PLOTPIXEL_FLIPX( 8,  7, nPaletteOffset);
		PLOTPIXEL_FLIPX( 7,  8, nPaletteOffset);
		PLOTPIXEL_FLIPX( 6,  9, nPaletteOffset);
		PLOTPIXEL_FLIPX( 5, 10, nPaletteOffset);
		PLOTPIXEL_FLIPX( 4, 11, nPaletteOffset);
		PLOTPIXEL_FLIPX( 3, 12, nPaletteOffset);
		PLOTPIXEL_FLIPX( 2, 13, nPaletteOffset);
		PLOTPIXEL_FLIPX( 1, 14, nPaletteOffset);
		PLOTPIXEL_FLIPX( 0, 15, nPaletteOffset);
	}
}

static void RenderTile_FlipY(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + ((StartY + 15) * 512) + StartX;

	for (int y = 15; y >= 0; y--, pPixel -= 512, pTileData += 16) {
		PLOTPIXEL( 0, nPaletteOffset);
		PLOTPIXEL( 1, nPaletteOffset);
		PLOTPIXEL( 2, nPaletteOffset);
		PLOTPIXEL( 3, nPaletteOffset);
		PLOTPIXEL( 4, nPaletteOffset);
		PLOTPIXEL( 5, nPaletteOffset);
		PLOTPIXEL( 6, nPaletteOffset);
		PLOTPIXEL( 7, nPaletteOffset);
		PLOTPIXEL( 8, nPaletteOffset);
		PLOTPIXEL( 9, nPaletteOffset);
		PLOTPIXEL(10, nPaletteOffset);
		PLOTPIXEL(11, nPaletteOffset);
		PLOTPIXEL(12, nPaletteOffset);
		PLOTPIXEL(13, nPaletteOffset);
		PLOTPIXEL(14, nPaletteOffset);
		PLOTPIXEL(15, nPaletteOffset);
	}
}

static void RenderTile_FlipXY(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + ((StartY + 15) * 512) + StartX;

	for (int y = 15; y >= 0; y--, pPixel -= 512, pTileData += 16) {
		PLOTPIXEL_FLIPX(15,  0, nPaletteOffset);
		PLOTPIXEL_FLIPX(14,  1, nPaletteOffset);
		PLOTPIXEL_FLIPX(13,  2, nPaletteOffset);
		PLOTPIXEL_FLIPX(12,  3, nPaletteOffset);
		PLOTPIXEL_FLIPX(11,  4, nPaletteOffset);
		PLOTPIXEL_FLIPX(10,  5, nPaletteOffset);
		PLOTPIXEL_FLIPX( 9,  6, nPaletteOffset);
		PLOTPIXEL_FLIPX( 8,  7, nPaletteOffset);
		PLOTPIXEL_FLIPX( 7,  8, nPaletteOffset);
		PLOTPIXEL_FLIPX( 6,  9, nPaletteOffset);
		PLOTPIXEL_FLIPX( 5, 10, nPaletteOffset);
		PLOTPIXEL_FLIPX( 4, 11, nPaletteOffset);
		PLOTPIXEL_FLIPX( 3, 12, nPaletteOffset);
		PLOTPIXEL_FLIPX( 2, 13, nPaletteOffset);
		PLOTPIXEL_FLIPX( 1, 14, nPaletteOffset);
		PLOTPIXEL_FLIPX( 0, 15, nPaletteOffset);
	}
}

static void RenderTile_Mask(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nMaskColour, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);
	
	unsigned short* pPixel = pDestDraw + (StartY * 512) + StartX;

	for (int y = 0; y < 16; y++, pPixel += 512, pTileData += 16) {
		PLOTPIXEL_MASK( 0, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 1, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 2, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 3, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 4, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 5, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 6, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 7, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 8, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 9, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(10, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(11, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(12, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(13, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(14, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(15, nMaskColour, nPaletteOffset);
	}
}

static void RenderTile_Mask_FlipX(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nMaskColour, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + (StartY * 512) + StartX;

	for (int y = 0; y < 16; y++, pPixel += 512, pTileData += 16) {
		PLOTPIXEL_MASK_FLIPX(15,  0, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(14,  1, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(13,  2, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(12,  3, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(11,  4, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(10,  5, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 9,  6, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 8,  7, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 7,  8, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 6,  9, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 5, 10, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 4, 11, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 3, 12, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 2, 13, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 1, 14, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 0, 15, nMaskColour, nPaletteOffset);
	}
}

static void RenderTile_Mask_FlipY(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nMaskColour, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + ((StartY + 15) * 512) + StartX;

	for (int y = 15; y >= 0; y--, pPixel -= 512, pTileData += 16) {
		PLOTPIXEL_MASK( 0, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 1, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 2, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 3, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 4, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 5, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 6, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 7, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 8, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK( 9, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(10, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(11, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(12, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(13, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(14, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK(15, nMaskColour, nPaletteOffset);
	}
}

static void RenderTile_Mask_FlipXY(unsigned short* pDestDraw, int nTileNumber, int StartX, int StartY, int nTilePalette, int nColourDepth, int nMaskColour, int nPaletteOffset, unsigned char *pTile)
{
	UINT32 nPalette = nTilePalette << nColourDepth;
	pTileData = pTile + (nTileNumber << 8);

	unsigned short* pPixel = pDestDraw + ((StartY + 15) * 512) + StartX;

	for (int y = 15; y >= 0; y--, pPixel -= 512, pTileData += 16) {
		PLOTPIXEL_MASK_FLIPX(15,  0, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(14,  1, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(13,  2, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(12,  3, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(11,  4, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX(10,  5, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 9,  6, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 8,  7, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 7,  8, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 6,  9, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 5, 10, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 4, 11, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 3, 12, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 2, 13, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 1, 14, nMaskColour, nPaletteOffset);
		PLOTPIXEL_MASK_FLIPX( 0, 15, nMaskColour, nPaletteOffset);
	}
}

#undef PLOTPIXEL
#undef PLOTPIXEL_FLIPX
#undef PLOTPIXEL_MASK
#undef CLIPPIXEL

void TC0480SCPCtrlWordWrite(int Offset, UINT16 Data)
{
	int Flip = TC0480SCPPriReg & 0x40;

	TC0480SCPCtrl[Offset] = Data;

	switch (Offset) {
		case 0x00:  {
			if (!Flip)  Data = -Data;
			BgScrollX[0] = Data;
			break;
		}

		case 0x01: {
			Data += 4;
			if (!Flip)  Data = -Data;
			BgScrollX[1] = Data;
			break;
		}

		case 0x02: {
			Data += 8;
			if (!Flip)  Data = -Data;
			BgScrollX[2] = Data;
			break;
		}

		case 0x03: {
			Data += 12;
			if (!Flip)  Data = -Data;
			BgScrollX[3] = Data;
			break;
		}

		case 0x04: {
			if (Flip)  Data = -Data;
			BgScrollY[0] = Data;
			break;
		}

		case 0x05: {
			if (Flip)  Data = -Data;
			BgScrollY[1] = Data;
			break;
		}

		case 0x06: {
			if (Flip)  Data = -Data;
			BgScrollY[2] = Data;
			break;
		}

		case 0x07: {
			if (Flip)  Data = -Data;
			BgScrollY[3] = Data;
			break;
		}

		case 0x08:   // bg0 zoom
		case 0x09:   // bg1 zoom
		case 0x0a:   // bg2 zoom
		case 0x0b: {  // bg3 zoom
			break;
		}

		case 0x0c: {
			if (!Flip) Data -= TC0480SCPTextXOffset;
			if (Flip) Data += TC0480SCPTextXOffset;
			CharScrollX = -Data;
			break;
		}

		case 0x0d: {
			if (!Flip) Data -= TC0480SCPTextYOffset + TC0480SCPYVisOffset;
			if (Flip) Data += TC0480SCPTextYOffset + TC0480SCPYVisOffset;
			CharScrollY = -Data;
			break;
		}

		case 0x0f: {
			// Double-width and flip stuff here			
			TC0480SCPPriReg = Data;
			if (Data & 0x40) bprintf(PRINT_NORMAL, _T("Flip\n"));	
			break;
		}
	}
}

static inline void DrawScanLine(int y, const UINT16 *src, int Transparent, int /*Pri*/)
{
	unsigned short* pPixel;
	int Length;
	
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
}

static void TC0480SCPRenderLayer01(int Layer, int Opaque, unsigned char *pSrc)
{
	int mx, my, Attr, Code, Colour, x, y, sx, TileIndex = 0, Offset, Flip, xFlip, yFlip, xZoom, yZoom, i, yIndex, ySrcIndex, RowIndex, xIndex, xStep;
	
	UINT16 *Dst16, *Src16;
	UINT16 ScanLine[512];
	unsigned short *pSrcTileMap = pTC0480SCPTempDraw;
	
	UINT16 *VideoRam = (UINT16*)TC0480SCPRam + 0x0000;
	UINT16 *ScrollRam = (UINT16*)TC0480SCPRam + 0x2000;
	
	if (Layer == 1) {
		VideoRam = (UINT16*)TC0480SCPRam + 0x0800;
		ScrollRam = (UINT16*)TC0480SCPRam + 0x2200;
	}
	
	xZoom = 0x10000 - (TC0480SCPCtrl[0x08 + Layer] & 0xff00);
	yZoom = 0x10000 - (((TC0480SCPCtrl[0x08 + Layer] & 0xff) - 0x7f) * 512);
	
	// Render the tilemap to a buffer
	memset(pTC0480SCPTempDraw, 0, 512 * 512 * sizeof(UINT16));
	for (my = 0; my < 32; my++) {
		for (mx = 0; mx < 32; mx++) {
			Offset = 2 * TileIndex;
			Attr = VideoRam[Offset];
			Code = VideoRam[Offset + 1] & (TC0480SCPTilesNum - 1);
			Colour = Attr & 0xff;
			Flip = (Attr & 0xc000) >> 14;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			
			Colour += TC0480SCPColBase;
		
			x = 16 * mx;
			y = 16 * my;
			
			if (Opaque) {
				if (xFlip) {
					if (yFlip) {
						RenderTile_FlipXY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					} else {
						RenderTile_FlipX(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					}
				} else {
					if (yFlip) {
						RenderTile_FlipY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					} else {
						RenderTile(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						RenderTile_Mask_FlipXY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					} else {
						RenderTile_Mask_FlipX(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					}
				} else {
					if (yFlip) {
						RenderTile_Mask_FlipY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					} else {
						RenderTile_Mask(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					}
				}				
			}
			
			TileIndex++;
		}
	}
	
	sx = ((BgScrollX[Layer] + 15 + Layer * 4) << 16) + ((255 - (TC0480SCPCtrl[0x10 + Layer] & 0xff)) << 8);
	sx += (-TC0480SCPXOffset - 15 - Layer * 4) * xZoom;

	yIndex = (BgScrollY[Layer] << 16) + ((TC0480SCPCtrl[0x14 + Layer] & 0xff) << 8);
	yIndex -= (-TC0480SCPYOffset - TC0480SCPYVisOffset) * yZoom;

	y = 0;
		
	do {
		ySrcIndex = (yIndex >> 16) & 0x1ff;

		RowIndex = ySrcIndex;

		xIndex = sx - ((ScrollRam[RowIndex] << 16)) - ((ScrollRam[RowIndex + 0x800] << 8) & 0xffff);

		Src16 = pSrcTileMap + (ySrcIndex * 512);
		Dst16 = ScanLine;

		xStep = xZoom;

		if (Opaque) {
			for (i = 0; i < 512; i++) {
				*Dst16++ = Src16[(xIndex >> 16) & 0x1ff];
				xIndex += xStep;
			}
		} else {
			for (i = 0; i < 512; i++) {
				int Pix = Src16[(xIndex >> 16) & 0x1ff];
				
				if (Pix) {
					*Dst16++ = Pix;
				} else {
					*Dst16++ = 0x8000;
				}
				xIndex += xStep;
			}
		}

		if (Opaque) {
			DrawScanLine(y, ScanLine, 0, 0);
		} else {
			DrawScanLine(y, ScanLine, 1, 0);
		}
		
		yIndex += yZoom;
		y++;
	} while (y < nScreenHeight);
}

static void TC0480SCPRenderLayer23(int Layer, int Opaque, unsigned char *pSrc)
{
	int mx, my, Attr, Code, Colour, x, y, sx, TileIndex = 0, Offset, Flip, xFlip, yFlip, xZoom, yZoom, i, yIndex, ySrcIndex, RowIndex, RowZoom, xIndex, xStep;
	
	UINT16 *Dst16, *Src16;
	UINT16 ScanLine[512];
	unsigned short *pSrcTileMap = pTC0480SCPTempDraw;

	xZoom = 0x10000 - (TC0480SCPCtrl[0x08 + Layer] & 0xff00);
	yZoom = 0x10000 - (((TC0480SCPCtrl[0x08 + Layer] & 0xff) - 0x7f) * 512);
	
	UINT16 *VideoRam = (UINT16*)TC0480SCPRam + 0x1000;
	UINT16 *ScrollRam = (UINT16*)TC0480SCPRam + 0x2400;
	UINT16 *RowZoomRam = (UINT16*)TC0480SCPRam + 0x3000;
	UINT16 *ColumnRam = (UINT16*)TC0480SCPRam + 0x3400;
	
	if (Layer == 3) {
		VideoRam = (UINT16*)TC0480SCPRam + 0x1800;
		ScrollRam = (UINT16*)TC0480SCPRam + 0x2600;
		RowZoomRam = (UINT16*)TC0480SCPRam + 0x3200;
		ColumnRam = (UINT16*)TC0480SCPRam + 0x3600;
	}

	// Render the tilemap to a buffer
	memset(pTC0480SCPTempDraw, 0, 512 * 512 * sizeof(UINT16));
	for (my = 0; my < 32; my++) {
		for (mx = 0; mx < 32; mx++) {
			Offset = 2 * TileIndex;
			Attr = VideoRam[Offset];
			Code = VideoRam[Offset + 1] & (TC0480SCPTilesNum - 1);
			Colour = Attr & 0xff;
			Flip = (Attr & 0xc000) >> 14;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			
			Colour += TC0480SCPColBase;
		
			x = 16 * mx;
			y = 16 * my;
		
			if (Opaque) {
				if (xFlip) {
					if (yFlip) {
						RenderTile_FlipXY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					} else {
						RenderTile_FlipX(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					}
				} else {
					if (yFlip) {
						RenderTile_FlipY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					} else {
						RenderTile(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, pSrc);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						RenderTile_Mask_FlipXY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					} else {
						RenderTile_Mask_FlipX(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					}
				} else {
					if (yFlip) {
						RenderTile_Mask_FlipY(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					} else {
						RenderTile_Mask(pTC0480SCPTempDraw, Code, x, y, Colour, 4, 0, 0, pSrc);
					}
				}				
			}
			
			TileIndex++;
		}
	}

	sx = ((BgScrollX[Layer] + 15 + Layer * 4) << 16) + ((255 - (TC0480SCPCtrl[0x10 + Layer] & 0xff)) << 8);
	sx += (-TC0480SCPXOffset - 15 - Layer * 4) * xZoom;

	yIndex = (BgScrollY[Layer] << 16) + ((TC0480SCPCtrl[0x14 + Layer] & 0xff) << 8);
	yIndex -= (-TC0480SCPYOffset - TC0480SCPYVisOffset) * yZoom;

	y = 0;
	
	do {
		ySrcIndex = ((yIndex >> 16) + ColumnRam[(y - TC0480SCPYOffset) & 0x1ff]) &0x1ff;

		RowIndex = ySrcIndex;

		if (TC0480SCPPriReg & (Layer - 1)) {
			RowZoom = RowZoomRam[RowIndex];
		} else {
			RowZoom = 0;
		}

		xIndex = sx - ((ScrollRam[RowIndex] << 16)) - ((ScrollRam[RowIndex + 0x800] << 8) & 0xffff);
		xIndex -= (TC0480SCPXOffset - 0x1f + Layer * 4) * ((RowZoom & 0xff) << 8);

		xStep = xZoom;
		if (RowZoom) {
			if (!(RowZoom & 0xff00)) {
				xStep -= ((RowZoom * 256) & 0xffff);
			} else {
				xStep -= (((RowZoom & 0xff) * 256) & 0xffff);
			}
		}

		Src16 = pSrcTileMap + (ySrcIndex * 512);
		Dst16 = ScanLine;

		if (Opaque) {
			for (i = 0; i < 512; i++) {
				*Dst16++ = Src16[(xIndex >> 16) & 0x1ff];
				xIndex += xStep;
			}
		} else	{
			for (i = 0; i < 512; i++) {
				int Pix = Src16[(xIndex >> 16) & 0x1ff];
					
				if (Pix) {
					*Dst16++ = Pix;
				} else {
					*Dst16++ = 0x8000;
				}
				xIndex += xStep;
			}
		}

		if (Opaque) {
			DrawScanLine(y, ScanLine, 0, 0);
		} else {
			DrawScanLine(y, ScanLine, 1, 0);
		}

		yIndex += yZoom;
		y++;
	} while (y < nScreenHeight);
}

void TC0480SCPTilemapRender(int Layer, int Opaque, unsigned char *pSrc)
{
	switch (Layer) {
		case 0: {
			TC0480SCPRenderLayer01(0, Opaque, pSrc);
			break;
		}
		
		case 1: {
			TC0480SCPRenderLayer01(1, Opaque, pSrc);
			break;
		}
		
		case 2: {
			TC0480SCPRenderLayer23(2, Opaque, pSrc);
			break;
		}
		
		case 3: {
			TC0480SCPRenderLayer23(3, Opaque, pSrc);
			break;
		}
	}
}

static int TC0480SCPPlaneOffsets[4] = { 0, 1, 2, 3 };
static int TC0480SCPXOffsets[8]     = { 4, 0, 12, 8, 20, 16, 28, 24 };
static int TC0480SCPYOffsets[8]     = { 0, 32, 64, 96, 128, 160, 192, 224 };

void TC0480SCPRenderCharLayer()
{
	int mx, my, Attr, Code, Colour, x, y, TileIndex = 0, Flip, xFlip, yFlip;
	
	UINT16 *VideoRam = (UINT16*)TC0480SCPRam + 0x6000;
	UINT16 *CharRam = (UINT16*)TC0480SCPRam + 0x7000;
	
	GfxDecode(256, 4, 8, 8, TC0480SCPPlaneOffsets, TC0480SCPXOffsets, TC0480SCPYOffsets, 0x100, (UINT8*)CharRam, TC0480SCPChars);
	
	for (my = 0; my < 64; my++) {
		for (mx = 0; mx < 64; mx++) {
			Attr = VideoRam[TileIndex];
			Code = Attr & 0xff;
			Colour = (Attr & 0x3f00) >> 8;
			Flip = (Attr & 0xc000) >> 14;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
			
			Colour += TC0480SCPColBase;
			
			x = 8 * mx;
			y = 8 * my;
		
			x -= CharScrollX & 0x1ff;
			y -= CharScrollY & 0x1ff;
			if (x < -8) x += 512;
			if (y < -8) y += 512;

			if (x >= 8 && x < (nScreenWidth - 8) && y >= 8 && y < (nScreenHeight - 8)) {
				if (xFlip) {
					if (yFlip) {
						Render8x8Tile_Mask_FlipXY(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					} else {
						Render8x8Tile_Mask_FlipX(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					}
				} else {
					if (yFlip) {
						Render8x8Tile_Mask_FlipY(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					} else {
						Render8x8Tile_Mask(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					} else {
						Render8x8Tile_Mask_FlipX_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					}
				} else {
					if (yFlip) {
						Render8x8Tile_Mask_FlipY_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					} else {
						Render8x8Tile_Mask_Clip(pTransDraw, Code, x, y, Colour, 4, 0, 0, TC0480SCPChars);
					}
				}
			}			
			
			TileIndex++;
		}
	}
}

void TC0480SCPReset()
{
	memset(TC0480SCPChars, 0, 256 * 8 * 8);
	memset(TC0480SCPCtrl, 0, 0x18);
	BgScrollX[0] = BgScrollX[1] = BgScrollX[2] = BgScrollX[3] = 0;
	BgScrollY[0] = BgScrollY[1] = BgScrollY[2] = BgScrollY[3] = 0;
	CharScrollX = 0;
	CharScrollY = 0;
	TC0480SCPPriReg = 0;
}

int TC0480SCPGetBgPriority()
{
	return TC0480SCPBgPriLookup[(TC0480SCPPriReg &0x1c) >> 2];
}

void TC0480SCPInit(int nNumTiles, int Pixels, int xOffset, int yOffset, int xTextOffset, int yTextOffset, int VisYOffset)
{
	TC0480SCPRam = (unsigned char*)malloc(0x10000);
	memset(TC0480SCPRam, 0, 0x10000);
	TC0480SCPChars = (unsigned char*)malloc(256 * 8 * 8);
	memset(TC0480SCPChars, 0, 256 * 8 * 8);
	
	TC0480SCPTilesNum = nNumTiles;
	
	TC0480SCPXOffset = -(xOffset + Pixels);
	TC0480SCPYOffset = yOffset;
	TC0480SCPTextXOffset = xTextOffset - TC0480SCPXOffset + 3;
	TC0480SCPTextYOffset = yTextOffset + TC0480SCPYOffset;
	
	pTC0480SCPTempDraw = (UINT16*)malloc(512 * 512 * sizeof(UINT16));
	memset(pTC0480SCPTempDraw, 0, 512 * 512 * sizeof(UINT16));
	
	TC0480SCPColBase = 0;
	TC0480SCPYVisOffset = VisYOffset;
	
	TaitoIC_TC0480SCPInUse = 1;
}

void TC0480SCPSetColourBase(int Base)
{
	TC0480SCPColBase = Base;
}

void TC0480SCPExit()
{
	free(TC0480SCPRam);
	TC0480SCPRam = NULL;
	
	free(TC0480SCPChars);
	TC0480SCPChars = NULL;
	
	free(pTC0480SCPTempDraw);
	pTC0480SCPTempDraw = NULL;
	
	memset(TC0480SCPCtrl, 0, 0x18);
	BgScrollX[0] = BgScrollX[1] = BgScrollX[2] = BgScrollX[3] = 0;
	BgScrollY[0] = BgScrollY[1] = BgScrollY[2] = BgScrollY[3] = 0;
	CharScrollX = 0;
	CharScrollY = 0;
	TC0480SCPPriReg = 0;
	
	TC0480SCPTilesNum = 0;
	TC0480SCPXOffset = 0;
	TC0480SCPYOffset = 0;
	TC0480SCPTextXOffset = 0;
	TC0480SCPTextYOffset = 0;
	TC0480SCPColBase = 0;
	TC0480SCPYVisOffset = 0;
}

void TC0480SCPScan(int nAction)
{
	struct BurnArea ba;
	
	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = TC0480SCPRam;
		ba.nLen	  = 0x10000;
		ba.szName = "TC0480SCP Ram";
		BurnAcb(&ba);
	}
	
	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(TC0480SCPCtrl);
		SCAN_VAR(BgScrollX);
		SCAN_VAR(BgScrollY);
		SCAN_VAR(CharScrollX);
		SCAN_VAR(CharScrollY);
		SCAN_VAR(TC0480SCPPriReg);
	}
}
