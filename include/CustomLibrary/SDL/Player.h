#ifndef _CTL_SDL2_PLAYER_
#define _CTL_SDL2_PLAYER_

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "TypeTraits.h"

namespace ctl::sdl
{
	namespace detail
	{
		template<typename, typename>
		class _Player_
		{
		};

		template<typename Impl>
		class _Player_<Impl, tag::isMusic> : public crtp<Impl, _Player_, tag::isMusic>
		{
		public:
			//(-1 forever)
			void normal(int loop)
			{
				if (!Mix_PlayingMusic())
					Mix_PlayMusic(this->underlying()->obj()->music(), loop);

				else if (Mix_PausedMusic())
					Mix_ResumeMusic();
			}

			void fade(int loop, int ms)
			{
				if (!Mix_PlayingMusic())
					Mix_FadeInMusic(this->underlying()->obj()->music(), loop, ms);

				else if (Mix_PausedMusic())
					Mix_ResumeMusic();
			}
		};

	} // namespace detail

	template<typename T>
	using Player = FunctionalO<T, detail::_Player_>;
} // namespace ctl::sdl

#endif // !_CTL_SDL2_PLAYER_
