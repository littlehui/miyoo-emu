# gpSP makefile
# Gilead Kutnick - Exophase
# pandora port - notaz
# respberry pi - DPR

# Global definitions

CC        = arm-linux-gcc

OBJS      = main.o cpu.o memory.o video.o input.o sound.o gui.o cc_lut.o\
            cheats.o zip.o  arm_stub.o  warm.o cpu_threaded.o video_blend.o sha1.o imageio.o

BIN       = gpsp

# Platform specific definitions 
VPATH      += .. ../arm
CFLAGS     += -DARM_ARCH -DPC_BUILD -Wall -march=armv5te -mtune=arm926ej-s
CFLAGS     += -Ofast -fdata-sections -ffunction-sections -fno-PIC
CFLAGS     += `/opt/miyoo/arm-miyoo-linux-uclibcgnueabi/sysroot/usr/bin/sdl-config --cflags`

# expecting to have PATH set up to get correct sdl-config first
LIBS       += `/opt/miyoo/arm-miyoo-linux-uclibcgnueabi/sysroot/usr/bin/sdl-config --libs`
LIBS       += -ldl -lpthread -lz -lgcov -lasound -Wl,--as-needed -Wl,--gc-sections -flto -s -lpng

# Compilation:
all: $(BIN)

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

cpu.o cpu_threaded.o: CFLAGS += -Wno-unused-variable -Wno-unused-label

$(BIN): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(BIN)

clean:
	rm -f *.o $(BIN)
