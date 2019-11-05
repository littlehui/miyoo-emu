// FB Alpha The NewZealand Story driver module
// Based on MAME driver by Chris Moore, Brad Oliver, Nicola Salmoria, and many others

#include "tiles_generic.h"
#include "burn_ym2203.h"

static unsigned char *Mem, *Rom0, *Rom1, *Rom2, *Gfx;
static unsigned char DrvJoy1[8], DrvJoy2[8], DrvJoy3[8], DrvDips[2], DrvReset;
static unsigned int *Palette;

static int tnzs_bg_flag, tnzs_screenflip;
static int tnzs_cpu1_reset;
static int tnzs_bank0, tnzs_bank1;
static int soundlatch;

static struct BurnInputInfo DrvInputList[] = {
	{"P1 Coin"    ,   BIT_DIGITAL  , DrvJoy3 + 4,	"p1 coin"  },
	{"P2 Coin"    ,   BIT_DIGITAL  , DrvJoy3 + 5,	"p2 coin"  },

	{"P1 Start"  ,    BIT_DIGITAL  , DrvJoy1 + 7,	"p1 start" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 0, 	"p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 1, 	"p1 right" },
	{"P1 Up",	  BIT_DIGITAL,   DrvJoy1 + 2,   "p1 up"    },
	{"P1 Down",	  BIT_DIGITAL,   DrvJoy1 + 3,   "p1 down", },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 fire 2"},

	{"P2 Start"  ,    BIT_DIGITAL  , DrvJoy2 + 7,	"p2 start" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 0, 	"p2 left"  },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 1, 	"p2 right" },
	{"P2 Up",	  BIT_DIGITAL,   DrvJoy2 + 2,   "p2 up"    },
	{"P2 Down",	  BIT_DIGITAL,   DrvJoy2 + 3,   "p2 down", },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy2 + 5,	"p2 fire 2"},

	{"Service Mode",  BIT_DIGITAL,   DrvJoy3 + 0,   "diag"     },
	{"Tilt",          BIT_DIGITAL,   DrvJoy3 + 1,   "tilt"     },

	{"Reset"        , BIT_DIGITAL  , &DrvReset  ,	"reset"    },
	{"Dip 1"     ,    BIT_DIPSWITCH, DrvDips + 0 ,	"dip 1"	   },
	{"Dip 2"     ,    BIT_DIPSWITCH, DrvDips + 1 ,	"dip 2"	   },
};

STDINPUTINFO(Drv);

static struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xfe, NULL                     },

	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x13, 0x01, 0x01, 0x00, "Upright"     		  },
	{0x13, 0x01, 0x01, 0x01, "Cocktail"    		  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x13, 0x01, 0x02, 0x02, "Off"     		  },
	{0x13, 0x01, 0x02, 0x00, "On"    		  },

	{0   , 0xfe, 0   , 2   , "Invulnerability (Debug)"},
	{0x13, 0x01, 0x08, 0x08, "Off"     		  },
	{0x13, 0x01, 0x08, 0x00, "On"			  },

	{0   , 0xfe, 0   , 4   , "Coin A"                 },
	{0x13, 0x01, 0x30, 0x00, "4C 1C"     		  },
	{0x13, 0x01, 0x30, 0x10, "3C 1C"    		  },
	{0x13, 0x01, 0x30, 0x20, "2C 1C"     		  },
	{0x13, 0x01, 0x30, 0x30, "1C 1C"    		  },

	{0   , 0xfe, 0   , 4   , "Coin A"                 },
	{0x13, 0x01, 0xc0, 0xc0, "1C 2C"     		  },
	{0x13, 0x01, 0xc0, 0x80, "1C 3C"    		  },
	{0x13, 0x01, 0xc0, 0x40, "1C 4C"     		  },
	{0x13, 0x01, 0xc0, 0x00, "1C 5C"    		  },

	// Default Values
	{0x14, 0xff, 0xff, 0xff, NULL                     },

	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x14, 0x01, 0x03, 0x02, "Easy"     		  },
	{0x14, 0x01, 0x03, 0x03, "Medium"    		  },
	{0x14, 0x01, 0x03, 0x01, "Hard"     		  },
	{0x14, 0x01, 0x03, 0x00, "Hardest"    		  },

	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x14, 0x01, 0x0c, 0x00, "50000 150000"		  },
	{0x14, 0x01, 0x0c, 0x0c, "70000 200000"		  },
	{0x14, 0x01, 0x0c, 0x04, "100000 250000"	  },
	{0x14, 0x01, 0x0c, 0x08, "200000 300000"    	  },

	{0   , 0xfe, 0   , 4   , "Lives"		  },
	{0x14, 0x01, 0x30, 0x20, "2"     		  },
	{0x14, 0x01, 0x30, 0x30, "3"			  },
	{0x14, 0x01, 0x30, 0x00, "4"     		  },
	{0x14, 0x01, 0x30, 0x10, "5"			  },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x14, 0x01, 0x40, 0x00, "No"     		  },
	{0x14, 0x01, 0x40, 0x40, "Yes"    		  },
};

