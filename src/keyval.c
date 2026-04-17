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
	uint64_t flags;
};

struct index_list {
	int64_t len;
	int64_t *list;
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


void keyvals_from_envp(
    struct keyval *keyvals,
    char *envp[],
    struct index_list *index_list)
{
	int64_t index_map[] = {0, 0, 0, 0, 0, 0, 0, 0, 0,
	                         0, 0, 0, 0, 1,  0, 0, 0, 0,
	                         2,  3,  4,  0, 0, 5,  0, 0};

	for (int64_t i = 0; envp[i] != NULL; ++i) {
		char first = envp[i][0];

		int64_t mapped_index = (first | 32) - 97;
		mapped_index =
		    mapped_index * (mapped_index >= 0 && mapped_index <= 25);
		mapped_index = index_map[mapped_index] - 1;

		if (mapped_index == -1) {
			continue;
		}

		for (int64_t k = 0; k < index_list[mapped_index].len; ++k) {
			int64_t l = index_list[mapped_index].list[k];
			if (keyvals[l].flags & KEYVAL_FILLED) {
				continue;
			}

			for (int64_t j = 0;
			     envp[i][j] != '\0' && j < keyvals[l].key.length;
			     ++j) {
				if (envp[i][j] != keyvals[l].key.data[j]) {
					break;
				}

				if (j + 1 == keyvals[l].key.length &&
				    envp[i][j + 1] != '\0' &&
				    envp[i][j + 1] == '=' &&
				    envp[i][j + 2] != '\0') {
					keyvals[l].flags |= KEYVAL_FILLED;
					keyvals[l].val = (struct string){
					    .data = envp[i] + j + 2,
					    .length =
						strlen(envp[i]) - (j + 2)};
				}
			}
		}
	}
}
