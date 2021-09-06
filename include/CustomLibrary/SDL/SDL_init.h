#pragma once

#include "../Traits.h"
#include "../Geometry.h"

#if not defined NDEBUG
#include <SDL.h>
#include <SDL_ttf.h>
#endif

// -----------------------------------------------------------------------------
// Uniform Defines
// -----------------------------------------------------------------------------

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

