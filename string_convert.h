#pragma once
#ifndef NATIVA_STRING_CONVERT
#define NATIVA_STRING_CONVERT

#include <cstdint>
#include <cassert>
#include "string.h"
#include "string_builder.h"

namespace nativa
{
#pragma region Parse

	template <typename T>
	T parse_unsigned_integral(const nativa::string_view& str)
	{
		T res = 0;
		for (auto c : str)
		{
			assert('0' <= c && c <= '9');
			res *= 10;
			res += c - '0';
		}
		return res;
	}

	template <typename T>
	T parse_signed_integral(const nativa::string_view& str)
	{
		using Unsigned = typename std::make_unsigned<T>::type;
		return str.begin()[0] == '-'
			? -static_cast<T>(parse_unsigned_integral<Unsigned>(str[{1, str.size()}]))
			: parse_unsigned_integral<Unsigned>(str);
	}
#pragma endregion

	nativa::string to_string(const string& value)
	{
		return value;
	}

	const char* const number_map = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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
	nativa::string signed_integral_to_string_impl(T value, int base)
	{
		assert(base <= 36);

		string_builder builder;

		if (value == 0) return "0";

		if (value > 0)
		{
			while (value > 0)
			{
				builder.append(number_map[value % base]);
				value /= base;
			}
		}
		else
		{
			while (value < 0)
			{
				builder.append(number_map[-(value % base)]);
				value /= base;
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
		return signed_integral_to_string_impl(value, 10);
	}

	nativa::string to_string(int64_t value, const nativa::string_view& style)
	{
		return signed_integral_to_string_impl(value, nativa::parse_signed_integral<int>(style));
	}

	nativa::string to_string(uint32_t value)
	{
		return unsigned_integral_to_string_impl(value);
	}

	nativa::string to_string(int32_t value)
	{
		return signed_integral_to_string_impl(value, 10);
	}

	nativa::string to_string(int32_t value, const nativa::string_view& style)
	{
		return signed_integral_to_string_impl(value, nativa::parse_signed_integral<int>(style));
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
	nativa::string to_string(const T& value, const nativa::string_view& style)
	{
		return to_string(value); // fallback impl
	}
}

#endif