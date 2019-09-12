#pragma once

#include "SDLWindow.h"

namespace ctl
{
	class Frame
	{
	public:
		Frame(const Dim&);

		virtual void draw();

	private:
		Dim m_size;
	};

	class Gui
	{
	public:
		Gui(const Dim&);

		void draw();

		Gui& addFrame(Frame*, const Point&);
		Gui& moveFrame();
		Gui& delFrame();

	private:
		Dim m_grid;
		std::vector<Frame*> m_elements;
	};

	Frame::Frame(const Dim& dim)
		: m_size(dim)
	{
	}

	Gui::Gui(const Dim& grid)
		: m_grid(grid)
	{
	}

	void Gui::draw()
	{
		for (auto& i : m_elements)
			i->draw();
	}

	Gui& Gui::addFrame(Frame* f, const Point& pos)
	{
		m_elements.emplace_back(f);
	}
}