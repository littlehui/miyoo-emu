/********************************************************************************
 Newer Seta Hardware
 MAME driver by Luca Elia (l.elia@tin.it)
 ********************************************************************************

 to make this driver work, we need modify souce code of sek.cpp
 
extern "C" int M68KIRQAcknowledge(int nIRQ)
{
	if (nSekIRQPending[nSekActive] & SEK_IRQSTATUS_AUTO) {
		m68k_set_irq(0);
		nSekIRQPending[nSekActive] = 0;
	}

	// Enable IrqCallback for M68K
	// by OopsWare 2007.11
	if (pSekExt->IrqCallback) {
		return pSekExt->IrqCallback(nIRQ);
	} else
		return M68K_INT_ACK_AUTOVECTOR;
}


static int A68KIRQAcknowledge(int nIRQ)
{
	if (nSekIRQPending[nSekActive] & SEK_IRQSTATUS_AUTO) {
		M68000_regs.irq &= 0x78;
		nSekIRQPending[nSekActive] = 0;
	}

	nSekIRQPending[nSekActive] = 0;

	// Enable IrqCallback for A68K
	// by OopsWare 2007.11
	if (pSekExt->IrqCallback) {
		return pSekExt->IrqCallback(nIRQ);
	} else
		return -1;

}

 this game need custom irq call back vector, lookup 'SekSetIrqCallback()' all drivers
 only several SEGA game need it, but i don't known whether it'll cause any matter .

 ********************************************************************************
 port to Finalburn Alpha by OopsWare. 2007
 ********************************************************************************/

#include "burnint.h"
#include "x1010.h"

static unsigned char *Mem = NULL, *MemEnd = NULL;
static unsigned char *RamStart, *RamEnd;

static unsigned char *Rom68K;
static unsigned char *RomGfx;

static unsigned char *Ram68K;
static unsigned char *RamUnknown;

static unsigned short *RamSpr;
static unsigned short *RamSprBak;
static unsigned short *RamPal;
static unsigned int *CurPal;
static unsigned short *RamTMP68301;
static unsigned short *RamVReg;

