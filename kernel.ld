/*
*   链接脚本
*   2020-5-1
*   by qzt
*/


ENTRY(_start)
SECTIONS {
    . = 0xc0000000;
    _kstart = .;
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
    _kend = .;


    /*
    *
    * 内核空间最后128M空间用于映射硬件的空间, 比如说显存空间等
    * 4G-128M
    */
    . = 0xf8000000;
    _VCGAMEM = .;
    /*
    * 往后32K需要空出给CGA显存
    */
    . = 0xf8008000;
    _EVCGAMEM = .;
}