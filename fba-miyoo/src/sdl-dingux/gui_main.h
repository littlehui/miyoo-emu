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
 
#ifndef _GUI_MAIN_H_
#define _GUI_MAIN_H_

#define START_X	8
#define START_Y	120

#define BLANC		0
#define ROUGE		9
#define ORANGE		18
#define JAUNE		27
#define VERT		36
#define BLEU		45

//#define OPTIONS_FOR_GCW0
#ifndef OPTIONS_FOR_GCW0
  #define OPTIONS_FOR_A320
#endif

#define OPTION_FBA_RUN			1
#define OPTION_FBA_SOUND		2
#define OPTION_FBA_SAMPLERATE		3
#define OPTION_FBA_VSYNC		4
#define OPTION_FBA_SHOWFPS		5
#define OPTION_FBA_68K			6
#define OPTION_FBA_Z80			7
#define OPTION_FBA_ANALOG		8

#define OPTION_FBA_FIRST		OPTION_FBA_RUN
#define OPTION_FBA_LAST			OPTION_FBA_ANALOG

#define OPTION_GUI_DELAYSPEED		0
#define OPTION_GUI_REPEATSPEED		1
#define OPTION_GUI_LIST			2
#define OPTION_GUI_SHADOW		3
#define OPTION_GUI_SKIN			4
#define OPTION_GUI_PATH0		5
#define OPTION_GUI_PATH1		6
#define OPTION_GUI_PATH2		7
#define OPTION_GUI_PATH3		8
#define OPTION_GUI_PATH4		9
#define OPTION_GUI_PATH5		10
#define OPTION_GUI_PATH6		11
#define OPTION_GUI_RETURN		12

#define OPTION_GUI_LAST			OPTION_GUI_RETURN

void GuiRun();

#endif // _GUI_MAIN_H_
