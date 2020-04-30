ENTRY(_start)
SECTIONS {
    . = 0xc0000400;
    .text : {
        *(.text)
    }

    . = ALIGN(4);
    .bss : {
    *(.bss)
    }

    . = ALIGN(4);
    _end = .;
}