/* This file is part of hdd_serial_spoofer by namazso, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) namazso 2018
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include <random>
#include "fnv.hpp"
#include <tuple>
#include <ntddk.h>

extern unsigned long long g_startup_time;

static inline bool is_good_char(char c)
{
	const auto u = uint8_t(c);
	return (u >= uint8_t('0') && u <= uint8_t('9'))
		|| (u >= uint8_t('A') && u <= uint8_t('Z'))
		|| (u >= uint8_t('a') && u <= uint8_t('z'));
}
static inline bool is_hex(char c)
{
	const auto u = uint8_t(c);
	return (u >= uint8_t('0') && u <= uint8_t('9'))
		|| (u >= uint8_t('A') && u <= uint8_t('F'))
		|| (u >= uint8_t('a') && u <= uint8_t('f'));
}
static inline uint8_t unhex_char(char c)
{
	const auto u = uint8_t(c);
	if (u >= uint8_t('0') && u <= uint8_t('9'))
		return u - uint8_t('0');
	if (u >= uint8_t('A') && u <= uint8_t('F'))
		return u - uint8_t('A') + 0xA;
	if (u >= uint8_t('a') && u <= uint8_t('f'))
		return u - uint8_t('a') + 0xa;
	return 0xFF;
}
static inline uint8_t unhex_byte(char a, char b) { return (unhex_char(a) << 4) + unhex_char(b); }
static inline char hex_char(uint8_t v)
{
	if (v < 0xA)
		return char(uint8_t('0') + v);
	return char(uint8_t('A') + v - 0xA);
}
static inline std::pair<char, char> hex_byte(uint8_t v) { return { hex_char(v >> 4), hex_char(v & 0xF) }; }

static fnv::hash hash_subserial(const char* serial, size_t len)
{
	auto h = fnv::hash_init();
	for (auto i = 0u; i < len; ++i)
		if (is_good_char(serial[i]))
			h = fnv::hash_byte(h, serial[i]);
	return h;
}

void randomize_subserial(char* serial, size_t len)
{
	const auto seed = hash_subserial(serial, len) ^ g_startup_time;
	auto engine = std::mt19937_64{ seed };
	const auto distribution = std::uniform_int_distribution<unsigned>('A', 'Z');

	KdPrint(("Randomizing subserial: seed: %016llX len: %d\n old: ", seed, len));
	for (auto i = 0u; i < len; ++i)
		KdPrint(("%02hhX ", uint8_t(serial[i])));
	KdPrint(("\n new: "));

	for (auto i = 0u; i < len; ++i)
		if(is_good_char(serial[i]))
			serial[i] = char(distribution(engine));

	for (auto i = 0u; i < len; ++i)
		KdPrint(("%02hhX ", uint8_t(serial[i])));
	KdPrint(("\n"));
}

void spoof_serial(char* serial, bool is_smart)
{
	// must be 20 or less
	size_t len;
	char buf[21];
	bool is_serial_hex;
	if(is_smart)
	{
		is_serial_hex = false;
		len = 20;
		memcpy(buf, serial, 20);
	}
	else
	{
		is_serial_hex = true;
		for (len = 0; serial[len]; ++len)
			if (!is_hex(serial[len]))
				is_serial_hex = false;

		if(is_serial_hex)
		{
			len /= 2;
			len = std::min<size_t>(len, 20);
			for (auto i = 0u; i < len; ++i)
				buf[i] = unhex_byte(serial[i * 2], serial[i * 2 + 1]);
		}
		else
		{
			memcpy(buf, serial, len);
		}
	}

	buf[len] = 0;
	char split[2][11];
	memset(split, 0, sizeof(split));

	for (auto i = 0u; i < len; ++i)
		split[i % 2][i / 2] = buf[i];

	randomize_subserial(split[0], (len + 1) / 2);
	randomize_subserial(split[1], len / 2);

	for (auto i = 0u; i < len; ++i)
		buf[i] = split[i % 2][i / 2];
	buf[len] = 0;

	if(is_smart)
	{
		memcpy(serial, buf, 20);
	}
	else
	{
		if (is_serial_hex)
		{
			for (auto i = 0u; i < len; ++i)
				std::tie(serial[i * 2], serial[i * 2 + 1]) = hex_byte(buf[i]);
			serial[len * 2] = 0;
		}
		else
		{
			memcpy(serial, buf, len + 1);
		}
	}
}