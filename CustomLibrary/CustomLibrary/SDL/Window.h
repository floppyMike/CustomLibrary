#pragma once

#include <CustomLibrary/Vector.h>

#include "State.h"
#include "BasicTypes.h"

namespace ctl
{
	class Window
	{
	public:
		Window(const char* name, const SDLDim<int>& dim, Uint32 windowFlags = SDL_WINDOW_SHOWN)
			: m_dim(dim)
		{
			m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				m_dim.w, m_dim.h, windowFlags);
			if (m_window == nullptr)
				throw ctl::Log(SDL_GetError());

			m_id = SDL_GetWindowID(m_window);
		}

		~Window()
		{
			destroy();
		}

		void destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window);
		}

		constexpr auto* window() noexcept { return m_window; }
		constexpr auto ID() const noexcept { return m_id; }
		constexpr const auto& dim() const noexcept { return m_dim; }

	private:
		SDL_Window* m_window = nullptr;
		Uint32 m_id;
		SDLDim<int> m_dim;
	};


	template<typename ImplWatch, typename ImplStateDB>
	class BasicWindow
	{
	public:
		using type = BasicWindow;
		using watch_t = ImplWatch;

		/**
		* @summary construct basics for a window
		* @param "name" window name
		* @param "dim" dimension of window
		* @param "windowFlags" flags for window creation
		* @exception "Log" if window creation fails
		* @remarks flags are https://wiki.libsdl.org/SDL_WindowFlags#Values
		*/
		BasicWindow(const char* name, const SDLDim<int>& dim, Uint32 windowFlags = SDL_WINDOW_SHOWN)
			: m_dim(dim)
		{
			m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				m_dim.w, m_dim.h, windowFlags);
			if (m_window == nullptr)
				throw ctl::Log(SDL_GetError());

			m_id = SDL_GetWindowID(m_window);
		}

		/**
		* @summary deallocate resources if out of scope
		*/
		virtual ~BasicWindow()
		{
			destroy();
		}

		/**
		* @summary deallocate resources of window and close it
		*/
		void destroy() noexcept
		{
			if (m_window != nullptr)
				SDL_DestroyWindow(m_window);
		}

		/**
		* @summary allocate and set current state
		* @tparam "State" derived State class
		* @param "args" arguments for state class
		*/
		template<typename State, typename ...Args>
		StateBase& setState(Args&& ... args)
		{
			m_state.reset(new State(std::forward<Args>(args)...));
			return *m_state;
		}

		/**
		* @summary deallocate current state
		*/
		BasicWindow& removState() noexcept
		{
			m_state.reset(nullptr);
			return *this;
		}

		/**
		* @summary accessors for combatibility with SDL functions
		*/
		constexpr auto* window() noexcept { return m_window; }
		constexpr auto ID() const noexcept { return m_id; }
		constexpr const auto& dim() const noexcept { return m_dim; }

		/**
		* @summary const accessor for checking for mouse focus
		*/
		constexpr bool focus(size_t foc) const noexcept { return m_watch.state(foc); }

	protected:
		/**
		* @summary handles the update procedure
		* @param "e" event polled from engine
		*/
		void update(const SDL_Event& e)
		{
			if (e.window.windowID == m_id)
			{
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					m_dim.w = e.window.data1;
					m_dim.h = e.window.data2;
					break;

				case SDL_WINDOWEVENT_CLOSE:
					destroy();
					break;

				default:
					m_watch.listen(e);
					break;
				}

				m_state->input(e);
				m_state->event(e);
			}
		}

		SDL_Window *m_window = nullptr;
		Uint32 m_id;

		SDLDim<int> m_dim;
		ImplWatch m_watch;

		ImplStateDB m_state;
	};

	class EventWatch
	{
	public:
		enum Focus { MOUSE, KEYBOARD, FULLSCREEN, MINIMIZED, SHOWN, CLOSE };

		void listen(const SDL_Event& e) noexcept;
		bool state(size_t foc) const noexcept 
		{
			return m_focus[foc];
		}

	private:
		std::bitset<6> m_focus = std::bitset<6>(0b010011);
	};


	//----------------------------------------------
	//Implementation
	//----------------------------------------------
	
	inline void EventWatch::listen(const SDL_Event& e) noexcept
	{
		if (e.type == SDL_WINDOWEVENT)
			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_ENTER:		   m_focus[MOUSE] = true; break;
			case SDL_WINDOWEVENT_LEAVE:		   m_focus[MOUSE] = false; break;
			case SDL_WINDOWEVENT_FOCUS_GAINED: m_focus[KEYBOARD] = true; break;
			case SDL_WINDOWEVENT_FOCUS_LOST:   m_focus[KEYBOARD] = false; break;
			case SDL_WINDOWEVENT_MINIMIZED:    m_focus[MINIMIZED] = true; break;
			case SDL_WINDOWEVENT_MAXIMIZED:
			case SDL_WINDOWEVENT_RESTORED:	   m_focus[MINIMIZED] = false; break;

			default: break;
			}
	}

}