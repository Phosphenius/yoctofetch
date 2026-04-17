/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

typedef int pid_t;

struct fd_result {
	int ok;

	union {
		int fd;
		int errno;
	};
};

struct io_result {
	int64_t ok;

	union {
		int64_t n_bytes;
		int64_t errno;
	};
};

struct string {
	const char *data;
	int64_t length;
};

#define STR(cstring) \
	(struct string){.data = cstring, .length = sizeof(cstring) - 1}

#define STR_INIT(cstring) {.data = cstring, .length = sizeof(cstring) - 1}
