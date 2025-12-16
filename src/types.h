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
	int ok;

	union {
		long int n_bytes;
		int errno;
	};
};

struct string {
	const char *data;
	long int length;
};

#define STR(cstring) \
	(struct string){.data = cstring, .length = sizeof(cstring) - 1}

#define STR_INIT(cstring) {.data = cstring, .length = sizeof(cstring) - 1}
