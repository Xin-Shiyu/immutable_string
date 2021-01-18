#include "string_view.h"
#include "string.h"
#include <cassert>

namespace nativa
{
	int nativa::string_view::compare_to(const string_view& another) const
	{
		auto lp = this->m_begin;
		auto rp = another.m_begin;
		for (;;)
		{
			if (lp == this->m_end)
			{
				if (rp == this->m_end)
				{
					return 0;
				}

				return -1;
			}

			if (rp == this->m_end)
			{
				return 1;
			}

			char l = *lp;
			char r = *rp;
			if (l < r) return -1;
			else if (l > r) return 1;
			else if (l == '\0') return 0;

			++l;
			++r;
		}
	}

	ptrdiff_t string_view::last_index_of(char target) const
	{
		return find_last(target, m_end) - m_begin;
	}

	ptrdiff_t string_view::last_index_of(char target, size_t before) const
	{
		assert(before < size());

		return find_last(target, m_begin + before) - m_begin;
	}

	ptrdiff_t string_view::index_of(const char target, size_t from) const
	{
		assert(from < size());

		auto res = find(target, m_begin + from);
		if (res != m_end)
		{
			return res - m_begin;
		}

		return -1;
	}

	ptrdiff_t string_view::index_of(const string_view& target, size_t from) const
	{
		assert(from < size());

		auto res = find(target, m_begin + from);
		if (res != m_end)
		{
			return res - m_begin;
		}

		return -1;
	}

	nativa::string string_view::clone() const
	{
		size_t size = this->size();
		if (size == 0) return string("");
		char* mutable_raw;
		auto res = string_internals::alloc(size, mutable_raw);
		this->copy_to(mutable_raw);
		return std::move(res);
	}

	nativa::string_view string_view::slice(size_t begin, size_t length) const
	{
		assert(m_begin + begin + length <= m_end);
		return nativa::string_view(m_begin + begin, m_begin + length);
	}

	void string_view::copy_to(char* buffer) const
	{
		auto it = m_begin;
		auto dest = buffer;
		while (it < m_end)
		{
			*dest = *it;
			++it;
			++dest;
		}
	}

	const char* string_view::begin() const
	{
		return m_begin;
	}

	const char* string_view::end() const
	{
		return m_end;
	}

	size_t string_view::size() const
	{
		return m_end - m_begin;
	}

	string_view::string_view(const char* begin, const char* end) noexcept
	{
		m_begin = begin;
		m_end = end;
	}

	const char* string_view::find_last(char target, const char* before) const
	{
		while (before >= m_begin && target != *before) --before;
		return before;
	}

	const char* string_view::find(const char target, const char* from) const
	{
		while (from != m_end && target != *from) ++from;
		return from;
	}

	const char* string_view::find(const string_view& target, const char* from) const
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
}

nativa::string_view operator""_ns(const char* c_str, size_t len)
{
	return nativa::string_view(c_str, c_str + len);
}