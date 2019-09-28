#include "Globals.h"

using namespace ctl;

struct State : public sdl::IState
{
	State(ctl::sdl::Renderer* r)
		: m_rend(r)
		, m_r(m_rend)
		, m_l(m_rend)
		, m_c(m_rend)
		, m_p(m_rend)
		, m_multi(m_rend)
		, m_texture(m_rend)
	{
		m_r.shape({ 10, 10, 40, 40 });
		m_l.shape({ 100, 100, 500, 400 });
		m_c.shape({ 200, 100, 40 });
		m_p.shape({ 200, 100 });

		m_multi.push(ctl::sdl::Rect<int, int>(400, 400, 50, 50));
		m_multi.push(ctl::sdl::Rect<int, int>(400, 300, 50, 50));

		m_multi.push(ctl::sdl::Line<int>(400, 300, 50, 50));
		m_multi.push(ctl::sdl::Line<int>(400, 300, 449, 449));
		m_multi.push(ctl::sdl::Line<int>(50, 50, 449, 449));

		m_multi.push(ctl::sdl::Point<int>(20, 400));
		m_multi.push(ctl::sdl::Point<int>(20, 401));
		m_multi.push(ctl::sdl::Point<int>(20, 402));
		m_multi.push(ctl::sdl::Point<int>(21, 401));
		m_multi.push(ctl::sdl::Point<int>(19, 401));

		m_texture.load("assets/ass.png");
		m_texture.shape({ 200, 20, m_texture.shape().w >> 2, m_texture.shape().h >> 2 });

		m_font.load("assets/ass1.ttf", 40);
		//m_text.loadBlended("Hello There!");

		//ctl::sdl::FontLoad<>().set(&m_font).load("assets/ass1.ttf", 40);
		//ctl::sdl::TexLoad<>(m_rend).set(&m_text).load(
		//	ctl::sdl::Surface(
		//		ctl::sdl::TextLoad<>().set(&m_font).loadBlended("Hello There!")));
		//m_tRend.set(&m_text).shape({ 10, 200, m_tRend.shape().w, m_tRend.shape().h });
	}

	void event(const SDL_Event&) override
	{

	}
	void input(const SDL_Event&) override
	{

	}
	void update() override
	{

	}
	void fixedUpdate() override
	{

	}
	void draw() override
	{
		m_rend->setColor({ 0, 0, 0, 0xFF });
		m_r.drawFilled();
		m_l.draw();
		m_c.draw();
		m_p.draw();

		m_rend->setColor({ 0, 0, 0xFF, 0xFF });
		m_multi.draw();

		m_texture.draw();
		//m_tRend.draw();
	}

private:
	ctl::sdl::Renderer* m_rend;

	ctl::sdl::RectDraw<> m_r;
	ctl::sdl::LineDraw<> m_l;
	ctl::sdl::CircleDraw<> m_c;
	ctl::sdl::PointDraw<> m_p;

	ctl::sdl::MultiShape<ctl::sdl::Renderer,
		ctl::sdl::Rect<int, int>,
		ctl::sdl::Line<int>,
		ctl::sdl::Point<int>> m_multi;

	sdl::Texture<sdl::TexLoad, sdl::TexRend> m_texture;

	sdl::Font<sdl::FontLoad> m_font;
	sdl::Texture<sdl::TexRend, sdl::TextLoad> m_text;
	//ctl::sdl::Texture m_text;
	//ctl::sdl::TexRend<> m_tRend;
};


int main(int argc, char** argv)
{
	try
	{
		sdl::SDL sdl;
		sdl.initIMG(IMG_INIT_JPG | IMG_INIT_PNG)
			.initTTF();

		sdl::SDLWindow win("Test", { 640, 490 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		win.setState<State>(&win.renderer());

		sdl::RunLoop<> loop;
		loop.addWindow(&win);

		loop.run(30);

	}
	catch (const std::exception & err)
	{
		std::cerr << err.what();
	}


	return 0;
}