//
// Created by littlehui on 2019/12/8.
//
// Z80 (Zed Eight-Ty) Interface
#include "burnint.h"
#include "zetdrzz.h"
#ifdef EMU_DRZ80

struct DrZ80 Doze;

static unsigned char * ppMemFetch[0x0100];
static unsigned char * ppMemFetchData[0x0100];
static unsigned char * ppMemRead[0x0100];
static unsigned char * ppMemWrite[0x0100];

#endif // EMU_DRZ80

static int nOpenedCPU = -1;
static int nCPUCount = 0;

unsigned char __fastcall ZetDummyReadHandler(unsigned short) { return 0; }
void __fastcall ZetDummyWriteHandler(unsigned short, unsigned char) { }
unsigned char __fastcall ZetDummyInHandler(unsigned short) { return 0; }
void __fastcall ZetDummyOutHandler(unsigned short, unsigned char) { }

static void Z80DebugCallback(unsigned short pc, unsigned int d)
{
    printf("z80 error at PC: 0x%08x   OpCodes: %08X\n", pc, d);
}

static unsigned int z80_rebasePC(unsigned short address)
{
    Doze.Z80PC_BASE	= (unsigned int) Doze.ppMemFetch[ address >> 8 ];
    Doze.Z80PC		= Doze.Z80PC_BASE + address;
    return Doze.Z80PC;
}

static void z80_write8(unsigned short a, unsigned char d)
{
    //printf("z80_write8(0x%04x, 0x%04x);\n", d, a);
    unsigned char * p = ppMemWrite[ a >> 8 ];
    if ( p )
        * (p + a) = d;
    else
        ; //Z80.WriteHandler(d, a);
}

static void z80_write16(unsigned short d,unsigned short a)
{
//	printf("z80_write16(0x%04x, 0x%04x);\n", a, d);
    unsigned char * p = ppMemWrite[ a >> 8 ];
    if ( p ) {
        p+=a;	*p = d & 0xff;
        p++;	*p = d >> 8;
    } else {
        printf("z80_write16(0x%04x, 0x%04x); error at pc: 0x%04x\n", d, a, Doze.Z80PC - Doze.Z80PC_BASE);
        //z80_write8( (unsigned char)(d & 0xFF), a );
        //z80_write8( (unsigned char)(d >> 8), a + 1 );
        //Doze.z80_write8 ( a, d & 0xff );
        //Doze.z80_write8 ( a + 1, d >> 8 );
    }
}

static unsigned char z80_read8(unsigned short a)
{
    //printf("z80_read8(0x%04x);  PC: %08x\n", a, Doze.Z80PC - Doze.Z80PC_BASE);
    unsigned char * p = ppMemRead[ a >> 8 ];
    if ( p )
        return *(p + a);
    else
        return 0; // Doze.ReadHandler(a);
}

static unsigned short z80_read16(unsigned short a)
{
//	printf("z80_read16(0x%04x);\n", a);

    unsigned short d = 0;
    unsigned char * p = ppMemRead[ a >> 8 ];
    if ( p ) {
        p+=a;	d = *p;
        p++;	d |= (*p) << 8;
    } else {
        printf("z80_read16(0x%04x); error at pc: 0x%04x\n", a, Doze.Z80PC - Doze.Z80PC_BASE);
        //return z80_read8(a) | (z80_read8(a+1) << 8) ;
    }
    return d;
}

static void z80_irq_callback(void)
{
    //printf("z80_irq_callback();\n");
    Doze.Z80_IRQ = 0x00;
}

void ZetSetReadHandler(unsigned char (__fastcall *pHandler)(unsigned short))
{
    //printf("ZetSetReadHandler(%p);\n", pHandler);
    Doze.z80_read8 = pHandler;
}

void ZetSetWriteHandler(void (__fastcall *pHandler)(unsigned short, unsigned char))
{
    //printf("ZetSetWriteHandler(%p);\n", pHandler);
    Doze.z80_write8 = pHandler;
}

