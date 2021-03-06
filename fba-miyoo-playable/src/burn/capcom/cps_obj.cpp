#include "cps.h"
// CPS Objs (sprites)

int nCpsObjectBank;

unsigned char *BootlegSpriteRam = NULL;

int Sf2Hack = 0;

// Our copy of the sprite table
static unsigned char *ObjMem = NULL;

static int nMax = 0;
static int nGetNext = 0;

static int nMaxZValue;
static int nMaxZMask;

static int nZOffset;

// Object frames, so you can lag the Objs by nFrameCount-1 frames
struct ObjFrame {
	int nShiftX, nShiftY;
	unsigned char* Obj;
	int nCount;
};

static int nFrameCount = 0;
static struct ObjFrame of[3];

int CpsObjInit()
{
	nMax = 0x100;				// CPS1 has 256 sprites

	if (Cps == 2) {				// CPS2 has 1024 sprites
		nMax = 0x400;
	}

	nFrameCount = 2;			// CPS2 sprites lagged by 1 frame and double buffered
								// CPS1 sprites lagged by 1 frame

	ObjMem = (unsigned char*)malloc((nMax << 3) * nFrameCount);
	if (ObjMem == NULL) {
		return 1;
	}

	// Set up the frame buffers
	for (int i = 0; i < nFrameCount; i++) {
		of[i].Obj = ObjMem + (nMax << 3) * i;
		of[i].nCount = 0;
	}

	nGetNext=0;

	if (Cps == 2) {
		memset(ZBuf, 0, 384 * 224 * 2);
		nMaxZMask = nZOffset = 0;
		nMaxZValue = 1;
	}

	return 0;
}

int CpsObjExit()
{
	for (int i = 0; i < nFrameCount; i++) {
		of[i].Obj = NULL;
		of[i].nCount = 0;
	}

	free(ObjMem);
	ObjMem = NULL;

	nFrameCount = 0;
	nMax = 0;

	return 0;
}

// Get CPS sprites into Obj
int CpsObjGet()
{
	int i;
	unsigned char *pg, *po;
	struct ObjFrame* pof;
	unsigned char* Get = NULL;

	pof = of + nGetNext;

	pof->nCount = 0;

	po = pof->Obj;
	pof->nShiftX = -0x40;
	pof->nShiftY = -0x10;

	if (Cps == 2) {
		Get = CpsRam708 + ((nCpsObjectBank ^ 1) << 15);		// Select CPS2 sprite buffer
//		Get = CpsRam708 + ((GetCurrentFrame() & 1) << 15);	// Select CPS2 sprite buffer

		pof->nShiftX = -CpsSaveFrg[0][0x9];
		pof->nShiftY = -CpsSaveFrg[0][0xB];
	} else {
		int nOff = *((unsigned short*)(CpsReg + 0x00)) << 8;
		nOff &= 0xfff800;
		Get = CpsFindGfxRam(nOff, 0x800);		
		
		if (Sf2Hack) {
			Get = CpsFindGfxRam(0x910000, 0x800);
		} else {
			if (Dinopic) {
				Get = BootlegSpriteRam + 0x1000;
			}
		}
	}
	
	if (Get==NULL) return 1;

	// Make a copy of all active sprites in the list
	for (pg = Get, i = 0; i < nMax; pg += 8, i++) {
		unsigned short* ps = (unsigned short*)pg;

		if (Cps == 2) {
			if (ps[1] & 0x8000)	{													// end of sprite list?
				break;
			}
			if (ps[0] == 0 && ps[1] == 0x0100 && ps[2] == 0 && ps[3] == 0xff00) {	// Slammasters end of sprite list?
				break;
			}
		} else {
			if (Dinopic) {
				if (ps[1] == 0xff00) {													// end of sprite list
					break;
				}
			} else {
				if (ps[3] == 0xff00) {													// end of sprite list
					break;
				}
			}
		}
		
		if (Dinopic) {
			if (((ps[2] - 461) | ps[1]) == 0) {													// sprite blank
				continue;
			}
		} else {
			if ((ps[0] | ps[3]) == 0) {													// sprite blank
				continue;
			}
		}

		// Okay - this sprite is active:
		memcpy(po, pg, 8); // copy it over

		pof->nCount++;
		po += 8;
	}

	nGetNext++;
	if (nGetNext >= nFrameCount) {
		nGetNext = 0;
	}

	return 0;
}

