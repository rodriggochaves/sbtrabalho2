global _start

section .data
a dd 6
b dd 8

section .text
_start:
  mov eax, [a]
  add eax, [b]

  ; exit
  mov eax, 1
  mov ebx, 0
  int 80h
