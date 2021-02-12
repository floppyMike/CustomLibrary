#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <CustomLibrary/SDL/All.h>
#include <CustomLibrary/SDL/Scale.h>

using namespace ctl;

class App
{
public:
	App()
		: m_win("Widgets", { 640, 480 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
		, m_rend(&m_win)
	{
		m_rect.relative_size({ 100, 100, 100, 100 }, { 640, 480 });
	}

	void pre_pass() {}

	void event(const SDL_Event &e)
	{
		if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			m_rend.do_render(true);
			m_rect.resize({ e.window.data1, e.window.data2 });
		}
	}

	void update() {}

	void render()
	{
		if (!m_rend.will_render())
			return;

		m_rend.do_render(false);

		auto r = sdl::render(&m_rend);
		r.fill(sdl::BLACK);

		m_rend.color(sdl::ORANGE);
		sdl::draw(&m_rect, &m_rend).filled_rect();

		r.render();
	}

private:
	sdl::Window					m_win;
	sdl::Delayed<sdl::Renderer> m_rend;

	sdl::Scaleable<sdl::RectFrame> m_rect;
};

int main(int argc, char **argv)
{
	sdl::SDL s;
	s.init_IMG();

	App						a;
	sdl::SimpleRunLoop<App> r(&a);
	r.run(60);

	return 0;
}