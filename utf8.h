#pragma once
#ifndef NATIVA_ENCODING_UTF8
#define NATIVA_ENCODING_UTF8

#include <type_traits>
#include <utility>
#include <cstdint>

namespace nativa
{
	namespace encoding
	{
		struct utf8
		{
			class iterator
			{
			private:
				const char* m_base;

			public:
				iterator& operator++();

				iterator operator++(int);

				char32_t operator*();

				bool operator!=(const utf8::iterator& another);

				iterator(const char* base);
			};

			class accessor
			{
			public:
				iterator begin();

				iterator end();

				accessor(const char* begin, const char* end);

			private:
				iterator m_begin;

				iterator m_end;
			};
		};

		inline utf8::iterator& utf8::iterator::operator++()
		{
			char c = *m_base;

			m_base += 1;

			if (c & 0b10000000) // multibyte
			{
				m_base += 1; // at least two bytes, no need to check the second bit

				if (c & 0b00100000)
				{
					m_base += 1;

					if (c & 0b00010000)
					{
						m_base += 1;
					}
				}
			}

			return *this;
		}
		
		inline utf8::iterator utf8::iterator::operator++(int)
		{
			utf8::iterator temp = *this;
			this->operator++();
			return temp;
		}

		inline char32_t utf8::iterator::operator*()
		{
			char c = *m_base;
			char32_t res = 0;
			const char* it = m_base;

			res += (*it) & 0b1111111;
			++it;

			if (c & 0b10000000)
			{
				res &= 0b11111;
				res <<= 6;
				res |= (*it) & 0b111111;
				++it;

				if (c & 0b00100000)
				{
					res &= 0b1111111111;
					res <<= 6;
					res |= (*it) & 0b111111;
					++it;

					if (c & 0b00010000)
					{
						res &= 0b1111111111111111;
						res <<= 6;
						res |= (*it) & 0b111111;
						++it;

					}
				}
			}

			return res;
		}

		inline bool utf8::iterator::operator!=(const utf8::iterator& another)
		{
			return m_base != another.m_base;
		}

		inline utf8::iterator::iterator(const char* base)
			: m_base(base)
		{
		}
		
		inline utf8::iterator utf8::accessor::begin()
		{
			return m_begin;
		}

		inline utf8::iterator utf8::accessor::end()
		{
			return m_end;
		}

		inline utf8::accessor::accessor(const char* begin, const char* end)
			: m_begin(utf8::iterator(begin)), m_end(utf8::iterator(end))
		{
		}
	}
}

#endif