/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2002 Xodnizel
 *  Copyright (C) 2002 Paul Kuliniewicz
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
 */

/// \file
/// \brief Handles joystick input using the SDL.

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "dingoo.h"

#define MAX_JOYSTICKS	1
static SDL_Joystick *s_Joysticks[MAX_JOYSTICKS] = {NULL};

static int s_jinited = 0;

/**
 * Tests if the given button is active on the joystick.
 */
int DTestButtonJoy(ButtConfig *bc)
{
    return(0);
}

/**
 * Shutdown the DINGOO joystick subsystem.
 */
int KillJoysticks()
{
    int n;  // joystick index

	if(!s_jinited) {
		return -1;
	}

	for(n = 0; n < MAX_JOYSTICKS; n++) {
		if (s_Joysticks[n] != 0) {
			SDL_JoystickClose(s_Joysticks[n]);
		}
		s_Joysticks[n]=0;
	}
	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    return(0);
}

/**
 * Initialize the DINGOO joystick subsystem.
 */
int InitJoysticks()
{
    int n; /* joystick index */
	int total;

	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	total = SDL_NumJoysticks();
	if(total>MAX_JOYSTICKS) {
		total = MAX_JOYSTICKS;
	}

	for(n = 0; n < total; n++) {
		/* Open the joystick under SDL. */
		s_Joysticks[n] = SDL_JoystickOpen(n);
		//printf("Could not open joystick %d: %s.\n",
		//joy[n] - 1, SDL_GetError());
		continue;
	}
    SDL_JoystickEventState(SDL_ENABLE);
	s_jinited = 1;
    return(1);
}
