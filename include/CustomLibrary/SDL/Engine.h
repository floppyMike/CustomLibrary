#pragma once

#include "SDL_init.h"

#include "../Timer.h"

#include <thread>
#include <vector>
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
			if (SDL_Init(SDLFlags) < 0)
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
		auto init_IMG(int flags = IMG_INIT_PNG) -> auto &
		{
			if ((IMG_Init(flags) & flags) != flags)
				throw std::runtime_error(SDL_GetError());

			return *this;
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
		auto init_Mix(int feq = 44100, Uint16 format = MIX_DEFAULT_FORMAT, int channels = 2, int chunksize = 2048)
			-> auto &
		{
			if (Mix_OpenAudio(feq, format, channels, chunksize) < 0)
				throw std::runtime_error(SDL_GetError());

			return *this;
		}
#endif

#if defined _SDL2TTF_
		/**
		 * @brief init SDL_ttf
		 * @exception "Log" if initialization fails
		 */
		auto init_TTF() -> auto &
		{
			if (TTF_Init() == -1)
				throw std::runtime_error(SDL_GetError());

			return *this;
		}
#endif

		/**
		 * @brief sets a hint
		 * @param "name" name of hint https://wiki.libsdl.org/CategoryHints#Hints
		 * @param "value" value to set the hint at
		 */
		auto set_hint(const char *name, const char *value) noexcept -> SDL &
		{
			if (SDL_SetHint(name, value) == 0U)
				;
			// err::g_log.write(err::Logger::Catagory::WARN)
			//<< "SDL: set_hint: " << name << " failed with value " << value;

			return *this;
		}
	};

	// -----------------------------------------------------------------------------
	// Window Interface
	// -----------------------------------------------------------------------------

	/**
	 * @brief Window Interface for RunLoop
	 */
	class IWindow
	{
	public:
		/**
		 * @brief Takes place when a new iteration begins
		 */
		virtual void pre_pass(){};
		/**
		 * @brief Takes place when a new event is caught
		 * @param e New event
		 */
		virtual void event(const SDL_Event &e){};
		/**
		 * @brief Used for updating your stuff per iteration
		 */
		virtual void update(){};
		/**
		 * @brief Updates stuff like 'update' but it catches up when lag occures
		 */
		virtual void fixed_update(){};
		/**
		 * @brief Used to render the objects drawn to the buffer
		 */
		virtual void render(){};
	};

	// -----------------------------------------------------------------------------
	// Runtime Loops
	// -----------------------------------------------------------------------------

	/**
	 * @brief Simplified SDL2 runtime frame used by the application. Supports only 1 window and has support for window
	 * update, event, render and pre_pass
	 *
	 * @tparam IWindow Window Implementation
	 */
	template<typename ImplWin = IWindow>
	class SimpleRunLoop
	{
	public:
		SimpleRunLoop() = default;

		/**
		 * @brief Initialize the run loop with the window.
		 * @param win Window ptr to use
		 */
		explicit SimpleRunLoop(ImplWin *win)
			: m_win(win)
		{
		}

		/**
		 * @brief Select a window to use.
		 * @param win Window ptr
		 */
		void window(ImplWin *win) { m_win = win; }

		/**
		 * @brief Starts the runloop of the application
		 * @param fps Hz of the amound of frames per second
		 */
		void run(size_t fps)
		{
			const std::chrono::milliseconds frameTime(1000 / fps);

			for (bool quit = false; !quit;)
			{
				const auto endTime = std::chrono::steady_clock::now() + frameTime;

				m_win->pre_pass();

				SDL_Event e;
				while (SDL_PollEvent(&e) != 0)
				{
					m_win->event(e);
					if (e.type == SDL_QUIT)
						quit = true;
				}

				m_win->update();
				m_win->render();

				std::this_thread::sleep_until(endTime);
			}
		}

	private:
		ImplWin *m_win;
	};

	/**
	 * @brief Basic SDL2 runtime frame used by the application
	 * @tparam ImplWin Window type -> Default: IWindow
	 */
	template<typename ImplWin = IWindow>
	class RunLoop
	{
		template<typename... T>
		void _invoke_(void (ImplWin::*f)(const T &...), const T &...arg);

	public:
		RunLoop() = default;

		/**
		 * @brief Starts the runloop of the application
		 * @param fps Hz of the amound of frames per second
		 */
		void run(size_t fps);

		/**
		 * @brief Add a Window to be used
		 *
		 * @param win Window ptr
		 * @return Windows reference
		 */
		auto add_window(ImplWin *win) -> ImplWin &;

		/**
		 * @brief Get the frames per second in Hz
		 * @return double
		 */
		constexpr auto fps() const noexcept { return m_fps; }
		/**
		 * @brief Get the movment delta
		 * This value is used to make object movment in update to be independant of the fps
		 * @return double
		 */
		constexpr auto delta() const noexcept { return m_delta; }

	private:
		std::vector<ImplWin *> m_windows;

		double m_fps   = 0.;
		double m_delta = 0.;
	};

	template<typename ImplWin>
	inline void RunLoop<ImplWin>::run(size_t fps)
	{
		const std::chrono::milliseconds frameTime(1000 / fps);

		Timer timer;
		timer.start();
		std::chrono::duration<double> lastTime(0);
		std::chrono::duration<double> lag(0);

		unsigned long long frames = 0;

		for (bool quit = false; !quit; ++frames)
		{
			const auto endTime = std::chrono::steady_clock::now() + frameTime;

			const auto time	   = timer.ticks<std::chrono::duration<double>>();
			const auto elapsed = time - lastTime;
			lastTime		   = time;
			lag += elapsed;

			if (time >= std::chrono::seconds(1))
				m_fps = frames / time.count();

			_invoke_(&ImplWin::pre_pass);

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				_invoke_(&ImplWin::event, e);
				if (e.type == SDL_QUIT)
					quit = true;
			}

			m_delta = elapsed.count();
			_invoke_(&ImplWin::update);

			while (lag >= frameTime)
			{
				lag -= frameTime;
				_invoke_(&ImplWin::fixed_update);
			}

			_invoke_(&ImplWin::render);

			std::this_thread::sleep_until(endTime);
		}
	}

	template<typename ImplWin>
	inline auto RunLoop<ImplWin>::add_window(ImplWin *win) -> ImplWin &
	{
		return *m_windows.emplace_back(win);
	}

	template<typename ImplWin>
	template<typename... T>
	inline void RunLoop<ImplWin>::_invoke_(void (ImplWin::*f)(const T &...), const T &...arg)
	{
		for (auto &i : m_windows) (i->*f)(arg...);
	}

} // namespace ctl::sdl
