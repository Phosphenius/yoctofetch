/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

enum {
	STDIN_FILENO = 0,
	STDOUT_FILENO = 1,
	STDERR_FILENO = 2,
};

#define AT_FDCWD -100

#if defined(__aarch64__)

enum {
	__NR_openat = 56,
	__NR_close = 57,
	__NR_writev = 66,
	__NR_read = 63,
	__NR_write = 64,
	__NR_uname = 160,
	__NR_sysinfo = 179,
};

#elif defined(__x86_64__)

enum {
	__NR_read = 0,
	__NR_write = 1,
	__NR_close = 3,
	__NR_writev = 20,
	__NR_uname = 63,
	__NR_sysinfo = 99,
	__NR_openat = 257,
};

#else
	#error "Architecture not supported"
#endif

struct fd_result open(const char *path, int64_t flags)
{
	int64_t result = (int64_t)syscall4(
	    __NR_openat,
	    (void *)AT_FDCWD,
	    (void *)path,
	    (void *)flags,
	    0);

	if (result >= 0) {
		return (struct fd_result){.ok = 1, .fd = result};
	}

	return (struct fd_result){.ok = 0, .errno = -result};
}

struct io_result read(int64_t fd, void *buf, size_t count)
{
	int64_t result = (int64_t)syscall3(
	    __NR_read, (void *)fd, buf, (void *)count);

	if (result >= 0) {
		return (struct io_result){.ok = 1, .n_bytes = result};
	}

	return (struct io_result){.ok = 0, .errno = -result};
}

struct io_result write(int64_t fd, void *buf, size_t count)
{
	int64_t result = (int64_t)syscall3(
	    __NR_write, (void *)fd, buf, (void *)count);

	if (result >= 0) {
		return (struct io_result){.ok = 1, .n_bytes = result};
	}

	return (struct io_result){.ok = 0, .errno = -result};
}
