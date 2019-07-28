
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

#include <stdlib.h>
#include <linux/soundcard.h>
//#include <sys/sysmips.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "SDL.h"
#include "SDL_thread.h"
#include "shared.h"
#include "saves.h"
//#include "filters.h"
#include "sdlsms.h"
#include "bigfontwhite.h"
#include "bigfontred.h"
#include "font.h"
#include "settingsdir.h"

int selectpressed = 0,startpressed = 0;
static int quit = 0;
static int volume = 30;
static int startvolume=50;
static int showfps = -1;
static int fullscreen = 1;
static SDL_Surface *font = NULL;
static SDL_Surface *bigfontred = NULL;
static SDL_Surface *bigfontwhite = NULL;
static const char *rom_filename;
static int fps = 0;
static int framecounter = 0;
static long time1 = 0;
static t_sdl_sync sdl_sync;
static t_sdl_video sdl_video;
static t_sdl_sound sdl_sound;
static t_sdl_controls sdl_controls;
static t_sdl_joystick sdl_joystick;
/*static t_filterfunc filters[FILTER_NUM] = {
  filter_2xsai,
  filter_super2xsai,
  filter_supereagle,
  filter_advmame2x,
  filter_tv2x,
  filter_normal2x,
  filter_bilinear,
  filter_dotmatrix
};*/
static int readvolume()
{
	char *mixer_device = "/dev/mixer";
	int mixer;
    int basevolume = 50;
	mixer = open(mixer_device, O_RDONLY);
	if (ioctl(mixer, SOUND_MIXER_READ_VOLUME, &basevolume) == -1) {
		fprintf(stderr, "Failed opening mixer for read - VOLUME\n");
	}
	close(mixer);
	return  (basevolume>>8) & basevolume ;
}

static void setvolume(int involume)
{
	char *mixer_device = "/dev/mixer";
	int mixer;
    int newvolume = involume;
	if (newvolume > 100) newvolume = 100;
	if (newvolume < 0) newvolume = 0;
	int oss_volume = newvolume | (newvolume << 8); // set volume for both channels
	mixer = open(mixer_device, O_WRONLY);
	if (ioctl(mixer, SOUND_MIXER_WRITE_VOLUME, &oss_volume) == -1) {
		fprintf(stderr, "Failed opening mixer for write - VOLUME\n");
	}
	close(mixer);
}



/* video */

/* alekmaul's scaler taken from mame4all */
void bitmap_scale(int startx, int starty, int viswidth, int visheight, int newwidth, int newheight,int pitchsrc,int pitchdest, uint16_t *src, uint16_t *dst) {
  unsigned int W,H,ix,iy,x,y;
  x=startx<<16;
  y=starty<<16;
  W=newwidth;
  H=newheight;
  ix=(viswidth<<16)/W;
  iy=(visheight<<16)/H;

  do
  {
    uint16_t *buffer_mem=&src[(y>>16)*pitchsrc];
    W=newwidth; x=startx<<16;
    do {
      *dst++=buffer_mem[x>>16];
      x+=ix;
    } while (--W);
    dst+=pitchdest;
    y+=iy;
  } while (--H);
}

/* end alekmaul's scaler taken from mame4all */

uint16_t gfx_font_width(SDL_Surface* inFont, char* inString) {
	if((inFont == NULL) || (inString == NULL))
		return 0;
	uintptr_t i, tempCur, tempMax;
	for(i = 0, tempCur = 0, tempMax = 0; inString[i] != '\0'; i++) {
		if(inString[i] == '\t')
			tempCur += 4;
		else if((inString[i] == '\r') || (inString[i] == '\n'))
			tempCur = 0;
		else
			tempCur++;
		if(tempCur > tempMax) tempMax = tempCur;
	}
	tempMax *= (inFont->w >> 4);
	return tempMax;
}

uint16_t gfx_font_height(SDL_Surface* inFont) {
	if(inFont == NULL)
		return 0;
	return (inFont->h >> 4);
}


void gfx_font_print(SDL_Surface* dest,int16_t inX, int16_t inY, SDL_Surface* inFont, char* inString) {
	if((inFont == NULL) || (inString == NULL))
		return;

	uint16_t* tempBuffer = dest->pixels;
	uint16_t* tempFont = inFont->pixels;
	uint8_t*  tempChar;
	int16_t   tempX = inX;
	int16_t   tempY = inY;
	uintptr_t i, j, x, y;

    SDL_LockSurface(dest);
    SDL_LockSurface(inFont);

	for(tempChar = (uint8_t*)inString; *tempChar != '\0'; tempChar++) {
		if(*tempChar == ' ') {
			tempX += (inFont->w >> 4);
			continue;
		}
		if(*tempChar == '\t') {
			tempX += ((inFont->w >> 4) << 2);
			continue;
		}
		if(*tempChar == '\r') {
			tempX = inX;
			continue;
		}
		if(*tempChar == '\n') {
			tempX = inX;
			tempY += (inFont->h >> 4);
			continue;
		}
		for(j = ((*tempChar >> 4) * (inFont->h >> 4)), y = tempY; (j < (((*tempChar >> 4) + 1) * (inFont->h >> 4))) && (y < 240); j++, y++) {
			for(i = ((*tempChar & 0x0F) * (inFont->w >> 4)), x = tempX; (i < (((*tempChar & 0x0F) + 1) * (inFont->w >> 4))) && (x < 320); i++, x++) {
				tempBuffer[(y * dest->w) + x] |= tempFont[(j * inFont->w) + i];
			}
		}
		tempX += (inFont->w >> 4);
	}
    SDL_UnlockSurface(dest);
    SDL_UnlockSurface(inFont);
}


