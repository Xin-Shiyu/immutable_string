#pragma once
#ifndef NATIVA_IMMUTABLE_STRING
#define NATIVA_IMMUTABLE_STRING

#include <initializer_list>
#include <utility>

namespace nativa
{
	using ref_counter_t = size_t;

	class string;

	struct string_util
	{
		static constexpr const char* c_str_end(const char* c_str)
		{
			auto it = c_str;
			while (*it != '\0') ++it;
			return it;
		}

		static void c_str_copy(char* dest, const char* src_begin, const char* src_end);
	};

	/// <summary>
	/// The implementation should be consistent.
	/// </summary>
	struct string_internals
	{
		/// <summary>
		/// Allocates a runtime string.
		/// </summary>
		/// <param name="length">Length of the string</param>
		/// <param name="mutable_raw">An out param, mutable buffer of the string</param>
		/// <returns>The ready-to-use string</returns>
		static nativa::string alloc(size_t length, char*& mutable_raw);

		/// <summary>
		/// Frees the runtime string.
		/// </summary>
		/// <param name="disposed">The string to be disposed; move it as it should not be used again</param>
		static void free(nativa::string&& disposed);
	};

	/// <summary>
	/// An immutable string.
	/// Should be easily copied.
	/// Passing by ref means temporary borrowing.
	/// </summary>
	class string
	{
		friend struct string_util;
		friend struct string_internals;

	public:
		// Create from C-style string or string literals.
		constexpr string(const char* c_str)
			: m_counter(nullptr), m_begin(c_str), m_end(string_util::c_str_end(c_str))
		{
		}

		// Copy ctor.
		// Increases ref count
		string(const string& str) noexcept;

		// Move ctor.
		// Invalidates the old one
		// Suppressing ref count change
		string(string&& str) noexcept;

		// Dtor.
		// Decreases ref count if valid
		~string() noexcept;

#pragma region Public Static Methods
		static int compare(string left, string right);

		/// <summary>
		/// Concatenate several strings into one.
		/// </summary>
		/// <typeparam name="Enumerable">An enumerable of strings</typeparam>
		/// <param name="enumerable">The strings</param>
		/// <returns>The new string</returns>
		template <typename Enumerable = std::initializer_list<string>>
		static string concat(const Enumerable& enumerable);

		template <typename Enumerable = std::initializer_list<string>>
		string join(const Enumerable& enumerable) const;
#pragma endregion

		/// <summary>
		/// Split the string into parts by the given delimiter.
		/// </summary>
		/// <typeparam name="OutputIt">An insert iterator</typeparam>
		/// <param name="delim">The delimiter string</param>
		/// <param name="output">The insert iterator of the collection into which the result goes</param>
		template <typename OutputIt>
		void split(const string& delim, OutputIt output) const;

		/// <summary>
		/// Split the string into parts by the given delimiter.
		/// </summary>
		/// <typeparam name="OutputIt">An insert iterator</typeparam>
		/// <param name="delim">The delimiter char</param>
		/// <param name="output">The insert iterator of the collection into which the result goes</param>
		template <typename OutputIt>
		void split(char delim, OutputIt output) const;

		/// <summary>
		/// Get a substring of the string.
		/// </summary>
		/// <param name="begin">The index where the substring begins</param>
		/// <param name="length">The length of the substring</param>
		/// <returns>The substring</returns>
		nativa::string substring(size_t begin, size_t length) const;

		ptrdiff_t last_index_of(char target) const;

		ptrdiff_t last_index_of(char target, size_t before) const;

		ptrdiff_t index_of(char target, size_t from = 0) const;

		ptrdiff_t index_of(const string& target, size_t from = 0) const;

		void copy_to(char* buffer) const;

		const char* c_str() const;

		const char* begin() const;

		const char* end() const;

		size_t size() const;

		ref_counter_t reference_count() const;

	private:
		/// <summary>
		/// Should be nullptr if not ref-counted
		/// </summary>
		ref_counter_t* m_counter;
		const char* m_begin;
		const char* m_end;

		string(ref_counter_t* counter, const char* begin, const char* end) noexcept;

		nativa::string substring(const char* begin, const char* end) const;

		const char* find_last(char target, const char* before) const;

		const char* find(char target, const char* from) const;

		const char* find(const string& target, const char* from) const;
	};

#pragma region Template Function Impl

	template <typename Enumerable>
	inline string string::concat(const Enumerable& enumerable)
	{
		size_t new_size = 0;
		auto begin = std::begin(enumerable);
		auto end = std::end(enumerable);
		for (auto it = begin; it != end; ++it)
		{
			new_size += it->size();
		}

		if (new_size == 0) return string("");

		char* buffer;
		string res = string_internals::alloc(new_size, buffer);

		auto buf_it = buffer;
		for (auto it = begin; it != end; ++it)
		{
			it->copy_to(buf_it);
			buf_it += it->size();
		}
		return std::move(res);
	}
	
	template <typename Enumerable>
	inline string string::join(const Enumerable& enumerable) const
	{
		size_t new_size = 0;
		size_t delim_size = this->size();
		auto begin = std::begin(enumerable);
		auto end = std::end(enumerable);
		if (begin != end)
		{
			auto it = begin;
			new_size += it->size();
			++it;

			for (; it != end; ++it)
			{
				new_size += it->size() + delim_size;
			}
		}

		if (new_size == 0) return string("");

		char* buffer;
		string res = string_internals::alloc(new_size, buffer);

		auto buf_it = buffer;
		// if (begin != end)
		// Not needed because if begin == end then
		// new_size == 0 and the flow never reaches here
		{
			auto it = begin;
			string_util::c_str_copy(buf_it, it->m_begin, it->m_end);
			buf_it += it->size();
			++it;

			for (; it != end; ++it)
			{
				this->copy_to(buf_it);
				buf_it += delim_size;
				it->copy_to(buf_it);
				buf_it += it->size();
			}
		}

		return std::move(res);
	}

	template <typename OutputIt>
	inline void string::split(const string& delim, OutputIt output) const
	{
		auto it = m_begin;
		auto d_size = delim.size();
		for (;;)
		{
			auto begin = it;
			it = find(delim, it);
			output = substring(begin, it);

			if (it == m_end) break;
			else it += d_size;
		}
	}

	template <typename OutputIt>
	inline void string::split(char delim, OutputIt output) const
	{
		auto it = m_begin;
		for (;;)
		{
			auto begin = it;
			it = find(delim, it);
			output = substring(begin, it);

			if (it == m_end) break;
			else ++it;
		}
	}

#pragma endregion
}

nativa::string operator""_ns(const char* c_str, size_t);

#endif