void CpsObjDrawInit()
{
	nZOffset = nMaxZMask;

	if (nZOffset >= 0xFC00) {
		// The Z buffer might moverflow the next fram, so initialise it
		memset(ZBuf, 0, 384 * 224 * 2);
		nZOffset = 0;
	}

	nMaxZValue = nZOffset + 1;
	nMaxZMask = nZOffset;

	return;
}

int Cps1ObjDraw(int nLevelFrom,int nLevelTo)
{
	int i; unsigned short *ps; int nPsAdd;
	struct ObjFrame *pof;
	(void)nLevelFrom; (void)nLevelTo;

	// Draw the earliest frame we have in history
	pof=of+nGetNext;

	// Point to Obj list
	ps=(unsigned short *)pof->Obj;

	if (!CpsDrawSpritesInReverse) {
		ps+=(pof->nCount-1)<<2; nPsAdd=-4; // CPS1 is reversed
	} else {
		nPsAdd=4;
	}

	// Go through all the Objs
	for (i=0; i<pof->nCount; i++,ps+=nPsAdd) {
		int x,y,n,a,bx,by,dx,dy; int nFlip;

		if (Dinopic) {
			n = ps[0]; a = ps[1]; x = ps[2] - 461; y = 0x2f0 - ps[3];
			bx = 1;
			by = 1;
		} else {
			x = ps[0]; y = ps[1]; n = ps[2]; a = ps[3];
			
			// Find out sprite size
			bx=((a>> 8)&15)+1;
			by=((a>>12)&15)+1;
		}
		
		n = GfxRomBankMapper(GFXTYPE_SPRITES, n);
		if (n == -1) continue;
		
		n |= (y & 0x6000) << 3; // high bits of address
		
		// CPS1 coords are 9 bit signed?
		x&=0x01ff; if (x>=0x1c0) x-=0x200;
		y&=0x01ff; y^=0x100; y-=0x100;

		x+=pof->nShiftX;
		y+=pof->nShiftY;

		// Find the palette for the tiles on this sprite
		CpstPal = CpsObjPal + ((a & 0x1F) << 4);

		nFlip=(a>>5)&3;		

		// Take care with tiles if the sprite goes off the screen
		if (x<0 || y<0 || x+(bx<<4)>384 || y+(by<<4)>224) {
			nCpstType=CTT_16X16 | CTT_CARE;
		} else {
			nCpstType=CTT_16X16;
		}

		nCpstFlip=nFlip;
		for (dy=0;dy<by;dy++) {
			for (dx=0;dx<bx;dx++) {
				int ex,ey;
				if (nFlip&1) ex=(bx-dx-1);
				else ex=dx;
				if (nFlip&2) ey=(by-dy-1);
				else ey=dy;

				nCpstX=x+(ex<<4);
				nCpstY=y+(ey<<4);
				nCpstTile = (n & ~0x0F) + (dy << 4) + ((n + dx) & 0x0F);
				nCpstTile <<= 7;
				CpstOneObjDoX[0]();
			}
		}

	}
	return 0;
}

