
/*
    Copyright (c) 2002, 2003 Gregory Montoir

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "config.h"
#ifdef USE_ZLIB
#include "unzip.h"
#endif



#ifdef USE_ZLIB

static int check_zip(const char* filename)
{
  int zip = 0;
  static char SIG[] = { 0x50, 0x4B, 0x03, 0x04 };
  FILE* fp = fopen(filename, "rb");
  if(fp) {
    zip = fgetc(fp) == SIG[0] &&
          fgetc(fp) == SIG[1] &&
          fgetc(fp) == SIG[2] &&
          fgetc(fp) == SIG[3];
    fclose(fp);
  }
  return zip;
}


static int load_rom_zip(const char* filename)
{
  int size, skip_header;
  char romfile[0x100];
  unz_file_info fi;
  unzFile zf = unzOpen(filename);
  if(!zf) return 0;

  if(unzGoToFirstFile(zf) != UNZ_OK)
    return 0;

  if(unzGetCurrentFileInfo(zf, &fi, romfile, sizeof(romfile), NULL, 0, NULL, 0) != UNZ_OK)
    return 0;

  size = fi.uncompressed_size;
  if(size < 0x8000) return 0;

  if((size / 512) & 1) {
    size -= 512;
    skip_header = 1;
  }
  else {
    skip_header = 0;
  }

  printf("size=%d %s\n", size, romfile);

  cart.pages = (size / 0x4000);
  cart.rom = (unsigned char*)malloc(size);
  if(!cart.rom) return 0;

  if(unzOpenCurrentFile(zf) != UNZ_OK)
		return 0;

  // skip header
  if(skip_header)
    unzReadCurrentFile(zf, cart.rom, 512);
  unzReadCurrentFile(zf, cart.rom, size);

  unzCloseCurrentFile(zf);
  unzClose(zf);

  if(strcasecmp(strrchr(romfile, '.'), ".gg") == 0)
    cart.type = TYPE_GG;
  else
    cart.type = TYPE_SMS;

  return 1;
}

#endif


static int load_rom_normal(const char* filename)
{
  int size;
  FILE *fd = fopen(filename, "rb");
  if(!fd) return 0;

  /* Seek to end of file, and get size */
  fseek(fd, 0, SEEK_END);
  size = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  /* Don't load games smaller than 32k */
  if(size < 0x8000) return 0;

  /* Take care of image header, if present */
  if((size / 512) & 1) {
    size -= 512;
    fseek(fd, 512, SEEK_SET);
  }

  cart.pages = (size / 0x4000);
  cart.rom = (unsigned char*)malloc(size);
  if(!cart.rom) return 0;
  fread(cart.rom, size, 1, fd);

  fclose(fd);

  /* Figure out game image type */
  if(strcasecmp(strrchr(filename, '.'), ".gg") == 0)
    cart.type = TYPE_GG;
  else
    cart.type = TYPE_SMS;

  return 1;
}


/* Load a game file */

int load_rom(const char *filename)
{
#if USE_ZLIB
  if(check_zip(filename))
    return load_rom_zip(filename);
#endif
  return load_rom_normal(filename);
}


/* Load SRAM data */
void load_sram(const char* game_name)
{
  if(sms.save) {
    char name[0x100];
    FILE *fd;
    strcpy(name, game_name);
    strcpy(strrchr(name, '.'), ".sav");
    fd = fopen(name, "rb");
    if(fd) {
        fread(sms.sram, 0x8000, 1, fd);
        fclose(fd);
    }
  }
}


/* Save SRAM data */
void save_sram(const char* game_name)
{
    if(sms.save) {
        char name[0x100];
        FILE *fd = NULL;
        strcpy(name, game_name);
        strcpy(strrchr(name, '.'), ".sav");
        fd = fopen(name, "wb");
        if(fd) {
            fwrite(sms.sram, 0x8000, 1, fd);
            fclose(fd);
        }
    }
}


/* Load system state */
int load_state(const char* game_name, int state_slot)
{
    char name[0x100];
    FILE *fd = NULL;
    strcpy(name, game_name);
    sprintf(strrchr(name, '.'), ".st%d", state_slot);
    fd = fopen(name, "rb");
    if(!fd) return (0);
    system_load_state(fd);
    fclose(fd);
    return (1);
}


/* Save system state */
int save_state(const char* game_name, int state_slot)
{
    char name[0x100];
    FILE *fd = NULL;
    strcpy(name, game_name);
    sprintf(strrchr(name, '.'), ".st%d", state_slot);
    fd = fopen(name, "wb");
    if(!fd) return (0);
    system_save_state(fd);
    fclose(fd);
    return(1);
}


/* This is called in system_reset() */
/* not useful since --usesram and no reset during run-time */
void system_load_sram(void) 
{
  //load_sram(cfg.game_name); // TOSEE
}
