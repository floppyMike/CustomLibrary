#include <iostream>

#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/Window.h>
#include <CustomLibrary/SDL/Renderer.h>
#include <CustomLibrary/SDL/Drawable.h>

#include <CustomLibrary/RandomGenerator.h>

using namespace ctl;

rnd::Random<rnd::Linear>	   g_rand;
static constexpr mth::Dim<int> START_DIM = { 800, 800 };

class Stars
{
public:
	Stars() { generate(m_star_amount); }

	auto regenerate(const mth::Dim<int> &dim) -> void
	{
		m_dimension = dim;
		for (auto &dot : m_coords)
			dot = { (float)g_rand.rand_number(0, m_dimension.w), (float)g_rand.rand_number(0, m_dimension.h) };
	}

	auto generate(size_t n) -> void
	{
		m_star_amount = n;

		if (n < m_coords.size())
			m_coords.erase(m_coords.begin() + n, m_coords.end());
		else if (n > m_coords.size())
		{
			m_coords.reserve(n);
			for (size_t i = m_coords.size(); i < n; ++i)
				m_coords.emplace_back(g_rand.rand_number(0, m_dimension.w), g_rand.rand_number(0, m_dimension.h));
		}
	}

	friend auto operator<<(sdl::Renderer &r, const Stars &ss) -> sdl::Renderer &
	{
		r.color({ 0xFF, 0xFF, 0xFF, 0xFF });

		const std::span s(ss.m_coords);
		sdl::draw(&s, &r).points();

		return r;
	}

	auto translate_amount(int n)
	{
		const long r = m_star_amount + n;

		if (r < 0)
		{
			m_star_amount = 0;
			std::cerr << "Minimum size reached." << std::endl;
		}
		else if (r > std::numeric_limits<size_t>::max())
		{
			m_star_amount = -1;
			std::cerr << "Maximum size reached." << std::endl;
		}
		else
			m_star_amount = r;

		return m_star_amount;
	}

private:
	mth::Dim<int>				   m_dimension	 = START_DIM;
	size_t						   m_star_amount = 1000;
	std::vector<mth::Point<float>> m_coords;
};

class App
{
public:
	static constexpr SDL_Color BG = sdl::BLACK;

	App()
		: m_win("Starfield", START_DIM, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
		, m_r(&m_win)
	{
		m_r.blend_mode(SDL_BLENDMODE_BLEND);
	}

	void pre_pass() {}
	void event(const SDL_Event &e)
	{
		switch (e.type)
		{
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				m_s.regenerate({ e.window.data1, e.window.data2 }), m_r.do_render(true);
			break;

		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_UP)
				m_s.generate(m_s.translate_amount(100));
			else if (e.key.keysym.sym == SDLK_DOWN)
				m_s.generate(m_s.translate_amount(-100));

			m_r.do_render(true);
			break;
		}
	};
	void update(){};
	void fixed_update(){};
	void render()
	{
		if (m_r.will_render())
		{
			auto r = sdl::render(&m_r);
			r.fill(sdl::BLACK);
			m_r << m_s;
			r.locking_render();
		}
	}

private:
	sdl::Window					m_win;
	sdl::Delayed<sdl::Renderer> m_r;

	Stars m_s;
};

auto main(int argc, char **argv) -> int
{
	sdl::SDL		  s;
	App				  a;
	sdl::RunLoop<App> r;

	r.add_window(&a);
	r.run(60);

	return 0;
}