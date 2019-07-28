#include "mame_layer.h"
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>

#define PROT_READ     0
#define MAP_PRIVATE   0 
#define BLOCKSIZE     1024
#define MEMSIZE       0x8000000

static int fd = -1;
char swap_path[512] = "./";

void *my_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
  int fsize;
  char *p;

  fsize = lseek(fd, 0, SEEK_END);
  p = (char*)malloc(fsize);
  if(!p) {
    return (void *)(-1);
  }

  lseek(fd, 0, SEEK_SET );
  read(fd, p, fsize );
  return (void*)p;
}

int my_munmap(void *addr, size_t length)
{
  if(addr) {
    free(addr);
  }
  return 0;
}

/*
  fixed
  main
  mainbios
  sprites
  ym

audiocpu
audiocrypt
fixed
mainbios
maincpu
sprites
ym

*/

UINT8 *memory_region( GAME_ROMS *r, char *region ) {
	if (strcmp(region,"audiocpu")==0) return r->cpu_z80.p;
	if (strcmp(region,"audiocrypt")==0) return r->cpu_z80c.p;
	if (strcmp(region,"fixed")==0) return r->game_sfix.p;
	if (strcmp(region,"maincpu")==0) return r->cpu_m68k.p;
	if (strcmp(region,"mainbios")==0) return r->bios_m68k.p;
	if (strcmp(region,"sprites")==0) return r->tiles.p;
	if (strcmp(region,"ym")==0) return r->adpcma.p;
	printf("memory_region %s not found",region);
	
	return NULL;
}
UINT32 memory_region_length( GAME_ROMS *r, char *region ) {
	if (strcmp(region,"audiocpu")==0) return r->cpu_z80.size;
	if (strcmp(region,"audiocrypt")==0) return r->cpu_z80c.size;
	if (strcmp(region,"fixed")==0) return r->game_sfix.size;
	if (strcmp(region,"maincpu")==0) return r->cpu_m68k.size;
	if (strcmp(region,"mainbios")==0) return r->bios_m68k.size;
	if (strcmp(region,"sprites")==0) return r->tiles.size;
	if (strcmp(region,"ym")==0) return r->adpcma.size;
	printf("memory_region_length %s not found",region);
	
	return 0;
}

void *malloc_or_die(UINT32 b) {
#if 1
	void *a=malloc(b);
	if (a) {
    return a;
  }
	printf("Not enough memory :( exiting\n");
	exit(1);
	return NULL;
#else
  char *home = getenv("HOME");
  if(home) {
		sprintf(swap_path, "%s/.gngeo", home); 
  }
  else {
    sprintf(swap_path, "./.gngeo", home);
	}
  mkdir(swap_path, 0777);

  if(errno == EROFS || errno == EACCES || errno == EPERM) {
    getcwd(swap_path, 512);
    strcat(swap_path, "/.gngeo");
    mkdir(swap_path, 0777);
  }

  strcat(swap_path, "/gngeo.swp");
  fd = open(swap_path, O_CREAT | O_RDWR | O_SYNC);
  if(fd < 0) {
    printf("Error creating swap\n");
    exit(-1);
  }
  lseek(fd, MEMSIZE, SEEK_SET);
  write(fd, " ", 1);
  return my_mmap(0, MEMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
#endif
}

