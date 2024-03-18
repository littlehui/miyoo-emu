
// Externals
extern Config *g_config;

/* MENU COMMANDS */

// Fullscreen mode
extern char *scale_tag[] = {
        "Original",
        //"Hardware",
        "Aspect",
        "FS Sharp",
        "FS Smooth"
        //"Hardware X2",
        //"HWX2 CRT",
        //"HWX2 GRID",
        //"HWX2 CRT BRIGHT",
        //"HWX2 GRID BRIGHT"
};

static void fullscreen_update(unsigned long key)
{
    int val;
    g_config->getOption("SDL.Fullscreen", &val);

	if (key == DINGOO_RIGHT) val = val < 3 ? val+1 : 3;
	if (key == DINGOO_LEFT) val = val > 0 ? val-1 : 0;
   
	g_config->setOption("SDL.Fullscreen", val);
}

// Clip sides
static void clip_update(unsigned long key)
{
    int val, tmp;
    g_config->getOption("SDL.ClipSides", &tmp);

    if (key == DINGOO_RIGHT) val = 1;
    if (key == DINGOO_LEFT) val = 0;

    g_config->setOption("SDL.ClipSides", val);
}

// PPU emulation
static void newppu_update(unsigned long key)
{
    int val, tmp;
    g_config->getOption("SDL.NewPPU", &tmp);

    if (key == DINGOO_RIGHT) val = 1;
    if (key == DINGOO_LEFT) val = 0;

    g_config->setOption("SDL.NewPPU", val);
}

// NTSC TV's colors
static void ntsc_update(unsigned long key)
{
    int val;

    if (key == DINGOO_RIGHT) val = 1;
    if (key == DINGOO_LEFT) val = 0;

    g_config->setOption("SDL.NTSCpalette", val);
}

// NTSC Tint
static void tint_update(unsigned long key)
{
    int val;
    g_config->getOption("SDL.Tint", &val);

    if (key == DINGOO_RIGHT) val = val < 255 ? val+1 : 255;
    if (key == DINGOO_LEFT) val = val > 0 ? val-1 : 0;

    g_config->setOption("SDL.Tint", val);
}

// NTSC Hue
static void hue_update(unsigned long key)
{
    int val;
    g_config->getOption("SDL.Hue", &val);

    if (key == DINGOO_RIGHT) val = val < 255 ? val+1 : 255;
    if (key == DINGOO_LEFT) val = val > 0 ? val-1 : 0;

    g_config->setOption("SDL.Hue", val);
}

// Scanline start
static void slstart_update(unsigned long key)
{
    int val;
    g_config->getOption("SDL.ScanLineStart", &val);

    if (key == DINGOO_RIGHT) val = val < 239 ? val+1 : 239;
    if (key == DINGOO_LEFT) val = val > 0 ? val-1 : 0;

    g_config->setOption("SDL.ScanLineStart", val);
}

// Scanline end
static void slend_update(unsigned long key)
{
    int val;
    g_config->getOption("SDL.ScanLineEnd", &val);

    if (key == DINGOO_RIGHT) val = val < 239 ? val+1 : 239;
    if (key == DINGOO_LEFT) val = val > 0 ? val-1 : 0;

    g_config->setOption("SDL.ScanLineEnd", val);
}


/* VIDEO SETTINGS MENU */

static SettingEntry vd_menu[] =
        {
                {"视频分辨率", "设置视频扫描模式", "SDL.Fullscreen", fullscreen_update},
                {"裁减黑边", "裁减左右黑边", "SDL.ClipSides", clip_update},
                {"新物理运算器(PPU)", "是否使用新物理运算器模拟引擎", "SDL.NewPPU", newppu_update},
                {"NTSC色彩", "设置模拟器 NTSC(TV's)色彩", "SDL.NTSCpalette", ntsc_update},
                {"淡色(Tint)", "设置NTSC的淡色效果", "SDL.Tint", tint_update},
                {"色调(Hue)", "设置NTSC的色调", "SDL.Hue", hue_update},
                {"扫描线开始行", "The first drawn scanline", "SDL.ScanLineStart", slstart_update},
                {"扫描线结束行", "The last drawn scanline", "SDL.ScanLineEnd", slend_update},
        };

int RunVideoSettings()
{
    static int index = 0;
    static int spy = 72;
    int done = 0, y, i;

    char tmp[32];
    int  itmp;

    g_dirty = 1;
    while (!done) {
        // Parse input
        readkey();
        if (parsekey(DINGOO_B)) done = 1;
        if (parsekey(DINGOO_UP, 1)) {
            if (index > 0) {
                index--;
                spy -= 15;
            } else {
                index = 7;
                spy = 72 + 15*index;
            }
        }

        if (parsekey(DINGOO_DOWN, 1)) {
            if (index < 7) {
                index++;
                spy += 15;
            } else {
                index = 0;
                spy = 72;
            }
        }
        if (parsekey(DINGOO_RIGHT, 1) || parsekey(DINGOO_LEFT, 1))
            vd_menu[index].update(g_key);

        // Draw stuff
        if( g_dirty )
        {
            draw_bg(g_bg);

            //Draw Top and Bottom Bars
            DrawChar(gui_screen, SP_SELECTOR, 0, 37);
            DrawChar(gui_screen, SP_SELECTOR, 81, 37);
            DrawChar(gui_screen, SP_SELECTOR, 0, 225);
            DrawChar(gui_screen, SP_SELECTOR, 81, 225);
            DrawText2(gui_screen, "B键 - 返回", 235, 225);
            DrawChar(gui_screen, SP_LOGO, 12, 9);

            // Draw selector
            DrawChar(gui_screen, SP_SELECTOR, 56, spy);
            DrawChar(gui_screen, SP_SELECTOR, 77, spy);

            DrawText2(gui_screen, "视频 设置", 8, 37);

            // Draw menu
            for(i=0,y=72;i < 8;i++,y+=15) {
                DrawText2(gui_screen, vd_menu[i].name, 60, y);

                g_config->getOption(vd_menu[i].option, &itmp);
                if (!strncmp(vd_menu[i].name, "视频分辨率", 5)) {
                    sprintf(tmp, "%s", scale_tag[itmp]);
                }
                else if (!strncmp(vd_menu[i].name, "裁减黑边", 4) \
					|| !strncmp(vd_menu[i].name, "New PPU", 7)   \
					|| !strncmp(vd_menu[i].name, "NTSC 色彩", 5)) {
                    sprintf(tmp, "%s", itmp ? "on" : "off");
                }
                else sprintf(tmp, "%d", itmp);

                DrawText2(gui_screen, tmp, 210, y);
            }

            // Draw info
            DrawText2(gui_screen, vd_menu[index].info, 8, 225);

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
