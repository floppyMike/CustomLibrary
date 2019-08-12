#pragma once

#include <vector>
#include <algorithm>

namespace ctl
{
	template<typename ...T>
	struct ObSu
	{
		class Observer
		{
		public:
			virtual void update(const T&...) = 0;
		};

		class Subject
		{
		public:
			auto& attach(Observer *o) 
			{ 
				m_list.emplace_back(o); 
				return *this; 
			}
			auto& detach(Observer *o) 
			{ 
				m_list.erase(std::find(m_list.begin(), m_list.end(), o)); 
				return *this; 
			}

			void notify(const T&... para)
			{
				for (auto& i : m_list)
					i->update(para...);
			}

			constexpr const auto& data() const noexcept { return m_list; }

		private:
			std::vector<Observer*> m_list;
		};
	};
}