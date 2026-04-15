# SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Makefile for GNU make—utilizes GNU coreutils.

include base.mk

override CFLAGS = $(CFLAGS_EXTRA) $(CFLAGS_CONFIG) -ffreestanding \
	-fno-stack-protector
override CPPFLAGS = $(CPPFLAGS_CONFIG) -MMD -MP
override LDFLAGS = $(LDFLAGS_CONFIG) -nostdlib

$(BINNAME): src/yoctofetch.o arch/$(ARCH)/start.o arch/$(ARCH)/syscall.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: install
install: $(BINNAME)
	install -d $(DESTDIR)$(prefix)/bin
	install -m 755 $< $(DESTDIR)$(prefix)/bin
	install -d $(DESTDIR)$(prefix)/share/man/man1/
	install -m 644 yoctofetch.1 $(DESTDIR)$(prefix)/share/man/man1/
	install -d $(DESTDIR)$(prefix)/etc
	install -m 777 etc/yoctofetch.conf $(DESTDIR)$(prefix)/etc
