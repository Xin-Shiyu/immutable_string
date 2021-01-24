#pragma once
#ifndef NATIVA_STRING_BUILDER
#define NATIVA_STRING_BUILDER

#include <cstddef>
#include <vector>
#include "string.h"

namespace nativa
{
	class string_builder
	{
	public:
		string_builder();

		string_builder& append(const string_view& str);

		string_builder& append(char c);

		size_t size() const;

		void shrink_to_fit();

		void resize(size_t size);
		
		std::vector<char>::iterator begin();

		std::vector<char>::iterator end();

		nativa::string to_string();

	private:
		std::vector<char> m_buffer;

		char* grow(size_t size);
	};

	template <size_t Capacity>
	class fixed_string_builder
	{
	public:
		fixed_string_builder();

		fixed_string_builder& append(const string_view& str);

		fixed_string_builder& append(char c);

		size_t size() const;

		void resize(size_t size);

		char* begin();

		char* end();

		nativa::string to_string() const;

	private:
		char m_buffer[Capacity];

		char* m_end;

		char* grow(size_t size);
	};

#pragma region Template Function Impl
	template <size_t Capacity>
	inline fixed_string_builder<Capacity>::fixed_string_builder()
	{
		m_end = m_buffer;
	}

	template <size_t Capacity>
	inline fixed_string_builder<Capacity>& fixed_string_builder<Capacity>::append(const string_view& str)
	{
		assert(str.size() + this->size() <= Capacity);

		str.copy_to(grow(str.size()));

		return *this;
	}

	template <size_t Capacity>
	inline fixed_string_builder<Capacity>& fixed_string_builder<Capacity>::append(char c)
	{
		*grow(1) = c;

		return *this;
	}

	template <size_t Capacity>
	inline size_t fixed_string_builder<Capacity>::size() const
	{
		return m_end - m_buffer;
	}

	template <size_t Capacity>
	inline void fixed_string_builder<Capacity>::resize(size_t size)
	{
		m_end = m_buffer + size;
	}

	template <size_t Capacity>
	inline char* fixed_string_builder<Capacity>::begin()
	{
		return m_buffer;
	}

	template <size_t Capacity>
	inline char* fixed_string_builder<Capacity>::end()
	{
		return m_end;
	}

	template <size_t Capacity>
	inline nativa::string fixed_string_builder<Capacity>::to_string() const
	{
		char* buffer;
		auto res = string_internals::alloc(this->size(), buffer);
		string_view(m_buffer, m_end).copy_to(buffer);
		return std::move(res);
	}

	template <size_t Capacity>
	inline char* fixed_string_builder<Capacity>::grow(size_t size)
	{
		assert(this->size() + size <= Capacity);

		auto old = m_end;

		m_end += size;
		
		return old;
	}
#pragma endregion

}

#endif