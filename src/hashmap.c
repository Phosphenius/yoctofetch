/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct hashmap {
	int capacity;
	struct string *data;
};

void hashmap_add(struct hashmap *map, struct string key, struct string value)
{
	int index = string_hash(key) % map->capacity;

	/* We literally do not care about collisions. */
	map->data[index] = value;
}

struct string
    hashmap_get_or(struct hashmap *map, struct string key, struct string alt)
{
	int index = string_hash(key) % map->capacity;

	if (string_equals(map->data[index], STR(""))) {
		return alt;
	}

	return map->data[index];
}

void parse_buffer_into_hashmap(
    struct hashmap *map, char *buffer, int buffer_length)
{
	int buffer_index = 0;
	int key_len = 0;
	char *key_start = NULL;
	int val_len = 0;
	char *val_start = NULL;
	char input = 0;

start:
	if (buffer_index >= buffer_length) {
		return;
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
		return;
	}

	input = buffer[buffer_index++];

	if (input != '\n') {
		goto comment;
	}

	if (buffer_index + 1 >= buffer_length) {
		return;
	} else {
		goto start;
	}

key_char:
	if (buffer_index >= buffer_length) {
		return;
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
		return;
	}

	val_start = &buffer[buffer_index];
	input = buffer[buffer_index++];

	if (input == '"') {
		goto equalsign;
	} else if (input >= ' ' && input <= '~') {
		goto value_char;
	}

	return;

value_char:
	if (buffer_index >= buffer_length) {
		return;
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

	return;

closing_double_quote:
	if (buffer_index >= buffer_length) {
		return;
	}

	input = buffer[buffer_index++];

	if (input == '\n') {
		goto success;
	}

	return;

success:
	hashmap_add(
	    map,
	    (struct string){.data = key_start, .length = key_len},
	    (struct string){.data = val_start, .length = val_len});

	if (buffer_index + 1 >= buffer_length) {
		return;
	} else {
		val_len = 0;
		goto start;
	}
}
