#pragma once
#ifndef NATIVA_STRING_VIEW
#define NATIVA_STRING_VIEW

#include <cstddef>
#include <functional>
#include <utility>

namespace nativa
{
	class string;

	constexpr const char* c_str_end(const char* c_str);

	/// <summary>
	/// A view to a char array without owning it
	/// Designed to be the base of string
	/// Should pass by reference
	/// </summary>
	class string_view
	{
	public:
		// Create from string literals.
		template <size_t N>
		constexpr string_view(const char(&c_str)[N]);

		constexpr string_view();

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
		void split(char delim, InsertIt output) const;

		/// <summary>
		/// Creates a string owning a clone of the view.
		/// </summary>
		/// <returns>A string that owns a clone of the view</returns>
		nativa::string clone() const;

		/// <summary>
		/// Creates a slice with given begin index and length.
		/// </summary>
		/// <param name="begin">The begin index of the slice</param>
		/// <param name="length">The length of the slice</param>
		/// <returns>The expected slice</returns>
		nativa::string_view slice(size_t begin, size_t length) const;

		/// <summary>
		/// Copies the data of the string_view to a buffer
		/// </summary>
		/// <param name="buffer">A pointer to the buffer, make sure it has enough space</param>
		void copy_to(char* buffer) const;

		const char* begin() const;

		const char* end() const;

		size_t size() const;

		bool operator<(const string_view& right) const;

		bool operator<=(const string_view& right) const;

		bool operator==(const string_view& right) const;

		bool operator>=(const string_view& right) const;

		bool operator>(const string_view& right) const;

		bool operator!=(const string_view& right) const
		{
			return !this->operator==(right);
		}

		/// <summary>
		/// Creates a slice from a certain index to a certain index exclusive.
		/// </summary>
		/// <param name="range">An std::pair representing the range, right exclusive</param>
		/// <returns>The expected slice</returns>
		nativa::string_view operator[](const std::pair<size_t, size_t>& range) const;

		/// <summary>
		/// Gets an enumerable treating the string as the given encoding
		/// </summary>
		/// <typeparam name="Encoding">A type with at least one member type called accessor</typeparam>
		/// <returns>An enumerable of a certain type of char</returns>
		template <typename Encoding>
		auto access_as() const;

		template <typename Encoding>
		size_t length() const;

		bool is_empty() const;

		bool starts_with(const string_view& pattern) const;

		bool ends_with(const string_view& pattern) const;

		bool contains(const string_view& pattern) const;

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

	constexpr const char* c_str_end(const char* c_str)
	{
		auto it = c_str;
		while (*it != '\0') ++it;
		return it;
	}

	template <size_t N>
	inline constexpr string_view::string_view(const char(&c_str)[N])
		: m_begin(c_str), m_end(c_str + N - 1) // - 1 for the '\0'
	{
	}

	inline constexpr string_view::string_view()
		: m_begin(""), m_end("" + 1)
	{
	}

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

	template <typename Encoding>
	inline auto string_view::access_as() const
	{
		return typename Encoding::accessor(m_begin, m_end);
	}

	template <typename Encoding>
	inline size_t string_view::length() const
	{
		size_t res = 0;
		for (const auto& c : this->access_as<Encoding>()) ++res;
		return res;
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

namespace std
{
	template <>
	struct hash<nativa::string_view>
	{
		size_t operator()(const nativa::string_view& sv) const
		{
			size_t hash = 0;
			for (char c : sv)
			{
				hash *= 31;
				hash += c;
			}
			return hash;
		}
	};
}

nativa::string_view operator""_ns(const char* c_str, size_t len);

#endif