#include "tiles_generic.h"
#include "dec0.h"
unsigned short dec_pri_reg=0; // graphics layer priority register 

unsigned char  dec_inputbits0[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char  dec_inputbits1[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char  dec_inputbits2[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned char  dec_dipsw[2]         = {0, 0};
unsigned short dec_input[3]       = {0x00,0x00,0x00};
unsigned char robocopSoundLatch = 0;

static int i8751_return;
bool VBL_ACK;

static void baddudes_i8751_write(int data)
{
	i8751_return=0;

	switch (data&0xffff) {
		case 0x714: i8751_return=0x700; break;
		case 0x73b: i8751_return=0x701; break;
		case 0x72c: i8751_return=0x702; break;
		case 0x73f: i8751_return=0x703; break;
		case 0x755: i8751_return=0x704; break;
		case 0x722: i8751_return=0x705; break;
		case 0x72b: i8751_return=0x706; break;
		case 0x724: i8751_return=0x707; break;
		case 0x728: i8751_return=0x708; break;
		case 0x735: i8751_return=0x709; break;
		case 0x71d: i8751_return=0x70a; break;
		case 0x721: i8751_return=0x70b; break;
		case 0x73e: i8751_return=0x70c; break;
		case 0x761: i8751_return=0x70d; break;
		case 0x753: i8751_return=0x70e; break;
		case 0x75b: i8751_return=0x70f; break;
	}

//	if (!i8751_return) logerror("%04x: warning - write unknown command %02x to 8571\n",activecpu_get_pc(),data);
}


short dec_controls_read(unsigned int a)
{
	int relVal =0 ;
	switch (a)
	{
		case 0: /* Player 1 & 2 joystick & buttons */
			return dec_input[0];
		//	return (readinputport(0) + (readinputport(1) << 8));

		case 2: /* Credits, start buttons */
			VBL_ACK = !VBL_ACK;
			relVal = dec_input[1] - (VBL_ACK<< 7);
			return relVal;
		//case 4: /* Byte 4: Dipswitch bank 2, Byte 5: Dipswitch Bank 1 */
		//	return (readinputport(3) + (readinputport(4) << 8));

		case 8: /* Intel 8751 mc, Bad Dudes & Heavy Barrel only */
			//return i8751_return;
			return i8751_return;
		default:
			break;
	}
	return ~0x00;
}

void dec_ctrl_writeword(unsigned int a, unsigned short d)
{
	switch (a)
	{
	case 0: // Playfield & Sprite priority 
	//	dec0_priority_w(0,data,mem_mask);
		dec_pri_reg=d;
		printf("priority reg: %x,%x\n",a,d);
		break;
	case 2:// DMA flag 
	//	dec0_update_sprites_w(0,0,mem_mask);
	//	printf("oh no, is dma: %x,%x\n",a,d);
		break;
	case 4: // 6502 
		robocopSoundLatch = d & 0xFF;
		bprintf(PRINT_NORMAL, _T("sound code: %x\n"),d);
		m6502SetIRQ(M6502_NMI);
		break;
	case 6:
		printf("write to i8751 %x, %x\n",a,d);
		baddudes_i8751_write(d);
		SekSetIRQLine(5, SEK_IRQSTATUS_AUTO);
		break;
	case 8: //mame says int ack for vbl, but just ignores it???
		VBL_ACK = false;
		break;
	case 0xa:  // unknown
		break;
	case 0xc: // coin block?
		break;
	case 0xe: //reset i8751??
		i8751_return=0;
		break;
	default:
		printf("unknown dec_ctrl_writeword %x,%x\n",a,d);
	}
}


void DecMakeInputs()
{
	// Reset Inputs
	dec_input[0] = dec_input[1] = dec_input[2] = 0xFFFF;
	// Compile Digital Inputs
	dec_input[0] -= (dec_inputbits0[0] & 1) << 0;
	dec_input[0] -= (dec_inputbits0[1] & 1) << 1;
	dec_input[0] -= (dec_inputbits0[2] & 1) << 2;
	dec_input[0] -= (dec_inputbits0[3] & 1) << 3;
	dec_input[0] -= (dec_inputbits0[4] & 1) << 4;
	dec_input[0] -= (dec_inputbits0[5] & 1) << 5;
	dec_input[0] -= (0) << 6;
	dec_input[0] -= (0) << 7;
	dec_input[0] -= (dec_inputbits1[0] & 1) << 8;
	dec_input[0] -= (dec_inputbits1[1] & 1) << 9;
	dec_input[0] -= (dec_inputbits1[2] & 1) << 10;
	dec_input[0] -= (dec_inputbits1[3] & 1) << 11;
	dec_input[0] -= (dec_inputbits1[4] & 1) << 12;
	dec_input[0] -= (dec_inputbits1[5] & 1) << 13;
	dec_input[0] -= (0) << 14;
	dec_input[0] -= (0) << 15;

	dec_input[1] -= (0) << 0;
	dec_input[1] -= (0) << 1;
	dec_input[1] -= (dec_inputbits2[2] & 1) << 2;
	dec_input[1] -= (dec_inputbits2[3] & 1) << 3;
	dec_input[1] -= (dec_inputbits2[4] & 1) << 4;
	dec_input[1] -= (dec_inputbits2[5] & 1) << 5;
	dec_input[1] -= (0) << 6;
	dec_input[1] -= (1) << 7;
}

void DecSharedInit()
{
	GenericTilesInit();
}

void DecSharedExit()
{
	GenericTilesExit();
}

