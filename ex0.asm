section .text
global _start
_start:
  mov eax, [a]
  add eax, [b]
  mov eax, 1
  mov ebx, 0
  int 80h
section .data
  a dd 6
  b dw 8
  c db 4
