#if not defined _CTL_SDL2_Text_
#define _CTL_SDL2_Text_

#include <memory>
#include <SDL_ttf.h>

#include "SDL_init.h"

#include <cassert>

namespace ctl::sdl
{
	namespace detail
	{
		struct Font_Destructor
		{
			void operator()(TTF_Font *f)
			{
				TTF_CloseFont(f);
			}
		};
	} // namespace detail

	using Font = std::unique_ptr<TTF_Font, detail::Font_Destructor>;

	/**
	 * @brief Load font from a file
	 *
	 * @param path Path to the file
	 * @param ptsize Pixel size of the font
	 *
	 * @return Font
	 */
	auto load_font(const char *path, int ptsize)
	{
		auto *t = TTF_OpenFont(path, ptsize);

		if (t == nullptr)
			throw std::runtime_error(TTF_GetError());

		return Font(t);
	}

} // namespace ctl::sdl

#endif // !_CTL_SDL2_Text_
