#ifndef _CTL_SDL2_MUSIC_
#define _CTL_SDL2_MUSIC_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "../Traits.h"

#include <cassert>
#include <memory>

#include "TypeTraits.h"
#include "Loader.h"
#include "Player.h"

namespace ctl::sdl
{
	/**
	 * @brief Manages music to be played
	 */
	class Music
	{
		struct Unique_Destructor
		{
			void operator()(Mix_Music *m) { Mix_FreeMusic(m); }
		};

	public:
		Music() = default;

		/**
		 * @brief Get the Mix_Music ptr
		 * @return Mix_Music ptr
		 */
		[[nodiscard]] auto music() const noexcept
		{
			assert(m_music && "Mix_Music is a nullptr.");
			return m_music.get();
		}

		/**
		 * @brief Set the music
		 * @param mus Mix_Music ptr
		 */
		auto music(Mix_Music *mus) noexcept { m_music.reset(mus); }

		/**
		 * @brief Pauses the Music
		 */
		static void pause()
		{
			if (Mix_PausedMusic() == 0)
				Mix_PauseMusic();
		}
		/**
		 * @brief Check if music is paused
		 * @return bool
		 */
		static auto is_paused() { return Mix_PausedMusic() != 0; }
		/**
		 * @brief Check if music is playing
		 * @return bool
		 */
		static auto playing() { return Mix_PlayingMusic() != 0; }
		/**
		 * @brief Stop the music
		 */
		static void stop() { Mix_HaltMusic(); }
		/**
		 * @brief Fade out the music
		 * @param ms Time in ms till music fades out
		 */
		static void stop_fade(int ms) { Mix_FadeOutMusic(ms); }
		/**
		 * @brief Sets the volume of the music
		 * @param vol Music volume from 0 till 255
		 */
		static void volume(unsigned char vol) { Mix_VolumeMusic(vol); }
		/**
		 * @brief Resume the music
		 */
		static void unpause() { Mix_ResumeMusic(); }

	private:
		std::unique_ptr<Mix_Music, Unique_Destructor> m_music;
	};

} // namespace ctl::sdl

#endif // !_CTL_SDL2_MUSIC_