// Delay sprite drawing by one frame
int Cps2ObjDraw(int nLevelFrom, int nLevelTo)
{
	const int nPsAdd = 4;

	unsigned short *ps;
	struct ObjFrame *pof;
	CpstOneDoFn pCpstOne;
	int nCount;

	bool bMask = 0;

	// Draw the earliest frame we have in history
	pof = of + nGetNext;

	// Point to Obj list
	ps = (unsigned short*)pof->Obj + nPsAdd * (nMaxZValue - nZOffset - 1);
	nCount = nZOffset + pof->nCount;

	// Go through all the Objs
	for (ZValue = (unsigned short)nMaxZValue; ZValue <= nCount; ZValue++, ps += nPsAdd) {
		int x, y, n, a, bx, by, dx, dy;
		int nFlip;
		int v = ps[0] >> 13;

		if ((nSpriteEnable & (1 << v)) == 0) {
			continue;
		}

		// Check if sprite is between these levels
		if (v > nLevelTo) {
			bMask = 1;
			continue;
		}
		if (v < nLevelFrom) {
			continue;
		}

		if (bMask) {
			nMaxZMask = ZValue;
		} else {
			nMaxZValue = ZValue;
		}

		// Select CpstOne function;
		if (bMask || nMaxZMask > nMaxZValue) {
			pCpstOne = CpstOneObjDoX[1];
		} else {
			pCpstOne = CpstOneObjDoX[0];
		}

		x = ps[0];
		y = ps[1];
		n = ps[2];
		a = ps[3];

		if (a & 0x80) {														// marvel vs capcom ending sprite off-set
			x += CpsSaveFrg[0][0x9];
		}

		// CPS2 coords are 10 bit signed (-512 to 511)
		x &= 0x03FF; x ^= 0x200; x -= 0x200;
		y &= 0x03FF; y ^= 0x200; y -= 0x200;

#if 0
		// This *MAY* be needed to get correct sprite positions when raster interrups are used.
		if (nRasterline[1]) {
			for (int i = 1; i < MAX_RASTER; i++) {
				if ((y < nRasterline[i]) || (nRasterline[i] == 0)) {
					x -= CpsSaveFrg[i - 1][0x09];
					y -= CpsSaveFrg[i - 1][0x0B];
					break;
				}
			}
		} else {
			x -= CpsSaveFrg[0][0x9];
			y -= CpsSaveFrg[0][0xB];
		}
#else
		// Ignore sprite offsets when raster interrupts are used (seems to work for all games).
		x += pof->nShiftX;
		y += pof->nShiftY;

//		x -= CpsSaveFrg[0][0x9];
//		y -= CpsSaveFrg[0][0xB];

#endif
		n |= (ps[1] & 0x6000) << 3;	// high bits of address

		// Find the palette for the tiles on this sprite
		CpstPal = CpsObjPal + ((a & 0x1F) << 4);

		nFlip = (a >> 5) & 3;
		// Find out sprite size
		bx = ((a >> 8) & 15) + 1;
		by = ((a >> 12) & 15) + 1;

		// Take care with tiles if the sprite goes off the screen
		if (x < 0 || y < 0 || x + (bx << 4) > 383 || y + (by << 4) > 223) {
			nCpstType = CTT_16X16 | CTT_CARE;
		} else {
			nCpstType = CTT_16X16;
		}

//		if (v == 0) {
//			bprintf(PRINT_IMPORTANT, _T("  - %4i: 0x%04X 0x%04X 0x%04X 0x%04X\n"), ZValue - (unsigned short)nMaxZValue, ps[0], ps[1], ps[2], ps[3]);
//		}

		nCpstFlip = nFlip;
		for (dy = 0; dy < by; dy++) {
			for (dx = 0; dx < bx; dx++) {
				int ex, ey;

				if (nFlip & 1) {
					ex = (bx - dx - 1);
				} else {
					ex = dx;
				}

				if (nFlip & 2) {
					ey = (by - dy - 1);
				} else {
					ey = dy;
				}

				nCpstX = x + (ex << 4);
				nCpstY = y + (ey << 4);

//				nCpstTile = n + (dy << 4) + dx;								// normal version
				nCpstTile = (n & ~0x0F) + (dy << 4) + ((n + dx) & 0x0F);	// pgear fix
				nCpstTile <<= 7;											// Find real tile address

				pCpstOne();
			}
		}
	}

	return 0;
}
