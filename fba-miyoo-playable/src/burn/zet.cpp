// Z80 (Zed Eight-Ty) Interface
#include "burnint.h"
#include "zet.h"

struct DrZ80 Drz80;
static unsigned char * ppMemFetch[0x0100];
static unsigned char * ppMemFetchData[0x0100];
static unsigned char * ppMemRead[0x0100];
static unsigned char * ppMemWrite[0x0100];



int nZetCpuCore = 0; // 0 - CZ80, 1 - MAME_Z80; 3;Drz80 not switchable on the fly, set before initializing!
static int nOpenedCPU = -1;
static int nCPUCount = 0;
int nHasZet = -1;

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
    Drz80.Z80PC_BASE	= (unsigned int) Drz80.ppMemFetch[ address >> 8 ];
    Drz80.Z80PC		= Drz80.Z80PC_BASE + address;
    return Drz80.Z80PC;
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
        printf("z80_write16(0x%04x, 0x%04x); error at pc: 0x%04x\n", d, a, Drz80.Z80PC - Drz80.Z80PC_BASE);
        //z80_write8( (unsigned char)(d & 0xFF), a );
        //z80_write8( (unsigned char)(d >> 8), a + 1 );
        //Drz80.z80_write8 ( a, d & 0xff );
        //Drz80.z80_write8 ( a + 1, d >> 8 );
    }
}

static unsigned char z80_read8(unsigned short a)
{
    //printf("z80_read8(0x%04x);  PC: %08x\n", a, Drz80.Z80PC - Drz80.Z80PC_BASE);
    unsigned char * p = ppMemRead[ a >> 8 ];
    if ( p )
        return *(p + a);
    else
        return 0; // Drz80.ReadHandler(a);
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
        printf("z80_read16(0x%04x); error at pc: 0x%04x\n", a, Drz80.Z80PC - Drz80.Z80PC_BASE);
        //return z80_read8(a) | (z80_read8(a+1) << 8) ;
    }
    return d;
}

static void z80_irq_callback(void)
{
    //printf("z80_irq_callback();\n");
    Drz80.Z80_IRQ = 0x00;
}


void ZetSetReadHandler(unsigned char (__fastcall *pHandler)(unsigned short))
{
    Drz80.z80_read8 = pHandler;
}

void ZetSetWriteHandler(void (__fastcall *pHandler)(unsigned short, unsigned char))
{
    Drz80.z80_write8 = pHandler;
}

void ZetSetInHandler(unsigned char (__fastcall *pHandler)(unsigned short))
{
    Drz80.z80_in = pHandler;
}

void ZetSetOutHandler(void (__fastcall *pHandler)(unsigned short, unsigned char))
{
    Drz80.z80_out = pHandler;
}

void ZetNewFrame()
{
    Drz80.nCyclesTotal = 0;
}

int ZetInit(int nCount)
{
    memset( &ppMemFetch, 0, 0x0400 );
    memset( &ppMemFetchData, 0, 0x0400 );
    memset( &ppMemRead, 0, 0x0400 );
    memset( &ppMemWrite, 0, 0x0400 );

    memset(&Drz80, 0, sizeof(Drz80)); //struct

    Drz80.z80_in			= ZetDummyInHandler;
    Drz80.z80_out		= ZetDummyOutHandler;
    Drz80.z80_rebasePC	= z80_rebasePC;
    //Drz80.z80_rebaseSP	= z80_rebaseSP;
    Drz80.z80_read8		= z80_read8;
    Drz80.z80_read16		= z80_read16;
    Drz80.z80_write8		= z80_write8;
    Drz80.z80_write16	= z80_write16;
    Drz80.z80_irq_callback=z80_irq_callback;

    Drz80.ppMemFetch		= &ppMemFetch[0];
    Drz80.ppMemFetchData	= &ppMemFetchData[0];
    Drz80.ppMemRead		= &ppMemRead[0];
    Drz80.ppMemWrite 	= &ppMemWrite[0];

    Drz80.debugCallback	= Z80DebugCallback;

    Drz80.nInterruptLatch = -1;

    ZetOpen(0);

    nCPUCount = nCount;
    nHasZet = nCount;
    return 0;
}

