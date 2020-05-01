/*
*   链接脚本
*   2020-5-1
*   by qzt
*/


ENTRY(_start)
SECTIONS {
    . = 0xc0000400;
    .text : {
        *(.text)
    }
    _etext = .;

    . = ALIGN(4);
    .data : {
    *(.data)
    }

    .rodata : {
    *(.rodata)
    }

    .rdata : {
    *(.rdata)
    }
    . = ALIGN(4);
    _edata = .;

    .bss : {
    *(.bss)
    }

    . = ALIGN(4);
    _end = .;
}