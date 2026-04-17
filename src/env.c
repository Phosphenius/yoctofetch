/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct string trim_shell(struct string shell_raw)
{
	for (int64_t i = 0; i < shell_raw.length - 5; ++i) {
		if (shell_raw.data[i] != '/') {
			continue;
		}

		if ((shell_raw.data[i + 1] == 'b' &&
		     shell_raw.data[i + 2] == 'i' &&
		     shell_raw.data[i + 3] == 'n') ||
		    (shell_raw.data[i + 1] == 'u' &&
		     shell_raw.data[i + 2] == 's' &&
		     shell_raw.data[i + 3] == 'r')) {
			return (struct string){.data = shell_raw.data + i + 5,
			                       .length =
			                           shell_raw.length - i - 5};
		}
	}

	return shell_raw;
}
