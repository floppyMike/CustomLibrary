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
	class UserEvent
	{
	public:
		UserEvent()
		{
			SDL_zero(m_event);

			m_event.type = SDL_RegisterEvents(1);
			if (m_event.type == std::numeric_limits<Uint32>::max())
				throw Log("UserEvent: event not registered.");
		}

		void pushEvent() noexcept
		{
			if (SDL_PushEvent(&m_event) < 0)
				Log::logWrite(SDL_GetError(), Log::Sev::WARNING);
		}

		constexpr const auto& type() const noexcept { return m_event.type; }

		constexpr const auto& code() const noexcept { return m_event.user.code; }
		constexpr auto& code(const Sint32& code) noexcept 
		{ 
			m_event.user.code = code; 
			return *this; 
		}

		constexpr const auto& data() const noexcept { return std::make_pair(m_event.user.data1, m_event.user.data2); }
		constexpr auto& data(void* data1, void* data2) noexcept 
		{ 
			m_event.user.data1 = data1;
			m_event.user.data2 = data2;
			return *this; 
		}

	private:
		SDL_Event m_event;
	};

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
		SDL(const size_t& fps, const Uint32& SDLFlags = SDL_INIT_VIDEO);

		/**
		* @summary frees the subsystems
		*/
		virtual ~SDL();

		/**
		* @summary wraps resource creation
		* @tparam "Result" expected result type
		* @param "c" resource creation function
		* @param "args" arguments for resource creator
		* @exception "Log" if resource creation fails
		* @remarks function result must be convertible to "Result"
		* @returns Result
		*/
		template<typename Result, typename Creator, typename... Arguments>
		static auto makeResource(Creator c, Arguments&& ... args)
		{
			auto r = c(std::forward<Arguments>(args)...);

			if (!r) 
				throw Log(SDL_GetError());

			return Result(r);
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
		auto& initMix(const int& feq = 44100, const Uint16& format = MIX_DEFAULT_FORMAT, const int& channels = 2, const int& chunksize = 2048)
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
		static void setHint(const char* name, const char* value) noexcept
		{
			if (!SDL_SetHint(name, value))
				Log::logWrite(std::string("SDL: setHint: ") + name + " failed with value " + value, Log::Sev::WARNING);
		}

		/**
		* @summary run the engine
		*/
		void run()
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

				while (lag >= m_frameTime)
				{
					lag -= m_frameTime;
					for (auto& i : m_windows)
						i->_invoke_(&StateBase::fixedUpdate);
				}

				for (auto& i : m_windows)
					i->_render_();

				std::this_thread::sleep_until(endTime);
			}
		}

		/**
		* @summary construct a window
		* @tparam "WindowType" window type to construct
		* @param "argv" parameters of specified window constructor
		*/
		template<typename WindowType, typename... Args>
		auto& addWin(Args&&... argv)
		{
			m_windows.emplace_back(std::make_unique<WindowType>(std::forward<Args>(argv)...));
			m_event.attach(m_windows.back().get());

			return *this;
		}
		
		/**
		* @summary destroy a window
		* @param "winID" window id to destroy
		*/
		auto& detachWin(const Uint32& winID)
		{
			const auto winPtr = std::find_if(m_windows.begin(), m_windows.end(),
				[&winID](std::unique_ptr<WindowBase>& ptr) { return ptr->ID() == winID; });

			m_windows.erase(winPtr);
			m_event.detach(winPtr->get());

			return *this;
		}

		/**
		* @summary access const delta
		* @returns delta const reference
		*/
		constexpr const auto& delta() const noexcept { return m_delta; }

		/**
		* @summary access const frames / second
		* @returns fps const reference
		*/
		constexpr const auto& FPS() const noexcept { return m_fps; }

	private:
		ObSu<SDL_Event>::Subject m_event;
		std::vector<std::unique_ptr<WindowBase>> m_windows;

		double m_fps = 0.;
		double m_delta = 0.;

		std::chrono::milliseconds m_frameTime;
	};
}