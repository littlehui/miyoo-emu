// Externals
extern Config *g_config;

#define CONTROL_MENUSIZE 8

/* MENU COMMANDS */

int keyCheck(unsigned long key)
{
    if (key == DINGOO_A) return DefaultGamePad[0][1];
    if (key == DINGOO_B) return DefaultGamePad[0][0];
	if (key == DINGOO_Y) return DefaultGamePad[0][9];
	if (key == DINGOO_X) return DefaultGamePad[0][8];

	return -1;	
}

//!!! BUG LOCATED IN SDL LIBRARY GUI.CPP 
//!!! << configGamePadButton prefix creation SDL.Input.GamePad.%d missing "." at end >> 
//!!! FOLLOWING STRINGS MAY BE DEPRECATED IF FIXED IN FUTURE RELEASE
static void setA(unsigned long key)
{   	
	g_config->setOption("SDL.Input.GamePad.0A", keyCheck(key));
	UpdateInput(g_config);
}

static void setB(unsigned long key)
{   	
	g_config->setOption("SDL.Input.GamePad.0B", keyCheck(key));
	UpdateInput(g_config);
}

static void setTurboB(unsigned long key)
{   	
	g_config->setOption("SDL.Input.GamePad.0TurboB", keyCheck(key));
	UpdateInput(g_config);
}

static void setTurboA(unsigned long key)
{   	
	g_config->setOption("SDL.Input.GamePad.0TurboA", keyCheck(key));
	UpdateInput(g_config);
}

static void MergeControls(unsigned long key)
{
	int val;

	if (key == DINGOO_RIGHT)
		val = 1;
	if (key == DINGOO_LEFT)
		val = 0;

	g_config->setOption("SDL.MergeControls", val);
}

static void setAnalogStick(unsigned long key)
{
	int val;

	if (key == DINGOO_RIGHT)
		val = 1;
	if (key == DINGOO_LEFT)
		val = 0;

	g_config->setOption("SDL.AnalogStick", val);
}

static void setAutoFirePattern(unsigned long key)
{
	int val = 0;

	if (key == DINGOO_RIGHT && val < 2)
		++val;
	if (key == DINGOO_LEFT && val > 0)
		--val;

	g_config->setOption("SDL.AutoFirePattern", val);
}

static void resetMappings(unsigned long key)
{
	g_config->setOption("SDL.Input.GamePad.0A", DefaultGamePad[0][0]);
	g_config->setOption("SDL.Input.GamePad.0B", DefaultGamePad[0][1]);
	g_config->setOption("SDL.Input.GamePad.0TurboA", DefaultGamePad[0][8]);
	g_config->setOption("SDL.Input.GamePad.0TurboB", DefaultGamePad[0][9]);
	g_config->setOption("SDL.MergeControls", 0);
	g_config->setOption("SDL.AnalogStick", 0);
	g_config->setOption("SDL.AutoFirePattern", 0);
	UpdateInput(g_config);
}
/* CONTROL SETTING MENU */

static SettingEntry cm_menu[] = 
{
        {"按键 B", "按键 B 映射", "SDL.Input.GamePad.0B", setB},
        {"按键 A", "按键 A 映射", "SDL.Input.GamePad.0A", setA},
        {"连发 B", "连发 B 映射", "SDL.Input.GamePad.0TurboB", setTurboB},
        {"连发 A", "连发 A 映射", "SDL.Input.GamePad.0TurboA", setTurboA},
        {"合并 P1/P2", "双玩家控制合并，好像没什么用", "SDL.MergeControls", MergeControls},
        {"模拟摇杆", "是否开启左模拟摇杆", "SDL.AnalogStick", setAnalogStick},
        {"连发频率", "连发频率", "SDL.AutoFirePattern", setAutoFirePattern},
        {"重置按键", "重置默认按键配置", "", resetMappings},
};

