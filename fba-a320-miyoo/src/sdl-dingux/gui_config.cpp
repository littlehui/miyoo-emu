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
#include "sdl_run.h"
#include "gui_main.h"
#include "gui_gfx.h"
#include "gui_romlist.h"
#include "gui_config.h"

CONFIG cfg;

static char g_string[255];

void gui_load_cfg()
{
	FILE *fp;
	char arg1[128];
	char arg2[255];
	signed long argd;
	char ligne[256];
	char filename[512];

	// force default settings
	cfg.delayspeed = 30;
	cfg.repeatspeed = 1;
	cfg.list = 0;
	cfg.FXshadow = 90;
	cfg.skin = 1;
        //sprintf((char*)cfg.rompath, "%s", "./roms/");

	sprintf(filename, "%s/frontend.cfg", szAppHomePath);

	// read config file
	if((fp = fopen(filename, "r")) != NULL) {
		while(fgets(ligne,sizeof(ligne),fp) != NULL) {
			sscanf(ligne, "%s %d", &arg1, &argd);
			sscanf(ligne, "%s %s", &arg1, &arg2);
			if(strcmp(arg1, "#") != 0) {
				if (strcmp(arg1,"delay_speed") == 0) cfg.delayspeed = argd;
				else if (strcmp(arg1,"repeat_speed") == 0) cfg.repeatspeed = argd;
				else if (strcmp(arg1,"list") == 0) cfg.list = argd;
				else if (strcmp(arg1,"shadow") == 0) cfg.FXshadow = argd;
				else if (strcmp(arg1,"skin") == 0) cfg.skin = argd;
			}
		}
		fclose(fp);
	} else {
		gui_write_cfg();
	}
}

void gui_write_cfg()
{
	FILE *fp;
	char filename[512];

	sprintf(filename, "%s/frontend.cfg", szAppHomePath);

	printf("Writing frontend.cfg\n");
	fp = fopen(filename, "w");

	fputs("# FBA frontend options\n",fp);
	fputs("\n",fp);
	sprintf((char*)g_string, "delay_speed %d\n", cfg.delayspeed);
	fputs(g_string,fp);
	sprintf((char*)g_string, "repeat_speed %d\n", cfg.repeatspeed);
	fputs(g_string,fp);
	sprintf((char*)g_string, "list %d\n",cfg.list);
	fputs(g_string,fp);
	sprintf((char*)g_string, "shadow %d\n", cfg.FXshadow);
	fputs(g_string,fp);
	sprintf((char*)g_string, "skin %d\n", cfg.skin);
	fputs(g_string,fp);

	fclose(fp);
}
