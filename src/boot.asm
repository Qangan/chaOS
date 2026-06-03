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

mov eax, TSS
mov [tssseg + 2], ax
shr eax, 16
mov [tssseg + 4], al
mov [tssseg + 7], ah
mov ax, 0x28
ltr ax

[EXTERN kernel_entry]
call kernel_entry

[BITS 32]
[GLOBAL loop]
loop:
    jmp loop

[GLOBAL outb]
outb:
    mov dx, [esp+4] 
    mov al, [esp+8] 
    out dx, al
    ret

[GLOBAL inb]
inb:
    mov dx, [esp+4]
    in al, dx
    ret

[GLOBAL cli]
cli:
    cli
    ret

[GLOBAL sti]
sti:
    sti
    ret

[GLOBAL get_eflags]
get_eflags:
    pushfd
    pop eax
    ret

[GLOBAL restore_ctx]
restore_ctx:
    mov esp, [esp + 4]
    popa
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 8
    iret

[GLOBAL lidt]
lidt:
    mov eax, [esp + 4]
    lidt [eax]
    ret

[GLOBAL get_esp]
get_esp:
    mov eax, esp
    ret

[GLOBAL syscallprint]
syscallprint:
    mov eax, [esp + 4]
    int 0x30
    ret

[EXTERN universal_handler]
[GLOBAL collect_context]
collect_context:
    push ds
    push es
    push fs
    push gs
    pusha
    cld
    mov eax, DATA
    mov ds, eax
    mov es, eax
    mov fs, eax
    mov gs, eax
    mov ebx, esp
    and esp, -16
    sub esp, 12 
    push ebx
    call universal_handler
    mov esp, ebx
    popa
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 8 
    iretd


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
    dw 0x2f
    dd gdt

global kcodeseg
align 8
gdt:
    .null:  dq 0
    kcodeseg:    dd 0x0000FFFF, 0b0000_0000_1100_1111_1001_1010_0000_0000
    kdataseg:    dd 0x0000FFFF, 0b0000_0000_1100_1111_1001_0010_0000_0000
    ucodeseg:    dd 0x0000FFFF, 0b0000_0000_1100_1111_1111_1010_0000_0000
    udataseg:    dd 0x0000FFFF, 0b0000_0000_1100_1111_1111_0010_0000_0000

    tssseg:      dd 0x0000006B, 0b0000_0000_0000_0000_1000_1001_0000_0000

TSS:
    prevTSS: dd 0
    esp0: dd 0x7C00
    ss0: db DATA
    times 12 dq 0

CODE equ 0x8
DATA equ 0x10

msg: db 'git gud lol', 0 

times 510-($-$$) db 0
dw 0xAA55
