/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct buffer {
	char *data;
	long int capacity;
	long int length;
	int fd;
	int error;
};

enum color {
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
};

struct config {
	int use_color;
	enum color color;
	int curr_logo_line;
	struct logo logo;
	int no_logo;
};

static const struct string bold_color_table[] = {
    [BLACK] = STR_INIT("\033[1;30m"),
    [RED] = STR_INIT("\033[1;31m"),
    [GREEN] = STR_INIT("\033[1;32m"),
    [YELLOW] = STR_INIT("\033[1;33m"),
    [BLUE] = STR_INIT("\033[1;34m"),
    [MAGENTA] = STR_INIT("\033[1;35m"),
    [CYAN] = STR_INIT("\033[1;36m"),
    [WHITE] = STR_INIT("\033[1;37m"),
};

static const struct string RESET = STR_INIT("\033[0m");

static const struct string BOLD = STR_INIT("\033[1m");
static const struct string BOLD_RESET = STR_INIT("\033[22m");

/* Heavily inspired by https://nullprogram.com/blog/2023/02/13/ */

void buffer_flush(struct buffer *buffer)
{
	buffer->error |= buffer->fd < 0;

	if (!buffer->error && buffer->length) {
		long int chars_written = 0;

		do {
			chars_written = (long int)syscall3(
			    __NR_write,
			    (void *)(size_t)buffer->fd,
			    (void *)buffer->data,
			    (void *)buffer->length);

			buffer->length -= chars_written * (chars_written >= 0);
			buffer->error |= chars_written < 0;
		} while (chars_written != 0 && !buffer->error);
	}
}

void buffer_append(
    struct buffer *restrict buffer, const char *restrict data, long int length)
{
	const char *end = data + length;

	while (!buffer->error && data < end) {
		long int left = end - data;
		long int available = buffer->capacity - buffer->length;
		long int amount = available < left ? available : left;

		for (long int i = 0; i < amount; ++i) {
			buffer->data[buffer->length + i] = data[i];
		}

		buffer->length += amount;
		data += amount;

		if (amount < left) {
			buffer_flush(buffer);
		}
	}
}

void buffer_append_string(struct buffer *buffer, struct string string)
{
	buffer_append(buffer, string.data, string.length);
}

void buffer_append_char(struct buffer *buffer, char data)
{
	buffer_append(buffer, &data, 1);
}

void buffer_append_int(struct buffer *buffer, int value)
{
	char tmp[64];
	char *end = tmp + sizeof tmp;
	char *beg = end;

	do {
		*--beg = '0' + value % 10;
	} while (value /= 10);

	buffer_append(buffer, beg, end - beg);
}

void buffer_apply_bold_color(
    struct buffer *buffer, enum color color, int use_color)
{
	if (use_color) {
		buffer_append_string(buffer, bold_color_table[color]);
	} else {
		buffer_append_string(buffer, BOLD);
	}
}

void buffer_reset_bold_color(struct buffer *buffer, int use_color)
{
	if (use_color) {
		buffer_append_string(buffer, RESET);
	} else {
		buffer_append_string(buffer, BOLD_RESET);
	}
}

void buffer_append_logo_line(struct buffer *buffer, struct config *config)
{
	if (config->no_logo) {
		return;
	}

	buffer_apply_bold_color(buffer, config->color, config->use_color);

	if (config->curr_logo_line > config->logo.height) {
		for (int i = 0; i < config->logo.width; ++i) {
			buffer_append_char(buffer, ' ');
		}

		buffer_append_char(buffer, '\t');

		return;
	}

	buffer_append(
	    buffer,
	    config->logo.data[config->curr_logo_line++],
	    config->logo.width);
	buffer_append_char(buffer, '\t');

	buffer_reset_bold_color(buffer, config->use_color);
}

void buffer_append_user_at_host(
    struct buffer *buffer, struct config *config, char *user, char *nodename)
{
	buffer_append_logo_line(buffer, config);

	buffer_append_string(buffer, BOLD);
	buffer_append(buffer, user, strlen(user));
	buffer_append_char(buffer, '@');
	buffer_append(buffer, nodename, strlen(nodename));
	buffer_append_string(buffer, BOLD_RESET);
	buffer_append_char(buffer, '\n');
}

