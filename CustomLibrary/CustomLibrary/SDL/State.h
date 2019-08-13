#pragma once

#include <SDL.h> 

namespace ctl
{
	class StateBase
	{
	public:
		/**
		* @summary virtual interface functions
		*/
		virtual void event(const SDL_Event &e) = 0;
		virtual void input(const SDL_Event &e) = 0;
		virtual void update() = 0;
		virtual void fixedUpdate() = 0;
		virtual void render() = 0;
	};
}