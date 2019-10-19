#pragma once

#include <memory>
#include <functional>

namespace ctl::sdl
{
	class IState
	{
	public:
		virtual ~IState() {}

		virtual void event(const SDL_Event&) {}
		virtual void input(const SDL_Event&) {}
		virtual void update() {}
		virtual void fixedUpdate() {}
		virtual void draw() {}
	};


	template<typename ImplState>
	class StateManager
	{
	public:
		const auto& operator->() const noexcept { return m_state; }
		auto& operator->() noexcept { return m_state; }

		template<typename State, typename... Arg>
		void set(Arg&&... args)
		{
			m_func = [args = std::make_tuple(std::forward<Arg>(args)...)] { return std::apply([](auto&&... args)
				{ return std::make_unique<State>(args...); }, std::move(args)); };
		}

		void update()
		{
			if (m_func)
			{
				m_state = m_func();
				m_func = nullptr;
			}
		}

	private:
		std::unique_ptr<ImplState> m_state;
		std::function<std::unique_ptr<ImplState> ()> m_func;
	};

}