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
		string_builder& append(string str);

		string_builder& append(char c);

		size_t size() const;

		void shrink_to_fit();

		void resize(size_t size);

		string to_string();

	private:
		std::vector<char> m_buffer;

		char* grow(size_t size);
	};
}

#endif