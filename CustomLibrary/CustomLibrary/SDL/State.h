#pragma once

#include <SDL.h> 

namespace ctl
{
	class StateBase
	{
	public:
		virtual void event(const SDL_Event&) = 0;
		virtual void input(const SDL_Event&) = 0;
		virtual void update() = 0;
		virtual void fixedUpdate() = 0;
		virtual void render() = 0;
	};
}