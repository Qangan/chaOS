[BITS 16]

init_stack:
    cli
    xor ax, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

read_payload:
    mov ds, ax
    mov es, ax
    mov ah, 0x2
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0 
    mov bx, 0x7E00
    int 13h

loop:
    jmp loop

times 510-($-$$) db 0 
dw 0xAA55

