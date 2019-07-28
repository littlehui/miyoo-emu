#include "tmnt.h"
#include "upd7759.h"
// TMNT - Run module
// Based on DTMNT and MAME

unsigned char bTmntRecalcPal=0;

unsigned char *TmntRam08,*TmntRam10,*TmntTile,*TmntZ80Ram,*PriRam,*K051960Ram;
static unsigned char *TmntSampleROM01 = NULL;
static unsigned char *TmntSampleROM02 = NULL;
static unsigned char *TmntSampleROM03 = NULL;
unsigned char *Tmnt68KROM01,*TmntRam06,*TmntSprite, *TmntZ80Rom, *TmntSpriteTemp/*,*Ram14*/;
static unsigned char *Mem=NULL,*MemEnd=NULL;
static unsigned char *RamStart=NULL,*RamEnd=NULL;

static int tmnt_soundlatch=0;
unsigned char bInt5=0;
static int nCyclesDone[2], nCyclesTotal[2];
static int nCyclesSegment;

unsigned char DrvReset=0;
inline static int CheckSleep(int)
{
	return 0;
}

// This routine is called first to determine how much memory is needed (MemEnd-(unsigned char *)0),
// and then to set up all the pointers
static int MemIndex()
{
	unsigned char *Next; Next=Mem;
	Tmnt68KROM01 = Next;	Next+=0x060000; // 68000 program

	RamStart = Next;
	K051960Ram = Next;	Next+=0x000800; // spr ram
	TmntRam10 = Next;	Next+=0x008000; // tile ram
	TmntRam08 = Next;	Next+=0x001000; // palette
	PriRam = Next;		Next+=0x0100;
    TmntRam06 = Next;	Next+=0x004000;
	RamEnd = Next;

	TmntTile = Next;	Next+=0x100000; // Tile bitmaps
	TmntSprite = Next;	Next+=0x400000; // Tile bitmaps
	MemEnd = Next;
	return 0;
}

//////// 
// graphics decoding (taken from mame)
//

/*
    This recursive function doesn't use additional memory
    (it could be easily converted into an iterative one).
    It's called shuffle because it mimics the shuffling of a deck of cards.
*/
static void shuffle(UINT16 *buf,int len)
{
	int i;
	UINT16 t;

	if (len == 2) 
		return;

	if (len % 4) 
	{
		//fatalerror("shuffle() - not modulo 4");   /* must not happen */
		return;
	}

	len /= 2;

	for (i = 0;i < len/2;i++)
	{
		t = buf[len/2 + i];
		buf[len/2 + i] = buf[len + i];
		buf[len + i] = t;
	}

	shuffle(buf,len);
	shuffle(buf + len,len);
}




static int Interleve2(unsigned char *pd,int i,int nLen)
{
	unsigned char *pt;
	unsigned short *pts,*ptd;
	int a;

	pt=(unsigned char *)malloc(nLen); 
	if (pt==NULL) 
	{
		return 1;
	}
	memset(pt,0,nLen);
	BurnLoadRom(pt,i,1);

	nLen>>=1; 
	pts=(unsigned short *)pt;
	ptd=(unsigned short *)pd;
	for (a=0;a<nLen; a++, pts++, ptd+=2)
	{
		*ptd=*pts;
	}
	free(pt);
	return 0;
}

/*
static GraphicsDecodeInfo tmnt_CHAR_Info= {
	8,8,
	0x100000/32,
	4,
	{ 3*8, 2*8, 1*8, 0*8 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};


*/

