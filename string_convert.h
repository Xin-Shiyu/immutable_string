#pragma once
#ifndef NATIVA_STRING_CONVERT
#define NATIVA_STRING_CONVERT

#include <cstdint>
#include <cassert>
#include "string.h"
#include "string_builder.h"

namespace nativa
{
	nativa::string to_string(const string& value)
	{
		return value;
	}

	const char* const number_map = "0123456789ABCDEF";

	template <typename T>
	nativa::string unsigned_integral_to_string_impl(T value)
	{
		string_builder builder;
		bool neg = false;

		if (value == 0) return "0";

		while (value > 0)
		{
			builder.append(number_map[value % 10]);
			value /= 10;
		}

		std::reverse(builder.begin(), builder.end());

		return builder.to_string();
	}

	template <typename T>
	nativa::string signed_integral_to_string_impl(T value)
	{
		string_builder builder;

		if (value == 0) return "0";

		if (value > 0)
		{
			while (value > 0)
			{
				builder.append(number_map[value % 10]);
				value /= 10;
			}
		}
		else
		{
			while (value < 0)
			{
				builder.append(number_map[-(value % 10)]);
				value /= 10;
			}
			builder.append('-');
		}

		std::reverse(builder.begin(), builder.end());

		return builder.to_string();
	}

	nativa::string to_string(uint64_t value)
	{
		return unsigned_integral_to_string_impl(value);
	}

	nativa::string to_string(int64_t value)
	{
		return signed_integral_to_string_impl(value);
	}

	nativa::string to_string(uint32_t value)
	{
		return unsigned_integral_to_string_impl(value);
	}

	nativa::string to_string(int32_t value)
	{
		return signed_integral_to_string_impl(value);
	}

	nativa::string to_string(bool value)
	{
		if (value)
		{
			return "true";
		}
		else
		{
			return "false";
		}
	}

	template <typename T>
	nativa::string to_string(T value, const nativa::string_view& style)
	{
		return to_string(value); // fallback impl
	}

	template <typename T>
	T parse(const nativa::string_view& str);

	template <>
	size_t parse(const nativa::string_view& str)
	{
		size_t res = 0;
		for (auto c : str)
		{
			assert('0' <= c && c <= '9');
			res *= 10;
			res += c - '0';
		}
		return res;
	}
}

#endif