unsigned char ZetReadByte(unsigned short address)
{
    if (nOpenedCPU < 0) return 0;
    return 0;
}

void ZetWriteByte(unsigned short address, unsigned char data)
{
    if (nOpenedCPU < 0) return;
}

void ZetClose()
{
    nOpenedCPU = -1;
}

int ZetOpen(int nCPU)
{
    nOpenedCPU = nCPU;
    nOpenedCPU = 0;
    return 0;
}

int ZetGetActive()
{
    return 0;
}



#ifdef EMU_DRZ80

void DozeAsmCall(unsigned short a)
{
    //IFLOG printf("DozeAsmCall(0x%04x); PC: 0x%04x SP: 0x%04x", a, Drz80.Z80PC-Drz80.Z80PC_BASE, Drz80.Z80SP);

    int pc = Drz80.Z80PC-Drz80.Z80PC_BASE;

    Drz80.Z80SP -= 2;

    unsigned char * p = ppMemWrite[ Drz80.Z80SP >> 8 ];
    if ( p ) {
        p += Drz80.Z80SP;
        *p = pc & 0xff;
        p++;
        *p = pc >> 8;
        //*((unsigned char *)Drz80.Z80SP) = pc & 0xff;
        //*((unsigned char *)(Drz80.Z80SP+1)) = pc >> 8;
    } else {

        printf("DozeAsmCall Error PUSH PC!\n");
    }

    Drz80.Z80PC  = Drz80.z80_rebasePC( a );

    //IFLOG printf("-> 0x%04x 0x%04x\n", Drz80.Z80PC-Drz80.Z80PC_BASE, Drz80.Z80SP);
}

static unsigned char DozeAsmRead(int a)
{
    unsigned char * p = ppMemRead[ a >> 8 ];
    if ( p )
        return *(p + a);
    else
        return 0; // Drz80.ReadHandler(a);
}

static int Interrupt(int nVal)
{
    //if ((Z80.Z80IF & 0x03) == 0) 					// ????
    if ((Drz80.Z80IF & 0xFF) == 0) 					// not enabled
        return 0;

    //IFLOG printf("    IRQ taken  nIF: %02x  nIM: %02x PC: 0x%04x OpCode: 0x%02x\n", Drz80.Z80IF, Drz80.Z80IM, Drz80.Z80PC-Drz80.Z80PC_BASE, DozeAsmRead(Drz80.Z80PC-Drz80.Z80PC_BASE) );

    if ( DozeAsmRead(Drz80.Z80PC-Drz80.Z80PC_BASE) == 0x76 )
        Drz80.Z80PC ++;

    Drz80.Z80IF = 0;

    if (Drz80.Z80IM == 0) {
        DozeAsmCall((unsigned short)(nVal & 0x38));	// rst nn
        return 13;									// cycles done
    } else {
        if (Drz80.Z80IM == 2) {
            int nTabAddr = 0, nIntAddr = 0;
            printf("Drz80.Z80IM == 2\n");
            // Get interrupt address from table (I points to the table)
            nTabAddr = (Drz80.Z80I & 0xFF00) + nVal;

            // Read 16-bit table value
            nIntAddr  = DozeAsmRead((unsigned short)(nTabAddr + 1)) << 8;
            nIntAddr |= DozeAsmRead((unsigned short)(nTabAddr));

            DozeAsmCall((unsigned short)(nIntAddr));
            return 19;								// cycles done
        } else {
            DozeAsmCall(0x38);						// rst 38h
            return 13;								// cycles done
        }
    }
}

