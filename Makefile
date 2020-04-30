# makefile for kernel

# kernel entry point
# KRLENTRYPOINT		= 0x100400
KRLENTRYPOINT		= 0xc0000400
LINKSCRIPT			= kernel.ld

#
KRLENTRYOFFSET		= 0x400

# program,flags,etc
ASM			= nasm
DASM		= ndisasm
CC			= gcc
LD			= ld
ASMBFLAGS		= -I boot/include/
ASMKFLAGS		= -I include/ -f elf
CFLAGS			= -I include/ -m32 -c -fno-builtin
LDFLAGS			= -s -m elf_i386 -T$(LINKSCRIPT)
DASMFLAGS		= -u -o $(KRLENTRYPOINT) -e $(KRLENTRYOFFSET)

# This Program
QXOSBOOT		= boot/boot.img boot/LOADER.BIN
QXOSKERNEL		= kernel/QXOSKRL.BIN
OBJS			= kernel/kernel.o kernel/kernel2.o kernel/kernel3.o \
					kernel/kernel4.o kernel/start.o kernel/i8259.o \
					kernel/global.o kernel/protect.o kernel/hwint.o \
					kernel/proc.o kernel/vm.o kernel/main.o \
					kernel/main_sam.o kernel/syscall.o lib/krllibc.o \
					lib/krlliba.o
DASMOUTPUT		= kernel.bin.asm

# All phony targets
.PHONY:			everything final image clean realclean disasm all buildimg run

# Default starting position
everything : $(QXOSBOOT) $(QXOSKERNEL)

all:	realclean everything

final:	all clean

image : final buildimg

clean :
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(QXOSBOOT) $(QXOSKERNEL)

disasm:
	$(DASM) $(DASMFLAGS) $(QXOSKERNEL) > $(DASMOUTPUT)

run : image
	bochs

#

buildimg:
	sudo mount -o loop boot/boot.img floppy/
	sudo cp -fv boot/LOADER.BIN floppy/
	sudo cp -fv kernel/QXOSKRL.BIN floppy/
	sudo umount floppy/

boot/boot.img: boot/boot.asm boot/include/define.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/LOADER.BIN: boot/loader.asm boot/include/define.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

kernel/kernel.o: kernel/kernel.asm include/kernel.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/kernel2.o: kernel/kernel2.asm include/kernel.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/kernel3.o: kernel/kernel3.asm include/kernel.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/kernel4.o: kernel/kernel4.asm include/kernel.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/main_sam.o: kernel/main_sam.asm include/kernel.inc
	$(ASM) $(ASMKFLAGS) -o $@ $<

lib/krlliba.o : lib/krlliba.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

$(QXOSKERNEL) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(QXOSKERNEL) $(OBJS)