static int LoadRoms()
{
	int nRet=0;
	// Load program roms and byteswap
	nRet=BurnLoadRom(Tmnt68KROM01+0x00001,0,2); 
	if (nRet!=0) 
	{
		return 1;
	}
	nRet=BurnLoadRom(Tmnt68KROM01+0x00000,1,2); 
	if (nRet!=0) 
	{
		return 1;
	}
	nRet=BurnLoadRom(Tmnt68KROM01+0x40001,2,2); 
	if (nRet!=0) 
	{
		return 1;
	}
	nRet=BurnLoadRom(Tmnt68KROM01+0x40000,3,2); 
	if (nRet!=0) 
	{
		return 1;
	}

	nRet=BurnLoadRom(TmntZ80Rom,10,1); // load z80 code rom


	// Load tile bitmaps
	Interleve2(TmntTile+0x000000,4,0x80000);
	Interleve2(TmntTile+0x000002,5,0x80000);

	// Load sprites
	TmntSpriteTemp=(unsigned char *)malloc(0x200000);

	Interleve2(TmntSpriteTemp+0x000000,6,0x80000);
	Interleve2(TmntSpriteTemp+0x000002,7,0x80000);
	Interleve2(TmntSpriteTemp+0x100000,8,0x80000);
	Interleve2(TmntSpriteTemp+0x100002,9,0x80000);

	for (int i = 0; i<0x200000;i++)
	{
		TmntSprite[i*2]=(TmntSpriteTemp[i^1] & 0x0F);//((TmntSpriteTemp[i] & 0xF0) >> 4);
		TmntSprite[(i*2)+1]=((TmntSpriteTemp[i^1] & 0xF0) >> 4);//(TmntSpriteTemp[i] & 0x0F);

	}

	free(TmntSpriteTemp);

	// Load samples
	TmntSampleROM01=(unsigned char *)malloc(0x20000);
	if (TmntSampleROM01==NULL)
	{
		return 1;
	}
	nRet=BurnLoadRom(TmntSampleROM01,11,1); 
	if (nRet!=0) 
	{
		return 1; // K007232
	}

	TmntSampleROM02=(unsigned char *)malloc(0x20000);
	if (TmntSampleROM02==NULL)
	{
		return 1;
	}

	nRet=BurnLoadRom(TmntSampleROM02,12,1); 
	if (nRet!=0) 
	{
		return 1; // UPD7759
	}

	TmntSampleROM03=(unsigned char *)malloc(0x80000);
	if (TmntSampleROM03==NULL)
	{
		return 1;
	}

	nRet=BurnLoadRom(TmntSampleROM03,13,1); 
	if (nRet!=0) 
	{
		return 1;
	}
	

	if (TmntSampleROM01!=NULL)
	{
		free(TmntSampleROM01);
	}
	TmntSampleROM01=NULL;

//	if (TmntSampleROM02!=NULL)
//	{
//		free(TmntSampleROM02);
//	}
//	TmntSampleROM02=NULL;

	if (TmntSampleROM03!=NULL)
	{
		free(TmntSampleROM01);
	}
	TmntSampleROM03=NULL;

	return 0;
}


// Z80 accesses
unsigned char __fastcall tmntZ80In(unsigned short wordValue)
{
	printf("\nZ80: read from port %x",wordValue);
	return 0;
}

void __fastcall tmntZ80Out(unsigned short wordValue,unsigned char val)
{
	printf("\nZ80: Write %x to port %x",val,wordValue);
}

unsigned char __fastcall tmntZ80Read(unsigned short wordValue)
{
	if (wordValue==0x9000)
	{
		//printf("\nZ80: tmnt_sres_r");
	}
	if (wordValue==0xa000)
	{
		//ZetRaiseIrq(1);
		return tmnt_soundlatch;
	//	printf("\nZ80: read from soundlatch");
	}
	if ((wordValue>=0xb000) && (wordValue<=0xb00d))
	{
	//	printf("\nZ80: read from K007232 %d",0xb000 - wordValue);
	}
	if (wordValue==0xc001)
	{
		return BurnYM2151ReadStatus();
	//	printf("\nZ80: YM2151 status read");
	}
	if (wordValue==0xf000)
	{
		return UPD7759BusyRead();
//		bprintf(PRINT_NORMAL, _T("\nZ80: UDP7759 busy read"));
	}
	return 0;
}