void buffer_append_os(
    struct buffer *buffer, struct config *config, struct string os_name)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("OS: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append_string(buffer, os_name);
	buffer_append_char(buffer, '\n');
}

void buffer_append_host(
    struct buffer *buffer,
    struct config *config,
    struct string prod_name,
    struct string fam_name)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Host: "));
	buffer_reset_bold_color(buffer, config->use_color);
	buffer_append_string(buffer, prod_name);
	buffer_append_string(buffer, STR(" ("));
	buffer_append_string(buffer, fam_name);
	buffer_append_string(buffer, STR(")\n"));
}
void buffer_append_model(
    struct buffer *buffer, struct config *config, struct string model_name)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Host: "));
	buffer_reset_bold_color(buffer, config->use_color);
	buffer_append_string(buffer, model_name);
	buffer_append_char(buffer, '\n');
}

void buffer_append_kernel(
    struct buffer *buffer,
    struct config *config,
    char *sysname,
    char *release,
    char *machine)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Kernel: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append(buffer, sysname, strlen(sysname));
	buffer_append_char(buffer, ' ');
	buffer_append(buffer, release, strlen(release));
	buffer_append_string(buffer, STR(" ("));
	buffer_append(buffer, machine, strlen(machine));
	buffer_append_string(buffer, STR(")\n"));
}

void buffer_append_uptime(
    struct buffer *buffer, struct config *config, long uptime)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Uptime: "));
	buffer_reset_bold_color(buffer, config->use_color);

	float uptime_mins = uptime / 60;
	int uptime_hrs = 0;
	int uptime_days = 0;

	while (uptime_mins >= (24 * 60)) {
		uptime_mins -= (24 * 60);
		uptime_days++;
	}

	while (uptime_mins >= 60) {
		uptime_mins -= 60;
		uptime_hrs++;
	}

	buffer_append_int(buffer, uptime_days);

	if (uptime_days == 1) {
		buffer_append_string(buffer, STR(" day, "));
	} else if (uptime_days != 0) {
		buffer_append_string(buffer, STR(" days, "));
	}

	buffer_append_int(buffer, uptime_hrs);

	if (uptime_hrs == 1) {
		buffer_append_string(buffer, STR(" hour"));
	} else if (uptime_hrs != 0) {
		buffer_append_string(buffer, STR(" hours"));
	}

	if (uptime_mins != 0) {
		buffer_append_string(buffer, STR(", "));
		buffer_append_int(buffer, uptime_mins);

		if (uptime_mins == 1) {
			buffer_append_string(buffer, STR(" minute"));
		} else {
			buffer_append_string(buffer, STR(" minutes"));
		}
	}

	buffer_append_char(buffer, '\n');
}

void buffer_append_shell(
    struct buffer *buffer, struct config *config, char *shell)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Shell: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append(buffer, shell, strlen(shell));
	buffer_append_char(buffer, '\n');
}

void buffer_append_desktop(
    struct buffer *buffer, struct config *config, char *wm, char *session)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("WM: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append(buffer, wm, strlen(wm));
	buffer_append_string(buffer, STR(" ("));
	buffer_append(buffer, session, strlen(session));
	buffer_append_string(buffer, STR(")\n"));
}

void buffer_append_terminal(
    struct buffer *buffer, struct config *config, char *term)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Terminal: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append(buffer, term, strlen(term));
	buffer_append_char(buffer, '\n');
}

void buffer_append_memory(
    struct buffer *buffer, struct config *config, int used_ram, int total_ram)
{
	buffer_append_logo_line(buffer, config);

	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Memory: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append_int(buffer, used_ram);
	buffer_append_string(buffer, STR("M / "));
	buffer_append_int(buffer, total_ram);
	buffer_append_string(buffer, STR("M\n"));
}

void buffer_append_swap(
    struct buffer *buffer, struct config *config, int used_swap, int total_swap)
{
	buffer_append_logo_line(buffer, config);
	buffer_apply_bold_color(buffer, config->color, config->use_color);
	buffer_append_string(buffer, STR("Swap: "));
	buffer_reset_bold_color(buffer, config->use_color);

	buffer_append_int(buffer, used_swap);

	buffer_append_string(buffer, STR("M / "));
	buffer_append_int(buffer, total_swap);
	buffer_append_string(buffer, STR("M\n"));
}
