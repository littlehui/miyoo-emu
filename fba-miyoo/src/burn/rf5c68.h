void RF5C68PCMUpdate(short* pSoundBuf, int length);
void RF5C68PCMReset();
void RF5C68PCMInit(int clock);
void RF5C68PCMRegWrite(unsigned char offset, unsigned char data);
unsigned char RF5C68PCMRead(unsigned char offset);
void RF5C68PCMWrite(unsigned char offset, unsigned char data);
