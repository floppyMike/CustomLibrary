#pragma once

#include <SDL2/SDL.h>

#include "../BasicTypes.h"
#include "../Error.h"

#include <cassert>
#include <memory>

namespace ctl::sdl
{
	/**
	 * @brief Manages a window instance
	 */
	class Window
	{
		struct Unique_Destructor
		{
			void operator()(SDL_Window *w) { SDL_DestroyWindow(w); }
		};

	public:
		/**
		 * @brief Initializes a new window
		 *
		 * @param name Window title
		 * @param dim Dimensions of the window
		 * @param window_flags Window creation flags
		 */
		Window(std::string_view name, const mth::Dim<int> &dim, Uint32 window_flags = SDL_WINDOW_SHOWN)
		{
			create(name, dim, window_flags);
		}

		/**
		 * @brief Initialize the renderer
		 *
		 * @tparam ImplWin Window type
		 * @param win Window ptr
		 * @param rendererFlags SDL_Renderer Flags https://wiki.libsdl.org/SDL_RendererFlags#Values
		 */
		auto create(std::string_view name, const mth::Dim<int> &dim, Uint32 window_flags = SDL_WINDOW_SHOWN) -> void
		{
			m_window.reset(SDL_CreateWindow(name.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dim.w, dim.h,
											window_flags));
			if (m_window)
				throw std::runtime_error(SDL_GetError());
		}

		/**
		 * @brief Kills and deallocated the window
		 */
		void destroy() noexcept { m_window.reset(); }

		/**
		 * @brief Get the window ptr
		 * @return SDL_Window ptr
		 */
		[[nodiscard]] auto get() const noexcept
		{
			assert(m_window != nullptr && "Window isn't loaded.");
			return m_window.get();
		}

		/**
		 * @brief Get the window ID registered by the OS
		 * @return unsigned int
		 */
		[[nodiscard]] auto ID() const noexcept { return SDL_GetWindowID(get()); }

		/**
		 * @brief Get the current window dimension
		 * @return Dimension
		 */
		[[nodiscard]] auto dim() const noexcept
		{
			mth::Dim<int> size;
			SDL_GetWindowSize(get(), &size.w, &size.h);
			return size;
		}

		/**
		 * @brief Change the title of the window
		 * @param name New title
		 */
		auto window_title(std::string_view name) const noexcept { SDL_SetWindowTitle(get(), name.data()); }

		/**
		 * @brief Get the title of the window
		 * @return string_view of the title
		 */
		[[nodiscard]] auto window_title() const noexcept { return SDL_GetWindowTitle(get()); }

	private:
		std::unique_ptr<SDL_Window, Unique_Destructor> m_window = nullptr;
	};

} // namespace ctl::sdl