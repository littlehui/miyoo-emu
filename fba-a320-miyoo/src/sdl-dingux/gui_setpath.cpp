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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "burner.h"
#include "sdl_run.h"
#include "gui_main.h"
#include "gui_gfx.h"
#include "gui_romlist.h"
#include "gui_config.h"
#include "gui_setpath.h"

typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

/* defines and macros */
#define FILE_LIST_ROWS 12
#define FILE_LIST_POSITION 8
#define DIR_LIST_X 8
#define DIR_LIST_Y 120

#define color16(red, green, blue) ((red << 11) | (green << 5) | blue)

#define COLOR_BG            color16(05, 03, 02)
#define COLOR_ROM_INFO      color16(22, 36, 26)
#define COLOR_ACTIVE_ITEM   color16(31, 63, 31)
#define COLOR_INACTIVE_ITEM color16(13, 40, 18)
#define COLOR_FRAMESKIP_BAR color16(15, 31, 31)
#define COLOR_HELP_TEXT     color16(16, 40, 24)

extern SDL_Surface *gui_screen;
extern SDL_Surface *barre;

void redraw_screen(void);
void put_string(char *string, unsigned int pos_x, unsigned int pos_y, unsigned char couleur, SDL_Surface *s);
void prep_bg();
void affiche_BG();

void print_string(char *s, u16 fg_color, u16 bg_color, int x, int y)
{
	put_string(s, x, y, fg_color == COLOR_ACTIVE_ITEM ? BLEU : BLANC, gui_screen);
}

int sort_function(const void *dest_str_ptr, const void *src_str_ptr)
{
	char *dest_str = *((char **)dest_str_ptr);
	char *src_str = *((char **)src_str_ptr);

	if(src_str[0] == '.') return 1;

	if(dest_str[0] == '.') return -1;

	return strcasecmp(dest_str, src_str);
}

