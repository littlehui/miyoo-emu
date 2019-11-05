// Cheat module

#include "burnint.h"
#include "vez.h"
#include "sh2.h"
#include "m6502.h"

bool bCheatsAllowed;
CheatInfo* pCheatInfo = NULL;

static bool bCheatsEnabled = false;

//----------------------------------------------------
// Cpu interface for cheat application

static int nCpuType;
extern int nSekCount;

static void set_cpu_type()
{
/*
	Probably not the best way to do things, as I imagine
	there will be games that need cheats applied on a cpu
	that isn't the first one.
*/

	if (nSekCount > -1)
	{
		nCpuType = 1;			// Motorola 68000
	}
	else if (nVezCount)
	{
		nCpuType = 2;			// NEC V30 / V33
	}
	else if (has_sh2)
	{
		nCpuType = 3;			// Hitachi SH2
	}
	else if (nHasZet > -1)
	{
		nCpuType = 5;			// Zilog Z80
	}
	else if (nM6502Count)
	{
		nCpuType = 4;			// MOS 6502
	}
	else
	{
		nCpuType = 0;			// Unknown (don't use cheats)
	}
}

static void cpu_open(int nCpu)
{
	switch (nCpuType)
	{
		case 1:	
			SekOpen(nCpu);
		break;

		case 2:	
			VezOpen(nCpu);	
		break;

		case 3:	
			Sh2Open(nCpu);
		break;

		case 4:
			m6502Open(nCpu);
		break;

		case 5:
			ZetOpen(nCpu);
		break;
	}
}

static void cpu_close()
{
	switch (nCpuType)
	{
		case 1:
			SekClose();
		break;

		case 2:
			VezClose();	
		break;

		case 3:
			Sh2Close();
		break;

		case 4:
			m6502Close();
		break;

		case 5:
			ZetClose();
		break;
	}
}

static int cpu_get_active()
{
	switch (nCpuType) {
		case 1: {
			return SekGetActive();
		}
		
		case 2: {
			return VezGetActive();
		}
		
		case 3: {
			return Sh2GetActive();
		}
		
		case 4: {
			return m6502GetActive();
		}
		
		case 5: {
			return ZetGetActive();
		}
	}
}

static unsigned char cpu_read_byte(unsigned int a)
{
	switch (nCpuType)
	{
		case 1:
			return SekReadByte(a);

		case 2:
			return cpu_readmem20(a);	

		case 3:
			return Sh2ReadByte(a);

		case 4:
			return m6502_read_byte(a);

		case 5:
			return ZetReadByte(a);
	}

	return 0;
}

static void cpu_write_byte(unsigned int a, unsigned char d)
{
	switch (nCpuType)
	{
		case 1:
			SekWriteByteROM(a, d);
		break;

		case 2:
			cpu_writemem20(a, d);
		break;

		case 3:
			Sh2WriteByte(a, d);
		break;

		case 4:
			m6502_write_byte(a, d);
		break;

		case 5:
			ZetWriteByte(a, d);
		break;
	}
}

//---------------------------------------------------------------


int CheatUpdate()
{
	if (nCpuType == -1) {
		set_cpu_type();
	}

	bCheatsEnabled = false;

	if (bCheatsAllowed) {
		CheatInfo* pCurrentCheat = pCheatInfo;
		CheatAddressInfo* pAddressInfo;

		while (pCurrentCheat) {
			if (pCurrentCheat->nStatus > 1) {
				pAddressInfo = pCurrentCheat->pOption[pCurrentCheat->nCurrent]->AddressInfo;
				if (pAddressInfo->nAddress) {
					bCheatsEnabled = true;
				}
			}
			pCurrentCheat = pCurrentCheat->pNext;
		}
	}

	return 0;
}

