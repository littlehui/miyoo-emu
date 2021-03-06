
#include <stdlib.h>
#include <string.h>

//#include <windows.h>
#include "vez.h"

void (*ArmNecIrq)(struct ArmNec *, int);
int (*ArmNecRun)(struct ArmNec *);
unsigned int *ArmNecCryptTable;

static struct VezContext * VezCPUContext = 0;
static struct VezContext * VezCurrentCPU = 0;
static int nCPUCount = 0;
static int nOpenedCPU = -1;

static unsigned int VezDummyReadHandler(unsigned int) { return 0; }
static void VezDummyWriteHandler(unsigned int, unsigned int) { }

static unsigned int VezDummyReadWordHandler(unsigned int a) 
{ 
	unsigned int a1 = VezCurrentCPU->cpu.read8(a);
	unsigned int a2 = VezCurrentCPU->cpu.read8(a + 1);
	return a1 | (a2 << 8) ;
}

static void VezDummyWriteWordHandler(unsigned int a, unsigned int d) 
{ 
	VezCurrentCPU->cpu.write8(a, d & 0xff);
	VezCurrentCPU->cpu.write8(a + 1, d >> 8);
}

static unsigned int VezDummyReadPort(unsigned int) { return 0; }
static void VezDummyWritePort(unsigned int, unsigned int) { }

static unsigned int VezDummyCheckPC(unsigned int ps, unsigned int ip) 
{
	return VezCurrentCPU->cpu.mem_base + (ps << 4) + ip;
}

static int VezDummyUnrecognizedCallback(unsigned int a)
{
	a -= VezCurrentCPU->cpu.mem_base ;
	
//	printf("Unknown Opcode at %08x, Emu Exit!\n", a);
	
//	PostQuitMessage(0);
	return 0;
}


int VezInit(int nCount, unsigned int * typelist)
{
	VezCPUContext = (struct VezContext *) malloc ( nCount * sizeof(struct VezContext) );
	if (VezCPUContext == NULL) return 1;
	memset(VezCPUContext, 0, nCount * sizeof(struct VezContext) );
	for(int i=0; i<nCount; i++) {
		VezCPUContext[i].cpu_type = typelist[i];
		
		VezCPUContext[i].cpu.ReadMemMap = VezCPUContext[i].ppMemRead;
		VezCPUContext[i].cpu.WriteMemMap = VezCPUContext[i].ppMemWrite;
		
		VezCPUContext[i].cpu.read8 = VezDummyReadHandler;
		VezCPUContext[i].cpu.read16 = VezDummyReadWordHandler;
		
		VezCPUContext[i].cpu.write8 = VezDummyWriteHandler;
		VezCPUContext[i].cpu.write16 = VezDummyWriteWordHandler;
		
		VezCPUContext[i].cpu.in8 = VezDummyReadPort;
		VezCPUContext[i].cpu.out8 = VezDummyWritePort;
		
		VezCPUContext[i].cpu.checkpc = VezDummyCheckPC;
		VezCPUContext[i].cpu.UnrecognizedCallback =  VezDummyUnrecognizedCallback;
		
	}
	nCPUCount = nCount;
	return 0;
}

void VezExit()
{
	for(int j=0;j<nCPUCount; j++) {
		if (! VezCPUContext[j].pCryptTable ) continue;
		
		unsigned int *pct = ArmV33CryptTable;
		unsigned int cnt = 0;
		while ( pct[cnt] ) cnt++;
		
		for(int i=0; i<cnt; i++) {
			unsigned int *pdst = & VezCPUContext[j].pCryptTable[ 0x100 * i ];
			unsigned int *psrc = (unsigned int *) (pct[i]);
			// restore opcode jump table
			memcpy( psrc, pdst, 0x400 );
		}
		free(VezCPUContext[j].pCryptTable);
	}

	free(VezCPUContext);
	VezCPUContext = 0;
	VezCurrentCPU = 0;
	nCPUCount = 0;
	nOpenedCPU = -1;
}

int VezDecryptOpcode(unsigned char * tbl)
{
	//VezCurrentCPU->
	//unsigned int ArmV33CryptTable[]
	unsigned int *pct = ArmNecCryptTable;
	
	unsigned int cnt = 0;
	while ( pct[cnt] ) cnt++;
	
	VezCurrentCPU->pCryptTable = (unsigned int *) malloc ( 0x400 * cnt );
	if (! VezCurrentCPU->pCryptTable ) return 1;
	
	for(int i=0; i<cnt; i++) {
		unsigned int *pdst = &( VezCurrentCPU->pCryptTable[ 0x100 * i ] );
		unsigned int *psrc = (unsigned int *) (pct[i]);
		// backup opcode jump table
		memcpy( pdst, psrc, 0x400 );
		// decrypt it
		for(int j=0;j<256;j++)
			psrc[j] = pdst[ tbl[j] ];
	}
	
	return 0;
}

