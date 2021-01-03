#include <iostream>

#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/Window.h>
#include <CustomLibrary/SDL/Renderer.h>
#include <CustomLibrary/SDL/Drawable.h>
#include <CustomLibrary/SDL/Event.h>

#include <CustomLibrary/Collider.h>
#include <CustomLibrary/RandomGenerator.h>

using namespace ctl;

static rnd::Random<rnd::Linear> g_rand;
static constexpr mth::Dim<int>	START_DIM = { 800, 800 };

class Field
{
public:
	static constexpr int								DIV		= 25;
	static constexpr std::array<mth::Rect<int, int>, 4> BORDERS = { mth::Rect{ 0, 0, DIV, START_DIM.h },
																	{ DIV, 0, START_DIM.w - 2 * DIV, DIV },
																	{ START_DIM.w - DIV, 0, DIV, START_DIM.h },
																	{ DIV, START_DIM.h - DIV, START_DIM.w - 2 * DIV,
																	  DIV } };
	static constexpr mth::Dim<int>						GRID	= { START_DIM.w / DIV - 2, START_DIM.h / DIV - 2 };
	static constexpr auto								LINES	= []() constexpr
	{
		std::array<mth::Line<int>, GRID.w + GRID.h - 2> arr{};

		auto iter = arr.begin();

		for (int i = 0; i < GRID.w - 1; ++i)
			*iter++ = { BORDERS[0].w + DIV * (i + 1), BORDERS[1].h, BORDERS[0].w + DIV * (i + 1),
						BORDERS[0].h - BORDERS[1].h };

		for (int i = 0; i < GRID.h - 1; ++i)
			*iter++ = { BORDERS[0].w, BORDERS[1].h + DIV * (i + 1), BORDERS[0].w + BORDERS[1].w,
						BORDERS[1].h + DIV * (i + 1) };

		return arr;
	}
	();

	static constexpr SDL_Color BG_COLOR		= { 30, 30, 30, 0xFF };
	static constexpr uint8_t   MOD			= 20;
	static constexpr SDL_Color BORDER_COLOR = { BG_COLOR.r + MOD * 2, BG_COLOR.g + MOD * 2, BG_COLOR.b + MOD * 2,
												0xFF };
	static constexpr SDL_Color LINE_COLOR	= { BG_COLOR.r + MOD, BG_COLOR.g + MOD, BG_COLOR.b + MOD, 0xFF };

	static constexpr auto grid_to_coord(const mth::Point<int> &p) noexcept
	{
		assert(p.x <= GRID.w && p.y <= GRID.h && "Point doesn't match the grid.");
		return mth::Point{ (p.x + 1) * DIV, (p.y + 1) * DIV };
	}

	static constexpr auto coord_to_grid(const mth::Point<int> &p) noexcept
	{
		assert(p.x >= DIV && p.y < BORDERS[1].w + DIV && p.y >= DIV && p.y < BORDERS[0].h - DIV
			   && "Coord not within border.");
		return mth::Point{ p.x / DIV - 1, p.y / DIV - 1 };
	}

	static auto draw_field(sdl::Renderer &r)
	{
		r.color(LINE_COLOR);
		for (const auto &l : LINES) sdl::draw(&l, &r).line();

		r.color(BORDER_COLOR);
		const std::span border(BORDERS);
		sdl::draw(&border, &r).filled_rects();
	}
};

class Snake
{
public:
	enum class Direction
	{
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	explicit Snake(const mth::Point<int> &loc)
		: m_loc(loc)
		, m_body(1, mth::Rect{ Field::grid_to_coord(loc), mth::Dim{ Field::DIV, Field::DIV } })
	{
	}

	auto mov() noexcept
	{
		switch (m_dir)
		{
		case Direction::UP: --m_loc.y; break;
		case Direction::DOWN: ++m_loc.y; break;
		case Direction::LEFT: --m_loc.x; break;
		case Direction::RIGHT: ++m_loc.x; break;
		}

		if (!mth::collision(m_loc, mth::Rect{ 0, 0, Field::GRID.w - 1, Field::GRID.h - 1 }))
			return false;

		m_tail->pos(Field::grid_to_coord(m_loc));

		const auto crash = std::find_if(m_body.begin(), m_body.end(),
										[this](const mth::Rect<int, int> &r) {
											return &r != &*m_tail && m_tail->x == r.x && m_tail->y == r.y;
										})
			== m_body.end();

		if (++m_tail == m_body.rend())
			m_tail = m_body.rbegin();

		return crash;
	}

	auto direction(Direction d) noexcept
	{
		switch (m_dir)
		{
		case Direction::UP:
			if (d != Direction::DOWN)
				m_dir = d;
			break;
		case Direction::DOWN:
			if (d != Direction::UP)
				m_dir = d;
			break;
		case Direction::LEFT:
			if (d != Direction::RIGHT)
				m_dir = d;
			break;
		case Direction::RIGHT:
			if (d != Direction::LEFT)
				m_dir = d;
			break;
		}
	}

