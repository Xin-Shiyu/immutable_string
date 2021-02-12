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
			utf8() = delete;

			class access_iterator
			{
			private:
				const char* m_base;

			public:
				access_iterator& operator++();

				access_iterator operator++(int);

				access_iterator& operator--();

				access_iterator operator--(int);

				access_iterator operator+(ptrdiff_t diff);

				access_iterator operator-(ptrdiff_t diff);

				char32_t operator*();

				bool operator!=(const utf8::access_iterator& another);

				bool operator==(const utf8::access_iterator& another);

				bool operator>=(const utf8::access_iterator& another);

				const char* base() const;

				access_iterator(const char* base);
			};

			class accessor
			{
			public:
				access_iterator begin();

				access_iterator end();

				accessor(const char* begin, const char* end);

			private:
				access_iterator m_begin;

				access_iterator m_end;
			};
		
			template <typename OutputIt>
			static void encode(char32_t c, OutputIt out);
		};

		inline utf8::access_iterator& utf8::access_iterator::operator++()
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
		
		inline utf8::access_iterator utf8::access_iterator::operator++(int)
		{
			utf8::access_iterator temp = *this;
			this->operator++();
			return temp;
		}

		inline utf8::access_iterator& utf8::access_iterator::operator--()
		{
			--m_base;
			while ((static_cast<uint8_t>(*m_base) >> 6) == 0b10) 
			{	// a char never starts with 10
				--m_base;
			}
			return *this;
		}

		inline utf8::access_iterator utf8::access_iterator::operator--(int)
		{
			utf8::access_iterator temp = *this;
			this->operator--();
			return temp;
		}

		inline utf8::access_iterator utf8::access_iterator::operator+(ptrdiff_t diff)
		{
			if (diff < 0) this->operator-(-diff);
			auto res = *this;
			while (diff != 0)
			{
				++res;
				--diff;
			}
			return res;
		}

		inline utf8::access_iterator utf8::access_iterator::operator-(ptrdiff_t diff)
		{
			if (diff < 0) this->operator+(-diff);
			auto res = *this;
			while (diff != 0)
			{
				--res;
				--diff;
			}
			return res;
		}

		inline char32_t utf8::access_iterator::operator*()
		{
			char c = *m_base;
			uint32_t res = 0;
			const char* it = m_base;

			res += static_cast<uint8_t>(*it) & 0b1111111;
			++it;

			if (c & 0b10000000)
			{
				res &= 0b11111;
				res <<= 6;
				res |= static_cast<uint8_t>(*it) & 0b111111;
				++it;

				if (c & 0b00100000)
				{
					res &= 0b1111111111;
					res <<= 6;
					res |= static_cast<uint8_t>(*it) & 0b111111;
					++it;

					if (c & 0b00010000)
					{
						res &= 0b1111111111111111;
						res <<= 6;
						res |= static_cast<uint8_t>(*it) & 0b111111;
						++it;
					}
				}
			}

			return static_cast<char32_t>(res);
		}

		inline bool utf8::access_iterator::operator!=(const utf8::access_iterator& another)
		{
			return m_base != another.m_base;
		}

		inline bool utf8::access_iterator::operator==(const utf8::access_iterator& another)
		{
			return m_base == another.m_base;
		}

		inline bool utf8::access_iterator::operator>=(const utf8::access_iterator& another)
		{
			return m_base >= another.m_base;
		}

		inline const char* utf8::access_iterator::base() const
		{
			return m_base;
		}

		inline utf8::access_iterator::access_iterator(const char* base)
			: m_base(base)
		{
		}
		
		inline utf8::access_iterator utf8::accessor::begin()
		{
			return m_begin;
		}

		inline utf8::access_iterator utf8::accessor::end()
		{
			return m_end;
		}

		inline utf8::accessor::accessor(const char* begin, const char* end)
			: m_begin(utf8::access_iterator(begin)), m_end(utf8::access_iterator(end))
		{
		}
	
		template <typename OutputIt>
		inline void utf8::encode(char32_t c, OutputIt out)
		{
			if (c <= U'\x007F')
			{
				*out = static_cast<uint8_t>(c);
				++out;
			}
			else if (c <= U'\x07FF')
			{
				*out = static_cast<uint8_t>(0b11000000 | (static_cast<uint32_t>(c) >> 6));
				++out;
				*out = static_cast<uint8_t>(0b10000000 | (static_cast<uint32_t>(c) & 0b111111));
				++out;
			}
			else if (c <= U'\xFFFF')
			{
				*out = static_cast<uint8_t>(0b11100000 | (static_cast<uint32_t>(c) >> 12));
				++out;
				*out = static_cast<uint8_t>(0b10000000 | ((static_cast<uint32_t>(c) >> 6) & 0b111111));
				++out;
				*out = static_cast<uint8_t>(0b10000000 | (static_cast<uint32_t>(c) & 0b111111));
				++out;
			}
			else if (c <= U'\x1FFFF')
			{
				*out = static_cast<uint8_t>(0b11100000 | (static_cast<uint32_t>(c) >> 18));
				++out;
				*out = static_cast<uint8_t>(0b10000000 | ((static_cast<uint32_t>(c) >> 12) & 0b111111));
				++out;
				*out = static_cast<uint8_t>(0b10000000 | ((static_cast<uint32_t>(c) >> 6) & 0b111111));
				++out;
				*out = static_cast<uint8_t>(0b10000000 | (static_cast<uint32_t>(c) & 0b111111));
				++out;
			}
		}
	}
}

#endif