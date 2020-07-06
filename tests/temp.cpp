/*Ideas:
- Event binders extension
- Utility system for single function like getMousePos
- Generate Circle extension
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <CustomLibrary/SDL/Texture.h>
#include <CustomLibrary/SDL/Animation.h>
#include <CustomLibrary/SDL/Button.h>
#include <CustomLibrary/SDL/Drawable.h>
#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/Geometry.h>
#include <CustomLibrary/SDL/Loader.h>
#include <CustomLibrary/SDL/Music.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/State.h>
#include <CustomLibrary/SDL/Text.h>

#include <chrono>
#include <iostream>
#include <tuple>

using namespace ctl;
using namespace std::chrono_literals;

struct State : sdl::IState
{
	State(sdl::LDelayedRenderer<sdl::Renderer> *r)
		: m_rend(r)
		, m_r({ 10, 10, 40, 40 })
		, m_c({ 100, 100, 50 })
		, m_l({ 400, 300, 300, 400 })
	{
		m_t.load(m_rend).file("assets/ass.png");
		m_ani.load(m_rend).file("assets/llama.png");

		m_t.shape(mth::Rect(300, 0, m_t.shape().w >> 2, m_t.shape().h >> 2));

		sdl::Font f;
		f.load().file("assets/ass1.ttf", 40);

		m_text.load(m_rend).blended(f.font(), "Hello There!", sdl::BLUE);
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

		constexpr size_t LLAMA = 48;
		m_ani.shape(mth::Rect<int, int>(500, 300, LLAMA * 2, LLAMA * 2));
		for (size_t y = 0; y < 3; ++y)
			for (size_t x = 0; x < 2; ++x)
				m_ani.push_frame({ mth::Rect<int, int>(x * LLAMA, y * LLAMA, LLAMA, LLAMA), 100ms });
		m_ani.start_ani();

		m_c.func([] { std::cout << "Hello There!\n"; });

		m_song.load().file("assets/x.wav");
		m_song.play().normal(-1);
		sdl::Music::volume(10);
	}

	void event(const SDL_Event &e) override {}
	void input(const SDL_Event &e) override { m_c.input(e); }
	void update() override { m_rend->do_render(true); }
	void fixed_update() override {}
	void draw() override
	{
		m_rend->option().color(sdl::BLACK);

		m_ani.draw(m_rend).animated();

		m_c.draw(m_rend).filled_circle();
		m_r.draw(m_rend).rect();
		m_l.draw(m_rend).line();

		m_t.draw(m_rend).texture();
		m_text.draw(m_rend).texture();

		m_multi.draw(m_rend).all();
	}

private:
	sdl::LDelayedRenderer<sdl::Renderer> *m_rend;

	sdl::ERectFrame<sdl::Drawable>				   m_r;
	sdl::ECircleFrame<sdl::Drawable, sdl::LButton> m_c;
	sdl::ELineFrame<sdl::Drawable>				   m_l;

	sdl::Texture m_t;
	sdl::Text	 m_text;

	sdl::Animation m_ani;

	sdl::Music m_song;

	sdl::MultiShape<mth::Rect<int, int>, mth::Line<int>, mth::Point<int>> m_multi;
};

int main(int argc, char **argv)
{
	try
	{
		sdl::SDL sdl;
		sdl.init_IMG(IMG_INIT_JPG | IMG_INIT_PNG).init_TTF().init_Mix();

		sdl::SDLWindow win("Test", { 640, 490 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		win.queue_state<State>(&win.renderer());

		// Window win;

		sdl::RunLoop<sdl::IWindow> loop;
		loop.add_window(&win);

		loop.run(30);
	}
	catch (const std::exception &err)
	{
		std::cerr << err.what();
	}

	return 0;
}
