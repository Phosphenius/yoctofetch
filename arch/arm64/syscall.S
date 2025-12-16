/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

.text
.global syscall1, syscall3, syscall4

syscall1:
    mov     x8, x0
    mov     x0, x1
    svc     #0
    ret

syscall3:
    mov     x8, x0
    mov     x0, x1
    mov     x1, x2
    mov     x2, x3
    svc     #0
    ret

syscall4:
    mov     x8, x0
    mov     x0, x1
    mov     x1, x2
    mov     x2, x3
    mov     x3, x4
    svc     #0
    ret