STDDIPINFO(Drv);

void __fastcall tnzs_bankswitch_w(unsigned char data)
{
	if (~data & 0x10) {
		tnzs_cpu1_reset = 1;
	}

	tnzs_bank0 = (data & 7) | 0x10;

	ZetMapArea(0x8000, 0xbfff, 0, Rom0 + 0x10000 + 0x4000 * (data & 7));
	ZetMapArea(0x8000, 0xbfff, 1, Rom0 + 0x10000 + 0x4000 * (data & 7));
	ZetMapArea(0x8000, 0xbfff, 2, Rom0 + 0x10000 + 0x4000 * (data & 7));
}

void __fastcall tnzs_bankswitch1_w(unsigned char data)
{
	tnzs_bank1 = data & 3;

	ZetMapArea(0x8000, 0x9fff, 0, Rom1 + 0x10000 + 0x2000 * (data & 3));
	ZetMapArea(0x8000, 0x9fff, 2, Rom1 + 0x10000 + 0x2000 * (data & 3));
}

void __fastcall tnzs_cpu0_write(unsigned short address, unsigned char data)
{
	if (address > 0xf7ff) return;

	switch (address)
	{
		case 0xf400:
			tnzs_bg_flag = data;
		break;

		case 0xf600:
			tnzs_bankswitch_w(data);
		break;
	}

	if (address >= 0xe000 && address <= 0xefff)
	{
		if (address == 0xef10 && data == 0xff) {
			data = 0; // probably not right...
		}

		Rom0[address] = data;
		return;
	}

	if (address >= 0xf300 && address <= 0xf3ff)
	{
		Rom0[address & 0xff03] = data;
		return;
	}
}

inline static unsigned char pal5bit(unsigned char bits)
{
	bits &= 0x1f;
	return (bits << 3) | (bits >> 2);
}

void __fastcall tnzs_cpu1_write(unsigned short address, unsigned char data)
{
	switch (address)
	{
		case 0xa000:
			tnzs_bankswitch1_w(data);
		break;

		case 0xb004:
			soundlatch = data;
			ZetClose();
			ZetOpen(2);
			ZetRaiseIrq(0);
			ZetClose();
			ZetOpen(0);
		break;
	}

	if (address >= 0xf000 && address <= 0xf3ff)
	{
		Rom1[address & 0xf3ff] = data;
	
		unsigned short pal = (Rom1[address | 1] << 8) | (Rom1[address & ~1]);

		Palette[(address >> 1) & 0x3ff] = (pal5bit(pal >> 10) << 16) | (pal5bit(pal >> 5) << 8) | pal5bit(pal);
		return;
	}
}

unsigned char __fastcall tnzs_cpu1_read(unsigned short address)
{
	switch (address)
	{
		case 0xb002:
			return DrvDips[0];

		case 0xb003:
			return DrvDips[1];

		// lhld can't use handlers, so use a MapArea instead
		case 0xc000: // inp2
		case 0xc001: // inp3
		case 0xc002: // inp4
			return 0;
	}

	if (address >= 0xf000 && address <= 0xf003) {
		return Rom1[address];
	}

	return 0;
}

