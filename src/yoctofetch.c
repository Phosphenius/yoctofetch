/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define O_RDONLY 0

#include <stddef.h>

#include "types.h"

#include "syscall.h"

#include "unistd.c"

#include "logos.c"
#include "string.c"

#include "buffered_io.c"
#include "env.c"
#include "os_release.c"
#include "sysinfo.c"
#include "uname.c"

enum {
	FILE_BUF_LEN = 128,
};

struct string read_file(char *file, char *buf, int buf_len, struct string alt)
{
	struct fd_result fd_res = open(file, O_RDONLY);

	if (!fd_res.ok) {
		return alt;
	}

	struct io_result read_res = read(fd_res.fd, buf, buf_len);

	if (!read_res.ok) {
		return alt;
	}

	return (struct string){.data = buf, .length = read_res.n_bytes - 1};
}

int main(
    __attribute__((unused)) int argc,
    __attribute__((unused)) char *argv[],
    char *envp[])
{
	char stdout_buffer_backend[1 << 12];

	struct buffer stdout_buffer = {
	    .fd = STDOUT_FILENO,
	    .capacity = sizeof stdout_buffer_backend,
	    .data = stdout_buffer_backend,
	    .error = 0,
	    .length = 0};

	struct utsname uts = {0};
	struct sysinfo info = {0};

	/* In theory, these should never fail, but let’s test them anyways and
	 * just exit in case the impossible happens … */
	if (uname(&uts) != 0 || sysinfo(&info) != 0) {
		return -1;
	}

	struct env_index_cache_entry
	    env_index_cache[NUM_ENV_INDEX_CACHE_ENTRIES] = {0};
	populate_env_cache(envp, env_index_cache);

	char *user = getenv_or("USER", envp, env_index_cache, "Unknown");

	char os_release_buffer[1 << 12];

	struct os_release_result os_release_res =
	    parse_os_release(os_release_buffer, sizeof os_release_buffer);

	int use_color = 1;

	use_color = getenv_or("NO_COLOR", envp, env_index_cache, NULL) == NULL;

	struct config config = {
	    .color = GREEN,
	    .curr_logo_line = 0,
	    .no_logo = 1,
	    .logo = {0},
	    .use_color = use_color};

	if (string_equals(os_release_res.id, STR("guix"))) {
		config.logo = guix_logo;
		config.color = YELLOW;
		config.no_logo = 0;
	}

	if (string_equals(os_release_res.id, STR("arch"))) {
		config.logo = arch_logo;
		config.color = CYAN;
		config.no_logo = 0;
	}

	buffer_append_user_at_host(&stdout_buffer, &config, user, uts.nodename);

	buffer_append_os(&stdout_buffer, &config, os_release_res.name);

	char prod_name_buf[FILE_BUF_LEN] = {0};
	char fam_name_buf[FILE_BUF_LEN] = {0};

	struct string prod_name = read_file(
	    "/sys/devices/virtual/dmi/id/product_name",
	    prod_name_buf,
	    FILE_BUF_LEN,
	    STR("Unknown"));

	struct string fam_name = read_file(
	    "/sys/devices/virtual/dmi/id/product_family",
	    fam_name_buf,
	    FILE_BUF_LEN,
	    STR("Unknown"));

	struct string model_name = STR("");

	if (string_equals(prod_name, STR("Unknown")) &&
	    string_equals(fam_name, STR("Unknown"))) {
		memset(prod_name_buf, 0, FILE_BUF_LEN);
		model_name = read_file(
		    "/sys/firmware/devicetree/base/model",
		    prod_name_buf,
		    FILE_BUF_LEN,
		    STR("Unknown"));
	}

	if (string_equals(model_name, STR(""))) {
		buffer_append_host(
		    &stdout_buffer, &config, prod_name, fam_name);
	} else {
		buffer_append_model(&stdout_buffer, &config, model_name);
	}

	struct string strings[] = {
	    (struct string){.data = uts.sysname, .length = strlen(uts.sysname)},
	    STR_INIT(" "),
	    (struct string){.data = uts.release, .length = strlen(uts.release)},
	};

	buffer_append_name_value(
	    &stdout_buffer,
	    &config,
	    STR("Kernel: "),
	    sizeof strings / sizeof strings[0],
	    strings);

	buffer_append_uptime(&stdout_buffer, &config, info.uptime);

	char *shell_raw = getenv_or("SHELL", envp, env_index_cache, NULL);
	char *shell = trim_shell(shell_raw);

	if (shell == NULL) {
		shell = "Unknown";
	}

	buffer_append_name_value(
	    &stdout_buffer,
	    &config,
	    STR("Shell: "),
	    1,
	    &(struct string){.data = shell, .length = strlen(shell)});

	char *wm =
	    getenv_or("XDG_CURRENT_DESKTOP", envp, env_index_cache, "Unknown");

	char *session =
	    getenv_or("XDG_SESSION_TYPE", envp, env_index_cache, "Unknown");

	buffer_append_desktop(&stdout_buffer, &config, wm, session);

	char *term = getenv_or("TERM", envp, env_index_cache, "Unknown");

	buffer_append_name_value(
	    &stdout_buffer,
	    &config,
	    STR("Terminal: "),
	    1,
	    &(struct string){.data = term, .length = strlen(term)});

	buffer_append_memory(
	    &stdout_buffer,
	    &config,
	    (info.totalram - info.freeram - info.bufferram) / 1024 / 1024,
	    info.totalram / 1024 / 1024);

	buffer_append_swap(
	    &stdout_buffer,
	    &config,
	    (info.totalswap - info.freeswap) / 1024 / 1024,
	    info.totalswap / 1024 / 1024);

	while (config.curr_logo_line <= config.logo.height && !config.no_logo) {
		buffer_append_logo_line(&stdout_buffer, &config);
		buffer_append_char(&stdout_buffer, '\n');
	}

	buffer_flush(&stdout_buffer);

	return 0;
}
