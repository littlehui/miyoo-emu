
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "version.h"
#include "burner.h"
#include "snd.h"
#include "sdl_audio.h"
#include "ao_audio.h"

extern bool bPauseOn;
extern int nBurnFPS;

// mimic api from aud_interface.cpp
// Aud == Snd

// native buffers, size depends on emulated frame (rate/fps)
int nSndSampleRate[5] = {11025, 16000, 22050, 32000, 44100};  // Sample rate
static int nSndChannels = 2;                    // Always stereo
short *nSndSegBuffer = NULL;                    // Buffer where Burn driver will write snd data
//int nSndSegCount = 6;                           // Segs in the ring buffer
//int nSndSegLen = 0;                             // Seg length in samples (calculated from Rate/Fps)
int nSndSegSize = 0;                            // Seg size in bytes (calculated from Rate/Fps)
int bSndOkay = 0;

// Dummy driver
int  dummy_open(int rate, int channels, int format) { return 0; }
void dummy_close(void) { }
int  dummy_play(unsigned char *data, int len) { return 0; }
void dummy_pause(int flag) { }

typedef struct {
	int  (*open)(int rate, int channels, int format);
	void (*close)(void);
	int  (*play)(unsigned char *data, int len);
	void (*pause)(int flag);
} SNDDRV;

SNDDRV SndDrvList[] = 
{
	{	// No sound
		&dummy_open, &dummy_close, &dummy_play, &dummy_pause
	},
	{	// libao
#ifdef USE_LIBAO
		&libao_open, &libao_close, &libao_play, &libao_pause
#else
		&dummy_open, &dummy_close, &dummy_play, &dummy_pause
#endif
	},
	{	// SDL with mutexes
		&sdl_open_m, &sdl_close_m, &sdl_play_m, &sdl_pause
	},
	{	// SDL with SDL_LockAudio
		&sdl_open, &sdl_close, &sdl_play, &sdl_pause
	}

};

SNDDRV *pSndDrv = &SndDrvList[0];

// General code for sound
int SndInit()
{
	if(bSndOkay) SndExit();

	if(options.sound > 3)
		options.sound = 0;

	pSndDrv = &SndDrvList[options.sound];

	if (options.sound) {
		int i = options.samplerate;

		if(i > 4) i = 0;

		nBurnSoundRate = nSndSampleRate[i];
		nBurnSoundLen = ((nBurnSoundRate * 100) / nBurnFPS );
	}

	pBurnSoundOut = NULL;

	nSndSegSize = nBurnSoundLen * nSndChannels * 2;
	nSndSegBuffer = (short *)malloc(nSndSegSize);
	memset(nSndSegBuffer,0,nSndSegSize);

	printf("nBurnSoundLen: %i samples\n", nBurnSoundLen);

	bSndOkay = (*pSndDrv->open)(nBurnSoundRate, nSndChannels, 16);

	if(bSndOkay) {
		pBurnSoundOut  = (short *)nSndSegBuffer;
		return 1;
	} else {
		pBurnSoundOut = NULL;
		nBurnSoundRate = 0;
		nBurnSoundLen = 0;
	}

	options.sound = 0;
	pSndDrv = &SndDrvList[0]; // null sound
	return 0;
}

void SndPause(int flag)
{
	(*pSndDrv->pause)(flag);
}

void SndExit()
{
	if(bSndOkay) {
		(*pSndDrv->close)();
		bSndOkay = 0;
	}

	pBurnSoundOut = NULL;
	nBurnSoundRate = 0;
	nBurnSoundLen = 0;
	if(nSndSegBuffer) {
		free(nSndSegBuffer);
		nSndSegBuffer = NULL;
	}
}

void SndPlay()
{
	if(!bPauseOn) {
		(*pSndDrv->play)((unsigned char *)nSndSegBuffer, nSndSegSize);
	}
}