#pragma once
#ifndef NATIVA_STRING_CONVERT
#define NATIVA_STRING_CONVERT

#include <cstdint>
#include <cassert>
#include "string.h"
#include "string_builder.h"

namespace nativa
{
	template <typename T>
	T parse_unsigned_integral(const nativa::string_view& str)
	{
		T res = 0;
		for (auto c : str)
		{
			assert('0' <= c && c <= '9');
			res *= 10;
			res += static_cast<size_t>(c) - '0';
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

	nativa::string to_string(const string& value)
	{
		return value;
	}

	namespace to_string_impl
	{
		const char* const number_map = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

		template <typename T, size_t BuilderLen>
		void unsigned_integral_out_reversed(T value, unsigned base, fixed_string_builder<BuilderLen>& builder)
		{
			while (value != 0)
			{
				builder.append(number_map[value % base]);
				value /= base;
			}
		}

		template <typename T, size_t BuilderLen>
		void signed_integral_out_reversed(T value, unsigned base, fixed_string_builder<BuilderLen>& builder)
		{
			using Unsigned = typename std::make_unsigned<T>::type;

			if (value > 0)
			{
				unsigned_integral_out_reversed(static_cast<Unsigned>(value), base, builder);
			}
			else
			{
				unsigned_integral_out_reversed(static_cast<Unsigned>(-value), base, builder);
				builder.append('-');
			}
		}

		template <typename T>
		nativa::string unsigned_integral(T value, unsigned base)
		{
			assert(2 <= base && base <= 36);
			if (value == 0) return "0";

			fixed_string_builder<128> builder;
			unsigned_integral_out_reversed(value, base, builder);
			std::reverse(builder.begin(), builder.end());

			return builder.to_string();
		}

		template <typename T>
		nativa::string signed_integral(T value, unsigned base)
		{
			assert(2 <= base && base <= 36);
			if (value == 0) return "0";

			fixed_string_builder<128> builder;
			signed_integral_out_reversed(value, base, builder);

			std::reverse(builder.begin(), builder.end());

			return builder.to_string();
		}

		template <typename T>
		nativa::string float_point(T value, int base, int precision)
		{
			// TO-DO: rewrite
			assert(2 <= base && base <= 36);

			ptrdiff_t integral_part = static_cast<ptrdiff_t>(value);

			fixed_string_builder<128> builder;
			signed_integral_out_reversed(integral_part, base, builder);

			std::reverse(builder.begin(), builder.end());

			builder.append('.');
			if (value < 0) value = -value;
			const int max_precision = 100 / base; // I wrote this for no reason
			if (precision > max_precision) precision = max_precision;
			int zero_count = 0;
			for (int i = 0; i < precision; ++i)
			{
				value *= base;
				size_t digit = static_cast<size_t>(value) % base;
				if (digit == 0) zero_count += 1;
				else zero_count = 0;
				assert(0 <= digit && digit <= base);
				char digit_char = static_cast<char>(number_map[digit]);
				builder.append(digit_char);
			}
			builder.resize(builder.size() - zero_count);

			return builder.to_string();
		}
	}

	nativa::string to_string(uint64_t value)
	{
		return to_string_impl::unsigned_integral(value, 10);
	}

	nativa::string to_string(uint64_t value, const nativa::string_view& style)
	{
		return to_string_impl::unsigned_integral(value, nativa::parse_unsigned_integral<unsigned>(style));
	}

	nativa::string to_string(int64_t value)
	{
		return to_string_impl::signed_integral(value, 10);
	}

	nativa::string to_string(int64_t value, const nativa::string_view& style)
	{
		return to_string_impl::signed_integral(value, nativa::parse_unsigned_integral<unsigned>(style));
	}

	nativa::string to_string(uint32_t value)
	{
		return to_string_impl::unsigned_integral(value, 10);
	}

	nativa::string to_string(uint32_t value, const nativa::string_view& style)
	{
		return to_string_impl::unsigned_integral(value, nativa::parse_unsigned_integral<unsigned>(style));
	}

	nativa::string to_string(int32_t value)
	{
		return to_string_impl::signed_integral(value, 10);
	}

	nativa::string to_string(int32_t value, const nativa::string_view& style)
	{
		return to_string_impl::signed_integral(value, nativa::parse_unsigned_integral<unsigned>(style));
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

	nativa::string to_string(double value)
	{
		return to_string_impl::float_point(value, 10, 10);
	}

	nativa::string to_string(float value)
	{
		return to_string_impl::float_point(value, 10, 6);
	}

	nativa::string to_string(double value, const nativa::string_view& style)
	{
		auto point = style.index_of('.');
		auto s_size = style.size();
		if (point == -1) point = s_size;
		unsigned base = (point == 0)
			? 10
			: parse_unsigned_integral<unsigned>(style[{0, point}]);
		unsigned precision = (point == s_size)
			? 10
			: parse_unsigned_integral<unsigned>(style[{point + 1, s_size}]);
		return to_string_impl::float_point(value, base, precision);
	}

	nativa::string to_string(float value, const nativa::string_view& style)
	{
		auto point = style.index_of('.');
		auto s_size = style.size();
		if (point == -1) point = s_size;
		unsigned base = (point == 0)
			? 10
			: parse_unsigned_integral<unsigned>(style[{0, point}]);
		unsigned precision = (point == s_size)
			? 6
			: parse_unsigned_integral<unsigned>(style[{point + 1, s_size}]);
		return to_string_impl::float_point(value, base, precision);
	}

	template <typename T>
	nativa::string to_string(const T& value, const nativa::string_view& style)
	{
		return to_string(value); // fallback impl
	}
}

#endif