void gfx_font_print_char(SDL_Surface* dest,int16_t inX, int16_t inY, SDL_Surface* inFont, char inChar) {
	char tempStr[2] = { inChar, '\0' };
	gfx_font_print(dest,inX, inY, inFont, tempStr);
}

void gfx_font_print_center(SDL_Surface* dest,int16_t inY, SDL_Surface* inFont, char* inString) {
	int16_t tempX = (320 - gfx_font_width(inFont, inString)) >> 1;
	gfx_font_print(dest,tempX, inY, inFont, inString);
}

void gfx_font_print_fromright(SDL_Surface* dest,int16_t inX, int16_t inY, SDL_Surface* inFont, char* inString) {
	int16_t tempX = inX - gfx_font_width(inFont, inString);
	gfx_font_print(dest,tempX, inY, inFont, inString);
}

SDL_Surface* gfx_tex_load_tga_from_array(uint8_t* buffer) {
	if(buffer == NULL)
		return NULL;

	uint8_t  tga_ident_size;
	uint8_t  tga_color_map_type;
	uint8_t  tga_image_type;
	uint16_t tga_color_map_start;
	uint16_t tga_color_map_length;
	uint8_t  tga_color_map_bpp;
	uint16_t tga_origin_x;
	uint16_t tga_origin_y;
	uint16_t tga_width;
	uint16_t tga_height;
	uint8_t  tga_bpp;
	uint8_t  tga_descriptor;

	tga_ident_size = buffer[0];
	tga_color_map_type = buffer[1];
	tga_image_type = buffer[2];
	tga_color_map_start = buffer[3] + (buffer[4] << 8);
	tga_color_map_length = buffer[5] + (buffer[6] << 8);
	tga_color_map_bpp = buffer[7];
	tga_origin_x = buffer[8] + (buffer[9] << 8);
	tga_origin_y = buffer[10] + (buffer[11] << 8);
	tga_width = buffer[12] + (buffer[13] << 8);
	tga_height = buffer[14] + (buffer[15] << 8);
	tga_bpp = buffer[16];
	tga_descriptor = buffer[17];

	uint32_t bufIndex = 18;

	SDL_Surface* tempTexture = SDL_CreateRGBSurface(SDL_SWSURFACE, tga_width, tga_height, 16, sdl_video.surf_screen->format->Rmask, sdl_video.surf_screen->format->Gmask, sdl_video.surf_screen->format->Bmask, sdl_video.surf_screen->format->Amask);
	if(tempTexture == NULL) {
		return NULL;
	}

	int upsideDown = (tga_descriptor & 0x20) > 0;

	uintptr_t i;
	uintptr_t iNew;
	uint8_t tempColor[3];
    SDL_LockSurface(tempTexture);
	uint16_t* tempTexPtr = tempTexture->pixels;
	for(i = 0; i < (tga_width * tga_height); i++) {
		tempColor[2] = buffer[bufIndex + 0];
		tempColor[1] = buffer[bufIndex + 1];
		tempColor[0] = buffer[bufIndex + 2];
		bufIndex += 3;

		if (upsideDown)
			iNew = i;
		else
			iNew = (tga_height - 1 - (i / tga_width)) * tga_width + i % tga_width;

		tempTexPtr[iNew] = SDL_MapRGB(sdl_video.surf_screen->format,tempColor[0], tempColor[1], tempColor[2]);
	}
    SDL_UnlockSurface(tempTexture);
	return tempTexture;
}

void menu()
{
    SDL_Surface* TmpScreen;
    int pressed = 0;
    int currentselection = 1;
    int miniscreenwidth = 140;
    int miniscreenheight = 135;
    SDL_Rect dstRect;
    char *cmd = NULL;
    SDL_Event Event;
    SDL_Surface* miniscreen = SDL_CreateRGBSurface(SDL_SWSURFACE, miniscreenwidth, miniscreenheight, 16, sdl_video.surf_screen->format->Rmask, sdl_video.surf_screen->format->Gmask, sdl_video.surf_screen->format->Bmask, sdl_video.surf_screen->format->Amask);
    SDL_LockSurface(miniscreen);
if(IS_GG)
    bitmap_scale(104,28,160,144,miniscreenwidth,miniscreenheight,256,0,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)miniscreen->pixels);
