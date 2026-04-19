/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct os_release_result {
	struct string name;
	struct string id;
};

enum {
	OS_RELEAES_NAME = 0,
	OS_RELEAES_ID,
};

struct os_release_result parse_os_release(char *buf, int64_t buf_len)
{
	const char os_release_path[] = "/etc/os-release";

	struct fd_result fd_res = open(os_release_path, O_RDONLY);

	if (!fd_res.ok) {
		goto error;
	}

	struct io_result read_res = read(fd_res.fd, buf, buf_len);

	if (!read_res.ok) {
		goto error;
	}

	struct keyval keyvals[] = {
	    [OS_RELEAES_NAME] =
		{.key = STR_INIT("NAME"), .val = STR_INIT("Unknown")},
	    [OS_RELEAES_ID] = {
				   .key = STR_INIT("ID"),   .val = STR_INIT("Unknown")}
        };

	const int64_t keyvals_len = sizeof keyvals / sizeof keyvals[0];

	find_keyvals_in_buffer(keyvals, keyvals_len, buf, buf_len);

	struct os_release_result result = {
	    .name = keyvals[OS_RELEAES_NAME].val,
	    .id = keyvals[OS_RELEAES_ID].val};

	return result;

error:
	return (struct os_release_result){.name = STR("Unknown"),
	                                  .id = STR("unknown")};
}
