/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

.section .note.GNU-stack,"",@progbits

.text
.globl _start

_start:
xor %rbp, %rbp
popq %rdi
movq %rsp, %rsi
pushq %rdi
lea 8(%rsi, %rdi,8), %rdx
and $0xfffffffffffffff0, %rsp
call main
movq %rax, %rdi
movq $60, %rax
syscall
ret