else
    bitmap_scale(56,4,256,192,miniscreenwidth,miniscreenheight,256,0,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)miniscreen->pixels);
    SDL_UnlockSurface(sdl_video.surf_screen);
    SDL_UnlockSurface(miniscreen);
    char text[50];
    TmpScreen = SDL_DisplayFormat(sdl_video.surf_screen);
    SDL_PollEvent(&Event);
    while (((currentselection != 1) && (currentselection != 7)) || (!pressed))
    {

        pressed = 0;
        SDL_FillRect( TmpScreen, NULL, 0 );

        dstRect.x = 320-5-miniscreenwidth;
        dstRect.y = 30;
        dstRect.h = miniscreenheight;
        dstRect.w = miniscreenwidth;
        SDL_BlitSurface(miniscreen,NULL,TmpScreen,&dstRect);

        gfx_font_print_center(TmpScreen,5,bigfontwhite,"DINGUX SMS_SDL");

        if (currentselection == 1)
            gfx_font_print(TmpScreen,5,25,bigfontred,"Continue");
        else
            gfx_font_print(TmpScreen,5,25,bigfontwhite,"Continue");

/*
        sprintf(text,"Volume %d",volume);

        if (currentselection == 2)
            gfx_font_print(TmpScreen,5,45,bigfontred,text);
        else
            gfx_font_print(TmpScreen,5,45,bigfontwhite,text);
*/
        sprintf(text,"Save State %d",sdl_controls.state_slot);

        if (currentselection == 3)
            gfx_font_print(TmpScreen,5,65,bigfontred,text);
        else
            gfx_font_print(TmpScreen,5,65,bigfontwhite,text);

        sprintf(text,"Load State %d",sdl_controls.state_slot);

        if (currentselection == 4)
            gfx_font_print(TmpScreen,5,85,bigfontred,text);
        else
            gfx_font_print(TmpScreen,5,85,bigfontwhite,text);

        if (currentselection == 5)
        {
            if (fullscreen == 1)
                gfx_font_print(TmpScreen,5,105,bigfontred,"Stretched FS");
            else
                if (fullscreen == 2)
                    gfx_font_print(TmpScreen,5,105,bigfontred,"Aspect FS");
                else
                    gfx_font_print(TmpScreen,5,105,bigfontred,"Native Res");
        }
        else
        {
            if (fullscreen == 1)
                gfx_font_print(TmpScreen,5,105,bigfontwhite,"Stretched FS");
            else
                if (fullscreen == 2)
                    gfx_font_print(TmpScreen,5,105,bigfontwhite,"Aspect FS");
                else
                    gfx_font_print(TmpScreen,5,105,bigfontwhite,"Native Res");
        }

        sprintf(text,"%s",showfps ? "Show fps on" : "Show fps off");

        if (currentselection == 6)
            gfx_font_print(TmpScreen,5,125,bigfontred,text);
        else
            gfx_font_print(TmpScreen,5,125,bigfontwhite,text);


        if (currentselection == 7)
            gfx_font_print(TmpScreen,5,145,bigfontred,"Quit");
        else
            gfx_font_print(TmpScreen,5,145,bigfontwhite,"Quit");









        gfx_font_print_center(TmpScreen,240-40-gfx_font_height(font),font,"Dingux sms_sdl has been ported by joyrider");
        gfx_font_print_center(TmpScreen,240-30-gfx_font_height(font),font,"Thanks to alekmaul for the scaler example,");
        gfx_font_print_center(TmpScreen,240-20-gfx_font_height(font),font,"Harteex for the tga loading and bin2h,");
        gfx_font_print_center(TmpScreen,240-10-gfx_font_height(font),font,"everyone on #dingoo-a320 and #dingoonity!");

        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_KEYDOWN)
            {
                switch(Event.key.keysym.sym)
                {
                    case SDLK_UP:
                        currentselection--;
			if(currentselection == 2) currentselection--; 
                        if (currentselection == 0)
                            currentselection = 7;
                        break;
                    case SDLK_DOWN:
                        currentselection++;
			if(currentselection == 2) currentselection++; 
                        if (currentselection == 8)
                            currentselection = 1;
                        break;
                    case SDLK_LCTRL:
                    case SDLK_LALT:
                    case SDLK_RETURN:
                        pressed = 1;
                        break;
                    case SDLK_TAB:
                        if(currentselection == 2)
                        {
                            volume-=10;
                            if (volume < 0)
                                volume = 0;
                            setvolume(volume);
                        }
                        break;
                    case SDLK_BACKSPACE:
                         if(currentselection == 2)
                        {
                            volume+=10;
                            if (volume > 100)
                                volume = 100;
                            setvolume(volume);
                        }
                        break;
                    case SDLK_LEFT:
                        switch(currentselection)
                        {
                            case 2:
                                volume--;
                                if (volume < 0)
                                    volume = 0;
                                setvolume(volume);
                                break;
                            case 3:
                            case 4:
                                sdl_controls.state_slot--;
				if (sdl_controls.state_slot == 0)
					sdl_controls.state_slot = 1;
                                break;
                            case 5:
                                fullscreen--;
                                    if (fullscreen < 0)
                                        fullscreen = 2;
                                break;
                            case 6:
                                showfps = !showfps;
                                break;

                        }
                        break;
                    case SDLK_RIGHT:
                        switch(currentselection)
                        {
                            case 2:
                                volume++;
                                if (volume >100)
                                    volume = 100;
                                setvolume(volume);
                                break;
                            case 3:
                            case 4:
                                sdl_controls.state_slot++;
				if (sdl_controls.state_slot == 10)
					sdl_controls.state_slot = 9;
                                break;
                            case 5:
                                fullscreen++;
                                if (fullscreen > 2)
                                    fullscreen = 0;
                                break;
                            case 6:
                                showfps = !showfps;
                                break;
                        }
                        break;


                }
            }
        }

        if (pressed)
        {
            switch(currentselection)
            {
                case 6:
                    showfps = !showfps;						
                    break;
                case 5 :
                    fullscreen++;
                    if (fullscreen > 2)
                        fullscreen = 0;
                    break;
                case 3 :
                    if(save_state(rom_filename, sdl_controls.state_slot))
                    	currentselection = 1;
                    break;
                case 4 :
		    if(load_state(rom_filename, sdl_controls.state_slot))
                    	currentselection = 1;
                    break;
            }
        }
    SDL_BlitSurface(TmpScreen,NULL,sdl_video.surf_screen,NULL);
    SDL_Flip(sdl_video.surf_screen);
    SDL_Delay(4);

        ++sdl_video.frames_rendered; 
    }
    if (currentselection == 7)
    {
        quit = 1;
    }
}


