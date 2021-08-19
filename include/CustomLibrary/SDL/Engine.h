#pragma once

#include "SDL_init.h"

#include <thread>
#include <stdexcept>

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// SDL predefined Colors
	// -----------------------------------------------------------------------------

	// clang-format off
	static constexpr SDL_Color	BLACK = { 0, 0, 0, 0xFF }, 
								WHITE = { 0xFF, 0xFF, 0xFF, 0xFF },
								RED = { 0xFF, 0, 0, 0xFF }, 
								GREEN = { 0, 0xFF, 0, 0xFF }, 
								BLUE = { 0, 0, 0xFF, 0xFF },
								YELLOW = { 0xFF, 0xFF, 0, 0xFF }, 
								ORANGE = { 0xFF, 0xFF >> 1, 0xFF >> 2, 0xFF },
								DARK_GRAY = { 30, 30, 30, 0xFF },
								GRAY = { 50, 50, 50, 0xFF },
								LIGHT_GRAY = { 70, 70, 70, 0xFF };
	// clang-format on

	// -----------------------------------------------------------------------------
	// Core
	// -----------------------------------------------------------------------------

	/**
	 * @brief Handles initalization of SDL2 and its subsystems
	 */
	class SDL
	{
	public:
		/**
		 * @brief Initialize SDL2
		 * @param SDLFlags Flags used for initialization https://wiki.libsdl.org/SDL_Init#Remarks
		 */
		explicit SDL(Uint32 SDLFlags = SDL_INIT_VIDEO)
		{
			if (SDL_Init(SDLFlags) != 0)
				throw std::runtime_error(SDL_GetError());
		}

		/**
		 * @brief frees the subsystems
		 */
		~SDL()
		{
			SDL_Quit();

#if defined _SDL2MIX_
			Mix_Quit();
#endif

#if defined _SDL2TTF_
			TTF_Quit();
#endif

#if defined _SDL2IMG_
			IMG_Quit();
#endif
		}

#if defined _SDL2IMG_
		/**
		 * @brief init SDL_image
		 * @param "flags" flags for initializer
		 * @exception std::runtime_error if initialization fails
		 */
		static void init_IMG(int flags)
		{
			if ((IMG_Init(flags) & flags) != flags)
				throw std::runtime_error(SDL_GetError());
		}
#endif

#if defined _SDL2MIX_
		/**
		 * @brief init SDL_mixer
		 * @param feq frequency
		 * @param format format
		 * @param channels channels
		 * @param chunksize chunksize
		 * @exception "Log" if initialization fails
		 */
		static void init_Mix(int feq = 44100, Uint16 format = MIX_DEFAULT_FORMAT, int channels = 2, int chunksize = 2048)
		{
			if (Mix_OpenAudio(feq, format, channels, chunksize) < 0)
				throw std::runtime_error(SDL_GetError());
		}
#endif

#if defined _SDL2TTF_
		/**
		 * @brief init SDL_ttf
		 * @exception "Log" if initialization fails
		 */
		static void init_TTF()
		{
			if (TTF_Init() == -1)
				throw std::runtime_error(SDL_GetError());
		}
#endif
	};

	// -----------------------------------------------------------------------------
	// Application concept
	// -----------------------------------------------------------------------------

	template<typename T>
	concept is_application = requires(T a)
	{
		a.pre_pass();
		a.event(SDL_Event());
		a.update();
		a.render();
	};

	// -----------------------------------------------------------------------------
	// Runtime Loops
	// -----------------------------------------------------------------------------

	template<is_application App>
	void run(App *app, size_t fps)
	{
		const std::chrono::microseconds frame_time(1000000 / fps);

		for (bool quit = false; !quit;)
		{
			const auto end_time = std::chrono::steady_clock::now() + frame_time;

			app->pre_pass();

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				app->event(e);
				if (e.type == SDL_QUIT)
					quit = true;
			}

			app->update();
			app->render();

			std::this_thread::sleep_until(end_time);
		}
	}

} // namespace ctl::sdl
