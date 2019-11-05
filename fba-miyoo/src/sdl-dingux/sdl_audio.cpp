/*
 * FinalBurn Alpha for Dingux/OpenDingux
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "version.h"
#include "burner.h"
#include "sdl_audio.h"

// ============================================================================
// SDL backend
SDL_mutex *sound_mutex;
SDL_cond *sound_cv;

// sdl-related buffers
static int BUFFSIZE;
static unsigned char *buffer;
static unsigned int buf_read_pos = 0;
static unsigned int buf_write_pos = 0;

static int buffered_bytes=0;

static int sdl_write_buffer_m(unsigned char* data, int len)
{
	SDL_LockMutex(sound_mutex);
#if 0
	while(BUFFSIZE - buffered_bytes < len) SDL_CondWait(sound_cv, sound_mutex);

	if(buf_write_pos + len <= BUFFSIZE ) {
		memcpy(buffer + buf_write_pos, data, len);
	} else {
		int tail = BUFFSIZE - buf_write_pos;
		memcpy(buffer + buf_write_pos, data, tail);
		memcpy(buffer, data + tail, len - tail);
	}
	buf_write_pos = (buf_write_pos + len) % BUFFSIZE;
	buffered_bytes += len;
#else
	for(int i = 0; i < len; i += 4) {
		while(buffered_bytes == BUFFSIZE) SDL_CondWait(sound_cv, sound_mutex);

		*(int*)((char*)(buffer + buf_write_pos)) = *(int*)((char*)(data + i));
		//memcpy(buffer + buf_write_pos, data + i, 4);
		buf_write_pos = (buf_write_pos + 4) % BUFFSIZE;
		buffered_bytes += 4;
	}
#endif
	SDL_CondSignal(sound_cv);
	SDL_UnlockMutex(sound_mutex);
	return len;
}

static int sdl_write_buffer(unsigned char* data, int len)
{
	for(int i = 0; i < len; i += 4) {
		if(buffered_bytes == BUFFSIZE) return len; // just drop samples

		*(int*)((char*)(buffer + buf_write_pos)) = *(int*)((char*)(data + i));
		//memcpy(buffer + buf_write_pos, data + i, 4);
		buf_write_pos = (buf_write_pos + 4) % BUFFSIZE;
		buffered_bytes += 4;
	}

	return len;
}

static int sdl_read_buffer(unsigned char* data,int len)
{
	if(buffered_bytes >= len) {
		if(buf_read_pos + len <= BUFFSIZE ) {
			memcpy(data, buffer + buf_read_pos, len);
		} else {
			int tail = BUFFSIZE - buf_read_pos;
			memcpy(data, buffer + buf_read_pos, tail);
			memcpy(data + tail, buffer, len - tail);
		}
		buf_read_pos = (buf_read_pos + len) % BUFFSIZE;
		buffered_bytes -= len;
	}

	return len;
}

void sdl_callback_m(void *unused, Uint8 *stream, int len)
{
	SDL_LockMutex(sound_mutex);

	sdl_read_buffer((unsigned char *)stream, len);

	SDL_CondSignal(sound_cv);
	SDL_UnlockMutex(sound_mutex);
}

void sdl_callback(void *unused, Uint8 *stream, int len)
{
	sdl_read_buffer((unsigned char *)stream, len);
}


static inline int sdl_calc_samples(int rate)
{
	int s;

	// Find the value which is slighly bigger than nBurnSoundLen*2
	for(s = 512; s < (nBurnSoundLen * 2); s <<= 1); 

	return s;
}

static int sdl_open_common(int rate, int channels, int format, int mutex)
{
	int SAMPLESIZE;
	SDL_AudioSpec aspec, obtained;

	SAMPLESIZE = sdl_calc_samples(rate);
	BUFFSIZE = SAMPLESIZE * channels * 2 * 8;
	buffer = (unsigned char *) malloc(BUFFSIZE);

	// add some silence to the buffer
	buffered_bytes = 0;
	buf_read_pos = 0;
	buf_write_pos = 0;

	aspec.format   = AUDIO_S16;
	aspec.freq     = rate;
	aspec.channels = channels;
	aspec.samples  = SAMPLESIZE;
	aspec.callback = (mutex ? sdl_callback_m : sdl_callback);
	aspec.userdata = NULL;

	/* initialize the SDL Audio system */
	if (SDL_InitSubSystem (SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE)) {
		printf("SDL: Initializing of SDL Audio failed: %s.\n", SDL_GetError());
		return 0;
	}

	/* Open the audio device and start playing sound! */
	if(SDL_OpenAudio(&aspec, &obtained) < 0) {
			printf("SDL: Unable to open audio: %s\n", SDL_GetError());
			return(0);
	}

	if(mutex) {
		sound_mutex = SDL_CreateMutex();
		sound_cv = SDL_CreateCond();
		printf("Using mutexes for synchro\n");
	}

	printf("audio frequency %d\n", obtained.freq);
	printf("audio samples %d\n", obtained.samples);
	printf("audio channels %d\n", obtained.channels);

	SDL_PauseAudio(0);

	return 1;
}

int sdl_open_m(int rate, int channels, int format)
{
	return sdl_open_common(rate, channels, format, 1);
}

int sdl_open(int rate, int channels, int format)
{
	return sdl_open_common(rate, channels, format, 0);
}


// close audio device
static void sdl_close_common(int mutex)
{
	SDL_PauseAudio(1);

	if(mutex) {
		SDL_LockMutex(sound_mutex);
		buffered_bytes = BUFFSIZE;
		SDL_CondSignal(sound_cv);
		SDL_UnlockMutex(sound_mutex);
		SDL_Delay(100);

		SDL_DestroyCond(sound_cv);
		SDL_DestroyMutex(sound_mutex);
	}
	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
	free(buffer);
	buffer = NULL;
}

void sdl_close_m(void)
{
	sdl_close_common(1);
}

void sdl_close(void)
{
	sdl_close_common(0);
}

void sdl_pause(int flag)
{
	SDL_PauseAudio(flag);
}

int sdl_play_m(unsigned char *data, int len)
{
	return sdl_write_buffer_m(data, len);
}

int sdl_play(unsigned char *data, int len)
{
	int result;

	SDL_LockAudio();
	result = sdl_write_buffer(data, len);
	SDL_UnlockAudio();

	return result;
}