static unsigned char DrvButton[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy3[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy4[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvJoy5[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInput[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static unsigned char DrvReset = 0;
static unsigned char bRecalcPalette = 0;

static unsigned int gfx_code_mask;
//static unsigned char bMahjong = 0;
static unsigned char Mahjong_keyboard = 0;

static int yoffset;
static int sva_x;
static int sva_y;
static int sva_w;
static int sva_h;

#define M68K_CYCS	50000000 / 3

inline static unsigned int CalcCol(unsigned short nColour)
{
	int r, g, b;

	r = (nColour & 0x7c00) >> 7;	// Red
	r |= r >> 5;
	g = (nColour & 0x03E0) >> 2;  // Green
	g |= g >> 5;
	b = (nColour & 0x001f) << 3;	// Blue
	b |= b >> 5;

	return BurnHighCol(r, g, b, 0);
}

// ---- Toshiba TMP68301  ---------------------------------------------

static int tmp68301_timer[3] = {0, 0, 0};
static int tmp68301_timer_counter[3] = {0, 0, 0};
static int tmp68301_irq_vector[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static void tmp68301_update_timer( int i )
{
	unsigned short TCR	=	RamTMP68301[(0x200 + i * 0x20)/2];
	unsigned short MAX1	=	RamTMP68301[(0x204 + i * 0x20)/2];
	unsigned short MAX2	=	RamTMP68301[(0x206 + i * 0x20)/2];

	int max = 0;
	double duration = 0;

//	timer_adjust(tmp68301_timer[i],TIME_NEVER,i,0);
	tmp68301_timer[i] = 0;
	tmp68301_timer_counter[i] = 0;
	//bprintf(PRINT_NORMAL, _T("Tmp68301: update timer %d. TCR: %04x MAX: %04x %04x\n"), i, TCR, MAX1, MAX2);

	// timers 1&2 only
	switch( (TCR & 0x0030)>>4 )	{					// MR2..1
	case 1:	max = MAX1;	break;
	case 2:	max = MAX2;	break;
	}

	switch ( (TCR & 0xc000)>>14 ) {					// CK2..1
	case 0:	// System clock (CLK)
		if (max) {
			int scale = (TCR & 0x3c00)>>10;			// P4..1
			if (scale > 8) scale = 8;
			duration = M68K_CYCS;					//Machine->drv->cpu[0].cpu_clock;
			duration /= 1 << scale;
			duration /= max;
		}
		break;
	}

//  logerror("CPU #0 PC %06X: TMP68301 Timer %d, duration %lf, max %04X\n",activecpu_get_pc(),i,duration,max);
//	bprintf(PRINT_NORMAL, _T("TMP68301 Timer %d, duration %lf, max %04X TCR %04X\n"),i,duration,max,TCR);

	if (!(TCR & 0x0002))				// CS
	{
		if (duration) {
			// timer_adjust(tmp68301_timer[i],TIME_IN_HZ(duration),i,0);
			// active tmp68301 timer i, and set duration
			tmp68301_timer[i] = (int) (M68K_CYCS / duration);
			//tmp68301_timer_counter[i] = 0;
			//bprintf(PRINT_NORMAL, _T("Tmp68301: update timer #%d duration to %d (%8.3f)\n"), i, tmp68301_timer[i], duration);
		} else
			//logerror("CPU #0 PC %06X: TMP68301 error, timer %d duration is 0\n",activecpu_get_pc(),i);
			bprintf(PRINT_ERROR, _T("Tmp68301: error timer %d duration is 0\n"), i, TCR, MAX1, MAX2);
	}
}

static void tmp68301_timer_callback(int i)
{
	unsigned short TCR	= RamTMP68301[(0x200 + i * 0x20)/2];
	unsigned short IMR	= RamTMP68301[0x94/2];		// Interrupt Mask Register (IMR)
	unsigned short ICR	= RamTMP68301[0x8e/2+i];	// Interrupt Controller Register (ICR7..9)
	unsigned short IVNR	= RamTMP68301[0x9a/2];		// Interrupt Vector Number Register (IVNR)

//  logerror("CPU #0 PC %06X: callback timer %04X, j = %d\n",activecpu_get_pc(),i,tcount);
//	bprintf(PRINT_NORMAL, _T("Tmp68301: timer[%d] TCR: %04x IMR: %04x\n"), i, TCR, IMR);

	if	( (TCR & 0x0004) &&	!(IMR & (0x100<<i))	) {
		int level = ICR & 0x0007;
		// Interrupt Vector Number Register (IVNR)
		tmp68301_irq_vector[level]	=	IVNR & 0x00e0;
		tmp68301_irq_vector[level]	+=	4+i;

		//cpunum_set_input_line(0,level,HOLD_LINE);
		//bprintf(PRINT_NORMAL, _T("Tmp68301: CB IRQ[%x] %04x  timer[%d]\n"), level, tmp68301_irq_vector[level], i);
		//SekSetIRQLine(tmp68301_irq_vector[level], SEK_IRQSTATUS_AUTO);

		//SekSetIRQLine(level, SEK_IRQSTATUS_ACK);
		SekSetIRQLine(level, SEK_IRQSTATUS_AUTO);
	}

	if (TCR & 0x0080) {	// N/1
		// Repeat
		tmp68301_update_timer(i);
	} else {
		// One Shot
	}
}

static void tmp68301_update_irq_state(int i)
{
	/* Take care of external interrupts */
	unsigned short IMR	= RamTMP68301[0x94/2];		// Interrupt Mask Register (IMR)
	unsigned short IVNR	= RamTMP68301[0x9a/2];		// Interrupt Vector Number Register (IVNR)

	if	( !(IMR & (1<<i)) )	{
		unsigned short ICR = RamTMP68301[0x80/2+i];	// Interrupt Controller Register (ICR0..2)

		// Interrupt Controller Register (ICR0..2)
		int level = ICR & 0x0007;

		// Interrupt Vector Number Register (IVNR)
		tmp68301_irq_vector[level]	=	IVNR & 0x00e0;
		tmp68301_irq_vector[level]	+=	i;

		//tmp68301_IE[i] = 0;		// Interrupts are edge triggerred
		//cpunum_set_input_line(0,level,HOLD_LINE);
		//bprintf(PRINT_NORMAL, _T("Tmp68301: UP IRQ[%x] %04x  timer[%d] IMR:%04x IVNR:%04x ICR:%04x\n"), level, tmp68301_irq_vector[level], i, IMR, IVNR, ICR);

		//SekSetIRQLine(level, SEK_IRQSTATUS_ACK);
		SekSetIRQLine(level, SEK_IRQSTATUS_AUTO);
	}
}

static void tmp68301_regs_w(unsigned int addr, unsigned short val )
{
	//bprintf(PRINT_NORMAL, _T("Tmp68301: write val %04x to location %06x\n"), val, addr);
	//tmp68301_update_timer( (addr >> 5) & 3 );
	switch ( addr ) {
	case 0x200: tmp68301_update_timer(0); break;
	case 0x220: tmp68301_update_timer(1); break;
	case 0x240: tmp68301_update_timer(2); break;
	}
}

void __fastcall Tmp68301WriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	sekAddress &= 0x0003ff;
	unsigned char *p = (unsigned char *)RamTMP68301;
	p[sekAddress ^ 1] = byteValue;
//	bprintf(PRINT_NORMAL, _T("TMP68301 Reg %04X <- %04X & %04X   %04x\n"),sekAddress&0xfffe, (sekAddress&1)?byteValue:byteValue<<8,(sekAddress&1)?0x00ff:0xff00, RamTMP68301[sekAddress>>1]);
}

void __fastcall Tmp68301WriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	sekAddress &= 0x0003ff;
	RamTMP68301[ sekAddress >> 1 ] = wordValue;
	tmp68301_regs_w( sekAddress, wordValue );
//	bprintf(PRINT_NORMAL, _T("TMP68301 Reg %04X <- %04X & %04X   %04x\n"),sekAddress,wordValue,0xffff, RamTMP68301[sekAddress>>1]);
}

static struct BurnInputInfo grdiansInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvButton + 0,	"p1 coin"},

	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 1,	"p1 right"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 down"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},
	{"P1 Button 3",	BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 3"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},

	{"P2 Coin",		BIT_DIGITAL,	DrvButton + 1,	"p2 coin"},

	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 1,	"p2 right"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},
	{"P2 Button 3",	BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 3"},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Diagnostics",	BIT_DIGITAL, DrvButton + 3,	"diag"},
	{"Service",		BIT_DIGITAL, DrvButton + 2,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvInput + 3,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvInput + 4,	"dip"},
};

STDINPUTINFO(grdians);

static struct BurnDIPInfo grdiansDIPList[] = {

	// Defaults
	{0x15,	0xFF, 0xFF,	0x00, NULL},
	{0x16,	0xFF, 0xFF,	0x00, NULL},

	// DIP 1
	{0,		0xFE, 0,	4,	  "Difficulty"},
	{0x15,	0x01, 0x03, 0x01, "Easy"},
	{0x15,	0x01, 0x03, 0x00, "Normal"},
	{0x15,	0x01, 0x03, 0x02, "Hard"},
	{0x15,	0x01, 0x03, 0x03, "Hardest"},
//	{0,		0xFE, 0,	2,	  "Unknown"},
//	{0x15,	0x01, 0x04,	0x00, "Off"},
//	{0x15,	0x01, 0x04,	0x04, "On"},
	{0,		0xFE, 0,	2,	  "Title"},
	{0x15,	0x01, 0x08,	0x00, "Guardians"},
	{0x15,	0x01, 0x08,	0x08, "Denjin Makai II"},
	{0,		0xFE, 0,	4,	  "Lives"},
	{0x15,	0x01, 0x30, 0x10, "1"},
	{0x15,	0x01, 0x30, 0x00, "2"},
	{0x15,	0x01, 0x30, 0x20, "3"},
	{0x15,	0x01, 0x30, 0x30, "4"},
	{0,		0xFE, 0,	2,	  "Test mode"},
	{0x15,	0x01, 0x40, 0x00, "Off"}, 
	{0x15,	0x01, 0x40, 0x40, "On"},
	{0,		0xFE, 0,	2,	  "Demo sounds"},
	{0x15,	0x01, 0x80, 0x80, "Off"},
	{0x15,	0x01, 0x80, 0x00, "On"},

	// DIP 2
	{0,		0xFE, 0,	16,	  "Coin 1"},
	{0x16,	0x01, 0x0f, 0x00, "1 coin 1 credit"},
	{0x16,	0x01, 0x0f, 0x01, "1 coin 2 credits"},
	{0x16,	0x01, 0x0f, 0x02, "1 coin 3 credits"},
	{0x16,	0x01, 0x0f, 0x03, "1 coin 4 credits"},
	{0x16,	0x01, 0x0f, 0x04, "1 coin 5 credits"},
	{0x16,	0x01, 0x0f, 0x05, "1 coin 6 credits"},
	{0x16,	0x01, 0x0f, 0x06, "1 coin 7 credits"},
	{0x16,	0x01, 0x0f, 0x07, "2 coins 1 credit"},
	{0x16,	0x01, 0x0f, 0x08, "2 coins 3 credits"},
	{0x16,	0x01, 0x0f, 0x09, "2 coins 5 credits"},
	{0x16,	0x01, 0x0f, 0x0a, "3 coins 1 credit"},
	{0x16,	0x01, 0x0f, 0x0b, "3 coins 2 credits"},
	{0x16,	0x01, 0x0f, 0x0c, "3 coins 4 credits"},
	{0x16,	0x01, 0x0f, 0x0d, "4 coins 1 credit"},
	{0x16,	0x01, 0x0f, 0x0e, "4 coins 3 credits"},
	{0x16,	0x01, 0x0f, 0x0f, "Free play"},
	{0,		0xFE, 0,	16,	  "Coin 2"},
	{0x16,	0x01, 0xf0, 0x00, "1 coin 1 credit"},
	{0x16,	0x01, 0xf0, 0x10, "1 coin 2 credits"},
	{0x16,	0x01, 0xf0, 0x20, "1 coin 3 credits"},
	{0x16,	0x01, 0xf0, 0x30, "1 coin 4 credits"},
	{0x16,	0x01, 0xf0, 0x40, "1 coin 5 credits"},
	{0x16,	0x01, 0xf0, 0x50, "1 coin 6 credits"},
	{0x16,	0x01, 0xf0, 0x60, "1 coin 7 credits"},
	{0x16,	0x01, 0xf0, 0x70, "2 coins 1 credit"},
	{0x16,	0x01, 0xf0, 0x80, "2 coins 3 credits"},
	{0x16,	0x01, 0xf0, 0x90, "2 coins 5 credits"},
	{0x16,	0x01, 0xf0, 0xa0, "3 coins 1 credit"},
	{0x16,	0x01, 0xf0, 0xb0, "3 coins 2 credits"},
	{0x16,	0x01, 0xf0, 0xc0, "3 coins 4 credits"},
	{0x16,	0x01, 0xf0, 0xd0, "4 coins 1 credit"},
	{0x16,	0x01, 0xf0, 0xe0, "4 coins 3 credits"},
	{0x16,	0x01, 0xf0, 0xf0, "Free play"},

};

STDDIPINFO(grdians);

static struct BurnInputInfo mj4simaiInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvButton + 0,	"p1 coin"},
	{"P2 Coin",		BIT_DIGITAL,	DrvButton + 1,	"p2 coin"},

	{"A",			BIT_DIGITAL,	DrvJoy1 + 0,	"mah a"},
	{"E",			BIT_DIGITAL,	DrvJoy1 + 1,	"mah e"},
	{"I",			BIT_DIGITAL,	DrvJoy1 + 2,	"mah i"},
	{"M",			BIT_DIGITAL,	DrvJoy1 + 3,	"mah m"},
	{"Kan",			BIT_DIGITAL,	DrvJoy1 + 4,	"mah kan"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 start"},

	{"B",			BIT_DIGITAL,	DrvJoy2 + 0,	"mah b"},
	{"F",			BIT_DIGITAL,	DrvJoy2 + 1,	"mah f"},
	{"J",			BIT_DIGITAL,	DrvJoy2 + 2,	"mah j"},
	{"N",			BIT_DIGITAL,	DrvJoy2 + 3,	"mah n"},
	{"Reach",		BIT_DIGITAL,	DrvJoy2 + 4,	"mah reach"},
	{"Bet",			BIT_DIGITAL,	DrvJoy2 + 5,	"mah bet"},

	{"C",			BIT_DIGITAL,	DrvJoy3 + 0,	"mah c"},
	{"G",			BIT_DIGITAL,	DrvJoy3 + 1,	"mah g"},
	{"K",			BIT_DIGITAL,	DrvJoy3 + 2,	"mah k"},
	{"Chi",			BIT_DIGITAL,	DrvJoy3 + 3,	"mah chi"},
	{"Ron",			BIT_DIGITAL,	DrvJoy3 + 4,	"mah ron"},

	{"D",			BIT_DIGITAL,	DrvJoy4 + 0,	"mah d"},
	{"H",			BIT_DIGITAL,	DrvJoy4 + 1,	"mah h"},
	{"L",			BIT_DIGITAL,	DrvJoy4 + 2,	"mah l"},
	{"Pon",			BIT_DIGITAL,	DrvJoy4 + 3,	"mah pon"},

	{"LastChange",	BIT_DIGITAL,	DrvJoy5 + 0,	"mah lc"},
	{"Score",		BIT_DIGITAL,	DrvJoy5 + 1,	"mah score"},
	{"DoubleUp",	BIT_DIGITAL,	DrvJoy5 + 2,	"mah du"},		// ????
	{"FlipFlop",	BIT_DIGITAL,	DrvJoy5 + 3,	"mah ff"},
	{"Big",			BIT_DIGITAL,	DrvJoy5 + 4,	"mah big"},		// ????
	{"Smaill",		BIT_DIGITAL,	DrvJoy5 + 5,	"mah small"},	// ????

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Service",		BIT_DIGITAL, DrvButton + 2,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvInput + 3,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvInput + 4,	"dip"},
};

STDINPUTINFO(mj4simai);

static struct BurnDIPInfo mj4simaiDIPList[] = {

	// Defaults
	{0x1F,	0xFF, 0xFF,	0x00, NULL},
	{0x20,	0xFF, 0xFF,	0x00, NULL},

	// DIP 1
	{0,		0xFE, 0,	8,	  "Coinage"},
	{0x1F,	0x01, 0x07, 0x00, "1 coin 1 credit"},
	{0x1F,	0x01, 0x07, 0x01, "1 coin 2 credits"},
	{0x1F,	0x01, 0x07, 0x02, "1 coin 3 credits"},
	{0x1F,	0x01, 0x07, 0x03, "1 coin 4 credits"},
	{0x1F,	0x01, 0x07, 0x04, "2 coins 1 credit"},
	{0x1F,	0x01, 0x07, 0x05, "3 coins 1 credit"},
	{0x1F,	0x01, 0x07, 0x06, "4 coins 1 credit"},
	{0x1F,	0x01, 0x07, 0x07, "5 coins 1 credit"},
	{0,		0xFE, 0,	2,	  "Demo sound"},
	{0x1F,	0x01, 0x08,	0x08, "Off"},
	{0x1F,	0x01, 0x08,	0x00, "On"},
	{0,		0xFE, 0,	2,	  "Tumo pin"},
	{0x1F,	0x01, 0x10,	0x10, "Off"},
	{0x1F,	0x01, 0x10,	0x00, "On"},
	{0,		0xFE, 0,	2,	  "Flip screen"},
	{0x1F,	0x01, 0x20,	0x00, "Off"},
	{0x1F,	0x01, 0x20,	0x20, "On"},
	{0,		0xFE, 0,	2,	  "Free play"},
	{0x1F,	0x01, 0x40,	0x00, "Off"},
	{0x1F,	0x01, 0x40,	0x40, "On"},
	{0,		0xFE, 0,	2,	  "Test mode"},
	{0x1F,	0x01, 0x80,	0x00, "Off"},
	{0x1F,	0x01, 0x80,	0x80, "On"},

	// DIP 2
	{0,		0xFE, 0,	8,	  "Difficulty"},
	{0x20,	0x01, 0x07, 0x03, "0"},
	{0x20,	0x01, 0x07, 0x04, "1"},
	{0x20,	0x01, 0x07, 0x05, "2"},
	{0x20,	0x01, 0x07, 0x06, "3"},
	{0x20,	0x01, 0x07, 0x07, "4"},
	{0x20,	0x01, 0x07, 0x00, "5"},
	{0x20,	0x01, 0x07, 0x01, "6"},
	{0x20,	0x01, 0x07, 0x02, "7"},
	{0,		0xFE, 0,	2,	  "Continue"},
	{0x20,	0x01, 0x08, 0x08, "Off"},
	{0x20,	0x01, 0x08, 0x00, "On"},
	{0,		0xFE, 0,	2,	  "Select girl"},
	{0x20,	0x01, 0x10, 0x00, "Off"},
	{0x20,	0x01, 0x10, 0x10, "On"},
	{0,		0xFE, 0,	2,	  "Com put"},
	{0x20,	0x01, 0x20, 0x00, "Off"},
	{0x20,	0x01, 0x20, 0x20, "On"},
};

STDDIPINFO(mj4simai);


static struct BurnInputInfo myangelInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvButton + 0,	"p1 coin"},

	{"P1 Start",	  BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},
	{"P1 Button 3",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 3"},
	{"P1 Button 4",	BIT_DIGITAL,	DrvJoy1 + 3,	"p1 fire 4"},

	{"P2 Coin",		BIT_DIGITAL,	DrvButton + 1,	"p2 coin"},

	{"P2 Start",	  BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},
	{"P2 Button 3",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 3"},
	{"P2 Button 4",	BIT_DIGITAL,	DrvJoy2 + 3,	"p2 fire 4"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Diagnostics",	BIT_DIGITAL, DrvButton + 3,	"diag"},
	{"Service",		BIT_DIGITAL, DrvButton + 2,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvInput + 3,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvInput + 4,	"dip"},
};

STDINPUTINFO(myangel);

static struct BurnDIPInfo myangel2DIPList[] = {

	// Defaults
	{0x0F,	0xFF, 0xFF,	0x00, NULL},
	{0x10,	0xFF, 0xFF,	0x00, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Test mode"},
	{0x0F,	0x01, 0x01, 0x00, "Off"},
	{0x0F,	0x01, 0x01, 0x01, "On"},
	{0,		0xFE, 0,	2,	  "Increase lives while playing"},
	{0x0F,	0x01, 0x08,	0x08, "No"},
	{0x0F,	0x01, 0x08,	0x00, "Yes"},
	{0,		0xFE, 0,	4,	  "Lives"},
	{0x0F,	0x01, 0x30, 0x10, "2"},
	{0x0F,	0x01, 0x30, 0x00, "3"},
	{0x0F,	0x01, 0x30, 0x20, "4"},
	{0x0F,	0x01, 0x30, 0x30, "5"},
	{0,		0xFE, 0,	2,	  "Demo sounds"},
	{0x0F,	0x01, 0x40, 0x40, "Off"},
	{0x0F,	0x01, 0x40, 0x00, "On"},
	{0,		0xFE, 0,	2,	  "Flip screen"},
	{0x0F,	0x01, 0x80, 0x00, "Off"},
	{0x0F,	0x01, 0x80, 0x80, "On"},

	// DIP 2
	{0,		0xFE, 0,	16,	  "Coinage"},
	{0x10,	0x01, 0x0f, 0x00, "1 coin 1 credit"},
	{0x10,	0x01, 0x0f, 0x01, "1 coin 2 credits"},
	{0x10,	0x01, 0x0f, 0x02, "1 coin 3 credits"},
	{0x10,	0x01, 0x0f, 0x03, "1 coin 4 credits"},
	{0x10,	0x01, 0x0f, 0x04, "1 coin 5 credits"},
	{0x10,	0x01, 0x0f, 0x05, "1 coin 6 credits"},
	{0x10,	0x01, 0x0f, 0x06, "1 coin 7 credits"},
	{0x10,	0x01, 0x0f, 0x07, "2 coins 1 credit"},
	{0x10,	0x01, 0x0f, 0x08, "2 coins 3 credits"},
	{0x10,	0x01, 0x0f, 0x09, "2 coins 5 credits"},
	{0x10,	0x01, 0x0f, 0x0a, "3 coins 1 credit"},
	{0x10,	0x01, 0x0f, 0x0b, "3 coins 2 credits"},
	{0x10,	0x01, 0x0f, 0x0c, "3 coins 4 credits"},
	{0x10,	0x01, 0x0f, 0x0d, "4 coins 1 credit"},
	{0x10,	0x01, 0x0f, 0x0e, "4 coins 3 credits"},
	{0x10,	0x01, 0x0f, 0x0f, "Free play"},

};

static struct BurnDIPInfo myangelDIPList[] = {

	{0,		0xFE, 0,	2,	  "Push start to freeze (cheat)"},
	{0x10,	0x01, 0x80, 0x00, "No"},
	{0x10,	0x01, 0x80, 0x80, "Yes"},

};

STDDIPINFO(myangel2);
STDDIPINFOEXT(myangel, myangel2, myangel);

static struct BurnDIPInfo pzlbowlDIPList[] = {

	// Defaults
	{0x15,	0xFF, 0xFF,	0x00, NULL},
	{0x16,	0xFF, 0xFF,	0x80, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Test mode"},
	{0x15,	0x01, 0x01, 0x00, "Off"}, 
	{0x15,	0x01, 0x01, 0x01, "On"},
	{0,		0xFE, 0,	2,	  "Demo sound"},
	{0x15,	0x01, 0x02, 0x02, "Off"},
	{0x15,	0x01, 0x02, 0x00, "On"}, 
	{0,		0xFE, 0,	2,	  "Flip screen"},
	{0x15,	0x01, 0x04, 0x00, "Off"}, 
	{0x15,	0x01, 0x04, 0x04, "On"},
	{0,		0xFE, 0,	8,	  "Difficulty"},
	{0x15,	0x01, 0x38, 0x08, "Easiest"},
	{0x15,	0x01, 0x38, 0x10, "Easier"},
	{0x15,	0x01, 0x38, 0x18, "Easy"},
	{0x15,	0x01, 0x38, 0x00, "Normal"},
	{0x15,	0x01, 0x38, 0x20, "Hard"},
	{0x15,	0x01, 0x38, 0x28, "Harder"},
	{0x15,	0x01, 0x38, 0x30, "Very hard"},
	{0x15,	0x01, 0x38, 0x38, "Hardest"},
	{0,		0xFE, 0,	4,	  "Winning rounds (player vs player)"},
	{0x15,	0x01, 0xc0, 0x80, "1"},
	{0x15,	0x01, 0xc0, 0x00, "2"}, 
	{0x15,	0x01, 0xc0, 0x40, "3"}, 
	{0x15,	0x01, 0xc0, 0xc0, "5"},
	
	// DIP 2
	{0,		0xFE, 0,	16,	  "Coinage"},
	{0x16,	0x01, 0x0f, 0x00, "1 coin 1 credit"},
	{0x16,	0x01, 0x0f, 0x01, "1 coin 2 credits"},
	{0x16,	0x01, 0x0f, 0x02, "1 coin 3 credits"},
	{0x16,	0x01, 0x0f, 0x03, "1 coin 4 credits"},
	{0x16,	0x01, 0x0f, 0x04, "1 coin 5 credits"},
	{0x16,	0x01, 0x0f, 0x05, "1 coin 6 credits"},
	{0x16,	0x01, 0x0f, 0x06, "1 coin 7 credits"},
	{0x16,	0x01, 0x0f, 0x07, "2 coins 1 credit"},
	{0x16,	0x01, 0x0f, 0x08, "2 coins 3 credits"},
	{0x16,	0x01, 0x0f, 0x09, "2 coins 5 credits"},
	{0x16,	0x01, 0x0f, 0x0a, "3 coins 1 credit"},
	{0x16,	0x01, 0x0f, 0x0b, "3 coins 2 credits"},
	{0x16,	0x01, 0x0f, 0x0c, "3 coins 4 credits"},
	{0x16,	0x01, 0x0f, 0x0d, "4 coins 1 credit"},
	{0x16,	0x01, 0x0f, 0x0e, "4 coins 3 credits"},
	{0x16,	0x01, 0x0f, 0x0f, "Free play"},
	{0,		0xFE, 0,	2,	  "Allow continue"},
	{0x16,	0x01, 0x10, 0x10, "No"},
	{0x16,	0x01, 0x10, 0x00, "Yes"},
	{0,		0xFE, 0,	2,	  "Join In"},
	{0x16,	0x01, 0x20, 0x20, "No"},
	{0x16,	0x01, 0x20, 0x00, "Yes"},
//	{0,		0xFE, 0,	2,	  "Unused"},
//	{0x16,	0x01, 0x40, 0x00, "Off"},
//	{0x16,	0x01, 0x40, 0x40, "On"},
	{0,		0xFE, 0,	2,	  "Language"},
	{0x16,	0x01, 0x80, 0x00, "Japanese"},
	{0x16,	0x01, 0x80, 0x80, "English"},

};

STDDIPINFO(pzlbowl);

static struct BurnInputInfo penbrosInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvButton + 0,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy1 + 7,	"p1 start"},

	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 1,	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvButton + 1,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy2 + 7,	"p2 start"},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 1,	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,		"reset"},
	{"Diagnostics",	BIT_DIGITAL, DrvButton + 3,	"diag"},
	{"Service",		BIT_DIGITAL, DrvButton + 2,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvInput + 3,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvInput + 4,	"dip"},
};