int CheatEnable(int nCheat, int nOption)
{
	int nCurrentCheat = 0;
	CheatInfo* pCurrentCheat = pCheatInfo;
	CheatAddressInfo* pAddressInfo;
	int nOpenCPU = -1;

	if (!bCheatsAllowed) {
		return 1;
	}

	if (nOption >= CHEAT_MAX_OPTIONS) {
		return 1;
	}

	while (pCurrentCheat && nCurrentCheat <= nCheat) {
		if (nCurrentCheat == nCheat) {

			if (nOption == -1) {
				nOption = pCurrentCheat->nDefault;
			}

			if (pCurrentCheat->nType != 1) {

				// Return OK if the cheat is already active with the same option
				if (pCurrentCheat->nCurrent == nOption) {
					return 0;
				}

				// Deactivate old option (if any)
				pAddressInfo = pCurrentCheat->pOption[nOption]->AddressInfo;
				while (pAddressInfo->nAddress) {

					if (pAddressInfo->nCPU != nOpenCPU) {

						if (nOpenCPU != -1) {
							cpu_close();
						}

						nOpenCPU = pAddressInfo->nCPU;
						cpu_open(nOpenCPU);
					}

					// Write back original values to memory
					cpu_write_byte(pAddressInfo->nAddress, pAddressInfo->nOriginalValue);
					pAddressInfo++;
				}
			}

			// Activate new option
			pAddressInfo = pCurrentCheat->pOption[nOption]->AddressInfo;
			while (pAddressInfo->nAddress) {

				if (pAddressInfo->nCPU != nOpenCPU) {
					if (nOpenCPU != -1) {
						cpu_close();
					}

					nOpenCPU = pAddressInfo->nCPU;
					cpu_open(nOpenCPU);
				}

				// Copy the original values
				pAddressInfo->nOriginalValue = cpu_read_byte(pAddressInfo->nAddress);

				if (pCurrentCheat->nType != 0) {
					if (pAddressInfo->nCPU != nOpenCPU) {
						if (nOpenCPU != -1) {
							cpu_close();
						}

						nOpenCPU = pAddressInfo->nCPU;
						cpu_open(nOpenCPU);
					}

					// Activate the cheat
					cpu_write_byte(pAddressInfo->nAddress, pAddressInfo->nValue);
				}

				pAddressInfo++;
			}

			// Set cheat status and active option
			if (pCurrentCheat->nType != 1) {
				pCurrentCheat->nCurrent = nOption;
			}
			if (pCurrentCheat->nType == 0) {
				pCurrentCheat->nStatus = 2;
			}
			if (pCurrentCheat->nType == 2) {
				pCurrentCheat->nStatus = 1;
			}

			break;
		}
		pCurrentCheat = pCurrentCheat->pNext;
		nCurrentCheat++;
	}

	if (nOpenCPU != -1) {
		cpu_close();
	}

	CheatUpdate();

	if (nCurrentCheat == nCheat && pCurrentCheat) {
		return 0;
	}

	return 1;
}

int CheatApply()
{
	if (!bCheatsEnabled) {
		return 0;
	}

	int nOpenCPU = -1;

	CheatInfo* pCurrentCheat = pCheatInfo;
	CheatAddressInfo* pAddressInfo;
	while (pCurrentCheat) {
		if (pCurrentCheat->nStatus > 1) {
			pAddressInfo = pCurrentCheat->pOption[pCurrentCheat->nCurrent]->AddressInfo;
			while (pAddressInfo->nAddress) {

				if (pAddressInfo->nCPU != nOpenCPU) {
					if (nOpenCPU != -1) {
						cpu_close();
					}

					nOpenCPU = pAddressInfo->nCPU;
					cpu_open(nOpenCPU);
				}

				cpu_write_byte(pAddressInfo->nAddress, pAddressInfo->nValue);
				pAddressInfo++;
			}
		}
		pCurrentCheat = pCurrentCheat->pNext;
	}

	if (nOpenCPU != -1) {
		cpu_close();
	}

	return 0;
}

int CheatInit()
{
	nCpuType = -1;

	CheatExit();

	bCheatsEnabled = false;

	return 0;
}

