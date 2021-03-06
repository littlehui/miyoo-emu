#include "cps.h"
#include "burn_ym2151.h"

// CPS1 sound Mixing

int bPsmOkay = 0;										// 1 if the module is okay
static short* WaveBuf = NULL;

static int nPos;

int PsmInit()
{
	int nMemLen, nRate, nRet;
	bPsmOkay = 0;										// not OK yet

	if (nBurnSoundRate > 0) {
		nRate = nBurnSoundRate;
	} else {
		nRate = 11025;
	}

	if (BurnYM2151Init(3579540, 50.0)) {				// Init FM sound chip
		return 1;
	}

	// Allocate a buffer for the intermediate sound (between YM2151 and pBurnSoundOut)
	nMemLen = nBurnSoundLen * 2 * sizeof(short);
	WaveBuf = (short*)malloc(nMemLen);
	if (WaveBuf == NULL) {
		PsmExit();
		return 1;
	}
	memset(WaveBuf, 0, nMemLen);						// Init to silence

	// Init ADPCM
	MSM6295ROM = CpsAd;
	if (Forgottn) {
		nRet = MSM6295Init(0, 6061, 21.5, 1);
	} else {
		nRet = MSM6295Init(0, 7576, 21.5, 1);
	}

	if (nRet!=0) {
		PsmExit(); return 1;
	}

	bPsmOkay = 1;										// OK

	return 0;
}

int PsmExit()
{
	bPsmOkay = 0;

	MSM6295Exit(0);

	free(WaveBuf);
	WaveBuf = NULL;

	BurnYM2151Exit();									// Exit FM sound chip
	return 0;
}

void PsmNewFrame()
{
	nPos = 0;
}

int PsmUpdate(int nEnd)
{
	if (bPsmOkay == 0 || pBurnSoundOut == NULL) {
		return 1;
	}

	if (nEnd <= nPos) {
		return 0;
	}
	if (nEnd > nBurnSoundLen) {
		nEnd = nBurnSoundLen;
	}

	// Render FM
	BurnYM2151Render(pBurnSoundOut + (nPos << 1), nEnd - nPos);

	// Render ADPCM
	MSM6295Render(0, pBurnSoundOut + (nPos << 1), nEnd - nPos);

	nPos = nEnd;

	return 0;
}