static inline void TryInt()
{
    int nDid;

    if (Drz80.nInterruptLatch & ZET_IRQSTATUS_NONE) return;

    nDid = Interrupt(Drz80.nInterruptLatch & 0xFF);	// Success! we did some cycles, and took the interrupt
    if (nDid > 0 && (Drz80.nInterruptLatch & ZET_IRQSTATUS_AUTO)) {
        Drz80.nInterruptLatch = ZET_IRQSTATUS_NONE;
    }

    Drz80.nCyclesLeft -= nDid;
}

static void DozeRun()
{

    TryInt();

    if (Drz80.nCyclesLeft < 0) {
        //printf("DozeRun() -- (nCyclesLeft < 0)\n");
        return;
    }

    if (DozeAsmRead(Drz80.Z80PC-Drz80.Z80PC_BASE) == 0x76) {

        //IFLOG printf("DozeRun() -- (*pc == 0x76)\n");

        // cpu is halted (repeatedly doing halt inst.)
        int nDid = (Drz80.nCyclesLeft >> 2) + 1;
        Drz80.Z80I = (unsigned short)(((Drz80.Z80I + nDid) & 0x7F) | (Drz80.Z80I & 0xFF80)); // Increase R register
        Drz80.nCyclesLeft -= nDid;
        return;
    }

    // Find out about mid-exec EIs
    Drz80.nEI = 1;
    DrZ80Run(&Drz80, Drz80.nCyclesLeft);

#if 0
    printf("CyclesLeft %d\n", Drz80.nCyclesLeft );
	printf(" AF: 0x%04x BC: 0x%04x DE: 0x%04x HL: 0x%04x\n", Drz80.Z80A >> 16 | Drz80.Z80F, Drz80.Z80BC >> 16,  Drz80.Z80DE >> 16,  Drz80.Z80HL >> 16 );
	printf(" AF' 0x%04x BC' 0x%04x DE' 0x%04x HL' 0x%04x\n", Drz80.Z80A2 >> 16 | Drz80.Z80F2, Drz80.Z80BC2 >> 16,  Drz80.Z80DE2 >> 16,  Drz80.Z80HL2 >> 16 );
	printf(" IX: 0x%04x IY: 0x%04x PC: 0x%04x SP: 0x%04x\n", Drz80.Z80IX >> 16,  Drz80.Z80IY >> 16,  Drz80.Z80PC-Drz80.Z80PC_BASE,  Drz80.Z80SP );
	printf(" IR: 0x%08x IF: 0x%02x IM: 0x%02x\n", Drz80.Z80I,  Drz80.Z80IF,  Drz80.Z80IM );
#endif

//IFLOG printf("Find out about mid-exec EIs: 0x%08x\n", Drz80.nEI );

    // Just enabled interrupts
    while (Drz80.nEI == 2) {


        //printf("    EI executed\n");
        // (do one more instruction before interrupt)
        int nTodo = Drz80.nCyclesLeft;
        Drz80.nCyclesLeft = 0;
        Drz80.nEI = 0;

        DrZ80Run(&Drz80, Drz80.nCyclesLeft);

#if 0
        printf("CyclesLeft %d\n", Drz80.nCyclesLeft );
	printf(" AF: 0x%04x BC: 0x%04x DE: 0x%04x HL: 0x%04x\n", Drz80.Z80A >> 16 | Drz80.Z80F, Drz80.Z80BC >> 16,  Drz80.Z80DE >> 16,  Drz80.Z80HL >> 16 );
	printf(" AF' 0x%04x BC' 0x%04x DE' 0x%04x HL' 0x%04x\n", Drz80.Z80A2 >> 16 | Drz80.Z80F2, Drz80.Z80BC2 >> 16,  Drz80.Z80DE2 >> 16,  Drz80.Z80HL2 >> 16 );
	printf(" IX: 0x%04x IY: 0x%04x PC: 0x%04x SP: 0x%04x\n", Drz80.Z80IX >> 16,  Drz80.Z80IY >> 16,  Drz80.Z80PC-Drz80.Z80PC_BASE,  Drz80.Z80SP );
	printf(" IR: 0x%08x IF: 0x%02x IM: 0x%02x\n", Drz80.Z80I,  Drz80.Z80IF,  Drz80.Z80IM );
#endif

        Drz80.nCyclesLeft += nTodo;

        TryInt();

        // And continue the rest of the exec
        DrZ80Run(&Drz80, Drz80.nCyclesLeft);
#if 0
        printf("CyclesLeft %d\n", Drz80.nCyclesLeft );
	printf(" AF: 0x%04x BC: 0x%04x DE: 0x%04x HL: 0x%04x\n", Drz80.Z80A >> 16 | Drz80.Z80F, Drz80.Z80BC >> 16,  Drz80.Z80DE >> 16,  Drz80.Z80HL >> 16 );
	printf(" AF' 0x%04x BC' 0x%04x DE' 0x%04x HL' 0x%04x\n", Drz80.Z80A2 >> 16 | Drz80.Z80F2, Drz80.Z80BC2 >> 16,  Drz80.Z80DE2 >> 16,  Drz80.Z80HL2 >> 16 );
	printf(" IX: 0x%04x IY: 0x%04x PC: 0x%04x SP: 0x%04x\n", Drz80.Z80IX >> 16,  Drz80.Z80IY >> 16,  Drz80.Z80PC-Drz80.Z80PC_BASE,  Drz80.Z80SP );
	printf(" IR: 0x%08x IF: 0x%02x IM: 0x%02x\n", Drz80.Z80I,  Drz80.Z80IF,  Drz80.Z80IM );
#endif

    }

//IFLOG printf("DozeRun(); %d  PC: %04x  PS: %04x\n", Drz80.nCyclesLeft, Drz80.Z80PC - Drz80.Z80PC_BASE, Drz80.Z80SP );
}

