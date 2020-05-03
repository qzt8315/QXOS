; 加载内核 进入保护模式
; 需要支持ELF格式内核


%include "define.inc"

; load加载位置， 32k
org	LOADER_LOAD_ADR_PHY
jmp	LOAD_START

;GDT
Descriptor_GDT:		Descriptor	0,		0,	0
Descriptor_CODE:	Descriptor	0,		0xfffff,	DA_32 | DA_LIMIT_4K | DA_CR
Descriptor_DATA:	Descriptor	0,		0xfffff,	DA_32 | DA_LIMIT_4K | DA_DRW
Descriptor_VIDEO:	Descriptor	0xb8000,	0xfffff,	DA_DRW| DA_DPL3

GDT_LEN		equ	$ - Descriptor_GDT

; GDT选择子
Selector_CODE	equ		Descriptor_CODE - Descriptor_GDT
Selector_DATA	equ		Descriptor_DATA - Descriptor_GDT
Selector_VIDEO	equ		Descriptor_VIDEO - Descriptor_GDT

GDP_PTR:	dw	GDT_LEN - 1
		dd	0
; ; ARDS缓冲区
; MemoryInfoCache:
; 	times	MEMORYINFOCACHESIZE	db	0
; ; ARDS计数
; ARDS_CNT:	dw	0
; ; 物理地址大小
; MEM_SIZE:	dd	0

LOAD_START:
	; 找到kernel，加载到0x90000的地址的位置，并将控制权交给kernel
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	esp, TOP_OF_STACK
	
	; 设置显存位置
	mov	ax, GM_BASE
	mov	gs, ax
	call	CLS
	
	; 测试栈
	; push	0xffff

	; 显示finding kernel
	push	STR0_LEN
	push	STR0
	call	SHOW_STR
	add	sp, 4

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

.loop_1:
	push	ax
	call	FIND_LOADER_FILE
	cmp	ax, 0
	pop	ax
	jne	LOAD_LOADER
	add	ax, FILE_ITEM_LEN
	loop	.loop_1
	
	; 显示not found kernel
	push	STR2_LEN
	push	STR2
	call	SHOW_STR
	add	sp, 4
	
	; 未找到kernel	
	jmp	$

LOAD_LOADER:
	; 显示found kernel
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
	sub		edi, 2
	loop	save_info
	
	mov	bp, sp

	; 加载FAT表
	mov	ax, 1
	mov	bx, LOAD_FAT_ADR_OFF
	mov	cl, FAT_TABLE_CNT
	mov	dl, DRV_NUM
	call	LOAD_SECTION
	
	mov	ax, KERNEL_LOAD_ADDR_BASE
	mov	es, ax
	mov	ax, word [bp]
	mov	word [load_addr], KERNEL_LOAD_ADDR_OFF
loop_load:
	;更换es
	push	ax
	mov	ax, KERNEL_LOAD_ADDR_BASE
	mov	es, ax
	pop	ax
	
	; 保存ax
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
	; 更换es
	mov	ax, LOAD_ADR_BASE
	mov	es, ax
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
	
	
	; 显示jump to kernel
	; push	STR4_LEN
	; push	STR4
	; call	SHOW_STR
	; 同时去掉之前保存内核的文件信息
	add	sp, 6

	; 关闭软驱马达
	call KillMotor

		
	; 调用BIOS获取内存大小
	mov	ax, LOADER_LOAD_ADR_BASE
	mov	es, ax
	mov	di, MemoryInfoCache
	xor	ebx, ebx
.loop_2:
	mov	eax, 0E820h

	mov	ecx, ARDSSIZE

	mov	edx, 0534d4150h
	
	int	15h

	jc	FAIL
	inc	word [ARDS_CNT]
	cmp	ebx, 0
	je	SUCCESS
	add	di, 20
	jmp	.loop_2

; 获取ARDS失败
FAIL:
	; 显示获取内存信息失败
	push	STR5_LEN
	push	STR5
	call	SHOW_STR
	add	sp, 4

	jmp	$
SUCCESS:
	; 显示内存信息
	push	STR6_LEN
	push	STR6
	call	SHOW_STR
	add	sp, 4
	
	
	push	word [ARDS_CNT]
	push	MemoryInfoCache
	call	SHOW_MEM_INFO
	add	sp, 4

	;
	; 分页
	;
	push	es
	push	si
	push	di

	; 统计内存大小
	mov	cx, [ARDS_CNT]
	mov	si, MemoryInfoCache
	add	si, ARDS_LOW_LEN_OFF
