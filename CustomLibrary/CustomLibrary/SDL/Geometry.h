#pragma once

#include <SDL.h>

#include <CustomLibrary/utility.h>
#include "SDLTraits.h"

#include <vector>
#include <type_traits>
#include <variant>

namespace ctl::sdl
{
	//----------------------------------------------
	//Single
	//----------------------------------------------

	template<typename ImplRend = Renderer>
	class RectDraw : public Shapeable<Rect<int, int>, RectDraw<ImplRend>>, 
		Renderable<ImplRend, RectDraw<ImplRend>>
	{
		using base1 = Shapeable<Rect<int, int>, RectDraw<ImplRend>>;
		using base2 = Renderable<ImplRend, RectDraw<ImplRend>>;

	public:
		using base2::base2;

		void draw() const
		{
			if (SDL_RenderDrawRect(this->m_rend->get(), this->m_shape.rectPtr()) != 0)
				throw Log(SDL_GetError());
		}

		void drawFilled() const
		{
			if (SDL_RenderFillRect(this->m_rend->get(), this->m_shape.rectPtr()) != 0)
				throw Log(SDL_GetError());
		}

		using base2::renderer;
		using base1::shape;
	};


	template<typename ImplRend = Renderer>
	class CircleDraw : public Shapeable<Circle<int, unsigned int>, CircleDraw<ImplRend>>, 
		Renderable<ImplRend, CircleDraw<ImplRend>>
	{
		using base1 = Shapeable<Circle<int, unsigned int>, CircleDraw<ImplRend>>;
		using base2 = Renderable<ImplRend, CircleDraw<ImplRend>>;

	public:
		using base2::base2;

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

		using base2::renderer;
		using base1::shape;

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
	class LineDraw : public Shapeable<Line<int>, LineDraw<ImplRend>>, 
		Renderable<ImplRend, LineDraw<ImplRend>>
	{
		using base1 = Shapeable<Line<int>, LineDraw<ImplRend>>;
		using base2 = Renderable<ImplRend, LineDraw<ImplRend>>;

	public:
		using base2::base2;

		void draw() const
		{
			if (SDL_RenderDrawLine(this->m_rend->get(), this->m_shape.x1, this->m_shape.y1, this->m_shape.x2, this->m_shape.y2) != 0)
				throw Log(SDL_GetError());
		}

		using base2::renderer;
		using base1::shape;
	};


	template<typename ImplRend = Renderer>
	class PointDraw : public Shapeable<Point<int>, PointDraw<ImplRend>>, 
		Renderable<ImplRend, PointDraw<ImplRend>>
	{
		using base1 = Shapeable<Point<int>, PointDraw<ImplRend>>;
		using base2 = Renderable<ImplRend, PointDraw<ImplRend>>;

	public:
		using base2::base2;

		void draw() const
		{
			if (SDL_RenderDrawPoint(this->m_rend->get(), this->m_shape.x, this->m_shape.y) != 0)
				throw Log(SDL_GetError());
		}
		
		using base2::renderer;
		using base1::shape;
	};


	//----------------------------------------------
	//Multi
	//----------------------------------------------

	template<typename ImplRend, typename... Shapes>
	class MultiShape : Renderable<ImplRend, MultiShape<ImplRend, Shapes...>>
	{
		static_assert(std::conjunction_v<hasSDLTag<Shapes>...>, "Shapes must have the tag.");
		using base = Renderable<ImplRend, MultiShape<ImplRend, Shapes...>>;

		template<typename T>
		void drawHandler(T& arg) const
		{
			using tag_t = typename T::value_type::tag;

			if constexpr (std::is_same_v<Tags::isRect, tag_t>)
				SDL_RenderDrawRects(this->m_rend->get(), arg.front().rectPtr(), arg.size());

			else if constexpr (std::is_same_v<Tags::isLine, tag_t>)
				SDL_RenderDrawLines(this->m_rend->get(), arg.front().pointPtr(), arg.size() << 1);

			else if constexpr (std::is_same_v<Tags::isPoint, tag_t>)
				SDL_RenderDrawLines(this->m_rend->get(), arg.front().pointPtr(), arg.size());

			else
				static_assert(false, "Type is not supported for mass drawing.");
		}

	public:
		using base::Renderable;

		template<typename T>
		auto& push(const T& arg)
		{
			static_assert(std::disjunction_v<std::is_same<Shapes, T>...>, "Type must be of pack.");
			std::get<std::vector<T>>(m_packs).emplace_back(arg);

			return *this;
		}

		void draw() const
		{
			std::apply([this](auto&&... arg)
				{
					(this->drawHandler(arg), ...);
				}, m_packs);
		}

	private:
		std::tuple<std::vector<Shapes>...> m_packs;
	};

}