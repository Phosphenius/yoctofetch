/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

enum {
	KEYVAL_FILLED = 1,
	KEYVAL_CANDIDATE = 2
};

struct keyval {
	struct string key;
	struct string val;
	int flags;
};

void find_keyvals_in_buffer(
    struct keyval *keyvals, int64_t size, char *buffer, int64_t buffer_length)
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
	int64_t val_len = 0;

	int64_t key_index = 0;

	int64_t num_candidates = 0;
	int64_t propable_candidate_index = 0;

	char input = 0;

	for (int64_t i = 0; i < buffer_length; ++i) {
		input = buffer[i];

		switch (state) {
		case start: {
			if (key_index > 0 && val_beg != NULL && val_len > 0) {
				keyvals[propable_candidate_index].val =
				    (struct string){.data = val_beg,
				                    .length = val_len};

				keyvals[propable_candidate_index].flags |=
				    KEYVAL_FILLED;
				keyvals[propable_candidate_index].flags &=
				    ~KEYVAL_CANDIDATE;

				key_index = 0;
				val_beg = NULL;
				val_len = 0;
			}

			state = wait;

			for (int64_t j = 0; j < size; ++j) {
				if (keyvals[j].flags & KEYVAL_FILLED) {
					continue;
				}

				if (input == keyvals[j].key.data[0]) {
					keyvals[j].flags |= KEYVAL_CANDIDATE;
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
				for (int64_t j = 0; j < size; ++j) {
					if (!(keyvals[j].flags &
					      KEYVAL_CANDIDATE)) {
						continue;
					}

					if (input !=
					    keyvals[j].key.data[key_index]) {
						keyvals[j].flags &=
						    ~KEYVAL_CANDIDATE;
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
			if (keyvals[k].flags & KEYVAL_FILLED) {
				continue;
			}

			if (first == keyvals[k].key.data[0]) {
				keyvals[k].flags |= KEYVAL_CANDIDATE;
			}
		}

		for (int k = 0; k < size; ++k) {
			if (!(keyvals[k].flags & KEYVAL_CANDIDATE)) {
				continue;
			}

			for (int j = 0;
			     envp[i][j] != '\0' && j < keyvals[k].key.length;
			     ++j) {
				if (envp[i][j] != keyvals[k].key.data[j]) {
					keyvals[k].flags &= ~KEYVAL_CANDIDATE;
					break;
				}

				if (j + 1 == keyvals[k].key.length &&
				    envp[i][j + 1] != '\0' &&
				    envp[i][j + 1] == '=' &&
				    envp[i][j + 2] != '\0') {
					keyvals[k].flags |= KEYVAL_FILLED;
					keyvals[k].val = (struct string){
					    .data = envp[i] + j + 2,
					    .length =
						strlen(envp[i]) - (j + 2)};
				}
			}
		}
	}
}
