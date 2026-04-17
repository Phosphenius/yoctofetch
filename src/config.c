/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct config {
	int show_os;
	int show_host;
	int show_kernel;
	int show_uptime;
	int show_shell;
	int show_desktop;
	int show_terminal;
	int show_memory;
	int show_swap;
};

#ifndef NO_CONFIG_FILE

struct config config_from_buffer(char *buffer, int buffer_length)
{
	struct keyval keyvals[10] = {NULL};
	int found[10] = {0};

	find_keyvals_in_buffer(keyvals, 10, buffer, buffer_length);

	return (struct config){
	    .show_os = string_equals(
		keyval_get_or(keyvals, 10, STR("show_os"), STR("1"), found),
		STR("1")),
	    .show_host = string_equals(
		keyval_get_or(keyvals, 10, STR("show_host"), STR("1"), found),
		STR("1")),
	    .show_kernel = string_equals(
		keyval_get_or(keyvals, 10, STR("show_kernel"), STR("1"), found),
		STR("1")),
	    .show_uptime = string_equals(
		keyval_get_or(keyvals, 10, STR("show_uptime"), STR("1"), found),
		STR("1")),
	    .show_shell = string_equals(
		keyval_get_or(keyvals, 10, STR("show_shell"), STR("1"), found),
		STR("1")),
	    .show_desktop = string_equals(
		keyval_get_or(
		    keyvals, 10, STR("show_desktop"), STR("1"), found),
		STR("1")),
	    .show_terminal = string_equals(
		keyval_get_or(
		    keyvals, 10, STR("show_terminal"), STR("1"), found),
		STR("1")),
	    .show_memory = string_equals(
		keyval_get_or(keyvals, 10, STR("show_memory"), STR("1"), found),
		STR("1")),
	    .show_swap = string_equals(
		keyval_get_or(keyvals, 10, STR("show_swap"), STR("1"), found),
		STR("1"))};
}

struct config config_from_file(struct string user)
{
	char buf[1 << 8];

	struct fd_result fd_res = open("/etc/yoctofetch.conf", O_RDONLY);

	if (!fd_res.ok) {
		goto alternative_file;
	}

	struct io_result read_res = read(fd_res.fd, buf, 1 << 8);

	if (!read_res.ok) {
		goto error;
	}

	return config_from_buffer(buf, read_res.n_bytes);

alternative_file:

	char user_config_path[255] = "/home/";
	int offset = 6;

	for (int i = 0; i < user.length && i < 255; ++i) {
		user_config_path[offset++] = user.data[i];
	}

	char remaining_path[] = "/.config/yoctofetch/yoctofetch.conf";
	int remaining_path_len = sizeof remaining_path;

	int available = 255 - (offset + 1);

	if (remaining_path_len > available) {
		goto error;
	}

	memcpy(&user_config_path[offset], remaining_path, remaining_path_len);

	fd_res = open(user_config_path, O_RDONLY);

	if (!fd_res.ok) {
		goto error;
	}

	read_res = read(fd_res.fd, buf, 1 << 8);

	if (!read_res.ok) {
		goto error;
	}

	return config_from_buffer(buf, read_res.n_bytes);
error:
	return (struct config){1, 1, 1, 1, 1, 1, 1, 1, 1};
}

#endif
