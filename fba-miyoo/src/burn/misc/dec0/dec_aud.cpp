#include "dec0.h"
#include "tiles_generic.h"
#include "timer.h"
#include "msm6295.h"
#include "burn_ym2203.h"
#include "burn_ym3812.h"

static unsigned char *Ram, *Rom;

void dec0_aud_write_byte(unsigned short address, unsigned char data)
{
	if (address == 0x0800) // YM2203 write
	{
		BurnYM2203Write(0, 0, data);
		return ;
	}
	if ( address ==0x0801)
	{
		BurnYM2203Write(0, 1, data);
		return ;
	}
	if (address == 0x1000) // YM3812 write
	{
		BurnYM3812Write(0, data);
		return ;
	}
	if (address == 0x1001)
	{
		BurnYM3812Write(1, data);
		return ;
	}
	if (address == 0x3800) // OKI write
	{
		MSM6295Command(0, data);
		return ;
	}
	bprintf(PRINT_NORMAL, _T("unmapped audio write: %x: %x,\n"),address,data);
}

unsigned char dec0_aud_read_byte(unsigned short address)
{
	switch (address)
	{
	case 0x3000:  // soundlatch read
		bprintf(PRINT_NORMAL, _T("soundlatch read: %x,\n"),robocopSoundLatch);
		return robocopSoundLatch;
	
		break; 
	case 0x3800: // oki staus read
		return MSM6295ReadStatus(0);
		break;
	default:
		bprintf(PRINT_NORMAL, _T("unmapped audio read: %x,\n"),address);
		break;
	}

	return 0;
}

inline static double DrvGetTime()
{
	return (double)SekTotalCycles() / 10000000;
}


static int dec0SynchroniseStream(int nSoundRate)
{
	int result = (long long)SekTotalCycles() * nSoundRate / 10000000;
	BurnTimerUpdate(result);
	return result;
}

// this is wrong
inline static void dec0YMxxxxIrqHandler(int, int nStatus)
{
	if (nStatus) {
		m6502SetIRQ(M6502_IRQ);
	} else {
		m6502SetIRQ(0);
	}
}


void init_dec0_aud(int romNumPrg, int romNumSamples)
{
	Ram = (unsigned char*) malloc(0x0600);
	Rom = (unsigned char*) malloc(0x8000);
	
	memset(Ram,0x00, sizeof(Ram));

	MSM6295ROM = (unsigned char*) malloc(0x10000);

	BurnLoadRom(Rom, romNumPrg,1 );
	BurnLoadRom(MSM6295ROM,romNumSamples,1);

	m6502Init(1);
	m6502Open(0);

	m6502MapMemory(Ram, 0x0000, 0x05ff, M6502_RAM); // Ram

	m6502MapMemory(Rom, 0x8000, 0xffff, M6502_ROM); // Rom


	m6502SetReadHandler(dec0_aud_read_byte);
	m6502SetWriteHandler(dec0_aud_write_byte);
	m6502Close();

	BurnYM3812Init(3000000, &dec0YMxxxxIrqHandler, &dec0SynchroniseStream, 1);

	BurnYM2203Init(1, 1500000, &dec0YMxxxxIrqHandler, dec0SynchroniseStream, DrvGetTime, 0);

	// Setup the OKIM6295 emulation
	MSM6295Init(0, 1023924/132, 30.0, 1);
	BurnTimerAttachSek(10000000);
}

void reset_dec0_aud()
{
	MSM6295Reset(0);
	BurnYM3812Reset();
	BurnYM2203Reset();

	m6502Open(0);
	m6502Reset();
	m6502Close();
}

void exit_dec0_aud()
{
	BurnYM3812Exit();
	BurnYM2203Exit();
	MSM6295Exit(0);
	free(Ram);
	free(Rom);
	free(MSM6295ROM);
	m6502Exit();
}

// not sure we need this yet
void render_dec_aud(int /*interleave*/, int /*cycleseg*/)
{

}

void endframe_dec_aud(int cycles)
{
	BurnTimerEndFrame(cycles);

	if (pBurnSoundOut)
	{
		BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
		BurnYM3812Update(pBurnSoundOut, nBurnSoundLen);
		MSM6295Render(0, pBurnSoundOut, nBurnSoundLen);
	}	
}
