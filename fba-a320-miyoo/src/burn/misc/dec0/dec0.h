
#include "m6502.h"
//dec_vid.cpp
extern unsigned char *robocopSpriteRam;


extern unsigned char robocopSoundLatch;
extern unsigned int  *robocopPalette;
extern unsigned char *robocopSprites;
extern unsigned char *robocopChars;
extern unsigned char *robocopBG1;
extern unsigned char *robocopBG2;

extern unsigned char *dec0_pf1_data;
extern unsigned char *dec0_pf1_control_0;
extern unsigned char *dec0_pf1_control_1;
extern unsigned char *dec0_pf1_colscroll;
extern unsigned char *dec0_pf1_rowscroll;

extern unsigned char *dec0_pf2_data;
extern unsigned char *dec0_pf2_control_0;
extern unsigned char *dec0_pf2_control_1;
extern unsigned char *dec0_pf2_colscroll;
extern unsigned char *dec0_pf2_rowscroll;

extern unsigned char *dec0_pf3_data;
extern unsigned char *dec0_pf3_control_0;
extern unsigned char *dec0_pf3_control_1;
extern unsigned char *dec0_pf3_colscroll;
extern unsigned char *dec0_pf3_rowscroll;

extern unsigned char *robocopPaletteRam1;
extern unsigned char *robocopPaletteRam2;


int decvid_init();
int decvid_exit();
int decvid_calcpal();
void decvid_drawsprites(int pri_mask,int pri_val);
void decvid_drawchars();
void decvid_drawbg1();
void decvid_drawbg2();

void decodechars(unsigned char * rBitmap, unsigned char *pbBits, int size, int numberoftiles);
void decodesprite(unsigned char * rBitmap, unsigned char *pbBits, int numberoftiles);
void decodeBG1(unsigned char * rBitmap, unsigned char *pbBits, int numberoftiles);
void decodeBG2(unsigned char * rBitmap, unsigned char *pbBits, int numberoftiles);


//dec_misc.cpp

extern unsigned short dec_pri_reg;
void dec_ctrl_writeword(unsigned int a, unsigned short d);
short dec_controls_read(unsigned int a);
extern bool VBL_ACK;

extern unsigned char  dec_inputbits0[9];
extern unsigned char  dec_inputbits1[9];
extern unsigned char  dec_inputbits2[9];

extern unsigned char  dec_dipsw[2];
extern unsigned short dec_input[3];

void DecMakeInputs();


void DecSharedInit();
void DecSharedExit();


// Dec_aud

void init_dec0_aud(int romNumPrg, int romNumSamples);
void reset_dec0_aud();
void render_dec_aud(int interleave, int cycleseg);

void endframe_dec_aud(int cycles);
void exit_dec0_aud();
