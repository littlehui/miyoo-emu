#include "burnint.h"
#include "burn_sound.h"
#include "burn_ym2608.h"

void (*BurnYM2608Update)(short* pSoundBuf, int nSegmentEnd);

static int (*BurnYM2608StreamCallback)(int nSoundRate);

static int nBurnYM2608SoundRate;

static short* pBuffer;
static short* pYM2608Buffer[6];

static int* pAYBuffer;

static int nYM2608Position;
static int nAY8910Position;

static unsigned int nSampleSize;
static unsigned int nFractionalPosition;

static int bYM2208AddSignal;

// ----------------------------------------------------------------------------
// Dummy functions

static void YM2608UpdateDummy(short*, int /* nSegmentEnd */)
{
	return;
}

static int YM2608StreamCallbackDummy(int /* nSoundRate */)
{
	return 0;
}

// ----------------------------------------------------------------------------
// Execute YM2608 for part of a frame

static void AY8910Render(int nSegmentLength)
{
	if (nAY8910Position >= nSegmentLength) {
		return;
	}

//	bprintf(PRINT_NORMAL, _T("    AY8910 render %6i -> %6i\n"), nAY8910Position, nSegmentLength);

	nSegmentLength -= nAY8910Position;

	pYM2608Buffer[2] = pBuffer + 2 * 4096 + 4 + nAY8910Position;
	pYM2608Buffer[3] = pBuffer + 3 * 4096 + 4 + nAY8910Position;
	pYM2608Buffer[4] = pBuffer + 4 * 4096 + 4 + nAY8910Position;

	AY8910Update(0, &pYM2608Buffer[2], nSegmentLength);

	nAY8910Position += nSegmentLength;
}

static void YM2608Render(int nSegmentLength)
{
	if (nYM2608Position >= nSegmentLength) {
		return;
	}

//	bprintf(PRINT_NORMAL, _T("    YM2608 render %6i -> %6i\n", nYM2608Position, nSegmentLength));

	nSegmentLength -= nYM2608Position;

	pYM2608Buffer[0] = pBuffer + 0 * 4096 + 4 + nYM2608Position;
	pYM2608Buffer[1] = pBuffer + 1 * 4096 + 4 + nYM2608Position;

	YM2608UpdateOne(0, &pYM2608Buffer[0], nSegmentLength);

	nYM2608Position += nSegmentLength;
}

// ----------------------------------------------------------------------------
// Update the sound buffer

static void YM2608UpdateResample(short* pSoundBuf, int nSegmentEnd)
{
	int nSegmentLength = nSegmentEnd;
	int nSamplesNeeded = nSegmentEnd * nBurnYM2608SoundRate / nBurnSoundRate + 1;

//	bprintf(PRINT_NORMAL, _T("    YM2608 update        -> %6i\n"), nSegmentLength);

	if (nSamplesNeeded < nAY8910Position) {
		nSamplesNeeded = nAY8910Position;
	}
	if (nSamplesNeeded < nYM2608Position) {
		nSamplesNeeded = nYM2608Position;
	}

	if (nSegmentLength > nBurnSoundLen) {
		nSegmentLength = nBurnSoundLen;
	}
	nSegmentLength <<= 1;

	YM2608Render(nSamplesNeeded);
	AY8910Render(nSamplesNeeded);

	pYM2608Buffer[0] = pBuffer + 0 * 4096 + 4;
	pYM2608Buffer[1] = pBuffer + 1 * 4096 + 4;
	pYM2608Buffer[2] = pBuffer + 2 * 4096 + 4;
	pYM2608Buffer[3] = pBuffer + 3 * 4096 + 4;
	pYM2608Buffer[4] = pBuffer + 4 * 4096 + 4;
	pYM2608Buffer[5] = pBuffer + 5 * 4096 + 4;

	for (int i = (nFractionalPosition >> 16) - 4; i < nSamplesNeeded; i++) {
		pYM2608Buffer[5][i] = (((int)pYM2608Buffer[2][i] + pYM2608Buffer[3][i] + pYM2608Buffer[4][i]) * (4096 * 60 / 100)) >> 12;
	}

	for (int i = (nFractionalPosition & 0xFFFF0000) >> 15; i < nSegmentLength; i += 2, nFractionalPosition += nSampleSize) {
		int nSample;

#define CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))

		// Left channel
		nSample = INTERPOLATE4PS_16BIT((nFractionalPosition >> 4) & 0x0FFF,
									   pYM2608Buffer[0][(nFractionalPosition >> 16) - 3] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 3],
									   pYM2608Buffer[0][(nFractionalPosition >> 16) - 2] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 2],
									   pYM2608Buffer[0][(nFractionalPosition >> 16) - 1] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 1],
									   pYM2608Buffer[0][(nFractionalPosition >> 16) - 0] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 0]);
		if (bYM2208AddSignal) {
			pSoundBuf[i + 0] += CLIP(nSample);
		} else {
			pSoundBuf[i + 0] = CLIP(nSample);
		}

		// Right channel
		nSample = INTERPOLATE4PS_16BIT((nFractionalPosition >> 4) & 0x0FFF,
									   pYM2608Buffer[1][(nFractionalPosition >> 16) - 3] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 3],
									   pYM2608Buffer[1][(nFractionalPosition >> 16) - 2] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 2],
									   pYM2608Buffer[1][(nFractionalPosition >> 16) - 1] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 1],
									   pYM2608Buffer[1][(nFractionalPosition >> 16) - 0] + pYM2608Buffer[5][(nFractionalPosition >> 16) - 0]);
		if (bYM2208AddSignal) {
			pSoundBuf[i + 1] += CLIP(nSample);
		} else {
			pSoundBuf[i + 1] = CLIP(nSample);
		}

