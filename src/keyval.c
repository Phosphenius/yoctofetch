/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct keyval {
	struct string key;
	struct string val;
	/* TODO: Turn this into flags. */
	int filled;
	int candidate;
};

void find_keyvals_in_buffer(
    struct keyval *keyvals, int size, char *buffer, int buffer_length)
{
	enum state {
		start,
		wait,
		key,
		separator,
		closing,
		val,
	} state = start;

	char *val_beg = NULL;
	int val_len = 0;

	int key_index = 0;

	int num_candidates = 0;
	int propable_candidate_index = 0;

	char input = 0;

	for (int i = 0; i < buffer_length; ++i) {
		input = buffer[i];

		switch (state) {
		case start: {
			if (key_index > 0 && val_beg != NULL && val_len > 0) {
				keyvals[propable_candidate_index].val =
				    (struct string){.data = val_beg,
				                    .length = val_len};

				keyvals[propable_candidate_index].filled = 1;
				keyvals[propable_candidate_index].candidate = 0;

				key_index = 0;
				val_beg = NULL;
				val_len = 0;
			}

			state = wait;

			for (int j = 0; j < size; ++j) {
				if (keyvals[j].filled) {
					continue;
				}

				if (input == keyvals[j].key.data[0]) {
					keyvals[j].candidate = 1;
					num_candidates++;
					key_index++;
					state = key;

					break;
				}
			}

			break;
		}

		case wait: {
			if (input == '\n') {
				state = start;
			}

			break;
		}

		case key: {
			if (input == '=') {
				state = separator;
			} else if (num_candidates == 0) {
				key_index = 0;
				state = start;
			} else {
				for (int j = 0; j < size; ++j) {
					if (!keyvals[j].candidate) {
						continue;
					}

					if (input !=
					    keyvals[j].key.data[key_index]) {
						keyvals[j].candidate = 0;
					} else {
						propable_candidate_index = j;
					}
				}

				key_index++;
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
				key_index = 0;
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

		for (int k = 0; k < size; ++k) {
			if (keyvals[k].filled) {
				continue;
			}

			if (first == keyvals[k].key.data[0]) {
				keyvals[k].candidate = 1;
			}
		}

		for (int k = 0; k < size; ++k) {
			if (!keyvals[k].candidate) {
				continue;
			}

			for (int j = 0;
			     envp[i][j] != '\0' && j < keyvals[k].key.length;
			     ++j) {
				if (envp[i][j] != keyvals[k].key.data[j]) {
					keyvals[k].candidate = 0;
					break;
				}

				if (j + 1 == keyvals[k].key.length &&
				    envp[i][j + 1] != '\0' &&
				    envp[i][j + 1] == '=' &&
				    envp[i][j + 2] != '\0') {
					keyvals[k].filled = 1;
					keyvals[k].val = (struct string){
					    .data = envp[i] + j + 2,
					    .length =
						strlen(envp[i]) - (j + 2)};
				}
			}
		}
	}
}
