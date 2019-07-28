/* 
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
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/input.h>
#include <linux/soundcard.h>
#include <SDL.h>

#include "main.h"
#include "menu.h"
#include "plat.h"
#include "cspace.h"
#include "blit320.h"
#include "pl_gun_ts.h"
#include "plugin_lib.h"
#include "libpicofe/menu.h"
#include "libpicofe/plat.h"
#include "libpicofe/input.h"
#include "libpicofe/in_sdl.h"
#include "libpicofe/linux/in_evdev.h"
#include "../libpcsxcore/psxmem_map.h"

//#define USE_DEBE              1
#define FB_DEBE_SIZE          4096
#define FB_VRAM_SIZE          (320*240*2*2)

#define MIYOO_VIR_SET_MODE    _IOWR(0x100, 0, unsigned long)
#define MIYOO_FB0_PUT_OSD     _IOWR(0x100, 0, unsigned long)
#define MIYOO_FB0_SET_MODE    _IOWR(0x101, 0, unsigned long)
#define MIYOO_FB0_GET_VER     _IOWR(0x102, 0, unsigned long)
#define MIYOO_FB0_SET_FLIP    _IOWR(0x103, 0, unsigned long)

SDL_Surface* screen;
struct plat_target plat_target;

extern int in_enable_vibration;
static int motordev = -1;
static int memdev = -1;
static int fbdev = -1;
static int lcddev = -1;
#if defined(USE_DEBE)
static uint8_t *debe = NULL;
static uint32_t *debe_ctrl = NULL;
static uint32_t *debe_buff = NULL;
#endif
static void *fb_vaddrs[2];
static unsigned int fb_paddrs[2];

static unsigned short *psx_vram;
static int psx_step, psx_width, psx_height, psx_bpp;
static int psx_offset_x, psx_offset_y, psx_src_width, psx_src_height;
static int fb_offset_x, fb_offset_y;

static const struct in_default_bind in_sdl_defbinds[] = {
  { SDLK_UP,        IN_BINDTYPE_PLAYER12, DKEY_UP },
  { SDLK_DOWN,      IN_BINDTYPE_PLAYER12, DKEY_DOWN },
  { SDLK_LEFT,      IN_BINDTYPE_PLAYER12, DKEY_LEFT },
  { SDLK_RIGHT,     IN_BINDTYPE_PLAYER12, DKEY_RIGHT },
  { SDLK_LSHIFT,    IN_BINDTYPE_PLAYER12, DKEY_TRIANGLE },
  { SDLK_LCTRL,     IN_BINDTYPE_PLAYER12, DKEY_CROSS },
  { SDLK_LALT,      IN_BINDTYPE_PLAYER12, DKEY_CIRCLE },
  { SDLK_SPACE,     IN_BINDTYPE_PLAYER12, DKEY_SQUARE },
  { SDLK_RETURN,    IN_BINDTYPE_PLAYER12, DKEY_START },
  { SDLK_ESCAPE,    IN_BINDTYPE_PLAYER12, DKEY_SELECT },
  { SDLK_TAB,       IN_BINDTYPE_PLAYER12, DKEY_L1 },
  { SDLK_BACKSPACE, IN_BINDTYPE_PLAYER12, DKEY_R1 },
  { SDLK_e,         IN_BINDTYPE_PLAYER12, DKEY_L2 },
  { SDLK_t,         IN_BINDTYPE_PLAYER12, DKEY_R2 },
  { SDLK_RCTRL,     IN_BINDTYPE_EMU, SACTION_ENTER_MENU },
  { SDLK_F1,        IN_BINDTYPE_EMU, SACTION_SAVE_STATE },
  { SDLK_F2,        IN_BINDTYPE_EMU, SACTION_LOAD_STATE },
  { SDLK_F3,        IN_BINDTYPE_EMU, SACTION_PREV_SSLOT },
  { SDLK_F4,        IN_BINDTYPE_EMU, SACTION_NEXT_SSLOT },
  { SDLK_F5,        IN_BINDTYPE_EMU, SACTION_TOGGLE_FSKIP },
  { SDLK_F6,        IN_BINDTYPE_EMU, SACTION_SCREENSHOT },
  { SDLK_F7,        IN_BINDTYPE_EMU, SACTION_TOGGLE_FPS },
  { SDLK_F8,        IN_BINDTYPE_EMU, SACTION_SWITCH_DISPMODE },
  { SDLK_F11,       IN_BINDTYPE_EMU, SACTION_TOGGLE_FULLSCREEN },
  { SDLK_F12,       IN_BINDTYPE_EMU, SACTION_FAST_FORWARD },
  { 0, 0, 0 }
};

const struct menu_keymap in_sdl_key_map[] =
{
  { SDLK_UP,        PBTN_UP },
  { SDLK_DOWN,      PBTN_DOWN },
  { SDLK_LEFT,      PBTN_LEFT },
  { SDLK_RIGHT,     PBTN_RIGHT },
  { SDLK_LALT,      PBTN_MOK },
  { SDLK_LCTRL,     PBTN_MBACK },
  { SDLK_SPACE,     PBTN_MA2 },
  { SDLK_LSHIFT,    PBTN_MA3 },
  { SDLK_TAB,       PBTN_L },
  { SDLK_BACKSPACE, PBTN_R },
};

const struct menu_keymap in_sdl_joy_map[] =
{
  { SDLK_UP,    PBTN_UP },
  { SDLK_DOWN,  PBTN_DOWN },
  { SDLK_LEFT,  PBTN_LEFT },
  { SDLK_RIGHT, PBTN_RIGHT },
  { SDLK_WORLD_0, PBTN_MOK },
  { SDLK_WORLD_1, PBTN_MBACK },
  { SDLK_WORLD_2, PBTN_MA2 },
  { SDLK_WORLD_3, PBTN_MA3 },
};

static const struct in_pdata in_sdl_platform_data = {
  .defbinds  = in_sdl_defbinds,
  .key_map   = in_sdl_key_map,
  .kmap_size = sizeof(in_sdl_key_map) / sizeof(in_sdl_key_map[0]),
  .joy_map   = in_sdl_joy_map,
  .jmap_size = sizeof(in_sdl_joy_map) / sizeof(in_sdl_joy_map[0]),
};

static void *fb_flip(void)
{
  static int flip=0;

#if defined(USE_DEBE)
  if(debe){
    if(flip == 0){
      *debe_ctrl|= (1 << 8);
      *debe_ctrl&= ~(1 << 9);
    }
    else{
      *debe_ctrl&= ~(1 << 8);
      *debe_ctrl|= (1 << 9);
    }
    *debe_buff|= 1;
  }
  else
#endif
  {
    flip^=1;
    SDL_Flip(screen);
  }
	return fb_vaddrs[flip];
}

void plat_video_menu_enter(int is_rom_loaded)
{
	if (pl_vout_buf != NULL) {
		if (psx_bpp == 16)
			bgr555_to_rgb565(pl_vout_buf, pl_vout_buf, 320*240*2);
		else
			memset(pl_vout_buf, 0, 320*240*2);
	}
}

void plat_video_menu_begin(void)
{
	g_menuscreen_ptr = fb_flip();
	ioctl(lcddev, MIYOO_FB0_SET_MODE, 0);
}

void plat_video_menu_end(void)
{
	g_menuscreen_ptr = fb_flip();
}

void plat_video_menu_leave(void)
{
	if (psx_vram == NULL) {
		printf("%s, GPU plugin did not provide vram\n", __func__);
		exit(-1);
	}

	memset(g_menuscreen_ptr, 0, 320*240 * psx_bpp/8);
	g_menuscreen_ptr = fb_flip();
	memset(g_menuscreen_ptr, 0, 320*240 * psx_bpp/8);
}

void *plat_prepare_screenshot(int *w, int *h, int *bpp)
{
	bgr555_to_rgb565(pl_vout_buf, pl_vout_buf, 320*240*2);
	*w = 320;
	*h = 240;
	*bpp = psx_bpp;
	return pl_vout_buf;
}

void plat_minimize(void)
{
}

#define RGB24(R,G,B)  	(((((R) & 0xf8) << 8) | (((G) & 0xfc) << 3) | (((B) & 0xf8) >> 3)))

static inline uint16_t mask_filter(uint16_t *s)
{
  uint16_t c1 = s[0];
  uint16_t c2 = s[1];
  uint16_t r = ((c1 & 0x7c00) + (c2 & 0x7c00)) >> 1;
  uint16_t g = ((c1 & 0x03e0) + (c2 & 0x03e0)) >> 1;
  uint16_t b = ((c1 & 0x001f) + (c2 & 0x001f)) >> 1;
  return (r & 0x7c00) | (g & 0x3e0) | (b & 0x01f);
}

static inline void blit256_x15(uint16_t* dst, const void* src)
{
  asm volatile (
    "   .arm                          ;\n"
    "   .align 2                      ;\n"
    "a: .word  0x80008000             ;\n"
    "   mov     r0, %0                ;\n"
    "   mov     r1, %1                ;\n"
    "   mov     r10, #16              ;\n"
    "   ldr     r9, a                 ;\n"
    "0: ldmia   r1!, {r2-r8,lr}       ;\n"
    "   orr     r2, r9                ;\n"
    "   orr     r3, r9                ;\n"
    "   orr     r4, r9                ;\n"
    "   orr     r5, r9                ;\n"
    "   orr     r6, r9                ;\n"
    "   orr     r7, r9                ;\n"
    "   orr     r8, r9                ;\n"
    "   orr     lr, r9                ;\n"
    "   str     r2, [r0], #4          ;\n"
    "   lsr     r2, #16               ;\n"
    "   orr     r2, r3, lsl #16       ;\n"
    "   str     r2, [r0], #4          ;\n"
    "   lsr     r3, #16               ;\n"
    "   orr     r3, r4, lsl #16       ;\n"
    "   str     r3, [r0], #4          ;\n"
    "   lsr     r4, #16               ;\n"
    "   orr     r4, r4, lsl #16       ;\n"
    "   str     r4, [r0], #4          ;\n"
    "   str     r5, [r0], #4          ;\n"
    "   str     r6, [r0], #4          ;\n"
    "   lsr     r6, #16               ;\n"
    "   orr     r6, r7, lsl #16       ;\n"
    "   str     r6, [r0], #4          ;\n"
    "   lsr     r7, #16               ;\n"
    "   orr     r7, r8, lsl #16       ;\n"
    "   str     r7, [r0], #4          ;\n"
    "   lsr     r8, #16               ;\n"
    "   orr     r8, r8, lsl #16       ;\n"
    "   str     r8, [r0], #4          ;\n"
    "   subs    r10, #1               ;\n"
    "   str     lr, [r0], #4          ;\n"
    "   bgt     0b                    ;\n"
    :: "r"(dst), "r"(src) 
    : "cc", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "lr");
}

static inline void blit256_x24(uint16_t* dst, const void* src)
{
	uint32_t uCount=32;
  const uint8_t* src8 = (const uint8_t*)src;
  do {
    dst[ 0] = RGB24(src8[0], src8[ 1], src8[ 2] );
    dst[ 1] = RGB24(src8[3], src8[ 4], src8[ 5] );
    dst[ 2] = dst[1];
    dst[ 3] = RGB24(src8[6], src8[ 7], src8[ 8] );
    dst[ 4] = RGB24(src8[9], src8[10], src8[11] );

    dst[ 5] = RGB24(src8[12], src8[13], src8[14] );
    dst[ 6] = RGB24(src8[15], src8[16], src8[17] );
    dst[ 7] = dst[6];
    dst[ 8] = RGB24(src8[18], src8[19], src8[20] );
    dst[ 9] = RGB24(src8[21], src8[22], src8[23] );
    dst += 10;
    src8  += 24;
  } while (--uCount);
}

static inline void blit320_x15(uint16_t* dst, const void* src)
{
  asm volatile (
    "   .arm                          ;\n"
    "   .align 2                      ;\n"
    "c: .word  0x80008000             ;\n"
    "   mov     r0, %0                ;\n"
    "   mov     r1, %1                ;\n"
    "   mov     r10, #20              ;\n"
    "   ldr     r9, c                 ;\n"
    "0: ldmia   r1!, {r2-r8,lr}       ;\n"
    "   orr     r2, r9                ;\n"
    "   orr     r3, r9                ;\n"
    "   orr     r4, r9                ;\n"
    "   orr     r5, r9                ;\n"
    "   orr     r6, r9                ;\n"
    "   orr     r7, r9                ;\n"
    "   orr     r8, r9                ;\n"
    "   orr     lr, r9                ;\n"
    "   subs    r10, #1               ;\n"
    "   stmia   r0!, {r2-r8,lr}       ;\n"
    "   bgt     0b                    ;\n"
    :: "r"(dst), "r"(src) 
    : "cc", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "lr");
}

static inline void blit320_x24(uint16_t* dst, const void* src)
{
	uint32_t uCount=20;
  uint8_t* src8 = (uint8_t*)src;
  do {
    dst[ 0] = RGB24(src8[ 0], src8[ 1], src8[ 2] );
    dst[ 1] = RGB24(src8[ 3], src8[ 4], src8[ 5] );
    dst[ 2] = RGB24(src8[ 6], src8[ 7], src8[ 8] );
    dst[ 3] = RGB24(src8[ 9], src8[10], src8[11] );
    dst[ 4] = RGB24(src8[12], src8[13], src8[14] );
    dst[ 5] = RGB24(src8[15], src8[16], src8[17] );
    dst[ 6] = RGB24(src8[18], src8[19], src8[20] );
    dst[ 7] = RGB24(src8[21], src8[22], src8[23] );

    dst[ 8] = RGB24(src8[24], src8[25], src8[26] );
    dst[ 9] = RGB24(src8[27], src8[28], src8[29] );
    dst[10] = RGB24(src8[30], src8[31], src8[32] );
    dst[11] = RGB24(src8[33], src8[34], src8[35] );
    dst[12] = RGB24(src8[36], src8[37], src8[38] );
    dst[13] = RGB24(src8[39], src8[40], src8[41] );
    dst[14] = RGB24(src8[42], src8[43], src8[44] );
    dst[15] = RGB24(src8[45], src8[46], src8[47] );
    dst += 16;
    src8  += 48;
  } while (--uCount);
}

static inline void blit368_x15(uint16_t* dst, const void* src)
{
  asm volatile (
    "   .arm                          ;\n"
    "   .align 2                      ;\n"
    "   .macro ustr rl rt             ;\n"
    "     lsr   \\rl, #16             ;\n"
    "     orr   \\rl, \\rt, lsl #16   ;\n"
    "     orr   \\rl, r9              ;\n"
    "     str   \\rl, [r0], #4        ;\n"
    "   .endm                         ;\n"
    "e: .word  0x80008000             ;\n"
    "   mov     r0, %0                ;\n"
    "   mov     r1, %1                ;\n"
    "   mov     r10, #23              ;\n"
    "   ldr     r9, e                 ;\n"
    "0: ldmia   r1!, {r2-r8,lr}       ;\n"
    "   ustr    r2, r3                ;\n"
    "   ustr    r3, r4                ;\n"
    "   ustr    r4, r5                ;\n"
    "   subs    r10, #1               ;\n"
    "   orr     r6, r9                ;\n"
    "   orr     r7, r9                ;\n"
    "   orr     r8, r9                ;\n"
    "   orr     lr, r9                ;\n"
    "   stmia   r0!, {r6-r8,lr}       ;\n"
    "   bgt     0b                    ;\n"
    :: "r"(dst), "r"(src) 
    : "cc", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "lr");
}

static inline void blit368_x24(uint16_t* dst, const void* src)
{
	uint32_t uCount=20;
  const uint8_t* src8 = (const uint8_t*)src;
  do {
    dst[ 0] = RGB24(src8[ 0], src8[ 1], src8[ 2] );
    dst[ 1] = RGB24(src8[ 3], src8[ 4], src8[ 5] );
    dst[ 2] = RGB24(src8[ 6], src8[ 7], src8[ 8] );
    dst[ 3] = RGB24(src8[ 9], src8[10], src8[11] );
    dst[ 4] = RGB24(src8[12], src8[13], src8[14] );
    dst[ 5] = RGB24(src8[15], src8[16], src8[17] );
    dst[ 6] = RGB24(src8[18], src8[19], src8[20] );
    dst[ 7] = RGB24(src8[21], src8[22], src8[23] );

    dst[ 8] = RGB24(src8[27], src8[28], src8[29] );
    dst[ 9] = RGB24(src8[30], src8[31], src8[32] );
    dst[10] = RGB24(src8[33], src8[34], src8[35] );
    dst[11] = RGB24(src8[36], src8[37], src8[38] );
    dst[12] = RGB24(src8[39], src8[40], src8[41] );
    dst[13] = RGB24(src8[42], src8[43], src8[44] );
    dst[14] = RGB24(src8[45], src8[46], src8[47] );
    dst[15] = RGB24(src8[48], src8[49], src8[50] );
    dst += 16;
    src8  += 54;
  } while (--uCount);
}

static inline void blit384_x15(uint16_t* dst, const void* src)
{
	uint32_t uCount=32;
  uint16_t* src16 = (uint16_t*)src;
  do {
    dst[ 0] = src16[0] | 0x8000;
    dst[ 1] = src16[1] | 0x8000;
    dst[ 2] = src16[2] | 0x8000;
    dst[ 3] = src16[3] | 0x8000;
    dst[ 4] = mask_filter(&src16[4]) | 0x8000;
    dst[ 5] = src16[6] | 0x8000;
    dst[ 6] = src16[7] | 0x8000;
    dst[ 7] = src16[8] | 0x8000;
    dst[ 8] = src16[9] | 0x8000;
    dst[ 9] = mask_filter(&src16[10]) | 0x8000;
    dst += 10;
    src16 += 12;
  } while (--uCount);
}

static inline void blit384_x24(uint16_t* dst, const void* src)
{
	uint32_t uCount=32;
  const uint8_t* src8 = (const uint8_t*)src;
  do {
    dst[0] = RGB24(src8[ 0], src8[ 1], src8[ 2] );
    dst[1] = RGB24(src8[ 3], src8[ 4], src8[ 5] );
    dst[2] = RGB24(src8[ 6], src8[ 7], src8[ 8] );
    dst[3] = RGB24(src8[ 9], src8[10], src8[11] );
    dst[4] = RGB24(src8[12], src8[13], src8[14] );
    dst[5] = RGB24(src8[18], src8[19], src8[20] );
    dst[6] = RGB24(src8[21], src8[22], src8[23] );
    dst[7] = RGB24(src8[24], src8[25], src8[26] );
    dst[8] = RGB24(src8[27], src8[28], src8[29] );
    dst[9] = RGB24(src8[30], src8[31], src8[32] );
    dst += 10;
    src8  += 36;
  } while (--uCount);
}

static inline void blit512_x15(uint16_t* dst, const void* src)
{
  asm volatile (
    "   .arm                          ;\n"
    "   .align 2                      ;\n"
    "i: .word  0x80008000             ;\n"
    "   mov     r0, %0                ;\n"
    "   mov     r1, %1                ;\n"
    "   mov     r10, #32              ;\n"
    "   ldr     r9, i                 ;\n"
    "0: ldmia   r1!, {r2-r8,lr}       ;\n"
    "   lsl     r2, #16               ;\n"
    "   lsr     r2, #16               ;\n"
    "   orr     r2, r3, lsl #16       ;\n"
    "   orr     r2, r9                ;\n"
    "   str     r2, [r0], #4          ;\n"
    "   lsr     r4, #16               ;\n"
    "   lsr     r3, #16               ;\n"
    "   orr     r3, r4, lsl #16       ;\n"
    "   orr     r3, r9                ;\n"
    "   str     r3, [r0], #4          ;\n"
    "   lsr     r5, #16               ;\n"
    "   orr     r5, r6, lsl #16       ;\n"
    "   orr     r5, r9                ;\n"
    "   str     r5, [r0], #4          ;\n"
    "   lsr     r8, #16               ;\n"
    "   lsr     lr, #16               ;\n"
    "   orr     r7, r9                ;\n"
    "   str     r7, [r0], #4          ;\n"
    "   orr     r8, lr, lsl #16       ;\n"
    "   subs    r10, #1               ;\n"
    "   orr     r8, r9                ;\n"
    "   str     r8, [r0], #4          ;\n"
    "   bgt     0b                    ;\n"
    :: "r"(dst), "r"(src) 
    : "cc", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "lr");
}

static inline void blit512_x24(uint16_t* dst, const void* src)
{
	uint32_t uCount=32;
  const uint8_t* src8 = (const uint8_t*)src;
  do {
    dst[ 0] = RGB24(src8[ 0], src8[ 1], src8[ 2] );
    dst[ 1] = RGB24(src8[ 3], src8[ 4], src8[ 5] );
    dst[ 2] = RGB24(src8[ 9], src8[10], src8[11] );
    dst[ 3] = RGB24(src8[12], src8[13], src8[14] );
    dst[ 4] = RGB24(src8[18], src8[19], src8[20] );

    dst[ 5] = RGB24(src8[24], src8[25], src8[26] );
    dst[ 6] = RGB24(src8[27], src8[28], src8[29] );
    dst[ 7] = RGB24(src8[33], src8[34], src8[35] );
    dst[ 8] = RGB24(src8[36], src8[37], src8[38] );
    dst[ 9] = RGB24(src8[42], src8[43], src8[44] );

    dst += 10;
    src8  += 48;
  } while (--uCount);
}

static inline void blit640_x15(uint16_t* dst, const void* src)
{
  asm volatile (
    "   .arm                          ;\n"
    "   .align 2                      ;\n"
    "   .macro lhw_str rl rt          ;\n"
    "     lsl   \\rl, #16             ;\n"
    "     lsr   \\rl, #16             ;\n"
    "     orr   \\rl, \\rt, lsl #16   ;\n"
    "     orr   \\rl, r9              ;\n"
    "     str   \\rl, [r0], #4        ;\n"
    "   .endm                         ;\n"
    "k: .word  0x80008000             ;\n"
    "   mov     r0, %0                ;\n"
    "   mov     r1, %1                ;\n"
    "   mov     r10, #40              ;\n"
    "   ldr     r9, k                 ;\n"
    "0: ldmia   r1!, {r2-r8,lr}       ;\n"
    "   lhw_str r2, r3                ;\n"
    "   lhw_str r4, r5                ;\n"
    "   lhw_str r6, r7                ;\n"
    "   subs    r10, #1               ;\n"
    "   lhw_str r8, lr                ;\n"
    "   bgt     0b                    ;\n"
    :: "r"(dst), "r"(src) 
    : "cc", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "lr");
}

static inline void blit640_x24(uint16_t* dst, const void* src)
{
	uint32_t uCount=20;
  const uint8_t* src8 = (const uint8_t*) src;
  do {
    dst[ 0] = RGB24(src8[ 0], src8[ 1], src8[ 2] );
    dst[ 1] = RGB24(src8[ 6], src8[ 7], src8[ 8] );
    dst[ 2] = RGB24(src8[12], src8[13], src8[14] );
    dst[ 3] = RGB24(src8[18], src8[19], src8[20] );

    dst[ 4] = RGB24(src8[24], src8[25], src8[26] );
    dst[ 5] = RGB24(src8[30], src8[31], src8[32] );
    dst[ 6] = RGB24(src8[36], src8[37], src8[38] );
    dst[ 7] = RGB24(src8[42], src8[43], src8[44] );

    dst[ 8] = RGB24(src8[48], src8[49], src8[50] );
    dst[ 9] = RGB24(src8[54], src8[55], src8[56] );
    dst[10] = RGB24(src8[60], src8[61], src8[62] );
    dst[11] = RGB24(src8[66], src8[67], src8[68] );

    dst[12] = RGB24(src8[72], src8[73], src8[74] );
    dst[13] = RGB24(src8[78], src8[79], src8[80] );
    dst[14] = RGB24(src8[84], src8[85], src8[86] );
    dst[15] = RGB24(src8[90], src8[91], src8[92] );

    dst += 16;
    src8  += 96;
  } while(--uCount);
}

#define make_flip_func(name, blit)                                                      \
static void name(int doffs, const void *vram_, int w, int h, int sstride, int bgr24)    \
{                                                                                       \
  const uint16_t *vram = vram_;                                                         \
  uint16_t *dst = (uint16_t*)g_menuscreen_ptr + (fb_offset_y * 320);                    \
  uint32_t i, len = psx_step * 1024;                                                    \
                                                                                        \
  vram += psx_offset_y * 1024 + psx_offset_x;                                           \
  if(bgr24){                                                                            \
    for (i=psx_src_height; i>0; i--, vram+= len, dst+= 320) {                           \
      blit##_x24(dst, vram);                                                            \
    }                                                                                   \
  }                                                                                     \
  else{                                                                                 \
    for (i=psx_src_height; i>0; i--, vram+= len, dst+= 320) {                           \
      blit##_x15(dst, vram);                                                            \
    }                                                                                   \
  }                                                                                     \
}

make_flip_func(raw_blit_soft_256, blit256)
make_flip_func(raw_blit_soft_320, blit320)
make_flip_func(raw_blit_soft_368, blit368)
make_flip_func(raw_blit_soft_384, blit384)
make_flip_func(raw_blit_soft_512, blit512)
make_flip_func(raw_blit_soft_640, blit640)

void *plat_gvideo_set_mode(int *w_, int *h_, int *bpp_)
{
	int poff_w, poff_h, w_max;
	int w = *w_, h = *h_, bpp = *bpp_;

	if (!w || !h || !bpp)
		return NULL;

	printf("psx mode: %dx%d@%d\n", w, h, bpp);
	psx_width = w;
	psx_height = h;
	psx_bpp = bpp;

	switch (w + (bpp != 16) + !soft_scaling) {
	case 640:
		pl_plat_blit = raw_blit_soft_640;
		w_max = 640;
		break;
	case 512:
		pl_plat_blit = raw_blit_soft_512;
		w_max = 512;
		break;
	case 384:
		//pl_plat_blit = raw_blit_soft_384;
		//w_max = 384;
		//break;
	case 368:
		pl_plat_blit = raw_blit_soft_368;
		w_max = 368;
		break;
	case 256:
		pl_plat_blit = raw_blit_soft_256;
		w_max = 256;
		break;
	default:
		pl_plat_blit = raw_blit_soft_320;
		w_max = 320;
		break;
	}

	psx_step = 1;
	if (h > 256) {
		psx_step = 2;
		h /= 2;
	}

	poff_w = poff_h = 0;
	if (w > w_max) {
		poff_w = w / 2 - w_max / 2;
		w = w_max;
	}
	fb_offset_x = 0;
	if (w < 320)
		fb_offset_x = 320/2 - w / 2;
	if (h > 240) {
		poff_h = h / 2 - 240/2;
		h = 240;
	}
	fb_offset_y = 240/2 - h / 2;

	psx_offset_x = poff_w * psx_bpp/8 / 2;
	psx_offset_y = poff_h;
	psx_src_width = w;
	psx_src_height = h;

	if (fb_offset_x || fb_offset_y) {
		// not fullscreen, must clear borders
		memset(g_menuscreen_ptr, 0, 320*240 * psx_bpp/8);
		g_menuscreen_ptr = fb_flip();
		memset(g_menuscreen_ptr, 0, 320*240 * psx_bpp/8);
	}
	pl_set_gun_rect(fb_offset_x, fb_offset_y, w > 320 ? 320 : w, h);
	ioctl(lcddev, MIYOO_FB0_SET_MODE, (w_max << 16) | bpp);

	// adjust for hud
	*w_ = 320;
	*h_ = fb_offset_y + psx_src_height;
	return g_menuscreen_ptr;
}

/* not really used, we do raw_flip */
void plat_gvideo_open(int is_pal)
{
}

