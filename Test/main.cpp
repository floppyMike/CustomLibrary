#include <tuple>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/State.h>
#include <CustomLibrary/SDL/Texture.h>
#include <CustomLibrary/SDL/Text.h>
#include <CustomLibrary/SDL/Geometry.h>
#include <CustomLibrary/SDL/Button.h>
#include <CustomLibrary/SDL/Animation.h>

using namespace std::chrono_literals;

using namespace ctl;

struct State : sdl::IState
{
	State(sdl::Renderer* r)
		: m_rend(r)
		, m_r(m_rend, { 10, 10, 40, 40 })
		, m_c(m_rend, { 200, 100, 40 })
		, m_l(m_rend, { 100, 100, 500, 400 })
		, m_p(m_rend, { 200, 100 })
		, m_multi(m_rend)
		, m_texture(m_rend)
		, m_ani(m_rend)
		, m_text(m_rend)
	{
		m_multi.push(sdl::Rect(400, 400, 50, 50));
		m_multi.push(sdl::Rect(400, 300, 50, 50));

		m_multi.push(sdl::Line(400, 300, 50, 50));
		m_multi.push(sdl::Line(400, 300, 449, 449));
		m_multi.push(sdl::Line(50, 50, 449, 449));

		m_multi.push(sdl::Point(20, 400));
		m_multi.push(sdl::Point(20, 401));
		m_multi.push(sdl::Point(20, 402));
		m_multi.push(sdl::Point(21, 401));
		m_multi.push(sdl::Point(19, 401));

		m_texture.load("assets/ass.png");
		m_texture.shape({ 200, 20, m_texture.shape().w >> 2, m_texture.shape().h >> 2 });

		m_font.load("assets/ass1.ttf", 40);
		m_text.font(m_font.font()).load_blended("Hello There!");
		m_text.shape({ 10, 200, m_text.shape().w, m_text.shape().h });

		m_r.func([] { std::cout << "Button Press!\n"; });

		constexpr size_t LLAMA = 48;
		m_ani.load("assets/llama.png").shape(sdl::Rect<int, int>(500, 300, LLAMA * 2, LLAMA * 2));
		for (size_t y = 0; y < 3; ++y)
			for (size_t x = 0; x < 2; ++x)
				m_ani.push_frame({ sdl::Rect<int, int>(x * LLAMA, y * LLAMA, LLAMA, LLAMA), 100ms });
		m_ani.start_ani();
	}

	void event(const SDL_Event& e) override
	{
		m_r.event(e);
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
		m_rend->color({ 0, 0, 0, 0xFF });
		m_r.draw_filled_rect();
		m_l.draw_line();
		m_c.draw_circle();
		m_p.draw_point();

		m_rend->color({ 0, 0, 0xFF, 0xFF });
		m_multi.draw();

		m_texture.draw();
		m_text.draw();

		m_ani.draw(m_ani.blit_ani().rect_ptr());
	}

private:
	ctl::sdl::Renderer* m_rend;

	sdl::RectFrame<sdl::EDrawable, sdl::EButton> m_r;
	sdl::CircleFrame<sdl::EDrawable> m_c;
	sdl::LineFrame<sdl::EDrawable> m_l;
	sdl::PointFrame<sdl::EDrawable> m_p;

	sdl::MultiShape<sdl::Rect<int, int>,
		sdl::Line<int>,
		sdl::Point<int>> m_multi;

	sdl::Texture m_texture;
	sdl::TextureFrame<sdl::ETextureRender, sdl::ETextureLoader, sdl::EAnimation> m_ani;

	sdl::Font m_font;
	sdl::Text m_text;
};


int main(int argc, char** argv)
{
	try
	{
		sdl::SDL sdl;
		sdl.initIMG(IMG_INIT_JPG | IMG_INIT_PNG)
			.initTTF();

		sdl::SDLWindow win("Test", { 640, 490 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		win.queue_state<State>(&win.renderer());

		sdl::RunLoop<> loop;
		loop.add_window(&win);

		loop.run(30);
	}
	catch (const std::exception & err)
	{
		std::cerr << err.what();
	}


	return 0;
}