# makefile for boot

# program,flags,etc
ASM		= nasm
ASMFLAGS	= -I include/

# This program
TARGET		= boot.img LOADER.BIN

# all phony target
.PHONY : everything clean all


# Default starting position
everything : $(TARGET)

clean:
	rm -f $(TARGET)

all: clean everything

boot.img : boot.asm include/define.inc
	$(ASM) $(ASMFLAGS) -o $@ $<

LOADER.BIN : loader.asm include/define.inc
	$(ASM) $(ASMFLAGS) -o $@ $<


