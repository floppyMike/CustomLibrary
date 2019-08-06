#pragma once

#include <SDL.h>

#include <array>
#include <type_traits>
#include <string>
#include <thread>

#include <CustomLibrary/Timer.h>
#include <CustomLibrary/Error.h>

#include "Window.h"

namespace ctl
{
	class SDL
	{
	public:

		//--------------------------------------------------
		//-------------Constructors & Destructor------------
		//--------------------------------------------------

		//Flags: https://wiki.libsdl.org/SDL_Init#Remarks
		SDL(const size_t &fps, const Uint32& SDL_Flags = SDL_INIT_VIDEO)
			: m_fpsWant(fps)
		{
			if (SDL_Init(SDL_Flags) < 0)
				throw Log(SDL_GetError());
		}

		virtual ~SDL()
		{
			//Quit SDL subsystems
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
		auto& initIMG(const int& flags = IMG_INIT_PNG)
		{
			if ((IMG_Init(flags) & (flags)) != (flags))
				throw Log(SDL_GetError());

			return *this;
		}
#endif //SDL_IMAGE_H_

#ifdef SDL_MIXER_H_
		auto& initMix(const int& feq = 44100, const Uint16& format = MIX_DEFAULT_FORMAT, const int& channels = 2, const int& chunksize = 2048)
		{
			if (Mix_OpenAudio(feq, format, channels, chunksize) < 0)
				throw Log(SDL_GetError());

			return *this;
		}
#endif //SDL_MIXER_H_

#ifdef SDL_TTF_H_
		auto& initTTF()
		{
			if (TTF_Init() == -1)
				throw Log(TTF_GetError());

			return *this;
		}
#endif //SDL_TTF_H_

		auto& setHint(const std::string_view& name, const std::string_view& value)
		{
			if (!SDL_SetHint(name.data(), value.data()))
				Log::note("SDL: setHint: " + std::string(name) + " failed with value " + std::string(value));

			return *this;
		}

		auto& setEventState(const Uint32& type, const char& state)
		{
			SDL_EventState(type, state);

			return *this;
		}

		//--------------------------------------------------
		//--------------------Methods-----------------------
		//--------------------------------------------------

		//Required in main()
		void run()
		{
			const auto &frameTime = std::chrono::milliseconds(1000 / m_fpsWant);

			Timer timer;
			timer.start();
			std::chrono::duration<double> lastTime(0);
			std::chrono::duration<double> lag(0);

			unsigned long long frames = 0;

			for (bool quit = false; !quit; ++frames)
			{
				const auto& time = timer.ticks<std::chrono::duration<double>>();
				const auto& elapsed = time - lastTime;
				lastTime = time;
				lag += elapsed;

				if (time >= std::chrono::seconds(1))
					m_fps = frames / time.count();

				const auto& endTime = std::chrono::steady_clock::now() + frameTime;

				static SDL_Event e;
				while (SDL_PollEvent(&e) != 0)
				{
					m_event.notify(e);

					if (e.type == SDL_QUIT)
						quit = true;
				}

				m_delta = elapsed.count();
				for (auto& i : m_windows)
					i->_invoke_(&StateBase::update);
				while (lag >= frameTime)
				{
					lag -= frameTime;
					for (auto& i : m_windows)
						i->_invoke_(&StateBase::fixedUpdate);
				}

				for (auto& i : m_windows)
					i->_render_();

				std::this_thread::sleep_until(endTime);
			}
		}

		auto& addWin(WindowBase *w)
		{
			m_windows.emplace_back(w);
			m_event.attach(w);

			return *this;
		}
			
		//void detachWin(WindowBase *w)
		//{
		//	m_windows.erase(std::find(m_windows.rbegin(), m_windows.rend(), w).base());
		//	m_event.detach(w);
		//}

		static void pushEvent(SDL_Event* e) noexcept
		{
			if (SDL_PushEvent(e) < 0)
				Log::note(SDL_GetError(), Log::Sev::WARNING);
		}

		static void pushFlag(const Uint32& type, const Uint32& winID, void* data1 = nullptr, void* data2 = nullptr) noexcept
		{
			SDL_Event temp;
			SDL_zero(temp);
			temp.user.type = type;
			temp.window.windowID = winID;
			temp.user.data1 = data1;
			temp.user.data2 = data2;

			if (SDL_PushEvent(&temp) < 0)
				Log::note(SDL_GetError(), Log::Sev::WARNING);
		}

		//----------------------------------------------------
		//--------------------Operators-----------------------
		//----------------------------------------------------

		constexpr const auto& delta() const { return m_delta; }
		constexpr const auto& FPS() const { return m_fps; }

	private:
		ObSu<SDL_Event>::Subject m_event;
		std::vector<WindowBase*> m_windows;

		double m_fps = 0.;
		double m_delta = 0.;

		size_t m_fpsWant;
	};
}