void ZetSetInHandler(unsigned char (__fastcall *pHandler)(unsigned short))
{
    //printf("ZetSetInHandler(%p);\n", pHandler);
    Doze.z80_in = pHandler;
}

void ZetSetOutHandler(void (__fastcall *pHandler)(unsigned short, unsigned char))
{
    //printf("ZetSetOutHandler(%p);\n", pHandler);
    Doze.z80_out = pHandler;
}

void ZetNewFrame()
{
//	printf("ZetNewFrame();\n");
    Doze.nCyclesTotal = 0;
}

int ZetInit(int nCount)
{
    memset( &ppMemFetch, 0, 0x0400 );
    memset( &ppMemFetchData, 0, 0x0400 );
    memset( &ppMemRead, 0, 0x0400 );
    memset( &ppMemWrite, 0, 0x0400 );

    memset(&Doze, 0, sizeof(Doze)); //struct

    Doze.z80_in			= ZetDummyInHandler;
    Doze.z80_out		= ZetDummyOutHandler;
    Doze.z80_rebasePC	= z80_rebasePC;
    //Doze.z80_rebaseSP	= z80_rebaseSP;
    Doze.z80_read8		= z80_read8;
    Doze.z80_read16		= z80_read16;
    Doze.z80_write8		= z80_write8;
    Doze.z80_write16	= z80_write16;
    Doze.z80_irq_callback=z80_irq_callback;

    Doze.ppMemFetch		= &ppMemFetch[0];
    Doze.ppMemFetchData	= &ppMemFetchData[0];
    Doze.ppMemRead		= &ppMemRead[0];
    Doze.ppMemWrite 	= &ppMemWrite[0];

    Doze.debugCallback	= Z80DebugCallback;

    Doze.nInterruptLatch = -1;

    ZetOpen(0);

    nCPUCount = nCount;

    return 0;
}

void ZetClose()
{
    nOpenedCPU = -1;
}

int ZetOpen(int nCPU)
{
    nOpenedCPU = nCPU;

    return 0;
}


int ZetRun(int nCycles)
{
    if (nCycles <= 0) {
        return 0;
    }

    Doze.nCyclesTotal += nCycles;
    Doze.nCyclesSegment = nCycles;
    Doze.nCyclesLeft = nCycles;

    DozeRun();
    nCycles = Doze.nCyclesSegment - Doze.nCyclesLeft;

    Doze.nCyclesTotal -= Doze.nCyclesLeft;
    Doze.nCyclesLeft = 0;
    Doze.nCyclesSegment = 0;

    return nCycles;
}

void ZetRunAdjust(int nCycles)
{
    //printf("ZetRunAdjust(%d);\n", nCycles);

    if (nCycles < 0 && Doze.nCyclesLeft < -nCycles) {
        nCycles = 0;
    }

    Doze.nCyclesTotal += nCycles;
    Doze.nCyclesSegment += nCycles;
    Doze.nCyclesLeft += nCycles;
}

void ZetRunEnd()
{
//	printf("ZetRunEnd();\n");

    Doze.nCyclesTotal -= Doze.nCyclesLeft;
    Doze.nCyclesSegment -= Doze.nCyclesLeft;
    Doze.nCyclesLeft = 0;

}

// This function will make an area callback ZetRead/ZetWrite
int ZetMemCallback(int nStart, int nEnd, int nMode)
{
//	printf("ZetMemCallback(0x%04x, 0x%04x, %d);\n", nStart, nEnd, nMode);

    nStart >>= 8;
    nEnd += 0xff;
    nEnd >>= 8;

    // Leave the section out of the memory map, so the Doze* callback with be used
    for (int i = nStart; i < nEnd; i++) {
        switch (nMode) {
            case 0:
                Doze.ppMemRead[i] = NULL;
                break;
            case 1:
                Doze.ppMemWrite[i] = NULL;
                break;
            case 2:
                Doze.ppMemFetch[i] = NULL;
                break;
        }
    }

    return 0;
}

int ZetMemEnd()
{
    return 0;
}

void ZetExit()
{
    //memset(&Doze, 0, sizeof(Doze));
    nCPUCount = 0;
}

int ZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem)
{
    //printf("ZetMapArea(0x%04x, 0x%04x, %d, %p);\n", nStart, nEnd, nMode, Mem);

    int s = nStart >> 8;
    int e = (nEnd + 0xFF) >> 8;

    // Put this section in the memory map, giving the offset from Z80 memory to PC memory
    for (int i = s; i < e; i++) {
        switch (nMode) {
            case 0:
                Doze.ppMemRead[i] = Mem - nStart;
                break;
            case 1:
                Doze.ppMemWrite[i] = Mem - nStart;
                break;
            case 2:
                Doze.ppMemFetch[i] = Mem - nStart;
                Doze.ppMemFetchData[i] = Mem - nStart;
                break;
        }
    }

    return 0;
}

int ZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem01, unsigned char *Mem02)
{
    //printf("ZetMapArea(0x%04x, 0x%04x, %d, %p, %p);\n", nStart, nEnd, nMode, Mem01, Mem02);

    int s = nStart >> 8;
    int e = (nEnd + 0xFF) >> 8;

    if (nMode != 2) {
        return 1;
    }

    // Put this section in the memory map, giving the offset from Z80 memory to PC memory
    for (int i = s; i < e; i++) {
        Doze.ppMemFetch[i] = Mem01 - nStart;
        Doze.ppMemFetchData[i] = Mem02 - nStart;
    }

    return 0;
}

int ZetReset()
{
    //printf("ZetReset();\n");

    Doze.spare			= 0;
    Doze.z80irqvector	= 0;

    Doze.Z80A			= 0x00 <<24;
    Doze.Z80F			= (1<<2); /* set ZFlag */
    Doze.Z80BC			= 0x0000<<16;
    Doze.Z80DE			= 0x0000<<16;
    Doze.Z80HL			= 0x0000<<16;
    Doze.Z80A2			= 0x00<<24;
    Doze.Z80F2			= 1<<2;  /* set ZFlag */
    Doze.Z80BC2			= 0x0000<<16;
    Doze.Z80DE2			= 0x0000<<16;
    Doze.Z80HL2			= 0x0000<<16;
    Doze.Z80IX			= 0xFFFF<<16;
    Doze.Z80IY			= 0xFFFF<<16;
    Doze.Z80I			= 0x00;
    Doze.Z80IM			= 0x00;
    Doze.Z80_IRQ		= 0x00;
    Doze.Z80IF			= 0x00;
    Doze.Z80PC			= Doze.z80_rebasePC(0);
    //Doze.Z80SP			= 0Doze.z80_rebaseSP(0xffff); /* 0xf000 */

    /*
     * reset SP to 0xF000
     */
    Doze.Z80SP			= 0xf000;

    //Doze.nInterruptLatch = 0;
    //Doze.nEI			= 0;
    //Doze.nCyclesLeft	= 0;
    //Doze.nCyclesTotal	= 0;
    //Doze.nCyclesSegment	= 0;

    return 0;
}

#ifdef EMU_DRZ80

int ZetNmi()
{
//	printf("ZetNmi();\n");

    //Doze.iff &= 0xFF00;						// reset iff1
    //DozeAsmCall((unsigned short)0x66);		// Do nmi
    Doze.Z80IF &= 0x02;						// reset iff1
    DozeAsmCall((unsigned short)0x66);		// Do nmi
    Doze.nCyclesTotal += 12;
    return 12;
}

#endif

int ZetPc(int n)
{
    //printf("ZetPc(%d);\n", n);
    return Doze.Z80PC - Doze.Z80PC_BASE;
}

int ZetScan(int nAction)
{
    //printf("ZetScan(%d);\n", nAction);

    if ((nAction & ACB_DRIVER_DATA) == 0) {
        return 0;
    }

    char szText[] = "Z80 #0";
    for (int i = 0; i < nCPUCount; i++) {
        szText[5] = '1' + i;
        ScanVar(&Doze, 19 * 4 + 16, szText);
    }

    return 0;
}