void __fastcall tmntZ80Write(unsigned short wordValue,unsigned char val)
{
	if (wordValue==0x9000)
	{
		UPD7759ResetWrite(val & 2);
//		bprintf(PRINT_NORMAL, _T("\nZ80: write %d to UPD7759 reset"), val & 2);
		tmnt_soundlatch = val;
	}
	if ((wordValue>=0xb000) && (wordValue<=0xb00d))
	{
//		printf("\nZ80: write %d to K007232 %d",val,0xb000 - wordValue);
		//ZetNmi();
	//	ZetLowerIrq();
	}
	if (wordValue==0xc000)
	{
		BurnYM2151SelectRegister(val);
	}
	if (wordValue==0xc001)
	{
		BurnYM2151WriteRegister(val);
	}
	if (wordValue==0xd000)
	{
		UPD7759PortWrite(val);
//		bprintf(PRINT_NORMAL, _T("\nZ80: write %d to UPD7759_0_message"), val);
	}
	if (wordValue==0xe000)
	{
		UPD7759StartWrite(val);
//		bprintf(PRINT_NORMAL, _T("\nZ80: write %d to UPD7759_0_start"), val);
	}
}
// end of z80 accesses

void tmntYM2151IRQHandler(int /*irq*/)
{

}


unsigned short __fastcall TmntReadWord(unsigned int /*sekAddress*/)
{
	return 0;
}

void __fastcall TmntWriteWord(unsigned int sekAddress, unsigned short wordValue)
{
	switch (sekAddress) {
		default: {
			bprintf(PRINT_NORMAL, _T("TmntWriteWord: value %x to location %x\n"), wordValue, sekAddress);
		}
	}
}


unsigned char __fastcall TmntReadByte(unsigned int a)
{
	if (a>=0xa0000 && a<0xa0020)
	{
		return (unsigned char)(~TmntAoo[(a>>1)&0xf]);
	}
	return 0xff;
}
/*
static ADDRESS_MAP_START( tmnt_writemem, ADDRESS_SPACE_PROGRAM, 16 )

	AM_RANGE(0x0a0000, 0x0a0001) AM_WRITE(tmnt_0a0000_w)
	AM_RANGE(0x0a0008, 0x0a0009) AM_WRITE(tmnt_sound_command_w)
	AM_RANGE(0x0a0010, 0x0a0011) AM_WRITE(watchdog_reset16_w)
//  AM_RANGE(0x10e800, 0x10e801) AM_WRITE(MWA16_NOP) ???
	AM_RANGE(0x140000, 0x140007) AM_WRITE(K051937_word_w)
ADDRESS_MAP_END
*/
void __fastcall TmntWriteByte(unsigned int a,unsigned char d)
{
	unsigned int ab;
	static int last;
	ab=(a>>16)&0xff;
	if (ab==0x08)
	{
		TmntPalWrite(a,d);
		return;
	} // Palette write

	switch (a)
	{
	case 0xa0001:
		{

		/* bit 3 high then low triggers irq on sound CPU */
		if (last == 0x08 && (d & 0x08) == 0)
		{
			ZetSetIRQLine(0xff, ZET_IRQSTATUS_AUTO);
			//cpunum_set_input_line_and_vector(1,0,HOLD_LINE,0xff);
		}

		last = d & 0x08;

		if (d&0x20) 
		{
			bInt5=1;
		}
		else 
		{
			bInt5=0;
		}
		return;
		}
	case 0x0a0011:  // watchdog reset
		return;
	case 0x0a0009: // sound code
		tmnt_soundlatch = d&0xFF;
		return;




	default:
		bprintf(PRINT_NORMAL, _T("TmntWriteByte: value %x to location %x\n"), d, a);
		break;

	}
}

void TmntReset()
{
	SekOpen(0);
	SekReset();
	SekClose();

	ZetOpen(0);
	ZetReset();
	ZetClose();
	
	bInt5=0;
	tmnt_soundlatch = 0;
	BurnYM2151Reset();
	UPD7759Reset();

}

