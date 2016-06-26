global _start

section .data
a dd 6
b dw 8
c db 4
section .text
  mov eax, [a]
  add eax, [b]
