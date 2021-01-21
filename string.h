#pragma once
#ifndef NATIVA_IMMUTABLE_STRING
#define NATIVA_IMMUTABLE_STRING

#include <initializer_list>
#include <utility>
#include "string_view.h"

namespace nativa
{
	using ref_counter_t = size_t;

	struct string_internals;

	/// <summary>
	/// An immutable string.
	/// Should be easily copied.
	/// Passing by ref means temporary borrowing.
	/// </summary>
	class string : public string_view
	{
		friend struct string_internals;

	public:

		// Create from string literals.
		template <size_t N>
		constexpr string(const char(&c_str)[N])
			: m_counter(nullptr), string_view(c_str)
		{
		}
		
		constexpr string()
			: m_counter(nullptr), string_view()
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

		string& operator=(const string& str) noexcept;

		string& operator=(string&& str) noexcept;

#pragma region Public Static Methods
		/// <summary>
		/// Concatenate several strings into one.
		/// </summary>
		/// <typeparam name="Enumerable">An enumerable of strings</typeparam>
		/// <param name="enumerable">The strings</param>
		/// <returns>The new string</returns>
		template <typename Enumerable = std::initializer_list<string>>
		static string concat(const Enumerable& enumerable);

		template <typename Enumerable = std::initializer_list<string>>
		static string join(const string_view& delim, const Enumerable& enumerable);
#pragma endregion

		/// <summary>
		/// Get a substring of the string.
		/// Equivalent to .slice(...).clone()
		/// </summary>
		/// <param name="begin">The index where the substring begins</param>
		/// <param name="length">The length of the substring</param>
		/// <returns>The substring</returns>
		nativa::string substring(size_t begin, size_t length) const;

		string_view view() const;

		const char* c_str() const;

		operator const char*() const;

		ref_counter_t ref_count() const;

	private:
		/// <summary>
		/// Should be nullptr if not ref-counted
		/// </summary>
		ref_counter_t* m_counter;

		string(ref_counter_t* counter, const char* begin, const char* end) noexcept;
	};

#pragma region Template Function Impl

	template <typename Enumerable>
	inline string string::concat(const Enumerable& enumerable)
	{
		size_t new_size = 0;
		auto begin = std::begin(enumerable);
		auto end = std::end(enumerable);

		static_assert(
			std::is_convertible<decltype(*begin), string_view>::value,
			"The elements in the enumerable should be convertible to string_view");

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
	inline string string::join(const string_view& delim, const Enumerable& enumerable)
	{
		size_t new_size = 0;
		size_t delim_size = delim.size();
		auto begin = std::begin(enumerable);

		static_assert(
			std::is_convertible<decltype(*begin), string_view>::value,
			"The elements in the enumerable should be convertible to string_view");

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
			it->copy_to(buf_it);
			buf_it += it->size();
			++it;

			for (; it != end; ++it)
			{
				delim.copy_to(buf_it);
				buf_it += delim_size;
				it->copy_to(buf_it);
				buf_it += it->size();
			}
		}

		return std::move(res);
	}

#pragma endregion

	/// <summary>
	/// Universal interface for allocation of nativa::string
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
}

#endif

