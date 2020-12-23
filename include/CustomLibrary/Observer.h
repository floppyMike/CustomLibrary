#pragma once

#include <vector>
#include <algorithm>

namespace ctl
{
	/**
	 * @brief Typical object & subject coding pattern
	 * @tparam Parameter types to use for the update function
	 */
	template<typename... T>
	struct ObSu
	{
		/**
		 * @brief The observer that the subject updates
		 */
		class Observer
		{
		public:
			virtual void update(const T &...) = 0;
		};

		/**
		 * @brief The subject that updates the observers
		 */
		class Subject
		{
		public:
			/**
			 * @brief Attaches a observer to the subject
			 * @param o Object that inherited the observer
			 */
			void attach(Observer *o) { m_list.emplace_back(o); }

			/**
			 * @brief Detaches a observer of the subject.
			 * @param o Object that inherited the observer
			 * @return if inside the list
			 */
			auto detach(Observer *o) -> bool
			{
				const auto i = std::find(m_list.begin(), m_list.end(), o);
				bool	   l = i != m_list.end();

				if (l)
					m_list.erase(i);

				return l;
			}

			/**
			 * @brief Update all the attached observers
			 * @param para Parameters to use
			 */
			void notify(const T &...para)
			{
				for (auto &i : m_list) i->update(para...);
			}

			/**
			 * @brief Gives the underlying array of observers
			 * @return Array ptr of observer
			 */
			constexpr auto data() const noexcept -> const auto & { return m_list.data(); }

		private:
			std::vector<Observer *> m_list;
		};
	};
} // namespace ctl