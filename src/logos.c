/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define GUIX_LOGO_WIDTH 40
#define GUIX_LOGO_HEIGHT 9

char guix_logo_data[][GUIX_LOGO_WIDTH + 1] = {
    "\033[1;39m..                             `.",
    "\033[1;39m`--..```..`           `..```..--`",
    "\033[1;39m  .-:///-:::.       `-:::///:-.  ",
    "\033[1;39m     ````.:::`     `:::.````     ",
    "\033[1;39m          -//:`    -::-          ",
    "\033[1;39m           ://:   -::-           ",
    "\033[1;39m           `///- .:::`           ",
    "\033[1;39m            -+++-:::.            ",
    "\033[1;39m             :+/:::-             ",
    "\033[1;39m             `-....`             "};

#define ARCH_LOGO_WIDTH 40
#define ARCH_LOGO_HEIGHT 18

char arch_logo_data[][ARCH_LOGO_WIDTH + 1] = {
    "                  -`                 ",
    "                 .o+`                ",
    "                `ooo/                ",
    "               `+oooo:               ",
    "              `+oooooo:              ",
    "              -+oooooo+:             ",
    "            `/:-:++oooo+:            ",
    "           `/++++/+++++++:           ",
    "          `/++++++++++++++:          ",
    "         `/+++ooooooooooooo/`        ",
    "        ./ooosssso++osssssso+`       ",
    "       .oossssso-````/ossssss+`      ",
    "      -osssssso.      :ssssssso.     ",
    "     :osssssss/        osssso+++.    ",
    "    /ossssssss/        +ssssooo/-    ",
    "  `/ossssso+/:-        -:/+osssso+-  ",
    " `+sso+:-`                 `.-/+oso: ",
    "`++:.                           `-/+/",
    ".`                                 `/"};

enum logo {
	LOGO_NONE,
	LOGO_GUIX,
	LOGO_ARCH,
};
