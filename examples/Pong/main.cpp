#include "CustomLibrary/SDL/Engine.h"
#include "CustomLibrary/SDL/Window.h"
#include "CustomLibrary/SDL/Renderer.h"
#include "CustomLibrary/SDL/Render.h"
#include "CustomLibrary/SDL/Drawable.h"
#include "CustomLibrary/SDL/Event.h"

#include "CustomLibrary/RandomGenerator.h"
#include "CustomLibrary/Collider.h"

using namespace ctl;

static rnd::Random<rnd::Linear> g_rand;

class Ball
{
public:
	static constexpr int RAD = 5;
	static constexpr int VEL = 3;

	Ball(const mth::Point<int> &start, sdl::Renderer *r)
		: m_shape(start, RAD)
		, m_cache(&m_shape, r)
	{
		m_cache.generate();
	}

	[[nodiscard]] constexpr auto shape() const noexcept -> const auto & { return m_shape; }
	constexpr void				 pos(const mth::Point<int> &p) noexcept
	{
		m_shape.pos(p);
		m_cache.mov(p.x - m_shape.x, p.y - m_shape.y);
	}
	constexpr void mov(int dx, int dy) noexcept
	{
		m_shape.x += dx, m_shape.y += dy;
		m_cache.mov(dx, dy);
	}

	void update() noexcept // WARNING: This style of updating physics make the velocity FPS dependant...
	{
		m_cache.mov(m_velx, m_vely);
		m_shape.x += m_velx;
		m_shape.y += m_vely;
	}

	void bounce_y() noexcept { m_vely = -m_vely; }
	void bounce_x() noexcept { m_velx = -m_velx; }

	friend auto operator<<(sdl::Renderer &r, const Ball &b) -> sdl::Renderer &
	{
		r.color(sdl::YELLOW);
		b.m_cache.filled_circle();
		return r;
	}

private:
	mth::Circle<int, int>							m_shape;
	sdl::Draw<mth::Circle<int, int>, sdl::Renderer> m_cache;

	int m_velx = VEL, m_vely = -VEL;
};

class Field
{
public:
	static constexpr int	  DIV		   = 10;
	static constexpr int	  BLOCK_WIDTH  = 5;
	static constexpr mth::Dim DIM		   = { 10, 50 };
	static constexpr mth::Dim START_DIM	   = { DIM.w * BLOCK_WIDTH * DIV, DIM.h *DIV };
	static constexpr int	  BLOCK_BORDER = 25;
	static constexpr int	  BLOCK_N	   = DIM.w * BLOCK_BORDER / 4;

	static constexpr std::array<mth::Point<int>, 4> BORDER_LINES = {
		mth::Point<int>{ 0, START_DIM.h - 1 }, { 0, 0 }, { START_DIM.w - 1, 0 }, { START_DIM.w - 1, START_DIM.h - 1 }
	};

	static auto draw_field(sdl::Renderer &r)
	{
		r.color(sdl::LIGHT_GRAY);
		std::span t(BORDER_LINES);
		sdl::draw(&t, &r).lines();
	}

	Field() { regenerate(); }

	void regenerate()
	{
		m_blocks.clear();
		std::vector<bool> spaces(DIM.w * BLOCK_BORDER, false);

		for (auto i = 0; i < g_rand.rand_number(10, BLOCK_N); ++i)
		{
			const int loc = g_rand.rand_number(0, DIM.w * BLOCK_BORDER - i - 1);

			auto n = 0;
			for (; n < loc; ++n)
				if (spaces[n])
					++n;

			spaces[n] = true;
			m_blocks.emplace_back((n % DIM.w) * DIV * BLOCK_WIDTH, n / DIM.w * DIV, DIV * BLOCK_WIDTH, DIV);
		}
	}

	[[nodiscard]] auto inside_field(const Ball &b) const noexcept -> bool
	{
		return b.shape().y - b.shape().r <= DIV * BLOCK_BORDER - 1;
	}

	auto collide(const Ball &b) -> std::optional<mth::Rect<int, int>>
	{
		const auto block = std::find_if(m_blocks.begin(), m_blocks.end(),
										[c = b.shape()](const mth::Rect<int, int> &p) { return mth::collision(c, p); });

		if (block == m_blocks.end())
			return std::nullopt;

		const auto shape = *block;
		m_blocks.erase(block);

		return shape;
	}

	friend auto operator<<(sdl::Renderer &r, const Field &f) -> sdl::Renderer &
	{
		r.color(sdl::BLUE);
		std::span<const mth::Rect<int, int>> t(f.m_blocks.begin(), f.m_blocks.end());
		sdl::draw(&t, &r).filled_rects();

		return r;
	}

private:
	std::vector<mth::Rect<int, int>> m_blocks;
};

class Pong
{
public:
	static constexpr int SPEED = 3;

	Pong(const mth::Point<int> &pos)
		: m_body(pos, { Field::DIV * Field::BLOCK_WIDTH, Field::DIV })
	{
	}

	[[nodiscard]] constexpr auto shape() const noexcept -> const auto & { return m_body; }
	constexpr void				 pos(mth::Point<int> p) { return m_body.pos(p); }

