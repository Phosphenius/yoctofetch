/*
 * SPDX-FileCopyrightText: 2026 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

struct hashmap {
	int capacity;
	struct string *data;
};

void hashmap_add(struct hashmap *map, struct string key, struct string value)
{
	int index = string_hash(key) % map->capacity;

	/* We literally do not care about collisions. */
	map->data[index] = value;
}

struct string
    hashmap_get_or(struct hashmap *map, struct string key, struct string alt)
{
	int index = string_hash(key) % map->capacity;

	if (string_equals(map->data[index], STR(""))) {
		return alt;
	}

	return map->data[index];
}