#endif

int ZetRun(int nCycles)
{
    if (nCycles <= 0) {
        return 0;
    }

    Drz80.nCyclesTotal += nCycles;
    Drz80.nCyclesSegment = nCycles;
    Drz80.nCyclesLeft = nCycles;

    DozeRun();
    nCycles = Drz80.nCyclesSegment - Drz80.nCyclesLeft;

    Drz80.nCyclesTotal -= Drz80.nCyclesLeft;
    Drz80.nCyclesLeft = 0;
    Drz80.nCyclesSegment = 0;

    return nCycles;

}

void ZetRunAdjust(int nCycles)
{

    if (nCycles < 0 && Drz80.nCyclesLeft < -nCycles) {
        nCycles = 0;
    }

    Drz80.nCyclesTotal += nCycles;
    Drz80.nCyclesSegment += nCycles;
    Drz80.nCyclesLeft += nCycles;
}

void ZetRunEnd()
{
    Drz80.nCyclesTotal -= Drz80.nCyclesLeft;
    Drz80.nCyclesSegment -= Drz80.nCyclesLeft;
    Drz80.nCyclesLeft = 0;
}

// This function will make an area callback ZetRead/ZetWrite
int ZetMemCallback(int nStart, int nEnd, int nMode)
{

    nStart >>= 8;
    nEnd += 0xff;
    nEnd >>= 8;

    // Leave the section out of the memory map, so the Doze* callback with be used
    for (int i = nStart; i < nEnd; i++) {
        switch (nMode) {
            case 0:
                Drz80.ppMemRead[i] = NULL;
                break;
            case 1:
                Drz80.ppMemWrite[i] = NULL;
                break;
            case 2:
                Drz80.ppMemFetch[i] = NULL;
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
    nCPUCount = 0;
}


int ZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem)
{

    int s = nStart >> 8;
    int e = (nEnd + 0xFF) >> 8;

    // Put this section in the memory map, giving the offset from Z80 memory to PC memory
    for (int i = s; i < e; i++) {
        switch (nMode) {
            case 0:
                Drz80.ppMemRead[i] = Mem - nStart;
                break;
            case 1:
                Drz80.ppMemWrite[i] = Mem - nStart;
                break;
            case 2:
                Drz80.ppMemFetch[i] = Mem - nStart;
                Drz80.ppMemFetchData[i] = Mem - nStart;
                break;
        }
    }

    return 0;
}

int ZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem01, unsigned char *Mem02)
{

    int s = nStart >> 8;
    int e = (nEnd + 0xFF) >> 8;

    if (nMode != 2) {
        return 1;
    }

    // Put this section in the memory map, giving the offset from Z80 memory to PC memory
    for (int i = s; i < e; i++) {
        Drz80.ppMemFetch[i] = Mem01 - nStart;
        Drz80.ppMemFetchData[i] = Mem02 - nStart;
    }

    return 0;
}

