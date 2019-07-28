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

#include <stdio.h>
#include <SDL/SDL.h>

#include "burner.h"
#include "sdl_run.h"
#include "sdl_video.h"
#include "sdl_input.h"

#define COLORMIX(a, b) ( ((((a & 0xF81F) + (b & 0xF81F)) >> 1) & 0xF81F) | ((((a & 0x07E0) + (b & 0x07E0)) >> 1) & 0x07E0) )

int VideoBufferWidth = 0;
int VideoBufferHeight = 0;
int PhysicalBufferWidth = 0;

unsigned short *BurnVideoBuffer = NULL; // source FBA video buffer
unsigned short *VideoBuffer = NULL; // screen buffer

SDL_Surface *screen;

// --------------------------------

static unsigned int myHighCol16(int r, int g, int b, int /* i */)
{
	unsigned int t;
	t =(r<<8)&0xf800; // rrrr r000 0000 0000
	t|=(g<<3)&0x07e0; // 0000 0ggg ggg0 0000
	t|=(b>>3)&0x001f; // 0000 0000 000b bbbb
	return t;
}

static void Blit_null() {}

static void (*BurnerVideoTrans)() = Blit_null;

static void Blit_864x224_to_320x240() 
{
	// Taito 864x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++)
		for (int j=0; j<32; j++) {
			p[0] = COLORMIX(q[0], q[1]); // 0, 1, 2
			p[1] = COLORMIX(q[3], q[4]); // 3, 4, 5
			p[2] = COLORMIX(q[6], q[7]); // 6, 7, 8
			p[3] = COLORMIX(q[9], q[10]); // 9, 10, 11
			p[4] = COLORMIX(q[12], q[13]); // 12, 13
			p[5] = COLORMIX(q[14], q[15]); // 14, 15, 16
			p[6] = COLORMIX(q[17], q[18]); // 17, 18, 19
			p[7] = COLORMIX(q[20], q[21]); // 20, 21, 22
			p[8] = COLORMIX(q[23], q[24]); // 23, 24, 25
			p[9] = COLORMIX(q[26], q[27]); // 26, 27
			p += 10;
			q += 27;
		}
}

static void Blit_640x240_to_320x240() 
{
	// Taito 640x240
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<240; i++)
		for (int j=0; j<320; j++) {
			p[0] = COLORMIX(q[0], q[1]);
			p += 1;
			q += 2;
		}
}

static void Blit_640x224_to_320x240() 
{
	// Taito 640x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++)
		for (int j=0; j<320; j++) {
			p[0] = COLORMIX(q[0], q[1]);
			p += 1;
			q += 2;
		}
}

static void Blit_512x256_to_320x240() 
{
	// Konami 512x256
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<240; i++) {
		for (int j=0; j<64; j++) {
			p[0] = COLORMIX(q[0], q[1]);
			p[1] = q[2];
			p[2] = COLORMIX(q[3], q[4]);
			p[3] = q[5];
			p[4] = COLORMIX(q[6], q[7]);
			p += 5;
			q += 8;
		}
		if(i % 16 == 0) q += 512;
	}
}

static void Blitf_512x256_to_320x240() 
{
	// Konami 512x256
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer + 512 * 256 - 1 - 8;
	
	for (int i=0; i<240; i++) {
		for (int j=0; j<64; j++) {
			p[0] = COLORMIX(q[7], q[6]);
			p[1] = q[5];
			p[2] = COLORMIX(q[4], q[3]);
			p[3] = q[2];
			p[4] = COLORMIX(q[1], q[0]);
			p += 5;
			q -= 8;
		}
		if(i % 16 == 0) q -= 512;
	}
}

static void Blit_512x224_to_320x240() 
{
	// galpinbl, hotpinbl 512x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++) {
		for (int j=0; j<64; j++) {
			p[0] = COLORMIX(q[0], q[1]);
			p[1] = q[2];
			p[2] = COLORMIX(q[3], q[4]);
			p[3] = q[5];
			p[4] = COLORMIX(q[6], q[7]);
			p += 5;
			q += 8;
		}
	}
}