	void update() noexcept // WARNING: This style of updating physics make the velocity FPS dependant...
	{
		m_body.x += m_velx;
		m_body.y += m_vely;
	}

	void go_down(int speed) noexcept { m_vely += speed; }
	void go_right(int speed) noexcept { m_velx += speed; }

	friend auto operator<<(sdl::Renderer &r, const Pong &p) -> sdl::Renderer &
	{
		r.color(sdl::WHITE);
		sdl::draw(&p.shape(), &r).filled_rect();
		return r;
	}

private:
	mth::Rect<int, int> m_body;
	int					m_velx = 0, m_vely = 0;
};

class App
{
public:
	App()
		: m_w("Pong", Field::START_DIM, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)
		, m_r(&m_w)
		, m_pong({ 0, Field::START_DIM.h - Field::DIV })
		, m_ball({ Field::START_DIM.w / 2, Field::START_DIM.h - 1 }, &m_r)
	{
		m_r.logical_size(Field::START_DIM);
	}

	void pre_pass() {}
	void event(const SDL_Event &e)
	{
		switch (e.type)
		{
		case SDL_KEYDOWN: _handle_pong_control(e, Pong::SPEED); break;
		case SDL_KEYUP: _handle_pong_control(e, -Pong::SPEED); break;
		case SDL_QUIT: std::cout << "Final score: " << m_score << std::endl; break;
		}
	}
	void update()
	{
		const auto ball_before = m_ball.shape();
		const auto pong_before = m_pong.shape();

		m_pong.update();
		m_ball.update();

		_handle_pong_border_();
		_handle_ball_border_();

		_handle_ball_fall_();

		if (m_field.inside_field(m_ball))
		{
			const auto b = m_field.collide(m_ball);

			if (!b.has_value())
				return;

			std::cout << "Score increased to " << ++m_score << std::endl;

			_handle_block_bounce_(*b, ball_before);
		}
		else if (mth::collision(m_pong.shape(), m_ball.shape()))
			_handle_block_bounce_(pong_before, ball_before);
	}
	void render()
	{
		sdl::render(&m_r).fill(sdl::DARK_GRAY);

		m_r << m_field << m_ball << m_pong;
		Field::draw_field(m_r);

		sdl::render(&m_r).render();
	}

private:
	sdl::Window	  m_w;
	sdl::Renderer m_r;

	Field m_field;
	Pong  m_pong;
	Ball  m_ball;

	size_t m_score = 0;

	void _handle_block_bounce_(mth::Rect<int, int> r, mth::Circle<int, int> c)
	{
		if (c.x < r.x)
			m_ball.bounce_x(), m_ball.mov(-Ball::VEL - 1, 0);
		else if (c.x > r.x + r.w)
			m_ball.bounce_x(), m_ball.mov(Ball::VEL + 1, 0);

		if (c.y < r.y)
			m_ball.bounce_y(), m_ball.mov(0, -Ball::VEL - 1);
		else if (c.y > r.y + r.h)
			m_ball.bounce_y(), m_ball.mov(0, Ball::VEL + 1);
	}

	void _handle_exit_()
	{
		auto e = sdl::create_exit_event(m_w.ID());
		SDL_PushEvent(&e);
	}

	void _handle_pong_border_()
	{
		if (m_pong.shape().x < 0)
			m_pong.pos({ 0, m_pong.shape().y });
		else if (m_pong.shape().x + m_pong.shape().w > Field::START_DIM.w)
			m_pong.pos({ Field::START_DIM.w - m_pong.shape().w, m_pong.shape().y });
		if (m_pong.shape().y < Field::BLOCK_BORDER * Field::DIV + Ball::VEL + 2)
			m_pong.pos({ m_pong.shape().x, Field::BLOCK_BORDER * Field::DIV + Ball::VEL + 2 });
		else if (m_pong.shape().y + m_pong.shape().h > Field::START_DIM.h)
			m_pong.pos({ m_pong.shape().x, Field::START_DIM.h - m_pong.shape().h });
	}

	void _handle_ball_border_()
	{
		if (m_ball.shape().x - m_ball.shape().r <= 0 || m_ball.shape().x + m_ball.shape().r >= Field::START_DIM.w)
			m_ball.bounce_x();
		if (m_ball.shape().y - m_ball.shape().r <= 0)
			m_ball.bounce_y();
	}

	void _handle_ball_fall_()
	{
		if (m_ball.shape().y > Field::START_DIM.h)
			_handle_exit_();
	}

	void _handle_pong_control(const SDL_Event &e, int speed)
	{
		if (e.key.repeat == 0)
			switch (e.key.keysym.sym)
			{
			case SDLK_UP: m_pong.go_down(-speed); break;
			case SDLK_RIGHT: m_pong.go_right(speed); break;
			case SDLK_DOWN: m_pong.go_down(speed); break;
			case SDLK_LEFT: m_pong.go_right(-speed); break;
			}
	}
};

auto main() -> int
{
	try
	{
		sdl::SDL				s;
		App						a;
		sdl::SimpleRunLoop<App> r;

		r.window(&a);
		r.run(60);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	::getchar();
	return 0;
}