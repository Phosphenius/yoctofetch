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

-include config.mk

BINNAME = yoctofetch

CFLAGS_EXTRA = -Wall -Werror -Wextra -Wpedantic -pedantic-errors -g3 \
				 -fno-unwind-tables \
				 -fno-asynchronous-unwind-tables
.PHONY: all
all: $(BINNAME)

.PHONY: clang-format
clang-format:
	clang-format -i src/*.c src/*.h

.PHONY: clean
clean:
	-rm src/*.o src/*.d
	-rm arch/$(ARCH)/*.o
	-rm $(BINNAME)

.PHONY: distclean
distclean: clean
	-rm config.mk

-include src/*.d

