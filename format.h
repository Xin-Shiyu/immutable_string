#pragma once
#ifndef NATIVA_FORMAT
#define NATIVA_FORMAT

#include <cstddef>
#include <vector>
#include <unordered_map>
#include "string.h"
#include "string_convert.h"

namespace nativa
{
	class format_internal
	{
		template <typename... Args>
		friend nativa::string format(const string_view& fmt, Args... args);

		struct format_guide
		{
			size_t target;
			nativa::string_view style;

			format_guide(size_t target, nativa::string_view style);
		};

		template <typename First, typename... Args>
		static size_t count_args(First, Args... args);

		static size_t count_args();

		std::vector<nativa::string> temp; // to own the temporary strings
		std::vector<nativa::string_view> res;
		std::vector<std::vector<format_guide>> guides;

		void parse(const string_view& fmt);

		template <typename T>
		void fill_one(size_t index, const T& arg);

		void fill_one(size_t index, const string_view& arg);

		void fill_one(size_t index, const char* arg);

		template <typename First, typename...Args>
		void fill(size_t index, const First& first, Args... args);

		void fill(size_t);
	};

	template <typename... Args>
	nativa::string format(const string_view& fmt, Args... args)
	{
		format_internal context;

		context.guides.resize(format_internal::count_args(args...));

		context.parse(fmt);

		context.fill(0, args...);

		return string::concat(context.res);
	}

	template <typename First, typename ...Args>
	inline size_t format_internal::count_args(First, Args ...args)
	{
		return count_args(args...) + 1;
	}

	inline size_t format_internal::count_args()
	{
		return 0;
	}

	inline format_internal::format_guide::format_guide(size_t target, nativa::string_view style)
		: target(target), style(style)
	{
	}

	inline void format_internal::parse(const string_view& fmt)
	{
		// assume format parts and "normal" parts to be one after one
		// also assume that normal parts come first
		// if a "normal part" has no length we ignore it
		size_t size = fmt.size();
		ptrdiff_t begin = 0;
		ptrdiff_t end = 0;
		for (;;)
		{
			begin = fmt.index_of('{', end);

			if (begin == -1)
			{
				if (end == 0) // the string has no format requirement
				{
					res.emplace_back(fmt);
				}
				else if (end != size) // this is the last normal part
				{
					res.emplace_back(fmt[{end, size}]);
				}
				break;
			}

			if (end != begin) // create a normal part
			{
				res.emplace_back(fmt[{end, begin}]);
			}

			end = fmt.index_of('}', begin + 1);

			assert(end != -1); // otherwise this is a malformed format string

			end += 1;

			auto slice = fmt[{begin, end}];
			res.emplace_back(slice);

			auto style = ""_ns;
			ptrdiff_t colon = slice.index_of(':');

			if (colon != -1)
			{
				colon += 1;
				style = slice[{colon, slice.size() - 1}];
				slice = slice[{0, colon}];
			}

			size_t target_index = nativa::parse_unsigned_integral<size_t>(slice[{1, slice.size() - 1}]);
			if (target_index < guides.size())
			{
				guides[target_index].emplace_back(res.size() - 1, style);
			}
		}
	}

	template <typename T>
	inline void format_internal::fill_one(size_t index, const T& arg)
	{
		std::unordered_map<nativa::string_view, nativa::string_view>
			cache;
		for (const format_guide& guide : guides[index])
		{
			auto it = cache.find(guide.style);
			if (it == cache.end())
			{
				// no style, call the default to_string
				// otherwise call the fancy one
				nativa::string str = (guide.style.size() == 0)
					? nativa::to_string(arg)
					: nativa::to_string(arg, guide.style);
				temp.emplace_back(std::move(str)); // to supress ref count change
				cache[guide.style] = str;
				res[guide.target] = str.view();
			}
			else
			{
				res[guide.target] = it->second;
			}
		}
	}

	template <>
	inline void format_internal::fill_one(size_t index, const string_view& arg)
	{
		for (const format_guide& guide : guides[index])
		{
			res[guide.target] = arg;
		}
	}

	inline void format_internal::fill_one(size_t index, const char* arg)
	{
		auto end = arg;
		while (*end != '\0') ++end;
		for (const format_guide& guide : guides[index])
		{
			res[guide.target] = nativa::string_view(arg, end);
		}
	}

	template <typename First, typename...Args>
	inline void format_internal::fill(size_t index, const First& first, Args... args)
	{
		fill_one(index, first);
		fill(index + 1, args...);
	}

	inline void format_internal::fill(size_t)
	{
		// empty impl for stopping the recursion
	}
}

#endif