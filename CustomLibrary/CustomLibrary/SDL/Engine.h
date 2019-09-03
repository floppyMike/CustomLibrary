#pragma once

#include <SDL.h>

#include <type_traits>
#include <string>

#include <CustomLibrary/Timer.h>
#include <CustomLibrary/Error.h>

#include "Event.h"
#include "Window.h"

namespace ctl
{
	class SDL
	{
	public:
		/**
		* @summary initialize SDL and construct the engine
		* @param "fps" global application frames / second
		* @param "SDLFlags" flags for SDL creation
		* @exception "Log" if initialization fails
		* @remarks Flags: https://wiki.libsdl.org/SDL_Init#Remarks
		*/
		SDL(const Uint32& SDLFlags = SDL_INIT_VIDEO);

		/**
		* @summary frees the subsystems
		*/
		virtual ~SDL()
		{
#ifdef SDL_MIXER_H_
			Mix_Quit();
#endif //SDL_MIXER_H_

#ifdef SDL_TTF_H_
			TTF_Quit();
#endif //_SDL_TTF_H

#ifdef SDL_IMAGE_H_
			IMG_Quit();
#endif //SDL_IMAGE_H_

			SDL_Quit();
		}

#ifdef SDL_IMAGE_H_
		/**
		* @summary init SDL_image
		* @param "flags" flags for initializer
		* @exception "Log" if initialization fails
		*/
		auto& initIMG(const int& flags = IMG_INIT_PNG)
		{
			if ((IMG_Init(flags) & flags) != flags)
				throw Log(SDL_GetError());

			return *this;
		}
#endif //SDL_IMAGE_H_

#ifdef SDL_MIXER_H_
		/**
		* @summary init SDL_mixer
		* @param "feq" frequency
		* @param "format" format
		* @param "channels" channels
		* @param "chunksize" chunksize
		* @exception "Log" if initialization fails
		*/
		auto& initMix(const int& feq = 44100, const Uint16 & format = MIX_DEFAULT_FORMAT, const int& channels = 2, const int& chunksize = 2048)
		{
			if (Mix_OpenAudio(feq, format, channels, chunksize) < 0)
				throw Log(SDL_GetError());

			return *this;
		}
#endif //SDL_MIXER_H_

#ifdef SDL_TTF_H_
		/**
		* @summary init SDL_ttf
		* @exception "Log" if initialization fails
		*/
		auto& initTTF()
		{
			if (TTF_Init() == -1)
				throw Log(SDL_GetError());

			return *this;
		}
#endif //SDL_TTF_H_

		/**
		* @summary sets a hint
		* @param "name" name of hint
		* @param "value" value to set the hint at
		*/
		SDL& setHint(const char* name, const char* value) noexcept;
	};

	template<typename ImplWinDB>
	class RunLoop
	{
		using type = RunLoop<ImplWinDB>;
		using window_t = typename ImplWinDB::window_t;

		template<typename... T>
		void _invoke_(void (window_t::* f)(const T& ...), const T& ... arg);

	public:
		RunLoop(size_t fps)
			: m_frameTime(1000 / fps)
		{
		}

		void run();

		template<typename... T>
		window_t& addWindow(T&& ... arg);
		template<typename... T>
		void removWindow(T&& ... arg);

		constexpr const auto& fps() const noexcept { return m_fps; }
		constexpr const auto& delta() const noexcept { return m_delta; }

	private:
		ImplWinDB m_windows;
		
		double m_fps = 0.;
		double m_delta = 0.;

		std::chrono::milliseconds m_frameTime;
	};


//----------------------------------------------
//Implementation
//----------------------------------------------

	inline ctl::SDL::SDL(const Uint32& SDLFlags)
	{
		if (SDL_Init(SDLFlags) < 0)
			throw Log(SDL_GetError());
	}

	inline SDL& SDL::setHint(const char* name, const char* value) noexcept
	{
		if (!SDL_SetHint(name, value))
			Log::logWrite(std::string("SDL: setHint: ") + name + " failed with value " + value, Log::Sev::WARNING);
	}

	template<typename ImplWinDB>
	inline void RunLoop<ImplWinDB>::run()
	{
		Timer timer;
		timer.start();
		std::chrono::duration<double> lastTime(0);
		std::chrono::duration<double> lag(0);

		unsigned long long frames = 0;

		for (bool quit = false; !quit; ++frames)
		{
			const auto time = timer.ticks<std::chrono::duration<double>>();
			const auto elapsed = time - lastTime;
			lastTime = time;
			lag += elapsed;

			if (time >= std::chrono::seconds(1))
				m_fps = frames / time.count();

			const auto endTime = std::chrono::steady_clock::now() + m_frameTime;

			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				_invoke_(ImplWinDB::event(e));

				if (e.type == SDL_QUIT)
					quit = true;
			}

			m_delta = elapsed.count();
			_invoke_(ImplWinDB::update);

			while (lag >= m_frameTime)
			{
				lag -= m_frameTime;
				_invoke_(ImplWinDB::fixedUpdate);
			}

			_invoke_(ImplWinDB::render);

			std::this_thread::sleep_until(endTime);
		}
	}

	template<typename ImplWinDB>
	template<typename ...T>
	inline void RunLoop<ImplWinDB>::_invoke_(void(window_t::* f)(const T& ...), const T& ...arg)
	{
		for (auto& i : m_windows)
			(i.*f)(arg...);
	}

	template<typename ImplWinDB>
	template<typename ...T>
	inline auto RunLoop<ImplWinDB>::addWindow(T&& ...arg) -> window_t&
	{
		return m_windows.push(std::forward<T>(arg)...);
	}

	template<typename ImplWinDB>
	template<typename ...T>
	inline void RunLoop<ImplWinDB>::removWindow(T&& ...arg)
	{
		m_windows.erase(std::forward<T>(arg)...);
	}

}