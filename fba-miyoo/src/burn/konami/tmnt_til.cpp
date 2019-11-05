#include "tmnt.h"
// TMNT - Tiles
/////////////////////////////////
// All the code is pretty much stolen from DTMNT and MAME, then hacked till it worked. 
// Thanks to Dave for releasing the source to DMNT and the MAME team
//
// Todo:
// Black garbage on the title screen of tmnt 
// add support for zoom
// Rewrite the slow drawing stuff
// Add a bit more flexability to the code to allow other games to work. See WIP MIA driver.
// Fix the shadows
//
// -Kev

static unsigned int nPageBase[4]={0,0,0,0};
static unsigned short nScrollX[3]={12*8,0,0},nScrollY[3]={2*8,0,0};

void RawTileClip(unsigned int nTile,unsigned int nPal, int startx, int starty)
// Draw a background layer with a primitive form of clipping. A pretty slow and rubish function really :)
{
	unsigned int *ps; 
	int y;
	int x;
	int drawx, drawy;
	unsigned int s;  
	unsigned short *pPix;


	nPal<<=4; 
	nPal&=0x3f0; 

	nTile<<=5;  
	if (nTile>=0x300000-0x20) 
	{
		return;
	}

	ps=(unsigned int *)(TmntTile+nTile); 

	drawx = startx;
	drawy = starty;

	for (y=0;y<8; y++,ps++)
	{
		s=*ps;

		for (x=0; x<8; drawx++ ,x++,s<<=4)
		{
			pPix=pTransDraw + (nScreenWidth * drawy)+ drawx;

			unsigned char p;
			p=(unsigned char)((s>>28)&15);
			if (p)
			{
				if ((drawx>=0)&&(drawy>=0)&&(drawy<224)&&(drawx<304)) {
					*pPix=p+nPal;
				}
			}
		}
		drawx = startx;
		drawy++;
	}
}

void RawTile(unsigned int nTile,unsigned int nPal, int startx, int starty)
// Draw a background layer with a primitive form of clipping. A pretty slow and rubish function really :)
{
	unsigned int *ps; 
	int y;
	int x;
	int drawx, drawy;
	unsigned int s;  
	unsigned short *pPix;


	nPal<<=4; 
	nPal&=0x3f0; 

	nTile<<=5;  
	if (nTile>=0x300000-0x20) 
	{
		return;
	}

	ps=(unsigned int *)(TmntTile+nTile); 

	drawx = startx;
	drawy = starty;

	for (y=0;y<8; y++,ps++)
	{
		s=*ps;

		for (x=0; x<8; drawx++ ,x++,s<<=4)
		{
			pPix=pTransDraw + (nScreenWidth * drawy)+ drawx;

			unsigned char p;
			p=(unsigned char)((s>>28)&15);
			if (p)
			{
				*pPix=p+nPal;
			}
		}
		drawx = startx;
		drawy++;
	}
}






// Work out and draw the Fixed/A/B layers
void TmntTileLayer(int nLayer)
{	
	unsigned char PalBase[3]={0,0x20,0x28};
//	int ntile = 0;

	int sx,sy;

	int nTile;

	int nPal;	
	int page;


	for (int y=0; y<32; y++ )
	{
		for (int x=0; x<64; x++ )
		{	
			sx=(x+(nScrollX[nLayer]>>3))&63; 
			sy=(y+(nScrollY[nLayer]>>3))&31;
	
			nTile=*((unsigned short *)((TmntRam10+(nLayer<<13)) + (sy<<7)+(sx<<1)));
			// Get palette
			nPal=PalBase[nLayer] + ((nTile>>13)&7);		

			// Get tile page (0-3)
			page=(nTile>>10)&3; 
			nTile=((nTile&0x1000)>>2) | (nTile&0x03ff);
			if (page) 
			{
				nTile+=nPageBase[page];
			}
			int RenderXPos = ((x*8)-(nScrollX[nLayer]&7))&0x1ff;
			RenderXPos -=8;
			int RenderYPos = ((y*8)-(nScrollY[nLayer]&7))&0x1ff;

			if (nTile!=0x10) // blank tile?
			{
				//	Render8x8Tile_Clip(nTile, ((x-1)*8)-(xoff&7),(y*8)-(yoff&7),nPal,3,0,TmntTile);

				if (RenderXPos > 10 && RenderXPos < 290 && RenderYPos > 10 && RenderYPos < 210)
				{
					RawTile(nTile,nPal,RenderXPos,RenderYPos);
				}
				else
				{
					RawTileClip(nTile,nPal,RenderXPos,RenderYPos);
				}

			}
		}
	}
}

