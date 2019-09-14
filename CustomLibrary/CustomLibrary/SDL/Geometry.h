#pragma once

#include <SDL.h>

#include <CustomLibrary/utility.h>
#include <CustomLibrary/SDL/Engine.h>
#include <CustomLibrary/SDL/SDLWindow.h>
#include <CustomLibrary/SDL/SDLTraits.h>

#include <vector>
#include <type_traits>
#include <variant>

namespace ctl
{
	namespace sdl
	{
		//----------------------------------------------
		//Single
		//----------------------------------------------

		template<typename ImplRend = Renderer>
		class DrawableRect : Object<ImplRend, Rect<int, int>>
		{
			using base = Object<ImplRend, Rect<int, int>>;

		public:
			using base::Object;

			void draw() const
			{
				const SDL_Rect r = this->m_shape;
				if (SDL_RenderDrawRect(this->m_rend->get(), &r) != 0)
					throw Log(SDL_GetError());
			}

			void drawFilled() const
			{
				const SDL_Rect r = this->m_shape;
				if (SDL_RenderFillRect(this->m_rend->get(), &r) != 0)
					throw Log(SDL_GetError());
			}

			using base::renderer;
			using base::shape;
		};


		template<typename ImplRend = Renderer>
		class DrawableCircle : Object<ImplRend, Circle<int, unsigned int>>
		{
			using base = Object<ImplRend, Circle<int, unsigned int>>;

		public:
			using base::Object;

			void draw() const
			{
				_draw_(SDL_RenderDrawPoints);
			}

			void drawFilled() const
			{
				_draw_(SDL_RenderDrawLines);
			}

			void drawP(int pres) const
			{
				std::vector<SDL_Point> ps;
				ps.reserve(pres + 1);

				for (size_t i = 0; i < pres; ++i)
				{
					const auto x = to_radians(360.f / pres * (i + 1.f));
					ps[i] = { static_cast<int>(this->m_shape.r * std::cos(x) + this->m_shape.x), static_cast<int>(this->m_shape.r * std::sin(x) + this->m_shape.y) };
				}
				ps.back() = ps.front();

				if (SDL_RenderDrawLines(this->m_rend->get(), ps.data(), ps.size()) != 0)
					throw Log(SDL_GetError());
			}

			using base::renderer;
			using base::shape;

