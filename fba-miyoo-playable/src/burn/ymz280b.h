// Yamaha YMZ280B module

int YMZ280BInit(int nClock, void (*IRQCallback)(int), int nChannels);
void YMZ280BReset();
int YMZ280BScan();
void YMZ280BExit();
int YMZ280BRender(short* pSoundBuf, int nSegmenLength);
void YMZ280BWriteRegister(unsigned char nValue);
unsigned int YMZ280BReadStatus();

extern unsigned char* YMZ280BROM;

extern unsigned int nYMZ280BStatus;
extern unsigned int nYMZ280BRegister;

inline static void YMZ280BSelectRegister(unsigned char nRegister)
{
	nYMZ280BRegister = nRegister;
}

