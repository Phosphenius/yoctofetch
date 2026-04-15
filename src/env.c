/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define ENV_INDEX_CACHE_SIZE 20

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
			/* FIXME: Shouldn’t we continue here instead of break?
			 * Breaking the loop stops  */
			/* building the rest of the cache. */
			break;
		}

		env_index_cache[index]
		    .indices[env_index_cache[index].length++] = i;
	}
}

char *find_env_name(char *env_name, int index, char *envp[])
{
	for (int i = 0; envp[index][i] != '\0' && env_name[i] != '\0'; ++i) {
		if (envp[index][i] != env_name[i]) {
			break;
		}

		if (env_name[i + 1] == '\0' && envp[index][i + 1] != '\0' &&
		    envp[index][i + 1] == '=' && envp[index][i + 2] != '\0') {
			return envp[index] + i + 2;
		}
	}

	return NULL;
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

		char *res = find_env_name(env_name, i, envp);

		if (res != NULL) {
			return res;
		}
	}

	/* Fallback—search without cache in case cache is too small. */
	if ((length + 1) >= ENV_INDEX_CACHE_SIZE) {
		for (int i = 0; envp[i] != NULL; ++i) {
			char *res = find_env_name(env_name, i, envp);

			if (res != NULL) {
				return res;
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