static void sdlsms_video_blit_center(SDL_Surface* screen, SDL_Surface* buffer)
{
  /* center the bitmap in the screen only if necessary */
  if(screen->w != buffer->w || screen->h != buffer->h) {
    int dx, dy;
    SDL_Rect rect;
    
    //SDL_FillRect(screen, NULL, 0);
    if(screen->h > buffer->h) {
      dx = (screen->w - buffer->w) >> 1;
      dy = (screen->h - buffer->h) >> 1;
      rect.x = dx;
      rect.y = dy;
      rect.w = buffer->w - dx;
      rect.h = buffer->h - dy;
      SDL_BlitSurface(buffer, NULL, screen, &rect);
    }
    else {
      dx = (buffer->w - screen->w) >> 1;
      dy = (buffer->h - screen->h) >> 1;
      rect.x = dx;
      rect.y = dy;
      rect.w = screen->w + dx;
      rect.h = screen->h + dy;
      SDL_BlitSurface(buffer, &rect, screen, NULL);
    }
  }
  else
	  SDL_BlitSurface(buffer, NULL, screen, NULL);
}

static void sdlsms_video_take_screenshot()
{
  int status;
  char ssname[0x100];

  strcpy(ssname, rom_filename);
  sprintf(strrchr(ssname, '.'), "-%03d.bmp", sdl_video.current_screenshot);
  ++sdl_video.current_screenshot;
  SDL_LockSurface(sdl_video.surf_screen);
  status = SDL_SaveBMP(sdl_video.surf_screen, ssname);
  SDL_UnlockSurface(sdl_video.surf_screen);
  if(status == 0)
    printf("[INFO] Screenshot written to '%s'.\n", ssname);
}

static int sdlsms_video_init(int frameskip, int afullscreen, int filter)
{
  int screen_width, screen_height;
  Uint32 vidflags = SDL_HWSURFACE/* | SDL_TRIPLEBUF*/;
  FILE *f;
  int vol;
  screen_width  = (IS_GG) ? GG_SCREEN_WIDTH  : SMS_SCREEN_WIDTH;
  screen_height = (IS_GG) ? GG_SCREEN_HEIGHT : SMS_SCREEN_HEIGHT;

 /* if(filter >= 0 && filter < FILTER_NUM)
    sdl_video.current_filter = filter;
  else*/
  sdl_video.current_filter = -1;
  sdl_video.surf_bitmap = NULL;
//  if(fullscreen) {
//    vidflags |= SDL_FULLSCREEN;
//  }

 /* if(sdl_video.current_filter != -1) {
    screen_width  <<= 1;
    screen_height <<= 1;
  }
*/

  if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }

  /* Create the 16 bits bitmap surface (RGB 565) */
  /* Even if we are in GameGear mode, we need to allocate space for a buffer of 256x192 */
  sdl_video.surf_bitmap = SDL_CreateRGBSurface(SDL_SWSURFACE, SMS_SCREEN_WIDTH, 
                           SMS_SCREEN_HEIGHT + FILTER_MARGIN_HEIGHT * 2, 16, 
                           0xF800, 0x07E0, 0x001F, 0);
  if(!sdl_video.surf_bitmap) {
    printf("ERROR: can't create surface: %s.\n", SDL_GetError());
    return 0;  	  	
  }
 
 /* if(sdl_video.current_filter >= 0) {
    sdl_video.surf_filter = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                              SMS_SCREEN_WIDTH * 2, SMS_SCREEN_HEIGHT * 2, 
                              16, 0xF800, 0x07E0, 0x001F, 0);
    filter_init_2xsai(565);
  }
*/

  sdl_video.surf_screen = SDL_SetVideoMode(320, 240, 16, /*vidflags*/SDL_SWSURFACE);
  if(!sdl_video.surf_screen) {
    printf("ERROR: can't set video mode (%dx%d): %s.\n", 320, 240, SDL_GetError());
    return 0;
  }

  sdl_video.bitmap_offset = sdl_video.surf_bitmap->pitch * FILTER_MARGIN_HEIGHT;

 // if(fullscreen) {
    SDL_ShowCursor(SDL_DISABLE);
 // }

 // SDL_WM_SetCaption(SMSSDL_TITLE, NULL);

    startvolume = readvolume();
 const char *settingsFile = getSettingsFile("sms_sdl.dat");
    f = settingsFile ? fopen(settingsFile, "rb") : NULL;
    free(settingsFile);
    if(f)
    {
        fread(&fullscreen,sizeof(int),1,f);
        fread(&volume,sizeof(int),1,f);
        fread(&sdl_controls.state_slot,sizeof(int),1,f);
        fread(&showfps,sizeof(int),1,f);
        fclose(f);
    }
    else
    {
        fullscreen = 1;
        volume = 75;
        sdl_controls.state_slot = 1;
	showfps = 0;
    }
    setvolume(volume);
  return 1;
}

