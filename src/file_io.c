/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct string read_file(char *file, char *buf, int64_t buf_len, struct string alt)
{
	struct fd_result fd_res = open(file, O_RDONLY);

	if (!fd_res.ok) {
		return alt;
	}

	struct io_result read_res = read(fd_res.fd, buf, buf_len);

	if (!read_res.ok) {
		return alt;
	}

	return (struct string){.data = buf, .length = read_res.n_bytes - 1};
}
