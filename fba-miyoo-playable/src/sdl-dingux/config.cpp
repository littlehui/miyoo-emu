// Burner Config file module
#include "SDL/SDL.h"

#include "burner.h"
int nIniVersion = 0;

CFG_OPTIONS options;
CFG_KEYMAP keymap;

// ---------------------------------------------------------------------------
// config file parsing

TCHAR* LabelCheck(TCHAR* s, TCHAR* pszLabel)
{
	INT32 nLen;
	if (s == NULL) {
		return NULL;
	}
	if (pszLabel == NULL) {
		return NULL;
	}
	nLen = _tcslen(pszLabel);

	SKIP_WS(s);													// Skip whitespace

	if (_tcsncmp(s, pszLabel, nLen)){							// Doesn't match
		return NULL;
	}
	return s + nLen;
}

// Read in the config file for the whole application
int ConfigAppLoad()
{
	char szConfig[MAX_PATH];
	char szLine[256];
	FILE *h;

	sprintf((char*)szConfig, "%s/%s", szAppHomePath, "fbasdl.ini");

	if ((h = fopen(szConfig,"rt")) == NULL) {
		return 1;
	}

	// Go through each line of the config file
	while (fgets(szLine, sizeof(szLine), h)) {
		int nLen;

		nLen = strlen(szLine);

		// Get rid of the linefeed at the end
		if (szLine[nLen - 1] == 10) {
			szLine[nLen - 1] = 0;
			nLen--;
		}

#define VAR(x) { char *szValue = LabelCheck(szLine,#x);		\
  if (szValue) x = strtol(szValue, NULL, 0); }
#define FLT(x) { char *szValue = LabelCheck(szLine,#x);		\
  if (szValue) x = atof(szValue); }
#define STR(x) { char *szValue = LabelCheck(szLine,#x " ");	\
  if (szValue) strcpy(x,szValue); }

		VAR(nIniVersion);

		// Other
		STR(szAppRomPaths[0]);
		STR(szAppRomPaths[1]);
		STR(szAppRomPaths[2]);
		STR(szAppRomPaths[3]);
		STR(szAppRomPaths[4]);
		STR(szAppRomPaths[5]);
		STR(szAppRomPaths[6]);
		STR(szAppRomPaths[7]);
		STR(szAppRomPaths[8]);
		STR(szAppRomPaths[9]);
		STR(szAppRomPaths[10]);
		STR(szAppRomPaths[11]);
		STR(szAppRomPaths[12]);
		STR(szAppRomPaths[13]);
		STR(szAppRomPaths[14]);
		STR(szAppRomPaths[15]);
		STR(szAppRomPaths[16]);
		STR(szAppRomPaths[17]);
		STR(szAppRomPaths[18]);
		STR(szAppRomPaths[19]);
#undef STR
#undef FLT
#undef VAR
	}

	fclose(h);
	return 0;
}

// Write out the config file for the whole application
int ConfigAppSave()
{
	char szConfig[MAX_PATH];
	FILE *h;

	sprintf((char*)szConfig, "%s/%s", szAppHomePath, "fbasdl.ini");

	if ((h = fopen(szConfig, "wt")) == NULL) {
		return 1;
	}


#define VAR(x) fprintf(h, #x " %d\n", x)
#define FLT(x) fprintf(h, #x " %f\n", x)
#define STR(x) fprintf(h, #x " %s\n", x)

	fprintf(h,"\n// The application version this file was saved from\n");
	// We can't use the macros for this!
	fprintf(h, "nIniVersion 0x%06X", nBurnVer);

	fprintf(h,"\n\n// The paths to search for rom zips. (include trailing backslash)\n");
	STR(szAppRomPaths[0]);
	STR(szAppRomPaths[1]);
	STR(szAppRomPaths[2]);
	STR(szAppRomPaths[3]);
	STR(szAppRomPaths[4]);
	STR(szAppRomPaths[5]);
	STR(szAppRomPaths[6]);
	STR(szAppRomPaths[7]);
	STR(szAppRomPaths[8]);
	STR(szAppRomPaths[9]);
	STR(szAppRomPaths[10]);
	STR(szAppRomPaths[11]);
	STR(szAppRomPaths[12]);
	STR(szAppRomPaths[13]);
	STR(szAppRomPaths[14]);
	STR(szAppRomPaths[15]);
	STR(szAppRomPaths[16]);
	STR(szAppRomPaths[17]);
	STR(szAppRomPaths[18]);
	STR(szAppRomPaths[19]);

	fprintf(h,"\n\n\n");

#undef STR
#undef FLT
#undef VAR

	fclose(h);
	return 0;
}

void ConfigGameDefault()
{
	// Initialize configuration options
	options.sound = 2;
	options.samplerate = 2;		// 0 - 11025, 1 - 16000, 2 - 22050, 3 - 32000
	options.vsync = 0;
	options.rescale = 0;		// no scaling by default
	options.rotate = 0;
	options.showfps = 0;
	options.frameskip = -1;		// auto frameskip by default
	options.create_lists = 0;
	options.m68kcore = 0;		// 0 - c68k, 1 - m68k, 2 - a68k
	options.z80core = 0;
	options.sense = 100;

	keymap.up = SDLK_UP;
	keymap.down = SDLK_DOWN;
	keymap.left = SDLK_LEFT;
	keymap.right = SDLK_RIGHT;
	keymap.fire1 = SDLK_LCTRL;	// A
	keymap.fire2 = SDLK_LALT;	// B
	keymap.fire3 = SDLK_SPACE;	// X
	keymap.fire4 = SDLK_LSHIFT;	// Y
	keymap.fire5 = SDLK_TAB;	// L
	keymap.fire6 = SDLK_BACKSPACE;	// R
	keymap.coin1 = SDLK_ESCAPE;	// SELECT
	keymap.start1 = SDLK_RETURN;	// START
	keymap.pause = SDLK_p;
	keymap.quit = SDLK_q;
	keymap.qsave = SDLK_s;		// quick save
	keymap.qload = SDLK_l;		// quick load
}

