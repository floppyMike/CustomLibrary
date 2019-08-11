/**
* @licence 
* MIT License
*
* Copyright (c) 2019 Michaellu
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.

* @file CString.h
* @summary provides definitions of the object cstring
* @author Michaellu
* @version 1.0
*/

#pragma once

#include <string>
#include "Error.h"

namespace ctl
{
	template<size_t t_size, typename CharT, typename Traits = std::char_traits<CharT>>
	class basic_cstring
	{
		static constexpr size_t WITH_NULL = t_size + 1;

		/**
		* @summary base class for iterators
		* @tparam "PtrType" type of pointer
		* @tparam "t_size" size of array contained
		*/
		template<typename PtrType>
		class basic_iterator
		{
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = typename std::remove_cv_t<std::remove_pointer_t<PtrType>>;
			using difference_type = std::ptrdiff_t;
			using pointer = PtrType;
			using reference = typename std::remove_pointer_t<PtrType>&;

			/**
			* @summary constructs object
			* @param "ptr" begin ptr
			* @param "diff" reference to this
			* @remarks ptr must be in range of cstring array
			*/
			constexpr basic_iterator(PtrType ptr, const basic_cstring* str) noexcept
				: m_ref(str)
				, m_ptr(ptr)
			{
				assert(m_ptr <= m_ref->c_str() + m_ref->size() && m_ptr >= m_ref->c_str() && "Iterator out of range.");
			}

			/**
			* @summary default constructs object
			*/
			constexpr basic_iterator() = default;
			constexpr basic_iterator(const basic_iterator&) = default;
			constexpr basic_iterator(basic_iterator&&) = default;

			constexpr basic_iterator& operator=(const basic_iterator&) = default;
			constexpr basic_iterator& operator=(basic_iterator&&) = default;

			/**
			* @remarks iterator should be inside range of cstring
			*/
			constexpr reference operator*() const noexcept
			{
				assert(m_ptr <= m_ref->c_str() + m_ref->size() && m_ptr >= m_ref->c_str() && "Iterator out of accessible range.");
				return *m_ptr;
			}

			/**
			* @remarks can't go after end
			*/
			constexpr auto& operator++() noexcept
			{
				++m_ptr;
				assert(m_ptr <= m_ref->c_str() + m_ref->size() && "Iterator out of range.");

				return *this;
			}
			constexpr auto operator++(int) noexcept
			{
				auto temp = *this;
				++* this;
				return temp;
			}

			/**
			* @remarks can't go before begin
			*/
			constexpr auto& operator--() noexcept
			{
				--m_ptr;
				assert(m_ptr >= m_ref->c_str() && "Iterator out of range.");

				return *this;
			}
			constexpr auto operator--(int) noexcept
			{
				auto temp = *this;
				--* this;
				return temp;
			}

			/**
			* @summary Arithmetic operators
			* @returns modified iterator
			*/
			constexpr auto operator+(const size_t& dis) noexcept { return basic_iterator(m_ptr + dis, m_ref); }
			constexpr auto operator-(const size_t& dis) noexcept { return basic_iterator(m_ptr - dis, m_ref); }

			/**
			* @summary Assignment operators
			*/
			constexpr auto& operator+=(const size_t& dis) noexcept
			{
				m_ptr += dis;
				assert(m_ptr <= m_ref->c_str() + m_ref->size() && "Iterator out of range.");
				return *this;
			}
			constexpr auto& operator-=(const size_t& dis) noexcept
			{
				m_ptr -= dis;
				assert(m_ptr >= m_ref->c_str() && "Iterator out of range.");
				return *this;
			}

			/**
			* @remarks must be from same string
			*/
			constexpr auto operator!=(const basic_iterator& other) noexcept
			{
				assert(m_ref->c_str() == other.m_ref->c_str() && "Iterator not from same string.");
				return m_ptr != other.m_ptr;
			}
			constexpr auto operator==(const basic_iterator& other) noexcept
			{
				return !(*this != other);
			}

		protected:
			const basic_cstring* m_ref = nullptr;
			PtrType m_ptr = nullptr;
		};

	public:
		using value_type = CharT;
		using traits_type = Traits;

		using size_type = size_t;
		using difference_type = std::ptrdiff_t;

		using reference = value_type&;
		using const_reference = const value_type&;

		using pointer = value_type*;
		using const_pointer = const value_type*;

		using iterator = basic_iterator<CharT*>;
		using const_iterator = basic_iterator<const CharT*>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		/**
		* @summary access functions
		* @remarks "i" must be in range of array
		* @returns char from string
		*/
		constexpr const auto& operator[](const size_type& i) const noexcept 
		{
			assert(i > t_size - 1 && "Operator out of boundry"); 
			return m_vals[i]; 
		}
		constexpr auto& operator[](const size_type& i) noexcept 
		{ 
			assert(i > t_size - 1 && "Operator out of boundry");
			return m_vals[i]; 
		}

		/**
		* @summary at access functions
		* @exception std::out_of_range if i isn't inside range
		* @remarks "i" must be in range of array
		* @returns char from string
		*/
		constexpr const auto& at(const size_type& i) const
		{
			if (i >= t_size)
				throw std::out_of_range("cstring::at is called out of range with i = " + std::to_string(i));

			return m_vals[i];
		}
		constexpr auto& at(const size_type& i)
		{
			if (i >= t_size)
				throw std::out_of_range("cstring::at is called out of range with i = " + std::to_string(i));

			return m_vals[i];
		}

