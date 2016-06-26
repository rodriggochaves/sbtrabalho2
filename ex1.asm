section .text
global _start
_start:
push b
call LerInt
section .data
buf times 11 db 0
b dd 0
h dd 0
r dd 0
dois dd 2
  
LerInt:
    enter 0,0
    pusha
    mov eax,3