#undef CLIP

	}

	if (nSegmentEnd >= nBurnSoundLen) {
		int nExtraSamples = nSamplesNeeded - (nFractionalPosition >> 16);

//		bprintf(PRINT_NORMAL, _T("   %6i rendered, %i extra, %i <- %i\n"), nSamplesNeeded, nExtraSamples, nExtraSamples, (nFractionalPosition >> 16) + nExtraSamples - 1);

		for (int i = -4; i < nExtraSamples; i++) {
			pYM2608Buffer[0][i] = pYM2608Buffer[0][(nFractionalPosition >> 16) + i];
			pYM2608Buffer[1][i] = pYM2608Buffer[1][(nFractionalPosition >> 16) + i];
			pYM2608Buffer[2][i] = pYM2608Buffer[2][(nFractionalPosition >> 16) + i];
			pYM2608Buffer[3][i] = pYM2608Buffer[3][(nFractionalPosition >> 16) + i];
			pYM2608Buffer[4][i] = pYM2608Buffer[4][(nFractionalPosition >> 16) + i];
		}

		nFractionalPosition &= 0xFFFF;

		nYM2608Position = nExtraSamples;
		nAY8910Position = nExtraSamples;

		dTime += 100.0 / nBurnFPS;
	}
}

static void YM2608UpdateNormal(short* pSoundBuf, int nSegmentEnd)
{
	int nSegmentLength = nSegmentEnd;

//	bprintf(PRINT_NORMAL, _T("    YM2608 update        -> %6i\n", nSegmentLength));

	if (nSegmentEnd < nAY8910Position) {
		nSegmentEnd = nAY8910Position;
	}
	if (nSegmentEnd < nYM2608Position) {
		nSegmentEnd = nYM2608Position;
	}

	if (nSegmentLength > nBurnSoundLen) {
		nSegmentLength = nBurnSoundLen;
	}

	YM2608Render(nSegmentEnd);
	AY8910Render(nSegmentEnd);

	pYM2608Buffer[0] = pBuffer + 4 + 0 * 4096;
	pYM2608Buffer[1] = pBuffer + 4 + 1 * 4096;
	pYM2608Buffer[2] = pBuffer + 4 + 2 * 4096;
	pYM2608Buffer[3] = pBuffer + 4 + 3 * 4096;
	pYM2608Buffer[4] = pBuffer + 4 + 4 * 4096;

#ifdef BUILD_X86_ASM
	if (bBurnUseMMX) {
		for (int n = nFractionalPosition; n < nSegmentLength; n++) {
			pAYBuffer[n] = pYM2608Buffer[2][n] + pYM2608Buffer[3][n] + pYM2608Buffer[4][n];
		}
		BurnSoundCopy_FM_OPN_A(pYM2608Buffer[0], pAYBuffer, pSoundBuf, nSegmentLength, 65536 * 60 / 100, 65536 * 60 / 100);
	} else {
#endif
		for (int n = nFractionalPosition; n < nSegmentLength; n++) {
			int nAYSample, nTotalSample;

			nAYSample  = pYM2608Buffer[2][n];
			nAYSample += pYM2608Buffer[3][n];
			nAYSample += pYM2608Buffer[4][n];

			nAYSample  *= 4096 * 60 / 100;
			nAYSample >>= 12;

			nTotalSample = nAYSample + pYM2608Buffer[0][n];
			if (nTotalSample < -32768) {
				nTotalSample = -32768;
			} else {
				if (nTotalSample > 32767) {
					nTotalSample = 32767;
				}
			}
			
			if (bYM2208AddSignal) {
				pSoundBuf[(n << 1) + 0] += nTotalSample;
			} else {
				pSoundBuf[(n << 1) + 0] = nTotalSample;
			}

			nTotalSample = nAYSample + pYM2608Buffer[1][n];
			if (nTotalSample < -32768) {
				nTotalSample = -32768;
			} else {
				if (nTotalSample > 32767) {
					nTotalSample = 32767;
				}
			}
			
			if (bYM2208AddSignal) {
				pSoundBuf[(n << 1) + 1] += nTotalSample;
			} else {
				pSoundBuf[(n << 1) + 1] = nTotalSample;
			}
		}
#ifdef BUILD_X86_ASM
	}
#endif

	nFractionalPosition = nSegmentLength;

	if (nSegmentEnd >= nBurnSoundLen) {
		int nExtraSamples = nSegmentEnd - nBurnSoundLen;

		for (int i = 0; i < nExtraSamples; i++) {
			pYM2608Buffer[0][i] = pYM2608Buffer[0][nBurnSoundLen + i];
			pYM2608Buffer[1][i] = pYM2608Buffer[1][nBurnSoundLen + i];
			pYM2608Buffer[2][i] = pYM2608Buffer[2][nBurnSoundLen + i];
			pYM2608Buffer[3][i] = pYM2608Buffer[3][nBurnSoundLen + i];
			pYM2608Buffer[4][i] = pYM2608Buffer[4][nBurnSoundLen + i];
		}

		nFractionalPosition = 0;

		nYM2608Position = nExtraSamples;
		nAY8910Position = nExtraSamples;

		dTime += 100.0 / nBurnFPS;
	}
}

