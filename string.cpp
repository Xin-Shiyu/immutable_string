#include <cassert>
#include "string.h"

#pragma region String Utils

namespace nativa
{
	void string_util::c_str_copy(char* dest, const char* src_begin, const char* src_end)
	{
		while (src_begin < src_end)
		{
			*dest = *src_begin;
			++src_begin;
			++dest;
		}
	}

#pragma endregion

	string string_internals::alloc(size_t length, char*& mutable_raw)
	{
		// The memory looks like:
		// [ counter ] [ string ] [ \0 ]
		assert(length > 0);

		size_t buffer_len = alignof(ref_counter_t) + length + 1;
		char* buffer = new char[buffer_len];

		ref_counter_t* counter = reinterpret_cast<ref_counter_t*>(buffer);
		*counter = 1;

		mutable_raw = buffer + alignof(ref_counter_t);

		char* end = mutable_raw + length;
		*end = '\0';

		return string(counter, mutable_raw, end);
	}

	void string_internals::free(string&& disposed)
	{
		assert(disposed.m_counter != nullptr);

		delete disposed.m_counter;
	}

	string string::substring(size_t begin, size_t length) const
	{
		const char* const p_begin = m_begin + begin;
		const char* const p_end = p_begin + length;
		return substring(p_begin, p_end);
	}

	ptrdiff_t string::last_index_of(char target) const
	{
		return find_last(target, m_end) - m_begin;
	}

	ptrdiff_t string::last_index_of(char target, size_t before) const
	{
		assert(before < size());

		return find_last(target, m_begin + before) - m_begin;
	}

	ptrdiff_t string::index_of(const char target, size_t from) const
	{
		assert(from < size());

		auto res = find(target, m_begin + from);
		if (res != m_end)
		{
			return res - m_begin;
		}
		
		return -1;
	}

	ptrdiff_t string::index_of(const string& target, size_t from) const
	{
		assert(from < size());

		auto res = find(target, m_begin + from);
		if (res != m_end)
		{
			return res - m_begin;
		}

		return -1;
	}

	void string::copy_to(char* buffer) const
	{
		string_util::c_str_copy(buffer, m_begin, m_end);
	}

	const char* string::c_str() const
	{
		return m_begin;
	}

	const char* string::begin() const
	{
		return m_begin;
	}

	const char* string::end() const
	{
		return m_end;
	}

	size_t string::size() const
	{
		return m_end - m_begin;
	}

	string::string(ref_counter_t* counter, const char* begin, const char* end) noexcept
		: m_counter(counter), m_begin(begin), m_end(end)
	{
	}

	string string::substring(const char* begin, const char* end) const
	{
		char* mutable_raw;
		assert(end <= m_end);
		assert(begin <= end);

		if (begin == end) return string("");
		string res = string_internals::alloc(end - begin, mutable_raw);
		string_util::c_str_copy(mutable_raw, begin, end);
		return std::move(res);
	}

	const char* string::find_last(char target, const char* before) const
	{
		while (before >= m_begin && target != *before) --before;
		return before;
	}

	const char* string::find(const char target, const char* from) const
	{
		while (from != m_end && target != *from) ++from;
		return from;
	}

	const char* string::find(const string& target, const char* from) const
	{
		// TO-DO: optimize
		auto end = m_end - target.size();
		for (auto m_it = from; m_it <= end; ++m_it)
		{
			auto w_it = m_it;
			auto t_it = target.m_begin;
			while (*w_it == *t_it)
			{
				++w_it;
				++t_it;
				if (t_it == target.m_end) return m_it;
			}
		}
		return m_end;
	}

	string::string(const string& str) noexcept
		: m_counter(str.m_counter), m_begin(str.m_begin), m_end(str.m_end)
	{
		if (m_counter)
		{
			*m_counter += 1;
		}
	}

	string::string(string&& str) noexcept
		: m_counter(str.m_counter), m_begin(str.m_begin), m_end(str.m_end)
	{
		str.m_counter = nullptr;
		str.m_begin = nullptr;
		str.m_end = nullptr;
	}

	string::~string() noexcept
	{
		if (m_counter)
		{
			*m_counter -= 1;
			if (*m_counter == 0)
			{
				// move is used to supress ref count change
				string_internals::free(std::move(*this));
			}
		}
	}

	int string::compare(string left, string right)
	{
		auto lp = left.m_begin;
		auto rp = right.m_begin;
		// No need for bound check
		for (;;)
		{
			char l = *lp;
			char r = *rp;
			if (l < r) return -1;
			else if (l > r) return 1;
			else if (l == '\0') return 0;

			++l;
			++r;
		}
	}
}

nativa::string operator""_ns(const char* c_str, size_t)
{
	return nativa::string(c_str);
}