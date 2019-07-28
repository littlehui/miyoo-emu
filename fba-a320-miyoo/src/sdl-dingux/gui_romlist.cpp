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
#include "gui_romlist.h"

ROMLIST romlist;

unsigned int romsort[NB_FILTERS][NB_MAX_GAMES];

static void sort_alphabet(unsigned int minimun, unsigned int maximum)
{
	unsigned int tampon, ligne1, ligne2;
	unsigned int min;

	// tri des dossiers en premier
	for(ligne1 = minimun; ligne1 < maximum - 1; ++ligne1) {
		min = ligne1;

		for(ligne2 = ligne1 + 1; ligne2 < maximum; ++ligne2) {
			if(strcmp(romlist.name[romsort[0][ligne2]], romlist.name[romsort[0][min]]) < 0) {
				min = ligne2;
			}
		}

		if (min != ligne1){
			tampon = romsort[0][ligne1];
			romsort[0][ligne1] = romsort[0][min];
			romsort[0][min] = tampon;
		}
	}
}

void gui_sort_romlist()
{
	char g_string[2048];
	FILE *fp;

	romlist.nb_list[0] = 0;
	romlist.long_max = 0;
	romlist.nb_rom = 0;

	for(int i = 0; i < nBurnDrvCount; i++) {
		// save it!
		nBurnDrvSelect  = i;

		romlist.zip[i] = BurnDrvGetTextA(DRV_NAME);
		if(!(romlist.parent[i] = BurnDrvGetTextA(DRV_PARENT)))
			romlist.parent[i] = "fba";
		romlist.name[i] = BurnDrvGetTextA(DRV_FULLNAME);
		romlist.year[i] = BurnDrvGetTextA(DRV_DATE);
		romlist.manufacturer[i] = BurnDrvGetTextA(DRV_MANUFACTURER);
		romlist.longueur[i] = strlen(romlist.name[i]);
		if(romlist.long_max < romlist.longueur[i] ) romlist.long_max = romlist.longueur[i];

		romlist.etat[i] = ROUGE;
		for(int j = 0; j < DIRS_MAX; j++) {
			if(strlen(szAppRomPaths[j]) > 0) {
				sprintf(g_string, "%s%hs.zip", szAppRomPaths[j], romlist.zip[i] );
				if((fp = fopen(g_string, "r")) != NULL) {
					fclose(fp);
					romlist.etat[i] = (BurnDrvGetTextA(DRV_PARENT) ? ORANGE : JAUNE);
					++romlist.nb_rom;
					break;
				}
			}
		}

		romsort[0][i] = i;
	}

	romlist.nb_list[0] = nBurnDrvCount;

	// sort alpha
	sort_alphabet(0, romlist.nb_list[0]);

	romlist.nb_list[1] = 0;
	romlist.nb_list[2] = 0;
	romlist.nb_list[3] = 0;

	for (int i = 0; i < romlist.nb_list[0]; ++i) {
		if(romlist.etat[romsort[0][i]] == ROUGE) {
			romsort[1][romlist.nb_list[1]] = romsort[0][i];
			++romlist.nb_list[1];
		} else {
			romsort[2][romlist.nb_list[2]] = romsort[0][i];
			++romlist.nb_list[2];

			if(romlist.etat[romsort[0][i]] == VERT || romlist.etat[romsort[0][i]] == BLEU ) {
				romsort[3][romlist.nb_list[3]] = romsort[0][i];
				++romlist.nb_list[3];
			}
		}
	}

	printf("romlist.nb_rom=%i\n", romlist.nb_rom);
	printf("romlist.nb_list[0]=%i\n", romlist.nb_list[0]);
	printf("romlist.nb_list[1]=%i\n", romlist.nb_list[1]);
	printf("romlist.nb_list[2]=%i\n", romlist.nb_list[2]);
	printf("romlist.nb_list[3]=%i\n", romlist.nb_list[3]);
}