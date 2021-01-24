#pragma once

#include <memory>
#include <functional>

namespace ctl::sdl
{
    class SDL_Event;

	// -----------------------------------------------------------------------------
	// State Interface
	// -----------------------------------------------------------------------------

	/**
	 * @brief State Interface
	 */
	class IState
	{
	public:
		virtual ~IState() = default;

		/**
		 * @brief Used for handling general events
		 */
		virtual void event(const SDL_Event &) {}
		/**
		 * @brief Used for handling input events
		 */
		virtual void input(const SDL_Event &) {}
		/**
		 * @brief Used for updating objects per iteration
		 */
		virtual void update() {}
		/**
		 * @brief Used for updating objects in a constant fashion without influence of lag
		 */
		virtual void fixed_update() {}
		/**
		 * @brief Used for drawing the object onto the renderers buffer
		 */
		virtual void draw() {}
	};

	// -----------------------------------------------------------------------------
	// State Manager
	// -----------------------------------------------------------------------------

	/**
	 * @brief Manages states and state changes
	 * @tparam ImplState State Type to manage
	 */
	template<typename ImplState>
	class StateManager
	{
	public:
		/**
		 * @brief Access constant state method
		 * @return const state ref
		 */
		auto operator->() const noexcept -> const auto & { return m_state; }
		/**
		 * @brief Access state method
		 * @return state ref
		 */
		auto operator->() noexcept -> auto & { return m_state; }

		/**
		 * @brief Queues a new state
		 *
		 * @tparam State State type
		 * @param args State constructor arguments
		 */
		template<typename State, typename... _Arg>
		void set(_Arg &&... args)
		{
			m_func = [args = std::make_tuple(std::forward<_Arg>(args)...)]() mutable {
				return std::apply([](auto &&... args) { return std::make_unique<State>(std::forward<_Arg>(args)...); },
								  std::move(args));
			};
		}

		/**
		 * @brief Changes state when queued
		 */
		void update()
		{
			if (m_func)
			{
				m_state = m_func();
				m_func	= nullptr;
			}
		}

	private:
		std::unique_ptr<ImplState>					m_state;
		std::function<std::unique_ptr<ImplState>()> m_func;
	};

} // namespace ctl::sdl