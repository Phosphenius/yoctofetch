/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define ENV_INDEX_CACHE_SIZE 16

struct env_index_cache_entry {
	size_t length;
	size_t indices[ENV_INDEX_CACHE_SIZE];
};

void populate_env_cache(
    char *envp[], struct env_index_cache_entry env_index_cache[])
{
	for (int i = 0; envp[i] != NULL; ++i) {
		char first = envp[i][0] | 32;
		int index = first - 97;

		if (index < 0 || index > 25) {
			continue;
		}

		if (env_index_cache[index].length >= ENV_INDEX_CACHE_SIZE) {
			break;
		}

		env_index_cache[index]
		    .indices[env_index_cache[index].length++] = i;
	}
}

char *getenv_or(
    char *env_name,
    char *envp[],
    struct env_index_cache_entry env_index_cache[],
    char *alt)
{
	int cache_index = (env_name[0] | 32) - 97;

	int length = env_index_cache[cache_index].length *
	             (cache_index >= 0 && cache_index <= 25);

	for (int cached_index_index = 0; cached_index_index < length;
	     ++cached_index_index) {
		int i =
		    env_index_cache[cache_index].indices[cached_index_index];
		for (int j = 0, k = 0;
		     envp[i][j] != '\0' && env_name[k] != '\0';
		     ++j, ++k) {
			if (envp[i][j] != env_name[k]) {
				break;
			}

			if (env_name[k + 1] == '\0' && envp[i][j + 1] != '\0' &&
			    envp[i][j + 1] == '=' && envp[i][j + 2] != '\0') {
				return envp[i] + j + 2;
			}
		}
	}

	/* Fallback—search without cache in case cache is too small. */
	for (int i = 0; envp[i] != NULL; ++i) {
		for (int j = 0, k = 0;
		     envp[i][j] != '\0' && env_name[k] != '\0';
		     ++j, ++k) {
			if (envp[i][j] != env_name[k]) {
				break;
			}

			if (env_name[k + 1] == '\0' && envp[i][j + 1] != '\0' &&
			    envp[i][j + 1] == '=' && envp[i][j + 2] != '\0') {
				return envp[i] + j + 2;
			}
		}
	}

	return alt;
}

char *trim_shell(char *shell_raw)
{
	if (shell_raw == NULL) {
		return shell_raw;
	}

	size_t len = strlen(shell_raw);

	for (size_t i = 0; i < len - 5; ++i) {
		if (shell_raw[i] != '/') {
			continue;
		}

		if ((shell_raw[i + 1] == 'b' && shell_raw[i + 2] == 'i' &&
		     shell_raw[i + 3] == 'n') ||
		    (shell_raw[i + 1] == 'u' && shell_raw[i + 2] == 's' &&
		     shell_raw[i + 3] == 'r')) {
			return shell_raw + i + 5;
		}
	}

	return NULL;
}