static void sdlsms_video_finish_update()
{
    char buf[100];
    SDL_Rect dest;
/*  if(sdl_video.current_filter != -1) {
    SDL_LockSurface(sdl_video.surf_bitmap);
	  SDL_LockSurface(sdl_video.surf_filter);
    filters[sdl_video.current_filter](
      (Uint8*)sdl_video.surf_bitmap->pixels + sdl_video.bitmap_offset,
      sdl_video.surf_bitmap->pitch, 
      (Uint8*)sdl_video.surf_filter->pixels,
      sdl_video.surf_filter->pitch,
      sdl_video.surf_bitmap->w, 
      sdl_video.surf_bitmap->h - 2 * FILTER_MARGIN_HEIGHT
    );
	  SDL_UnlockSurface(sdl_video.surf_filter);
    SDL_UnlockSurface(sdl_video.surf_bitmap);
    sdlsms_video_blit_center(sdl_video.surf_screen, sdl_video.surf_filter);
  }
  else {*/
 

//sdlsms_video_blit_center(sdl_video.surf_screen, sdl_video.surf_bitmap);
	SDL_FillRect(sdl_video.surf_screen,NULL,0); //clear everything (menu screen leftovers)
	SDL_LockSurface(sdl_video.surf_screen);
	SDL_LockSurface(sdl_video.surf_bitmap);
        if (fullscreen == 1)
	{
		if(IS_GG)
			bitmap_scale(104,28,160,144,320,240,256,0,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)sdl_video.surf_screen->pixels);
		else
			bitmap_scale(56,4,256,192,320,240,256,0,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)sdl_video.surf_screen->pixels);
	}
       	else
       		if (fullscreen == 2)
		{
			if(IS_GG)
        	        	bitmap_scale(104,28,160,144,267,240,256,53,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)sdl_video.surf_screen->pixels+25);
			else
        	        	bitmap_scale(56,4,256,192,304,240,256,16,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)sdl_video.surf_screen->pixels+8);				
		}
                else
		{
			if(IS_GG)
        	        	bitmap_scale(104,28,160,144,160,144,256,160,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)sdl_video.surf_screen->pixels+15440);
			else
        	        	bitmap_scale(56,4,256,192,256,192,256,64,(uint16_t*)sdl_video.surf_bitmap,(uint16_t*)sdl_video.surf_screen->pixels+7680+32);	
		}
	SDL_UnlockSurface(sdl_video.surf_bitmap);
	SDL_UnlockSurface(sdl_video.surf_screen);
        if(showfps) {
            sprintf(buf,"%d fps",fps);
            dest.x = 8;
            dest.y = 8;
            dest.w = 36;
            dest.h = 8;
            SDL_FillRect(sdl_video.surf_screen, &dest, 0);

            gfx_font_print(sdl_video.surf_screen, 8, 8, font, buf);
            framecounter++;
            if (SDL_GetTicks() - time1 > 1000)
            {
                fps=framecounter;
                framecounter=0;
                time1 = SDL_GetTicks();
            }
        } 
	SDL_Flip(sdl_video.surf_screen);
 // }
}

static void sdlsms_video_update()
{
  int skip_current_frame = 0;

  if(sdl_video.frame_skip > 1)
    skip_current_frame = (sdl_video.frames_rendered % sdl_video.frame_skip == 0) ? 0 : 1;

  if(!skip_current_frame) {
   //SDL_LockSurface(sdl_video.surf_screen);
    sms_frame(0);
   //SDL_UnlockSurface(sdl_video.surf_screen);
    sdlsms_video_finish_update();
  }
  else 
    sms_frame(1);

  ++sdl_video.frames_rendered;
}

static void sdlsms_video_close()
{
  
  if(sdl_video.surf_screen)
  {
        setvolume(startvolume);
	SDL_FreeSurface(sdl_video.surf_screen);
	FILE *f;
const char *settingsFile = getSettingsFile("sms_sdl.dat");
    f = settingsFile ? fopen(settingsFile, "wb") : NULL;
    free(settingsFile);
        if(f)
        {
            fwrite(&fullscreen,sizeof(int),1,f);
            fwrite(&volume,sizeof(int),1,f);
            fwrite(&sdl_controls.state_slot,sizeof(int),1,f);
            fwrite(&showfps,sizeof(int),1,f);
            fsync(f);
            fclose(f);
        }

  }
  if(sdl_video.surf_bitmap) SDL_FreeSurface(sdl_video.surf_bitmap);
 // if(sdl_video.surf_filter) SDL_FreeSurface(sdl_video.surf_filter);
  printf("[INFO] Frames rendered = %lu.\n", sdl_video.frames_rendered);
}


/* sound */

static void sdlsms_sound_callback(void *userdata, Uint8 *stream, int len)
{
  if(sdl_sound.current_emulated_samples < len) {
    memset(stream, 0, len);
  }
  else {
    memcpy(stream, sdl_sound.buffer, len);
    /* loop to compensate desync */
    do {
      sdl_sound.current_emulated_samples -= len;
    } while(sdl_sound.current_emulated_samples > 2 * len);
    memcpy(sdl_sound.buffer,
           sdl_sound.current_pos - sdl_sound.current_emulated_samples,
           sdl_sound.current_emulated_samples);
    sdl_sound.current_pos = sdl_sound.buffer + sdl_sound.current_emulated_samples;
  }
}

