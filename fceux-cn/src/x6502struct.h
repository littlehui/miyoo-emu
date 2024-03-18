#ifndef _X6502STRUCTH
#define _X6502STRUCTH

typedef union {
struct {
uint16 _timestamp;
uint16 _soundtimestamp;
};
uint32 val;
}TSPDef;

typedef struct __X6502 {
  uint16 tcount;     /* Temporary cycle counter */
  uint16 PC;        /* I'll change this to uint32 later... */
                                /* I'll need to AND PC after increments to 0xFFFF */
                                /* when I do, though.  Perhaps an IPC() macro? */
        uint8 A,X,Y,S,P,mooPI;
        uint8 jammed;
  uint8 DB;         /* Data bus "cache" for reads from certain areas */

	int32 count;
  uint32 IRQlow;    /* Simulated IRQ pin held low(or is it high?).
                                   And other junk hooked on for speed reasons.*/
	TSPDef tsp; 

  int preexec;      /* Pre-exec'ing for debug breakpoints. */

	#ifdef FCEUDEF_DEBUGGER
        void (*CPUHook)(struct __X6502 *);
        uint8 (*ReadHook)(struct __X6502 *, unsigned int);
        void (*WriteHook)(struct __X6502 *, unsigned int, uint8);
	#endif

}__attribute__((packed)) X6502;

#endif
