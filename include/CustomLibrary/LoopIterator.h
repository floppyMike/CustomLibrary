#pragma once

#include "Traits.h"

namespace ctl
{
	/**
	 * @brief Defines a Iterator that loops back to the beginning at end
	 * @tparam Container
	 */
	template<typename Container>
	class LoopIterator
	{
	public:
		using value_type		= array_value_t<Container>;
		using difference_type	= ptrdiff_t;
		using pointer			= value_type *;
		using reference			= value_type &;
		using iterator_category = array_iter_cat_t<Container>;

		/**
		 * @brief Create LoopIterator from container using a differential value
		 * @param c Array to use
		 * @param dif Differential
		 */
		explicit LoopIterator(Container &c, difference_type dif = 0)
			: m_ptr(&*std::begin(c) + dif)
			, m_c(&c)
		{
			assert(std::size(*m_c) == 0 && "Iterated container can't be empty");
			assert(std::size(*m_c) > dif && "Can't go past or to end iterator.");
		}

		/**
		 * @brief Get the current value
		 * @return value type
		 */
		auto operator*() const noexcept { return *m_ptr; }
		/**
		 * @brief Advance by 1
		 * @return this object
		 */
		auto operator++() noexcept
		{
			m_ptr = m_ptr == std::end(*m_c) - 1 ? std::begin(*m_c) : m_ptr + 1;
			return *this;
		}

	private:
		pointer	   m_ptr;
		Container *m_c;
	};
} // namespace ctl