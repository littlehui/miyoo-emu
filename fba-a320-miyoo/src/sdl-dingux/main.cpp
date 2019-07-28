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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <SDL/SDL.h>

#include "burner.h"
#include "snd.h"
#include "sdl_run.h"
#include "sdl_video.h"
#include "gui_main.h"

char szAppBurnVer[16] = VERSION;

void CreateCapexLists()
{
	printf("Create rom lists (%d)\n",nBurnDrvCount);
	FILE *zipf;
	FILE *romf;
	zipf = fopen("zipname.fba","w");
	romf = fopen("rominfo.fba","w");
	char *fullname;
	int j;
	for(int i = 0; i < nBurnDrvCount; i++) {
		nBurnDrvSelect = i;
		fullname = (char*)malloc(strlen(BurnDrvGetTextA(DRV_FULLNAME)) + 1);
		strcpy(fullname, BurnDrvGetTextA(DRV_FULLNAME));
		for(j = 0; j < strlen(fullname); j++) {
			if(fullname[j] == ',') fullname[j] = ' ';
		}

		if(BurnDrvGetTextA(DRV_PARENT))
			fprintf(romf,
				"FILENAME( %s %s %s \"%s\" )\n",
				BurnDrvGetTextA(DRV_NAME),
				BurnDrvGetTextA(DRV_PARENT),
				BurnDrvGetTextA(DRV_DATE),
				BurnDrvGetTextA(DRV_MANUFACTURER));
		else
			fprintf(romf,
				"FILENAME( %s fba %s \"%s\" )\n",
				BurnDrvGetTextA(DRV_NAME),
				BurnDrvGetTextA(DRV_DATE),
				BurnDrvGetTextA(DRV_MANUFACTURER));

		fprintf(zipf,
			"%s,%s,%s %s\n",
			BurnDrvGetTextA(DRV_NAME),
			fullname,
			BurnDrvGetTextA(DRV_DATE),
			BurnDrvGetTextA(DRV_MANUFACTURER));
		free(fullname);
	}
	fclose(zipf);
	fclose(romf);
	char temp[24];
	strcpy(temp,"FBA ");
	strcat(temp,szAppBurnVer);
	strcat(temp,".dat");
	create_datfile(temp, 0);
}

int FindDrvByFileName(const char *fn)
{
	char romname[MAX_PATH];
	char *p;

	// FIXME: use p = strrchr(fn, '/');
	// and add given path to szAppRomPaths list
	strcpy(szAppRomPaths[0], fn);
	p = strrchr(szAppRomPaths[0], '/');
	if(p) {
		p++;
		strcpy(romname, p);

		*p = 0;
		p = strrchr(romname, '.');
		if(p) *p = 0;
		else {
			// error
			return -1;
		}
	} else {
		// error
		return -1;
	}

	// find rom by name
	for(nBurnDrvSelect = 0; nBurnDrvSelect < nBurnDrvCount; nBurnDrvSelect++) {
		if(strcasecmp(romname, BurnDrvGetText(DRV_NAME)) == 0)
			return nBurnDrvSelect;
	}

	if(nBurnDrvSelect >= nBurnDrvCount) {
		// unsupport rom ...
		nBurnDrvSelect = ~0U;
		printf("Rom %s not supported!\n", romname);
		return -1;
	}

	nBurnDrvSelect = 0;
	return -1;
}