int TmntInit()
{
	int nRet=0; int nLen=0;

	// Find out how much memory is needed
	Mem=NULL;
	MemIndex();
	nLen=MemEnd-(unsigned char *)0;
	Mem=(unsigned char *)malloc(nLen);
	if (Mem==NULL)
		return 1;
	memset(Mem,0,nLen); // blank all memory
	MemIndex(); // Index the allocated memory

	TmntZ80Rom=(unsigned char *)malloc(0x08000);
	if (TmntZ80Rom==NULL) return 1;
	memset(TmntZ80Rom,0x11,0x08000);

	TmntZ80Ram=(unsigned char *)malloc(0x0800);
	if (TmntZ80Ram==NULL) return 1;
	memset(TmntZ80Ram,0x00,0x0800);

	nRet=LoadRoms();
	if (nRet!=0)
		return 1; // Load the roms into memory

  	SekInit(0,0x68000);	// Allocate 68000
	SekOpen(0);

	// Map in memory:
	// ----------------- Cpu 1 ------------------------
	SekMapMemory(Tmnt68KROM01, 0x000000,0x05FFFF, SM_ROM); // 68000 Rom
	SekMapMemory(TmntRam06,    0x060000,0x063FFF, SM_RAM);
	SekMapMemory(TmntRam08,    0x080000,0x080FFF, SM_ROM); // Write goes through handler
	SekMapMemory(PriRam,       0x0c0000,0x0c00FF, SM_RAM);
	SekMapMemory(TmntRam10,    0x100000,0x107FFF, SM_RAM);
	SekMapMemory(K051960Ram,   0x140400,0x1407FF, SM_RAM);

	SekSetReadWordHandler(0,TmntReadWord);
	SekSetWriteWordHandler(0,TmntWriteWord);
	SekSetReadByteHandler(0,TmntReadByte);
	SekSetWriteByteHandler(0,TmntWriteByte);
	// ------------------------------------------------
//	SekReset();
	SekClose();
	
	// Setup the Z80 emulation
	ZetInit(1);
	ZetOpen(0);
	ZetMapArea(0x0000, 0x7fff, 0, TmntZ80Rom);
	ZetMapArea(0x0000, 0x7fff, 2, TmntZ80Rom);
	ZetMapArea(0x8000, 0x87FF, 0, TmntZ80Ram);
	ZetMapArea(0x8000, 0x87FF,1, TmntZ80Ram);
	ZetMapArea(0x8000, 0x87FF, 2, TmntZ80Ram);
	ZetMemEnd();
	ZetSetInHandler(tmntZ80In);
	ZetSetOutHandler(tmntZ80Out);
	ZetSetReadHandler(tmntZ80Read);
	ZetSetWriteHandler(tmntZ80Write);
//	ZetReset();
	ZetClose();

	// sound stuff
	// ym2151
    	BurnYM2151Init(3579545, 50.0);
	BurnYM2151SetIrqHandler(&tmntYM2151IRQHandler);
//	BurnYM2151Reset();
	
	UPD7759Init(UPD7759_STANDARD_CLOCK, TmntSampleROM02);

	bInt5=0;
	GenericTilesInit();
	TmntPalInit();
	
	TmntReset();
	return 0;
}


int TmntExit()
{
	GenericTilesExit();
	TmntPalExit();
	BurnYM2151Exit();
	UPD7759Exit();
	SekExit(); // Deallocate 68000
	ZetExit();
	// Deallocate all used memory
	if (Mem!=0)
		free(Mem);
	Mem=0;
	if (TmntZ80Rom!=0)
		free(TmntZ80Rom);
	TmntZ80Rom=0;
	if (TmntZ80Ram!=0)
		free(TmntZ80Ram);
	TmntZ80Ram=0;
	return 0;
}

