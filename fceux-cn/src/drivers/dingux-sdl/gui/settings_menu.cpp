#include "../src/drivers/dingux-sdl/config.h"

typedef struct _setting_entry {
	const char *name;
	const char *info;
	const std::string option;
	void (*update)(unsigned long);
} SettingEntry;

#include "../src/drivers/dingux-sdl/gui/main_settings.cpp"
#include "../src/drivers/dingux-sdl/gui/video_settings.cpp"
#include "../src/drivers/dingux-sdl/gui/sound_settings.cpp"
#include "../src/drivers/dingux-sdl/gui/control_settings.cpp"
#include "../src/drivers/dingux-sdl/gui/overclock_settings.cpp"
#define SETTINGS_MENUSIZE 6

static int cmd_main_settings() {
	return RunMainSettings();
}

static int cmd_video_settings() {
	return RunVideoSettings();
}

static int cmd_sound_settings() {
	return RunSoundSettings();
}

static int cmd_control_settings() {
	return RunControlSettings();
}

static int cmd_oc_settings() {
	return RunOverclockSettings();
}

static int cmd_config_save() {
	extern Config *g_config;
	g_config->save();
	return 2;
}

static MenuEntry
	settings_menu[] = {
        { "主要选项", "更改主要选项", cmd_main_settings },
        { "视频设置", "更改视频设置", cmd_video_settings },
        { "音频设置", "更改音频设置", cmd_sound_settings },
        { "控制设置", "更改控制设置", cmd_control_settings },
		{ "游戏超频", "针对一些卡顿游戏进行超频，比如洛克人", cmd_oc_settings },
        { "保存设置-返回游戏","覆盖模拟器默认设置", cmd_config_save }};


int RunSettingsMenu() {
	static int index = 0;
	static int spy = 72;
	int done = 0, y, i;

	g_dirty = 1;
	while (!done) {
		// Parse input
		readkey();
		if (parsekey(DINGOO_B))
			done = 1;

		if (parsekey(DINGOO_UP, 1)) {
			if (index > 0) {
				index--;
				spy -= 16;
			} else {
				index = SETTINGS_MENUSIZE - 1;
				spy = 72 + 16*index;
			}
		}

		if (parsekey(DINGOO_DOWN, 1)) {
			if (index < SETTINGS_MENUSIZE - 1) {
				index++;
				spy += 16;
			} else {
				index = 0;
				spy = 72;
			}
		}

		if (parsekey(DINGOO_A)) {
			done = settings_menu[index].command();
		}

		// Must draw bg only when needed
		// Draw stuff
		if (g_dirty) {
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

            DrawText2(gui_screen, "设置", 8, 37);

			// Draw menu
			for (i = 0, y = 72; i < SETTINGS_MENUSIZE; i++, y += 16) {
				DrawText2(gui_screen, settings_menu[i].name, 60, y);
			}

			// Draw info
			DrawText2(gui_screen, settings_menu[index].info, 8, 225);

			g_dirty = 0;
		}

		SDL_Delay(16);

		// Update real screen
		FCEUGUI_Flip();
	}

	// Clear screen
	dingoo_clear_video();

	g_dirty = 1;
	if (done == 2) {
	    done == 0;
        return 1;
	} else {
	    return 0;
	}
}
