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
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <SDL/SDL.h>

#include "burner.h"
#include "sdl_progress.h"
#include "sdl_run.h"
#include "font.h"

SDL_Surface *load_screen = NULL;
int fwidth = 320, fheight = 240; // text surface

void blit_loading_screen()
{
	extern SDL_Surface *screen;
	SDL_Rect dst;

	dst.x = (screen->w - 320) / 2;
	dst.y = (screen->h - 240) / 2;
	SDL_BlitSurface(load_screen, NULL, screen, &dst);
	SDL_Flip(screen);
}

void show_rom_loading_text(char *szText, int nSize, int nTotalSize)
{
	int doffset = 20;
	static long long size = 0;

	DrawRect((uint16 *)load_screen->pixels, doffset, 120, 300, 20, 0, fwidth);

	if (szText)
		DrawString (szText, (uint16 *)load_screen->pixels, doffset, 120, fwidth);

	if (nTotalSize == 0) {
		size = 0;
		DrawRect((uint16 *)load_screen->pixels, doffset, 140, 280, 12, 0x00FFFFFF, fwidth);
		DrawRect((uint16 *)load_screen->pixels, doffset+1, 141, 278, 10, 0x00808080, fwidth);
	} else {
		size += nSize;
		if (size > nTotalSize) size = nTotalSize;
		DrawRect((uint16 *)load_screen->pixels, doffset+1, 141, size * 278 / nTotalSize, 10, 0x00FFFF00, fwidth);
	}

	SDL_Event event;
	while(SDL_PollEvent(&event));
	blit_loading_screen();
}

void show_rom_error_text(char *szText)
{
	int doffset = 20;

	DrawRect((uint16 *)load_screen->pixels, doffset, 120, 300, 20, 0, fwidth);

	DrawString("Error loading rom (not found):", (uint16 *)load_screen->pixels, doffset, 160, fwidth);
	if (szText)
		DrawString (szText, (uint16 *)load_screen->pixels, doffset, 180, fwidth);
	DrawString("Exiting - press any key", (uint16 *)load_screen->pixels, doffset, 200, fwidth);

	blit_loading_screen();

	SDL_Event event;
	while(event.type != SDL_KEYDOWN) SDL_WaitEvent(&event);
	GameLooping = false;
}

int ProgressCreate()
{
	if(!load_screen)
		load_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, fwidth, fheight, 16, 0, 0, 0, 0);

	DrawString("Finalburn Alpha for OpenDingux (v " VERSION ")", (uint16 *)load_screen->pixels, 10, 20, fwidth);
	DrawString("Based on FinalBurnAlpha", (uint16 *)load_screen->pixels, 10, 35, fwidth);
	DrawString("Now loading ... ", (uint16 *)load_screen->pixels, 10, 105, fwidth);
	show_rom_loading_text("Open Zip", 0, 0);
}

int ProgressDestroy()
{
	if(load_screen) {
		SDL_FreeSurface(load_screen);
		load_screen = NULL;
	}
}

int ProgressUpdateBurner(double dProgress, const TCHAR* pszText, bool bAbs)
{
	return 0;
}


int ProgressError(TCHAR* szText, int bWarning)
{
	show_rom_error_text(szText);
	return 0;
}