static int TmntDraw()
{
	BurnTransferClear();
	TmntPalUpdate(bTmntRecalcPal); // Recalc whole pal if needed
	bTmntRecalcPal=0;
	BurnClearScreen();
	TmntTileDraw();
	BurnTransferCopy(TmntPal);
	return 0;
}


int TmntFrame()
{
	int nInterleave = 16;
	if (DrvReset)
	{
		TmntReset();
	}
	TmntInpMake();

	nCyclesTotal[0] = 8000000/60;							
	nCyclesTotal[1] = 3579545/60;
	nCyclesDone[0] = nCyclesDone[1] = 0;
	int nSoundBufferPos = 0;

	SekOpen(0);
	ZetOpen(0);
	if (bInt5)
	{
		SekSetIRQLine(5, SEK_IRQSTATUS_AUTO);
	}
	for (int i = 0; i < nInterleave; i++) {
    	int nCurrentCPU;
		int nNext;
		// Run 68000
		nCurrentCPU = 0;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;

		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (!CheckSleep(nCurrentCPU)) {					// See if this CPU is busywaiting
			nCyclesDone[nCurrentCPU] += SekRun(nCyclesSegment);
		} else {
			nCyclesDone[nCurrentCPU] += nCyclesSegment;
		}
		// Run Z80
		nCurrentCPU = 1;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesSegment = ZetRun(nCyclesSegment);
		nCyclesDone[nCurrentCPU] += nCyclesSegment;

		{
			// Render sound segment
			if (pBurnSoundOut) {
				int nSegmentLength = nBurnSoundLen / nInterleave;
				short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				UPD7759Update(pSoundBuf, nSegmentLength);
				nSoundBufferPos += nSegmentLength;
			}
		}
	}

	{
		// Make sure the buffer is entirely filled.
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			if (nSegmentLength) {
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				UPD7759Update(pSoundBuf, nSegmentLength);
			}
		}
	}
//	
	ZetClose();
	SekClose();
	
//	UPD7759Update(pBurnSoundOut, nBurnSoundLen);

	if (pBurnDraw) {
		TmntDraw();														// Draw screen if needed
	}

	return 0;
}

int TmntScan(int nAction,int *pnMin)
{
	if (nAction&4) // Scan volatile ram
	{
		struct BurnArea ba;
		if (pnMin!=NULL)
			*pnMin=0x010601; // Return minimum compatible version

		memset(&ba,0,sizeof(ba));
		ba.Data=RamStart;
		ba.nLen=RamEnd-RamStart;
		ba.szName="All Ram";
		BurnAcb(&ba);

		SekScan(nAction&3); // scan 68000 states
		SCAN_VAR(bInt5);
	}
	return 0;
}


//// MIA specific

// This routine is called first to determine how much memory is needed (MemEnd-(unsigned char *)0),
// and then to set up all the pointers
static int MiaIndex()
{
  unsigned char *Next; Next=Mem;
 Tmnt68KROM01  =Next;   Next+=0x040000; // 68000 program
  RamStart =Next;
      K051960Ram=Next;   Next+=0x000800; // spr ram
  TmntRam10=Next;   Next+=0x008000; // tile ram
  TmntRam08=Next;   Next+=0x001000; // palette
	 PriRam=Next;	Next+=0x0100;
	  TmntRam06=Next;   Next+=0x004000;
  RamEnd   =Next;
  TmntTile =Next;    Next+=0x040000; // Tile bitmaps
  TmntSprite=Next;   Next+=0x400000; // sprite bitmaps
  MemEnd=Next;
  return 0;
}



