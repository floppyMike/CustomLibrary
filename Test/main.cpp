#include "Globals.h"

using namespace ctl;

template<template<typename, template<typename> class...> class T, typename Shape, template<typename> class... Arg>
constexpr auto _extracted_tag_(const T<Shape, Arg...>&) -> typename Shape::tag;

template<template<typename> class... Ex>
class Test : public Ex<Test<Ex...>>...
{
public:
	Test() = default;

	//template<template<typename> class... T, typename >
	//Test(const Test<T...>& t)
	//{

	//}

	template<template<typename> class... Args>
	Test(Args<Test>&&... arg)
		: Args<Test>(std::forward<Args<Test>>(arg))...
	{
	}

private:

};

template<typename Impl>
class Ex1 : public crtp<Impl, Ex1>
{
public:
	Ex1() = default;
	Ex1(const Ex1&) = default;

	Ex1(int e)
		: m_hello_there(e)
	{
	}

	void print()
	{
		std::clog << m_hello_there << '\n';
	}

private:
	int m_hello_there = 5;
};

template<typename Impl>
class Ex2 : public crtp<Impl, Ex2>
{
public:
	Ex2() = default;

	void chaka()
	{
		std::cout << "Chaka!\n";
	}
};

template<typename Impl>
class Ex3 : public crtp<Impl, Ex3>
{
public:
	Ex3() = default;

private:
	std::string ohhh = "Hello there";
};


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
		Test<Ex3, Ex1> test(Ex1<Test<Ex3, Ex1>>(10));
		test.print();

		Test<Ex1, Ex2> test1(Ex2<Test<Ex1, Ex2>>(), Ex1<Test<Ex1, Ex2>>(5));
		test1 = static_cast<Ex1<Test<Ex1, Ex2>>>(*reinterpret_cast<Ex1<Test<Ex1, Ex2>>*>(&static_cast<Ex1<Test<Ex3, Ex1>>>(test)));
		test1.print();

		//sdl::SDL sdl;
		//sdl.initIMG(IMG_INIT_JPG | IMG_INIT_PNG)
		//	.initTTF();

		//sdl::SDLWindow win("Test", { 640, 490 }, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		//win.queue_state<State>(&win.renderer());

		//sdl::RunLoop<> loop;
		//loop.add_window(&win);

		//loop.run(30);
	}
	catch (const std::exception & err)
	{
		std::cerr << err.what();
	}


	return 0;
}