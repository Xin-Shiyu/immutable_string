#include <cassert>
#include <algorithm>
#include "string_view.h"
#include "string.h"  // implementation of string_view::clone relies on this

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
				if (rp == another.m_end)
				{
					return 0;
				}

				return -1;
			}

			if (rp == this->m_end)
			{
				return 1;
			}

			const char l = *lp;
			const char r = *rp;
			if (l < r) return -1;
			else if (l > r) return 1;

			++lp;
			++rp;
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

	ptrdiff_t string_view::index_of(const char target, size_t since) const
	{
		auto res = find(target, m_begin + since);
		if (res == m_end) return -1;
		return res - m_begin;
	}

	ptrdiff_t string_view::index_of(const string_view& target, size_t since) const
	{
		auto res = find(target, m_begin + since);
		if (res == m_end) return -1;
		return res - m_begin;
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
		return nativa::string_view(m_begin + begin, m_begin + begin + length);
	}

	void string_view::copy_to(char* buffer) const
	{
		std::copy(m_begin, m_end, buffer);
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

	bool string_view::operator<(const string_view& right) const
	{
		return this->compare_to(right) < 0;
	}

	bool string_view::operator<=(const string_view& right) const
	{
		return *this < right || *this == right;
	}

	bool string_view::operator==(const string_view& right) const
	{
		return this->compare_to(right) == 0;
	}

	bool string_view::operator>=(const string_view& right) const
	{
		return *this > right || *this == right;
	}

	bool string_view::operator>(const string_view& right) const
	{
		return this->compare_to(right) > 0;
	}

	nativa::string_view string_view::operator[](const std::pair<size_t, size_t>& range) const
	{
		if (range.first == range.second) return "";
		assert(range.first < this->size());
		assert(range.second <= this->size());
		return string_view(m_begin + range.first, m_begin + range.second);
	}

	bool string_view::is_empty() const
	{
		return size() == 0;
	}

	bool string_view::starts_with(const string_view& pattern) const
	{
		size_t p_size = pattern.size();
		if (p_size > this->size()) return false;
		return this->slice(0, p_size) == pattern;
	}

	bool string_view::ends_with(const string_view& pattern) const
	{
		size_t m_size = this->size();
		size_t p_size = pattern.size();
		if (p_size > m_size) return false;
		return this->slice(m_size - p_size, p_size) == pattern;
	}

	bool string_view::contains(const string_view& pattern) const
	{
		return find(pattern, m_begin) != m_end;
	}

	string_view::string_view(const char* begin, const char* end) noexcept
		: m_begin(begin), m_end(end)
	{
	}

	const char* string_view::find_last(char target, const char* before) const
	{
		assert(before != nullptr);

		while (before >= m_begin && target != *before) --before;
		return before;
	}

	const char* string_view::find(const char target, const char* since) const
	{
		assert(since != nullptr);

		while (since != m_end && target != *since) ++since;
		return since;
	}

	const char* string_view::find(const string_view& target, const char* since) const
	{
		assert(since != nullptr);

		// TO-DO: optimize
		auto end = m_end - target.size();
		for (auto m_it = since; m_it <= end; ++m_it)
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