static int MiaLoadRoms()
{
	int nRet=0;
	// Load program roms and byteswap
	nRet=BurnLoadRom(Tmnt68KROM01+0x00001,0,2); 
	if (nRet!=0) 
	{
		return 1;
	}
	nRet=BurnLoadRom(Tmnt68KROM01+0x00000,1,2); 
	if (nRet!=0) 
	{
		return 1;
	}

	nRet=BurnLoadRom(TmntTile,2,2); 
	nRet=BurnLoadRom(TmntTile+0x000001,3,2); 
	nRet=BurnLoadRom(TmntTile+0x020000,4,2); 
	nRet=BurnLoadRom(TmntTile+0x020001,5,2); 

	shuffle((UINT16*)TmntTile,0x20000);


	BurnLoadRom(TmntSprite+0x000000,6,2);
	BurnLoadRom(TmntSprite+0x000001,7,2);
	//shuffle((UINT16*)TmntSprite,0x100000/2);

	TmntSpriteTemp=(unsigned char *)malloc(0x80000*2);

	Interleve2(TmntSpriteTemp+0x000000,6,0x80000);
	Interleve2(TmntSpriteTemp+0x000002,7,0x80000);


	for (int i = 0; i<0x100000;i++)
	{
		TmntSprite[i*2]=(TmntSpriteTemp[i^1] & 0x0F);//((TmntSpriteTemp[i] & 0xF0) >> 4);
		TmntSprite[(i*2)+1]=((TmntSpriteTemp[i^1] & 0xF0) >> 4);//(TmntSpriteTemp[i] & 0x0F);

	}

	free(TmntSpriteTemp);


	return 0;
}



int MiaInit()
{
	int nRet=0;
	int nLen=0;

  // Find out how much memory is needed
	Mem=NULL;
	MiaIndex();
	nLen=MemEnd-(unsigned char *)0;
	Mem=(unsigned char *)malloc(nLen);
	if (Mem==NULL)
		return 1;
	memset(Mem,0,nLen); // blank all memory
	MiaIndex(); // Index the allocated memory

	nRet=MiaLoadRoms();
	if (nRet!=0)
		return 1; // Load the roms into memory

  	SekInit(0,0x68000);	// Allocate 68000

	SekOpen(0);

	// Map in memory:
	// ----------------- Cpu 1 ------------------------
	SekMapMemory(    Tmnt68KROM01  ,0x000000,0x03FFFF,SM_ROM); // 68000 Rom
	SekMapMemory(    TmntRam06,0x040000,0x063FFF,SM_RAM);
	SekMapMemory(TmntRam08,0x080000,0x080FFF,SM_ROM); // Write goes through handler
	SekMapMemory(   PriRam,0x0c0000,0x0c00FF,SM_RAM);
	SekMapMemory(TmntRam10,0x100000,0x107FFF,SM_RAM);
	SekMapMemory(    K051960Ram,0x140000,0x1407FF,SM_RAM);
	SekSetReadWordHandler(0,TmntReadWord);
	SekSetWriteWordHandler(0,TmntWriteWord);
	SekSetReadByteHandler(0,TmntReadByte);
	SekSetWriteByteHandler(0,TmntWriteByte);

	// ------------------------------------------------
	SekReset(); 
	bInt5=0;
	SekClose();
	MiaReset();
	GenericTilesInit();
	TmntPalInit();
	return 0;
}

int MiaFrame()
{
	TmntInpMake();

	SekOpen(0);
	if (bInt5)
	{
		SekSetIRQLine(5, SEK_IRQSTATUS_AUTO);
	}
	SekRun(8000000/60); // 8mhz
	SekClose();
	if (pBurnDraw!=NULL)
	{
		TmntDraw();
	}
	 return 0;
}


void MiaReset()
{
	bInt5=0;
	tmnt_soundlatch = 0;
	SekOpen(0);
	SekReset();
	SekClose();
}


int MiaExit()
{
	GenericTilesExit();
	TmntPalExit();
	SekExit(); // Deallocate 68000
	// Deallocate all used memory
	if (Mem!=0)
		free(Mem);
	Mem=0;
	if (TmntZ80Rom!=0)
		free(TmntZ80Rom);
	TmntZ80Rom=0;
	if (TmntZ80Ram!=0)
		free(TmntZ80Ram);
	TmntZ80Ram=0;
	return 0;
}