static void Blitf_512x224_to_320x240() 
{
	// galpinbl, hotpinbl 512x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer + 512 * 224 - 1 - 8;
	
	for (int i=0; i<224; i++) {
		for (int j=0; j<64; j++) {
			p[0] = COLORMIX(q[7], q[6]);
			p[1] = q[5];
			p[2] = COLORMIX(q[4], q[3]);
			p[3] = q[2];
			p[4] = COLORMIX(q[1], q[0]);
			p += 5;
			q -= 8;
		}
	}
}

static void Blit_448x224_to_320x240() 
{
	// IGS 448x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++)
		for (int j=0; j<64; j++) {
			p[0] = q[0];
			p[1] = COLORMIX(q[1], q[2]);
			p[2] = q[3];
			p[3] = COLORMIX(q[4], q[5]);
			p[4] = q[6];
			p += 5;
			q += 7;
		}
}

static void Blit_384x256_to_320x240()
{
	// Irem 384x256
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer + 384*8;
	
	for (int i=0; i<240; i++)
		for (int j=0; j<64; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = COLORMIX(q[2],q[3]);
			p[3] = q[4];
			p[4] = q[5];
			p += 5;
			q += 6;
		}
}

static void Blit_384x240_to_320x240() 
{
	// 384x240
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<240; i++)
		for (int j=0; j<64; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = COLORMIX(q[2],q[3]);
			p[3] = q[4];
			p[4] = q[5];
			p += 5;
			q += 6;
		}
}

static void Blitf_384x240_to_320x240() 
{
	// 384x240
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer + 384 * 240 - 1 - 6;
	
	for (int i=0; i<240; i++)
		for (int j=0; j<64; j++) {
			p[0] = q[5];
			p[1] = q[4];
			p[2] = COLORMIX(q[3],q[2]);
			p[3] = q[1];
			p[4] = q[0];
			p += 5;
			q -= 6;
		}
}

static void Blit_384x224_to_320x240() 
{
	// CPS1 & CPS2 384x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++)
		for (int j=0; j<64; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = COLORMIX(q[2],q[3]);
			p[3] = q[4];
			p[4] = q[5];
			p += 5;
			q += 6;
		}
}

static void Blitf_384x224_to_320x240() 
{
	// CPS1 & CPS2 384x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer + 384 * 224 - 1 - 6;
	
	for (int i=0; i<224; i++)
		for (int j=0; j<64; j++) {
			p[0] = q[5];
			p[1] = q[4];
			p[2] = COLORMIX(q[3],q[2]);
			p[3] = q[1];
			p[4] = q[0];
			p += 5;
			q -= 6;
		}
}

static void Blit_380x224_to_320x240() 
{
	// silkroad 380x224
	unsigned short * p = &VideoBuffer[2560+2];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++) {
		for (int j=0; j<63; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = COLORMIX(q[2],q[3]);
			p[3] = q[4];
			p[4] = q[5];
			p += 5;
			q += 6;
		}
		p[0] = q[0];
		p[1] = q[1];
		p += 5;
		q += 2;
	}
}

static void Blit_376x240_to_320x240() 
{
	// myangel2 376x240
	unsigned short * p = &VideoBuffer[3];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<240; i++) {
		for (int j=0; j<62; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = COLORMIX(q[2],q[3]);
			p[3] = q[4];
			p[4] = q[5];
			p += 5;
			q += 6;
		}
		p[0] = q[0];
		p[1] = q[1];
		p[2] = q[2];
		p[3] = q[3];
		p += 10;
		q += 4;
	}
}

