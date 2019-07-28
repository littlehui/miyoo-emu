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

#ifndef _GUI_GFX_H_
#define _GUI_GFX_H_

void gui_set_gamma(SDL_Surface *Surface, int FXshadow);
void ligneH(SDL_Surface* screenSurface, int x, int y, int w, Uint32 coul);
void ligneV(SDL_Surface* screenSurface, int x, int y, int h, Uint32 coul);
void carre_plein(SDL_Surface* screenSurface, int x, int y, int w, int h, Uint32 coul);
void drawSprite(SDL_Surface* imageSurface, SDL_Surface* screenSurface, int srcX, int srcY, int dstX, int dstY, int width, int height);
void preparation_fenetre(SDL_Surface *src, SDL_Surface *dest, int win_x , int win_y , int win_l , int win_h);

#endif // _GUI_GFX_H_