// This code is copy-pasted from gpsp source, thanks Exophase
// browsing files is commented out, we need dirs only
int gui_setpath(char *path)
{
	SDL_Event gui_event;
	char current_dir_name[MAX_PATH];
	DIR *current_dir;
	struct dirent *current_file;
	struct stat file_info;
	char current_dir_short[81];
	u32 current_dir_length;
	//u32 total_filenames_allocated;
	u32 total_dirnames_allocated;
	//char **file_list;
	char **dir_list;
	u32 num_files;
	u32 num_dirs;
	char *file_name;
	u32 file_name_length;
	u32 ext_pos = -1;
	u32 chosen_file, chosen_dir;
	u32 dialog_result = 1;
	s32 return_value = 1;
	u32 current_file_selection;
	u32 current_file_scroll_value;
	u32 current_dir_selection;
	u32 current_dir_scroll_value;
	u32 current_file_in_scroll;
	u32 current_dir_in_scroll;
	u32 current_file_number, current_dir_number;
	u32 current_column = 0;
	u32 repeat;
	u32 i;

	while(return_value == 1) {
		//current_file_selection = 0;
		//current_file_scroll_value = 0;
		current_dir_selection = 0;
		current_dir_scroll_value = 0;
		//current_file_in_scroll = 0;
		current_dir_in_scroll = 0;

		//total_filenames_allocated = 32;
		total_dirnames_allocated = 32;
		//file_list = (char **)malloc(sizeof(char *) * 32);
		dir_list = (char **)malloc(sizeof(char *) * 32);
		//memset(file_list, 0, sizeof(char *) * 32);
		memset(dir_list, 0, sizeof(char *) * 32);

		num_files = 0;
		num_dirs = 0;
		chosen_file = 0;
		chosen_dir = 0;

		getcwd(current_dir_name, MAX_PATH);
		current_dir = opendir(current_dir_name);

		// DEBUG
		//printf("Current directory: %s\n", current_dir_name);

		do {
			if(current_dir) current_file = readdir(current_dir); else current_file = NULL;

			if(current_file) {
				file_name = current_file->d_name;
				file_name_length = strlen(file_name);

				if((stat(file_name, &file_info) >= 0) && ((file_name[0] != '.') || (file_name[1] == '.'))) {
					if(S_ISDIR(file_info.st_mode)) {
						dir_list[num_dirs] = (char *)malloc(file_name_length + 1);
						strcpy(dir_list[num_dirs], file_name);

						num_dirs++;
					} /*else {
					// Must match one of the wildcards, also ignore the .
						if(file_name_length >= 4) {
							if(file_name[file_name_length - 4] == '.') ext_pos = file_name_length - 4;
							else if(file_name[file_name_length - 3] == '.') ext_pos = file_name_length - 3;
							else ext_pos = 0;

							for(i = 0; wildcards[i] != NULL; i++) {
								if(!strcasecmp((file_name + ext_pos), wildcards[i])) {
									file_list[num_files] = (char *)malloc(file_name_length + 1);

									strcpy(file_list[num_files], file_name);

									num_files++;
									break;
								}
							}
						}
					}*/
				}

				/*if(num_files == total_filenames_allocated) {
					file_list = (char **)realloc(file_list, sizeof(char *) * total_filenames_allocated * 2);
					memset(file_list + total_filenames_allocated, 0, sizeof(char *) * total_filenames_allocated);
					total_filenames_allocated *= 2;
				}*/

				if(num_dirs == total_dirnames_allocated) {
					dir_list = (char **)realloc(dir_list, sizeof(char *) * total_dirnames_allocated * 2);
					memset(dir_list + total_dirnames_allocated, 0, sizeof(char *) * total_dirnames_allocated);
					total_dirnames_allocated *= 2;
				}
			}
		} while(current_file);

		//qsort((void *)file_list, num_files, sizeof(char *), sort_function);
		qsort((void *)dir_list, num_dirs, sizeof(char *), sort_function);

		// DEBUG
		//for(i = 0; i < num_dirs; i++) printf("%s\n", dir_list[i]);
		//for(i = 0; i < num_files; i++) printf("%s\n", file_list[i]);

		closedir(current_dir);

		current_dir_length = strlen(current_dir_name);

		if(current_dir_length > 80) {
			memcpy(current_dir_short, "...", 3);
			memcpy(current_dir_short + 3, current_dir_name + current_dir_length - 77, 77);
			current_dir_short[80] = 0;
		} else {
			memcpy(current_dir_short, current_dir_name, current_dir_length + 1);
		}

		repeat = 1;

		/*if(num_files == 0)*/ current_column = 1;

		//clear_screen(COLOR_BG);
		char print_buffer[256];

		prep_bg();

		while(repeat) {
			affiche_BG();

			print_string(current_dir_short, COLOR_ACTIVE_ITEM, COLOR_BG, DIR_LIST_X, DIR_LIST_Y);

			/*for(i = 0, current_file_number = i + current_file_scroll_value; i < FILE_LIST_ROWS; i++, current_file_number++) {
				if(current_file_number < num_files) {
					strncpy(print_buffer,file_list[current_file_number], 38);
					print_buffer[38] = 0;
					if((current_file_number == current_file_selection) && (current_column == 0)) {
						print_string(print_buffer, COLOR_ACTIVE_ITEM, COLOR_BG, FILE_LIST_POSITION, ((i + 2) * 8));
					} else {
						print_string(print_buffer, COLOR_INACTIVE_ITEM, COLOR_BG, FILE_LIST_POSITION, ((i + 2) * 8));
					}
				}
			}*/
			for(i = 0, current_dir_number = i + current_dir_scroll_value; i < FILE_LIST_ROWS; i++, current_dir_number++) {
				if(current_dir_number < num_dirs) {
					strcpy(print_buffer,dir_list[current_dir_number]);
					//print_buffer[14] = 0;
					if((current_dir_number == current_dir_selection) && (current_column == 1)) {
						drawSprite(barre, gui_screen, 0, 0, 4, DIR_LIST_Y + ((i + 2) * 8), 312, 10);
						print_string(print_buffer, COLOR_ACTIVE_ITEM, COLOR_BG, DIR_LIST_X, DIR_LIST_Y + ((i + 2) * 8));
					} else {
						print_string(print_buffer, COLOR_INACTIVE_ITEM, COLOR_BG, DIR_LIST_X, DIR_LIST_Y + ((i + 2) * 8));
					}
				}
			}

			// Catch input
			// change to read key state later
			while(SDL_PollEvent(&gui_event)) {
				if(gui_event.type == SDL_KEYDOWN) {
					if(gui_event.key.keysym.sym == SDLK_RETURN) {
						if(current_column == 1) {
							repeat = 0;
							return_value = 0;
							sprintf(path, "%s/", current_dir_name);
							break;
						}
					}
					if(gui_event.key.keysym.sym == SDLK_LCTRL) {
						if(current_column == 1) {
							repeat = 0;
							chdir(dir_list[current_dir_selection]);
						} /*else {
							if(num_files != 0) {
								repeat = 0;
								return_value = 0;
								strcpy(result, file_list[current_file_selection]);
								sprintf(result, "%s/%s", current_dir_name, file_list[current_file_selection]);
								break;
							}
						}*/
					}
					if(gui_event.key.keysym.sym == SDLK_LALT || gui_event.key.keysym.sym == SDLK_ESCAPE) {
						return_value = -1;
						repeat = 0;
						break;
					}
					if(gui_event.key.keysym.sym == SDLK_UP) { // DINGOO UP - arrow down
						/*if(current_column == 0) {
							if(current_file_selection) {
								current_file_selection--;
								if(current_file_in_scroll == 0) {
									//clear_screen(COLOR_BG);
									current_file_scroll_value--;
								} else {
									current_file_in_scroll--;
								}
							}
						} else*/ {
							if(current_dir_selection) {
								current_dir_selection--;
								if(current_dir_in_scroll == 0) {
									//clear_screen(COLOR_BG);
									current_dir_scroll_value--;
								} else {
									current_dir_in_scroll--;
								}
							}
						}
					}
					if(gui_event.key.keysym.sym == SDLK_DOWN) {
						/*if(current_column == 0) {
							if(current_file_selection < (num_files - 1)) {
								current_file_selection++;
								if(current_file_in_scroll == (FILE_LIST_ROWS - 1)) {
									//clear_screen(COLOR_BG);
									current_file_scroll_value++;
								} else {
									current_file_in_scroll++;
								}
							}
						} else*/ {
							if(current_dir_selection < (num_dirs - 1)) {
								current_dir_selection++;
								if(current_dir_in_scroll == (FILE_LIST_ROWS - 1)) {
									current_dir_scroll_value++;
								} else {
									current_dir_in_scroll++;
								}
							}
						}
					}
					/*if(gui_event.key.keysym.sym == SDLK_LEFT) {
						if(current_column == 1) {
							if(num_files != 0) current_column = 0;
						}
					}
					if(gui_event.key.keysym.sym == SDLK_RIGHT) {
						if(current_column == 0) {
							if(num_dirs != 0) current_column = 1;
						}
					}*/
				}
			}

			redraw_screen();
		}

		// free pointers
		//for(i = 0; i < num_files; i++) free(file_list[i]);
		//free(file_list);

		for(i = 0; i < num_dirs; i++) free(dir_list[i]);
		free(dir_list);
	}


	return return_value;
}
