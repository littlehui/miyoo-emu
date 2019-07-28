PRGNAME     = potator

# define regarding OS, which compiler to use
EXESUFFIX = 
TOOLCHAIN = 
CC          = gcc
CCP         = g++
LD          = gcc

# add SDL dependencies
SDL_LIB     = 
SDL_INCLUDE = 

# change compilation / linking flag options
F_OPTS		=  -DHAVE_LOCALE -DUSE_PTHREADS -DM_CORE_GBA -DM_CORE_GB -DUSE_PNG -DCOLOR_16_BIT -DCOLOR_5_6_5 -Isrc -Iinclude -Iinclude/mgba-util -I/usr/include/SDL
CC_OPTS		= -O2 -fomit-frame-pointer -fdata-sections -ffunction-sections $(F_OPTS)
CFLAGS		= -I$(SDL_INCLUDE) $(CC_OPTS)
CXXFLAGS	=$(CFLAGS) 
LDFLAGS     = -lSDL -lm -Wl,--as-needed -Wl,--gc-sections -flto -lz -lpng -pthread

#./src/debugger
# Files to be compiled
SRCDIR    = ./src ./src/arm ./src/core ./src/util/vfs ./src/platform/sdl ./src/platform/posix
SRCDIR	 += ./src/feature ./src/feature/ffmpeg ./src/feature/gui 
SRCDIR   += ./src/feature/sqlite3 ./src/gb ./src/gb/debugger ./src/gb/extra ./src/gb/renderers
SRCDIR	+= ./src/gb/sio ./src/gba ./src/gba/debugger ./src/gba/cheats ./src/gba/extra ./src/gba/renderers
SRCDIR	+= ./src/gba/rr ./src/gba/sio ./src/lr35902 ./src/lr35902/debugger 
SRCDIR	+= ./src/third-party/blip_buf ./src/third-party/inih ./src/util ./src/util/gui ./version

VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CP   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C   = $(patsubst %.c, %.o, $(SRC_C))
OBJ_CP   = $(patsubst %.cpp, %.o, $(SRC_CP))
OBJS     = $(OBJ_C) $(OBJ_CP)

# Rules to make executable
$(PRGNAME)$(EXESUFFIX): $(OBJS)  
	$(LD) $(CFLAGS) -o $(PRGNAME) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_CP) : %.o : %.cpp
	$(CCP) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) $(OBJ_C) $(OBJ_CP)
