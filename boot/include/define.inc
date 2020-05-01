; fat12文件系统

; 使用到的参数
; FAT表占用扇区数量
FAT_TABLE_CNT	equ	9
; FAT表数量
FAT_NUM		equ	2
; 扇区大小
SECTION_SIZE	equ	512
; FAT12根目录文件数量
ROOT_FILE_CNT	equ	0xE0
; 根目录条目长度
FILE_ITEM_LEN	equ	32
; 根目录占用扇区数
ROOT_SEC_CNT	equ	(ROOT_FILE_CNT * FILE_ITEM_LEN + SECTION_SIZE - 1) / SECTION_SIZE
; 软盘大小
FLOPPY_SIZE	equ	0x168000
; 每条磁道扇区数量
SECS_PER_TRK	equ	0x12
; 驱动器号
DRV_NUM		equ	0
; 加载地址 48K 长度8K; FAT表加载到40K的位置，长度8k
;		Memory
;	---------0x7c00
;	引导扇区(512字节)
;	---------0x8000 32K
;	根目录加载区
;	---------0xa000 40K
;	FAT表
;	---------0xc000	48K
;	ARDS加载区域
;	---------0xe000 52K
;	loader.bin
;	---------0x90000 576K
;	kernel.bin
LOAD_ADR_BASE		equ	0
LOAD_ADR_OFF		equ	0x8000
LOAD_FAT_ADR_OFF	equ	0xa000
LOADER_LOAD_ADR_BASE	equ	0
LOADER_LOAD_ADR_OFF	equ	0xe000
LOADER_LOAD_ADR_PHY	equ	((LOADER_LOAD_ADR_BASE << 4) + LOADER_LOAD_ADR_OFF)
; 文件名称长度
FILE_NAME_LEN	equ	11
; 起始簇号偏移
FstClus_Off	equ	0x1a
; 文件大小偏移
FILE_SIZE_OFF	equ	0x1c



; boot.asm中使用到的参数

; loader大小不超过8k
; loader名称LOADER.BIN

; 栈顶位置
TOP_OF_STACK	equ	0x7c00
; 屏幕每行字符数
CHARS_PER_LINE	equ	80
; 每个字符占用显存数量
PER_CHAR_LEN	equ	2
; 字符显示颜色 黑底白字
CHAR_COLOR	equ	0x0f
; 显存位置
GM_BASE		equ	0xb800
GM_OFF		equ	0x0

; loader文件名
%define	LOADER_NAME	"LOADER  BIN"


; loder.asm中使用到的参数
; QXKERNEL.BIN的大小限制不超过64K

KERNEL_LOAD_ADDR_BASE	equ	0x9000
KERNEL_LOAD_ADDR_OFF	equ	0x0
KERNEL_LOAD_ADDR_PHY		equ	((KERNEL_LOAD_ADDR_BASE << 4) + KERNEL_LOAD_ADDR_OFF)


ARDS_LOW_BASE_OFF	equ	0x0
ARDS_HIGH_BASE_OFF	equ	0x4
ARDS_LOW_LEN_OFF	equ	0x8
ARDS_HIGH_LEN_OFF	equ	0xc
ARDS_TYPE_OFF		equ	0x10

ARDSSIZE		equ	20

MEMORYINFOCACHENUM	equ	20

MEMORYINFOCACHESIZE	equ	(ARDSSIZE * MEMORYINFOCACHENUM)

; 直接放到 0xe000
; ARDS缓冲区(400字节)20个ARDS
MemoryInfoCache	equ	0xc000
; ARDS计数(2字节)
ARDS_CNT	equ	0xc000+MEMORYINFOCACHESIZE
; 物理地址大小(4字节)
MEM_SIZE	equ	0xc000+MEMORYINFOCACHESIZE+2

%define	KERNEL_NAME	"QXOSKRL BIN"

; 分页所使用的参数
; 
; 页目录放置在64K的位置，而页表放置在68K的位置
; 页目录地址
PDE_ADDR_BASE		equ	0x1000
PDE_ADDR_OFF		equ	0x0
PDE_ADDR_PHY		equ	0x10000
; 页表地址
PTE_ADDR_BASE		equ	0x1100
PTE_ADDR_OFF		equ	0x0
PTE_ADDR_PHY		equ	0x11000

PG_ITEM_LEN		equ	0x4
PG_ITEM_NUM		equ	0x400

PAGE_SIZE		equ	0x1000

; 分页相关参数
PG_P			equ	0x1
PG_RW			equ	0x2
PG_USER			equ	0x4
PG_PWT_WRITE_THROUGH	equ	0x8
PG_PCD_NO_CACHE		equ	0x10
PG_G			equ	0x100

; ELF相关参数
; ELF HEADER相关参数 52字节长度
ELF_HEADER_IDENT_OFF		equ	0x0
ELF_HEADER_TYPE_OFF		equ	0x10
ELF_HEADER_MACHINE_OFF		equ	0x12
ELF_HEADER_VERSION_OFF		equ	0x14
ELF_HEADER_ENTRY_OFF		equ	0x18
ELF_HEADER_PHOFF_OFF		equ	0x1c
ELF_HEADER_SHOFF_OFF		equ	0x20
ELF_HEADER_FLAGS_OFF		equ	0x24
ELF_HEADER_EHSIZE_OFF		equ	0x28
ELF_HEADER_PHENTSIZE_OFF	equ	0x2a
ELF_HEADER_PHNUM_OFF		equ	0x2c
ELF_HEADER_SHENTSIZE_OFF	equ	0x2e
ELF_HEADER_SHNUM_OFF		equ	0x30
ELF_HEADER_SHSTRNDX_OFF		equ	0x32

ELF_IDENT_4BYTES		equ	0x464c457f

; PROGRAM HEADER相关参数	32字节长度
ELF_PROGRAM_HEADER_TYPE_OFF	equ	0x0
ELF_PROGRAM_HEADER_OFFSET_OFF	equ	0x4
ELF_PROGRAM_HEADER_VADDR_OFF	equ	0x8
ELF_PROGRAM_HEADER_PADDR_OFF	equ	0xc
ELF_PROGRAM_HEADER_FILESZ_OFF	equ	0x10
ELF_PROGRAM_HEADER_MEMSZ_OFF	equ	0x14
ELF_PROGRAM_HEADER_FLAGS_OFF	equ	0x18
ELF_PROGRAM_HEADER_ALIGN_OFF	equ	0x1c




; 描述符宏（64位）	参数分别为：	基址32位	界限20位	属性16位
%macro Descriptor 3
	dw	%2 & 0xffff	;段界限1
	dw	%1 & 0xffff
	db	(%1 >> 16) & 0xff ;基址1
	dw	(%3 & 0xf0ff) | ((%2 >> 8) & 0x0f00)
	db	(%1 >> 24)
%endmacro

; 描述符相关参数
; DA 表示Descriptor Attribute
DA_32		equ	0x4000
DA_LIMIT_4K	equ	0x8000
DA_DRW		equ	0x92
DA_C		equ	0x98
DA_CR		equ	0x9a

; DPL
DA_DPL3		equ	60h

; 内核实际加载基址 1M
K_P_BASE_ADDR	equ	0x100000
; 内核空间虚拟地址基址 3G
K_V_BASE_ADDR	equ	0xc0000000