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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "burner.h"
#include "gui_main.h"
#include "gui_gfx.h"
#include "gui_romlist.h"

static Uint32 getPixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;

	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	return *(Uint16 *)p;
 }

static void SetPixel(SDL_Surface* Surface, int x, int y, Uint32 pixel)
{
	int bpp = Surface->format->BytesPerPixel;

	Uint8 *p = (Uint8*)Surface->pixels + y * Surface->pitch + x * bpp;

	*(Uint16*)p = pixel;
}

void gui_set_gamma(SDL_Surface *Surface, int FXshadow)
{
	Uint32 pixel;
	Uint8 r, g, b, a;
	int x, y;
	// precalcul palette
	unsigned char precal_pal[256];
	float FLshadow;

	FLshadow = (float) FXshadow / 100 ;
	for(x = 255; x > -1; --x)
		precal_pal[x] = x * FLshadow;

	for(y = 239; y > -1; y--) {
		for(x = 319; x > -1; x--) {
			pixel = getPixel(Surface, x, y);
			SDL_GetRGBA(pixel, Surface->format, &r, &g, &b, &a);
			//Ici, on mettra du code pour modifier les pixels.
			if(r) r = precal_pal[r];
			if(g) g = precal_pal[g];
			if(b) b = precal_pal[b];
			//Et une fois qu'on les a modifiés :
			pixel = SDL_MapRGBA(Surface->format, r, g, b, a);
			//Et pour changer la valeur d'un pixel :
			SetPixel(Surface, x, y, pixel);
		}
	}
}

void ligneH(SDL_Surface* screenSurface, int x, int y, int w, Uint32 coul)
{
	SDL_Rect r;

	r.x = x;
	r.y = y;
	r.w = w;
	r.h = 1;

	SDL_FillRect(screenSurface, &r, coul);
}

void ligneV(SDL_Surface* screenSurface, int x, int y, int h, Uint32 coul)
{
	SDL_Rect r;

	r.x = x;
	r.y = y;
	r.w = 1;
	r.h = h;

	SDL_FillRect(screenSurface, &r, coul);
}

void carre_plein(SDL_Surface* screenSurface, int x, int y, int w, int h, Uint32 coul)
{
	SDL_Rect r;

	r.x = x;
	r.y = y;
	r.w = w;
	r.h = h;

	SDL_FillRect(screenSurface, &r, coul);
}

void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height)
{
	SDL_Rect srcRect;
	srcRect.x = srcX;
	srcRect.y = srcY;
	srcRect.w = width;
	srcRect.h = height;

	SDL_Rect dstRect;
	dstRect.x = dstX;
	dstRect.y = dstY;
	dstRect.w = width;
	dstRect.h = height;

	SDL_BlitSurface(imageSurface, &srcRect, screenSurface, &dstRect);
}

void preparation_fenetre(SDL_Surface *src, SDL_Surface *dest, int win_x, int win_y, int win_l, int win_h)
{
	Uint32 Pnoir = SDL_MapRGB(dest->format, 0, 0, 0);

	drawSprite(src, dest, win_x, win_y, win_x, win_y, win_l, win_h);
	ligneV(dest, win_x-1, win_y, win_h, Pnoir);
	ligneV(dest, win_x+win_l, win_y, win_h, Pnoir);
	ligneH(dest, win_x, win_y-1, win_l, Pnoir);
	ligneH(dest, win_x, win_y+win_h, win_l, Pnoir);
}