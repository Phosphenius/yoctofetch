/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct buffer {
	char *data;
	int64_t capacity;
	int64_t length;
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

struct buffer_config {
	int64_t use_color;
	enum color color;
	enum logo logo;
	int64_t no_logo;
};

void set_color_at(char *buffer, enum color color, int64_t pos)
{
	char color_char = '9'; /* Default color */

	switch (color) {
	case BLACK:
		color_char = '0';
		break;
	case RED:
		color_char = '1';
		break;
	case GREEN:
		color_char = '2';
		break;
	case YELLOW:
		color_char = '3';
		break;
	case BLUE:
		color_char = '4';
		break;
	case MAGENTA:
		color_char = '5';
		break;
	case CYAN:
		color_char = '6';
		break;
	case WHITE:
		color_char = '7';
		break;
	}

	/* FIXME: There should be a bounds check here or at least an assert. */
	buffer[pos] = color_char;
}

/* Heavily inspired by https://nullprogram.com/blog/2023/02/13/ */

void buffer_append(
    struct buffer *restrict buffer, const char *restrict data, int64_t length)
{
	int64_t available = buffer->capacity - buffer->length;
	int64_t amount = available < length ? available : length;

	for (int64_t i = 0; i < amount; ++i) {
		buffer->data[buffer->length + i] = data[i];
	}

	buffer->length += amount;
}

void buffer_append_string(struct buffer *buffer, struct string string)
{
	buffer_append(buffer, string.data, string.length);
}

void buffer_append_char(struct buffer *buffer, char data)
{
	buffer_append(buffer, &data, 1);
}

void buffer_append_int(struct buffer *buffer, int64_t value)
{
	char tmp[64];
	char *end = tmp + sizeof tmp;
	char *beg = end;

	do {
		*--beg = '0' + value % 10;
	} while (value /= 10);

	buffer_append(buffer, beg, end - beg);
}

void buffer_append_uptime(struct buffer *buffer, long uptime)
{
	float uptime_mins = uptime / 60;
	int64_t uptime_hrs = 0;
	int64_t uptime_days = 0;

	while (uptime_mins >= (24 * 60)) {
		uptime_mins -= (24 * 60);
		uptime_days++;
	}

	while (uptime_mins >= 60) {
		uptime_mins -= 60;
		uptime_hrs++;
	}

	if (uptime_days > 0) {
		buffer_append_int(buffer, uptime_days);
	}

	if (uptime_days == 1) {
		if (uptime_hrs != 0 || uptime_mins != 0) {
			buffer_append_string(buffer, STR(" day, "));
		} else {
			buffer_append_string(buffer, STR(" day "));
		}
	} else if (uptime_days != 0) {
		if (uptime_hrs != 0 || uptime_mins != 0) {
			buffer_append_string(buffer, STR(" days, "));
		} else {
			buffer_append_string(buffer, STR(" days "));
		}
	}

	if (uptime_hrs > 0) {
		buffer_append_int(buffer, uptime_hrs);
	}

	if (uptime_hrs == 1) {
		buffer_append_string(buffer, STR(" hour"));
	} else if (uptime_hrs != 0) {
		buffer_append_string(buffer, STR(" hours"));
	}

	if (uptime_mins != 0) {
		if (uptime_hrs >= 1) {
			buffer_append_string(buffer, STR(", "));
		}

		buffer_append_int(buffer, uptime_mins);

		if (uptime_mins == 1) {
			buffer_append_string(buffer, STR(" minute"));
		} else {
			buffer_append_string(buffer, STR(" minutes"));
		}
	} else if (uptime_mins == 0 && uptime_hrs == 0 && uptime_days == 0) {
		buffer_append_string(buffer, STR("< 1 minute"));
	}

	buffer_append_char(buffer, '\n');
}
