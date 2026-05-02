.SUFFIXES:

PSL1GHT := $(PSL1GHT)
PS3DEV := $(PS3DEV)

PPU_CC := $(PS3DEV)/ppu/bin/ppu-g++
PPU_AS := $(PS3DEV)/ppu/bin/ppu-as
PPU_LD := $(PS3DEV)/ppu/bin/ppu-ld
PPU_OBJCOPY := $(PS3DEV)/ppu/bin/ppu-objcopy

MAKE_SELF := $(PS3DEV)/bin/make_self
MAKE_PKG := $(PS3DEV)/bin/make_pkg

TARGET := playgame3

INCLUDES := -I$(PSL1GHT)/include -I$(PS3DEV)/ppu/include
CFLAGS := -O2 -Wall -mcpu=cell $(INCLUDES)
CXXFLAGS := $(CFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := -L$(PSL1GHT)/lib -L$(PS3DEV)/ppu/lib -specs=$(PSL1GHT)/ppu/lib/game.specs
LIBS := -lrsx -lgcm_sys -lsysutil -lio -lfs -lsysmodule -lm

CXXSRCS := \
	src/main.cpp \
	src/renderer.cpp \
	src/input.cpp \
	src/menu.cpp \
	src/games/pong.cpp \
	src/games/snake.cpp \
	src/games/asteroids.cpp \
	src/games/breakout.cpp \
	src/games/spaceinvaders.cpp

CXXOBJS := $(CXXSRCS:.cpp=.o)

.PHONY: all clean pkg iso

all: $(TARGET).self

$(TARGET).self: $(TARGET).elf
	$(MAKE_SELF) $(TARGET).elf $(TARGET).self

$(TARGET).elf: $(CXXOBJS)
	$(PPU_CC) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

.cpp.o:
	$(PPU_CC) $(CXXFLAGS) -c $< -o $@

pkg: $(TARGET).self
	$(MAKE_PKG) --contentid "UP0001-PLAYGAME3_00-0000000000000000" \
		--title "PlayGame3" \
		--title0 "PlayGame3 - 5 Vector Games Collection" \
		--category "HG" \
		--appver "01.00" \
		$(TARGET).pkg $(TARGET).self

iso: pkg
	mkdir -p ps3_root/PS3_GAME
	mkdir -p ps3_root/PS3_UPDATE
	cp -r pkg/* ps3_root/PS3_GAME/ 2>/dev/null || true
	$(MAKE_PKG) --contentid "UP0001-PLAYGAME3_00-0000000000000000" \
		--title "PlayGame3" \
		--title0 "PlayGame3" \
		ps3_root/PS3_GAME/USRDIR/playgame3.self $(TARGET).self
	@echo "ISO: Use ps3iso or genps3iso to create ISO from ps3_root/"

clean:
	rm -f $(CXXOBJS) $(TARGET).elf $(TARGET).self $(TARGET).pkg
	rm -rf ps3_root

install: pkg
	@echo "PKG created: $(TARGET).pkg"