void CheatExit()
{
	if (pCheatInfo) {
		CheatInfo* pCurrentCheat = pCheatInfo;
		CheatInfo* pNextCheat;

		do {
			pNextCheat = pCurrentCheat->pNext;
			for (int i = 0; i < CHEAT_MAX_OPTIONS; i++) {
				free(pCurrentCheat->pOption[i]);
			}
			free(pCurrentCheat);
		} while ((pCurrentCheat = pNextCheat) != 0);
	}

	pCheatInfo = NULL;
}

// Cheat search

static unsigned char *MemoryValues = NULL;
static unsigned char *MemoryStatus = NULL;
static UINT32 nMemorySize = 0;

#define NOT_IN_RESULTS	0
#define IN_RESULTS	1

unsigned int CheatSearchShowResultAddresses[CHEATSEARCH_SHOWRESULTS];
unsigned int CheatSearchShowResultValues[CHEATSEARCH_SHOWRESULTS];

int CheatSearchInit()
{
	set_cpu_type();
	
	switch (nCpuType) {
		case 1: {
			// sek
			nMemorySize = 0x01000000;
			break;
		}
		
		case 2: {
			// vez
			nMemorySize = 0x00100000;
			break;
		}
		
		case 3: {
			// sh-2
			nMemorySize = 0x02080000; // We only have CPS-3 on this CPU and nothing interesting happens after this range for cheats
			break;
		}

		case 4: {
			// m6502
			nMemorySize = 0x00010000;
			break;
		}
		
		case 5: {
			// zet
			nMemorySize = 0x00010000;
			break;
		}
	}

	return nCpuType;
}

void CheatSearchExit()
{
	free(MemoryValues);
	MemoryValues = NULL;
	free(MemoryStatus);
	MemoryStatus = NULL;
	
	nMemorySize = 0;
	
	memset(CheatSearchShowResultAddresses, 0, CHEATSEARCH_SHOWRESULTS);
	memset(CheatSearchShowResultValues, 0, CHEATSEARCH_SHOWRESULTS);
}

void CheatSearchStart()
{
	UINT32 nAddress;
	
	MemoryValues = (unsigned char*)malloc(nMemorySize);
	MemoryStatus = (unsigned char*)malloc(nMemorySize);
	
	int nActiveCPU = 0;
	
	if (nCpuType != 3) {
		nActiveCPU = cpu_get_active();
		cpu_close();
		cpu_open(0);
	}
	
	for (nAddress = 0; nAddress < nMemorySize; nAddress++) {
		MemoryValues[nAddress] = cpu_read_byte(nAddress);
	}
	
	if (nCpuType != 3) {
		cpu_close();
		cpu_open(nActiveCPU);
	}
			
	memset(MemoryStatus, IN_RESULTS, nMemorySize);
}

static void CheatSearchGetResults()
{
	UINT32 nAddress;
	unsigned int nResultsPos = 0;
	
	memset(CheatSearchShowResultAddresses, 0, CHEATSEARCH_SHOWRESULTS);
	memset(CheatSearchShowResultValues, 0, CHEATSEARCH_SHOWRESULTS);
	
	for (nAddress = 0; nAddress < nMemorySize; nAddress++) {		
		if (MemoryStatus[nAddress] == IN_RESULTS) {
			CheatSearchShowResultAddresses[nResultsPos] = nAddress;
			CheatSearchShowResultValues[nResultsPos] = MemoryValues[nAddress];
			nResultsPos++;
		}
	}
}

unsigned int CheatSearchValueNoChange()
{
	unsigned int nMatchedAddresses = 0;
	UINT32 nAddress;
	
	int nActiveCPU = 0;
	
	if (nCpuType != 3) {
		nActiveCPU = cpu_get_active();
		cpu_close();
		cpu_open(0);
	}
	
	for (nAddress = 0; nAddress < nMemorySize; nAddress++) {
		if (MemoryStatus[nAddress] == NOT_IN_RESULTS) continue;
		if (cpu_read_byte(nAddress) == MemoryValues[nAddress]) {
			MemoryValues[nAddress] = cpu_read_byte(nAddress);
			nMatchedAddresses++;
		} else {
			MemoryStatus[nAddress] = NOT_IN_RESULTS;
		}
	}

	if (nCpuType != 3) {	
		cpu_close();
		cpu_open(nActiveCPU);
	}
	
	if (nMatchedAddresses <= CHEATSEARCH_SHOWRESULTS) CheatSearchGetResults();
	
	return nMatchedAddresses;
}

