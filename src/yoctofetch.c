/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stddef.h>

#include "types.h"

#include "syscall.h"

#include "unistd.c"

#include "logos.c"
#include "string.c"

#include "argparse.c"
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

int main(int argc, char *argv[], char *envp[])
{
	char stdout_buffer_backend[1 << 12];

	struct buffer stdout_buffer = {
	    .fd = STDOUT_FILENO,
	    .capacity = sizeof stdout_buffer_backend,
	    .data = stdout_buffer_backend,
	    .error = 0,
	    .length = 0};

	if (test_arg(STR("--version"), argc, argv) ||
	    test_arg(STR("-V"), argc, argv)) {
		buffer_append_string(
		    &stdout_buffer,
		    STR("yoctofetch 1.2.0\n"
		        "Copyright (C) 2025 Luca Kredel\n"
		        "License GPLv3+: GNU Affero GPL version 3 or later "
		        "<https://www.gnu.org/licenses/gpl-3.0.html>\n\n"

		        "This is free software: you are free to change and "
		        "redistribute it.\n"
		        "There is NO WARRANTY, to the extent permitted by "
		        "law.\n"));
		buffer_flush(&stdout_buffer);

		return 0;
	} else if (
	    test_arg(STR("--help"), argc, argv) ||
	    test_arg(STR("-h"), argc, argv)) {
		buffer_append_string(
		    &stdout_buffer,
		    STR("Usage: yoctofetch [OPTION] …\n\n"
		        "\t-H, --no-user-at-host\tDo not display user@host\n"
		        "\t-O, --no-os\t\tDo not display OS\n"
		        "\t-K, --no-kernel\t\tDo not display kernel name and "
		        "version.\n"
		        "\t-U, --no-uptime\t\tDo not display uptime.\n"
		        "\t-S, --no-shell\t\tDo not display shell.\n"
		        "\t-D, --no-desktop\tDo not display desktop.\n"
		        "\t-T, --no-terminal\tDo not display terminal.\n"
		        "\t-M, --no-memory\t\tDo not display memory.\n"
		        "\t-P, --no-swap\t\tDo not display swap.\n\n"
		        "\t--guix-logo\t\tDisplay Guix logo.\n"
		        "\t--arch-logo\t\tDisplay Arch logo.\n\n"
		        "\t-V, --version\t\tShow version and exit.\n"
		        "\t-h, --help\t\tShow this help and exit.\n\n"
		        "\t--bottom-padding\tAdd 3 lines of padding.\n"));
		buffer_flush(&stdout_buffer);

		return 0;
	}

	struct utsname uts = {0};
	struct sysinfo info = {0};

	/* In theory, these should never fail, but let’s test them anyways and
	 * just exit in case the impossible happens … */
	if (uname(&uts) != 0 || sysinfo(&info) != 0) {
		return -1;
	}

	struct env_index_cache_entry env_index_cache[26] = {0};
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

	if (string_equals(os_release_res.id, STR("guix")) ||
	    test_arg(STR("--guix-logo"), argc, argv)) {
		config.logo = guix_logo;
		config.color = YELLOW;
		config.no_logo = 0;
	}

	if (string_equals(os_release_res.id, STR("arch")) ||
	    test_arg(STR("--arch-logo"), argc, argv)) {
		config.logo = arch_logo;
		config.color = CYAN;
		config.no_logo = 0;
	}

	if (test_arg(STR("--no-logo"), argc, argv) ||
	    test_arg(STR("-L"), argc, argv)) {
		config.no_logo = 1;
	}

	if (!(test_arg(STR("--user-at-host"), argc, argv) ||
	      test_arg(STR("-H"), argc, argv))) {
		buffer_append_user_at_host(
		    &stdout_buffer, &config, user, uts.nodename);
	}

	if (!(test_arg(STR("--no-os"), argc, argv) ||
	      test_arg(STR("-O"), argc, argv))) {
		buffer_append_os(&stdout_buffer, &config, os_release_res.name);
	}

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

	if (!(test_arg(STR("--no-host"), argc, argv) ||
	      test_arg(STR("-C"), argc, argv))) {
		if (string_equals(model_name, STR(""))) {
			buffer_append_host(
			    &stdout_buffer, &config, prod_name, fam_name);
		} else {
			buffer_append_model(
			    &stdout_buffer, &config, model_name);
		}
	}

	if (!(test_arg(STR("--no-kernel"), argc, argv) ||
	      test_arg(STR("-K"), argc, argv))) {
		buffer_append_kernel(
		    &stdout_buffer,
		    &config,
		    uts.sysname,
		    uts.release,
		    uts.machine);
	}

	if (!(test_arg(STR("--no-uptime"), argc, argv) ||
	      test_arg(STR("-U"), argc, argv))) {
		buffer_append_uptime(&stdout_buffer, &config, info.uptime);
	}

	if (!(test_arg(STR("--no-shell"), argc, argv) ||
	      test_arg(STR("-S"), argc, argv))) {
		char *shell_raw =
		    getenv_or("SHELL", envp, env_index_cache, NULL);
		char *shell = trim_shell(shell_raw);

		if (shell == NULL) {
			shell = "Unknown";
		}

		buffer_append_shell(&stdout_buffer, &config, shell);
	}

	if (!(test_arg(STR("--no-desktop"), argc, argv) ||
	      test_arg(STR("-D"), argc, argv))) {
		char *wm = getenv_or(
		    "XDG_CURRENT_DESKTOP", envp, env_index_cache, "Unknown");

		char *session = getenv_or(
		    "XDG_SESSION_TYPE", envp, env_index_cache, "Unknown");

		buffer_append_desktop(&stdout_buffer, &config, wm, session);
	}

	if (!(test_arg(STR("--no-terminal"), argc, argv) ||
	      test_arg(STR("-T"), argc, argv))) {
		char *term =
		    getenv_or("TERM", envp, env_index_cache, "Unknown");

		buffer_append_terminal(&stdout_buffer, &config, term);
	}

	if (!(test_arg(STR("--no-memory"), argc, argv) ||
	      test_arg(STR("-M"), argc, argv))) {
		buffer_append_memory(
		    &stdout_buffer,
		    &config,
		    (info.totalram - info.freeram - info.bufferram) / 1024 /
			1024,
		    info.totalram / 1024 / 1024);
	}

	if (!(test_arg(STR("--no-swap"), argc, argv) ||
	      test_arg(STR("-P"), argc, argv))) {
		buffer_append_swap(
		    &stdout_buffer,
		    &config,
		    (info.totalswap - info.freeswap) / 1024 / 1024,
		    info.totalswap / 1024 / 1024);
	}

	while (config.curr_logo_line <= config.logo.height && !config.no_logo) {
		buffer_append_logo_line(&stdout_buffer, &config);
		buffer_append_char(&stdout_buffer, '\n');
	}

	if (test_arg(STR("--bottom-padding"), argc, argv)) {
		buffer_append_string(&stdout_buffer, STR("\n\n\n"));
	}

	buffer_flush(&stdout_buffer);

	return 0;
}