void parse_cmd(int argc, char *argv[], char *path)
{
	int option_index, c;
	int val;
	char *p;
	printf("num args: %d\n",argc);
	for (c=0;c<argc;c++)
	{
		printf("args %d is %s\n",c,argv[c]);
	}

	static struct option long_opts[] = {
		{"sound-sdl-old", 0, &options.sound, 3},
		{"sound-sdl", 0, &options.sound, 2},
		{"sound-ao", 0, &options.sound, 1},
		{"no-sound", 0, &options.sound, 0},
		{"samplerate", required_argument, 0, 'r'},
		{"frameskip", required_argument, 0, 'c'},
		{"vsync", 0, &options.vsync, 1},
		{"scaling", required_argument, 0, 'a'},
		{"rotate", required_argument, 0, 'o'},
		{"sense", required_argument, 0, 'd'},
		{"showfps", 0, &options.showfps, 1},
		{"no-showfps", 0, &options.showfps, 0},
		{"create-lists", 0, &options.create_lists, 1},
		{"68kcore", required_argument, 0, 's'},
		{"z80core", required_argument, 0, 'z'},
	};

	option_index=optind=0;

	int z2;

	while((c=getopt_long(argc, argv, "", long_opts, &option_index))!=EOF) {
		switch(c) {
			case 'r':
				if(!optarg) continue;
				if(strcmp(optarg, "11025") == 0) options.samplerate = 0;
				if(strcmp(optarg, "16000") == 0) options.samplerate = 1;
				if(strcmp(optarg, "22050") == 0) options.samplerate = 2;
				if(strcmp(optarg, "32000") == 0) options.samplerate = 3;
				if(strcmp(optarg, "44100") == 0) options.samplerate = 4;
				break;
			case 'c':
				if(!optarg) continue;
				if(strcmp(optarg, "auto") == 0) options.frameskip = -1;
				else {
					z2=0;
					sscanf(optarg,"%d",&z2);
					if ((z2>60) || (z2<0)) z2=0;
					options.frameskip = z2;
				}
				break;
			case 's':
				if(!optarg) continue;
				z2=0;
				sscanf(optarg,"%d",&z2);
				if ((z2>2) || (z2<0)) z2=0;
				options.m68kcore = z2;
				break;
			case 'z':
				if(!optarg) continue;
				z2=0;
				sscanf(optarg,"%d",&z2);
				if ((z2>2) || (z2<0)) z2=0;
				options.z80core = z2;
				break;
			case 'a':
				if(!optarg) continue;
				z2=0;
				sscanf(optarg,"%d",&z2);
				if ((z2>3) || (z2<0)) z2=0;
				options.rescale = z2;
				break;
			case 'o':
				if(!optarg) continue;
				z2=0;
				sscanf(optarg,"%d",&z2);
				if ((z2>2) || (z2<0)) z2=0;
				options.rotate = z2;
				break;
			case 'd':
				if(!optarg) continue;
				z2=0;
				sscanf(optarg,"%d",&z2);
				if ((z2>100) || (z2<10)) z2=100;
				options.sense = z2;
				break;
		}
	}

	if(optind < argc) {
		strcpy(path, argv[optind]);
	}
}

/*
 * application main()
 */
#undef main
int main(int argc, char **argv )
{
	char path[MAX_PATH];

	BurnLibInit();		// init Burn core
	BurnPathsInit();	// init paths or create them if needed
/*
	if (argc < 2)
	{
		int c;
		printf ("Usage: %s <path to rom><shortname>.zip\n   ie: %s ./uopoko.zip\n Note: Path and .zip extension are mandatory.\n\n",argv[0], argv[0]);
		printf ("Supported (but not necessarily working) roms:\n\n");

		BurnLibInit();
		for (nBurnDrvSelect=0; nBurnDrvSelect<nBurnDrvCount; nBurnDrvSelect++)
		{
			//nBurnDrvActive=nBurnDrvSelect;
			printf ("%-20s ", BurnDrvGetTextA(DRV_NAME)); c++;
			if (c == 3)
			{
				c = 0;
				printf ("\n");
			}
		}
		printf ("\n\n");
		CreateCapexLists(); // generate rominfo.fba and zipname.fba
		return 0;
	}
*/

	ConfigGameDefault();

	// alter cfg if any param is given
	if (argc >= 2)
		parse_cmd(argc, argv, path);

	ConfigAppLoad();

	if((SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE)) < 0) {
		printf("Sdl failed to init\n");
		exit(0);
	}

	if (argc < 2)
		GuiRun();
	else {
		int drv;

		if((drv = FindDrvByFileName(path)) >= 0)
			RunEmulator(drv);
	}

	BurnLibExit();

	if (argc < 2)
		ConfigAppSave();
	SDL_Quit();

	return 0;
}