	auto increase_size() { m_tail = std::reverse_iterator(m_body.insert(m_tail.base(), m_body.back())) - 1; }

	[[nodiscard]] auto head_loc() const noexcept -> const auto & { return m_loc; }
	[[nodiscard]] auto body() const noexcept -> const auto & { return m_body; }

	friend auto operator<<(sdl::Renderer &r, const Snake &s) -> sdl::Renderer &
	{
		r.color(sdl::YELLOW);
		const std::span b(s.m_body);
		sdl::draw(&b, &r).filled_rects();

		r.color(sdl::GREEN);
		sdl::draw(&*(s.m_tail.base() == s.m_body.end() ? s.m_body.begin() : s.m_tail.base()), &r).filled_rect();

		return r;
	}

private:
	Direction		m_dir = Direction::RIGHT;
	mth::Point<int> m_loc;

	std::vector<mth::Rect<int, int>> m_body;
	decltype(m_body.rbegin())		 m_tail = m_body.rbegin();
};

class Apple
{
public:
	Apple(const Snake &s) { respawn(s); }

	auto respawn(const Snake &s) -> void
	{
		std::vector<bool> used_spaces_v((Field::GRID.w) * (Field::GRID.h), false);

		for (auto body_iter = s.body().begin(); body_iter != s.body().end(); ++body_iter)
		{
			const auto point								 = Field::coord_to_grid(body_iter->pos());
			used_spaces_v[point.x + Field::GRID.w * point.y] = true;
		}

		auto num = g_rand.rand_number<int>(0, used_spaces_v.size() - s.body().size());

		num = std::distance(used_spaces_v.begin(),
							std::find_if(used_spaces_v.begin(), used_spaces_v.end(),
										 [num](bool b) mutable { return !b && --num <= 0; }));

		m_loc	= { num % Field::GRID.w, num / Field::GRID.h };
		m_shape = { Field::grid_to_coord(m_loc), { Field::DIV, Field::DIV } };
	}

	[[nodiscard]] auto loc() const noexcept -> const auto & { return m_loc; }

	friend auto operator<<(sdl::Renderer &r, const Apple &a)
	{
		r.color(sdl::RED);
		sdl::draw(&a.m_shape, &r).filled_rect();
	}

private:
	mth::Point<int>		m_loc;
	mth::Rect<int, int> m_shape;
};

class App
{
public:
	static constexpr std::chrono::milliseconds TICK_DUR = std::chrono::milliseconds(100);

	App()
		: m_win("Snake", START_DIM, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
		, m_r(&m_win)
		, m_s({ 0, 0 })
		, m_a(m_s)
	{
		m_r.blend_mode(SDL_BLENDMODE_BLEND);
		m_r.logical_size(START_DIM);

		m_tick.start();
	}

	void pre_pass() {}

	void event(const SDL_Event &e)
	{
		switch (e.type)
		{
		case SDL_WINDOWEVENT: m_r.do_render(true); break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: m_next_dir = Snake::Direction::UP; break;
			case SDLK_DOWN: m_next_dir = Snake::Direction::DOWN; break;
			case SDLK_LEFT: m_next_dir = Snake::Direction::LEFT; break;
			case SDLK_RIGHT: m_next_dir = Snake::Direction::RIGHT; break;
			case SDLK_ESCAPE: m_pause = !m_pause; break;
			}
			break;
		}
	};

	void update()
	{
		if (m_tick.ticks<std::chrono::milliseconds>() >= TICK_DUR && !m_pause)
		{
			m_r.do_render(true);

			m_s.direction(m_next_dir);
			if (!m_s.mov())
			{
				auto e = sdl::create_exit_event(m_win.ID());
				SDL_PushEvent(&e);
				std::cerr << "Crash!\n"
						  << "Finished with score: " << m_score << std::endl;
			}

			m_tick.start();

			if (mth::collision(m_a.loc(), m_s.head_loc()))
			{
				std::cout << "New score: " << ++m_score << std::endl;

				m_s.increase_size();
				m_a.respawn(m_s);
			}
		}
	};

	void fixed_update(){};

	void render()
	{
		if (m_r.will_render())
		{
			auto r = sdl::render(&m_r);
			r.fill(Field::BG_COLOR);

			Field::draw_field(m_r);
			m_r << m_s << m_a;

			r.locking_render();
		}
	}

private:
	sdl::Window					m_win;
	sdl::Delayed<sdl::Renderer> m_r;

	ctl::Timer m_tick;
	Snake	   m_s;
	Snake::Direction m_next_dir = Snake::Direction::RIGHT;
	Apple	   m_a;

	uint32_t m_score = 0;
	bool	 m_pause = false;
};

auto main(int argc, char **argv) -> int
{
	try
	{
		sdl::SDL		  s;
		App				  a;
		sdl::RunLoop<App> r;

		r.add_window(&a);
		r.run(60);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	::getchar();
	return 0;
}