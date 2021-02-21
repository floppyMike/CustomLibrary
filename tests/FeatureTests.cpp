
#include <iostream>
#include <CustomLibrary/SDL/All.h>
#include <string>

using namespace ctl;

struct Renderer
{
	sdl::Renderer rend;
	bool		  do_render = true;
};

class App
{
public:
	App()
		: m_win("", { 640, 480 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
		, m_rend{ .rend = sdl::Renderer{ &m_win, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC } }
	{
	}

	void pre_pass() { m_fps.update(); }
	void event(const SDL_Event &e)
	{
		switch (e.type)
		{
		case SDL_MOUSEWHEEL:
		{
			m_cam.zoom(1.f + e.wheel.y * .1f, sdl::mouse_position());
			m_rend.do_render = true;
			break;
		}

		case SDL_MOUSEBUTTONDOWN: m_pressed = true; break;

		case SDL_MOUSEBUTTONUP: m_pressed = false; break;

		case SDL_MOUSEMOTION:
			if (m_pressed)
			{
				m_cam.translate(-e.motion.xrel, -e.motion.yrel);
				m_rend.do_render = true;
			}
			break;

		case SDL_WINDOWEVENT: m_rend.do_render = true; break;

		}
	}
	void update() { m_win.window_title(std::to_string(m_fps.fps()).c_str()); }
	void render()
	{
		if (!m_rend.do_render)
			return;

		m_rend.do_render = false;

		sdl::fill(m_rend.rend, sdl::BLACK);

		m_rend.rend.color(sdl::BLUE);
		sdl::draw(m_rend.rend, m_cam.world_screen(m_rect1));
		sdl::draw_filled(m_rend.rend, m_cam.world_screen(m_rect2));
		sdl::draw(m_rend.rend, m_cam.world_screen(m_line1));
		sdl::draw(m_rend.rend, m_cam.world_screen(m_line2));

		const auto v = sdl::generate_circle(m_cam.world_screen(m_cir1));
		if (!v.empty())
			sdl::draw(m_rend.rend, std::span(v));

		sdl::render(m_rend.rend);
	}

private:
	sdl::Window m_win;
	Renderer	m_rend;

	sdl::FpsCounter m_fps;

	sdl::Camera2D m_cam		= { 0, 0 };
	bool		  m_pressed = false;

	mth::Rect<int, int>	  m_rect1 = { 100, 100, 100, 100 };
	mth::Circle<int, int> m_cir1  = { 300, 150, 50 };
	mth::Rect<int, int>	  m_rect2 = { 400, 100, 100, 100 };
	mth::Line<int>		  m_line1 = { 550, 100, 650, 200 };
	mth::Line<int>		  m_line2 = { 650, 100, 550, 200 };
};

auto main() -> int
{
	sdl::SDL s;

	App application;

	sdl::RunLoop<App> r(&application);
	r.run(61);

	return 0;
}