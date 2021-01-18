#pragma once
#ifndef NATIVA_STRING_VIEW
#define NATIVA_STRING_VIEW

#include <utility>

namespace nativa
{
	class string;

	/// <summary>
	/// A view to a char array without owning it
	/// Designed to be the base of string
	/// Should pass by reference
	/// </summary>
	class string_view
	{
		constexpr static const char* c_str_end(const char* c_str)
		{
			auto it = c_str;
			while (*it != '\0') ++it;
			return it;
		}

	public:
		// Create from string literals.
		template <size_t N>
		constexpr string_view(const char(&c_str)[N])
			: m_begin(c_str), m_end(c_str + N - 1) // - 1 for the '\0'
		{
		}

		string_view(const char* begin, const char* end) noexcept;

		int compare_to(const string_view& another) const;

		ptrdiff_t last_index_of(char target) const;

		ptrdiff_t last_index_of(char target, size_t before) const;

		ptrdiff_t index_of(char target, size_t from = 0) const;

		ptrdiff_t index_of(const string_view& target, size_t from = 0) const;

		/// <summary>
		/// Split the string_view into substrings by the given delimiter.
		/// </summary>
		/// <typeparam name="OutputIt">An insert iterator</typeparam>
		/// <param name="delim">The delimiter string_view</param>
		/// <param name="output">The insert iterator of the collection into which the result goes</param>
		template <typename InsertIt>
		void split_clone(const string_view& delim, InsertIt output) const;

		/// <summary>
		/// Split the string_view into substrings by the given delimiter.
		/// </summary>
		/// <typeparam name="OutputIt">An insert iterator</typeparam>
		/// <param name="delim">The delimiter char</param>
		/// <param name="output">The insert iterator of the collection into which the result goes</param>
		template <typename InsertIt>
		void split_clone(char delim, InsertIt output) const;

		/// <summary>
		/// Split the string_view into slices by the given delimiter.
		/// </summary>
		/// <typeparam name="OutputIt">An insert iterator</typeparam>
		/// <param name="delim">The delimiter string_view</param>
		/// <param name="output">The insert iterator of the collection into which the result goes</param>
		template <typename InsertIt>
		void split(const string_view& delim, InsertIt output) const;

		/// <summary>
		/// Split the string_view into slices by the given delimiter.
		/// </summary>
		/// <typeparam name="OutputIt">An insert iterator</typeparam>
		/// <param name="delim">The delimiter char</param>
		/// <param name="output">The insert iterator of the collection into which the result goes</param>
		template <typename InsertIt>
		inline void split(char delim, InsertIt output) const;

		/// <summary>
		/// Creates a string owning a clone of the view
		/// </summary>
		/// <returns>A string that owns a clone of the view</returns>
		nativa::string clone() const;

		nativa::string_view slice(size_t begin, size_t length) const;

		void copy_to(char* buffer) const;

		const char* begin() const;

		const char* end() const;

		size_t size() const;
	protected:
		const char* m_begin;
		const char* m_end;

		const char* find_last(char target, const char* before) const;

		const char* find(char target, const char* from) const;

		const char* find(const string_view& target, const char* from) const;

	private:
		template <typename OutputIt, typename Elem>
		static auto cloned(OutputIt it)
		{
			struct cloned_iterator
			{
				OutputIt m_it;

				cloned_iterator(OutputIt it)
					: m_it(it)
				{
				}

				cloned_iterator& operator=(const Elem& elem)
				{
					m_it = elem.clone();
					return *this;
				}
			};

			return cloned_iterator(it);
		}
	};

	template <typename InsertIt>
	inline void string_view::split(const string_view& delim, InsertIt output) const
	{
		auto it = m_begin;
		auto d_size = delim.size();
		for (;;)
		{
			auto begin = it;
			it = find(delim, it);
			output = string_view(begin, it);

			if (it == m_end) break;
			else it += d_size;
		}
	}

	template <typename InsertIt>
	inline void string_view::split(char delim, InsertIt output) const
	{
		auto it = m_begin;
		for (;;)
		{
			auto begin = it;
			it = find(delim, it);
			output = string_view(begin, it);

			if (it == m_end) break;
			else ++it;
		}
	}


	template <typename InsertIt>
	inline void string_view::split_clone(const string_view& delim, InsertIt output) const
	{
		split(delim, cloned<InsertIt, string_view>(output));
	}

	template <typename InsertIt>
	inline void string_view::split_clone(char delim, InsertIt output) const
	{
		split(delim, cloned<InsertIt, string_view>(output));
	}
}

nativa::string_view operator""_ns(const char* c_str, size_t len);

#endif