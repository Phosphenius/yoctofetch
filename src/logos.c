/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define GUIX_LOGO_WIDTH 41
#define GUIX_LOGO_HEIGHT 9

char guix_logo_data[][GUIX_LOGO_WIDTH + 1] = {
    "\0\033[1;39m..                             `.",
    "\n\033[1;39m`--..```..`           `..```..--`",
    "\n\033[1;39m  .-:///-:::.       `-:::///:-.  ",
    "\n\033[1;39m     ````.:::`     `:::.````     ",
    "\n\033[1;39m          -//:`    -::-          ",
    "\n\033[1;39m           ://:   -::-           ",
    "\n\033[1;39m           `///- .:::`           ",
    "\n\033[1;39m            -+++-:::.            ",
    "\n\033[1;39m             :+/:::-             ",
    "\n\033[1;39m             `-....`             "};

#define ARCH_LOGO_WIDTH 48
#define ARCH_LOGO_HEIGHT 18

char arch_logo_data[][ARCH_LOGO_WIDTH + 1] = {
    "\0\033[1;39m                  -`                 ",
    "\n\033[1;39m                 .o+`                ",
    "\n\033[1;39m                `ooo/                ",
    "\n\033[1;39m               `+oooo:               ",
    "\n\033[1;39m              `+oooooo:              ",
    "\n\033[1;39m              -+oooooo+:             ",
    "\n\033[1;39m            `/:-:++oooo+:            ",
    "\n\033[1;39m           `/++++/+++++++:           ",
    "\n\033[1;39m          `/++++++++++++++:          ",
    "\n\033[1;39m         `/+++ooooooooooooo/`        ",
    "\n\033[1;39m        ./ooosssso++osssssso+`       ",
    "\n\033[1;39m       .oossssso-````/ossssss+`      ",
    "\n\033[1;39m      -osssssso.      :ssssssso.     ",
    "\n\033[1;39m     :osssssss/        osssso+++.    ",
    "\n\033[1;39m    /ossssssss/        +ssssooo/-    ",
    "\n\033[1;39m  `/ossssso+/:-        -:/+osssso+-  ",
    "\n\033[1;39m `+sso+:-`                 `.-/+oso: ",
    "\n\033[1;39m`++:.                           `-/+/",
    "\n\033[1;39m.`                                 `/"};

enum logo {
	LOGO_NONE,
	LOGO_GUIX,
	LOGO_ARCH,
};
