#if not defined _CTL_SDL2_Text_
#define _CTL_SDL2_Text_

#include <SDL_ttf.h>

#include "SDL_init.h"

#include "../BasicTypes.h"
#include "../Error.h"

#include "Texture.h"

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

} // namespace ctl::sdl

#endif // !_CTL_SDL2_Text_
