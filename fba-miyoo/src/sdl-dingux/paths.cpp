
// Paths module
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "burner.h"

char szAppHomePath[MAX_PATH] = "./.fba";			// ~/.fba
char szAppSavePath[MAX_PATH] = "./.fba/saves";		// ~/.fba/saves
char szAppConfigPath[MAX_PATH] = "./.fba/configs";	// ~/.fba/configs
char szAppHiscorePath[MAX_PATH] = "./.fba/hiscore";	// ~/.fba/highscore // for burn/hiscore.cpp
char szAppSamplesPath[MAX_PATH] = "./.fba/samples";	// ~/.fba/samples // for burn/snd/samples.cpp
char szAppPreviewPath[MAX_PATH] = "./.fba/previews";	// ~/.fba/previews
char szAppRomPaths[DIRS_MAX][MAX_PATH] = {{"./"}, {"/usr/local/share/roms/"}, {"roms/"}, };

void BurnPathsInit()
{
#ifndef WIN32
	char *home = getenv("HOME");
	if(home) sprintf(szAppHomePath, "%s/.fba", home);
	mkdir(szAppHomePath, 0777);
	if(!errno) {
		getcwd(szAppHomePath, MAX_PATH);
		strcat(szAppHomePath, "./.fba");
		mkdir(szAppHomePath, 0777);
	}

	sprintf(szAppSavePath, "%s/saves", szAppHomePath);
	mkdir(szAppSavePath, 0777);

	sprintf(szAppConfigPath, "%s/configs", szAppHomePath);
	mkdir(szAppConfigPath, 0777);

	sprintf(szAppPreviewPath, "%s/previews", szAppHomePath);
	mkdir(szAppPreviewPath, 0777);

	sprintf(szAppSamplesPath, "%s/samples", szAppHomePath);
	mkdir(szAppSamplesPath, 0777);
#else
	getcwd(szAppHomePath, MAX_PATH);
	strcat(szAppHomePath, "./.fba");
	mkdir(szAppHomePath);

	sprintf(szAppSavePath, "%s/saves", szAppHomePath);
	mkdir(szAppSavePath);

	sprintf(szAppConfigPath, "%s/configs", szAppHomePath);
	mkdir(szAppConfigPath);

	sprintf(szAppPreviewPath, "%s/previews", szAppHomePath);
	mkdir(szAppPreviewPath);

	sprintf(szAppSamplesPath, "%s/samples", szAppHomePath);
	mkdir(szAppSamplesPath);
#endif
}
