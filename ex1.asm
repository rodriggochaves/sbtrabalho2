section .text
global _start
_start:
push b
call LerInt
push h
call LerInt
mov eax,[b]
mul dword [h]
div dword [dois]
mov [r],eax
push dword [r]
call EscreverInt
mov eax,1
mov ebx,0
int 0x80
section .data
buf times 11 db 0
b dd 0
h dd 0
r dd 0
dois dd 2

LerChar:
    enter 0,0 ; stack frame
    pusha
    mov eax,3 ; chamada de leitura
    mov ebx,0 ; stdin
    mov ecx,[ebp+8] ; variavel de escrita
    mov edx,1 ; 1 byte
    int 0x80 ; system call
    popa
    leave ; limpa stack frame
    ret 4
  
EscreverChar:
    enter 0,0
    pusha
    mov eax,4
    mov ebx,1
    mov ecx,[ebp+8]
    mov edx,1
    int 0x80
    popa
    leave
    ret 4
  
LerString:
    enter 0,0
    push ebx
    push ecx
    push edx
    mov eax,3
    mov ebx,0
    mov ecx,[ebp+8]
    mov edx,30
    int 0x80
    pop edx
    pop ecx
    pop ebx
    leave
    ret 4
  
EscreverString:
    enter 0,0
    pusha
    mov eax,0
    mov ebx,[ebp+8]
    mov ecx,0
es1:  mov byte al,[ebx]
    cmp al,0
    jz es2
    push ebx
    call EscreverChar
    inc ebx
    jmp es1
es2:  popa
    leave
    ret 4
  
LerInt:
    enter 0,0
    pusha
    mov eax,3
    mov ebx,0
    mov ecx,buf
    mov edx,11
    int 0x80
    sub eax,1
    push eax      ;salva num lido
    mov ecx,0     ;ecx = contador
    mov eax,0     ;resultado
    mov ebx,0     ;aux
li3:  mov bl,[buf+ecx]
    sub ebx,30h     ; ascii para int
    add eax,ebx     ; soma no total
    inc ecx       ; incrementa contador
    cmp ecx,[esp]   ; se contador>=no de chars lidos
    jnb li2       ; sai
    mov edx,10
    mul edx       ; se nao, mul por 10
    jmp li3       ; lasso
li2:  mov edx,[ebp+8]   ; resultado
    mov [edx],eax
    pop eax
    popa
    leave
    ret 4
  
EscreverInt:
    enter 0,0
    pusha
    mov edx,0
    mov ecx,buf+10    ; ultimo char da string
    mov byte [ecx],0  ; terminador
    mov ebx,10
    mov eax,[ebp+8]   ; eax = numero
ei1:  sub ecx,1
    div ebx
    add edx,30h     ; resto em ascii
    mov [ecx],dl    ; str[i]=char
    mov edx,0
    cmp eax,0     ; se quociente = 0
    ja ei1
    push ecx
    call EscreverString
    popa
    leave
    ret 4

