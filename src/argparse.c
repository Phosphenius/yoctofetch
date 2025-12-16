/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

int test_arg(struct string arg, int argc, char *restrict argv[])
{
	for (int i = 1; i < argc; ++i) {
		for (int j = 0; j < arg.length && argv[i][j] != '\0'; ++j) {
			if (argv[i][j] != arg.data[j]) {
				break;
			}

			if (j + 1 == arg.length && argv[i][j + 1] == '\0') {
				return 1;
			}
		}
	}

	return 0;
}
