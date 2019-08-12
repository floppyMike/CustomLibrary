#include "CustomLibrary\SDL\Engine.h"

ctl::SDL::SDL(const size_t& fps, const Uint32& SDLFlags)
	: m_frameTime(1000 / fps)
{
	if (SDL_Init(SDLFlags) < 0)
		throw Log(SDL_GetError());
}

ctl::SDL::~SDL()
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