STDINPUTINFO(penbros);

static struct BurnDIPInfo penbrosDIPList[] = {

	// Defaults
	{0x13,	0xFF, 0xFF,	0x00, NULL},
	{0x14,	0xFF, 0xFF,	0x00, NULL},

	// DIP 1
	{0,		0xFE, 0,	2,	  "Test mode"},
	{0x13,	0x01, 0x01, 0x00, "Off"}, 
	{0x13,	0x01, 0x01, 0x01, "On"},
	{0,		0xFE, 0,	2,	  "Screen"},
	{0x13,	0x01, 0x02, 0x00, "Normal"}, 
	{0x13,	0x01, 0x02, 0x02, "Reverse"},
	{0,		0xFE, 0,	2,	  "Demo sound"},
	{0x13,	0x01, 0x08, 0x08, "Off"},
	{0x13,	0x01, 0x08, 0x00, "On"}, 
	{0,		0xFE, 0,	4,	  "Coin A"},
	{0x13,	0x01, 0x30, 0x10, "3 coins 1 play"},
	{0x13,	0x01, 0x30, 0x20, "2 coins 1 play"},
	{0x13,	0x01, 0x30, 0x00, "1 coin 1 play"},
	{0x13,	0x01, 0x30, 0x30, "1 coin 2 plays"},
	{0,		0xFE, 0,	4,	  "Coin B"},
	{0x13,	0x01, 0xc0, 0x40, "3 coins 1 play"},
	{0x13,	0x01, 0xc0, 0x80, "2 coins 1 play"},
	{0x13,	0x01, 0xc0, 0x00, "1 coin 1 play"},
	{0x13,	0x01, 0xc0, 0xc0, "1 coin 2 plays"},

	// DIP 2
	{0,		0xFE, 0,	4,	  "Difficulty"},
	{0x14,	0x01, 0x03, 0x01, "Easy"},
	{0x14,	0x01, 0x03, 0x00, "Normal"},
	{0x14,	0x01, 0x03, 0x02, "Hard"},
	{0x14,	0x01, 0x03, 0x03, "Very hard"},
	{0,		0xFE, 0,	4,	  "Player stock"},
	{0x14,	0x01, 0x0c, 0x0c, "2"},
	{0x14,	0x01, 0x0c, 0x00, "3"},
	{0x14,	0x01, 0x0c, 0x08, "4"},
	{0x14,	0x01, 0x0c, 0x04, "5"},
	{0,		0xFE, 0,	4,	  "Extend"},
	{0x14,	0x01, 0x30, 0x20, "150000, 500000pts"},
	{0x14,	0x01, 0x30, 0x00, "200000, 700000pts"},
	{0x14,	0x01, 0x30, 0x30, "250000 every"},
	{0x14,	0x01, 0x30, 0x10, "None"},
	{0,		0xFE, 0,	4,	  "Match count"},
	{0x14,	0x01, 0xc0, 0x00, "2"},
	{0x14,	0x01, 0xc0, 0x80, "3"},
	{0x14,	0x01, 0xc0, 0x40, "4"},
	{0x14,	0x01, 0xc0, 0xc0, "5"},
};

STDDIPINFO(penbros);

// Rom information

static struct BurnRomInfo grdiansRomDesc[] = {
	{ "u2.bin",		  0x080000, 0x36adc6f2, BRF_ESS | BRF_PRG },	// 68000 code
	{ "u3.bin",		  0x080000, 0x2704f416, BRF_ESS | BRF_PRG },
	{ "u4.bin",		  0x080000, 0xbb52447b, BRF_ESS | BRF_PRG },
	{ "u5.bin",		  0x080000, 0x9c164a3b, BRF_ESS | BRF_PRG },

	{ "u16.bin",	  0x400000, 0x6a65f265,	BRF_GRA },				// GFX
	{ "u20.bin",	  0x400000, 0xa7226ab7,	BRF_GRA },
	{ "u15.bin",	  0x400000, 0x01672dcd,	BRF_GRA },
	{ "u19.bin",	  0x400000, 0xc0c998a0,	BRF_GRA },
	{ "u18.bin",	  0x400000, 0x967babf4,	BRF_GRA },
	{ "u22.bin",	  0x400000, 0x6239997a,	BRF_GRA },
	{ "u17.bin",	  0x400000, 0x0fad0629,	BRF_GRA },
	{ "u21.bin",	  0x400000, 0x6f95e466,	BRF_GRA },

	{ "u32.bin",    0x100000, 0xcf0f3017, BRF_SND },				// PCM

};

