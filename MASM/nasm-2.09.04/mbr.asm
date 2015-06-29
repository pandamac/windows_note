;File: MBR.ASM
;Compile: nasm -f bin MBR.ASM -o MBR.COM
;////////////////////////////////////////////////////////////////////////////////

jmp start

data:
	db "xiangchangquan+2011122021",0xa,0xb
start:
	xor bx,bx
	mov ds,bx
	mov ax,[0x413]

	sub ax,2 ;分配2KB
	mov [0x413],ax
	shl ax,0x6

	mov es,ax
	mov si,0x7c00
	xor di,di
	mov cx,0x200
	rep movsb
	push es
	push Main
	retf
Main:
	call ShowMessage
	call GetInput
	call BootOriginOs ;启动原始系统
ShowMessage:
	mov bp,data
	mov cx,0x19
	mov dx,0x0505
	mov bx,0x100c
	mov ax,0x1301
	int 10h
	ret
GetInput:
	mov ah,0
	int 16h
	cmp al,0xd
	jnz GetInput
	ret
BootOriginOs:		;启动原始系统
	xor dx,dx
	mov es,dx
	mov ax,0x0201
	mov bx,0x7c00
	mov cx,0x0002
	mov dx,0x0080
	int 13h
	jmp 0x0:0x7c00

times 510-($-$$) db 0 ;$当前地址，$为起始地址
dw 0xAA55