void __fastcall tnzs_cpu2_out(unsigned short port, unsigned char data)
{
	switch (port & 0xff)
	{
		case 0x00:
			BurnYM2203Write(0, 0, data);
		break;

		case 0x01:
			BurnYM2203Write(0, 1, data);
		break;
	}
}

unsigned char __fastcall tnzs_cpu2_in(unsigned short port)
{
	switch (port & 0xff)
	{
		case 0x00:
			return BurnYM2203Read(0, 0);

		case 0x02:
			return soundlatch;
	}

	return 0;
}

inline static void DrvYM2203IRQHandler(int, int nStatus)
{
	if (nStatus & 1) {
		ZetNmi();
	} else {
		ZetSetIRQLine(0,    ZET_IRQSTATUS_NONE);
	}
}

inline static int DrvSynchroniseStream(int nSoundRate)
{
	return (long long)ZetTotalCycles() * nSoundRate / 6000000;
}

inline static double DrvGetTime()
{
	return (double)ZetTotalCycles() / 6000000;
}

static int DrvDoReset()
{
	memset (Palette, 0, 0x200 * sizeof(int));
	memset (Rom0 + 0x0c000, 0, 0x2000);
	memset (Rom0 + 0x0e000, 0, 0x1000);
	memset (Rom0 + 0x0f000, 0, 0x0400);
	memset (Rom0 + 0x10000, 0, 0x8000);
	memset (Rom1 + 0x0d000, 0, 0x1000);
	memset (Rom1 + 0x0f000, 0, 0x0400);
	memset (Rom2 + 0x0c000, 0, 0x2000);

	tnzs_bg_flag = tnzs_screenflip = 0;
	tnzs_cpu1_reset = 0;
	soundlatch = 0;

	for (int i = 0; i < 3; i++) {
		ZetOpen(i);
		ZetReset();
		if (i == 0) tnzs_bankswitch_w(0x17);
		if (i == 1) tnzs_bankswitch1_w(0x00);
		ZetClose();
	}

	BurnYM2203Reset();

	return 0;
}

static int tnzs_gfx_convert()
{
	unsigned char *tmp = (unsigned char*)malloc(0x100000);
	if (tmp == NULL) {
		return 1;
	}

	memcpy (tmp, Gfx, 0x100000);

	static int Plane[4]  = { 0x600000, 0x400000, 0x200000, 0x000000 };
	static int XOffs[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 64, 65, 66, 67, 68, 69, 70, 71 };
	static int YOffs[16] = { 0, 8, 16, 24, 32, 40, 48, 56, 128, 136, 144, 152, 160, 168, 176, 184 };

	GfxDecode(0x2000, 4, 16, 16, Plane, XOffs, YOffs, 0x100, tmp, Gfx);

	free (tmp);

	return 0;
}

