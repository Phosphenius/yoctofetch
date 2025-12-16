# SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
#
# SPDX-License-Identifier: GPL-3.0-or-later

#/usr/bin/env bash
complete -W "-H --no-user-at-host -O --no-os -K --no-kernel -U --no-uptime -S \
	--no-shell -S --no-shell -D --no-desktop -T --no-terminal -M \
	--no-memory -P --no-swap --guix-logo --arch-logo -V --version -h \
	--help --bottom-padding" yoctofetch
