/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#define O_RDONLY 0

#include <stddef.h>
#include <stdint.h>

#include "types.h"

#include "syscall.h"

#include "unistd.c"

#include "gather_io.c"

#include "logos.c"
#include "string.c"

#include "keyval.c"

#include "buffered_io.c"
#include "env.c"
#include "file_io.c"
#include "os_release.c"
#include "sysinfo.c"
#include "uname.c"

#include "config.c"

enum {
	FILE_BUF_LEN = 1 << 9,
	GATHER_STACK_LENGTH = 32,
};

enum {
	ENV_USER = 0,
	ENV_SHELL,
	ENV_NO_COLOR,
	ENV_XDG_CURRENT_DESKTOP,
	ENV_XDG_SESSION_TYPE,
	ENV_TERM,
};

void gather_stack_push(
    struct iovec *stack, int64_t *stack_pointer, struct iovec value)
{
	if ((*stack_pointer) + 1 >= GATHER_STACK_LENGTH) {
		/* Output will simply be truncated if stack is full. */
		return;
	}

	stack[(*stack_pointer)++] = value;
}

void gather_stack_push_buffer(
    struct iovec *stack, int64_t *stack_pointer, struct buffer buffer)
{
	gather_stack_push(
	    stack,
	    stack_pointer,
	    (struct iovec){.iov_base = buffer.data, .iov_len = buffer.length});
}

