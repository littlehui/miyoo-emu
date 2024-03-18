
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "../dingoo.h"
#include "../dingoo-video.h"

#include "../dface.h"

#include "font.h"
#ifdef SDL_TRIPLEBUF
#  define DINGOO_MULTIBUF SDL_TRIPLEBUF
#else
#  define DINGOO_MULTIBUF SDL_DOUBLEBUF
#endif
TTF_Font *sdl_ttf_font =NULL;
int sdl_high = 5;

typedef struct _letter
{
    int x, y, w, h;
    int pre, pos;
} Letter;

static int g_max_height = 13;
static SDL_Surface *g_font;
static Letter g_letters[] =
{
	/*" "*/ {80,214,5,13,0,0},
	/*"!"*/ {1,227,5,13,0,0},
	/*"""*/ {6,227,6,13,0,0},
	/*"#"*/ {12,227,8,13,0,0},
	/*"$"*/ {20,227,8,13,0,0},
	/*"%"*/ {28,227,7,13,0,0},
	/*"&"*/ {35,227,8,13,0,0},
	/*"'"*/ {43,227,3,13,0,0},
	/*"("*/ {46,227,5,13,0,0},
	/*")"*/ {51,227,5,13,0,0},
	/*"*"*/ {56,227,8,13,0,0},
	/*"+"*/ {64,227,7,13,0,0},
	/*","*/ {71,227,5,13,0,0},
	/*"-"*/ {76,227,7,13,0,0},
	/*"."*/ {83,227,5,13,0,0},
	/*"/"*/ {88,227,7,13,0,0},
	/*"0"*/ {1,214,8,13,0,0},
	/*"1"*/ {9,214,7,13,0,0},            
	/*"2"*/ {16,214,8,13,0,0},
	/*"3"*/ {24,214,8,13,0,0},
	/*"4"*/ {32,214,8,13,0,0},
	/*"5"*/ {40,214,8,13,0,0},
	/*"6"*/ {48,214,8,13,0,0},
	/*"7"*/ {56,214,8,13,0,0},
	/*"8"*/ {64,214,8,13,0,0},
	/*"9"*/ {72,214,8,13,0,0},
	/*":"*/ {95,227,5,13,0,0},
	/*";"*/ {100,227,5,13,0,0},
	/*"<"*/ {105,227,5,13,0,0},
	/*"="*/ {110,227,8,13,0,0},
	/*">"*/ {118,227,5,13,0,0},
	/*"?"*/ {123,227,8,13,0,0},
	/*"@"*/ {131,227,8,13,0,0},
	/*"A"*/ {1,188,8,13,0,0},
	/*"B"*/ {9,188,8,13,0,0},
	/*"C"*/ {17,188,8,13,0,0},
	/*"D"*/ {25,188,8,13,0,0},
	/*"E"*/ {33,188,8,13,0,0},
	/*"F"*/ {41,188,8,13,0,0},
	/*"G"*/ {49,188,8,13,0,0},
	/*"H"*/ {57,188,8,13,0,0},
	/*"I"*/ {65,188,5,13,0,0},
	/*"J"*/ {70,188,8,13,0,0},
	/*"K"*/ {78,188,8,13,0,0},
	/*"L"*/ {86,188,8,13,0,0},
	/*"M"*/ {94,188,8,13,0,0},
	/*"N"*/ {102,188,8,13,0,0},
	/*"O"*/ {110,188,8,13,0,0},
	/*"P"*/ {118,188,8,13,0,0},
	/*"Q"*/ {126,188,8,13,0,0},
	/*"R"*/ {134,188,8,13,0,0},
	/*"S"*/ {142,188,8,13,0,0},
	/*"T"*/ {150,188,7,13,0,0},
	/*"U"*/ {157,188,8,13,0,0},
	/*"V"*/ {165,188,8,13,0,0},
	/*"W"*/ {173,188,8,13,0,0},
	/*"X"*/ {181,188,8,13,0,0},
	/*"Y"*/ {189,188,7,13,0,0},
	/*"Z"*/ {196,188,8,13,0,0},
	/*"["*/ {139,227,5,13,0,0},
	/*"\"*/ {144,227,7,13,0,0},
	/*"]"*/ {151,227,5,13,0,0},
	/*"^"*/ {156,227,8,13,0,0},
	/*"_"*/ {164,227,8,13,0,0},
	/*"`"*/ {172,227,3,13,0,0},
	/*"a"*/ {1,201,8,13,0,0},
	/*"b"*/ {9,201,8,13,0,0},
	/*"c"*/ {17,201,8,13,0,0},
	/*"d"*/ {25,201,8,13,0,0},
	/*"e"*/ {33,201,8,13,0,0},
	/*"f"*/ {41,201,7,13,0,0},
	/*"g"*/ {48,201,8,13,0,0},
	/*"h"*/ {56,201,8,13,0,0},
	/*"i"*/ {64,201,3,13,0,0},
	/*"j"*/ {67,201,6,13,0,0},
	/*"k"*/ {73,201,7,13,0,0},
	/*"l"*/ {80,201,5,13,0,0},
	/*"m"*/ {85,201,9,13,0,0},
	/*"n"*/ {94,201,8,13,0,0},
	/*"o"*/ {102,201,8,13,0,0},
	/*"p"*/ {110,201,8,13,0,0},
	/*"q"*/ {118,201,8,13,0,0},
	/*"r"*/ {126,201,8,13,0,0},
	/*"s"*/ {134,201,8,13,0,0},
	/*"t"*/ {142,201,7,13,0,0},
	/*"u"*/ {149,201,8,13,0,0},
	/*"v"*/ {157,201,8,13,0,0},
	/*"w"*/ {165,201,9,13,0,0},
	/*"x"*/ {174,201,8,13,0,0},
	/*"y"*/ {182,201,8,13,0,0},
	/*"z"*/ {190,201,8,13,0,0},
	/*"{"*/ {175,227,7,13,0,0},
	/*"|"*/ {182,227,3,13,0,0},
	/*"}"*/ {185,227,7,13,0,0},
	/*"~"*/ {192,227,8,13,0,0},

	/*SELECTOR*/ {0,174,240,14,0,0},
	/*ROM*/ {132,12,36,12,0,0},
	/*SETTINGS*/ {132,0,65,12,0,0},
	/*NOPREVIEW*/ {145,25,54,8,0,0},
	/*LEFTARROW*/ {1,165,9,8,0,0},
	/*RIGHTARROW*/ {11,165,9,8,0,0},
	/*UPARROW*/ {23,164,8,9,0,0},
	/*DOWNARROW*/ {33,164,8,9,0,0},
	/*MAINSETTINGS*/ {132,70,94,12,0,0},
	/*VIDEOSETTINGS*/ {132,58,99,12,0,0},
	/*SOUNDSETTINGS*/ {132,46,103,12,0,0},
	/*BROWSER*/ {132,82,89,12,0,0},
	/*PREVIEWBLOCK*/ {1,55,99,105,0,0},
	/*LOGO*/ {119,155,119,18,0,0}
};

