/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

static const char *guix_logo_data[] = {
    "..                             `.",
    "`--..```..`           `..```..--`",
    "  .-:///-:::.       `-:::///:-.  ",
    "     ````.:::`     `:::.````     ",
    "          -//:`    -::-          ",
    "           ://:   -::-           ",
    "           `///- .:::`           ",
    "            -+++-:::.            ",
    "             :+/:::-             ",
    "             `-....`             "};

static const char *arch_logo_data[] = {
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

struct logo {
	const char **data;
	int width;
	int height;
};

static const struct logo guix_logo = {
    .data = guix_logo_data, .width = 33, .height = 9};

static const struct logo arch_logo = {
    .data = arch_logo_data, .width = 40, .height = 18};
