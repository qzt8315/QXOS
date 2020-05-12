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
    HIGHMEM = .;
    _VCGAMEM = .;
    /*
    * 往后32K需要空出给CGA显存
    */
    . = 0xf8008000;
    _EVCGAMEM = .;

    /*
    *   4M空间用于暂时映射内核分页,以后所有进程的分页都直接拷贝这个PDE,再在这个PDE上修改
    *   
    */
    . = 0xf8400000;
    KERNEL_PAGE_POS = .;

    . = 0xf8000000+0x800000;
    /*
    *   vmalloc 给20MB虚拟地址空间
    */
    VMALLOCSPACE    = .;
    . = 0xf9000000;
    _EVMALLOCSPACE  = .;
    /*
    *   8MB隔离vmalloc
    */
    . = 0xf9800000;
    /*
    *   动态地址，用于访问大于1G内存地址
    */
    DYNAMICADDR     = .;
    . = 0xffffffff;
    _EDYNAMICADDR   = .;

}