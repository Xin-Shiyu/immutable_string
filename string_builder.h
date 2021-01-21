#pragma once
#ifndef NATIVA_STRING_BUILDER
#define NATIVA_STRING_BUILDER

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
		fixed_string_builder()
		{
			m_end = m_buffer;
		}

		fixed_string_builder& append(const string_view& str)
		{
			assert(str.size() + this->size() <= Capacity);

			str.copy_to(grow(str.size()));

			return *this;
		}

		fixed_string_builder& append(char c)
		{
			*grow(1) = c;

			return *this;
		}

		size_t size() const
		{
			return m_end - m_buffer;
		}

		void resize(size_t size)
		{
			m_end = m_buffer + size;
		}

		char* begin()
		{
			return m_buffer;
		}

		char* end()
		{
			return m_end;
		}

		nativa::string to_string()
		{
			char* buffer;
			auto res = string_internals::alloc(this->size(), buffer);
			string_view(m_buffer, m_end).copy_to(buffer);
			return std::move(res);
		}

	private:
		char m_buffer[Capacity];

		char* m_end;

		char* grow(size_t size)
		{
			assert(this->size() + size <= Capacity);

			auto old = m_end;

			m_end += size;
			
			return old;
		}
	};
}

#endif