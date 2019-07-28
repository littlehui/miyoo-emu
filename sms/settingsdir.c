/* for asprintf */
#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <settingsdir.h>

#define APPNAME "sms_sdl"

/**
 * Gets the path for saving and loading settings.
 * The settings directory will be created if it doesn't already exist.
 * Returns NULL on failure.
 */
const char *getSettingsDir(void)
{
	static const char *cached = NULL;
	if (cached) {
		return cached;
	}

	char *home = getenv("HOME");
	if (!home) {
		return NULL;
	}

	char *path;
	if (asprintf(&path, "%s/.%s", home, APPNAME) == -1) {
		return NULL;
	}

	int res = mkdir(path, 0755);
	if (res == -1 && errno != EEXIST) {
		free(path);
		return NULL;
	}
	/* Note: EEXIST could mean a non-directory file by that name exists;
	 *       we could try to find out but there is nothing we can do about it
	 *       here anyway, so let's not.
	 */

	cached = path;
	return path;
}

/**
 * Gets the full path of a settings file with the given file name.
 * The settings directory will be created if it doesn't already exist.
 * Returns NULL on failure.
 * The caller is responsible for calling free() on the returned string when
 * it no longer needs it.
 */
const char *getSettingsFile(const char *name)
{
	const char *dir = getSettingsDir();
	if (!dir) {
		return NULL;
	}

	char *path;
	if (asprintf(&path, "%s/%s", dir, name) == -1) {
		return NULL;
	}
	return path;
}

int zomfg(int argc, char **argv)
{
	const char *settingsDir = getSettingsDir();
	if (settingsDir) {
		printf("Settings dir:  %s\n", settingsDir);
	} else {
		printf("Failed to create settings dir\n");
	}

	const char *settingsFile = getSettingsFile("sms_sdl.dat");
	printf("Settings file: %s\n", settingsFile);
	free((void *)settingsFile);

	return 0;
}
