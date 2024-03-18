#ifndef _DMA_H_
#define _DMA_H_

#include <SDL/SDL.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


extern int dma_fd;
extern volatile uint16_t *dma_ptr;

typedef struct {
	uint64_t pfn : 54;
	unsigned int soft_dirty : 1;
	unsigned int file_page : 1;
	unsigned int swapped : 1;
	unsigned int present : 1;
} PagemapEntry;


int dma_map_buffer(void);
void dma_unmap_buffer(void);
#endif