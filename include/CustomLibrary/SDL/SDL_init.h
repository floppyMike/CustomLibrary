#pragma once

#include <SDL2/SDL.h>

#if defined SDL_h_
#define _SDL2_
#endif

#if defined SDL_IMAGE_H_
#define _SDL2IMG_
#endif

#if defined SDL_MIXER_H_
#define _SDL2MIX_
#endif

#if defined SDL_TTF_H_
#define _SDL2TTF_
#endif