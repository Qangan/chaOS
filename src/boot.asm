[BITS 16]

%define SECTORS ((N + 512) / 512)

msg: db "git gud lol", 0

init_stack_and_pos:
    cli
    mov ax, 0x07C0
    mov ds, ax
    xor ax, ax
    mov ss, ax
    mov sp, 0x7C00

prepare:
    mov es, ax
    mov ah, 0x2
    xor ch, ch
    xor dh, dh
    mov cl, 2
    mov al, 1
    mov bx, 0x7E00
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
    mov dh, 0 
    inc ch
    cmp ch, 80 ; max cyl
    jae err

cont:
    dec si
    jnz read_payload

loop:
    jmp loop

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


times 510-($-$$) db 0 
dw 0xAA55

