#pragma once

#include <vector>
#include <type_traits>

#include <SDL.h>

#include <CustomLibrary/utility.h>
#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/SDLTraits.h>

namespace ctl
{
	class ShapeInterface
	{
	public:
		virtual void draw() const = 0;
		virtual void drawFilled() const = 0;

		constexpr const auto& color() const { return m_col; }
		constexpr auto& color(const SDL_Color& col) { m_col = col; return *this; }
		
	protected:
		SDL_Color m_col = { 0, 0, 0, 0xFF };
	};

	//----------------------------------------------
	//Single
	//----------------------------------------------

	class Rectangle : public Object<SDLRect<int, int>>, public ShapeInterface
	{
	public:
		using Object::Object;

		void draw() const override
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

			if (SDL_RenderDrawRect(m_win->renderer(), m_shape.rectPtr()) != 0)
				throw Log(SDL_GetError());
		}

		void drawFilled() const override
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

			if (SDL_RenderFillRect(m_win->renderer(), m_shape.rectPtr()) != 0)
				throw Log(SDL_GetError());
		}
	};

	class Point : public Object<SDLPoint<int>>, public ShapeInterface
	{
	public:
		using Object::Object;

		void draw() const override
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);
			if (SDL_RenderDrawPoint(m_win->renderer(), m_shape.x, m_shape.y) != 0)
				throw Log(SDL_GetError());
		}

		void drawFilled() const override
		{
			draw();
		}
	};

	class Circle : public Object<SDLCircle<int, int>>, public ShapeInterface
	{
		void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int)) const
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

			const auto d = m_shape.r * 2;

			SDLPoint p(m_shape.r - 1, 0);
			SDLPoint tp(1, 1);

			auto err = tp.x - d;

			while (p.x >= p.y)
			{
				const std::array<SDLPoint<int>, 8> ps =
				{ SDLPoint<int>
					{ m_shape.x + p.x, m_shape.y + p.y },
					{ m_shape.x - p.x, m_shape.y + p.y },
					{ m_shape.x + p.x, m_shape.y - p.y },
					{ m_shape.x - p.x, m_shape.y - p.y },
					{ m_shape.x + p.y, m_shape.y + p.x },
					{ m_shape.x - p.y, m_shape.y + p.x },
					{ m_shape.x + p.y, m_shape.y - p.x },
					{ m_shape.x - p.y, m_shape.y - p.x },
				};

				if (func(m_win->renderer(), ps.front().pointPtr(), ps.size()) != 0)
					throw Log(SDL_GetError());

				if (err <= 0)
				{
					++p.y;
					err += tp.y;
					tp.y += 2;
				}
				if (err > 0)
				{
					--p.x;
					tp.x += 2;
					err += tp.x - d;
				}
			}
		}

	public:
		using Object::Object;

		void draw() const override
		{
			_draw_(SDL_RenderDrawPoints);
		}

		void drawFilled() const override
		{
			_draw_(SDL_RenderDrawLines);
		}

		template<size_t pres>
		void drawP() const
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

			std::array<SDLPoint<int>, pres + 1> ps;

			for (size_t i = 0; i < pres; ++i)
			{
				const auto x = to_radians(360.f / pres * (i + 1.f));
				ps[i] = { static_cast<int>(m_shape.r * std::cos(x) + m_shape.x), static_cast<int>(m_shape.r * std::sin(x) + m_shape.y) };
			}
			ps.back() = ps.front();

			if (SDL_RenderDrawLines(m_win->renderer(), ps.data(), ps.size()) != 0)
				throw Log(SDL_GetError());
		}
	};

	class Line : public Object<SDLLine<int>>, public ShapeInterface
	{
	public:
		using Object::Object;

		void draw() const override
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);
			if (SDL_RenderDrawLine(m_win->renderer(), m_shape.x1, m_shape.x2, m_shape.y1, m_shape.y2) != 0)
				throw Log(SDL_GetError());
		}

		void drawFilled() const override
		{
			draw();
		}
	};

	//----------------------------------------------
	//Multi
	//----------------------------------------------

	template<typename Shape>
	class MultiShape : public ShapeInterface, public Renderable
	{
		static_assert(hasSDLTag_v<Shape>, "Should be a shape.");

		using ShapeTag = typename Shape::tag;
		using PrimShape = std::conditional_t<std::is_same_v<Tags::isPoint, ShapeTag>, SDLPoint<int>,
			std::conditional_t < std::is_same_v<Tags::isRect, ShapeTag>, SDLRect<int, int>,
			std::conditional_t<std::is_same_v<Tags::isLine, ShapeTag>, SDLPoint<int>, void>>>;

	public:
		using Renderable::Renderable;

		MultiShape() noexcept = default;
		MultiShape(const MultiShape&) noexcept = default;
		MultiShape(MultiShape&&) noexcept = default;

		MultiShape& operator=(const MultiShape&) noexcept = default;
		MultiShape& operator=(MultiShape&&) noexcept = default;

		template<typename ...Arg>
		MultiShape& emplace_back(Arg&&... argv)
		{ 
			m_pool.emplace_back(std::forward<Arg>(argv)...); 
			return *this; 
		}

		MultiShape& push_back(const Shape& val)
		{ 
			m_pool.push_back(val); 
			return *this; 
		}
		//MultiShape& push_back(Shape&& val)
		//{ 
		//	m_pool.push_back(std::move(val));
		//	return *this; 
		//}

		MultiShape& clear() noexcept
		{ 
			m_pool.clear(); 
			return *this; 
		}

		MultiShape& reserve(const size_t& newSize)
		{ 
			m_pool.reserve(newSize); 
			return *this; 
		}

		template<typename Iter, 
			typename = typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<Iter>::value_type, Shape>>>
		MultiShape& insert_back(const Iter& begin, const Iter& end)
		{ 
			m_pool.insert(m_pool.end(), begin, end);
			return *this; 
		}

		void draw() const override
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

			if constexpr (std::is_same_v<Tags::isRect, ShapeTag>)
				SDL_RenderDrawRects(m_win->renderer(), m_pool.front().rectPtr(), m_pool.size());

			else if constexpr (std::is_same_v<Tags::isPoint, ShapeTag>)
				SDL_RenderDrawPoints(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());

			else if constexpr (std::is_same_v<Tags::isLine, ShapeTag>)
				SDL_RenderDrawLines(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());
		}
		void drawFilled() const override
		{
			SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

			if constexpr (std::is_same_v<Tags::isRect, ShapeTag>)
				SDL_RenderDrawRects(m_win->renderer(), m_pool.front().rectPtr(), m_pool.size());

			else if constexpr (std::is_same_v<Tags::isPoint, ShapeTag>)
				SDL_RenderDrawPoints(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());

			else if constexpr (std::is_same_v<Tags::isLine, ShapeTag>)
				SDL_RenderDrawLines(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());
		}

	private:
		std::vector<PrimShape> m_pool;
	};

	//struct Line {};
	//struct Square {};
	//struct SquareFilled {};
	//struct Point {};

	//template<typename Type>
	//class FastGeo
	//{
	//	using Evaluated =
	//		std::conditional_t<std::is_same_v<Line, Type>, SDLPoint,
	//		std::conditional_t<std::is_same_v<Square, Type>, SDLRect,
	//		std::conditional_t<std::is_same_v<SquareFilled, Type>, SDLRect,
	//		std::conditional_t<std::is_same_v<Point, Type>, SDLPoint, void>>>>;

	//public:
	//	FastGeo(ctl::SDLWindow* win)
	//		: m_win(win)
	//	{
	//	}

	//	template<typename ...Arg>
	//	auto& emplace_back(Arg&& ... argv) 
	//	{ 
	//		m_pool.emplace_back(std::move(argv)...); 
	//		return *this; 
	//	}

	//	auto& push_back(const Evaluated& val) 
	//	{ 
	//		m_pool.push_back(val); 
	//		return *this; 
	//	}
	//	auto& push_back(Evaluated&& val) 
	//	{ 
	//		m_pool.push_back(std::move(val));
	//		return *this; 
	//	}

	//	auto& clear() noexcept 
	//	{ 
	//		m_pool.clear(); 
	//		return *this; 
	//	}

	//	auto& reserve(const size_t& newSize) 
	//	{ 
	//		m_pool.reserve(newSize); 
	//		return *this; 
	//	}

	//	template<typename Iter, typename = typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<Iter>::value_type, Evaluated>>>
	//	auto& insert_back(const Iter& begin, const Iter& end) 
	//	{ 
	//		m_pool.insert(m_pool.end(), begin, end);
	//		return *this; 
	//	}

	//	void draw(const SDL_Color& col)
	//	{
	//		std::vector<Evaluated> temp(m_pool.begin(), m_pool.end());
	//		for (auto& i : temp)
	//		{
	//			i.x -= m_win->cam.loc().x;
	//			i.y -= m_win->cam.loc().y;
	//		}

	//		SDL_SetRenderDrawColor(m_win->renderer(), col.r, col.g, col.b, col.a);

	//		if constexpr (std::is_same_v<Type, Line>)
	//			SDL_RenderDrawLines(m_win->renderer(), temp.data(), temp.size());

	//		else if constexpr (std::is_same_v<Type, Square>)
	//			SDL_RenderDrawRects(m_win->renderer(), temp.data(), temp.size());

	//		else if constexpr (std::is_same_v<Type, SquareFilled>)
	//			SDL_RenderFillRects(m_win->renderer(), temp.data(), temp.size());

	//		else if constexpr (std::is_same_v<Type, Point>)
	//			SDL_RenderDrawPoints(m_win->renderer(), temp.data(), temp.size());
	//	}

	//	constexpr const auto& pool() const noexcept { return m_pool; }

	//private:
	//	SDLWindow* m_win;
	//	std::vector<Evaluated> m_pool;
	//};
}