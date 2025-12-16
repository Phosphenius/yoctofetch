/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

.text
.global _start

_start:
    ldr     x0, [sp]
    add     x1, sp, #8
    add     x3, x0, #2
    lsl     x3, x3, #3
    add     x2, sp, x3
    bl      main
    mov     x8, #93
    svc     #0
