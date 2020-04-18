; 从磁盘中载入Loader，并将控制权由引导扇区交给loader

; 使用命令 mount -o loop /home/test/QXOS/test/pmtest10.bin /mnt/floppy/ 直接加载镜像
; loader大小不超过8k
; loader名称LOADER.BIN

; 注意：扇区号从1开始，而不是0

; 用于测试引导扇区大小是否超出512字节
; %define __TEST_BIN_SIZE__

%include "define.inc"



; 挂载位置
org	0x7c00
jmp	short START_LABEL
nop


; BPB
BS_OEMName	db	"QX      "
BPB_BytesPerSec	dw	0x0200
BPB_SecPerClus	db	1
BPB_RsvdSecCnt	dw	1
BPB_NumFATs	db	FAT_NUM
BPB_RootEntCnt	dw	ROOT_FILE_CNT
BPB_TotSec16	dw	0xB40
BPB_Media	db	0xF0
BPB_FATSz16	dw	FAT_TABLE_CNT
BPB_SecPerTrk	dw	SECS_PER_TRK
BPB_NumHeads	dw	2
BPB_HiddSec	dd	0
BPB_TotSec32	dd	0
BS_DrvNum	db	DRV_NUM
BS_Reserved1	db	0
BS_BootSig	db	1
BS_VolID	dd	0
BS_VolLab	db	"QXOS       "
BS_FileSysType	db	"FAT12   "

START_LABEL:
	; 找到loader，加载到32K的地址的位置，并将控制权交给loader
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	esp, TOP_OF_STACK
	
	; 设置显存位置
	mov	ax, 0xb800
	mov	gs, ax
	call	CLS
	
	; 测试栈
	; push	0xffff

	; 显示finding loader
	;push	STR0_LEN
	;push	STR0
	;call	SHOW_STR
	;add	sp, 4

	; 软驱复位
	xor	ah, ah
	xor	dl, dl
	int	13h

	; 加载根目录分区
	; 缓冲区地址
	mov	ax, LOAD_ADR_BASE
	mov	es, ax
	mov	bx, LOAD_ADR_OFF
	; 根目录起始扇区号
	mov	ax, 1+FAT_TABLE_CNT * FAT_NUM
	; 根目录长度
	mov	cl, ROOT_SEC_CNT
	; 磁盘标号
	mov	dl, DRV_NUM
	; BIOS加载根目录
	call	LOAD_SECTION

	; 在根目录节点下查找是否有LOADER.BIN文件
	mov	cx, ROOT_FILE_CNT
	mov	ax, LOAD_ADR_OFF

.loop:
	push	ax
	call	FIND_LOADER_FILE
	cmp	ax, 0
	pop	ax
	jne	LOAD_LOADER
	add	ax, FILE_ITEM_LEN
	loop	.loop
	
	; 显示not found loader
	push	STR2_LEN
	push	STR2
	call	SHOW_STR
	add	sp, 4
	
	; 未找到LOADER	
	jmp	$

LOAD_LOADER:
	; 显示found loader
	push	STR3_LEN
	push	STR3
	call	SHOW_STR
	add	sp, 4

	; 获取文件大小和开始簇号
	xor	edi, edi
	mov	di, ax
	add	di, FILE_SIZE_OFF + 2
	;循环3次
	mov	cx, 3
save_info:
	push	word [es:edi]
	sub	edi, 2
	loop	save_info
	
	mov	bp, sp

	; 加载FAT表
	mov	ax, 1
	mov	bx, LOAD_FAT_ADR_OFF
	mov	cl, FAT_TABLE_CNT
	mov	dl, DRV_NUM
	call	LOAD_SECTION
	
	
	mov	ax, word [bp]
	mov	word [load_addr], LOADER_LOAD_ADR_OFF
loop_load:
	push	ax
	sub	ax, 2
	add	ax, 1 + FAT_TABLE_CNT * 2
	add	ax, ROOT_SEC_CNT
	; 加载loader到内存
	mov	bx, [load_addr]
	mov	cl, 1
	mov	dl, DRV_NUM
	call	LOAD_SECTION

	; 查找FAT表, 是否加载所有扇区
	pop	ax
	push	ax
	mov	bx, 3
	mul	bx
	mov	bx, 2
	div	bx
	mov	di, ax
	add	di, LOAD_FAT_ADR_OFF
	mov	ax, [es:di]
	pop	bx
	and	bx, 1
	cmp 	bx, 0
	jz	even
	shr	ax, 4
	
; 偶数
even:
	add	word[load_addr], SECTION_SIZE
	and	ax, 0x0fff
	cmp	ax, 0xff8
	jc	loop_load
	
	
	; 显示jump to loader
	push	STR4_LEN
	push	STR4
	call	SHOW_STR
	; 同时去掉之前保存的loader文件信息
	add	sp, 4+6
	
	; 控制权交给loader
	jmp	LOAD_ADR_BASE:LOADER_LOAD_ADR_OFF