.loop_mem_size:
	mov	eax, dword [ds:si]
	add	dword [MEM_SIZE], eax
	add	si, ARDSSIZE
	loop	.loop_mem_size
	
	; 写页目录
	mov	eax, dword [MEM_SIZE]
	add	eax, PAGE_SIZE * PG_ITEM_NUM - 1
	shr	eax, 22
	;add	ax, PAGE_SIZE - 1
	;mov	dx, [MEM_SIZE + 2]
	;mov	bx, PAGE_SIZE
	;div	bx
	;push	ax
	mov	cx, ax
	mov	ax, PDE_ADDR_BASE
	mov	es, ax
	mov	di, PDE_ADDR_OFF
	mov	eax, PTE_ADDR_PHY | PG_P | PG_RW
	cld
.loop_store_pde:
	stosd
	add	eax, PAGE_SIZE
	loop	.loop_store_pde

	; 写页表
	; pop	cx
	mov	eax, dword [MEM_SIZE]
	add	eax, PAGE_SIZE - 1
	shr	eax, 12
	mov	cx, ax
	mov	ax, PTE_ADDR_BASE
	mov	es, ax
	mov	di, PTE_ADDR_OFF
	mov	eax, 0 | PG_P | PG_RW

.loop_store_one_pte:
	stosd
	add	eax, PAGE_SIZE
	loop	.loop_store_one_pte

	mov	eax, PDE_ADDR_PHY
	or	eax, PG_PWT_WRITE_THROUGH
	mov	cr3, eax

	pop	di
	pop	si
	pop	ax
	mov	es, ax
	

	; 进入保护模式
	
	; 关中断
	cli	
	; 打开A20地址线
	in	al, 92h
	or	al, 0x02
	out	92h, al

	; 加载GDT
	mov	eax, cs
	shl	eax, 4
	add	eax, Descriptor_GDT
	mov	dword [GDP_PTR+2], eax
	lgdt	[GDP_PTR]

	; 进入保护模式并启用分页
	mov	eax, cr0
	or	eax, 0x80008001
	mov	cr0, eax

	; 切换到分段
	jmp	dword Selector_CODE:START_32
	
	; 控制权交给kernel
	; jmp	KERNEL_LOAD_ADDR_BASE:KERNEL_LOAD_ADDR_OFF

load_addr: dw	KERNEL_LOAD_ADDR_OFF


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
	mov	si, KERNEL_NAME_STR
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

KERNEL_NAME_STR:
	db	KERNEL_NAME
	

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
	
STR0	db	"finding kernel"
STR0_LEN	equ	$ - STR0

STR2	db	"not found kernel"
STR2_LEN	equ	$ - STR2

STR3	db	"found kernel"
STR3_LEN	equ	$ - STR3

STR4	db	"jump to kernel"
STR4_LEN	equ	$ - STR4

STR5	db	"not found memory info"
STR5_LEN	equ	$ - STR5

STR6	db	"ARDS:"
STR6_LEN	equ	$ - STR6

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

; 关闭软驱马达

KillMotor:
	push	ax
	push	dx
	mov	dx, 03f2h
	mov	al, 0
	out	dx, al
	pop	dx
	pop	ax
	ret

; 显示ADRS获取信息
; void	SHOW_MEN_INFO(void* pards, int cnt) pards表示指向ards缓冲区的地址 cnt表示ards的数量
SHOW_MEM_INFO:
	; 保存相关寄存器
	push	bp
	mov	bp, sp
	push	ax
	push	bx
	push	cx
	push	gs
	push	di
	push	si

	; 设置显示位置
	mov	ax, GM_BASE
	mov	gs, ax	
	
	mov	cx, [bp+6]
	mov	ax, [bp+4]
	mov	si, ax

.loop_show_ards:
	push	cx
	mov	cx, ARDSSIZE / 4

	; 重新计算行开始位置
	mov	ax, [CUR_LINE]
	mov	bx, CHARS_PER_LINE * PER_CHAR_LEN
	mul	bl
	mov	di, ax
	add	di, GM_OFF
	inc	word [CUR_LINE]
	
.loop_show_32bits_num:
	push	word [ds:(si+2)]
	push	word [ds:si]
	add	si, 4
	call	SHOW_32BITS_HEX
	add	sp, 4
	add	edi, PER_CHAR_LEN
	loop	.loop_show_32bits_num
	
	pop	cx
	loop .loop_show_ards

	; 恢复相关寄存器
	pop	si
	pop	di
	pop	ax
	mov	gs, ax
	pop	cx
	pop	bx
	pop	ax
	pop	bp

	ret

; void	SHOW_32BITS_HEX(int l16, int h16)   l16表示低16位数字， h16表示高16位数字 [gs:di]指向写入的开始位置
SHOW_32BITS_HEX:
	push	bp
	mov	bp, sp
	push	ax
	push	bx
	push	cx
	push	si

	; 设置字体颜色
	mov	ah, CHAR_COLOR
	; 显示0x
	mov	al, [NUM_CHAR]
	mov	word [gs:di], ax
	add	di, PER_CHAR_LEN
	mov	al, [NUM_CHAR+16]
	mov	word [gs:di], ax
	add	di, PER_CHAR_LEN
	
	; 显示
	mov	ebx, dword [bp+4]
	mov	cx,8