int ConfigGameLoad()
{
	FILE *f;
	char arg1[128];
	signed long argd;
	char line[256];
	char cfgname[MAX_PATH];

	sprintf((char*)cfgname, "%s/%s.cfg", szAppConfigPath, BurnDrvGetTextA(DRV_NAME));

	if(!(f = fopen(cfgname,"r"))) {
		// set default values and exit
		ConfigGameDefault();
		return 0;
	}

	while(fgets(line,sizeof(line),f) != NULL) {
		sscanf(line, "%s %d", &arg1, &argd);

		if(strcmp(arg1, "#") != 0) {
			if(strcmp(arg1, "FBA_SOUND") == 0) options.sound = argd;
			if(strcmp(arg1, "FBA_SAMPLERATE") == 0) options.samplerate = argd;
			if(strcmp(arg1, "FBA_VSYNC") == 0) options.vsync = argd;
			if(strcmp(arg1, "FBA_RESCALE") == 0) options.rescale = argd;
			if(strcmp(arg1, "FBA_ROTATE") == 0) options.rotate = argd;
			if(strcmp(arg1, "FBA_SHOWFPS") == 0) options.showfps = argd;
			if(strcmp(arg1, "FBA_FRAMESKIP") == 0) options.frameskip = argd;
			if(strcmp(arg1, "FBA_M68KCORE") == 0) options.m68kcore = argd;
			if(strcmp(arg1, "FBA_Z80CORE") == 0) options.z80core = argd;
			if(strcmp(arg1, "FBA_SENSE") == 0) options.sense = argd;

			if(strcmp(arg1, "KEY_UP") == 0) keymap.up = argd;
			if(strcmp(arg1, "KEY_DOWN") == 0) keymap.down = argd;
			if(strcmp(arg1, "KEY_LEFT") == 0) keymap.left = argd;
			if(strcmp(arg1, "KEY_COIN1") == 0) keymap.coin1 = argd;
			if(strcmp(arg1, "KEY_START1") == 0) keymap.start1 = argd;
			if(strcmp(arg1, "KEY_RIGHT") == 0) keymap.right = argd;
			if(strcmp(arg1, "KEY_FIRE1") == 0) keymap.fire1 = argd;
			if(strcmp(arg1, "KEY_FIRE2") == 0) keymap.fire2 = argd;
			if(strcmp(arg1, "KEY_FIRE3") == 0) keymap.fire3 = argd;
			if(strcmp(arg1, "KEY_FIRE4") == 0) keymap.fire4 = argd;
			if(strcmp(arg1, "KEY_FIRE5") == 0) keymap.fire5 = argd;
			if(strcmp(arg1, "KEY_FIRE6") == 0) keymap.fire6 = argd;
			if(strcmp(arg1, "KEY_QUIT") == 0) keymap.quit = argd;
			if(strcmp(arg1, "KEY_PAUSE") == 0) keymap.pause = argd;
		}
	}

	fclose(f);

	return 1;
}

int ConfigGameSave()
{
	FILE *fp;
	char cfgname[MAX_PATH];

	sprintf((char*)cfgname, "%s/%s.cfg", szAppConfigPath, BurnDrvGetTextA(DRV_NAME));
	fp = fopen(cfgname, "w");
	if(!fp) return 0;

	fprintf(fp, "# FBA config file\n");
	fprintf(fp, "# %s\n\n", BurnDrvGetTextA(DRV_NAME));

	fprintf(fp, "FBA_SOUND %d\n", options.sound);
	fprintf(fp, "FBA_SAMPLERATE %d\n", options.samplerate);
	fprintf(fp, "FBA_VSYNC %d\n", options.vsync);
	fprintf(fp, "FBA_RESCALE %d\n", options.rescale);
	fprintf(fp, "FBA_ROTATE %d\n", options.rotate);
	fprintf(fp, "FBA_SHOWFPS %d\n", options.showfps);
	fprintf(fp, "FBA_FRAMESKIP %d\n", options.frameskip);
	fprintf(fp, "FBA_M68KCORE %d\n", options.m68kcore);
	fprintf(fp, "FBA_Z80CORE %d\n", options.z80core);
	fprintf(fp, "FBA_SENSE %d\n", options.sense);

	fprintf(fp, "\n# Keys layout\n\n");
	fprintf(fp, "KEY_UP %d\n", keymap.up);
	fprintf(fp, "KEY_DOWN %d\n", keymap.down);
	fprintf(fp, "KEY_LEFT %d\n", keymap.left);
	fprintf(fp, "KEY_RIGHT %d\n", keymap.right);
	fprintf(fp, "KEY_COIN1 %d\n", keymap.coin1); 
	fprintf(fp, "KEY_START1 %d\n", keymap.start1);
	fprintf(fp, "KEY_FIRE1 %d\n", keymap.fire1);
	fprintf(fp, "KEY_FIRE2 %d\n", keymap.fire2);
	fprintf(fp, "KEY_FIRE3 %d\n", keymap.fire3);
	fprintf(fp, "KEY_FIRE4 %d\n", keymap.fire4);
	fprintf(fp, "KEY_FIRE5 %d\n", keymap.fire5);
	fprintf(fp, "KEY_FIRE6 %d\n", keymap.fire6);
	fprintf(fp, "KEY_QUIT %d\n", keymap.quit);
	fprintf(fp, "KEY_PAUSE %d\n", keymap.pause);
	fclose(fp);
	return 1;
}
