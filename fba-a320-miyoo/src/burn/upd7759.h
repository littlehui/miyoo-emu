#define UPD7759_STANDARD_CLOCK		640000

typedef void (*drqcallback)(int param);

extern void UPD7759Update(short *pSoundBuf, int nLength);
extern void UPD7759Reset();
extern void UPD7759Init(int clock, unsigned char* pSoundData);
extern void UPD7759SetDrqCallback(drqcallback Callback);
extern int UPD7759BusyRead();
extern void UPD7759ResetWrite(UINT8 Data);
extern void UPD7759StartWrite(UINT8 Data);
extern void UPD7759PortWrite(UINT8 Data);
extern int UPD7759Scan(int nAction,int *pnMin);
extern void UPD7759Exit();