.loop_show_32bits:
	rol	ebx, 4
	mov	ax, bx
	and	ax, 0xf
	mov	si, NUM_CHAR
	add	si, ax
	mov	al, byte [si]
	mov	ah, CHAR_COLOR
	mov	word [gs:di], ax
	add	di, PER_CHAR_LEN
	
	loop	.loop_show_32bits
	
	pop	si
	pop	cx
	pop	bx
	pop	ax
	pop	bp
	ret
	
	
NUM_CHAR:	db	"0123456789abcdefx"

[SECTION .s32]
ALIGN	32
[BITS 32]
; 调试位置 0xc560
START_32:
	mov	ax, Selector_DATA
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	ax, Selector_VIDEO
	mov	gs, ax
	mov	eax, TOP_OF_STACK
	mov	esp, eax

	; 读取内核文件，并根据ELF信息加载代码
	; 首先找到program header的位置
	mov	edi, KERNEL_LOAD_ADDR_PHY
	cmp	dword [edi], ELF_IDENT_4BYTES
	jz	.is_elf
	
	push	STR0_32_LEN
	push	STR0_32
	call	SHOW_STR_32
	add	esp, 8
	
	jmp	$
	
.is_elf:
	mov	edi, KERNEL_LOAD_ADDR_PHY
	mov	eax, dword [edi + ELF_HEADER_PHOFF_OFF]
	xor	ecx, ecx
	mov	cx, word [edi + ELF_HEADER_PHNUM_OFF]
	xor	edx, edx
	mov	dx, word [edi + ELF_HEADER_PHENTSIZE_OFF]
	push	dword [edi + ELF_HEADER_ENTRY_OFF]
	add	edi, eax
.loop_reaf_progam_header:
	push	dword [edi + ELF_PROGRAM_HEADER_FILESZ_OFF]
	mov	eax, dword [edi + ELF_PROGRAM_HEADER_OFFSET_OFF]
	add	eax, KERNEL_LOAD_ADDR_PHY
	push	eax
	mov		eax, dword [edi + ELF_PROGRAM_HEADER_VADDR_OFF]
	add		eax, K_P_BASE_ADDR
	sub		eax, K_V_BASE_ADDR
	push	eax
	call	memcpy
	add		esp, 12
	add		edi, edx
	loop	.loop_reaf_progam_header

	
	push	STR1_32_LEN
	push	STR1_32
	call	SHOW_STR_32
	add		esp, 8

	pop		eax
	add		eax, K_P_BASE_ADDR
	sub		eax, K_V_BASE_ADDR

	jmp	eax

; 保护模式下的字符串显示,并换行
; SHOW_STR_32(char* p_str, int len)
SHOW_STR_32:
	push	ebp
	mov	ebp, esp
	push	eax
	push	ebx
	push	ecx
	push	esi
	push	edi
	; 获得字符串地址
	mov	esi, [ebp+8]
	; 获得字符个数
	mov	ecx, [ebp+12]
	; 设置显存地址
	mov	al, CHARS_PER_LINE * PER_CHAR_LEN
	mov	bx, [CUR_LINE]
	inc	word [CUR_LINE]
	mul	bl
	movzx	edi, ax
	; 设置字体颜色
	mov	ah, CHAR_COLOR
	
.loop_show_str_32:
	mov	al, [ds:esi]
	inc	esi
	mov	[gs:edi], ax
	add	edi, PER_CHAR_LEN
	loop	.loop_show_str_32

	pop	edi
	pop	esi
	pop	ecx
	pop	ebx
	pop	eax
	pop	ebp
	ret

STR0_32:	db	"QXOSKRL.BIN is not an elf file"
STR0_32_LEN	equ $-STR0_32

STR1_32:	db	"jump to QXOSKRL"
STR1_32_LEN	equ $-STR1_32

;将指定位置的内存复制到指定位置
;void	memcpy(void* p_vaddr, void* s_addr, int	nfilesz) p_vaddr: 目标位置；f_addr: 源位置； nfilesz： 长度
memcpy:
	push	ebp
	mov	ebp, esp
	push	eax
	push	ecx
	push	edi
	push	esi
	
	; 目的地址
	mov	edi, [ebp+8]
	; 源地址
	mov	esi, [ebp+12]
	; 长度
	mov	ecx, [ebp+16]
	cmp	ecx, 0
	jz	.mem_cpy_ret
.loop_mem_cpy:
	mov	al, [esi]
	mov	[edi], al
	inc	edi
	inc	esi
	loop	.loop_mem_cpy
	
.mem_cpy_ret:
	
	pop	esi
	pop	edi
	pop	ecx
	pop	eax
	pop	ebp
	ret