unsigned int CheatSearchValueChange()
{
	unsigned int nMatchedAddresses = 0;
	UINT32 nAddress;
	
	int nActiveCPU = 0;
	
	if (nCpuType != 3) {
		nActiveCPU = cpu_get_active();
		cpu_close();
		cpu_open(0);
	}
	
	for (nAddress = 0; nAddress < nMemorySize; nAddress++) {
		if (MemoryStatus[nAddress] == NOT_IN_RESULTS) continue;
		if (cpu_read_byte(nAddress) != MemoryValues[nAddress]) {
			MemoryValues[nAddress] = cpu_read_byte(nAddress);
			nMatchedAddresses++;
		} else {
			MemoryStatus[nAddress] = NOT_IN_RESULTS;
		}
	}
	
	if (nCpuType != 3) {
		cpu_close();
		cpu_open(nActiveCPU);
	}
	
	if (nMatchedAddresses <= CHEATSEARCH_SHOWRESULTS) CheatSearchGetResults();
	
	return nMatchedAddresses;
}

unsigned int CheatSearchValueDecreased()
{
	unsigned int nMatchedAddresses = 0;
	UINT32 nAddress;
	
	int nActiveCPU = 0;
	
	if (nCpuType != 3) {
		nActiveCPU = cpu_get_active();
		cpu_close();
		cpu_open(0);
	}

	for (nAddress = 0; nAddress < nMemorySize; nAddress++) {
		if (MemoryStatus[nAddress] == NOT_IN_RESULTS) continue;
		if (cpu_read_byte(nAddress) < MemoryValues[nAddress]) {
			MemoryValues[nAddress] = cpu_read_byte(nAddress);
			nMatchedAddresses++;
		} else {
			MemoryStatus[nAddress] = NOT_IN_RESULTS;
		}
	}

	if (nCpuType != 3) {	
		cpu_close();
		cpu_open(nActiveCPU);
	}
	
	if (nMatchedAddresses <= CHEATSEARCH_SHOWRESULTS) CheatSearchGetResults();
	
	return nMatchedAddresses;
}

unsigned int CheatSearchValueIncreased()
{
	unsigned int nMatchedAddresses = 0;
	UINT32 nAddress;
	
	int nActiveCPU = 0;
	
	if (nCpuType != 3) {
		nActiveCPU = cpu_get_active();
		cpu_close();
		cpu_open(0);
	}
	
	for (nAddress = 0; nAddress < nMemorySize; nAddress++) {
		if (MemoryStatus[nAddress] == NOT_IN_RESULTS) continue;
		if (cpu_read_byte(nAddress) > MemoryValues[nAddress]) {
			MemoryValues[nAddress] = cpu_read_byte(nAddress);
			nMatchedAddresses++;
		} else {
			MemoryStatus[nAddress] = NOT_IN_RESULTS;
		}
	}
	
	if (nCpuType != 3) {
		cpu_close();
		cpu_open(nActiveCPU);
	}
	
	if (nMatchedAddresses <= CHEATSEARCH_SHOWRESULTS) CheatSearchGetResults();
	
	return nMatchedAddresses;
}

void CheatSearchDumptoFile()
{
	FILE *fp = fopen("cheatsearchdump.txt", "wt");
	UINT32 nAddress;
	
	if (fp) {
		char Temp[256];
		
		for (nAddress = 0; nAddress < nMemorySize; nAddress++) {
			if (MemoryStatus[nAddress] == IN_RESULTS) {
				sprintf(Temp, "Address %08X Value %02X\n", nAddress, MemoryValues[nAddress]);
				fwrite(Temp, 1, strlen(Temp), fp);
			}
		}
		
		fclose(fp);
	}
}

#undef NOT_IN_RESULTS
#undef IN_RESULTS
