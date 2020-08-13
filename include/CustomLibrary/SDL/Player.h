#ifndef _CTL_SDL2_PLAYER_
#define _CTL_SDL2_PLAYER_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "TypeTraits.h"

namespace ctl::sdl
{
	// -----------------------------------------------------------------------------
	// Predefinitions
	// -----------------------------------------------------------------------------

	class Music;

	// -----------------------------------------------------------------------------
	// Player Implementation
	// -----------------------------------------------------------------------------

	namespace detail
	{
		template<typename, typename, typename>
		class _Player_;

		/**
		 * @brief Handles music playing
		 */
		template<typename Full, typename Impl>
		class _Player_<Music, Full, Impl> : public Impl
		{
		public:
			using Impl::Impl;

			/**
			 * @brief Plays the music
			 * @param loop How many times the music is played. -1 is looped forever.
			 */
			void normal(int loop)
			{
				if (Mix_PlayingMusic() == 0)
					Mix_PlayMusic(this->obj()->music(), loop);

				else if (Mix_PausedMusic() != 0)
					Mix_ResumeMusic();
			}

			/**
			 * @brief Plays the music faded
			 *
			 * @param loop How many times the music is played. -1 is looped forever.
			 * @param ms Fade time
			 */
			void fade(int loop, int ms)
			{
				if (Mix_PlayingMusic() == 0)
					Mix_FadeInMusic(this->underlying()->obj()->music(), loop, ms);

				else if (Mix_PausedMusic() != 0)
					Mix_ResumeMusic();
			}
		};

	} // namespace detail

	// -----------------------------------------------------------------------------
	// Playing Exension
	// -----------------------------------------------------------------------------

	/**
	 * @brief Type for playing type construction
	 * @tparam T Object to play for type
	 */
	template<typename T>
	using Player = typename Filter<detail::_Player_, Functor<T>, T>::type;

	/**
	 * @brief Shows playing options for object
	 *
	 * @param ptr ptr to object
	 * @return Play type for further options
	 */
	template<typename _T>
	auto play(_T* const ptr)
	{
		return Player<_T>(ptr);
	}

} // namespace ctl::sdl

#endif // !_CTL_SDL2_PLAYER_
