#pragma once

#include "../Error.h"
#include "../Timer.h"
#include "Window.h"
#include <SDL.h>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

namespace ctl::sdl
{
	static constexpr SDL_Color BLACK  = { 0, 0, 0, 0xFF };
	static constexpr SDL_Color WHITE  = { 0xFF, 0xFF, 0xFF, 0xFF };
	static constexpr SDL_Color RED	  = { 0xFF, 0, 0, 0xFF };
	static constexpr SDL_Color GREEN  = { 0, 0xFF, 0, 0xFF };
	static constexpr SDL_Color BLUE	  = { 0, 0, 0xFF, 0xFF };
	static constexpr SDL_Color YELLOW = { 0xFF, 0xFF, 0, 0xFF };
	static constexpr SDL_Color ORANGE = { 0xFF, 0xFF >> 1, 0xFF >> 2, 0xFF };

	class SDL
	{
	public:
		SDL(const Uint32 &SDLFlags = SDL_INIT_VIDEO);

		/**
		 * @summary frees the subsystems
		 */
		virtual ~SDL()
		{
#ifdef SDL_MIXER_H_
			Mix_Quit();
#endif // SDL_MIXER_H_

#ifdef SDL_TTF_H_
			TTF_Quit();
#endif //_SDL_TTF_H

#ifdef SDL_IMAGE_H_
			IMG_Quit();
#endif // SDL_IMAGE_H_

			SDL_Quit();
		}

#ifdef SDL_IMAGE_H_
		/**
		 * @summary init SDL_image
		 * @param "flags" flags for initializer
		 * @exception "Log" if initialization fails
		 */
		auto &init_IMG(const int &flags = IMG_INIT_PNG)
		{
			if ((IMG_Init(flags) & flags) != flags) throw std::runtime_error(SDL_GetError());

			return *this;
		}
#endif // SDL_IMAGE_H_

#ifdef SDL_MIXER_H_
		/**
		 * @summary init SDL_mixer
		 * @param "feq" frequency
		 * @param "format" format
		 * @param "channels" channels
		 * @param "chunksize" chunksize
		 * @exception "Log" if initialization fails
		 */
		auto &init_Mix(const int &feq = 44100, const Uint16 &format = MIX_DEFAULT_FORMAT, const int &channels = 2,
					   const int &chunksize = 2048)
		{
			if (Mix_OpenAudio(feq, format, channels, chunksize) < 0) throw std::runtime_error(SDL_GetError());

			return *this;
		}
#endif // SDL_MIXER_H_

#ifdef SDL_TTF_H_
		/**
		 * @summary init SDL_ttf
		 * @exception "Log" if initialization fails
		 */
		auto &init_TTF()
		{
			if (TTF_Init() == -1) throw std::runtime_error(SDL_GetError());

			return *this;
		}
#endif // SDL_TTF_H_

		/**
		 * @summary sets a hint
		 * @param "name" name of hint
		 * @param "value" value to set the hint at
		 */
		SDL &set_hint(const char *name, const char *value) noexcept;
	};

	class IWindow
	{
	public:
		virtual void pre_pass(){};
		virtual void event(const SDL_Event &){};
		virtual void update(){};
		virtual void fixed_update(){};
		virtual void render(){};
	};

	template<typename ImplWin = IWindow>
	class RunLoop
	{
		template<typename... T>
		void _invoke_(void (ImplWin::*f)(const T &...), const T &... arg);

	public:
		RunLoop() = default;

		void run(size_t fps);

		ImplWin &add_window(ImplWin *win);

		constexpr auto fps() const noexcept { return m_fps; }
		constexpr auto delta() const noexcept { return m_delta; }

	private:
		std::vector<ImplWin *> m_windows;

		double m_fps   = 0.;
		double m_delta = 0.;
	};

	//----------------------------------------------
	// Implementation
	//----------------------------------------------

	inline SDL::SDL(const Uint32 &SDLFlags)
	{
		if (SDL_Init(SDLFlags) < 0) throw std::runtime_error(SDL_GetError());
	}

	inline SDL &SDL::set_hint(const char *name, const char *value) noexcept
	{
		if (!SDL_SetHint(name, value))
			err::g_log.write(err::Logger::Catagory::WARN)
				<< "SDL: set_hint: " << name << " failed with value " << value;

		return *this;
	}

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

			if (time >= std::chrono::seconds(1)) m_fps = frames / time.count();

			_invoke_(&ImplWin::pre_pass);

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				_invoke_(&ImplWin::event, e);
				if (e.type == SDL_QUIT) quit = true;
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
	inline ImplWin &RunLoop<ImplWin>::add_window(ImplWin *win)
	{
		return *m_windows.emplace_back(win);
	}

	// template<typename ImplWin>
	// inline void RunLoop<ImplWin>::removWindow(typename std::vector<ImplWin>::iterator iter)
	//{
	//	m_windows.erase(iter);
	//}

	template<typename ImplWin>
	template<typename... T>
	inline void RunLoop<ImplWin>::_invoke_(void (ImplWin::*f)(const T &...), const T &... arg)
	{
		for (auto &i : m_windows) (i->*f)(arg...);
	}

} // namespace ctl::sdl