static int DrvInit()
{
	Mem = (unsigned char*)malloc(0x30000 + 0x20000 + 0x10000 + 0x200000 + 0x804);
	if (Mem == NULL) {
		return 1;
	}

	Rom0 = Mem + 0x000000;
	Rom1 = Mem + 0x030000;
	Rom2 = Mem + 0x050000;
	Gfx  = Mem + 0x060000;
	Palette = (unsigned int*)(Mem + 0x260000);

	{
		if (BurnLoadRom(Rom0 + 0x10000, 0, 1)) return 1;
		memcpy (Rom0, Rom0 + 0x10000, 0x8000);

		if (BurnLoadRom(Rom1 + 0x00000, 1, 1)) return 1;
		memcpy (Rom1 + 0x10000, Rom1 + 0x08000, 0x8000);
		memset (Rom1 + 0x08000, 0, 0x08000);

		if (BurnLoadRom(Rom2 + 0x00000, 2, 1)) return 1;

		for (int i = 0; i < 8; i++)
			if (BurnLoadRom(Gfx + i * 0x20000, i + 3, 1)) return 1;

		if (tnzs_gfx_convert()) return 1;
	}

	ZetInit(3);
	ZetOpen(0);
	ZetSetWriteHandler(tnzs_cpu0_write);
	ZetMapArea(0x0000, 0x7fff, 0, Rom0 + 0x00000);
	ZetMapArea(0x0000, 0x7fff, 2, Rom0 + 0x00000);
	ZetMapArea(0x8000, 0xbfff, 0, Rom0 + 0x2c000);
	ZetMapArea(0x8000, 0xbfff, 2, Rom0 + 0x2c000);
	ZetMapArea(0xc000, 0xdfff, 0, Rom0 + 0x0c000);
	ZetMapArea(0xc000, 0xdfff, 1, Rom0 + 0x0c000);
	ZetMapArea(0xe000, 0xefff, 0, Rom0 + 0x0e000);
	ZetMapArea(0xe000, 0xefff, 1, Rom0 + 0x0e000);
	ZetMapArea(0xe000, 0xefff, 2, Rom0 + 0x0e000);
	ZetMapArea(0xf000, 0xf1ff, 0, Rom0 + 0x0f000);
	ZetMapArea(0xf000, 0xf1ff, 1, Rom0 + 0x0f000);
	ZetMapArea(0xf000, 0xf1ff, 2, Rom0 + 0x0f000);
	ZetMapArea(0xf200, 0xf2ff, 0, Rom0 + 0x0f200);
	ZetMapArea(0xf200, 0xf2ff, 1, Rom0 + 0x0f200);
	ZetMemEnd();
	ZetClose();

	ZetOpen(1);
	ZetSetWriteHandler(tnzs_cpu1_write);
	ZetSetReadHandler(tnzs_cpu1_read);
	ZetMapArea(0x0000, 0x7fff, 0, Rom1 + 0x00000);
	ZetMapArea(0x0000, 0x7fff, 2, Rom1 + 0x00000);
	ZetMapArea(0x8000, 0x9fff, 0, Rom1 + 0x10000);
	ZetMapArea(0x8000, 0x9fff, 2, Rom1 + 0x10000);
	ZetMapArea(0xc000, 0xc0ff, 0, Rom1 + 0x0c000); // read inputs
	ZetMapArea(0xd000, 0xdfff, 0, Rom1 + 0x0d000);
	ZetMapArea(0xd000, 0xdfff, 1, Rom1 + 0x0d000);
	ZetMapArea(0xd000, 0xdfff, 2, Rom1 + 0x0d000);
	ZetMapArea(0xe000, 0xefff, 0, Rom0 + 0x0e000);
	ZetMapArea(0xe000, 0xefff, 1, Rom0 + 0x0e000);
	ZetMapArea(0xe000, 0xefff, 2, Rom0 + 0x0e000);
	ZetMemEnd();
	ZetClose();

	ZetOpen(2);
	ZetSetInHandler(tnzs_cpu2_in);
	ZetSetOutHandler(tnzs_cpu2_out);
	ZetMapArea(0x0000, 0x7fff, 0, Rom2 + 0x0000);
	ZetMapArea(0x0000, 0x7fff, 2, Rom2 + 0x0000);
	ZetMapArea(0xc000, 0xdfff, 0, Rom2 + 0xc000);
	ZetMapArea(0xc000, 0xdfff, 1, Rom2 + 0xc000);
	ZetMapArea(0xc000, 0xdfff, 2, Rom2 + 0xc000);
	ZetMemEnd();
	ZetClose();

	BurnYM2203Init(1, 3000000, &DrvYM2203IRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
	BurnTimerAttachZet(6000000);

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
	ZetExit();
	BurnYM2203Exit();

	free (Mem);

	Mem = Rom0 = Rom1 = Rom2 = Gfx = NULL;
	Palette = NULL;

	tnzs_bg_flag = tnzs_screenflip = 0;
	tnzs_cpu1_reset = 0;
	tnzs_bank0 = tnzs_bank1 = 0;
	soundlatch = 0;

	return 0;
}

static inline void tnzs_plotpix(int x, unsigned char y, int color, unsigned char src, int transp)
{
	if (x < 0 || x > 255 || y > 223) return;
	if (transp && !src) return;

	int pxl = Palette[color | src];

	PutPix(pBurnDraw + ((y << 8) | x) * nBurnBpp, BurnHighCol(pxl >> 16, pxl >> 8, pxl, 0));
}

static void draw_16x16(int sx, int sy, int code, int color, int flipx, int flipy, int transp)
{
	unsigned char *src = Gfx + (code << 8);

	if (flipy) {
		for (int y = sy + 15; y >= sy; y--) {
			if (flipx) {
				for (int x = sx + 15; x >= sx; x--, src++) {
					tnzs_plotpix(x, y, color, *src, transp);
				}
			} else {
				for (int x = sx; x < sx + 16; x++, src++) {
					tnzs_plotpix(x, y, color, *src, transp);
				}
			}
		}
	} else {
		for (int y = sy; y < sy + 16; y++) {
			if (flipx) {
				for (int x = sx + 15; x >= sx; x--, src++) {
					tnzs_plotpix(x, y, color, *src, transp);
				}
			} else {
				for (int x = sx; x < sx + 16; x++, src++) {
					tnzs_plotpix(x, y, color, *src, transp);
				}
			}
		}
	}
}

static void draw_background()
{
	unsigned char *m              = Rom0 + 0xc400;
	unsigned char *tnzs_objctrl   = Rom0 + 0xf300;
	unsigned char *tnzs_scrollram = Rom0 + 0xf200;

	int x,y,column,tot,flag;
	int scrollx, scrolly;
	unsigned int upperbits;
	int ctrl2	=	tnzs_objctrl[1];

	if ((ctrl2 ^ (~ctrl2<<1)) & 0x40) {
		m += 0x800;
	}

	if (tnzs_bg_flag & 0x80)
		flag = 0;
	else
		flag = 1;

	tot = tnzs_objctrl[1] & 0x1f;
	if (tot == 1) tot = 16;

	upperbits = tnzs_objctrl[2] | (tnzs_objctrl[3] << 8);

	for (column = 0;column < tot;column++)
	{
		scrollx = tnzs_scrollram[column*16+4] - ((upperbits & 0x01) << 8);
		if (tnzs_screenflip)
			scrolly = tnzs_scrollram[column*16] + 1 - 256;
		else
			scrolly = -tnzs_scrollram[column*16] + 1;

		for (y = 0; y < 16; y++)
		{
			for (x = 0; x < 2; x++)
			{
				int code,color,flipx,flipy,sx,sy;
				int i = 32 * (column ^ 8) + 2 * y + x;

				code = m[i] + ((m[i + 0x1000] & 0x1f) << 8);
				color = (m[i + 0x1200] & 0xf8) << 1;
				sx = x << 4;
				sy = y << 4;
				flipx = m[i + 0x1000] & 0x80;
				flipy = m[i + 0x1000] & 0x40;
				if (tnzs_screenflip)
				{
					sy = 240 - sy;
					flipx = !flipx;
					flipy = !flipy;
				}

				sy = ((sy + 16 + scrolly) & 0xff) - 32; 
				sx += scrollx;

				draw_16x16(sx, sy, code, color, flipx, flipy, flag);
			}
		}

		upperbits >>= 1;
	}
}

static void draw_foreground()
{
	int i;
	unsigned char *tnzs_objctrl  = Rom0 + 0xf300;
	unsigned char *char_pointer  = Rom0 + 0xc000;
	unsigned char *x_pointer     = Rom0 + 0xc200;
	unsigned char *y_pointer     = Rom0 + 0xf000;
	unsigned char *ctrl_pointer  = Rom0 + 0xd000;
	unsigned char *color_pointer = Rom0 + 0xd200;
	int ctrl2		     = tnzs_objctrl[1];

	if ((ctrl2 ^ (~ctrl2<<1)) & 0x40)
	{
		char_pointer  += 0x800;
		x_pointer     += 0x800;
		ctrl_pointer  += 0x800;
		color_pointer += 0x800;
	}

	for (i=0x1ff;i >= 0;i--)
	{
		int code,color,sx,sy,flipx,flipy;

		code = char_pointer[i] + ((ctrl_pointer[i] & 0x1f) << 8);
		color = (color_pointer[i] & 0xf8) << 1;
		sx = x_pointer[i] - ((color_pointer[i] & 1) << 8);
		sy = y_pointer[i] ^ 0xff;
		flipx = ctrl_pointer[i] & 0x80;
		flipy = ctrl_pointer[i] & 0x40;
		if (tnzs_screenflip)
		{
			sy = 240 - sy;
			flipx = !flipx;
			flipy = !flipy;
		} else {
			sy -= 32;
		}

		sy += 2;

		draw_16x16(sx, sy, code, color, flipx, flipy, 1);
	}
}

static void clear_background(int col)
{
	for (int i = 0; i < 256 * 256; i++) {
		tnzs_plotpix(i & 0xff, i >> 8, col, 0, 0);
	}
}

static int DrvDraw()
{
	int ctrl2 = Rom0[0xf301];

	tnzs_screenflip = (Rom0[0xf300] & 0x40) >> 6;

	if (nBurnLayer & 2) {
		clear_background(0x1f0);
		draw_background();
	} else {
		Palette[0x200] = 0xff00ff;
		clear_background(0x200);
	}

	if (nBurnLayer & 1) draw_foreground();

	if (~ctrl2 & 0x20)
	{
		if (ctrl2 & 0x40)
		{
			memcpy(Rom0 + 0xc000, Rom0 + 0xc800, 0x0400);
			memcpy(Rom0 + 0xd000, Rom0 + 0xd800, 0x0400);
		}
		else
		{
			memcpy(Rom0 + 0xc800, Rom0 + 0xc000, 0x0400);
			memcpy(Rom0 + 0xd800, Rom0 + 0xd000, 0x0400);
		}

		memcpy(Rom0 + 0xc400, Rom0 + 0xcc00, 0x0400);
		memcpy(Rom0 + 0xd400, Rom0 + 0xdc00, 0x0400);
	}

	return 0;
}

static int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	ZetNewFrame();

	// Assemble inputs (work-around for lhld bug in Doze)
	{
		Rom1[0xc000] = Rom1[0xc001] = Rom1[0xc002] = 0xff;
		for (int i = 0; i < 8; i++) {
			Rom1[0xc000] ^= DrvJoy1[i] << i;
			Rom1[0xc001] ^= DrvJoy2[i] << i;
			Rom1[0xc002] ^= DrvJoy3[i] << i;
		}
	}

	int nInterleave = 10;

	int nCyclesSegment;
	int nCyclesDone[3], nCyclesTotal[3];

	nCyclesTotal[0] = 6000000 / 60;
	nCyclesTotal[1] = 6000000 / 60;
	nCyclesTotal[2] = 6000000 / 60;

	nCyclesDone[0] = nCyclesDone[1] = nCyclesDone[2] = 0;

	for (int i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run Z80 #0
		nCurrentCPU = 0;
		ZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += ZetRun(nCyclesSegment);
		if (i+1 == nInterleave) ZetSetIRQLine(0, ZET_IRQSTATUS_AUTO);
		ZetClose();

		// Run Z80 #1
		nCurrentCPU = 1;
		ZetOpen(nCurrentCPU);
		if (tnzs_cpu1_reset) {
			ZetReset();
			tnzs_cpu1_reset = 0;
		}
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesSegment = ZetRun(nCyclesSegment);
		nCyclesDone[nCurrentCPU] += nCyclesSegment;
		if (i+1 == nInterleave) ZetSetIRQLine(0, ZET_IRQSTATUS_AUTO);
		ZetClose();
	}
	
	ZetOpen(2);
	BurnTimerEndFrame(nCyclesTotal[2] - nCyclesDone[2]);
	BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
	ZetClose();

	if (pBurnDraw) {
		DrvDraw();
	}

	return 0;
}

