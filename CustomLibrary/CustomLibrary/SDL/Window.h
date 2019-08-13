#pragma once

#include <CustomLibrary/Observer.h>
#include <CustomLibrary/Vector.h>

#include "State.h"
#include "BasicTypes.h"

namespace ctl
{
	class WindowBase : public ObSu<SDL_Event>::Observer
	{
	public:
		enum Focus { MOUSE, KEYBOARD, FULLSCREEN, MINIMIZED, SHOWN, CLOSE };

		/**
		* @summary construct basics for a window
		* @param "name" window name
		* @param "dim" dimension of window
		* @param "windowFlags" flags for window creation
		* @exception "Log" if window creation fails
		* @remarks flags are https://wiki.libsdl.org/SDL_WindowFlags#Values
		*/
		WindowBase(const std::string_view& name, const SDLDim<Uint32>& dim, const Uint32& windowFlags = SDL_WINDOW_SHOWN);

		/**
		* @summary deallocate resources if out of scope
		*/
		virtual ~WindowBase()
		{
			destroy();
		}

		/**
		* @summary deallocate resources of window and close it
		*/
		void destroy() noexcept;

		/**
		* @summary invokes function of StateBase
		* @param "f" ptr to member function of StateBase
		*/
		void _invoke_(void (StateBase::*f)()) noexcept
		{
			((*m_state).*f)();
		}

		/**
		* @summary interface function for renderer implementation
		*/
		virtual void _render_() = 0;

		/**
		* @summary allocate and set current state
		* @tparam "State" derived State class
		* @param "args" arguments for state class
		*/
		template<typename State, typename ...Args>
		auto& setState(Args&&... args);

		/**
		* @summary deallocate current state
		*/
		auto removState() noexcept -> WindowBase&;

		/**
		* @summary accessors for combatibility with SDL functions
		*/
		constexpr auto* window() noexcept { return m_window; }
		constexpr const auto& ID() const noexcept { return m_id; }
		constexpr const auto& dim() const noexcept { return m_dim; }

		/**
		* @summary const accessor for checking for mouse focus
		*/
		constexpr const auto& focus(const Focus& foc) const noexcept { return m_focus[foc]; }

	protected:
		/**
		* @summary handles the update procedure of the observer
		* @param "e" event polled from engine
		*/
		void update(const SDL_Event& e) override final;

		SDL_Window *m_window = nullptr;
		Uint32 m_id;

		SDLDim<Uint32> m_dim;
		std::bitset<6> m_focus;

		std::unique_ptr<StateBase> m_state;
	};

	template<typename State, typename ...Args>
	inline auto& WindowBase::setState(Args&& ...args)
	{
		m_state = std::make_unique<State>(args...);
		return *this;
	}
}