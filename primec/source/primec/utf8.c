
/**
 * @file utf8.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-11-14
 */

#include <primec/utf8.h>

static const uint8_t g_masks[] =
{
	0x7F,
	0x1F,
	0x0F,
	0x07,
	0x03,
	0x01
};

static const struct
{
	uint8_t mask;
	uint8_t result;
	int32_t octets;
} g_sizes[] =
{
	{ 0x80, 0x00,  1 },
	{ 0xE0, 0xC0,  2 },
	{ 0xF0, 0xE0,  3 },
	{ 0xF8, 0xF0,  4 },
	{ 0xFC, 0xF8,  5 },
	{ 0xFE, 0xFC,  6 },
	{ 0x80, 0x80, -1 },
};

static int64_t get_utf8_size(
	const uint8_t c);

uint32_t primec_utf8_decode(
	const char** const string)
{
	const uint8_t** s = (const uint8_t**)string;
	uint32_t cp = 0;

	if (**s < 128)
	{
		cp = **s;
		++(*s);
		return cp;
	}

	int64_t size = get_utf8_size(**s);

	if (size == -1)
	{
		++(*s);
		return primec_utf8_invalid;
	}

	uint8_t mask = g_masks[size - 1];
	cp = **s & mask;
	++(*s);

	while (--size)
	{
		uint8_t c = **s;
		++*s;

		if ((c >> 6) != 0x02)
		{
			return primec_utf8_invalid;
		}

		cp <<= 6;
		cp |= c & 0x3f;
	}

	return cp;
}

uint64_t primec_utf8_encode(
	char* const string,
	uint32_t c)
{
	uint64_t length = 0;
	uint8_t first;

	if (c < 0x80)
	{
		first = 0;
		length = 1;
	}
	else if (c < 0x800)
	{
		first = 0xc0;
		length = 2;
	}
	else if (c < 0x10000)
	{
		first = 0xe0;
		length = 3;
	}
	else
	{
		first = 0xf0;
		length = 4;
	}

	for (uint64_t index = length - 1; index > 0; --index)
	{
		string[index] = (char)((c & 0x3f) | 0x80);
		c >>= 6;
	}

	string[0] = (char)(c | first);
	return length;
}

uint32_t primec_utf8_get(
	FILE* const file)
{
	char buffer[primec_utf8_max_size];
	const int32_t c = fgetc(file);

	if (c == EOF)
	{
		return primec_utf8_invalid;
	}

	buffer[0] = (char)c;
	const int64_t size = get_utf8_size((uint8_t)c);

	if (size > (int64_t)primec_utf8_max_size)
	{
		(void)fseek(file, size - 1, SEEK_CUR);
		return primec_utf8_invalid;
	}

	if (size > 1)
	{
		const size_t amt = fread(
			&buffer[1], 1, (size_t)(size - 1), file
		);

		if (amt != (size_t)(size - 1))
		{
			return primec_utf8_invalid;
		}
	}

	const char* ptr = buffer;
	return primec_utf8_decode(&ptr);
}

static int64_t get_utf8_size(
	const uint8_t c)
{
	const uint64_t count = (uint64_t)(
		sizeof(g_sizes) / sizeof(g_sizes[0])
	);

	for (uint64_t i = 0; i < count; ++i)
	{
		if ((c & g_sizes[i].mask) == g_sizes[i].result)
		{
			return g_sizes[i].octets;
		}
	}

	return -1;
}