STD_ROM_PICK(grdians);
STD_ROM_FN(grdians);

static struct BurnRomInfo mj4simaiRomDesc[] = {
	{ "ll.u2",		  0x080000, 0x7be9c781, BRF_ESS | BRF_PRG },	// 68000 code
	{ "lh1.u3",		  0x080000, 0x82aa3f72, BRF_ESS | BRF_PRG },
	{ "hl.u4",		  0x080000, 0x226063b7, BRF_ESS | BRF_PRG },
	{ "hh.u5",		  0x080000, 0x23aaf8df, BRF_ESS | BRF_PRG },

	{ "cha-03.u16",	  0x400000, 0xd367429a,	BRF_GRA },				// GFX
	{ "cha-04.u18",	  0x400000, 0x7f2008c3,	BRF_GRA },
	{ "cha-05.u15",	  0x400000, 0xe94ec40a,	BRF_GRA },
	{ "cha-06.u17",	  0x400000, 0x5cb0b3a9,	BRF_GRA },
	{ "cha-01.u21",	  0x400000, 0x35f47b37,	BRF_GRA },
	{ "cha-02.u22",	  0x400000, 0xf6346860,	BRF_GRA },

	{ "cha-07.u32",	  0x400000, 0x817519ee, BRF_SND },				// PCM

};

STD_ROM_PICK(mj4simai);
STD_ROM_FN(mj4simai);

static struct BurnRomInfo myangelRomDesc[] = {
	{ "kq1-prge.u2",  0x080000, 0x6137d4c0, BRF_ESS | BRF_PRG },	// 68000 code
	{ "kq1-prgo.u3",  0x080000, 0x4aad10d8, BRF_ESS | BRF_PRG },
	{ "kq1-tble.u4",  0x080000, 0xe332a514, BRF_ESS | BRF_PRG },
	{ "kq1-tblo.u5",  0x080000, 0x760cab15, BRF_ESS | BRF_PRG },

	{ "kq1-cg2.u20",  0x200000, 0x80b4e8de,	BRF_GRA },				// GFX
	{ "kq1-cg0.u16",  0x200000, 0xf8ae9a05,	BRF_GRA },
	{ "kq1-cg3.u19",  0x200000, 0x9bdc35c9,	BRF_GRA },
	{ "kq1-cg1.u15",  0x200000, 0x23bd7ea4,	BRF_GRA },
	{ "kq1-cg6.u22",  0x200000, 0xb25acf12,	BRF_GRA },
	{ "kq1-cg4.u18",  0x200000, 0xdca7f8f2,	BRF_GRA },
	{ "kq1-cg7.u21",  0x200000, 0x9f48382c,	BRF_GRA },
	{ "kq1-cg5.u17",  0x200000, 0xa4bc4516,	BRF_GRA },

	{ "kq1-snd.u32",  0x200000, 0x8ca1b449, BRF_SND },				// PCM

};

STD_ROM_PICK(myangel);
STD_ROM_FN(myangel);

static struct BurnRomInfo myangel2RomDesc[] = {
	{ "kqs1ezpr.u2",  0x080000, 0x2469aac2, BRF_ESS | BRF_PRG },	// 68000 code
	{ "kqs1ozpr.u3",  0x080000, 0x6336375c, BRF_ESS | BRF_PRG },
	{ "kqs1e-tb.u4",  0x080000, 0xe759b4cc, BRF_ESS | BRF_PRG },
	{ "kqs1o-tb.u5",  0x080000, 0xb6168737, BRF_ESS | BRF_PRG },

	{ "kqs1-cg4.u20", 0x200000, 0xd1802241,	BRF_GRA },				// GFX
	{ "kqs1-cg0.u16", 0x400000, 0xc21a33a7,	BRF_GRA },
	{ "kqs1-cg5.u19", 0x200000, 0xd86cf19c,	BRF_GRA },
	{ "kqs1-cg1.u15", 0x400000, 0xdca799ba,	BRF_GRA },
	{ "kqs1-cg6.u22", 0x200000, 0x3f08886b,	BRF_GRA },
	{ "kqs1-cg2.u18", 0x400000, 0xf7f92c7e,	BRF_GRA },
	{ "kqs1-cg7.u21", 0x200000, 0x2c977904,	BRF_GRA },
	{ "kqs1-cg3.u17", 0x400000, 0xde3b2191,	BRF_GRA },

	{ "kqs1-snd.u32", 0x400000, 0x792a6b49, BRF_SND },				// PCM
};

STD_ROM_PICK(myangel2);
STD_ROM_FN(myangel2);

static struct BurnRomInfo pzlbowlRomDesc[] = {
	{ "kup-u06.i03",  0x080000, 0x314e03ac, BRF_ESS | BRF_PRG },	// 68000 code
	{ "kup-u07.i03",  0x080000, 0xa0423a04, BRF_ESS | BRF_PRG },

	{ "kuc-u38.i00",  0x400000, 0x3db24172,	BRF_GRA },				// GFX
	{ "kuc-u39.i00",  0x400000, 0x9b26619b,	BRF_GRA },
	{ "kuc-u40.i00",  0x400000, 0x7e49a2cf,	BRF_GRA },
	{ "kuc-u41.i00",  0x400000, 0x2febf19b,	BRF_GRA },

	{ "kus-u18.i00",  0x400000, 0xe2b1dfcf, BRF_SND },				// PCM

};

STD_ROM_PICK(pzlbowl);
STD_ROM_FN(pzlbowl);

static struct BurnRomInfo penbrosRomDesc[] = {
	{ "u06.bin",	  0x080000, 0x7bbdffac, BRF_ESS | BRF_PRG },	// 68000 code
	{ "u07.bin",	  0x080000, 0xd50cda5f, BRF_ESS | BRF_PRG },

	{ "u38.bin",	  0x400000, 0x4247b39e,	BRF_GRA },				// GFX
	{ "u39.bin",	  0x400000, 0xf9f07faf,	BRF_GRA },
	{ "u40.bin",	  0x400000, 0xdc9e0a96,	BRF_GRA },

	{ "u18.bin",	  0x200000, 0xde4e65e2, BRF_SND },				// PCM
};

STD_ROM_PICK(penbros);
STD_ROM_FN(penbros);


static int MemIndex(int CodeSize, int GfxSize, int PcmSize, int ExtRamSize)
{
	unsigned char *Next; Next = Mem;
	Rom68K 		= Next; Next += CodeSize;			// 68000 ROM
	RomGfx		= Next; Next += GfxSize;			// GFX Rom
	X1010SNDROM	= Next; Next += PcmSize;			// PCM

	RamStart	= Next;

	Ram68K		= Next; Next += 0x010000;
	RamUnknown	= Next; Next += ExtRamSize;
	RamSpr		= (unsigned short *) Next; Next += 0x040000;
	RamSprBak	= (unsigned short *) Next; Next += 0x040000;
	RamPal		= (unsigned short *) Next; Next += 0x010000;
	RamTMP68301	= (unsigned short *) Next; Next += 0x000400;

	RamVReg		= (unsigned short *) Next; Next += 0x000040;

	RamEnd		= Next;

	CurPal		= (unsigned int *) Next; Next += 0x008000 * sizeof(unsigned int);

	MemEnd		= Next;
	return 0;
}

unsigned char __fastcall grdiansReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall grdiansReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x600000: return ~DrvInput[3]; // DIP 1
		case 0x600002: return ~DrvInput[4]; // DIP 2
		case 0x700000: return ~DrvInput[0]; // Joy 1
		case 0x700002: return ~DrvInput[1]; // Joy 2
		case 0x700004: return ~DrvInput[2]; // Coin

		case 0x70000C:
			//bprintf(PRINT_NORMAL, _T("watchdog_reset16_r\n"));
			return 0xFFFF;
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall grdiansWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
	}
}

void __fastcall grdiansWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x800000:
			//bprintf(PRINT_NORMAL, _T("lockout 0x%04x\n"), wordValue);
			break;

		case 0xE00010:
		case 0xE00012:
		case 0xE00014:
		case 0xE00016:
		case 0xE00018:
		case 0xE0001A:
		case 0xE0001C:
		case 0xE0001E:
			x1010_sound_bank_w( (sekAddress - 0xE00010) >> 1, wordValue );
			break;

		case 0xE00000:	// nop
			break;
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);
	}
}

unsigned char __fastcall setaSoundRegReadByte(unsigned int sekAddress)
{
	bprintf(PRINT_NORMAL, _T("x1-010 to read byte value of location %x\n"), sekAddress);
	return 0;
}

unsigned short __fastcall setaSoundRegReadWord(unsigned int sekAddress)
{
	bprintf(PRINT_NORMAL, _T("x1-010 to read word value of location %x\n"), sekAddress);
	return 0;
}

void __fastcall setaSoundRegWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	// bprintf(PRINT_NORMAL, _T("x1-010 to write byte value %x to location %x\n"), byteValue, sekAddress);
	unsigned int offset = (sekAddress & 0x00003fff) >> 1;
	int channel, reg;

	if (sekAddress & 1) {

		x1_010_chip->HI_WORD_BUF[ offset ] = byteValue;

	} else {

		offset ^= x1_010_chip->address;

		channel	= offset / sizeof(X1_010_CHANNEL);
		reg		= offset % sizeof(X1_010_CHANNEL);

		if( channel < SETA_NUM_CHANNELS && reg == 0 && (x1_010_chip->reg[offset]&1) == 0 && (byteValue&1) != 0 ) {
	 		x1_010_chip->smp_offset[channel] = 0;
	 		x1_010_chip->env_offset[channel] = 0;
		}
		x1_010_chip->reg[offset] = byteValue;

	}

}

void __fastcall setaSoundRegWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	//bprintf(PRINT_NORMAL, _T("x1-010 to write word value %x to location %x\n"), wordValue, sekAddress);
	unsigned int offset = (sekAddress & 0x00003fff) >> 1;
	int channel, reg;

	x1_010_chip->HI_WORD_BUF[ offset ] = wordValue >> 8;

	offset ^= x1_010_chip->address;

	channel	= offset / sizeof(X1_010_CHANNEL);
	reg		= offset % sizeof(X1_010_CHANNEL);

	if( channel < SETA_NUM_CHANNELS && reg == 0 && (x1_010_chip->reg[offset]&1) == 0 && (wordValue&1) != 0 ) {
	 	x1_010_chip->smp_offset[channel] = 0;
	 	x1_010_chip->env_offset[channel] = 0;
	}
	x1_010_chip->reg[offset] = wordValue & 0xff;
}

void __fastcall grdiansPaletteWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	bprintf(PRINT_NORMAL, _T("Pal to write byte value %x to location %x\n"), byteValue, sekAddress);
}

void __fastcall grdiansPaletteWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	//bprintf(PRINT_NORMAL, _T("Pal to write word value %x to location %x\n"), wordValue, sekAddress);
	sekAddress &= 0x00FFFF;
	sekAddress >>= 1;
	RamPal[sekAddress] = wordValue;
	CurPal[sekAddress] = CalcCol( wordValue );
}

void __fastcall grdiansClearWriteByte(unsigned int, unsigned char) {}
void __fastcall grdiansClearWriteWord(unsigned int, unsigned short) {}

int __fastcall grdiansSekIrqCallback(int irq)
{
	//bprintf(PRINT_NORMAL, _T("Sek Irq Call back %d vector %04x\n"), irq, tmp68301_irq_vector[irq]);
	return tmp68301_irq_vector[irq];
}