int main(
    __attribute__((unused)) int argc,
    __attribute__((unused)) char *argv[],
    char *envp[])
{
	struct iovec gather_stack[GATHER_STACK_LENGTH] = {0};
	int64_t gather_stack_pointer = 0;

	struct utsname uts = {0};
	struct sysinfo info = {0};

	/* In theory, these should never fail, but let’s test them anyways and
	 * just exit in case the impossible happens … */
	if (uname(&uts) != 0 || sysinfo(&info) != 0) {
		return -1;
	}

	struct keyval env_keyvals[] = {
	    [ENV_USER] = {.key = STR_INIT("USER"),                .val = STR_INIT("Unknown")},
	    [ENV_SHELL] =
		{.key = STR_INIT("SHELL"),               .val = STR_INIT("Unknown")},
	    [ENV_NO_COLOR] = {.key = STR_INIT("NO_COLOR"),            .val = STR_INIT("")       },
	    [ENV_XDG_CURRENT_DESKTOP] =
		{.key = STR_INIT("XDG_CURRENT_DESKTOP"),
	                  .val = STR_INIT("Unknown")                                        },
	    [ENV_XDG_SESSION_TYPE] =
		{.key = STR_INIT("XDG_SESSION_TYPE"),
	                  .val = STR_INIT("Unknown")                                        },
	    [ENV_TERM] = {.key = STR_INIT("TERM"),                .val = STR_INIT("Unknown")},
	};

	struct index_list i_list[] = {
		{.len = 1, .list = (int64_t[]){ENV_NO_COLOR}},
		{.len = 1, .list = (int64_t[]){ENV_SHELL}},
		{.len = 1, .list = (int64_t[]){ENV_TERM}},
		{.len = 1, .list = (int64_t[]){ENV_USER}},
		{.len = 2, .list = (int64_t[]){ENV_XDG_CURRENT_DESKTOP, ENV_XDG_SESSION_TYPE}}
	};

	keyvals_from_envp(env_keyvals, envp, i_list);

	struct string user = env_keyvals[ENV_USER].val;

#ifndef NO_CONFIG_FILE
	struct config config = config_from_file(user);
#else
	struct config config = {1, 1, 1, 1, 1, 1, 1, 1, 1};
#endif

	char os_release_buffer[1 << 9];

	struct os_release_result os_release_res =
	    parse_os_release(os_release_buffer, sizeof os_release_buffer);

	int64_t use_color = 1;

	use_color = string_equals(env_keyvals[ENV_NO_COLOR].val, STR(""));

	struct buffer_config buffer_config = {
	    .color = GREEN, .logo = LOGO_NONE, .use_color = use_color};

	if (string_equals(os_release_res.id, STR("guix"))) {
		buffer_config.logo = LOGO_GUIX;
		buffer_config.color = YELLOW;
		buffer_config.no_logo = 0;
	}

	if (string_equals(os_release_res.id, STR("arch"))) {
		buffer_config.logo = LOGO_ARCH;
		buffer_config.color = CYAN;
		buffer_config.no_logo = 0;
	}

	char user_at_host_buffer_backend[64] = "\033[0m\033[1m       ";
	struct buffer user_at_host_buffer = {
	    .capacity = sizeof user_at_host_buffer_backend,
	    .data = user_at_host_buffer_backend,
	    .length = 15};

	char os_buffer_backend[64] = "\033[0m\033[1;39m       OS: \033[0m";
	struct buffer os_buffer = {
	    .capacity = sizeof os_buffer_backend,
	    .data = os_buffer_backend,
	    .length = 26};

	char host_buffer_backend[64] = "\033[0m\033[1;39m       Host: \033[0m";
	struct buffer host_buffer = {
	    .capacity = sizeof host_buffer_backend,
	    .data = host_buffer_backend,
	    .length = 28};

	char kernel_buffer_backend[64] =
	    "\033[0m\033[1;39m       Kernel: \033[0m";
	struct buffer kernel_buffer = {
	    .capacity = sizeof kernel_buffer_backend,
	    .data = kernel_buffer_backend,
	    .length = 30};

	char uptime_buffer_backend[64] =
	    "\033[0m\033[1;39m       Uptime: \033[0m";
	struct buffer uptime_buffer = {
	    .capacity = sizeof uptime_buffer_backend,
	    .data = uptime_buffer_backend,
	    .length = 30};

	char shell_buffer_backend[64] =
	    "\033[0m\033[1;39m       Shell: \033[0m";
	struct buffer shell_buffer = {
	    .capacity = sizeof shell_buffer_backend,
	    .data = shell_buffer_backend,
	    .length = 29};

	char wm_buffer_backend[64] = "\033[0m\033[1;39m       WM: \033[0m";
	struct buffer wm_buffer = {
	    .capacity = sizeof wm_buffer_backend,
	    .data = wm_buffer_backend,
	    .length = 26};

	char term_buffer_backend[64] =
	    "\033[0m\033[1;39m       Terminal: \033[0m";
	struct buffer term_buffer = {
	    .capacity = sizeof term_buffer_backend,
	    .data = term_buffer_backend,
	    .length = 32};

	char mem_buffer_backend[64] = "\033[0m\033[1;39m       Memory: \033[0m";
	struct buffer mem_buffer = {
	    .capacity = sizeof mem_buffer_backend,
	    .data = mem_buffer_backend,
	    .length = 30};

	char swap_buffer_backend[64] = "\033[0m\033[1;39m       Swap: \033[0m";
	struct buffer swap_buffer = {
	    .capacity = sizeof swap_buffer_backend,
	    .data = swap_buffer_backend,
	    .length = 28};

	int64_t logo_height = 0;

	switch (buffer_config.logo) {
	case LOGO_NONE:
		logo_height = -1;
		break;
	case LOGO_GUIX:
		logo_height = GUIX_LOGO_HEIGHT;
		break;
	case LOGO_ARCH:
		logo_height = ARCH_LOGO_HEIGHT;
		break;
	};

	int64_t delay_logo = 0;
	int64_t logo_lines_written = 0;
	char trailer[] = "\033[0m\n";

	for (int64_t i = 0; i < 30; ++i) {
		if (logo_lines_written <= logo_height && !delay_logo) {
			struct iovec iov = {0};

			switch (buffer_config.logo) {
			case LOGO_NONE:
				break;

			case LOGO_GUIX:
				iov.iov_base =
				    guix_logo_data[logo_lines_written];
				iov.iov_len = GUIX_LOGO_WIDTH;

				if (buffer_config.use_color) {
					set_color_at(
					    guix_logo_data[logo_lines_written],
					    buffer_config.color,
					    5);
				}

				break;

			case LOGO_ARCH:
				iov.iov_base = arch_logo_data;
				iov.iov_len = ARCH_LOGO_WIDTH;

				if (buffer_config.use_color) {
					set_color_at(
					    arch_logo_data[logo_lines_written],
					    buffer_config.color,
					    5);
				}
				break;
			};

			logo_lines_written++;

			gather_stack_push(
			    gather_stack, &gather_stack_pointer, iov);
		}

		delay_logo = 0;

		switch (i) {
		case 0: {
			buffer_append_string(&user_at_host_buffer, user);
			buffer_append_char(&user_at_host_buffer, '@');
			buffer_append(
			    &user_at_host_buffer,
			    uts.nodename,
			    strlen(uts.nodename));
			buffer_append_char(&user_at_host_buffer, '\n');

			gather_stack_push_buffer(
			    gather_stack,
			    &gather_stack_pointer,
			    user_at_host_buffer);

			break;
		}

		case 1: {
			if (!config.show_os) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    os_buffer_backend, buffer_config.color, 9);
			}

			buffer_append_string(&os_buffer, os_release_res.name);
			buffer_append_char(&os_buffer, '\n');

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, os_buffer);

			break;
		}

		case 2: {
			if (!config.show_host) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    host_buffer_backend,
				    buffer_config.color,
				    9);
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

			if (string_equals(model_name, STR(""))) {
				buffer_append_string(&host_buffer, prod_name);
				buffer_append_string(&host_buffer, STR(" ("));
				buffer_append_string(&host_buffer, fam_name);
				buffer_append_string(&host_buffer, STR(")\n"));
			} else {
				buffer_append_string(&host_buffer, model_name);
				buffer_append_char(&host_buffer, '\n');
			}

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, host_buffer);
			break;
		}

		case 3: {
			if (!config.show_kernel) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    kernel_buffer_backend,
				    buffer_config.color,
				    9);
			}

			buffer_append(
			    &kernel_buffer, uts.sysname, strlen(uts.sysname));
			buffer_append_char(&kernel_buffer, ' ');
			buffer_append(
			    &kernel_buffer, uts.release, strlen(uts.release));
			buffer_append_string(&kernel_buffer, STR(" ("));
			buffer_append(
			    &kernel_buffer, uts.machine, strlen(uts.machine));
			buffer_append_string(&kernel_buffer, STR(")\n"));

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, kernel_buffer);

			break;
		}

		case 4: {
			if (!config.show_uptime) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    uptime_buffer_backend,
				    buffer_config.color,
				    9);
			}

			buffer_append_uptime(&uptime_buffer, info.uptime);

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, uptime_buffer);

			break;
		}

		case 5: {
			if (!config.show_shell) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    shell_buffer_backend,
				    buffer_config.color,
				    9);
			}

			struct string shell_raw = env_keyvals[ENV_SHELL].val;
			struct string shell = trim_shell(shell_raw);

			buffer_append_string(&shell_buffer, shell);
			buffer_append_char(&shell_buffer, '\n');

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, shell_buffer);

			break;
		}

		case 6: {
			if (!config.show_desktop) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    wm_buffer_backend, buffer_config.color, 9);
			}

			struct string wm =
			    env_keyvals[ENV_XDG_CURRENT_DESKTOP].val;

			struct string session =
			    env_keyvals[ENV_XDG_SESSION_TYPE].val;

			buffer_append_string(&wm_buffer, wm);
			buffer_append_string(&wm_buffer, STR(" ("));
			buffer_append_string(&wm_buffer, session);
			buffer_append_string(&wm_buffer, STR(")\n"));

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, wm_buffer);

			break;
		}

		case 7: {
			if (!config.show_terminal) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    term_buffer_backend,
				    buffer_config.color,
				    9);
			}

			struct string term = env_keyvals[ENV_TERM].val;

			buffer_append_string(&term_buffer, term);
			buffer_append_char(&term_buffer, '\n');

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, term_buffer);

			break;
		}

		case 8: {
			if (!config.show_memory) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    mem_buffer_backend, buffer_config.color, 9);
			}

			buffer_append_int(
			    &mem_buffer,
			    (info.totalram - info.freeram - info.bufferram) /
				1024 / 1024);
			buffer_append_string(&mem_buffer, STR("M / "));
			buffer_append_int(
			    &mem_buffer, info.totalram / 1024 / 1024);
			buffer_append_string(&mem_buffer, STR("M\n"));

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, mem_buffer);
			break;
		}

		case 9: {
			if (!config.show_swap) {
				delay_logo = 1;
				continue;
			}

			if (buffer_config.use_color) {
				set_color_at(
				    swap_buffer_backend,
				    buffer_config.color,
				    9);
			}

			buffer_append_int(
			    &swap_buffer,
			    (info.totalswap - info.freeswap) / 1024 / 1024);
			buffer_append_string(&swap_buffer, STR("M / "));
			buffer_append_int(
			    &swap_buffer, info.totalswap / 1024 / 1024);
			buffer_append_string(&swap_buffer, STR("M\n"));

			gather_stack_push_buffer(
			    gather_stack, &gather_stack_pointer, swap_buffer);
			break;
		}

		default: {
			struct iovec iov = {
			    .iov_base = trailer, .iov_len = sizeof trailer};

			gather_stack_push(
			    gather_stack, &gather_stack_pointer, iov);
			if (logo_lines_written > logo_height) {
				/* FIXME: I know … */
				i = 100;
			}
		}
		}
	}

	writev(STDOUT_FILENO, gather_stack, gather_stack_pointer + 1);

	return 0;
}
