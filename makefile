
CC = gcc
SOURCE = main.c cpu/fake6502.c assembler/asm6f.c apu/nes_apu.c machine/machine.c
CFLAGS = -I minifb/include -I minifb/src -I cpu -I sokol -I assembler -I apu -DASM_LIB -I . -I utils -I machine
MINIFB = minifb/src/*.c

ifeq ($(OS),Windows_NT)
	EXT = .exe
	MINIFB += minifb/src/windows/WinMiniFB.c 
	LDFLAGS = -l gdi32 -l ole32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		MINIFB += minifb/src/macosx/*.m
		CFLAGS += -DUSE_METAL_API
		LDFLAGS = -framework Cocoa -framework QuartzCore -framework Metal -framework MetalKit -framework AudioToolbox
	else ifeq ($(UNAME_S),Linux)
		MINIFB += minifb/src/x11/*.c
		LDFLAGS = -lX11
	endif
endif

minicube$(EXT) : $(SOURCE)
	$(CC) $(CFLAGS) $^ $(MINIFB) -s -Os $(LDFLAGS) -o $@
