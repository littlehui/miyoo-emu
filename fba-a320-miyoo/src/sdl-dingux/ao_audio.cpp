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

#ifdef USE_LIBAO
#include <ao/ao.h>

#include "ao_audio.h"

ao_device *ao_dev;
ao_sample_format ao;

int libao_open(int rate, int channels, int format)
{
	ao_initialize();
	ao.bits = format;
	ao.rate = rate;
	ao.channels = channels;
	ao.byte_format = AO_FMT_LITTLE;
	ao.matrix = NULL;
	ao_dev = ao_open_live(ao_default_driver_id(), &ao, NULL);

	if(ao_dev) return 1; else return 0;
}

void libao_close(void)
{
	ao_close(ao_dev);
	ao_shutdown();
}

int libao_play(unsigned char *data, int len)
{
	ao_play(ao_dev, (char *)data, len);
}

void libao_pause(int flag)
{
	; // dummy
}
#endif
