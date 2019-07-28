#include "tmnt.h"

static int K052109_memory_region;
static int K052109_gfxnum;
static void (*K052109_callback)(int tmap,int bank,int *code,int *color);
static unsigned char *K052109_ram;
static unsigned char *K052109_videoram_F,*K052109_videoram2_F,*K052109_colorram_F;
static unsigned char *K052109_videoram_A,*K052109_videoram2_A,*K052109_colorram_A;
static unsigned char *K052109_videoram_B,*K052109_videoram2_B,*K052109_colorram_B;
//static unsigned char K052109_charrombank[4];
static unsigned char has_extra_video_ram;
static int K052109_RMRD_line;
//static int K052109_tileflip_enable;
static unsigned char K052109_irq_enabled;
static int K052109_dx[3], K052109_dy[3];
//static unsigned char K052109_romsubbank,K052109_scrollctrl;
//tilemap *K052109_tilemap[3];

int K052109_is_IRQ_enabled(void)
{
	return K052109_irq_enabled;
}

void K052109_set_layer_offsets(int layer, int dx, int dy)
{
	K052109_dx[layer] = dx;
	K052109_dy[layer] = dy;
}

int K052109_vh_start(int gfx_memory_region,int /*plane0*/,int /*plane1*/,int /*plane2*/,int /*plane3*/,
		void (*callback)(int tmap,int bank,int *code,int *color))
{
	int gfx_index = 0, i;
/*	static gfx_layout charlayout =
	{
		8,8,
		0,				// filled in later 
		4,
		{ 0, 0, 0, 0 },	 filled in later 
		{ 0, 1, 2, 3, 4, 5, 6, 7 },
		{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
		32*8
	};

	for (gfx_index = 0; gfx_index < MAX_GFX_ELEMENTS; gfx_index++)
		if (Machine->gfx[gfx_index] == 0)
			break;
	if (gfx_index == MAX_GFX_ELEMENTS)
		return 1;
	
	charlayout.total = memory_region_length(gfx_memory_region) / 32;
	charlayout.planeoffset[0] = plane3 * 8;
	charlayout.planeoffset[1] = plane2 * 8;
	charlayout.planeoffset[2] = plane1 * 8;
	charlayout.planeoffset[3] = plane0 * 8;


	Machine->gfx[gfx_index] = allocgfx(&charlayout);
	if (!Machine->gfx[gfx_index])
		return 1;
	decodegfx(Machine->gfx[gfx_index], memory_region(gfx_memory_region), 0, Machine->gfx[gfx_index]->total_elements);

	if (Machine->drv->color_table_len)
	{
		Machine->gfx[gfx_index]->colortable = Machine->remapped_colortable;
		Machine->gfx[gfx_index]->total_colors = Machine->drv->color_table_len / 16;
	}
	else
	{
		Machine->gfx[gfx_index]->colortable = Machine->pens;
		Machine->gfx[gfx_index]->total_colors = Machine->drv->total_colors / 16;
	}
*/
	K052109_memory_region = gfx_memory_region;
	K052109_gfxnum = gfx_index;
	K052109_callback = callback;
	K052109_RMRD_line = CLEAR_LINE;
	K052109_irq_enabled = 0;

	has_extra_video_ram = 0;
/*
	K052109_tilemap[0] = tilemap_create(K052109_get_tile_info0,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);
	K052109_tilemap[1] = tilemap_create(K052109_get_tile_info1,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);
	K052109_tilemap[2] = tilemap_create(K052109_get_tile_info2,tilemap_scan_rows,TILEMAP_TRANSPARENT,8,8,64,32);
*/
	K052109_ram = TmntRam10;

	memset(K052109_ram,0,0x6000);

	K052109_colorram_F = &K052109_ram[0x0000];
	K052109_colorram_A = &K052109_ram[0x0800];
	K052109_colorram_B = &K052109_ram[0x1000];
	K052109_videoram_F = &K052109_ram[0x2000];
	K052109_videoram_A = &K052109_ram[0x2800];
	K052109_videoram_B = &K052109_ram[0x3000];
	K052109_videoram2_F = &K052109_ram[0x4000];
	K052109_videoram2_A = &K052109_ram[0x4800];
	K052109_videoram2_B = &K052109_ram[0x5000];

//	tilemap_set_transparent_pen(K052109_tilemap[0],0);
//	tilemap_set_transparent_pen(K052109_tilemap[1],0);
//	tilemap_set_transparent_pen(K052109_tilemap[2],0);

	for (i = 0; i < 3; i++)
	{
		K052109_dx[i] = K052109_dy[i] = 0;
	}

	return 0;
}

void K052109_set_RMRD_line(int state)
{
	K052109_RMRD_line = state;
}