int InitFont()
{
	// Load font image
	g_font = SDL_LoadBMP("./sp.bmp");
	if(g_font == NULL) return -1;

	int color_key = SDL_MapRGB(g_font->format, 255, 0, 255);
	SDL_SetColorKey(g_font, SDL_SRCCOLORKEY, color_key);

	if( sdl_ttf_font == NULL ){
		TTF_Init();
		sdl_ttf_font = TTF_OpenFont("./SourceHanSans-Regular-04.ttf", 14);
		if(sdl_ttf_font == NULL){
			return -1;
		}
		//SDL_VideoModeOK not work od beta
        sdl_high = 0;

/*        if(SDL_VideoModeOK(640, 480, 16, SDL_HWSURFACE | DINGOO_MULTIBUF) != 0)
		{
			sdl_high = 0;
		}*/
	}
	return 0;
}

void KillFont()
{
	SDL_FreeSurface(g_font);
	TTF_CloseFont( sdl_ttf_font );
	TTF_Quit();
}

int DrawChar(SDL_Surface *dest, uint8 c, int x, int y)
{
	Letter *l = &g_letters[c];
	SDL_Rect src, dst;

	dst.x = x;
	dst.y = y;
	src.x = l->x;
	src.y = l->y;
	src.w = l->w;
	src.h = l->h;

	SDL_BlitSurface(g_font, &src, dest, &dst);

	return l->w + l->pos;
} 

void DrawText(SDL_Surface *dest, const char *textmsg, int x, int y)
{
	if( textmsg == NULL ) return;

	char *str = (char *)textmsg;
	int x0 = x, y0 = y; 
	for(; *str; str++) {
		if(*str == '\n')
			y0 += g_max_height;
		else {
			uint8 c = (*str - 32) & 0x7F;
			x0 += DrawChar(dest, c, x0, y0);

			if(x0 > 320) break;
		}
	}
}

void DrawText2(SDL_Surface *dest, const char *textmsg, int x, int y)
{
	if( textmsg == NULL ) return;
	SDL_Color textColor={255, 255, 255};//������ɫ
	SDL_Surface *message=NULL;
	message=TTF_RenderUTF8_Blended(sdl_ttf_font,textmsg, textColor );//���ڳ�����
	SDL_Rect dect;
	dect.x=x;
	dect.y=y-sdl_high;
	dect.h=0;
	dect.w=0;
	if (message != NULL){
		SDL_BlitSurface(message, NULL, dest, &dect);
		SDL_FreeSurface(message);
	}
}
