#ifndef _CTL_SDL2_MUSIC_
#define _CTL_SDL2_MUSIC_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "../Tags.h"
#include "../Traits.h"

#include <cassert>
#include <memory>

#include "TypeTraits.h"
#include "Loader.h"
#include "Player.h"

namespace ctl::sdl
{
	class Music
	{
		struct Unique_Destructor
		{
			void operator()(Mix_Music *m) { Mix_FreeMusic(m); }
		};

	public:
		using base_t = Music;
		using tag_t	 = tag::isMusic;

		Music() = default;

		Mix_Music *music() const noexcept
		{
			assert(m_music && "Mix_Music is a nullptr.");
			return m_music.get();
		}

		auto &music(Mix_Music *mus) noexcept
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

		auto load() noexcept { return LoadO<std::decay_t<decltype(*this)>>(this); }
		auto play() noexcept { return Player<std::decay_t<decltype(*this)>>(this); }

	private:
		std::unique_ptr<Mix_Music, Unique_Destructor> m_music;
	};

} // namespace ctl::sdl

#endif // !_CTL_SDL2_MUSIC_