		/**
		* @summary front and back accessors
		* @returns value in question
		*/
		constexpr const auto& front() const noexcept { return *m_vals; }
		constexpr auto& front() noexcept { return *m_vals; }

		constexpr const auto& back() const noexcept { return m_vals[t_size - 1]; }
		constexpr auto& back() noexcept { return m_vals[t_size - 1]; }

		/**
		* @summary c-string accessor (better then directly)
		* @returns the c-string stored
		*/
		constexpr const auto* c_str() const noexcept { return m_vals; }
		constexpr auto* data() noexcept { return m_vals; }

		/**
		* @summary assign a c-string to the cstring
		* @param "ptr" from pointer
		* @param "count" up to
		* @remarks "count" must not be higher then the size of this
		*/
		basic_cstring& assign(const_pointer const ptr, const size_type& count) noexcept
		{
			assert(count <= t_size && "count is too large.");
			Traits::copy(m_vals, ptr, count);

			return *this;
		}
		/**
		* @summary fill out string with "ch"
		* @param "ch" character type to fill out with
		* @param "count" how much to fill out
		* @remarks "count" must not be higher then size of this
		*/
		basic_cstring& assign(const size_type& count, const_reference ch) noexcept
		{
			assert(count <= t_size && "count is too large.");
			Traits::assign(m_vals, count, ch);

			return *this;
		}
		/**
		* @summary replace this with "str"
		* @param "str" string to fill this out
		*/
		basic_cstring& assign(const basic_cstring& str) noexcept
		{
			return *this = str;
		}
		/**
		* @summary replace this with "ptr"
		* @param "str" string to fill this out
		* @remarks doesn't use Traits::len
		*/
		basic_cstring& assign(const_pointer const ptr) noexcept
		{
			const auto len = ::strnlen_s(ptr, WITH_NULL);
			return assign(ptr, len);
		}
		/**
		* @summary fill string with range
		* @param "begin" begin iterator
		* @param "end" end iterator
		* @param "dest" begin of this
		* @remarks "Iter" must be of catagory input_iterator and value type of char
		* @remarks doesn't utilize Traits::copy
		*/
		template<typename Iter, 
			typename = typename std::enable_if_t<
			std::is_convertible_v<typename std::iterator_traits<Iter>::iterator_category, std::input_iterator_tag> &&
			std::is_same_v<typename std::iterator_traits<Iter>::value_type, value_type>>>
		basic_cstring& assign(const Iter& begin, const Iter& end) noexcept
		{
			std::copy(begin, end, this->begin());
			return *this;
		}

		/**
		* @summary Copies from initializer_list
		* @param "list" initializer_list to copy from
		* @remarks "list.size()" must be equal or under cstring size
		*/
		basic_cstring& assign(const std::initializer_list<value_type>& list) noexcept
		{
			return assign(&*list.begin(), list.size());
		}

		/**
		* @summary fill container with a character
		* @param "ch" character to fill up with
		*/
		basic_cstring& fill(const value_type& ch) noexcept
		{
			return assign(t_size, ch);
		}

		/**
		* @summary description
		* @param "name" description
		* @exception "type" description
		* @remarks description
		* @returns description
		*/
		basic_cstring& swap(basic_cstring& other) noexcept
		{
			std::swap(m_vals, other.m_vals);
		}

		/**
		* @summary iterator begin
		*/
		constexpr auto begin() noexcept { return iterator(m_vals, this); }
		constexpr auto begin() const noexcept { return const_iterator(m_vals, this); }
		constexpr auto cbegin() const noexcept { return const_iterator(m_vals, this); }

		/**
		* @summary iterator end
		*/
		constexpr auto end() noexcept { return iterator(m_vals + t_size, this); }
		constexpr auto end() const noexcept { return const_iterator(m_vals + t_size, this); }
		constexpr auto cend() const noexcept { return const_iterator(m_vals + t_size, this); }

		/**
		* @summary reverse iterator begin
		*/
		constexpr auto rbegin() noexcept { return reverse_iterator(end()); }
		constexpr auto rbegin() const noexcept { return const_reverse_iterator(cend()); }
		constexpr auto crbegin() const noexcept { return const_reverse_iterator(cend()); }

		/**
		* @summary reverse iterator end
		*/
		constexpr auto rend() noexcept { return reverse_iterator(begin()); }
		constexpr auto rend() const noexcept { return const_reverse_iterator(cbegin()); }
		constexpr auto crend() const noexcept { return const_reverse_iterator(cbegin()); }

		/**
		* @summary Get the size of string
		* @returns size
		*/
		constexpr auto size() const noexcept 
		{ 
			return t_size; 
		}

		/**
		* @summary check if cstring is empty
		* @returns if empty
		*/
		constexpr auto empty() const noexcept
		{
			return t_size == 0;
		}

		/**
		* @summary ostream << overload
		*/
#ifdef _IOSTREAM_
		friend std::ostream& operator<<(std::ostream& out, const basic_cstring& str)
		{
			return out.write(str.m_vals, str.size());
		}
#endif // _IOSTREAM_

		//Not private since use of aggregate initialization
		value_type m_vals[WITH_NULL];
	};

	template<size_t size>
	using cstring = basic_cstring<size, char>;
}