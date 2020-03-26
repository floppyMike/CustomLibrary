#pragma once

#include "../Tags.h"
#include "../Traits.h"

#include <SDL_mixer.h>

#include <string>
#include <stdexcept>
#include <memory>

namespace ctl::sdl
{
	template<template<typename, typename...> class... Ex>
	class Music : public Ex<Music<Ex...>, tag::isMusic>...
	{
		struct Unique_Destructor { void operator()(Mix_Music* m) { Mix_FreeMusic(m); } };

	public:
		Music() = default;

		Mix_Music* const music() const noexcept { return m_music.get(); }
		Mix_Music* music() noexcept { return m_music.get(); }

		auto& music(Mix_Music* mus) noexcept
		{
			assert(mus && "Mix_Music is a nullptr.");
			m_music.reset(mus);
			return *this;
		}

		static void pause()
		{
			if (!Mix_PausedMusic())
				Mix_PauseMusic();
		}

		static bool is_paused() { return Mix_PausedMusic(); }
		static bool playing() { return Mix_PlayingMusic(); }
		static void stop() { Mix_HaltMusic(); }
		static void stop_fade(int ms) { Mix_FadeOutMusic(ms); }
		static void volume(unsigned char vol) { Mix_VolumeMusic(vol); }
		static void unpause() { Mix_ResumeMusic(); }

	private:
		std::unique_ptr<Mix_Music, Unique_Destructor> m_music = nullptr;
	};

	template<typename Impl, typename... T>
	class EPlayer : public crtp<Impl, EPlayer, T...>
	{
	public:
		EPlayer() = default;

		//(-1 forever)
		void play(const int& loop)
		{
			const Impl* pthis = this->underlying();

			if (!Mix_PlayingMusic())
				Mix_PlayMusic(pthis->music(), loop);

			else if (Mix_PausedMusic())
				Mix_ResumeMusic();
		}

		void play_fade(int loop, int ms)
		{
			const Impl* pthis = this->underlying();

			if (!Mix_PlayingMusic())
				Mix_FadeInMusic(pthis->music(), loop, ms);

			else if (Mix_PausedMusic())
				Mix_ResumeMusic();
		}

	};

}