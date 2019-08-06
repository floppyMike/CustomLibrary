#pragma once

#include "Engine.h"
#include "Window.h"

namespace ctl
{
	class StateBase
	{
	public:
		//----------------------------------------------------
		//---------------------Virtuals-----------------------
		//----------------------------------------------------

		virtual void event(const SDL_Event &e) = 0;
		virtual void input(const SDL_Event &e) = 0;
		virtual void update() = 0;
		virtual void fixedUpdate() = 0;
		virtual void render() = 0;
	};
}