#pragma once

#include <SDL.h>

#include <type_traits>
#include <string>
#include <thread>

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
		SDL(const size_t& fps, const Uint32& SDLFlags = SDL_INIT_VIDEO);

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
		static void setHint(const char* name, const char* value) noexcept;

		/**
		* @summary run the engine
		*/
		void run();

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
		auto& detachWin(const Uint32& winID);

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