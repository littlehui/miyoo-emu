// Z80 (Zed Eight-Ty) Interface

#ifndef _ZET_H_
#define _ZET_H_

#ifndef FASTCALL
 #undef __fastcall
 #define __fastcall
#endif

#define EMU_DRZ80
//#define EMU_MAME_Z80
//#define EMU_CZ80

#include "DrZ80.h"
extern struct DrZ80 Drz80;

extern int nHasZet;
extern int nZetCpuCore; // 0 - CZ80, 1 - MAME_Z80 ,3 Drz80
//DrZ80 no
void ZetWriteByte(unsigned short address, unsigned char data);
//DrZ80 no
unsigned char ZetReadByte(unsigned short address);

int ZetInit(int nCount);
void ZetExit();
void ZetNewFrame();
int ZetOpen(int nCPU);
void ZetClose();
//DrZ80 no
int ZetGetActive();
int ZetMemCallback(int nStart,int nEnd,int nMode);
//DrZ80 no
int ZetMemEnd();
int ZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem);
int ZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem01, unsigned char *Mem02);
int ZetReset();
int ZetPc(int n);
//DrZ80 no
int ZetBc(int n);
//DrZ80 no
int ZetDe(int n);
//DrZ80 no
int ZetHL(int n);
int ZetScan(int nAction);
int ZetRun(int nCycles);
//DrZ80 no
void ZetRunEnd();


/*inline static void ZetSetIRQLine(const int line, const int status)
{
    Drz80.nInterruptLatch = line | status;

}*/
//change
void ZetSetIRQLine(const int line, const int status);

int ZetNmi();

/*inline static int ZetIdle(int nCycles)
{
    Drz80.nCyclesTotal += nCycles;

}*/
//change
int ZetIdle(int nCycles);
/*inline static int ZetSegmentCycles()
{
    return Drz80.nCyclesSegment - Drz80.nCyclesLeft;

}*/
//change
int ZetSegmentCycles();

/*inline static int ZetTotalCycles()
{
    return Drz80.nCyclesTotal - Drz80.nCyclesLeft;

}*/
//change
int ZetTotalCycles();

void ZetRaiseIrq(int n);
void ZetLowerIrq();

//#define ZetRaiseIrq(n) ZetSetIRQLine(n, ZET_IRQSTATUS_AUTO)
//#define ZetLowerIrq() ZetSetIRQLine(0, ZET_IRQSTATUS_NONE)
//change
//void ZetRaiseIrq(int n);
//void ZetLowerIrq();

#define ZET_IRQSTATUS_NONE (0x8000)
#define ZET_IRQSTATUS_AUTO (0x2000)
#define ZET_IRQSTATUS_ACK  (0x1000)

void ZetSetReadHandler(unsigned char (__fastcall *pHandler)(unsigned short));
void ZetSetWriteHandler(void (__fastcall *pHandler)(unsigned short, unsigned char));
void ZetSetInHandler(unsigned char (__fastcall *pHandler)(unsigned short));
void ZetSetOutHandler(void (__fastcall *pHandler)(unsigned short, unsigned char));

#endif //_ZET_H_