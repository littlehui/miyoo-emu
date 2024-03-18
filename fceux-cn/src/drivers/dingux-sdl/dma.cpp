#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "dma.h"

int dma_fd;
volatile uint16_t *dma_ptr = NULL;
const int dma_size = 320 * 480 * 2;

int dma_map_buffer(void)
{
	if (dma_ptr)
		return -1;
	dma_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (dma_fd < 0) {
		//printf("failed to open /dev/mem\n");
		return -1;
	}
	return dma_ptr = mmap(0, dma_size, PROT_READ | PROT_WRITE, MAP_SHARED, dma_fd, 0x4200000);
}

void dma_unmap_buffer(void)
{
	if (dma_ptr) {
		munmap(dma_ptr, dma_size);
		close(dma_fd);
	}
}