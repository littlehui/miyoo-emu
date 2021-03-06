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

#ifndef _SDL_AUDIO_H_
#define _SDL_AUDIO_H_

// with mutexes
int  sdl_open_m(int rate, int channels, int format);
void sdl_close_m(void);
int  sdl_play_m(unsigned char *data, int len);
void sdl_pause(int flag);

// with SDL_LockAudio
int  sdl_open(int rate, int channels, int format);
void sdl_close(void);
int  sdl_play(unsigned char *data, int len);

#endif // _SDL_AUDIO_H_