
// Externals
extern Config *g_config;

#define DEFAULT_STEP_VALUE	    10
#define SCANLINES_PER_FRAME	    262
#define MAX_POSTRENDER_SCANLINES    (SCANLINES_PER_FRAME*3)
#define MAX_VBLANK_SCANLINES	    (SCANLINES_PER_FRAME*3)

/* MENU COMMANDS */

// Overclock enabled
static void ocenabled_update(unsigned long key)
{
	int val;
	g_config->getOption("SDL.OverclockEnabled", &val);

	if (key == DINGOO_RIGHT) val = 1;
	if (key == DINGOO_LEFT) val = 0;

	g_config->setOption("SDL.OverclockEnabled", val);
}

// Skip 7bit overlocking
static void skip7bitoc_update(unsigned long key)
{
	int val;
	g_config->getOption("SDL.Skip7BitOverclocking", &val);

	if (key == DINGOO_RIGHT) val = 1;
	if (key == DINGOO_LEFT) val = 0;

	g_config->setOption("SDL.Skip7BitOverclocking", val);
}

// Vblank Scanlines
static void vblanksls_update(unsigned long key)
{
	int val;
	int step = g_keyState[DINGOO_A] ? 1 : g_keyState[DINGOO_Y] ? SCANLINES_PER_FRAME : DEFAULT_STEP_VALUE;

	g_config->getOption("SDL.VBlankScanlines", &val);

	if (key == DINGOO_RIGHT) val = val < (MAX_VBLANK_SCANLINES - step) ? val + step : MAX_VBLANK_SCANLINES;
	if (key == DINGOO_LEFT) val = val > (0 + step) ? val - step : 0;;

	g_config->setOption("SDL.VBlankScanlines", val);
}

// Post-render Scanlines
static void postrendersls_update(unsigned long key)
{
	int val;
	int step = g_keyState[DINGOO_A] ? 1 : g_keyState[DINGOO_Y] ? SCANLINES_PER_FRAME : DEFAULT_STEP_VALUE;

	g_config->getOption("SDL.PostRenderScanlines", &val);

	if (key == DINGOO_RIGHT) val = val < (MAX_POSTRENDER_SCANLINES - step) ? val + step : MAX_POSTRENDER_SCANLINES;
	if (key == DINGOO_LEFT) val = val > (0 + step) ? val - step : 0;;

	g_config->setOption("SDL.PostRenderScanlines", val);
}

/* TIMING SETTINGS MENU */
static SettingEntry oc_menu[] =
{
	{ "游戏超频开关", "打开或关闭游戏超频", "SDL.OverclockEnabled", ocenabled_update },
	{ "Skip 7bit overclocking", "Don't Overclock 7bit samples", "SDL.Skip7BitOverclocking", skip7bitoc_update },
	{ "Post-rend Scanlines", "# of post-render scanlines", "SDL.PostRenderScanlines", postrendersls_update },
	{ "VBlank Scanlines", "# of extra VBlank scanlines", "SDL.VBlankScanlines", vblanksls_update },
};

int RunOverclockSettings()
{
	static int index = 0;
	static int spy = 72;
	int done = 0, y, i;

	char tmp[32];
	int  itmp;

	static const int menu_size = sizeof(oc_menu) / sizeof(oc_menu[0]);
	static const int max_entries = 8;
	static int offset_start = 0;
	static int offset_end = menu_size > max_entries ? max_entries : menu_size;

	g_dirty = 1;
	while (!done) {
		// Parse input
		readkey();
		// get the keyboard input
		#if SDL_VERSION_ATLEAST(1, 3, 0)	 
		g_keyState = SDL_GetKeyboardState(NULL);	 
		#else
		g_keyState = SDL_GetKeyState(NULL);
		#endif
		if (parsekey(DINGOO_B)) done = 1;

		if (parsekey(DINGOO_UP, 1)) {
			if (index > 0) {
				index--;

				if (index >= offset_start)
					spy -= 15;

				if ((offset_start > 0) && (index < offset_start)) {
					offset_start--;
					offset_end--;
				}
			} else {
				index = menu_size-1;
				offset_end = menu_size;
				offset_start = menu_size <= max_entries ? 0 : offset_end - max_entries;
				spy = 72 + 15*(index - offset_start);
			}
		}

		if (parsekey(DINGOO_DOWN, 1)) {
			if (index < (menu_size - 1)) {
				index++;

				if (index < offset_end)
					spy += 15;

				if ((offset_end < menu_size) && (index >= offset_end)) {
					offset_end++;
					offset_start++;
				}
			} else {
				index = 0;
				offset_start = 0;
				offset_end = menu_size <= max_entries ? menu_size : max_entries;
				spy = 72;
			}
		}

		if (parsekey(DINGOO_RIGHT, 1) || parsekey(DINGOO_LEFT, 1)) {
			oc_menu[index].update(g_key);
		}

		// Draw stuff
		if( g_dirty )
		{
			draw_bg(g_bg);

			//Draw Top and Bottom Bars
			DrawChar(gui_screen, SP_SELECTOR, 0, 37);
			DrawChar(gui_screen, SP_SELECTOR, 81, 37);
			DrawChar(gui_screen, SP_SELECTOR, 0, 225);
			DrawChar(gui_screen, SP_SELECTOR, 81, 225);
			DrawText2(gui_screen, "B - 返回", 235, 225);
			DrawChar(gui_screen, SP_LOGO, 12, 9);

			// Draw selector
			DrawChar(gui_screen, SP_SELECTOR, 56, spy);
			DrawChar(gui_screen, SP_SELECTOR, 77, spy);

			DrawText2(gui_screen, "游戏超频设置", 8, 37);

			// Draw menu
			// for(i=0,y=72;i <= menu_size;i++,y+=15) {
			for (i = offset_start, y = 72; i < offset_end; i++, y += 15) {
				DrawText2(gui_screen, oc_menu[i].name, OPTION_LABEL_COLUMN, y);

				g_config->getOption(oc_menu[i].option, &itmp);
				if (!strncmp(oc_menu[i].name, "游戏超频开关", 17)) {
					sprintf(tmp, "%s", itmp ? "on" : "off");
				} else if(!strncmp(oc_menu[i].name, "Skip 7bit overclocking", 22)) {
					sprintf(tmp, "%s", itmp ? "on" : "off");
				}
				else sprintf(tmp, "%d", itmp);

				DrawText2(gui_screen, tmp, OPTION_VALUE_COLUMN, y);
			}

			// Draw info
			DrawText2(gui_screen, oc_menu[index].info, 8, 225);

			// Draw offset marks
			if (offset_start > 0)
				DrawChar(gui_screen, SP_UPARROW, 157, 62);
			if (offset_end < menu_size)
				DrawChar(gui_screen, SP_DOWNARROW, 157, 203);

			g_dirty = 0;
		}

		SDL_Delay(16);

		// Update real screen
		FCEUGUI_Flip();
	}

	// Clear screen
	dingoo_clear_video();

	g_dirty = 1;
	return 0;
}