static int sdlsms_sound_init()
{
  int n;
  SDL_AudioSpec as_desired, as_obtained;
  
  if(SDL_Init(SDL_INIT_AUDIO) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }

  as_desired.freq = SOUND_FREQUENCY;
  as_desired.format = AUDIO_S16LSB;
  as_desired.channels = 2;
  as_desired.samples = SOUND_SAMPLES_SIZE;
  as_desired.callback = sdlsms_sound_callback;

  if(SDL_OpenAudio(&as_desired, &as_obtained) == -1) {
    printf("ERROR: can't open audio: %s.\n", SDL_GetError());
    return 0;
  }

  if(as_desired.samples != as_obtained.samples) {
    printf("ERROR: soundcard driver does not accept specified samples size.\n");
    return 0;
  }

  sdl_sound.current_emulated_samples = 0;
  n = SOUND_SAMPLES_SIZE * 2 * sizeof(short) *  11;
  sdl_sound.buffer = (char*)malloc(n);
  if(!sdl_sound.buffer) {
    printf("ERROR: can't allocate memory for sound.\n");
    return 0;
  }
  memset(sdl_sound.buffer, 0, n);
  sdl_sound.current_pos = sdl_sound.buffer;
  return 1;
}

static void sdlsms_sound_update()
{
  int i;
  short* p;

  SDL_LockAudio();
  p = (short*)sdl_sound.current_pos;
  for(i = 0; i < snd.bufsize; ++i) {
      *p = snd.buffer[0][i];
      ++p;
      *p = snd.buffer[1][i];
      ++p;
  }
  sdl_sound.current_pos = (char*)p;
  sdl_sound.current_emulated_samples += snd.bufsize * 2 * sizeof(short);
  SDL_UnlockAudio();
}

static void sdlsms_sound_close()
{
  SDL_PauseAudio(1);
  SDL_CloseAudio();
  if(sdl_sync.sem_sync)
    SDL_DestroySemaphore(sdl_sync.sem_sync);
  free(sdl_sound.buffer);
}


/* controls */

static void sdlsms_controls_init()
{
  sdl_controls.state_slot = 0;
  sdl_controls.pad[0].up = DINGOO_BUTTON_UP;
  sdl_controls.pad[0].down = DINGOO_BUTTON_DOWN;
  sdl_controls.pad[0].left = DINGOO_BUTTON_LEFT;
  sdl_controls.pad[0].right = DINGOO_BUTTON_RIGHT;
  sdl_controls.pad[0].b1 = DINGOO_BUTTON_A;
  sdl_controls.pad[0].b2 = DINGOO_BUTTON_B;
  sdl_controls.pad[0].start = DINGOO_BUTTON_START;
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

static int sdlsms_controls_update(SDLKey k, int p)
{
  if(!p) {
	  switch(k) {
    case SDLK_2:
    case SDLK_3:
    case SDLK_4:
    case SDLK_5:
    case SDLK_6:
    case SDLK_7:
    case SDLK_8:
      /* only 'hot change' if started with a filter *//*
      if(sdl_video.current_filter != -1) {
        int cur = k - SDLK_1;
        if(cur >= 0 && cur < FILTER_NUM) {
          SDL_FillRect(sdl_video.surf_filter, NULL, 0);
          sdl_video.current_filter = cur;
        }
      }
      break;
*/
	  case DINGOO_BUTTON_Y:
		  sdlsms_video_take_screenshot();
		  break;
	  case  DINGOO_BUTTON_L:
		  if(save_state(rom_filename, sdl_controls.state_slot))
  		  printf("[INFO] Saved state to slot #%d.\n", sdl_controls.state_slot);
		  break;
	  case  DINGOO_BUTTON_R:
		  if(load_state(rom_filename, sdl_controls.state_slot))
		    printf("[INFO] Loaded state from slot #%d.\n", sdl_controls.state_slot);
		  break;
	  case SDLK_RCTRL: //menu is now using SDKL_HOME
			menu();
			break;
    case SDLK_F4:
    case SDLK_F5:
      sdl_video.frame_skip += k == SDLK_F4 ? -1 : 1;
      if(sdl_video.frame_skip > 0) 
        sdl_video.frame_skip = 1;
      if(sdl_video.frame_skip == 1)
        printf("[INFO] Frame skip disabled.\n");
      else
        printf("[INFO] Frame skip set to %d.\n", sdl_video.frame_skip);
      break;
    case SDLK_F6:
    case SDLK_F7:
      sdl_controls.state_slot += k == SDLK_F6 ? -1 : 1;
      if(sdl_controls.state_slot < 0)
        sdl_controls.state_slot = 0;
		  printf("[INFO] Slot changed to #%d.\n", sdl_controls.state_slot);
      break;
    }
  }

  if(k == sdl_controls.pad[0].start) {
    if(p) input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
    else  input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
  }
  else if(k == sdl_controls.pad[0].up) {
    if(p) input.pad[0] |= INPUT_UP;
    else  input.pad[0] &= ~INPUT_UP;
  }
  else if(k == sdl_controls.pad[0].down) {
    if(p) input.pad[0] |= INPUT_DOWN;
    else  input.pad[0] &= ~INPUT_DOWN;
  }
  else if(k == sdl_controls.pad[0].left) {
    if(p) input.pad[0] |= INPUT_LEFT;
    else  input.pad[0] &= ~INPUT_LEFT;
  }
  else if(k == sdl_controls.pad[0].right) {
    if(p) input.pad[0] |= INPUT_RIGHT;
    else  input.pad[0] &= ~INPUT_RIGHT;
  }
  else if(k == sdl_controls.pad[0].b1) {
    if(p) input.pad[0] |= INPUT_BUTTON1;
    else  input.pad[0] &= ~INPUT_BUTTON1;
  }
  else if(k == sdl_controls.pad[0].b2) {
    if(p) input.pad[0] |= INPUT_BUTTON2;
    else  input.pad[0] &= ~INPUT_BUTTON2;
  }

  if(k == DINGOO_BUTTON_START)
    startpressed = p;

  if(k == DINGOO_BUTTON_SELECT)		
    selectpressed = p;
  return 1;
}



/* joystick */

static int smssdl_joystick_init()
{
  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }
  else {
    sdl_joystick.commit_range = 3276;
    sdl_joystick.xstatus = 1;
    sdl_joystick.ystatus = 1;
    sdl_joystick.number = 0;
    sdl_joystick.map_b1 = 1;
    sdl_joystick.map_b2 = 2;
    sdl_joystick.map_start = 4;
    sdl_joystick.joy = SDL_JoystickOpen(sdl_joystick.number);
    if(sdl_joystick.joy != NULL) {
      printf("Joystick (%s) has %d axes and %d buttons.\n", SDL_JoystickName(sdl_joystick.number), SDL_JoystickNumAxes(sdl_joystick.joy), SDL_JoystickNumButtons(sdl_joystick.joy));
      printf("Using button mapping I=%d II=%d START=%d.\n", sdl_joystick.map_b1, sdl_joystick.map_b2, sdl_joystick.map_start);
      SDL_JoystickEventState(SDL_ENABLE);
      return 1;
    }
    else {
      printf("ERROR: Could not open joystick: %s.\n", SDL_GetError());
      return 0;
    }
  }
}


