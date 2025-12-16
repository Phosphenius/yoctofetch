# SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
#
# SPDX-License-Identifier: GPL-3.0-or-later

# Variables explicitly set to prevent override from env.
# All of these should be sourced from config.mk, which takes env vars into accout.
ARCH =
CFLAGS_CONFIG =
DESTDIR =
LDFLAGS_CONFIG =
prefix =
CC=
CPPFLAGS_CONFIG =
SRCDIR =

-include config.mk

BINNAME = yoctofetch

CFLAGS_EXTRA = -Wall -Werror -Wextra -Wpedantic -pedantic-errors -g3 \
				 -fno-unwind-tables \
				 -fno-asynchronous-unwind-tables -Qn

override CFLAGS = $(CFLAGS_EXTRA) $(CFLAGS_CONFIG) -ffreestanding \
	-fno-stack-protector
override CPPFLAGS = $(CPPFLAGS_CONFIG) -MMD -MP
override LDFLAGS = $(LDFLAGS_CONFIG) -nostdlib

$(BINNAME): $(SRCDIR)/$(BINNAME).o arch/$(ARCH)/start.o arch/$(ARCH)/syscall.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: install
install: $(BINNAME)
	install -d $(DESTDIR)$(prefix)/bin
	install -m 755 $< $(DESTDIR)$(prefix)/bin
	install -d $(DESTDIR)$(prefix)/share/man/man1/
	install -m 644 yoctofetch.1 $(DESTDIR)$(prefix)/share/man/man1/
	install -d $(DESTDIR)$(prefix)/share/bash-completion/completions
	install -m 655 completion/yoctofetch.bash \
		$(DESTDIR)$(prefix)/share/bash-completion/completions

.PHONY: clang-format
clang-format:
	clang-format -i $(SRCDIR)/*.c $(SRCDIR)/*.h

.PHONY: clean
clean:
	-rm $(SRCDIR)/*.o $(SRCDIR)/*.d
	-rm arch/$(ARCH)/*.o
	-rm $(BINNAME)

.PHONY: distclean
distclean: clean
	-rm config.mk

-include $(SRCDIR)/*.d
