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
#ifndef _GUI_ROMLIST_H_
#define _GUI_ROMLIST_H_

#define NB_FILTERS	4
#define NB_MAX_GAMES	6000

typedef struct
{
	unsigned int nb_rom;
	unsigned int nb_list[NB_FILTERS];
	char *name[NB_MAX_GAMES];
	char *zip[NB_MAX_GAMES];
	char *year[NB_MAX_GAMES];
	char *manufacturer[NB_MAX_GAMES];
	char *parent[NB_MAX_GAMES];
	unsigned char etat[NB_MAX_GAMES];
	unsigned int longueur[NB_MAX_GAMES];
	unsigned int long_max;
} ROMLIST;

extern ROMLIST romlist;
extern unsigned int romsort[NB_FILTERS][NB_MAX_GAMES];

void gui_sort_romlist();

#endif // _GUI_ROMLIST_H_