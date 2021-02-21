
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
		: m_win("", { 640, 480 })
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
		}
	}
	void update() { m_win.window_title(std::to_string(m_fps.fps()).c_str()); }
	void render()
	{
		if (!m_rend.do_render)
			return;

		m_rend.do_render = false;

		sdl::fill(m_rend.rend, sdl::WHITE);

		const sdl::GenCircle c({ m_cam.world_screen(m_cir1.pos()), m_cir1.r * m_cam.scale() });

		m_rend.rend.color(sdl::BLACK);
		sdl::draw(m_rend.rend, { m_cam.world_screen(m_rect1.pos()), m_cam.world_screen(m_rect1.dim()) });
		sdl::draw_filled(m_rend.rend, { m_cam.world_screen(m_rect2.pos()), m_cam.world_screen(m_rect2.dim()) });
		sdl::draw(m_rend.rend, c);

		sdl::render(m_rend.rend);
	}

private:
	sdl::Window m_win;
	Renderer	m_rend;

	sdl::FpsCounter m_fps;

	sdl::Camera2D m_cam		= { 0, 0 };
	bool		  m_pressed = false;

	mth::Rect<int, int>	  m_rect1 = { 100, 100, 100, 100 };
	mth::Rect<int, int>	  m_rect2 = { 400, 100, 100, 100 };
	mth::Circle<int, int> m_cir1  = { 300, 150, 50 };
};

auto main() -> int
{
	sdl::SDL s;

	App application;

	sdl::RunLoop<App> r(&application);
	r.run(61);

	return 0;
}