static int DrvDoReset()
{
	SekOpen(0);
    SekSetIRQLine(0, SEK_IRQSTATUS_NONE);
	SekReset();
	SekClose();

	return 0;
}

static void loadDecodeGfx(unsigned char *p, int cnt, int offset2x)
{
	unsigned char * d = RomGfx;
	unsigned char * q = p + 1;

	for (int i=0; i<cnt; i++, p+=2, q+=2, d+=8) {
		*(d+0) |= (( (*p >> 7) & 1 ) << offset2x) | (( (*q >> 7) & 1 ) << (offset2x + 1));
		*(d+1) |= (( (*p >> 6) & 1 ) << offset2x) | (( (*q >> 6) & 1 ) << (offset2x + 1));
		*(d+2) |= (( (*p >> 5) & 1 ) << offset2x) | (( (*q >> 5) & 1 ) << (offset2x + 1));
		*(d+3) |= (( (*p >> 4) & 1 ) << offset2x) | (( (*q >> 4) & 1 ) << (offset2x + 1));
		*(d+4) |= (( (*p >> 3) & 1 ) << offset2x) | (( (*q >> 3) & 1 ) << (offset2x + 1));
		*(d+5) |= (( (*p >> 2) & 1 ) << offset2x) | (( (*q >> 2) & 1 ) << (offset2x + 1));
		*(d+6) |= (( (*p >> 1) & 1 ) << offset2x) | (( (*q >> 1) & 1 ) << (offset2x + 1));
		*(d+7) |= (( (*p >> 0) & 1 ) << offset2x) | (( (*q >> 0) & 1 ) << (offset2x + 1));
	}

}

static int grdiansInit()
{
	int nRet;

	Mem = NULL;
	MemIndex(0x0200000, 0x2000000, 0x0200000, 0x00C000);
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex(0x0200000, 0x2000000, 0x0200000, 0x00C000);

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000000, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100001, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100000, 3, 2); if (nRet != 0) return 1;

	// Load Gfx
	unsigned char * tmpGfx = (unsigned char *)malloc(0x0800000);
	for (int i=0; i<8; i+=2) {
		BurnLoadRom(tmpGfx + 0x0200000, i+5, 1);
		memcpy(tmpGfx + 0x0600000, tmpGfx + 0x0200000, 0x0200000);
		BurnLoadRom(tmpGfx + 0x0000000, i+4, 1);
		loadDecodeGfx( tmpGfx, 0x0800000 / 2, i );
	}

	free(tmpGfx);

	// Leave 1MB empty (addressable by the chip)
	BurnLoadRom(X1010SNDROM + 0x100000, 12, 1);

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x1FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram68K,		0x200000, 0x20FFFF, SM_RAM);	// CPU 0 RAM
		SekMapMemory(RamUnknown,	0x304000, 0x30FFFF, SM_RAM);	// ? seems tile data

		SekMapMemory((unsigned char *)RamSpr,
									0xC00000, 0xC3FFFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0xC40000, 0xC4FFFF, SM_ROM);	// Palette
		SekMapMemory((unsigned char *)RamVReg,
									0xC60000, 0xC6003F, SM_RAM);	// Video Registers
		SekMapMemory((unsigned char *)RamTMP68301,
									0xFFFC00, 0xFFFFFF, SM_ROM);	// TMP68301 Registers

		SekMapHandler(1,			0xB00000, 0xB03FFF, SM_READ | SM_WRITE);
		SekMapHandler(2,			0xC40000, 0xC4FFFF, SM_WRITE);	// Palette
		SekMapHandler(3,			0xC50000, 0xC5FFFF, SM_WRITE);
		SekMapHandler(4,			0xFFFC00, 0xFFFFFF, SM_WRITE);

		SekSetReadWordHandler(0, grdiansReadWord);
		SekSetReadByteHandler(0, grdiansReadByte);
		SekSetWriteWordHandler(0, grdiansWriteWord);
		SekSetWriteByteHandler(0, grdiansWriteByte);

		SekSetReadWordHandler (1, setaSoundRegReadWord);
		SekSetReadByteHandler (1, setaSoundRegReadByte);
		SekSetWriteWordHandler(1, setaSoundRegWriteWord);
		SekSetWriteByteHandler(1, setaSoundRegWriteByte);

		SekSetWriteWordHandler(2, grdiansPaletteWriteWord);
		SekSetWriteByteHandler(2, grdiansPaletteWriteByte);

		SekSetWriteWordHandler(3, grdiansClearWriteWord);
		SekSetWriteByteHandler(3, grdiansClearWriteByte);

		SekSetWriteWordHandler(4, Tmp68301WriteWord);
		SekSetWriteByteHandler(4, Tmp68301WriteByte);

		SekSetIrqCallback( grdiansSekIrqCallback );

		SekClose();
	}

	yoffset = 0;
	sva_x = 128;
	sva_y = 128;
	sva_w = 304;
	sva_h = 232;

	gfx_code_mask = (0x0800000 * 4 / 64) - 1;

	x1010_sound_init(50000000 / 3, 0x0000);

	DrvDoReset();	

	return 0;
}

// -- mj4simai -----------------------------------------------------------

unsigned char __fastcall mj4simaiReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall mj4simaiReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {

		case 0x600006: 
			//bprintf(PRINT_NORMAL, _T("watchdog_reset16_r\n"));
			return 0xffff;

		case 0x600000:
		case 0x600002:
			switch (Mahjong_keyboard)
			{
				case 0x01: return ~DrvInput[0];
				case 0x02: return ~DrvInput[1];
				case 0x04: return ~DrvInput[5];
				case 0x08: return ~DrvInput[6];
				case 0x10: return ~DrvInput[7];
				default:   return 0xffff;
			}	

		case 0x600100: return ~DrvInput[2];	// Coin

		case 0x600300: return ~DrvInput[3]; // DIP 1
		case 0x600302: return ~DrvInput[4]; // DIP 2

//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall mj4simaiWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
	}
}

void __fastcall mj4simaiWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x600200: break; // NOP

		case 0x600300:
		case 0x600302:
		case 0x600304:
		case 0x600306:
		case 0x600308:
		case 0x60030A:
		case 0x60030C:
		case 0x60030E:
			x1010_sound_bank_w( (sekAddress & 0xf) >> 1, wordValue );
			break;
		case 0x600004:
			Mahjong_keyboard = wordValue & 0xff; 
			break;
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);
	}
}

static int mj4simaiInit()
{
	int nRet;

	Mem = NULL;
	MemIndex(0x0200000, 0x2000000, 0x0500000, 0x000000);
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex(0x0200000, 0x2000000, 0x0500000, 0x000000);

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000000, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100001, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100000, 3, 2); if (nRet != 0) return 1;

	// Load Gfx
	unsigned char * tmpGfx = (unsigned char *)malloc(0x0800000);
	for (int i=0; i<6; i+=2) {
		BurnLoadRom(tmpGfx + 0x0000000, i+4, 1);
		BurnLoadRom(tmpGfx + 0x0400000, i+5, 1);
		loadDecodeGfx( tmpGfx, 0x0800000 / 2, i );
	}
	free(tmpGfx);

	// Leave 1MB empty (addressable by the chip)
	BurnLoadRom(X1010SNDROM + 0x100000, 10, 1);

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x1FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram68K,		0x200000, 0x20FFFF, SM_RAM);	// CPU 0 RAM

		SekMapMemory((unsigned char *)RamSpr,
									0xC00000, 0xC3FFFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0xC40000, 0xC4FFFF, SM_ROM);	// Palette
		SekMapMemory((unsigned char *)RamVReg,
									0xC60000, 0xC6003F, SM_RAM);	// Video Registers
		SekMapMemory((unsigned char *)RamTMP68301,
									0xFFFC00, 0xFFFFFF, SM_ROM);	// TMP68301 Registers

		SekMapHandler(1,			0xB00000, 0xB03FFF, SM_READ | SM_WRITE);
		SekMapHandler(2,			0xC40000, 0xC4FFFF, SM_WRITE);	// Palette
		SekMapHandler(3,			0xFFFC00, 0xFFFFFF, SM_WRITE);

		SekSetReadWordHandler(0, mj4simaiReadWord);
		SekSetReadByteHandler(0, mj4simaiReadByte);
		SekSetWriteWordHandler(0, mj4simaiWriteWord);
		SekSetWriteByteHandler(0, mj4simaiWriteByte);

		SekSetReadWordHandler (1, setaSoundRegReadWord);
		SekSetReadByteHandler (1, setaSoundRegReadByte);
		SekSetWriteWordHandler(1, setaSoundRegWriteWord);
		SekSetWriteByteHandler(1, setaSoundRegWriteByte);

		SekSetWriteWordHandler(2, grdiansPaletteWriteWord);
		SekSetWriteByteHandler(2, grdiansPaletteWriteByte);

		SekSetWriteWordHandler(3, Tmp68301WriteWord);
		SekSetWriteByteHandler(3, Tmp68301WriteByte);

		SekSetIrqCallback( grdiansSekIrqCallback );

		SekClose();
	}

	yoffset = 0;
	sva_x = 64;
	sva_y = 128;
	sva_w = 384;
	sva_h = 240;

	gfx_code_mask = (0x0800000 * 4 / 64) - 1;

	x1010_sound_init(50000000 / 3, 0x0000);

//	bMahjong = 1;

	DrvDoReset();	

	return 0;
}

// -- myangel -----------------------------------------------------------

unsigned char __fastcall myangelReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall myangelReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x700000: return ~DrvInput[0];
		case 0x700002: return ~DrvInput[1];
		case 0x700004: return ~DrvInput[2];

		case 0x700006:
			//bprintf(PRINT_NORMAL, _T("watchdog_reset16_r\n"));
			return 0xffff;

		case 0x700300: return ~DrvInput[3]; // DIP 1
		case 0x700302: return ~DrvInput[4]; // DIP 2

//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall myangelWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
	}
}

void __fastcall myangelWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x700200: break; // NOP

		case 0x700310:
		case 0x700312:
		case 0x700314:
		case 0x700316:
		case 0x700318:
		case 0x70031A:
		case 0x70031C:
		case 0x70031E:
			x1010_sound_bank_w( (sekAddress & 0xf) >> 1, wordValue );
			break;

//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);
	}
}