int K051960Draw(K051960Sprite sprite)
{
	unsigned short *temp;
	int x,y;
	int xpos,ypos;
	short TempPix;

	sprite.pal_offset<<=4; 
	sprite.pal_offset&=0x3f0; 

	if (sprite.zoomx == 0x10000 && sprite.zoomy == 0x10000)
	{

		switch (sprite.flip)
		{
		case 0: // no flip
			for (y=0;y<sprite.h;y++) 
			{
				for (x=0;x<sprite.w;x++)
				{
					xpos=sprite.x+x;
					ypos=sprite.y+y;
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if (TempPix) 
					{
						*temp=TempPix+sprite.pal_offset;
					}
				}
			}
			break;
		case 1: // flip x
			for (y=0;y<sprite.h;y++) 
			{
				ypos=sprite.y+y;
				for (x=0;x<(sprite.w);x++)
				{
					xpos=-x+(sprite.x+(sprite.w)-1);
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if (TempPix) 
					{
						*temp=TempPix+sprite.pal_offset;
					}
				}
			}
			break;
		case 2: // flip y
			for (y=sprite.h-1;y>=0;y--)
			{
				ypos=sprite.y+y;
				for (x=0;x<sprite.w;x++)
				{
					xpos=sprite.x+x;
					ypos=sprite.y+y;
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if (TempPix) 
					{
						*temp=TempPix+sprite.pal_offset;
					}
				}
			}
			break;
		case 3: // flipx flipy
			for (y=sprite.h-1;y>=0;y--)
			{
				ypos=sprite.y+y;
				for (x=0;x<(sprite.w);x++)
				{
					xpos=-x+(sprite.x+(sprite.w)-1);
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if (TempPix) 
					{
						*temp=TempPix+sprite.pal_offset;
					}
				}
			}
			break;
		}

	}
	return 0;
}



// Draw the selected sprite with clipping
int K051960DrawClip(K051960Sprite sprite)
{
	unsigned short *temp;
	int x,y;
	int xpos,ypos;
	short TempPix;

	sprite.pal_offset<<=4; 
	sprite.pal_offset&=0x3f0; 

	if (sprite.zoomx == 0x10000 && sprite.zoomy == 0x10000)
	{

		switch (sprite.flip)
		{
		case 0: // no flip
			for (y=0;y<sprite.h;y++) 
			{
				for (x=0;x<sprite.w;x++)
				{
					xpos=sprite.x+x;
					ypos=sprite.y+y;
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if ((xpos>=0)&&(ypos>=0)&&(ypos<224)&&(xpos<304)) 
					{
						if (TempPix) 
						{
							*temp=TempPix+sprite.pal_offset;

						}
					}
				}
			}
			break;
		case 1: // flip x
			for (y=0;y<sprite.h;y++) 
			{
				ypos=sprite.y+y;
				for (x=0;x<(sprite.w);x++)
				{
					xpos=-x+(sprite.x+(sprite.w)-1);
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if ((xpos>=0)&&(ypos>=0)&&(ypos<224)&&(xpos<304)) 
					{
						if (TempPix) 
						{
							*temp=TempPix+sprite.pal_offset;
						}
					}
				}
			}
			break;
		case 2: // flip y
			for (y=sprite.h-1;y>=0;y--)
			{
				ypos=sprite.y+y;
				for (x=0;x<sprite.w;x++)
				{
					xpos=sprite.x+x;
					ypos=sprite.y+y;
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if ((xpos>=0)&&(ypos>=0)&&(ypos<224)&&(xpos<304)) 
					{
						if (TempPix) 
						{
							*temp=TempPix+sprite.pal_offset;

						}
					}
				}
			}
			break;
		case 3: // flipx flipy
			for (y=sprite.h-1;y>=0;y--)
			{
				ypos=sprite.y+y;
				for (x=0;x<(sprite.w);x++)
				{
					xpos=-x+(sprite.x+(sprite.w)-1);
					temp=pTransDraw + (nScreenWidth * ypos)+(xpos);
					TempPix=TmntSprite[sprite.number^1];
					sprite.number++;
					if ((xpos>=0)&&(ypos>=0)&&(ypos<224)&&(xpos<304)) 
					{
						if (TempPix) 
						{
							*temp=TempPix+sprite.pal_offset;
						}
					}
				}
			}
			break;
		}

	}
	return 0;
}