// ----------------------------------------------------------------------------
// Callbacks for YM2608 core

void BurnYM2608UpdateRequest()
{
	YM2608Render(BurnYM2608StreamCallback(nBurnYM2608SoundRate));
}

static void BurnAY8910UpdateRequest()
{
	AY8910Render(BurnYM2608StreamCallback(nBurnYM2608SoundRate));
}

// ----------------------------------------------------------------------------
// Initialisation, etc.

void BurnYM2608Reset()
{
	BurnTimerReset();

	YM2608ResetChip(0);
}

void BurnYM2608Exit()
{
	YM2608Shutdown();
	AY8910Exit(0);

	BurnTimerExit();

	free(pBuffer);
	free(pAYBuffer);
	
	bYM2208AddSignal = 0;
}

int BurnYM2608Init(int nClockFrequency, unsigned char* YM2608ADPCMROM, int* nYM2608ADPCMSize, FM_IRQHANDLER IRQCallback, int (*StreamCallback)(int), double (*GetTimeCallback)(), int bAddSignal)
{
	BurnTimerInit(&YM2608TimerOver, GetTimeCallback);

	if (nBurnSoundRate <= 0) {
		BurnYM2608StreamCallback = YM2608StreamCallbackDummy;

		BurnYM2608Update = YM2608UpdateDummy;

		AY8910InitYM(0, nClockFrequency, 11025, NULL, NULL, NULL, NULL, BurnAY8910UpdateRequest);
		YM2608Init(1, nClockFrequency, 11025, (void**)(&YM2608ADPCMROM), nYM2608ADPCMSize, &BurnOPNTimerCallback, IRQCallback);
		return 0;
	}

	BurnYM2608StreamCallback = StreamCallback;

	if (nFMInterpolation == 3) {
		// Set YM2608 core samplerate to match the hardware
		nBurnYM2608SoundRate = nClockFrequency / 144;
		// Bring YM2608 core samplerate within usable range
		while (nBurnYM2608SoundRate > nBurnSoundRate * 3) {
			nBurnYM2608SoundRate >>= 1;
		}

		BurnYM2608Update = YM2608UpdateResample;

		nSampleSize = (unsigned int)nBurnYM2608SoundRate * (1 << 16) / nBurnSoundRate;
		nFractionalPosition = 0;
	} else {
		nBurnYM2608SoundRate = nBurnSoundRate;

		BurnYM2608Update = YM2608UpdateNormal;
	}

	AY8910InitYM(0, nClockFrequency, nBurnYM2608SoundRate, NULL, NULL, NULL, NULL, BurnAY8910UpdateRequest);
	YM2608Init(1, nClockFrequency, nBurnYM2608SoundRate, (void**)(&YM2608ADPCMROM), nYM2608ADPCMSize, &BurnOPNTimerCallback, IRQCallback);

	pBuffer = (short*)malloc(4096 * 6 * sizeof(short));
	memset(pBuffer, 0, 4096 * 6 * sizeof(short));

	pAYBuffer = (int*)malloc(4096 * sizeof(int));
	memset(pAYBuffer, 0, 4096 * sizeof(int));

	nYM2608Position = 0;
	nAY8910Position = 0;
	
	bYM2208AddSignal = bAddSignal;

	return 0;
}

void BurnYM2608Scan(int nAction, int* pnMin)
{
	BurnTimerScan(nAction, pnMin);
	AY8910Scan(nAction, pnMin);

	if (nAction & ACB_DRIVER_DATA) {
		SCAN_VAR(nYM2608Position);
		SCAN_VAR(nAY8910Position);
	}
}