void *plat_gvideo_flip(void)
{
	g_menuscreen_ptr = fb_flip();
	return g_menuscreen_ptr;
}

void plat_gvideo_close(void)
{
}

static void *pl_emu_mmap(unsigned long addr, size_t size, int is_fixed, enum psxMapTag tag)
{
	void *retval;

	retval = plat_mmap(addr, size, 0, is_fixed);
	if (tag == MAP_TAG_VRAM) {
		psx_vram = retval;
  }
	return retval;
}

static void pl_emu_munmap(void *ptr, size_t size, enum psxMapTag tag)
{
  plat_munmap(ptr, size);
}

void plat_sdl_event_handler(void *event_)
{
}

void plat_init(void)
{
	const char *main_fb_name = "/dev/fb0";
	const char *main_lcd_name = "/dev/miyoo_fb0";
	struct fb_fix_screeninfo fbfix;
	int ret;

  if (SDL_WasInit(SDL_INIT_EVERYTHING)) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);
  }
  else {
    SDL_Init(SDL_INIT_VIDEO);
  }
  screen = SDL_SetVideoMode(320, 240, 16, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if (screen == NULL) {
		printf("%s, failed to set video mode\n", __func__);
		exit(-1); 
  }
  SDL_ShowCursor(0);
  system("echo 10 > /proc/sys/vm/swappiness");
  
  memdev = open("/dev/mem", O_RDWR);
  if(memdev < 0){
    printf("failed to open /dev/mem\n");
    exit(-1);
  }
#if defined(USE_DEBE)
  debe  = mmap(0, FB_DEBE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memdev, 0x01e60000);
  debe_ctrl = (uint32_t*)(debe + 0x800);
  debe_buff = (uint32_t*)(debe + 0x870);
  printf("debe remap address: 0x%x\n", (unsigned int)debe);
#endif

  lcddev = open(main_lcd_name, O_RDWR);
	if (lcddev == -1) {
		printf("%s, failed to open %s\n", __func__, main_fb_name);
		exit(-1);
	}
#if defined(USE_DEBE)
	ioctl(lcddev, MIYOO_FB0_SET_FLIP, 1);
#endif

	fbdev = open(main_fb_name, O_RDWR);
	if (fbdev == -1) {
		printf("%s, failed to open %s\n", __func__, main_fb_name);
		exit(-1);
	}

	ret = ioctl(fbdev, FBIOGET_FSCREENINFO, &fbfix);
	if (ret == -1) {
		printf("%s, failed to do ioctl(FBIOGET_FSCREENINFO)\n", __func__);
		exit(1);
	}

	printf("framebuffer: \"%s\" @ %08lx\n", fbfix.id, fbfix.smem_start);
	fb_paddrs[0] = fbfix.smem_start;
	fb_paddrs[1] = fb_paddrs[0] + 320*240*2;

	if (fb_vaddrs[0] == NULL) {
		fb_vaddrs[0] = mmap(0, FB_VRAM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memdev, fb_paddrs[0]);
		if (fb_vaddrs[0] == MAP_FAILED) {
			printf("%s, failed to do mmap(fb_vaddrs)\n", __func__);
			exit(-1);
		}
		memset(fb_vaddrs[0], 0, FB_VRAM_SIZE);
	}
	printf("buffer mapped @%p\n", fb_vaddrs[0]);

	fb_vaddrs[1] = (char*)fb_vaddrs[0] + 320*240*2;
	memset(fb_vaddrs[0], 0, FB_VRAM_SIZE);

	g_menuscreen_w = 320;
	g_menuscreen_h = 240;
	g_menuscreen_ptr = fb_flip();

  in_sdl_init(&in_sdl_platform_data, plat_sdl_event_handler);
  in_probe();

	pl_plat_blit = raw_blit_soft_320;
	psx_src_width = 320;
	psx_src_height = 240;
	psx_bpp = 16;

	pl_rearmed_cbs.screen_w = 320;
	pl_rearmed_cbs.screen_h = 240;
	psxMapHook = pl_emu_mmap;
	psxUnmapHook = pl_emu_munmap;
  in_enable_vibration = 1;
}

void plat_pre_finish(void)
{
  if(lcddev > 0){
#if defined(USE_DEBE)
	  ioctl(lcddev, MIYOO_FB0_SET_FLIP, 0);
#endif
	  ioctl(lcddev, MIYOO_FB0_SET_MODE, (320 << 16) | 16);
    close(lcddev);
  }
  if(motordev > 0){
    ioctl(motordev, MIYOO_VIR_SET_MODE, 1);
    close(motordev);
  }
#if defined(USE_DEBE)
	munmap(debe, FB_DEBE_SIZE);
#endif
  system("echo 60 > /proc/sys/vm/swappiness");
}

void plat_finish(void)
{
  SDL_Quit();
	memset(fb_vaddrs[0], 0, FB_VRAM_SIZE);
	munmap(fb_vaddrs[0], FB_VRAM_SIZE);
	close(fbdev);
  close(memdev);
}

void plat_trigger_vibrate(int pad, int low, int high)
{
  if(motordev == -1){
    motordev = open("/dev/miyoo_vir", O_RDWR);
  }
  else{
    ioctl(motordev, MIYOO_VIR_SET_MODE, ((high > 0x80) ? 0 : 1));
  }
}

