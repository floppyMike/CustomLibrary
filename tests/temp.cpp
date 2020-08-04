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
	explicit State(sdl::Delayed<sdl::Renderer> *r)
		: m_rend(r)
		, m_r({ 10, 10, 40, 40 })
		, m_c({ 100, 100, 50 })
		, m_l({ 400, 300, 300, 400 })
	{
		// Create Texture from file
		sdl::load(&m_t, m_rend).file("assets/ass.png");
		m_t.shape(mth::Rect(300, 0, m_t.shape().w >> 2, m_t.shape().h >> 2));

		// Create Text from font file
		sdl::Font f;
		sdl::load(&f, m_rend).file("assets/ass1.ttf", 40);
		sdl::load(&m_text, m_rend).blended(f.font(), "Hello There!", sdl::BLUE);
		m_text.shape().pos({ 300, 300 });

		// Create Animation using a sprite sheet
		sdl::load(&m_ani, m_rend).file("assets/llama.png");
		constexpr size_t LLAMA = 48;
		m_ani.shape(mth::Rect<int, int>(500, 300, LLAMA * 2, LLAMA * 2));
		for (size_t y = 0; y < 3; ++y)
			for (size_t x = 0; x < 2; ++x)
				m_ani.push_back_frame({ mth::Rect<int, int>(x * LLAMA, y * LLAMA, LLAMA, LLAMA), 100ms });
		m_ani.start_ani();

		// Set the Button
		m_c.func([] { std::cout << "Hello There!\n"; });

		// Load music from a wav file
		sdl::load(&m_song, m_rend).file("assets/x.wav");
		sdl::play(&m_song).normal(-1);
		sdl::Music::volume(10);

		// Load multiple shapes
		m_boxes.assign({ SDL_Rect{ 50, 10, 5, 5 }, { 100, 10, 5, 5 }, { 67, 21, 5, 5 }, { 83, 12, 5, 5 } });

		// Load multiple points
		m_points.assign({ mth::Point<int>{ 10, 200 }, { 10, 201 }, { 10, 202 }, { 9, 201 }, { 11, 201 } });
	}

	void event(const SDL_Event &e) override {}
	void input(const SDL_Event &e) override { m_c.input(e); }
	void update() override { m_rend->do_render(true); }
	void fixed_update() override {}
	void draw() override
	{
		sdl::draw(&m_ani, m_rend).animated();

		m_rend->color(sdl::GREEN);

		auto r = std::span(m_boxes);
		sdl::draw(&r, m_rend).rects();

		m_rend->color(sdl::BLUE);

		auto p = std::span(m_points);
		sdl::draw(&p, m_rend).points();

		m_rend->color(sdl::BLACK);

		sdl::draw(&m_c, m_rend).generate().filled_circle();
		sdl::draw(&m_r, m_rend).rect();
		sdl::draw(&m_l, m_rend).line();

		sdl::draw(&m_t, m_rend).texture();
		sdl::draw(&m_text, m_rend).texture();
	}

private:
	sdl::Delayed<sdl::Renderer> *m_rend;

	std::vector<SDL_Rect>		 m_boxes;
	std::vector<mth::Point<int>> m_points;

	sdl::RectFrame					  m_r;
	sdl::ECircleFrame<sdl::Pressable> m_c;
	sdl::LineFrame					  m_l;

	sdl::Texture m_t;
	sdl::Text	 m_text;

	sdl::Animation m_ani;

	sdl::Music m_song;
};

auto main(int argc, char **argv) -> int
{
	// try
	//{
	sdl::SDL sdl;
	sdl.init_IMG(IMG_INIT_JPG | IMG_INIT_PNG).init_TTF().init_Mix();

	sdl::SDLWindow win("Test", { 640, 490 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	win.queue_state<State>(&win.renderer());

	// Window win;

	sdl::RunLoop<sdl::IWindow> loop;
	loop.add_window(&win);

	loop.run(30);
	//}
	// catch (const std::exception &err)
	//{
	//	std::cerr << err.what();
	//}

	return 0;
}