static void smssdl_joystick_update(SDL_Event event)
{
	int axisval;
  switch(event.type) {
	case SDL_JOYAXISMOTION:
		switch(event.jaxis.axis) {
		case 0:	/* X axis */
			axisval = event.jaxis.value;

			if(axisval > sdl_joystick.commit_range) {
				if(sdl_joystick.xstatus == 2)
					break;
				if(sdl_joystick.xstatus == 0) {
					input.pad[0] &= ~INPUT_LEFT;
				}
				input.pad[0] |= INPUT_RIGHT;
				sdl_joystick.xstatus = 2;
				break;
			}

			if(axisval < -sdl_joystick.commit_range) {
				if(sdl_joystick.xstatus == 0)
					break;
				if(sdl_joystick.xstatus == 2) {
				  input.pad[0] &= ~INPUT_RIGHT;
				}
				input.pad[0] |= INPUT_LEFT;
				sdl_joystick.xstatus = 0;
				break;
			}

			/* axis is centered */
			if(sdl_joystick.xstatus == 2) {
				input.pad[0] &= ~INPUT_RIGHT;
			}
			if(sdl_joystick.xstatus == 0) {
				input.pad[0] &= ~INPUT_LEFT;
			}
			sdl_joystick.xstatus = 1;
			break;

		case 1:	/* Y axis */
			axisval = event.jaxis.value;

			if(axisval > sdl_joystick.commit_range) {
				if(sdl_joystick.ystatus == 2)
					break;
				if(sdl_joystick.ystatus == 0) {
					input.pad[0] &= ~INPUT_UP;
				}
				input.pad[0] |= INPUT_DOWN;
				sdl_joystick.ystatus = 2;
				break;
			}
			
      if(axisval < -sdl_joystick.commit_range) {
				if(sdl_joystick.ystatus == 0)
					break;
				if(sdl_joystick.ystatus == 2) {
					input.pad[0] &= ~INPUT_DOWN;
				}
				input.pad[0] |= INPUT_UP;
				sdl_joystick.ystatus = 0;
				break;
			}

			/* axis is centered */
			if(sdl_joystick.ystatus == 2) {
				input.pad[0] &= ~INPUT_DOWN;
			}
			if(sdl_joystick.ystatus == 0) {
				input.pad[0] &= ~INPUT_UP;
			}
			sdl_joystick.ystatus = 1;
			break;
		}
		break;

	case SDL_JOYBUTTONDOWN:
		if(event.jbutton.button == sdl_joystick.map_b1) {
			input.pad[0] |= INPUT_BUTTON1;
		}
    else if(event.jbutton.button == sdl_joystick.map_b2) {
			input.pad[0] |= INPUT_BUTTON2;
		}
    else if(event.jbutton.button == sdl_joystick.map_start) {
			input.system |= (IS_GG) ? INPUT_START : INPUT_PAUSE;
		}
		break;

	case SDL_JOYBUTTONUP:
		if(event.jbutton.button == sdl_joystick.map_b1) {
			input.pad[0] &= ~INPUT_BUTTON1;
		}
    else if (event.jbutton.button == sdl_joystick.map_b2) {
			input.pad[0] &= ~INPUT_BUTTON2;
		}
    else if (event.jbutton.button == sdl_joystick.map_start) {
			input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
		}
		break;
	}
}


/* sync */

static Uint32 sdlsms_sync_timer_callback(Uint32 interval)
{
  SDL_SemPost(sdl_sync.sem_sync);
  return interval;
}

static int sdlsms_sync_init(int fullspeed)
{
  if(SDL_InitSubSystem(SDL_INIT_TIMER|SDL_INIT_EVENTTHREAD) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }
  sdl_sync.ticks_starting = SDL_GetTicks();
  if(fullspeed)
    sdl_sync.sem_sync = NULL;
  else
    sdl_sync.sem_sync = SDL_CreateSemaphore(0);
  return 1;
}