void VezNewFrame()
{
}

int VezOpen(int nCPU)
{
	nOpenedCPU = nCPU;
	VezCurrentCPU = VezCPUContext + nCPU;
	
	switch( VezCurrentCPU->cpu_type ) {
	case VEZ_NEC_V33:
		ArmNecIrq = ArmV33Irq;
		ArmNecRun = ArmV33Run;
		ArmNecCryptTable = ArmV33CryptTable;
		break;
	case VEZ_NEC_V30:
		ArmNecIrq = ArmV30Irq;
		ArmNecRun = ArmV30Run;
		ArmNecCryptTable = ArmV30CryptTable;
		break;
	}
	
	return 0;
}

void VezClose()
{
	nOpenedCPU = -1;
	VezCurrentCPU = 0;
}

int VezMemCallback(int nStart,int nEnd,int nMode)
{
	nStart >>= VEZ_MEM_SHIFT;
	nEnd += VEZ_MEM_MASK;
	nEnd >>= VEZ_MEM_SHIFT;

	for (int i = nStart; i < nEnd; i++) {
		if (nMode & VEZ_MEM_READ)	VezCurrentCPU->ppMemRead[i] = NULL;	
		if (nMode & VEZ_MEM_WRITE)	VezCurrentCPU->ppMemWrite[i] = NULL;
	}
	return 0;
}

int VezMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem)
{
	int s = nStart >> VEZ_MEM_SHIFT;
	int e = (nEnd + VEZ_MEM_MASK) >> VEZ_MEM_SHIFT;

	for (int i = s; i < e; i++) {
		if (nMode & VEZ_MEM_READ)	VezCurrentCPU->ppMemRead[i] = Mem - nStart;
		if (nMode & VEZ_MEM_WRITE)	VezCurrentCPU->ppMemWrite[i] = Mem - nStart;
	}
	return 0;
}

void VezSetReadHandler(unsigned int (*pHandler)(unsigned int))
{
	VezCurrentCPU->cpu.read8 = pHandler;
}

void VezSetWriteHandler(void (*pHandler)(unsigned int, unsigned int))
{
	VezCurrentCPU->cpu.write8 = pHandler;
}

void VezSetReadPort(unsigned int (*pHandler)(unsigned int))
{
	VezCurrentCPU->cpu.in8 = pHandler;
}

void VezSetWritePort(void (*pHandler)(unsigned int, unsigned int))
{
	VezCurrentCPU->cpu.out8 = pHandler;
}

int VezReset(unsigned int ps)
{
	//nec_reset( &(VezCurrentCPU->reg) );
	if ( VezCurrentCPU ) {
		memset( &(VezCurrentCPU->cpu), 0, 0x24 );
		
		VezCurrentCPU->cpu.sreg[1] = ps;		// 0xffff is default, currently for m92 system
		VezCurrentCPU->cpu.flags = 0x8000;			// MF
		
		VezCurrentCPU->cpu.mem_base = (unsigned int)VezCurrentCPU->ppMemRead[ 0 ];
		VezCurrentCPU->cpu.pc = VezCurrentCPU->cpu.mem_base + (VezCurrentCPU->cpu.sreg[1] << 4) + VezCurrentCPU->cpu.ip;
	}
	return 0;
}

int VezRun(int nCycles)
{
	VezCurrentCPU->cpu.cycles = nCycles;
	return nCycles - ArmNecRun( &(VezCurrentCPU->cpu) );
}

int VezPc(int n)
{
	return VezCPUContext[n].cpu.pc - VezCPUContext[n].cpu.mem_base;
}

struct ArmNec *VezCpu(int n)
{
	return &(VezCPUContext[n].cpu);
}

int VezScan(int nAction)
{
	return 0;
}

void VezSetIRQLine(const int line, const int status)
{
	if ( !status ) return ;
	if ( VezCurrentCPU ) {
		if (VezCurrentCPU->cpu.flags & 0x0200)	// check IF
			ArmNecIrq( &(VezCurrentCPU->cpu), line );
	}
}

