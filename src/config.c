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

struct config config_from_buffer(char *buffer, int buffer_length)
{
	struct string map_backend[113] = {NULL};
	struct hashmap map = {.capacity = 113, .data = map_backend};
	int buffer_index = 0;
	int key_len = 0;
	char *key_start = NULL;
	int val_len = 0;
	char *val_start = NULL;
	char input = 0;

start:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	key_start = &buffer[buffer_index];
	key_len = 0;
	input = buffer[buffer_index++];

	if (((input | 32) >= 'a' && (input | 32) <= 'z') || input == '_') {
		goto key_char;
	} else if (input == '#') {
		goto comment;
	}

	goto start;

comment:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	input = buffer[buffer_index++];

	if (input != '\n') {
		goto comment;
	}

	if (buffer_index + 1 >= buffer_length) {
		goto finish;
	} else {
		goto start;
	}

key_char:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	key_len++;
	input = buffer[buffer_index++];

	if (((input | 32) >= 'a' && (input | 32) <= 'z') || input == '_') {
		goto key_char;
	} else if (input == '=') {
		goto equalsign;
	}

	goto start;

equalsign:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	val_start = &buffer[buffer_index];
	input = buffer[buffer_index++];

	if (input == '"') {
		goto equalsign;
	} else if (input >= ' ' && input <= '~') {
		goto value_char;
	}

	goto error;

value_char:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	val_len++;
	input = buffer[buffer_index++];

	if (input == '"') {
		goto closing_double_quote;
	} else if (input == '\n') {
		goto success;
	} else if (input >= ' ' && input <= '~') {
		goto value_char;
	}

	goto error;

closing_double_quote:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	input = buffer[buffer_index++];

	if (input == '\n') {
		goto success;
	}

	goto error;

success:
	hashmap_add(
	    &map,
	    (struct string){.data = key_start, .length = key_len},
	    (struct string){.data = val_start, .length = val_len});

	if (buffer_index + 1 >= buffer_length) {
		goto finish;
	} else {
		val_len = 0;
		goto start;
	}

finish:
	struct string foo = hashmap_get_or(&map, STR("show_os"), STR(""));
	(void)foo;

	return (struct config){
	    .show_os = string_equals(
		hashmap_get_or(&map, STR("show_os"), STR("1")), STR("1")),
	    .show_host = string_equals(
		hashmap_get_or(&map, STR("show_host"), STR("1")), STR("1")),
	    .show_kernel = string_equals(
		hashmap_get_or(&map, STR("show_kernel"), STR("1")), STR("1")),
	    .show_uptime = string_equals(
		hashmap_get_or(&map, STR("show_uptime"), STR("1")), STR("1")),
	    .show_shell = string_equals(
		hashmap_get_or(&map, STR("show_shell"), STR("1")), STR("1")),
	    .show_desktop = string_equals(
		hashmap_get_or(&map, STR("show_desktop"), STR("1")), STR("1")),
	    .show_terminal = string_equals(
		hashmap_get_or(&map, STR("show_terminal"), STR("1")), STR("1")),
	    .show_memory = string_equals(
		hashmap_get_or(&map, STR("show_memory"), STR("1")), STR("1")),
	    .show_swap = string_equals(
		hashmap_get_or(&map, STR("show_swap"), STR("1")), STR("1"))};

error:
	return (struct config){1, 1, 1, 1, 1, 1, 1, 1, 1};
}

struct config config_from_file(const char *user)
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

	int user_str_len = strlen(user);

	char user_config_path[255] = "/home/";
	int offset = 6;

	for (int i = 0; i < user_str_len && i < 255; ++i) {
		user_config_path[offset++] = user[i];
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
