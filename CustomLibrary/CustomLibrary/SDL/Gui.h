#pragma once

#include "SDLWindow.h"

namespace ctl
{
	class Frame
	{
	public:
		Frame(const SDLDim&);

		virtual void draw();

	private:
		SDLDim m_size;
	};

	class Gui
	{
	public:
		Gui(const SDLDim&);

		void draw();

		Gui& addFrame(Frame*, const SDLPoint&);
		Gui& moveFrame();
		Gui& delFrame();

	private:
		SDLDim m_grid;
		std::vector<Frame*> m_elements;
	};

	Frame::Frame(const SDLDim& dim)
		: m_size(dim)
	{
	}

	Gui::Gui(const SDLDim& grid)
		: m_grid(grid)
	{
	}

	void Gui::draw()
	{
		for (auto& i : m_elements)
			i->draw();
	}

	Gui& Gui::addFrame(Frame* f, const SDLPoint& pos)
	{
		m_elements.emplace_back(f);
	}
}