static int myangelInit()
{
	int nRet;

	Mem = NULL;
	MemIndex(0x0200000, 0x1000000, 0x0300000, 0x000000);
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex(0x0200000, 0x1000000, 0x0300000, 0x000000);

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000000, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100001, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100000, 3, 2); if (nRet != 0) return 1;

	// Load Gfx
	unsigned char * tmpGfx = (unsigned char *)malloc(0x0400000);
	for (int i=0; i<8; i+=2) {
		BurnLoadRom(tmpGfx + 0x0000000, i+4, 1);
		BurnLoadRom(tmpGfx + 0x0200000, i+5, 1);
		loadDecodeGfx( tmpGfx, 0x0400000 / 2, i );
	}
	free(tmpGfx);

	// Leave 1MB empty (addressable by the chip)
	BurnLoadRom(X1010SNDROM + 0x100000, 12, 1);

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x1FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram68K,		0x200000, 0x20FFFF, SM_RAM);	// CPU 0 RAM

		SekMapMemory((unsigned char *)RamSpr,
									0xC00000, 0xC3FFFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0xC40000, 0xC4FFFF, SM_ROM);	// Palette
		SekMapMemory((unsigned char *)RamVReg,
									0xC60000, 0xC6003F, SM_RAM);	// Video Registers
		SekMapMemory((unsigned char *)RamTMP68301,
									0xFFFC00, 0xFFFFFF, SM_ROM);	// TMP68301 Registers

		SekMapHandler(1,			0xB00000, 0xB03FFF, SM_READ | SM_WRITE);
		SekMapHandler(2,			0xC40000, 0xC4FFFF, SM_WRITE);	// Palette
		SekMapHandler(3,			0xFFFC00, 0xFFFFFF, SM_WRITE);

		SekSetReadWordHandler(0, myangelReadWord);
		SekSetReadByteHandler(0, myangelReadByte);
		SekSetWriteWordHandler(0, myangelWriteWord);
		SekSetWriteByteHandler(0, myangelWriteByte);

		SekSetReadWordHandler (1, setaSoundRegReadWord);
		SekSetReadByteHandler (1, setaSoundRegReadByte);
		SekSetWriteWordHandler(1, setaSoundRegWriteWord);
		SekSetWriteByteHandler(1, setaSoundRegWriteByte);

		SekSetWriteWordHandler(2, grdiansPaletteWriteWord);
		SekSetWriteByteHandler(2, grdiansPaletteWriteByte);

		SekSetWriteWordHandler(3, Tmp68301WriteWord);
		SekSetWriteByteHandler(3, Tmp68301WriteByte);

		SekSetIrqCallback( grdiansSekIrqCallback );

		SekClose();
	}

	yoffset = 0x10;
	sva_x = 0;
	sva_y = 0;
	sva_w = 376;
	sva_h = 240;

	gfx_code_mask = (0x0400000 * 4 / 64) - 1;

	x1010_sound_init(50000000 / 3, 0x0000);

	DrvDoReset();	

	return 0;
}

// -- myangel2 -----------------------------------------------------------

unsigned char __fastcall myangel2ReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall myangel2ReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x600000: return ~DrvInput[0];
		case 0x600002: return ~DrvInput[1];
		case 0x600004: return ~DrvInput[2];

		case 0x600006:
			//bprintf(PRINT_NORMAL, _T("watchdog_reset16_r\n"));
			return 0xffff;

		case 0x600300: return ~DrvInput[3]; // DIP 1
		case 0x600302: return ~DrvInput[4]; // DIP 2

//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall myangel2WriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
	}
}

void __fastcall myangel2WriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		case 0x600200: break; // NOP

		case 0x600300:
		case 0x600302:
		case 0x600304:
		case 0x600306:
		case 0x600308:
		case 0x60030A:
		case 0x60030C:
		case 0x60030E:
			x1010_sound_bank_w( (sekAddress & 0xf) >> 1, wordValue );
			break;

//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);
	}
}

static int myangel2Init()
{
	int nRet;

	Mem = NULL;
	MemIndex(0x0200000, 0x1800000, 0x0500000, 0x000000);
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex(0x0200000, 0x1800000, 0x0500000, 0x000000);

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000000, 1, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100001, 2, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x100000, 3, 2); if (nRet != 0) return 1;

	// Load Gfx
	unsigned char * tmpGfx = (unsigned char *)malloc(0x0600000);
	for (int i=0; i<8; i+=2) {
		BurnLoadRom(tmpGfx + 0x0400000, i+4, 1);
		BurnLoadRom(tmpGfx + 0x0000000, i+5, 1);
		memcpy(tmpGfx + 0x0400000, tmpGfx + 0x0200000, 0x0200000);
		loadDecodeGfx( tmpGfx, 0x0600000 / 2, i );
	}
	free(tmpGfx);

	// Leave 1MB empty (addressable by the chip)
	BurnLoadRom(X1010SNDROM + 0x100000, 12, 1);

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x1FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram68K,		0x200000, 0x20FFFF, SM_RAM);	// CPU 0 RAM

		SekMapMemory((unsigned char *)RamSpr,
									0xD00000, 0xD3FFFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0xD40000, 0xD4FFFF, SM_ROM);	// Palette
		SekMapMemory((unsigned char *)RamVReg,
									0xD60000, 0xD6003F, SM_RAM);	// Video Registers
		SekMapMemory((unsigned char *)RamTMP68301,
									0xFFFC00, 0xFFFFFF, SM_ROM);	// TMP68301 Registers

		SekMapHandler(1,			0xB00000, 0xB03FFF, SM_READ | SM_WRITE);
		SekMapHandler(2,			0xD40000, 0xD4FFFF, SM_WRITE);	// Palette
		SekMapHandler(3,			0xFFFC00, 0xFFFFFF, SM_WRITE);

		SekSetReadWordHandler(0, myangel2ReadWord);
		SekSetReadByteHandler(0, myangel2ReadByte);
		SekSetWriteWordHandler(0, myangel2WriteWord);
		SekSetWriteByteHandler(0, myangel2WriteByte);

		SekSetReadWordHandler (1, setaSoundRegReadWord);
		SekSetReadByteHandler (1, setaSoundRegReadByte);
		SekSetWriteWordHandler(1, setaSoundRegWriteWord);
		SekSetWriteByteHandler(1, setaSoundRegWriteByte);

		SekSetWriteWordHandler(2, grdiansPaletteWriteWord);
		SekSetWriteByteHandler(2, grdiansPaletteWriteByte);

		SekSetWriteWordHandler(3, Tmp68301WriteWord);
		SekSetWriteByteHandler(3, Tmp68301WriteByte);

		SekSetIrqCallback( grdiansSekIrqCallback );

		SekClose();
	}

	yoffset = 0x10;
	sva_x = 0;
	sva_y = 0;
	sva_w = 376;
	sva_h = 240;

	gfx_code_mask = (0x0600000 * 4 / 64) - 1;

	x1010_sound_init(50000000 / 3, 0x0000);

	DrvDoReset();	

	return 0;
}

// -- pzlbowl -----------------------------------------------------------

unsigned char __fastcall pzlbowlReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall pzlbowlReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x400300: return ~DrvInput[3]; // DIP 1
		case 0x400302: return ~DrvInput[4]; // DIP 2
		case 0x500000: return ~DrvInput[0]; // Player 1
		case 0x500002: return ~DrvInput[1]; // Player 2

		case 0x500004: {
			//readinputport(4) | (rand() & 0x80 )
			static unsigned short prot = 0;
			prot ^= 0x80;
			return ~(prot | DrvInput[2]);
			}
		case 0x500006:
			//bprintf(PRINT_NORMAL, _T("watchdog_reset16_r\n"));
			return 0xffff;
		case 0x700000: {
			/*  The game checks for a specific value read from the ROM region.
    			The offset to use is stored in RAM at address 0x20BA16 */
			unsigned int address = (*(unsigned short *)(Ram68K + 0x00ba16) << 16) | *(unsigned short *)(Ram68K + 0x00ba18);
			bprintf(PRINT_NORMAL, _T("pzlbowl Protection read address %08x [%02x %02x %02x %02x]\n"), address,
			        Rom68K[ address - 2 ], Rom68K[ address - 1 ], Rom68K[ address - 0 ], Rom68K[ address + 1 ]);
			return Rom68K[ address - 2 ]; }
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall pzlbowlWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
	}
}

void __fastcall pzlbowlWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {

		case 0x400300:
		case 0x400302:
		case 0x400304:
		case 0x400306:
		case 0x400308:
		case 0x40030A:
		case 0x40030C:
		case 0x40030E:
			x1010_sound_bank_w( (sekAddress & 0xf) >> 1, wordValue );
			break;
		case 0x500004:
			//bprintf(PRINT_NORMAL, _T("Coin Counter %x\n"), wordValue);
			break;
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);
	}
}

static int pzlbowlInit()
{
	int nRet;

	Mem = NULL;
	MemIndex(0x0100000, 0x1000000, 0x0500000, 0x000000);
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex(0x0100000, 0x1000000, 0x0500000, 0x000000);

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000000, 1, 2); if (nRet != 0) return 1;

	// Load Gfx
	unsigned char * tmpGfx = (unsigned char *)malloc(0x0400000);
	for (int i=0; i<4; i++) {
		BurnLoadRom(tmpGfx, i+2, 1);
		loadDecodeGfx( tmpGfx, 0x0400000 / 2, i*2 );
	}
	free(tmpGfx);

	// Leave 1MB empty (addressable by the chip)
	BurnLoadRom(X1010SNDROM + 0x100000, 6, 1);

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x0FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram68K,		0x200000, 0x20FFFF, SM_RAM);	// CPU 0 RAM

		SekMapMemory((unsigned char *)RamSpr,
									0x800000, 0x83FFFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0x840000, 0x84FFFF, SM_ROM);	// Palette
		SekMapMemory((unsigned char *)RamVReg,
									0x860000, 0x86003F, SM_RAM);	// Video Registers
		SekMapMemory((unsigned char *)RamTMP68301,
									0xFFFC00, 0xFFFFFF, SM_ROM);	// TMP68301 Registers

		SekMapHandler(1,			0x900000, 0x903FFF, SM_READ | SM_WRITE);
		SekMapHandler(2,			0x840000, 0x84FFFF, SM_WRITE);	// Palette
		SekMapHandler(3,			0xFFFC00, 0xFFFFFF, SM_WRITE);

		SekSetReadWordHandler(0, pzlbowlReadWord);
		SekSetReadByteHandler(0, pzlbowlReadByte);
		SekSetWriteWordHandler(0, pzlbowlWriteWord);
		SekSetWriteByteHandler(0, pzlbowlWriteByte);

		SekSetReadWordHandler (1, setaSoundRegReadWord);
		SekSetReadByteHandler (1, setaSoundRegReadByte);
		SekSetWriteWordHandler(1, setaSoundRegWriteWord);
		SekSetWriteByteHandler(1, setaSoundRegWriteByte);

		SekSetWriteWordHandler(2, grdiansPaletteWriteWord);
		SekSetWriteByteHandler(2, grdiansPaletteWriteByte);

		SekSetWriteWordHandler(3, Tmp68301WriteWord);
		SekSetWriteByteHandler(3, Tmp68301WriteByte);

		SekSetIrqCallback( grdiansSekIrqCallback );

		SekClose();
	}

	yoffset = 0;
	sva_x = 16;
	sva_y = 256;
	sva_w = 384;
	sva_h = 240;

	gfx_code_mask = (0x0400000 * 4 / 64) - 1;

	x1010_sound_init(50000000 / 3, 0x0000);

	DrvDoReset();

	return 0;
}

// -- penbros -----------------------------------------------------------

unsigned char __fastcall penbrosReadByte(unsigned int sekAddress)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to read byte value of location %x\n"), sekAddress);
	}
	return 0;
}

unsigned short __fastcall penbrosReadWord(unsigned int sekAddress)
{
	switch (sekAddress) {
		case 0x500300: return ~DrvInput[3]; // DIP 1
		case 0x500302: return ~DrvInput[4]; // DIP 2
		case 0x600000: return ~DrvInput[0]; // Player 1
		case 0x600002: return ~DrvInput[1]; // Player 2
		case 0x600004: return ~DrvInput[2]; // Coin

		case 0x600006: 
			//bprintf(PRINT_NORMAL, _T("watchdog_reset16_r\n"));
			return 0xffff;
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to read word value of location %x\n"), sekAddress);
	}
	return 0;
}

void __fastcall penbrosWriteByte(unsigned int sekAddress, unsigned char byteValue)
{
	switch (sekAddress) {

		default:
			bprintf(PRINT_NORMAL, _T("Attempt to write byte value %x to location %x\n"), byteValue, sekAddress);
	}
}

