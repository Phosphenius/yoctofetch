/*
 * SPDX-FileCopyrightText: 2025 Luca Kredel <luca.kredel@web.de>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

void __attribute__((used)) *
    memcpy(void *restrict dest, const void *restrict src, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		*((unsigned char *)dest + i) = *((unsigned char *)src + i);
	}

	return dest;
}

void __attribute__((used)) * memset(void *dest, int ch, size_t count)
{
	for (size_t i = 0; i < count; ++i) {
		*((unsigned char *)dest + i) = ch;
	}

	return dest;
}

size_t strlen(const char *str)
{
	const char *p;
	for (p = str; *p; ++p) {
		;
	}

	return p - str;
}

int string_equals(struct string s1, struct string s2)
{
	if (s1.length == 0 && s2.length == 0) {
		return 1;
	}

	/* Do not treat strings as equal as at least one of them has a length
	 * != 0 despite holding NULL as a value. */
	if (s1.data == NULL || s2.data == NULL) {
		return 0;
	}

	if (s1.length != s2.length) {
		return 0;
	}

	/* TODO: Superfluous if we had `memcmp` */
	for (int64_t i = 0; i < s1.length; ++i) {
		if (s1.data[i] != s2.data[i]) {
			return 0;
		}
	}

	return 1;
}

unsigned long string_hash(struct string str)
{
	unsigned long basis = 0x100;

	for (int64_t i = 0; i < str.length; ++i) {
		basis ^= str.data[i] & 255;
		basis *= 1111111111111111111;
	}

	return basis;
}
