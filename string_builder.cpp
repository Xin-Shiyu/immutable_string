#include "string_builder.h"

namespace nativa
{
	string_builder& nativa::string_builder::append(string str)
	{
		str.copy_to(grow(str.size()));
		return *this;
	}

	string_builder& string_builder::append(char c)
	{
		*grow(1) = c;
		return *this;
	}

	size_t string_builder::size() const
	{
		return m_buffer.size();
	}

	void string_builder::shrink_to_fit()
	{
		m_buffer.shrink_to_fit();
	}

	void string_builder::resize(size_t size)
	{
		m_buffer.resize(size);
	}

	string string_builder::to_string()
	{
		char* mutable_raw;
		string res = string_internals::alloc(m_buffer.size(), mutable_raw);
		string_view(&m_buffer[0], &m_buffer[0] + m_buffer.size()).copy_to(mutable_raw);
		return std::move(res);
	}

	char* string_builder::grow(size_t size)
	{
		m_buffer.resize(m_buffer.size() + size);
		return &m_buffer[0] + m_buffer.size() - size;
		// You cannot dereference the end iterator
	}
}
