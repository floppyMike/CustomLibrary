#include <CustomLibrary/SDL/All.h>
#include <string>

using namespace ctl;

class App
{
public:
	App()
		: m_win("", { 640, 480 })
		, m_rend(&m_win, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)
	{
	}

	void pre_pass() { m_fps.update(); }
	void event(const SDL_Event &e) {}
	void update() { m_win.window_title(std::to_string(static_cast<int>(m_fps.fps())).c_str()); }
	void render()
	{
		sdl::fill(m_rend, sdl::WHITE);

		sdl::render(m_rend);
	}

private:
	sdl::Window	  m_win;
	sdl::Renderer m_rend;

	sdl::FpsCounter m_fps;
};

auto main() -> int
{
	sdl::SDL s;

	App application;

	sdl::RunLoop<App> r(&application);
	r.run(60);

	return 0;
}