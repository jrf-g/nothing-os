global superkillasm

section .text
superkillasm:
  mov eax, 0xFFFFFFFF
  mov cr3, eax
  mov ax, 0
  mov ds, ax
  mov eax, [0]
