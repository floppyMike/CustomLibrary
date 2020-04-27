/*Ideas:
- Event binders extension
- Utility system for single function like getMousePos
- Generate Circle extension
*/


#include <tuple>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <chrono>

#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/State.h>
#include <CustomLibrary/SDL/Texture.h>
#include <CustomLibrary/SDL/Text.h>
#include <CustomLibrary/SDL/Geometry.h>
#include <CustomLibrary/SDL/Button.h>
#include <CustomLibrary/SDL/Animation.h>
#include <CustomLibrary/SDL/Drawable.h>
#include <CustomLibrary/SDL/Loader.h>
#include <CustomLibrary/SDL/Music.h>

using namespace std::chrono_literals;

using namespace ctl;

struct State : sdl::IState
{
	State(sdl::LDelayedRender<sdl::Renderer>* r)
		: m_rend(r)
		, m_r(m_rend, { 10, 10, 40, 40 })
		, m_c(m_rend, { 100, 100, 50 })
		, m_l(m_rend, { 400, 300, 300, 400 })
		, m_t(m_rend)
	{
		sdl::Load<decltype(m_t)>(&m_t).load_texture("assets/ass.png");
		m_t.shape(mth::Rect(300, 0, m_t.shape().w >> 2, m_t.shape().h >> 2));
	}

	void event(const SDL_Event& e) override
	{
	}
	void input(const SDL_Event& e) override
	{
	}
	void update() override
	{
		m_rend->do_render(true);
	}
	void fixed_update() override
	{

	}
	void draw() override
	{
		sdl::Draw<decltype(m_c)>(&m_c).color(sdl::BLACK).draw_filled_circle();
		sdl::Draw<decltype(m_r)>(&m_r).draw_rect();
		sdl::Draw<decltype(m_l)>(&m_l).draw_line();

		sdl::Draw<decltype(m_t)>(&m_t).draw_texture();
	}

private:
	sdl::LDelayedRender<sdl::Renderer>* m_rend;

	sdl::RectFrame m_r;
	sdl::CircleFrame m_c;
	sdl::LineFrame m_l;

	sdl::Texture m_t;
};


int main(int argc, char** argv)
{
	try
	{
		sdl::SDL sdl;
		sdl.init_IMG(IMG_INIT_JPG | IMG_INIT_PNG)
			.init_TTF()
			.init_Mix();

		sdl::SDLWindow win("Test", { 640, 490 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		win.queue_state<State>(&win.renderer());

		//Window win;

		sdl::RunLoop<sdl::IWindow> loop;
		loop.add_window(&win);

		loop.run(30);
	}
	catch (const std::exception& err)
	{
		std::cerr << err.what();
	}


	return 0;
}