[BITS 16]

%define SECTORS ((N + 511) / 512)

init_stack_and_pos:
    cli
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov sp, 0x7C00
    
prepare:
    mov ax, 0x7E0
    mov es, ax
    xor bx, bx
    xor ch, ch
    xor dh, dh
    mov cl, 2
    mov al, 1
    mov si, SECTORS

read_payload:
    mov ah, 0x2
    int 0x13
    jc err
    mov di, es
    add di, 0x20
    mov es, di
    inc cl
    cmp cl, 19 ; max sec
    jb cont
    mov cl, 1
    inc dh
    cmp dh, 2 ; max head
    jb cont
    xor dh, dh 
    inc ch
    cmp ch, 80 ; max cyl
    jae err


lgdt [gdt_descriptor]
cld

mov eax, cr0
or eax, 1
mov cr0, eax

jmp CODE:next

[BITS 32]
next:
    mov eax, DATA
    mov ds, eax
    mov ss, eax
    mov es, eax
    mov fs, eax
    mov gs, eax

[EXTERN kernel_entry]
call kernel_entry

[BITS 32]
[GLOBAL loop]
loop:
    jmp loop

[BITS 16]
cont:
    dec si
    jmp read_payload

err:
    mov ah, 0x0E
    mov bx, msg
    .ploop:
        mov al, [bx]
        test al, al 
        jz loop
        int 0x10
        inc bx
        jmp .ploop


gdt_descriptor:
    dw 0x17
    dd gdt

align 8
gdt:
    .null:  dq 0
    codeseg:    dd 0x0000FFFF, 0b0000_0000_1100_1111_1001_1010_0000_0000
    dataseg:    dd 0x0000FFFF, 0b0000_0000_1100_1111_1001_0010_0000_0000

CODE equ 0x8
DATA equ 0x10

msg: db 'git gud lol', 0 

times 510-($-$$) db 0
dw 0xAA55