static void Blit_368x224_to_320x240() 
{
	// zerozone 368x224
	unsigned short * p = &VideoBuffer[2560];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++)
		for (int j=0; j<16; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = q[2];
			p[3] = q[3];
			p[4] = COLORMIX(q[4], q[5]);
			p[5] = q[6];
			p[6] = q[7];
			p[7] = q[8];
			p[8] = q[9];
			p[9] = q[10];
			p[10] = COLORMIX(q[11], q[12]);
			p[11] = q[13];
			p[12] = q[14];
			p[13] = q[15];
			p[14] = q[16];
			p[15] = COLORMIX(q[17], q[18]);
			p[16] = q[19];
			p[17] = q[20];
			p[18] = q[21];
			p[19] = q[22];
			p += 20;
			q += 23;
		}
}

static void Blit_352x240_to_320x240() 
{
	// V-System 352x240
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<240; i++)
		for (int j=0; j<32; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = q[2];
			p[3] = q[3];
			p[4] = q[4];
			p[5] = q[5];
			p[6] = q[6];
			p[7] = q[7];
			p[8] = q[8];
			p[9] = COLORMIX(q[9], q[10]);
			p += 10;
			q += 11;
		}
}

static void Blit_336x240_to_320x240() 
{
	// Atari 336x240
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<240; i++)
		for (int j=0; j<16; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = q[2];
			p[3] = q[3];
			p[4] = q[4];
			p[5] = q[5];
			p[6] = q[6];
			p[7] = q[7];
			p[8] = q[8];
			p[9] = COLORMIX(q[9], q[10]);
			p[10] = q[11];
			p[11] = q[12];
			p[12] = q[13];
			p[13] = q[14];
			p[14] = q[15];
			p[15] = q[16];
			p[16] = q[17];
			p[17] = q[18];
			p[18] = q[19];
			p[19] = q[20];
			p += 20;
			q += 21;
		}
}

static void Blit_320x240_to_320x240() 
{
	// Cave & Toaplan 320x240
	unsigned short * p = &VideoBuffer[0];
	unsigned short * q = &BurnVideoBuffer[0];
	memcpy( p, q, 320 * 240 * 2 );
}

static void Blit_256x256_to_320x240() 
{
	// 256x256
	unsigned short * p = &VideoBuffer[(320-256)/2];
	unsigned short * q = &BurnVideoBuffer[0];
	for (int i = 0; i < 240; i++) {
		memcpy( p, q, 256 * 2 );
		p += 320;
		q += 256;
		if(i % 16 == 0) q += 256;
	}
}

static void Blitf_256x256_to_320x240() 
{
	// 256x256
	unsigned short *p = &VideoBuffer[(320-256)/2];
	unsigned short *q = &BurnVideoBuffer[256 * 256 - 1];
	for (int i = 0; i < 240; i++) {
		for(int j = 0; j < 256; j++) {
			*p++ = *q--;
		}
		p += 64;
		if(i % 16 == 0) q -= 256;
	}
}

static void Blit_224x256_to_320x240() 
{
	// 224x256
	unsigned short * p = &VideoBuffer[(320-224)/2];
	unsigned short * q = &BurnVideoBuffer[0];
	for (int i = 0; i < 240; i++) {
		memcpy( p, q, 224 * 2 );
		p += 320;
		q += 224;
		if(i % 16 == 0) q += 224;
	}
}

static void Blit_256x256_to_400x240() 
{
	// 256x256
	unsigned short * p = &VideoBuffer[(400-256)/2];
	unsigned short * q = &BurnVideoBuffer[0];
	for (int i = 0; i < 240; i++) {
		memcpy( p, q, 256 * 2 );
		p += 400;
		q += 256;
		if(i % 16 == 0) q += 256; 
	}
}

static void Blit_224x256_to_400x240() 
{
	// 224x256
	unsigned short * p = &VideoBuffer[(400-224)/2];
	unsigned short * q = &BurnVideoBuffer[0];
	for (int i = 0; i < 240; i++) {
		memcpy( p, q, 224 * 2 );
		p += 400;
		q += 224;
		if(i % 16 == 0) q += 224;
	}
}