static void sdlsms_sync_close()
{
  float playtime = (float)(SDL_GetTicks() - sdl_sync.ticks_starting) / (float)1000;
  float avgfps = (float)sdl_video.frames_rendered / playtime;
  printf("[INFO] Average FPS = %.2f (%d%%).\n", avgfps, (int)(avgfps * 100 / MACHINE_FPS)); 
  printf("[INFO] Play time = %.2f sec.\n", playtime);
}


/* globals */

int sdlsms_init(const t_config* pcfg)
{
  //sysmips(MIPS_FIXADE, 0,0,0);
  rom_filename = pcfg->game_name;
  if(load_rom(rom_filename) == 0) {
    printf("ERROR: can't load `%s'.\n", pcfg->game_name);
    return 0;
  }
  else printf("Loaded `%s'.\n", pcfg->game_name);

  printf("Initializing SDL... ");
  if(SDL_Init(0) < 0) {
    printf("ERROR: %s.\n", SDL_GetError());
    return 0;
  }
  printf("Ok.\n");

  printf("Initializing SDL TIMER SUBSYSTEM... ");
  if(!sdlsms_sync_init(pcfg->fullspeed))
    return 0;
  printf("Ok.\n");

  printf("Initializing SDL CONTROLS SUBSYSTEM... ");
  sdlsms_controls_init();
  printf("Ok.\n");

 /* if(pcfg->joystick) {
    printf("Initializing SDL JOYSTICK SUBSYSTEM... ");
    if(smssdl_joystick_init())
      printf("Ok.\n");
  }
*/
  printf("Initializing SDL VIDEO SUBSYSTEM... ");
  if(!sdlsms_video_init(pcfg->frameskip, pcfg->fullscreen, pcfg->filter))
    return 0;
  font = gfx_tex_load_tga_from_array(fontarray);
  bigfontwhite = gfx_tex_load_tga_from_array(bigfontwhitearray);
  bigfontred = gfx_tex_load_tga_from_array(bigfontredarray);
  printf("Ok.\n");
  if(!pcfg->nosound) {
    printf("Initializing SDL SOUND SUBSYSTEM... ");
    if(!sdlsms_sound_init())
      return 0;
    printf("Ok.\n");
  }

  /* set up the virtual console emulation */
  SDL_LockSurface(sdl_video.surf_bitmap);
  printf("Initializing virtual console emulation... ");
  sms.use_fm = pcfg->fm;
  sms.country = pcfg->japan ? TYPE_DOMESTIC : TYPE_OVERSEAS;
  sms.save = pcfg->usesram;
  memset(&bitmap, 0, sizeof(t_bitmap));
  bitmap.width  = SMS_SCREEN_WIDTH;
  bitmap.height = SMS_SCREEN_HEIGHT;
  bitmap.depth  = sdl_video.surf_bitmap->format->BitsPerPixel;
  bitmap.pitch  = sdl_video.surf_bitmap->pitch;
  bitmap.data   = (unsigned char*)sdl_video.surf_bitmap->pixels+ sdl_video.bitmap_offset;;// + ((320- SMS_SCREEN_WIDTH)) + (((240-SMS_SCREEN_HEIGHT)/2)*bitmap.pitch) ;
  system_init(pcfg->nosound ? 0 : SOUND_FREQUENCY);
  load_sram(pcfg->game_name);
  SDL_UnlockSurface(sdl_video.surf_bitmap);
  printf("Ok.\n");

  return 1;
}

void sdlsms_emulate()
{

  printf("Starting emulation...\n");

  if(snd.enabled)
    SDL_PauseAudio(0);

  if(sdl_sync.sem_sync) {
    SDL_SetTimer(50, sdlsms_sync_timer_callback);
  }

  while(!quit) {
    /* pump SDL events */
    SDL_Event event;
    if(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_KEYUP:
      case SDL_KEYDOWN:
        if(!sdlsms_controls_update(event.key.keysym.sym, event.type == SDL_KEYDOWN))
          quit = 1;
        break;
      case SDL_JOYAXISMOTION:
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
        smssdl_joystick_update(event);
        break;
      case SDL_QUIT:
        quit = 1;
        break;
      }
    }

/*    if(selectpressed && startpressed) //menu is now using SDKL_HOME
    {
	menu();
	input.system &= (IS_GG) ? ~INPUT_START : ~INPUT_PAUSE;
	//sdl_sync.ticks_starting	= SDL_GetTicks();
	selectpressed = 0;
	startpressed = 0;
	SDL_FillRect(sdl_video.surf_screen,NULL,0);
    }     
*/
    sdlsms_video_update();
    if(snd.enabled)
      sdlsms_sound_update();

    if(sdl_sync.sem_sync && sdl_video.frames_rendered % 3 == 0)
      SDL_SemWait(sdl_sync.sem_sync);
  }
}

void sdlsms_shutdown() 
{
  /* shutdown the virtual console emulation */
  printf("Shutting down virtual console emulation...\n");
  save_sram(rom_filename);
  system_reset();
  system_shutdown();

  printf("Shutting down SDL...\n");
  if(snd.enabled)
    sdlsms_sound_close();
  sdlsms_video_close();
  sdlsms_sync_close();
  if(font)
    SDL_FreeSurface(font);

  if(bigfontred)
    SDL_FreeSurface(bigfontred);

  if(bigfontwhite);
    SDL_FreeSurface(bigfontwhite);

  SDL_Quit();
}
