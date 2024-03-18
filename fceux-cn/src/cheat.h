#ifndef _CHEAT_H_
#define _CHEAT_H_
#define MAX_cheat_keys  20
#define MAX_cjs_cheats  100
struct CJS_CHEAT {
	char *key_name;
	int key_add[MAX_cheat_keys];
	int key_value[MAX_cheat_keys];
};
typedef struct {
	char *cheat_name;
	int cheat_default;
    int cheat_max;
    CJS_CHEAT   cheat_key[MAX_cheat_keys];
} CJS_CHEATS;
extern CJS_CHEATS chj[MAX_cjs_cheats];
extern int cheat_file_num ;//0是第一个金手指。-1无
void FCEU_CheatResetRAM(void);
void FCEU_CheatAddRAM(int s, uint32 A, uint8 *p);

void FCEU_LoadGameCheats(FILE *override);
void FCEU_FlushGameCheats(FILE *override, int nosave);
void FCEU_ApplyPeriodicCheats(void);
void FCEU_PowerCheats(void);


int FCEU_CheatGetByte(uint32 A);
void FCEU_CheatSetByte(uint32 A, uint8 V);

void FCEU_CheatOn(void);
void FCEU_CheatOff(void);
void FCEU_Cheat_chj(void);
extern int savecheats;







#endif