static void Blit_448x224_to_400x240()
{
	// IGS 448x224
	unsigned short * p = &VideoBuffer[8*400+4];
	unsigned short * q = BurnVideoBuffer;
	
	for (int i=0; i<224; i++, p += 8)
		for (int j=0; j<56; j++) {
			p[0] = q[0];
			p[1] = q[1];
			p[2] = q[2];
			p[3] = COLORMIX(q[3], q[4]);
			p[4] = q[5];
			p[5] = q[6];
			p[6] = q[7];
			p += 7;
			q += 8;
		}
}

static unsigned int p_offset = 0, q_offset = 0;

static void Blit()
{
	register unsigned short *p = &VideoBuffer[p_offset];
	register unsigned short *q = &BurnVideoBuffer[0];
	for(int y = VideoBufferHeight; y--;) {
		for(int x = VideoBufferWidth; x--;)
			*p++ = *q++;
		p += screen->w - VideoBufferWidth;
	}
}

static void Blitf()
{
	register unsigned short *p = &VideoBuffer[p_offset];
	register unsigned short *q = &BurnVideoBuffer[q_offset];
	for(int y = VideoBufferHeight; y--;) {
		for(int x = VideoBufferWidth; x--;)
			*p++ = *q--;
		p += screen->w - VideoBufferWidth;
	}
}

typedef struct
{
	int dst_w;
	int dst_h;
	int src_w;
	int src_h;
	void (*blit)();
	void (*blitf)();
} BLIT_TABLE;

BLIT_TABLE blit_table[] = {
	{320, 240, 864, 224, Blit_864x224_to_320x240, Blit_864x224_to_320x240}, // Taito (darius)
	{320, 240, 640, 240, Blit_640x240_to_320x240, Blit_640x240_to_320x240}, // Taito (warriorb)
	{320, 240, 640, 224, Blit_640x224_to_320x240, Blit_640x224_to_320x240}, // Taito (darius2d)
	{320, 240, 512, 256, Blit_512x256_to_320x240, Blitf_512x256_to_320x240}, // Konami (hexion)
	{320, 240, 512, 224, Blit_512x224_to_320x240, Blitf_512x224_to_320x240}, // hotpinbl
	{320, 240, 448, 224, Blit_448x224_to_320x240, Blit_448x224_to_320x240}, // IGS (PGM)
	{320, 240, 384, 256, Blit_384x256_to_320x240, Blit_384x256_to_320x240}, // Irem
	{320, 240, 384, 240, Blit_384x240_to_320x240, Blitf_384x240_to_320x240}, // Cave
	{320, 240, 384, 224, Blit_384x224_to_320x240, Blitf_384x224_to_320x240}, // CPS1 & CPS2
	{320, 240, 380, 224, Blit_380x224_to_320x240, Blit_380x224_to_320x240}, // silkroad
	{320, 240, 376, 240, Blit_376x240_to_320x240, Blit_376x240_to_320x240}, // myangel2
	{320, 240, 368, 224, Blit_368x224_to_320x240, Blit_368x224_to_320x240}, // zerozone
	{320, 240, 352, 240, Blit_352x240_to_320x240, Blit_352x240_to_320x240}, // V-System
	{320, 240, 336, 240, Blit_336x240_to_320x240, Blit_336x240_to_320x240}, // Atari
	{320, 240, 320, 240, Blit_320x240_to_320x240, Blit_320x240_to_320x240}, // Cave & Toaplan
	{320, 240, 256, 256, Blit_256x256_to_320x240, Blitf_256x256_to_320x240},
	{320, 240, 224, 256, Blit_224x256_to_320x240, Blit_224x256_to_320x240},
	{400, 240, 256, 256, Blit_256x256_to_400x240, Blit_256x256_to_400x240},
	{400, 240, 224, 256, Blit_224x256_to_400x240, Blit_224x256_to_400x240},
	{400, 240, 448, 224, Blit_448x224_to_400x240, Blit_448x224_to_400x240}, // IGS (PGM)
	{  0,   0,   0,   0, NULL, NULL}
};

