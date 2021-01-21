#pragma once
#ifndef NATIVA_FORMAT
#define NATIVA_FORMAT

#include <deque>
#include <unordered_map>
#include "string.h"
#include "string_convert.h"

namespace nativa
{
	class format_internal;

	template <typename... Args>
	nativa::string format(const string_view& fmt, Args... args)
	{
		format_internal context;

		context.parse(fmt);

		context.fill(0, args...);

		return string::concat(context.res);
	}

	class format_internal
	{
		template <typename... Args>
		friend nativa::string format(const string_view& fmt, Args... args);

		struct format_guide
		{
			nativa::string_view* target;
			nativa::string_view style;

			format_guide(nativa::string_view* target, nativa::string_view style)
				: target(target), style(style)
			{
			}
		};

		std::deque<nativa::string> temp; // to own the temporary strings
		std::deque<nativa::string_view> res;
		std::deque<std::deque<format_guide>> guides;

		void parse(const string_view& fmt)
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
						res.push_back(fmt);
					}
					else if (end != size) // this is the last normal part
					{
						res.push_back(fmt[{end, size}]);
					}
					break;
				}

				if (end != begin) // create a normal part
				{
					res.push_back(fmt[{end, begin}]);
				}

				end = fmt.index_of('}', begin + 1);

				assert(end != -1); // otherwise this is a malformed format string

				end += 1;

				auto slice = fmt[{begin, end}];
				res.push_back(slice);
				
				auto style = ""_ns;
				ptrdiff_t colon = slice.index_of(':');

				if (colon != -1)
				{
					style = slice[{colon + 1, slice.size() - 1}];
					slice = slice[{0, colon}];
				}

				size_t target_index = nativa::parse<size_t>(slice[{1, slice.size()}]);
				if (target_index >= guides.size())
				{
					guides.resize(target_index + 1);
				}
				auto shit = res.back();
				
				guides[target_index].emplace_back(&res.back(), style);
			}
		}

		template <typename T>
		void fill_one(size_t index, const T& arg)
		{
			std::unordered_map<nativa::string_view, nativa::string_view>
				cache;
			for (format_guide guide : guides[index])
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
					*guide.target = str.view();
				}
				else
				{
					*guide.target = it->second;
				}
			}
		}

		template <>
		void fill_one(size_t index, const string_view& arg)
		{
			for (format_guide guide : guides[index])
			{
				*guide.target = arg;
			}
		}

		void fill_one(size_t index, const char* arg)
		{
			auto end = arg;
			while (*end != '\0') ++end;
			for (format_guide guide : guides[index])
			{
				*guide.target = nativa::string_view(arg, end);
			}
		}

		template <typename First, typename...Args>
		void fill(size_t index, const First& first, Args... args)
		{
			fill_one(index, first);
			fill(index + 1, args...);
		}

		void fill(size_t)
		{	
			// empty impl for stopping the recursion
		}
	};
}

#endif