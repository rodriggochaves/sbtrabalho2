section .data
a db 6
b db 8

section .text
  mov eax, [a]
  add eax, [b]