void VideoTrans()
{
	VideoBuffer = (unsigned short*)screen->pixels;
	if(SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
	BurnerVideoTrans();
	if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
}

int VideoInit()
{
	// Initialize SDL
	int flags = (options.vsync ? SDL_HWSURFACE | SDL_DOUBLEBUF : SDL_SWSURFACE);

	if(!(SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO)) {
		SDL_InitSubSystem(SDL_INIT_VIDEO);
	}

	//screen = SDL_SetVideoMode(320, 240, 16, flags);
	{
		int i = 0; // 0 - 320x240, 1 - 400x240, 2 - 480x272
		int surfacewidth, surfaceheight;
		#define NUMOFVIDEOMODES 3
		struct {
			int x;
			int y;
		} vm[NUMOFVIDEOMODES] = {
			{320, 240},
			{400, 240},
			{480, 272}
		};

		// check 3 videomodes: 480x272, 400x240, 320x240
		for(i = NUMOFVIDEOMODES-1; i >= 0; i--) {
			if(SDL_VideoModeOK(vm[i].x, vm[i].y, 16, SDL_SWSURFACE) != 0) {
				surfacewidth = vm[i].x;
				surfaceheight = vm[i].y;
				break;
			}
		}
		screen = SDL_SetVideoMode(surfacewidth, surfaceheight, 16, SDL_SWSURFACE);
	}

	if(!screen) {
		printf("SDL_SetVideoMode screen not initialised.\n");
	} else {
		printf("SDL_SetVideoMode successful.\n");
	}

	VideoBuffer = (unsigned short*)screen->pixels;

	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_SetCaption("Final Burn SDL", 0);

	BurnDrvGetFullSize(&VideoBufferWidth, &VideoBufferHeight);
	printf("w=%d h=%d\n",VideoBufferWidth, VideoBufferHeight);

	nBurnBpp = 2;
	BurnHighCol = myHighCol16;

	BurnRecalcPal();
	nBurnPitch = VideoBufferWidth * 2;
	PhysicalBufferWidth = screen->w;
	BurnVideoBuffer = (unsigned short *)malloc(VideoBufferWidth * VideoBufferHeight * 2);
	memset(BurnVideoBuffer, 0, VideoBufferWidth * VideoBufferHeight * 2);
	BurnerVideoTrans = Blit_320x240_to_320x240; // default blit

	// if source buffer < screen buffer then set general blitting routine with centering if needed
	if(VideoBufferWidth <= screen->w && VideoBufferHeight <= screen->h) {
		p_offset = (screen->w - VideoBufferWidth)/2 + (screen->h - VideoBufferHeight)/2*screen->w;
		q_offset = VideoBufferWidth*VideoBufferHeight-1;
		if(BurnDrvGetFlags() & BDF_ORIENTATION_FLIPPED)
			BurnerVideoTrans = Blitf;
		else
			BurnerVideoTrans = Blit;
	} else {
		// if source buffer is bigger than screen buffer then find an appropriate downscaler
		for(int i = 0; blit_table[i].dst_w != 0; i++) {
			if(blit_table[i].dst_w == screen->w && blit_table[i].dst_h == screen->h &&
			   blit_table[i].src_w == VideoBufferWidth && blit_table[i].src_h == VideoBufferHeight) {
				if (BurnDrvGetFlags() & BDF_ORIENTATION_FLIPPED)
					BurnerVideoTrans = blit_table[i].blitf;
				else
					BurnerVideoTrans = blit_table[i].blit;
				break;
			}
		}
	}

	return 0;
}

void VideoExit()
{
	free(BurnVideoBuffer);
	BurnVideoBuffer = NULL;
	BurnerVideoTrans = Blit_null;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void VideoClear()
{
	SDL_FillRect(screen,NULL,SDL_MapRGBA(screen->format, 0, 0, 0, 255));
	SDL_Flip(screen);
	SDL_FillRect(screen,NULL,SDL_MapRGBA(screen->format, 0, 0, 0, 255));
}

void VideoFlip()
{
	SDL_Flip(screen);
}