static int DrvScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029672;
	}

	if (nAction & ACB_VOLATILE) {
		ba.Data	  = Rom0 + 0xc000;
		ba.nLen	  = 0x2000;
		ba.szName = "CPU0 Video RAM";
		BurnAcb(&ba);

		ba.Data	  = Rom0 + 0xe000;
		ba.nLen	  = 0x1000;
		ba.szName = "Shared RAM";
		BurnAcb(&ba);

		ba.Data	  = Rom0 + 0xf000;
		ba.nLen	  = 0x0400;
		ba.szName = "CPU0 Misc Ram";
		BurnAcb(&ba);

		ba.Data	  = Rom0 + 0x10000;
		ba.nLen	  = 0x8000;
		ba.szName = "CPU0 Banked Ram";
		BurnAcb(&ba);

		ba.Data	  = Rom1 + 0xd000;
		ba.nLen	  = 0x1000;
		ba.szName = "CPU1 Main RAM";
		BurnAcb(&ba);

		ba.Data	  = Rom1 + 0xf000;
		ba.nLen	  = 0x0400;
		ba.szName = "Palette RAM";
		BurnAcb(&ba);

		ba.Data	  = Rom2 + 0xc000;
		ba.nLen	  = 0x2000;
		ba.szName = "CPU2 Main RAM";
		BurnAcb(&ba);

		ba.Data	  = (unsigned char*)Palette;
		ba.nLen	  = 0x0200 * sizeof(int);
		ba.szName = "Palette";
		BurnAcb(&ba);

		ZetScan(nAction);
		BurnYM2203Scan(nAction, pnMin);

		SCAN_VAR(soundlatch);
		SCAN_VAR(tnzs_bg_flag);
		SCAN_VAR(tnzs_screenflip);
		SCAN_VAR(tnzs_bank0);
		SCAN_VAR(tnzs_bank1);
		SCAN_VAR(tnzs_cpu1_reset);

		ZetOpen(0);
		tnzs_bankswitch_w(tnzs_bank0);
		ZetClose();
		ZetOpen(1);
		tnzs_bankswitch1_w(tnzs_bank1);
		ZetClose();
	}

	return 0;
}

