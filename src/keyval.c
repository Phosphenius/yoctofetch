/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct keyval {
	struct string key;
	struct string val;
	int filled;
};

void find_keyvals_in_buffer(
    struct keyval *keyvals, int size, char *buffer, int buffer_length)
{
	enum state {
		start,
		key,
		separator,
		closing,
		val,
	} state = start;

	char *key_beg = NULL;
	int key_len = 0;

	char *val_beg = NULL;
	int val_len = 0;

	int keyval_index = 0;

	char input = 0;

	for (int i = 0; i < buffer_length; ++i) {
		input = buffer[i];

		switch (state) {
		case start: {
			if (key_beg != NULL && key_len > 0 && val_beg != NULL &&
			    val_len > 0 && keyval_index < size) {
				keyvals[keyval_index++] = (struct keyval){
				    .key = (struct string){.data = key_beg,
				                           .length = key_len},
				    .val = (struct string){.data = val_beg,
				                           .length = val_len}
                                };

				key_beg = NULL;
				key_len = 0;
				val_beg = NULL;
				val_len = 0;
			}

			if (((input | 32) >= 'a' && (input | 32) <= 'z') ||
			    input == '_') {
				key_beg = buffer + i;
				key_len++;
				state = key;
			}

			break;
		}

		case key: {
			if (((input | 32) >= 'a' && (input | 32) <= 'z') ||
			    input == '_') {
				key_len++;
			} else if (input == '=') {
				state = separator;
			} else {
				key_beg = NULL;
				key_len = 0;
				state = start;
			}

			break;
		}

		case separator: {
			if (input != '"' && input != ' ' && input > ' ' &&
			    input <= '~') {
				val_beg = buffer + i;
				val_len++;

				state = val;
			}

			break;
		}

		case val: {
			if (input == '"') {
				state = closing;
			} else if (input == '\n') {
				state = start;
			} else if (input >= ' ' && input <= '~') {
				val_len++;
			}
			break;
		}

		case closing: {
			if (input != '\n') {
				key_beg = NULL;
				key_len = 0;
				val_beg = NULL;
				val_len = 0;
			}

			state = start;

			break;
		}
		}
	}
}

void keyvals_from_envp(struct keyval *keyvals, int size, char *envp[])
{
	for (int i = 0; envp[i] != NULL; ++i) {
		char first = envp[i][0];

		if (first != 'N' && first != 'S' && first != 'T' &&
		    first != 'U' && first != 'X') {
			continue;
		}

		int keyval_index = 0;

		for (int k = 0; k < size; ++k) {
			if(keyvals[k].filled) {
				continue;
		}

			if (first == keyvals[k].key.data[0]) {
				keyval_index = k;
			}
		}

		for (int j = 0;
		     envp[i][j] != '\0' && j < keyvals[keyval_index].key.length;
		     ++j) {
			if (envp[i][j] != keyvals[keyval_index].key.data[j]) {
				break;
			}

			if (j + 1 == keyvals[keyval_index].key.length &&
			    envp[i][j + 1] != '\0' && envp[i][j + 1] == '=' &&
			    envp[i][j + 2] != '\0') {
				keyvals[keyval_index].filled = 1;
				keyvals[keyval_index].val = (struct string){
				    .data = envp[i] + j + 2,
				    .length = strlen(envp[i]) - (j + 2)};
			}
		}
	}
}

struct string keyval_get_or(
    struct keyval *keyvals,
    int size,
    struct string key,
    struct string alt,
    int *found)
{
	for (int i = 0; i < size; ++i) {
		if (found[i]) {
			continue;
		}

		if (string_equals(key, keyvals[i].key)) {
			found[i] = 1;
			return keyvals[i].val;
		}
	}

	return alt;
}
