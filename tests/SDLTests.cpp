#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <CustomLibrary/SDL/Engine.h>
#include "CustomLibrary/SDL/SDLWindow.h"
#include "CustomLibrary/SDL/Geometry.h"
#include "CustomLibrary/SDL/Button.h"
#include "CustomLibrary/SDL/Drawable.h"

using namespace ctl;

class CustomWindow : public sdl::IWindow
{
	// Content...
};

TEST_CASE("Startup", "[Start]")
{
	SECTION("Using SDLWindow")
	{
		sdl::SDL sdl;
		sdl.init_IMG().init_Mix().init_TTF();

		sdl::RunLoop<sdl::SDLWindow> loop;

		sdl::SDLWindow win("Test", { 0, 0 });
		REQUIRE(&loop.add_window(&win) == &win);
	}

	SECTION("Using custom Window")
	{
		sdl::SDL sdl;

		sdl::RunLoop<CustomWindow> loop;

		CustomWindow win;
		REQUIRE(&loop.add_window(&win) == &win);
	}
}

template<typename Win>
struct Initer
{
	Initer()
	{
		sdl.init_IMG().init_Mix().init_TTF();
		loop.add_window(&win);
	}

	sdl::SDL		  sdl;
	sdl::RunLoop<Win> loop;
	Win				  win;
};

class Window : public sdl::IWindow
{
public:
	Window()
		: m_win("Test", { 0, 0 })
		, m_rend(&m_win)
	{
	}

protected:
	sdl::Window	  m_win;
	sdl::Renderer m_rend;
};

class Shape_Test : public Window
{
	void test()
	{
		std::vector<SDL_Rect> m_boxes(
			{ SDL_Rect{ 50, 10, 5, 5 }, { 100, 10, 5, 5 }, { 67, 21, 5, 5 }, { 83, 12, 5, 5 } });
		std::vector<mth::Point<int>> m_points(
			{ mth::Point<int>{ 10, 200 }, { 10, 201 }, { 10, 202 }, { 9, 201 }, { 11, 201 } });

		const sdl::RectFrame   m_r({ 10, 10, 40, 40 });
		const sdl::CircleFrame m_c({ 100, 100, 50 });
		const sdl::LineFrame   m_l({ 400, 300, 300, 400 });

		const auto s1 = std::span(m_boxes);
		sdl::draw(&s1, &this->m_rend).rects();
		sdl::draw(&s1, &this->m_rend).filled_rects();

		const auto s2 = std::span(m_points);
		sdl::draw(&s2, &this->m_rend).points();

		sdl::draw(&m_r, &this->m_rend).rect();
		sdl::draw(&m_r, &this->m_rend).filled_rect();

		// ...
	}
};

TEST_CASE("Shape", "[Shape]") { Initer<Shape_Test> init; }