void __fastcall penbrosWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {

		case 0x500300:
		case 0x500302:
		case 0x500304:
		case 0x500306:
		case 0x500308:
		case 0x50030A:
		case 0x50030C:
		case 0x50030E:
			x1010_sound_bank_w( (sekAddress & 0xf) >> 1, wordValue );
			break;
		case 0x600004:
			//bprintf(PRINT_NORMAL, _T("Coin Counter %x\n"), wordValue);
			break;
//		default:
//			bprintf(PRINT_NORMAL, _T("Attempt to write word value %x to location %x\n"), wordValue, sekAddress);
	}
}

static int penbrosInit()
{
	int nRet;

	Mem = NULL;
	MemIndex(0x0100000, 0x1000000, 0x0300000, 0x040000);
	int nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);										// blank all memory
	MemIndex(0x0100000, 0x1000000, 0x0300000, 0x040000);

	// Load and byte-swap 68000 Program roms
	nRet = BurnLoadRom(Rom68K + 0x000001, 0, 2); if (nRet != 0) return 1;
	nRet = BurnLoadRom(Rom68K + 0x000000, 1, 2); if (nRet != 0) return 1;

	// Load Gfx
	unsigned char * tmpGfx = (unsigned char *)malloc(0x0400000);
	for (int i=0; i<3; i++) {
		BurnLoadRom(tmpGfx, i+2, 1);
		loadDecodeGfx( tmpGfx, 0x0400000 / 2, i*2 );
	}
	free(tmpGfx);

	// Leave 1MB empty (addressable by the chip)
	BurnLoadRom(X1010SNDROM + 0x100000, 5, 1);

	{
		SekInit(0, 0x68000);										// Allocate 68000
	    SekOpen(0);

		// Map 68000 memory:
		SekMapMemory(Rom68K,		0x000000, 0x0FFFFF, SM_ROM);	// CPU 0 ROM
		SekMapMemory(Ram68K,		0x200000, 0x20FFFF, SM_RAM);	// CPU 0 RAM

		SekMapMemory(RamUnknown + 0x00000,	
									0x210000, 0x23FFFF, SM_RAM);
		SekMapMemory(RamUnknown + 0x30000,	
									0x300000, 0x30FFFF, SM_RAM);

		SekMapMemory((unsigned char *)RamSpr,
									0xB00000, 0xB3FFFF, SM_RAM);	// sprites
		SekMapMemory((unsigned char *)RamPal,
									0xB40000, 0xB4FFFF, SM_ROM);	// Palette
		SekMapMemory((unsigned char *)RamVReg,
									0xB60000, 0xB6003F, SM_RAM);	// Video Registers
		SekMapMemory((unsigned char *)RamTMP68301,
									0xFFFC00, 0xFFFFFF, SM_ROM);	// TMP68301 Registers

		SekMapHandler(1,			0xA00000, 0xA03FFF, SM_READ | SM_WRITE);
		SekMapHandler(2,			0xB40000, 0xB4FFFF, SM_WRITE);	// Palette
		SekMapHandler(3,			0xFFFC00, 0xFFFFFF, SM_WRITE);

		SekSetReadWordHandler(0, penbrosReadWord);
		SekSetReadByteHandler(0, penbrosReadByte);
		SekSetWriteWordHandler(0, penbrosWriteWord);
		SekSetWriteByteHandler(0, penbrosWriteByte);

		SekSetReadWordHandler (1, setaSoundRegReadWord);
		SekSetReadByteHandler (1, setaSoundRegReadByte);
		SekSetWriteWordHandler(1, setaSoundRegWriteWord);
		SekSetWriteByteHandler(1, setaSoundRegWriteByte);

		SekSetWriteWordHandler(2, grdiansPaletteWriteWord);
		SekSetWriteByteHandler(2, grdiansPaletteWriteByte);

		SekSetWriteWordHandler(3, Tmp68301WriteWord);
		SekSetWriteByteHandler(3, Tmp68301WriteByte);

		SekSetIrqCallback( grdiansSekIrqCallback );

		SekClose();
	}

	yoffset = 0;
	sva_x = 0;
	sva_y = 128;
	sva_w = 320;
	sva_h = 224;

	gfx_code_mask = (0x0400000 * 4 / 64) - 1;

	x1010_sound_init(50000000 / 3, 0x0000);

	DrvDoReset();

	return 0;
}


static int grdiansExit()
{
	SekExit();
	x1010_exit();

	free(Mem);
	Mem = NULL;

//	bMahjong = 0;

	return 0;
}


#define	DRAWGFX( op )												\
	code &= gfx_code_mask;											\
	if (!code) return;												\
	sx -= sva_x;													\
	sy -= sva_y;													\
																	\
	if (sx <= -8) return;											\
	if (sx >= sva_w) return;										\
	if (sy <= -8) return;											\
	if (sy >= sva_h) return;										\
																	\
	unsigned short * pd = (unsigned short *)pBurnDraw;				\
	unsigned char * ps = RomGfx + (code << 6); 						\
	unsigned int * pc = CurPal + (color << 4);					\
																	\
	if (sx < 0 || sx > sva_w - 8 || sy < 0 || sy > sva_h - 8) {		\
		if ( flipy ) {												\
			pd += (sy + 7) * sva_w + sx;							\
			if ( flipx ) {											\
				for (int i=7;i>=0;i--,pd-=sva_w)					\
					if ( sy+i < 0 || sy+i >= sva_h )				\
						ps += 8;									\
					else											\
						for (int j=7;j>=0;j--,ps++) {				\
							unsigned char c = op;					\
							if ( c && sx+j >= 0 && sx+j < sva_w ) 	\
								*(pd + j) = pc[ c ];				\
						}											\
			} else													\
				for (int i=7;i>=0;i--,pd-=sva_w)					\
					if ( sy+i < 0 || sy+i >= sva_h )				\
						ps += 8;									\
					else											\
						for (int j=0;j<8;j++,ps++) {				\
							unsigned char c = op;					\
							if ( c && sx+j >= 0 && sx+j < sva_w ) 	\
								*(pd + j) = pc[ c ];				\
						}											\
		} else {													\
			pd += sy * sva_w + sx;									\
			if ( flipx ) {											\
				for (int i=0;i<8;i++,pd+=sva_w)						\
					if ( sy+i < 0 || sy+i >= sva_h )				\
						ps += 8;									\
					else											\
						for (int j=7;j>=0;j--,ps++) {				\
							unsigned char c = op;					\
							if ( c && sx+j >= 0 && sx+j < sva_w ) 	\
								*(pd + j) = pc[ c ];				\
						}											\
			} else													\
				for (int i=0;i<8;i++,pd+=sva_w)						\
					if ( sy+i < 0 || sy+i >= sva_h )				\
						ps += 8;									\
					else											\
						for (int j=0;j<8;j++,ps++) {				\
							unsigned char c = op;					\
							if ( c && sx+j >= 0 && sx+j < sva_w ) 	\
								*(pd + j) = pc[ c ];				\
						}											\
		}															\
		return;														\
	}																\
																	\
																	\
	if ( flipy ) {													\
		pd += (sy + 7) * sva_w + sx;								\
		if ( flipx ) 												\
			for (int i=0;i<8;i++,pd-=sva_w)							\
				for (int j=7;j>=0;j--,ps++) {						\
					unsigned char c = op;							\
					if ( c ) *(pd + j) = pc[ c ];					\
				}													\
		else														\
			for (int i=0;i<8;i++,pd-=sva_w)							\
				for (int j=0;j<8;j++,ps++) {						\
					unsigned char c = op;							\
					if ( c ) *(pd + j) = pc[ c ];					\
				}													\
	} else {														\
		pd += sy * sva_w + sx;										\
		if ( flipx ) 												\
			for (int i=0;i<8;i++,pd+=sva_w)							\
				for (int j=7;j>=0;j--,ps++) {						\
					unsigned char c = op;							\
					if ( c ) *(pd + j) = pc[ c ];					\
				}													\
		else														\
			for (int i=0;i<8;i++,pd+=sva_w)							\
				for (int j=0;j<8;j++,ps++) {						\
					unsigned char c = op;							\
					if ( c ) *(pd + j) = pc[ c ];					\
				}													\
	}																\


static void drawgfx0(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy)
{
	//	4bpp tiles (----3210)
	DRAWGFX( *ps & 0x0f );
}

static void drawgfx1(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy)
{
	//	4bpp tiles (3210----)
	DRAWGFX( *ps >> 4 );
}

static void drawgfx2(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy)
{
	// 6bpp tiles (--543210)
	DRAWGFX( *ps & 0x3f );
}

static void drawgfx3(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy)
{
	// 8bpp tiles (76543210)
	DRAWGFX( *ps );
}

static void drawgfx4(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy)
{
	// 3bpp tiles?  (-----210)
	DRAWGFX( *ps & 0x07 );
}

static void drawgfx5(unsigned int code,unsigned int color,int flipx,int flipy,int sx,int sy)
{
	// 2bpp tiles??? (--10----)
	DRAWGFX( (*ps >> 4) & 0x03 );
}

static void drawgfxN(unsigned int,unsigned int,int,int,int,int)
{
	// unknown 
}

typedef void (*pDrawgfx)(unsigned int,unsigned int,int,int,int,int);