void K051960Sprites()
{
	// Decodes a sprite and returns a sprite structure. Partally taken from MAME, DMNT and my imagination
	static int size_table[16]=
	{16,16,32,16,
	16,32,32,32,
	64,32,32,64,
	64,64,128,128};

	int sproff=0x000000;
	struct K051960Sprite sprite;
	int c=0,offset=0;

	for (int i=128;i>=0;i--)
	{
		c =0;
		offset = 0;
		sproff=i*8;

		if ((K051960Ram[(sproff+0)^1]&0x80)==0) 
			continue;   //sprite is not enabled

		sprite.x=(( 256* K051960Ram[(sproff+6)^1] + K051960Ram[(sproff+7)^1]) & 0x01ff)-110;
		sprite.y=240- (( 256* K051960Ram[(sproff+4)^1] + K051960Ram[(sproff+5)^1]) & 0x01ff);

		sprite.zoomx = (K051960Ram[(sproff+6)^1] & 0xfc) >> 2;
		sprite.zoomy = (K051960Ram[(sproff+4)^1] & 0xfc) >> 2;
		sprite.zoomx = 0x10000 / 128 * (128 - sprite.zoomx);
		sprite.zoomy = 0x10000 / 128 * (128 - sprite.zoomy);

		sprite.flip = 0;
		sprite.flip|=(K051960Ram[(sproff+6)^1]&2)?SPRITEFLIPX:0; 
		sprite.flip|=(K051960Ram[(sproff+4)^1]&2)?SPRITEFLIPY:0;  

		// c is the code for the sprite size
		c=(K051960Ram[(sproff+1)^1]>>4)&0xE;  
		sprite.w=size_table[c];
		sprite.h=size_table[c+1];

		offset=(K051960Ram[(sproff+1)^1]&0x1f); 
		offset<<=15;
		offset|=(K051960Ram[(sproff+2)^1]<<7);  

		if (((offset&0x00700)==0x00200)&&(c==0x0a))
		{
			offset+=0x200;
		}
		if (((offset&0x001e0)==0x00080)&&(c==0x04))
		{
			offset+=0x080;
		}
		offset*=2;

		if (K051960Ram[(sproff+3)^1]&0x10)
		{
			sprite.number=offset;
		}
		else
		{
			sprite.number=0x200000+offset;

		}
		sprite.pal_offset=(K051960Ram[(sproff+3)^1]&0x0F)+0x10;  

		if (sprite.number < 0x3FFFC0) // if not a valid sprite so save on the function call.
		{
			if (sprite.x > 0 && (sprite.x + sprite.w) < 304 && sprite.y >0 && (sprite.y  + sprite.h) < 220)
			{
				K051960Draw(sprite);
			}
			else
			{
				K051960DrawClip(sprite);
			}

		}
	}
}

#define TILEPAGEA ((unsigned char*)(TmntRam10))[0x6B01]
#define TILEPAGEB ((unsigned char*)(TmntRam10))[0x6E01]

int TmntTileDraw()
// This needs a lot of work but basically works out and draws all the graphics for TMNT
{
	// Find the tile page base for each page
	nPageBase[1]=(TILEPAGEA&0xf0)<<7;
	nPageBase[2]=(TILEPAGEB&0x0f)<<11; 
	nPageBase[3]=(TILEPAGEB&0xf0)<<7;


	// Get the scroll-x values
	nScrollX[1]=((TmntRam10[0x6402^1] << 8) | (TmntRam10[0x6400^1]  ))+96;
	nScrollX[2]=((TmntRam10[0x6403^1] <<8) |(TmntRam10[0x6401^1] ))+ 96;

	// Get the scroll-y values
	nScrollY[1]=TmntRam10[0x6018^1]+16;
	nScrollY[2]=TmntRam10[0x6019^1]+16;

	// Figure out which order to draw everything . . . .
	if (PriRam[0x00000]&0x04)	
	{
		TmntTileLayer(2);
		K051960Sprites();
		TmntTileLayer(1);
		TmntTileLayer(0);
	}
	else
	{
		TmntTileLayer(2);
		TmntTileLayer(1);
		K051960Sprites();
		TmntTileLayer(0);
	}
	return 0;
}

