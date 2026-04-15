/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

extern void *syscall1(size_t number, void *arg1);
extern void *syscall3(size_t number, void *arg1, void *arg2, void *arg3);
extern void *
    syscall4(size_t number, void *arg1, void *arg2, void *arg3, void *arg4);