		private:
			void _draw_(int (*func)(SDL_Renderer*, const SDL_Point*, int)) const
			{
				const auto d = this->m_shape.r * 2;

				Point<int> p(this->m_shape.r - 1, 0);
				Point<int> tp(1, 1);

				int err = tp.x - d;

				while (p.x >= p.y)
				{
					const std::array<SDL_Point, 8> ps =
					{ SDL_Point
						{ this->m_shape.x + p.x, this->m_shape.y + p.y },
						{ this->m_shape.x - p.x, this->m_shape.y + p.y },
						{ this->m_shape.x + p.x, this->m_shape.y - p.y },
						{ this->m_shape.x - p.x, this->m_shape.y - p.y },
						{ this->m_shape.x + p.y, this->m_shape.y + p.x },
						{ this->m_shape.x - p.y, this->m_shape.y + p.x },
						{ this->m_shape.x + p.y, this->m_shape.y - p.x },
						{ this->m_shape.x - p.y, this->m_shape.y - p.x }
					};

					if (func(this->m_rend->get(), ps.data(), ps.size()) != 0)
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

		};


		template<typename ImplRend = Renderer>
		class DrawableLine : Object<ImplRend, Line<int>>
		{
			using base = Object<ImplRend, Line<int>>;

		public:
			using base::Object;

			void draw() const
			{
				if (SDL_RenderDrawLine(this->m_rend->get(), this->m_shape.x1, this->m_shape.y1, this->m_shape.x2, this->m_shape.y2) != 0)
					throw Log(SDL_GetError());
			}

			using base::renderer;
			using base::shape;
		};


		template<typename ImplRend = Renderer>
		class DrawablePoint : Object<ImplRend, Point<int>>
		{
			using base = Object<ImplRend, Point<int>>;

		public:
			using base::Object;

			void draw() const
			{
				if (SDL_RenderDrawPoint(this->m_rend->get(), this->m_shape.x, this->m_shape.y) != 0)
					throw Log(SDL_GetError());
			}
		
			using base::renderer;
			using base::shape;
		};


		//----------------------------------------------
		//Multi
		//----------------------------------------------

		template<typename ImplRend, typename... Shapes>
		class MultiShape : Renderable<ImplRend>
		{
			static_assert(std::conjunction_v<hasSDLTag_v<Shapes>...>, "Shapes must have the tag.");
			using base = Renderable<ImplRend>;

		public:
			using base::Renderable;

			template<typename T, typename... Para>
			auto& push(Para&&... arg)
			{
				static_assert(std::disjunction_v<std::is_same_v<Shapes, T>...>, "Type must be of pack.");
				std::get<std::vector<T>>(m_packs).emplace_back(std::forward<Para>(arg)...);

				return *this;
			}

			void draw()
			{
				std::visit([](auto&& arg)
					{
						using T = decltype(arg);

						if constexpr (std::is_same_v<)
							
					})
			}

		private:
			std::tuple<std::vector<Shapes>...> m_packs;
		};


		//template<typename Shape>
		//class MultiShape : public ShapeInterface, public Renderable
		//{
		//	static_assert(hasSDLTag_v<Shape>, "Should be a shape.");

		//	using ShapeTag = typename Shape::tag;
		//	using PrimShape = std::conditional_t<std::is_same_v<Tags::isPoint, ShapeTag>, Point<int>,
		//		std::conditional_t < std::is_same_v<Tags::isRect, ShapeTag>, Rect<int, int>,
		//		std::conditional_t<std::is_same_v<Tags::isLine, ShapeTag>, Point<int>, void>>>;

		//public:
		//	using Renderable::Renderable;

		//	MultiShape() noexcept = default;
		//	MultiShape(const MultiShape&) noexcept = default;
		//	MultiShape(MultiShape&&) noexcept = default;

		//	MultiShape& operator=(const MultiShape&) noexcept = default;
		//	MultiShape& operator=(MultiShape&&) noexcept = default;

		//	template<typename ...Arg>
		//	MultiShape& emplace_back(Arg&& ... argv)
		//	{
		//		m_pool.emplace_back(std::forward<Arg>(argv)...);
		//		return *this;
		//	}

		//	MultiShape& push_back(const Shape& val)
		//	{
		//		m_pool.push_back(val);
		//		return *this;
		//	}
		//	//MultiShape& push_back(Shape&& val)
		//	//{ 
		//	//	m_pool.push_back(std::move(val));
		//	//	return *this; 
		//	//}

		//	MultiShape& clear() noexcept
		//	{
		//		m_pool.clear();
		//		return *this;
		//	}

		//	MultiShape& reserve(const size_t& newSize)
		//	{
		//		m_pool.reserve(newSize);
		//		return *this;
		//	}

		//	template<typename Iter,
		//		typename = typename std::enable_if_t<std::is_same_v<typename std::iterator_traits<Iter>::value_type, Shape>>>
		//		MultiShape& insert_back(const Iter& begin, const Iter& end)
		//	{
		//		m_pool.insert(m_pool.end(), begin, end);
		//		return *this;
		//	}

		//	void draw() const override
		//	{
		//		SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

		//		if constexpr (std::is_same_v<Tags::isRect, ShapeTag>)
		//			SDL_RenderDrawRects(m_win->renderer(), m_pool.front().rectPtr(), m_pool.size());

		//		else if constexpr (std::is_same_v<Tags::isPoint, ShapeTag>)
		//			SDL_RenderDrawPoints(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());

		//		else if constexpr (std::is_same_v<Tags::isLine, ShapeTag>)
		//			SDL_RenderDrawLines(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());
		//	}
		//	void drawFilled() const override
		//	{
		//		SDL_SetRenderDrawColor(m_win->renderer(), m_col.r, m_col.g, m_col.b, m_col.a);

		//		if constexpr (std::is_same_v<Tags::isRect, ShapeTag>)
		//			SDL_RenderDrawRects(m_win->renderer(), m_pool.front().rectPtr(), m_pool.size());

		//		else if constexpr (std::is_same_v<Tags::isPoint, ShapeTag>)
		//			SDL_RenderDrawPoints(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());

		//		else if constexpr (std::is_same_v<Tags::isLine, ShapeTag>)
		//			SDL_RenderDrawLines(m_win->renderer(), m_pool.front().pointPtr(), m_pool.size());
		//	}

		//private:
		//	std::vector<PrimShape> m_pool;
		//};

		//struct Line {};
		//struct Square {};
		//struct SquareFilled {};
		//struct Point {};

		//template<typename Type>
		//class FastGeo
		//{
		//	using Evaluated =
		//		std::conditional_t<std::is_same_v<Line, Type>, Point,
		//		std::conditional_t<std::is_same_v<Square, Type>, Rect,
		//		std::conditional_t<std::is_same_v<SquareFilled, Type>, Rect,
		//		std::conditional_t<std::is_same_v<Point, Type>, Point, void>>>>;

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
}