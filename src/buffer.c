/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

/* Search multi-line buffer for a key and return its value (key="val"). */
struct string
    find_in_buffer(struct string string, char *buffer, int buffer_length)
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
