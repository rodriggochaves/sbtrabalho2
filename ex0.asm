global _start
section .text
_start:
  mov eax, [a]
  add eax, [b]
  mov eax, 1
section .data
  a dd 6
  b dw 8
  c db 4
