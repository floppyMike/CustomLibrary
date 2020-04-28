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
	State(sdl::DelayedRenderer<sdl::Renderer>* r)
		: m_rend(r)
		, m_r({ 10, 10, 40, 40 })
		, m_c({ 100, 100, 50 })
		, m_l({ 400, 300, 300, 400 })
	{
		sdl::Load<decltype(m_t)>(&m_t, m_rend).load_texture("assets/ass.png");
		m_t.shape(mth::Rect(300, 0, m_t.shape().w >> 2, m_t.shape().h >> 2));

		sdl::Font f;
		sdl::Load<decltype(f)>(&f).load_font("assets/ass1.ttf", 40);

		sdl::Load<decltype(m_text)>(&m_text, m_rend).font(f.font()).load_blended("Hello There!", sdl::BLUE);
		m_text.shape().pos({ 300, 300 });

		m_multi.push(mth::Rect(400, 400, 50, 50));
		m_multi.push(mth::Rect(400, 300, 50, 50));

		m_multi.push(mth::Line(400, 300, 50, 50));
		m_multi.push(mth::Line(400, 300, 449, 449));
		m_multi.push(mth::Line(50, 50, 449, 449));

		m_multi.push(mth::Point(20, 400));
		m_multi.push(mth::Point(20, 401));
		m_multi.push(mth::Point(20, 402));
		m_multi.push(mth::Point(21, 401));
		m_multi.push(mth::Point(19, 401));
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
		sdl::Draw<decltype(m_c)>(&m_c, m_rend).color(sdl::BLACK).draw_filled_circle();
		sdl::Draw<decltype(m_r)>(&m_r, m_rend).draw_rect();
		sdl::Draw<decltype(m_l)>(&m_l, m_rend).draw_line();

		sdl::Draw<decltype(m_t)>(&m_t, m_rend).draw_texture()
			.obj(&m_text).draw_texture();

		sdl::Draw<decltype(m_multi)>(&m_multi, m_rend).draw_all();
	}

private:
	sdl::DelayedRenderer<sdl::Renderer>* m_rend;

	sdl::RectFrame m_r;
	sdl::CircleFrame m_c;
	sdl::LineFrame m_l;

	sdl::Texture m_t;
	sdl::Text m_text;

	sdl::MultiShape<mth::Rect<int, int>,
		mth::Line<int>,
		mth::Point<int>> m_multi;
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