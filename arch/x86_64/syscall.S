/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

.section .note.GNU-stack,"",@progbits

.text
.globl  syscall1,  syscall3, syscall4

syscall1:
movq %rdi, %rax
movq %rsi, %rdi
syscall
ret

syscall3:
movq %rdi, %rax
movq %rsi, %rdi
movq %rdx, %rsi
movq %rcx, %rdx
syscall
ret

syscall4:
movq %rdi, %rax
movq %rsi, %rdi
movq %rdx, %rsi
movq %rcx, %rdx
movq %r8, %r10
syscall
ret