int ZetReset()
{

    Drz80.spare			= 0;
    Drz80.z80irqvector	= 0;

    Drz80.Z80A			= 0x00 <<24;
    Drz80.Z80F			= (1<<2); /* set ZFlag */
    Drz80.Z80BC			= 0x0000<<16;
    Drz80.Z80DE			= 0x0000<<16;
    Drz80.Z80HL			= 0x0000<<16;
    Drz80.Z80A2			= 0x00<<24;
    Drz80.Z80F2			= 1<<2;  /* set ZFlag */
    Drz80.Z80BC2			= 0x0000<<16;
    Drz80.Z80DE2			= 0x0000<<16;
    Drz80.Z80HL2			= 0x0000<<16;
    Drz80.Z80IX			= 0xFFFF<<16;
    Drz80.Z80IY			= 0xFFFF<<16;
    Drz80.Z80I			= 0x00;
    Drz80.Z80IM			= 0x00;
    Drz80.Z80_IRQ		= 0x00;
    Drz80.Z80IF			= 0x00;
    Drz80.Z80PC			= Drz80.z80_rebasePC(0);
    //Drz80.Z80SP			= 0Drz80.z80_rebaseSP(0xffff); /* 0xf000 */

    /*
     * reset SP to 0xF000
     */
    Drz80.Z80SP			= 0xf000;

    //Drz80.nInterruptLatch = 0;
    //Drz80.nEI			= 0;
    //Drz80.nCyclesLeft	= 0;
    //Drz80.nCyclesTotal	= 0;
    //Drz80.nCyclesSegment	= 0;

    return 0;
}

int ZetPc(int n)
{
    return Drz80.Z80PC - Drz80.Z80PC_BASE;

}

int ZetBc(int n)
{

}

int ZetDe(int n)
{

}

int ZetHL(int n)
{

}

int ZetScan(int nAction)
{

    if ((nAction & ACB_DRIVER_DATA) == 0) {
        return 0;
    }

    char szText[] = "Z80 #0";
    for (int i = 0; i < nCPUCount; i++) {
        szText[5] = '1' + i;
        ScanVar(&Drz80, 19 * 4 + 16, szText);
    }

    return 0;
}

void ZetSetIRQLine(const int line, const int status)
{
    Drz80.nInterruptLatch = line | status;

}



void ZetRaiseIrq(int n)
{
    ZetSetIRQLine(n, ZET_IRQSTATUS_AUTO);
}

void ZetLowerIrq()
{
    ZetSetIRQLine(0, ZET_IRQSTATUS_NONE);
}

int ZetNmi()
{
    Drz80.Z80IF &= 0x02;						// reset iff1
    DozeAsmCall((unsigned short)0x66);		// Do nmi
    Drz80.nCyclesTotal += 12;
    return 12;
}

//inline
int ZetIdle(int nCycles)
{
    Drz80.nCyclesTotal += nCycles;

}

int ZetSegmentCycles()
{
    return Drz80.nCyclesSegment - Drz80.nCyclesLeft;
}
//inline
int ZetTotalCycles() {
    return Drz80.nCyclesTotal - Drz80.nCyclesLeft;
}
