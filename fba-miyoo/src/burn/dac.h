extern void DACUpdate(short* Buffer, int Length);
extern void DACWrite(UINT8 Data);
extern void DACInit(int Clock);
extern void DACReset();
extern void DACExit();
extern int DACScan(int nAction,int *pnMin);
