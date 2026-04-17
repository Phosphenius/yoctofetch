/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct os_release_result {
	struct string name;
	struct string id;
};

struct os_release_result parse_os_release(char *buf, int buf_len)
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

	struct keyval keyvals[10] = {NULL};
	int found[10] = {0};

	find_keyvals_in_buffer(keyvals, 10, buf, buf_len);

	struct string id =
	    keyval_get_or(keyvals, 10, STR("ID"), STR("Unknown"), found);
	struct string name =
	    keyval_get_or(keyvals, 10, STR("NAME"), STR("Unknown"), found);

	struct os_release_result result = {.name = name, .id = id};

	return result;

error:
	return (struct os_release_result){.name = STR("Unknown"),
	                                  .id = STR("unknown")};
}
