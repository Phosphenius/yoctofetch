/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct iovec {
	void *iov_base;
	size_t iov_len;
};

struct io_result writev(int64_t fd, const struct iovec *iov, int64_t iovcnt)
{
	int64_t result = (int64_t)syscall3(
	    __NR_writev,
	    (void *)(size_t)fd,
	    (struct iovec *)iov,
	    (void *)(size_t)iovcnt);

	if (result >= 0) {
		return (struct io_result){.ok = 1, .n_bytes = result};
	}

	return (struct io_result){.ok = 0, .errno = -result};
}
