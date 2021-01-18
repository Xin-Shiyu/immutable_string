#include <cassert>
#include "string.h" // implementation of string_view::clone relies on this

#pragma region String Utils

namespace nativa
{
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
		return this->slice(begin, length).clone();
	}

	const char* string::c_str() const
	{
		return this->m_begin;
	}

	ref_counter_t string::ref_count() const
	{
		if (m_counter == nullptr) return 0;
		return *m_counter;
	}

	string::string(ref_counter_t* counter, const char* begin, const char* end) noexcept
		: m_counter(counter), string_view(begin, end)
	{
	}

	string::string(const string& str) noexcept
		: m_counter(str.m_counter), string_view(str.m_begin, str.m_end)
	{
		if (m_counter)
		{
			*m_counter += 1;
		}
	}

	string::string(string&& str) noexcept
		: m_counter(str.m_counter), string_view(str.m_begin, str.m_end)
	{
		str.m_counter = nullptr;
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
}