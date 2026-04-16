# SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
#
# SPDX-License-Identifier: GPL-3.0-or-later

.POSIX:
.SUFFIXES:

# POSIX compatible makefile.

include base.mk

CFLAGS_ALL = $(CFLAGS_EXTRA) $(CFLAGS_CONFIG) -ffreestanding \
	-fno-stack-protector
CPPFLAGS_ALL = $(CPPFLAGS_CONFIG) -MMD -MP
LDFLAGS_ALL = $(LDFLAGS_CONFIG) -nostdlib

src/yoctofetch.o: src/yoctofetch.c
	$(CC) $(CFLAGS_ALL) $(CPPFLAGS_ALL)  -c -o $@ $?

arch/$(ARCH)/start.o: arch/$(ARCH)/start.S
	$(CC) $(CPPFLAGS_ALL) -c -o $@ $?

arch/$(ARCH)/syscall.o: arch/$(ARCH)/syscall.S
	$(CC) $(CPPFLAGS_ALL) -c -o $@ $?

$(BINNAME): src/yoctofetch.o arch/$(ARCH)/start.o arch/$(ARCH)/syscall.o
	$(CC) $(LDFLAGS_ALL) $? -o $@ $(LOADLIBES) $(LDLIBS)
