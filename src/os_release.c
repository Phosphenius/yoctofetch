/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define O_RDONLY 0

struct os_release_result {
	struct string name;
	struct string id;
};

struct string
    find_os_release_value(struct string string, char *buffer, int buffer_length)
{
	int buffer_index = 0;
	int string_index = 0;
	char *result_string = NULL;
	int result_length = 0;
	char input = 0;

start:
	if (buffer_index >= buffer_length || string_index >= string.length) {
		goto error;
	}

	/* Setting the sixth bit to 1, thus ANDing with 32 (0x20) turns ASCII
	 * alphabetic chars to lower case. */
	input = buffer[buffer_index++] | 32;

	if (input == (string.data[string_index] | 32)) {
		string_index++;
		goto name_char;
	}

	goto start;

name_char:
	if (buffer_index >= buffer_length || string_index >= string.length) {
		goto error;
	}

	input = buffer[buffer_index++] | 32;

	if (input == (string.data[string_index] | 32)) {
		string_index += (string_index + 1 < string.length);
		goto name_char;
	} else if (input == '=') {
		goto equalsign;
	}

	string_index = 0;
	goto start;

equalsign:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	input = buffer[buffer_index++];

	if (input == '"') {
		goto equalsign;
	} else if (input >= ' ' && input <= '~') {
		result_string = buffer + buffer_index - 1;

		goto value_char;
	}

value_char:
	if (buffer_index >= buffer_length) {
		goto error;
	}

	input = buffer[buffer_index++];
	result_length++;

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
	return (struct string){.data = result_string, .length = result_length};
error:
	return STR("");
}

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

	struct string id = find_os_release_value(STR("id"), buf, buf_len);
	struct string name = find_os_release_value(STR("name"), buf, buf_len);
	struct os_release_result result = {.name = name, .id = id};

	return result;

error:
	return (struct os_release_result){.name = STR("Unknown"),
	                                  .id = STR("unknown")};
}
