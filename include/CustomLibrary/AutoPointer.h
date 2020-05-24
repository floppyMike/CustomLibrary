#pragma once

namespace ctl
{
	template<typename T>
	class auto_ptr
	{
	public:
		auto_ptr() = default;

		auto_ptr(const auto_ptr& ptr)
			: m_val(new T(*ptr.m_val))
		{
		}
		auto_ptr(auto_ptr& ptr)
			: m_val(ptr.m_val)
		{
			ptr.m_val = nullptr;
		}

		auto_ptr& operator=(const auto_ptr& ptr)
		{
			m_val = new T(*ptr.m_val);
		}
		auto_ptr& operator=(auto_ptr& ptr)
		{
			this->~auto_ptr();

			m_val = ptr.m_val;
			ptr.m_val = nullptr;
		}

		//auto_ptr(T* ptr)
		//	: m_val(ptr)
		//{
		//	ptr = nullptr;
		//}

		~auto_ptr()
		{
			if (m_val != nullptr)
				delete m_val;
		}

		auto& reset(T* ptr = nullptr)
		{
			this->~auto_ptr();

			m_val = ptr;
			ptr = nullptr;

			return *this;
		}

		auto& operator->()
		{
			return m_val;
		}

		constexpr auto* get() { return m_val; }

	private:
		T* m_val = nullptr;
	};
}