load_addr: dw	LOADER_LOAD_ADR_OFF



; ax:读取起始扇区 cl:读取扇区个数， es:bx指定缓存位置 dl驱动器号 注意：使用前注意需要进行软驱复位操作
LOAD_SECTION:
	; 保存栈顶位置, 并留出两个字节用于保存读取扇区号
	push	bp
	mov	bp, sp
	sub	esp, 2

	; 保存需读取扇区数
	mov	byte [bp-2], cl
	
	; 获取磁道号，磁头号，起始扇区号
	; 除数	
	push	bx
	mov	bl, SECS_PER_TRK
	
	; 做除法
	push	dx
	xor	dx, dx
	div	bl

	; 恢复dx
	pop	dx
	; 恢复bx
	pop	bx
	;起始扇区
	inc	ah
	mov	cl, ah
	;获取磁头号
	mov	dh, al
	and	dh, 1
	; 获取磁道号
	mov	ch, al
	shr	ch, 1
.go_read:
	mov	ah, 02h
	; 填入需要读取的扇区数
	mov	al, byte[bp-2]
	int	13h
	jc	.go_read
	
	; 恢复sp
	add	esp, 2
	; 恢复bp
	pop	bp

	ret

; 检查某条根目录记录是否是要寻找的内容,内容为LOADER_STR ax:存储条目的开始内存地址 返回0则不在该条目中，返回非零则在该条目录中，数值为簇号
FIND_LOADER_FILE:
	push	cx
	push	si
	push	di
	push	ax
	mov	cx, FILE_NAME_LEN
	mov	si, LOADER_NAME_STR
	mov	di, ax
	cld
.cmp_str:
	; ds:si
	lodsb
	cmp	al, byte [es:di]
	jne	ret_zero
	dec	cx
	inc	di
	cmp	cx, 0
	jne	.cmp_str
	; 确认是该目录返回簇号
	pop	si
	add	si, FstClus_Off
	mov	ax, word [es:si]

	; 恢复数据
	pop	di
	pop	si
	pop	cx
	ret
; 该记录不是需要的文件
ret_zero:
	pop	ax
	pop	di
	pop	si
	pop	cx
	mov	ax, 0
	ret

LOADER_NAME_STR:
	db	LOADER_NAME
	

; 显示一个字符串，并换行 void SHOW_STR(char* str, int len)

SHOW_STR:
	mov	bp, sp
	push	ax
	push	bx
	push	cx
	push	dx
	; 段内跳转 cs不会入栈
	; mov	cx, word [bp+4]
	; mov	ax, word [bp+6]
	mov	cx, word [bp+4]
	mov	ax, word [bp+2]
	mov	si, ax
	; 获取行号
	mov	ax,word [CUR_LINE]
	push	ax
	xor	dx, dx
	mov	bl, CHARS_PER_LINE * 2
	mul	bl
	xor	edi, edi
	mov	di, ax
	; 换行
	pop	ax
	inc	ax
	mov	word [CUR_LINE], ax
	; 设置字体颜色
	mov	ah, CHAR_COLOR
show_a_char:
	lodsb
	; 修改显存
	mov	[gs:edi], ax
	add	edi, 2
	loop	show_a_char
	
	pop	dx
	pop	cx
	pop	bx
	pop	ax
	ret
	
;STR0	db	"finding loader"
;STR0_LEN	equ	$ - STR0

STR2	db	"not found loader"
STR2_LEN	equ	$ - STR2

STR3	db	"found loader"
STR3_LEN	equ	$ - STR3

STR4	db	"jump to loader"
STR4_LEN	equ	$ - STR4

; 当前显示行数
CUR_LINE	dw	0

; 清屏，并将写屏行数恢复到第一行
CLS:
	mov	ax, 0600h
	mov	bx, 0700h
	mov	cx, 0
	mov	dx, 0148fh
	int	10h
	
	ret

%ifndef	__TEST_BIN_SIZE__
times	510-($-$$)	db 0
dw 0xAA55

; FAT1
dw	0xffff
dw	0xffff
db	0x0f
times	FAT_TABLE_CNT*SECTION_SIZE-5 db 0

; FAT2
dw	0xffff
dw	0xffff
db	0x0f
times	FAT_TABLE_CNT*SECTION_SIZE-5 db 0

; 根目录区
times	32	db	0

FILE_NAME	db	LOADER_NAME
FILE_ATTR	db	0x20
RE times 10	db	0
WRT_TIME	dw	0
WRT_DATE	dw	0
FISRT_CLUSTER	dw	2
FILE_SIZE	dd	LOADER_SIZE
times	ROOT_FILE_CNT*FILE_ITEM_LEN-64 db 0

; 数据区
SHOW_P:
	mov	edi, 80*2*2
	mov	ah, CHAR_COLOR
	mov	al, 'p'
	mov	[gs:edi], ax
	
	jmp	$	

LOADER_SIZE	equ	$-SHOW_P

times	FLOPPY_SIZE-($-$$) db 0
%endif
