#include "Globals.h"

using namespace ctl;


struct State : sdl::IState
{
	State(sdl::Renderer* r)
		: m_rend(r)
		, m_r(m_rend, { 10, 10, 40, 40 })
		, m_l(m_rend, { 100, 100, 500, 400 })
		, m_c(m_rend, { 200, 100, 40 })
		, m_p(m_rend, { 200, 100 })
		, m_multi(m_rend)
		, m_texture(m_rend)
		, m_text(m_rend)
	{
		m_multi.push(sdl::Rect<int, int>(400, 400, 50, 50));
		m_multi.push(sdl::Rect<int, int>(400, 300, 50, 50));

		m_multi.push(sdl::Line<int>(400, 300, 50, 50));
		m_multi.push(sdl::Line<int>(400, 300, 449, 449));
		m_multi.push(sdl::Line<int>(50, 50, 449, 449));

		m_multi.push(sdl::Point<int>(20, 400));
		m_multi.push(sdl::Point<int>(20, 401));
		m_multi.push(sdl::Point<int>(20, 402));
		m_multi.push(sdl::Point<int>(21, 401));
		m_multi.push(sdl::Point<int>(19, 401));

		m_texture.load("assets/ass.png");
		m_texture.shape({ 200, 20, m_texture.shape().w >> 2, m_texture.shape().h >> 2 });

		m_font.load("assets/ass1.ttf", 40);
		m_text.font(m_font.get()).load_blended("Hello There!");
		m_text.shape({ 10, 200, m_text.shape().w, m_text.shape().h });

	}

	void event(const SDL_Event& e) override
	{
	}
	void input(const SDL_Event& e) override
	{
	}
	void update() override
	{

	}
	void fixed_update() override
	{

	}
	void draw() override
	{
		m_rend->color({ 0, 0, 0, 0xFF });
		m_r.draw();
		m_l.draw();
		m_c.draw();
		m_p.draw();

		m_rend->color({ 0, 0, 0xFF, 0xFF });
		m_multi.draw();

		m_texture.draw();
		m_text.draw();
	}

private:
	ctl::sdl::Renderer* m_rend;

	sdl::RectFrame<sdl::EDrawable> m_r;
	sdl::CircleFrame<sdl::EDrawable> m_c;
	sdl::LineFrame<sdl::EDrawable> m_l;
	sdl::PointFrame<sdl::EDrawable> m_p;

	std::vector<int> as;
	std::vector<double> ass;

	sdl::MultiShape<sdl::Rect<int, int>,
		sdl::Line<int>,
		sdl::Point<int>> m_multi;

	sdl::Texture m_texture;

	sdl::Font m_font;
	sdl::Text m_text;
};


int main(int argc, char** argv)
{
	try
	{
		//Test<ETest1, ETest2> test1;
		//
		//Test<ETest1> test2(ETest1<Test<ETest1>>(1));
		//test2 = ETest1<Test<ETest1>>(10);

		//Test<ETest2> test3(ETest2<Test<ETest2>>(80));
		//
		//Test<ETest1, ETest2> test3(static_cast<ETest2<Test<ETest1, ETest2>>>(
		//	*reinterpret_cast<ETest2<Test<ETest1, ETest2>>*>(&static_cast<ETest2<Test<ETest2>>>(test2))));

		//Test<ETest1, ETest2> test4(std::move(test3));
		//
		//test4.print_int();

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