// The NewZealand Story (World, newer)

static struct BurnRomInfo tnzsRomDesc[] = {
	{ "b53-24.1",   0x20000, 0xd66824c6, 1 | BRF_ESS | BRF_PRG }, //  0 Z80 #0 Code

	{ "b53-25.3",   0x10000, 0xd6ac4e71, 2 | BRF_ESS | BRF_PRG }, //  1 Z80 #1 Code

	{ "b53-26.34",  0x10000, 0xcfd5649c, 3 | BRF_ESS | BRF_PRG }, //  2 Z80 #2 Code

	{ "b53-08.8",	0x20000, 0xc3519c2a, 4 | BRF_GRA },	      //  3 Graphics
	{ "b53-07.7",	0x20000, 0x2bf199e8, 4 | BRF_GRA },	      //  4
	{ "b53-06.6",	0x20000, 0x92f35ed9, 4 | BRF_GRA },	      //  5
	{ "b53-05.5",	0x20000, 0xedbb9581, 4 | BRF_GRA },	      //  6
	{ "b53-04.4",	0x20000, 0x59d2aef6, 4 | BRF_GRA },	      //  7
	{ "b53-03.3",	0x20000, 0x74acfb9b, 4 | BRF_GRA },	      //  8
	{ "b53-02.2",	0x20000, 0x095d0dc0, 4 | BRF_GRA },	      //  9
	{ "b53-01.1",	0x20000, 0x9800c54d, 4 | BRF_GRA },	      // 10
};

STD_ROM_PICK(tnzs);
STD_ROM_FN(tnzs);

struct BurnDriver BurnDrvtnzs = {
	"tnzs", NULL, NULL, "1988",
	"The NewZealand Story (World, newer)\0", NULL, "Taito Corporation Japan", "Misc",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S,
	NULL, tnzsRomInfo, tnzsRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, 0, NULL, NULL, NULL, NULL,
	256, 224, 4, 3
};