int RunControlSettings()
{
	static int index = 0;
	static int spy = 72;
	int done = 0, y, i;
	int err = 1;
	int editMode = 0;

	g_dirty = 1;
	while (!done) {
		// Parse input
		readkey();
		if (parsekey(DINGOO_SELECT)) {
			if(index < 4) // Allow edit mode only for button mapping menu items
			{
				editMode = 1;
				DrawText(gui_screen, ">>", 185, spy);
				g_dirty = 0;
			}
		}

		if (!editMode) {
			if (parsekey(DINGOO_A)) {
				if (index > 3) {
					cm_menu[index].update(g_key);
				}
			}
			if (parsekey(DINGOO_B)) {
				//ERROR CHECKING
				int iBtn1 = -1;
				int iBtn2 = -1;
				err = 1;
				for ( int i = 0; i < 5; i++ ) {
					for ( int y = 0; y < 5; y++ ) {
						g_config->getOption(cm_menu[i].option, &iBtn1);
						if (i != y) {
							g_config->getOption(cm_menu[y].option, &iBtn2);
							if (iBtn1 == iBtn2) {
								err = 0;
								g_dirty = 1;
							}
							
						}
					}
				}

				done= err;
			}
		}	
		if ( !editMode ) {
	   		if (parsekey(DINGOO_UP, 1)) {
				if (index > 0) {
					index--; 
					spy -= 15;
				} else {
					index = CONTROL_MENUSIZE - 1;
					spy = 72 + 15*index;
				}
			}

			if (parsekey(DINGOO_DOWN, 1)) {
				if (index < CONTROL_MENUSIZE - 1) {
					index++;
					spy += 15;
				} else {
					index = 0;
					spy = 72;
				}
			}

	   		if (parsekey(DINGOO_LEFT, 1)) {
				if (index >= 4 && index <= 6) {
					cm_menu[index].update(g_key);
				}
			}

	   		if (parsekey(DINGOO_RIGHT, 1)) {
				if (index >= 4 && index <= 6) {
					cm_menu[index].update(g_key);
				}
			}
		}

		if ( editMode ) {
			if (parsekey(DINGOO_A, 0) || parsekey(DINGOO_B, 0) || parsekey(DINGOO_X, 0) || parsekey(DINGOO_Y, 0)) {
				cm_menu[index].update(g_key);
				g_dirty = 1;
				editMode = 0;
			}
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
            DrawText2(gui_screen, "B键 - 返回", 235, 225);
            DrawChar(gui_screen, SP_LOGO, 12, 9);
			
			// Draw selector
			DrawChar(gui_screen, SP_SELECTOR, 56, spy);
			DrawChar(gui_screen, SP_SELECTOR, 77, spy);
			if (err == 0) {
                DrawText2(gui_screen, "!!!错误 - 按键映射重复!!! ", 8, 37);
			} else {
                DrawText2(gui_screen, "设置操作按键 - select键 编辑", 8, 37);
			}

			// Draw menu
			for(i=0,y=72;i < CONTROL_MENUSIZE;i++,y+=15) {
				int iBtnVal = -1;
				char cBtn[32];
				int mergeValue;

				DrawText2(gui_screen, cm_menu[i].name, 60, y);
				
				g_config->getOption(cm_menu[i].option, &iBtnVal);
				
				if (i == CONTROL_MENUSIZE-1)
					cBtn[0] = 0;
				else if (i == CONTROL_MENUSIZE-2)
				{
					int value;
					const char *autofire_text[3] = {"1 on/1 off", "2 on/2 off", "1 on/3 off"};
					g_config->getOption("SDL.AutoFirePattern", &value);
					sprintf(cBtn, "%s", autofire_text[value]);
				}
				else if (i == CONTROL_MENUSIZE-3)
				{
					int value;
					g_config->getOption("SDL.AnalogStick", &value);
					sprintf(cBtn, "%s", value ? "on" : "off");
				}
				else if (i == CONTROL_MENUSIZE-4)
				{
					int mergeValue;
					g_config->getOption("SDL.MergeControls", &mergeValue);
					sprintf(cBtn, "%s", mergeValue ? "on" : "off");
				}
				else if (iBtnVal == DefaultGamePad[0][1])
					sprintf(cBtn, "%s", "A");
				else if (iBtnVal == DefaultGamePad[0][0])
					sprintf(cBtn, "%s", "B");
				else if (iBtnVal == DefaultGamePad[0][8])
					sprintf(cBtn, "%s", "Y");
				else if (iBtnVal == DefaultGamePad[0][9])
					sprintf(cBtn, "%s", "X");
				else
					sprintf(cBtn, "%s", "<empty>");


				DrawText2(gui_screen, cBtn, 210, y);
				
			}   

			// Draw info
			DrawText2(gui_screen, cm_menu[index].info, 8, 225);

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
