/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct iovec {
	void *iov_base;
	size_t iov_len;
};

struct io_result writev(int fd, const struct iovec *iov, int iovcnt)
{
	int result = (long int)syscall3(
	    __NR_writev,
	    (void *)(size_t)fd,
	    (struct iovec *)iov,
	    (void *)(size_t)iovcnt);

	if (result >= 0) {
		return (struct io_result){.ok = 1, .n_bytes = result};
	}

	return (struct io_result){.ok = 0, .errno = -result};
}
