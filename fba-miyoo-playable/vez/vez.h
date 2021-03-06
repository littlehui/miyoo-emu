

#ifndef _NEC_INTRF_H
#define _NEC_INTRF_H

#include "armnec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define	VEZ_NEC_V33		0
#define	VEZ_NEC_V30		8

#define VEZ_MEM_SHIFT	11
#define VEZ_MEM_MASK	((1 << VEZ_MEM_SHIFT) - 1)
#define VEZ_PAGE_COUNT	(1 << (20 - VEZ_MEM_SHIFT))

#define	VEZ_MEM_READ		1
#define	VEZ_MEM_WRITE		2
#define	VEZ_MEM_RAM			(VEZ_MEM_READ | VEZ_MEM_WRITE)
#define	VEZ_MEM_ROM			(VEZ_MEM_READ)

struct VezContext
{
	struct ArmNec cpu;
	
	unsigned int cpu_type;
	unsigned char * ppMemRead[VEZ_PAGE_COUNT];
	unsigned char * ppMemWrite[VEZ_PAGE_COUNT];
	unsigned int  * pCryptTable;
};

int VezInit(int nCount, unsigned int * typelist);
void VezExit();
void VezNewFrame();
int VezOpen(int nCPU);
void VezClose();

int VezDecryptOpcode(unsigned char * tbl);

int VezMemCallback(int nStart,int nEnd,int nMode);
int VezMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem);

void VezSetReadHandler(unsigned int (*)(unsigned int));
void VezSetWriteHandler(void (*)(unsigned int, unsigned int));
void VezSetReadPort(unsigned int (*)(unsigned int));
void VezSetWritePort(void (*)(unsigned int, unsigned int));

int VezReset(unsigned int ps);
int VezScan(int nAction);
int VezRun(int nCycles);

int VezPc(int n);
struct ArmNec *VezCpu(int n);

#define VEZ_IRQSTATUS_NONE 0
#define VEZ_IRQSTATUS_AUTO 1
#define VEZ_IRQSTATUS_ACK  2

void VezSetIRQLine(const int line, const int status);

#ifdef __cplusplus
}
#endif

#endif // _NEC_INTRF_H
