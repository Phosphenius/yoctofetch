/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct config {
	int64_t show_os;
	int64_t show_host;
	int64_t show_kernel;
	int64_t show_uptime;
	int64_t show_shell;
	int64_t show_desktop;
	int64_t show_terminal;
	int64_t show_memory;
	int64_t show_swap;
	int64_t show_color;
};

#ifndef NO_CONFIG_FILE

enum {
	CONFIG_SHOW_OS = 0,
	CONFIG_SHOW_HOST,
	CONFIG_SHOW_KERNEL,
	CONFIG_SHOW_UPTIME,
	CONFIG_SHOW_SHELL,
	CONFIG_SHOW_DESKTOP,
	CONFIG_SHOW_TERMINAL,
	CONFIG_SHOW_MEMORY,
	CONFIG_SHOW_SWAP,
	CONFIG_SHOW_COLOR,
};

struct config config_from_buffer(char *buffer, int64_t buffer_length)
{
	struct keyval keyvals[] = {
	    [CONFIG_SHOW_OS] =
		{.key = STR_INIT("show_os"),       .val = STR_INIT("1")},
	    [CONFIG_SHOW_HOST] =
		{.key = STR_INIT("show_host"),     .val = STR_INIT("1")},
	    [CONFIG_SHOW_KERNEL] =
		{.key = STR_INIT("show_kernel"),   .val = STR_INIT("1")},
	    [CONFIG_SHOW_UPTIME] =
		{.key = STR_INIT("show_uptime"),   .val = STR_INIT("1")},
	    [CONFIG_SHOW_SHELL] =
		{.key = STR_INIT("show_shell"),    .val = STR_INIT("1")},
	    [CONFIG_SHOW_DESKTOP] =
		{.key = STR_INIT("show_desktop"),  .val = STR_INIT("1")},
	    [CONFIG_SHOW_TERMINAL] =
		{.key = STR_INIT("show_terminal"), .val = STR_INIT("1")},
	    [CONFIG_SHOW_MEMORY] =
		{.key = STR_INIT("show_memory"),   .val = STR_INIT("1")},
	    [CONFIG_SHOW_SWAP] =
		{.key = STR_INIT("show_swap"),     .val = STR_INIT("1")},
	    [CONFIG_SHOW_COLOR] = {
				  .key = STR_INIT("show_color"),    .val = STR_INIT("1")}
        };

	const int64_t keyvals_len = sizeof keyvals / sizeof keyvals[0];

	find_keyvals_in_buffer(keyvals, keyvals_len, buffer, buffer_length);

	return (struct config){
	    .show_os = string_equals(keyvals[CONFIG_SHOW_OS].val, STR("1")),
	    .show_host = string_equals(keyvals[CONFIG_SHOW_HOST].val, STR("1")),
	    .show_kernel =
		string_equals(keyvals[CONFIG_SHOW_KERNEL].val, STR("1")),
	    .show_uptime =
		string_equals(keyvals[CONFIG_SHOW_UPTIME].val, STR("1")),
	    .show_shell =
		string_equals(keyvals[CONFIG_SHOW_SHELL].val, STR("1")),
	    .show_desktop =
		string_equals(keyvals[CONFIG_SHOW_DESKTOP].val, STR("1")),
	    .show_terminal =
		string_equals(keyvals[CONFIG_SHOW_TERMINAL].val, STR("1")),
	    .show_memory =
		string_equals(keyvals[CONFIG_SHOW_MEMORY].val, STR("1")),
	    .show_swap = string_equals(keyvals[CONFIG_SHOW_SWAP].val, STR("1")),
	    .show_color =
		string_equals(keyvals[CONFIG_SHOW_COLOR].val, STR("1"))};
}

struct config config_from_file(struct string user)
{
	char buf[1 << 8];

	char user_config_path[255] = "/home/";
	int64_t offset = 6;

	for (int64_t i = 0; i < user.length && i < 255; ++i) {
		user_config_path[offset++] = user.data[i];
	}

	char remaining_path[] = "/.config/yoctofetch/yoctofetch.conf";
	int64_t remaining_path_len = sizeof remaining_path;

	int64_t available = 255 - (offset + 1);

	if (remaining_path_len > available) {
		goto error;
	}

	memcpy(&user_config_path[offset], remaining_path, remaining_path_len);

	struct fd_result fd_res = open(user_config_path, O_RDONLY);

	if (!fd_res.ok) {
		goto alternative_file;
	}

	struct io_result read_res = read(fd_res.fd, buf, 1 << 8);

	if (!read_res.ok) {
		goto error;
	}

	return config_from_buffer(buf, read_res.n_bytes);

alternative_file:

	fd_res = open("/etc/yoctofetch.conf", O_RDONLY);

	if (!fd_res.ok) {
		goto error;
	}

	read_res = read(fd_res.fd, buf, 1 << 8);

	if (!read_res.ok) {
		goto error;
	}

	return config_from_buffer(buf, read_res.n_bytes);
error:
	return (struct config){1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
}

#endif
