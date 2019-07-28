# gpSP makefile
# Gilead Kutnick - Exophase

# Global definitions

CC        = gcc
STRIP     = strip
AS        = as

PREFIX    = /usr
OBJS      = main.o cpu.o memory.o video.o input.o sound.o \
	    cpu_threaded.o gui.o x86_stub.o cheats.o zip.o
BIN       ?= gpsp 

# Platform specific definitions 

VPATH      += ..
CFLAGS     += -DPC_BUILD -Wall -m32
INCLUDES   = `sdl-config --cflags`
LIBS       = `sdl-config --libs` -lz -m32

# Compilation:

.SUFFIXES: .c .S

all:	${BIN}

%.o: %.c
	${CC} ${CFLAGS} ${INCLUDES} -c -o $@ $<

%.o: %.S
	${CC} ${CFLAGS} -c -o $@ $<

cpu.o cpu_threaded.o: CFLAGS += -Wno-unused-variable -Wno-unused-label

${BIN}:	${OBJS}
	${CC} ${OBJS} ${LIBS} -o ${BIN}  
#	${STRIP} ${BIN}

clean:
	rm -f *.o ${BIN} 