static void DrvDraw()
{
	memset(pBurnDraw, 0, sva_w * sva_h * 2);

	if (RamVReg[0x30/2] & 1) { // Blank Screen
		memcpy(RamSprBak, RamSpr, 0x040000);
		return;
	}

	unsigned short *s1  = RamSprBak + 0x3000 / 2;
	unsigned short *end = RamSprBak + 0x040000 / 2;

	for ( ; s1 < end; s1+=4 ) {
		int num		= s1[0];
		int xoffs	= s1[1];
		int yoffs	= s1[2];
		int sprite	= s1[3];
		pDrawgfx drawgfx = drawgfxN;

		// Single-sprite address
		unsigned short *s2 = RamSprBak + (sprite & 0x7fff) * 4;

		// Single-sprite tile size
		int global_sizex = xoffs & 0x0c00;
		int global_sizey = yoffs & 0x0c00;

		int use_global_size = num & 0x1000;

		xoffs &= 0x3ff;
		yoffs &= 0x3ff;

		// Color depth
		switch (num & 0x0700)
		{
			case 0x0700: 	// 8bpp tiles (76543210)
				drawgfx = drawgfx3; break;
			case 0x0600:	// 6bpp tiles (--543210) (myangel sliding blocks test)
				drawgfx = drawgfx2; break;
			case 0x0500:	// 4bpp tiles (3210----)
				drawgfx = drawgfx1; break;
			case 0x0400:	// 4bpp tiles (----3210)
				drawgfx = drawgfx0; break;
			case 0x0200:	// 3bpp tiles?  (-----210) (myangel "Graduate Tests")
				drawgfx = drawgfx4; break;
			case 0x0100:	// 2bpp tiles??? (--10----) (myangel2 question bubble, myangel endgame)
				drawgfx = drawgfx5; break;
			//case 0x0000:	// no idea!
			//	drawgfx = drawgfx0; break;
			//default:
				//bprintf(PRINT_NORMAL, _T("unknown gfxset %x\n"), (num & 0x0700)>>8 );
			//	drawgfx = drawgfxN;
		}

		// Number of single-sprites
		num = (num & 0x00ff) + 1;

		for( ; num > 0; num--,s2+=4 ) {
			if (s2 >= end)	break;

			if (sprite & 0x8000) {
				// "tilemap" sprite
				int clip_min_y;
				int clip_max_y;
				int clip_min_x;
				int clip_max_x;

				int dx,x,y;
				int flipx;
				int flipy;
				int sx       = s2[0];
				int sy       = s2[1];
				int scrollx  = s2[2];
				int scrolly  = s2[3];
				int tilesize = (scrollx & 0x8000) >> 15;
				int page     = (scrollx & 0x7c00) >> 10;
				int height   = ((sy & 0xfc00) >> 10) + 1;

				sx &= 0x3ff;
				sy &= 0x1ff;
				scrollx &= 0x3ff;
				scrolly &= 0x1ff;

				clip_min_y = (sy + yoffs) & 0x1ff;
				clip_max_y = clip_min_y + height * 0x10 - 1;
				if (clip_min_y > (sva_y + sva_h - 1)) continue;
				if (clip_max_y < sva_y) continue;

				clip_min_x = sva_x;
				clip_max_x = sva_x + sva_w - 1;

				if (clip_min_y < sva_y) clip_min_y = sva_y;
				if (clip_max_y > (sva_y + sva_h - 1)) clip_max_y = sva_y + sva_h - 1;

				dx = sx + (scrollx & 0x3ff) + xoffs + 0x10;

				/* Draw the rows */
				/* I don't think the following is entirely correct (when using 16x16
                   tiles x should probably loop from 0 to 0x20) but it seems to work
                   fine in all the games we have for now. */
				for (y = 0; y < (0x40 >> tilesize); y++)
				{
					int py = ((scrolly - (y+1) * (8 << tilesize) + 0x10) & 0x1ff) - 0x10 - yoffset;

					if (py < clip_min_y - 0x10) continue;
					if (py > clip_max_y) continue;

					for (x = 0; x < 0x40;x++)
					{
						int px = ((dx + x * (8 << tilesize) + 0x10) & 0x3ff) - 0x10;
						int tx, ty;
						int attr, code, color;
						unsigned short *s3;

						if (px < clip_min_x - 0x10) continue;
						if (px > clip_max_x) continue;

						s3 = RamSprBak + 2 * ((page * 0x2000/4) + ((y & 0x1f) << 6) + (x & 0x03f));

						attr  = s3[0];
						code  = s3[1] + ((attr & 0x0007) << 16);
						flipx = (attr & 0x0010);
						flipy = (attr & 0x0008);
						color = (attr & 0xffe0) >> 5;

						if (tilesize) code &= ~3;

						for (ty = 0; ty <= tilesize; ty++)
							for (tx = 0; tx <= tilesize; tx++)
								drawgfx(code ^ tx ^ (ty<<1), color, flipx, flipy, px + (flipx ? tilesize-tx : tx) * 8, py + (flipy ? tilesize-ty : ty) * 8 );

					}
				}

			} else {
				// "normal" sprite	
				int sx    = s2[0];
				int sy    = s2[1];
				int attr  = s2[2];
				int code  = s2[3] + ((attr & 0x0007) << 16);
				int flipx = (attr & 0x0010);
				int flipy = (attr & 0x0008);
				int color = (attr & 0xffe0) >> 5;

				int sizex = use_global_size ? global_sizex : sx;
				int sizey = use_global_size ? global_sizey : sy;
				int x,y;
				sizex = (1 << ((sizex & 0x0c00)>> 10))-1;
				sizey = (1 << ((sizey & 0x0c00)>> 10))-1;


				sx += xoffs;
				sy += yoffs;

				sx = (sx & 0x1ff) - (sx & 0x200);
				sy &= 0x1ff;
				sy -= yoffset;

				code &= ~((sizex+1) * (sizey+1) - 1);	// see myangel, myangel2 and grdians

				for (y = 0; y <= sizey; y++)
					for (x = 0; x <= sizex; x++)
						drawgfx( code++, color, flipx, flipy, sx + (flipx ? sizex-x : x) * 8, sy + (flipy ? sizey-y : y) * 8 );

			}

		}
		if (s1[0] & 0x8000) break;	// end of list marker
	}

	memcpy(RamSprBak, RamSpr, 0x040000);
}


#define M68K_CYCS_PER_FRAME	(M68K_CYCS / 60)
#define	SETA2_INTER_LEAVE		32
#define M68K_CYCS_PER_INTER	(M68K_CYCS_PER_FRAME / SETA2_INTER_LEAVE)

static int grdiansFrame()
{
	if (DrvReset)														// Reset machine
		DrvDoReset();

	if (bRecalcPalette) {
		for (int i=0;i<0x08000; i++)
			CurPal[i] = CalcCol( RamPal[i] );
		bRecalcPalette = 0;	
	}

	DrvInput[0] = 0x00;													// Joy1
	DrvInput[1] = 0x00;													// Joy2
	DrvInput[2] = 0x00;													// Buttons
	DrvInput[5] = 0x00;													// Joy3
	DrvInput[6] = 0x00;													// Joy4
	DrvInput[7] = 0x00;													// Joy5

	for (int i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvJoy1[i] & 1) << i;
		DrvInput[1] |= (DrvJoy2[i] & 1) << i;
		DrvInput[2] |= (DrvButton[i] & 1) << i;
		DrvInput[5] |= (DrvJoy3[i] & 1) << i;
		DrvInput[6] |= (DrvJoy4[i] & 1) << i;
		DrvInput[6] |= (DrvJoy5[i] & 1) << i;
	}

	int nCyclesDone = 0;
	int nCyclesNext = 0;
	int nCyclesExec = 0;

	SekNewFrame();

	SekOpen(0);

	for(int i=0; i<SETA2_INTER_LEAVE; i++) {
		nCyclesNext += M68K_CYCS_PER_INTER;
		nCyclesExec = SekRun( nCyclesNext - nCyclesDone );
		nCyclesDone += nCyclesExec;

		for (int j=0;j<3;j++)
		if (tmp68301_timer[j]) {
			tmp68301_timer_counter[j] += nCyclesExec;
			if (tmp68301_timer_counter[j] >= tmp68301_timer[j]) {
				// timer[j] timeout !
				tmp68301_timer[j] = 0;
				tmp68301_timer_counter[j] = 0;
				tmp68301_timer_callback(j);
			}
		}
	}

	tmp68301_update_irq_state(0);

	SekClose();

	if (pBurnDraw)
		DrvDraw();

	if (pBurnSoundOut)
		x1010_sound_update();

	return 0;
}

static int grdiansScan(int nAction,int *pnMin)
{
	struct BurnArea ba;

	if (pnMin) 											// Return minimum compatible version
		*pnMin =  0x029671;

	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = RamStart;
		ba.nLen	  = RamEnd - RamStart;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {

		// Scan 68000 state
		SekScan(nAction);
		
		x1010_scan(nAction, pnMin);

		// Scan Input state
		SCAN_VAR(DrvInput);

		// Scan TMP 68301 Chip state
		SCAN_VAR(tmp68301_timer);
		SCAN_VAR(tmp68301_timer_counter);
		SCAN_VAR(tmp68301_irq_vector);

		if (nAction & ACB_WRITE) {

			// palette changed 
			bRecalcPalette = 1;

			// x1-010 bank changed
			for (int i=0; i<SETA_NUM_BANKS; i++)
				memcpy(X1010SNDROM + i * 0x20000, X1010SNDROM + 0x100000 + x1_010_chip->sound_banks[i] * 0x20000, 0x20000);
		}
	}

	return 0;
}

struct BurnDriver BurnDrvGrdians = {
	"grdians", NULL, NULL, "1995",
	"Guardians\0Denjin Makai II\0", NULL, "Banpresto", "Newer Seta",
	L"Guardians\0\u96FB\u795E\u9B54\u584A \uFF29\uFF29\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_MISC_POST90S,
	NULL, grdiansRomInfo, grdiansRomName, grdiansInputInfo, grdiansDIPInfo,
	grdiansInit, grdiansExit, grdiansFrame, NULL, grdiansScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	304, 232, 4, 3
};

struct BurnDriver BurnDrvMj4simai = {
	"mj4simai", NULL, NULL, "1996",
	"Wakakusamonogatari Mahjong Yonshimai (Japan)\0", NULL, "Maboroshi Ware", "Newer Seta",
	L"\u82E5\u8349\u7269\u8A9E \u9EBB\u96C0\u56DB\u59C9\u59B9 (Japan)\0Wakakusamonogatari Mahjong Yonshimai\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 1, HARDWARE_MISC_POST90S,
	NULL, mj4simaiRomInfo, mj4simaiRomName, mj4simaiInputInfo, mj4simaiDIPInfo,
	mj4simaiInit, grdiansExit, grdiansFrame, NULL, grdiansScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	384, 240, 4, 3	// 16, 9
};

struct BurnDriver BurnDrvMyangel = {
	"myangel", NULL, NULL, "1996",
	"Kosodate Quiz My Angel (Japan)\0", NULL, "Namco", "Newer Seta",
	L"\u5B50\u80B2\u3066\u30AF\u30A4\u30BA \u30DE\u30A4 \u30A8\u30F3\u30B8\u30A7\u30EB (Japan)\0Kosodate Quiz My Angel\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_MISC_POST90S,
	NULL, myangelRomInfo, myangelRomName, myangelInputInfo, myangelDIPInfo,
	myangelInit, grdiansExit, grdiansFrame, NULL, grdiansScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	376, 240, 4, 3
};

struct BurnDriver BurnDrvMyangel2 = {
	"myangel2", NULL, NULL, "1996",
	"Kosodate Quiz My Angel 2 (Japan)\0", NULL, "Namco", "Newer Seta",
	L"\u5B50\u80B2\u3066\u30AF\u30A4\u30BA \u30DE\u30A4 \u30A8\u30F3\u30B8\u30A7\u30EB \uFF12 (Japan)\0Kosodate Quiz My Angel 2\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_MISC_POST90S,
	NULL, myangel2RomInfo, myangel2RomName, myangelInputInfo, myangel2DIPInfo,
	myangel2Init, grdiansExit, grdiansFrame, NULL, grdiansScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	376, 240, 4, 3
};

struct BurnDriver BurnDrvPzlbowl = {
	"pzlbowl", NULL, NULL, "1999",
	"Puzzle De Bowling (Japan)\0", NULL, "Nihon System / Moss", "Newer Seta",
	L"Puzzle De Bowling\0\u30D1\u30BA\u30EB \uFF24\uFF25 \u30DC\u30FC\u30EA\u30F3\u30B0\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_MISC_POST90S,
	NULL, pzlbowlRomInfo, pzlbowlRomName, grdiansInputInfo, pzlbowlDIPInfo,
	pzlbowlInit, grdiansExit, grdiansFrame, NULL, grdiansScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	384, 240, 4, 3
};

struct BurnDriver BurnDrvPenbros = {
	"penbros", NULL, NULL, "2000",
	"Penguin Brothers (Japan)\0", NULL, "Subsino", "Newer Seta",
	L"\u30DA\u30F3\u30AE\u30F3 \u30D6\u30E9\u30B6\u30FC\u30BA (Japan)\0Penguin Brothers\0", NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_16BIT_ONLY, 2, HARDWARE_MISC_POST90S,
	NULL, penbrosRomInfo, penbrosRomName, penbrosInputInfo, penbrosDIPInfo,
	penbrosInit, grdiansExit, grdiansFrame, NULL, grdiansScan, 0, NULL, NULL, NULL, &bRecalcPalette,
	320, 224, 4, 3
};
